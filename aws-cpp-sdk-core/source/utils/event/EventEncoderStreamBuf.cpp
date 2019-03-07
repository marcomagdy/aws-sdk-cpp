/*
 * Copyright 2010-2018 Amazon.com, Inc. or its affiliates. All Rights Reserved.
 *
 * Licensed under the Apache License, Version 2.0 (the "License").
 * You may not use this file except in compliance with the License.
 * A copy of the License is located at
 *
 *  http://aws.amazon.com/apache2.0
 *
 * or in the "license" file accompanying this file. This file is distributed
 * on an "AS IS" BASIS, WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either
 * express or implied. See the License for the specific language governing
 * permissions and limitations under the License.
 */
#include <aws/core/utils/event/EventEncoderStreamBuf.h>
#include <aws/core/utils/logging/LogMacros.h>
#include <aws/event-stream/event_stream.h>
#include <cstdint>
#include <cassert>

namespace Aws
{
    namespace Utils
    {
        namespace Event
        {
            const char TAG[] = "EventEncoderStreamBuf";
            EventEncoderStreamBuf::EventEncoderStreamBuf(const Aws::Client::AWSAuthSigner* signer, size_t bufferLength)
                : m_putArea(bufferLength),
                m_signer(signer),
                m_eof(false)
            {
                m_headers.alloc = nullptr;
                m_headers.data = nullptr;
                m_headers.length = 0;
                m_headers.current_size = 0;
                m_headers.item_size = 0;

                // The get area must be larger than put area to make room for the wrapping bits.
                m_getArea.reserve(bufferLength * 1.5);
                m_backbuf.reserve(bufferLength * 1.5);

                const auto pbegin = &m_putArea[0];
                setp(pbegin, pbegin + bufferLength);
                setg(&m_getArea[0], &m_getArea[0], &m_getArea[0]);
            }

            EventEncoderStreamBuf::~EventEncoderStreamBuf()
            {
                aws_array_list_clean_up(&m_headers);
            }

            void EventEncoderStreamBuf::SetEof()
            {
                std::unique_lock<std::mutex> lock(m_lock);
                m_eof = true;
            }

            void EventEncoderStreamBuf::FinalizeEvent(aws_array_list* headers)
            {
                if (pptr() >= pbase())
                {
                    // create the message here and copy to the get area
                    aws_byte_buf payload;
                    payload.len = pptr() - pbase();
                    payload.buffer = reinterpret_cast<uint8_t*>(pbase());
                    payload.capacity = 0;
                    payload.allocator = nullptr;


                    aws_event_stream_message message;
                    if(auto err = aws_event_stream_message_init(&message, aws_default_allocator(), headers, &payload))
                    {
                        AWS_LOGSTREAM_ERROR(TAG, "Error creating event-stream message from paylaod.");
                        return;
                    }

                    assert(m_signer);
                    if (!m_signer->SignEventMessage(message, m_priorSignature))
                    {
                        AWS_LOGSTREAM_ERROR(TAG, "Failed to sign event message frame.");
                        aws_event_stream_message_clean_up(&message);
                        return;
                    }

                    auto messageLength = aws_event_stream_message_total_length(&message);

                    // scope the lock
                    {
                        std::unique_lock<std::mutex> lock(m_lock);
                        m_signal.wait(lock, [this, messageLength]{ return messageLength <= (m_backbuf.capacity() - m_backbuf.size()); });
                        std::copy(message.message_buffer, message.message_buffer + messageLength,
                                std::back_inserter(m_backbuf));

                        const auto pbegin = &m_putArea[0];
                        setp(pbegin, pbegin + m_putArea.size());
                    }
                    aws_event_stream_message_clean_up(&message);
                    m_signal.notify_one();
                }
            }

            std::streampos EventEncoderStreamBuf::seekoff(std::streamoff, std::ios_base::seekdir, std::ios_base::openmode)
            {
                return std::streamoff(-1); // Seeking is not supported.
            }

            std::streampos EventEncoderStreamBuf::seekpos(std::streampos, std::ios_base::openmode)
            {
                return std::streampos(std::streamoff(-1)); // Seeking is not supported.
            }

            int EventEncoderStreamBuf::underflow()
            {
                {
                    std::unique_lock<std::mutex> lock(m_lock);
                    if (m_eof && m_backbuf.empty())
                    {
                        return std::char_traits<char>::eof();
                    }

                    m_signal.wait(lock, [this]{ return m_backbuf.empty() == false; });
                    m_getArea.clear();
                    std::copy(m_backbuf.begin(), m_backbuf.end(), std::back_inserter(m_getArea));
                    m_backbuf.clear();
                }
                m_signal.notify_one();
                const auto gbegin = &m_getArea[0];
                setg(gbegin, gbegin, gbegin + m_getArea.size());
                return std::char_traits<char>::to_int_type(*gptr());
            }

            int EventEncoderStreamBuf::overflow(int ch)
            {
                const auto eof = std::char_traits<char>::eof();

                if (ch == eof)
                {
                    if (pptr() > pbase())
                    {
                        FinalizeEvent(&m_headers);// wrap whatever is in the put area in an event
                    }
                    return eof;
                }

                FinalizeEvent(&m_headers);
                *pptr() = static_cast<char>(ch);
                pbump(1);
                return ch;
            }

            int EventEncoderStreamBuf::sync()
            {
                FinalizeEvent(&m_headers);
                return 0;
            }
        }
    }
}
