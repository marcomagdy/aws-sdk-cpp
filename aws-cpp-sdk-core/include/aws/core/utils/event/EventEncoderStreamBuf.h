/*
 * Copyright 2010-2019 Amazon.com, Inc. or its affiliates. All Rights Reserved.
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
#pragma once

#include <aws/core/Core_EXPORTS.h>
#include <aws/core/auth/AWSAuthSigner.h>
#include <aws/common/array_list.h>

#include <mutex>
#include <condition_variable>
#include <streambuf>
#include <ios>
#include <cassert>

namespace Aws
{
    namespace Utils
    {
        namespace Event
        {
            /**
             * Derived from std::streambuf, used as the underlying buffer for EventEncoderStream.
             * Frames the input bytes in event-streams and signs them.
             */
            class AWS_CORE_API EventEncoderStreamBuf : public std::streambuf
            {
            public:

                /**
                 * @param signer AWS authentication signer. This signer is invoked when wrapping the bits in an event.
                 * @param bufferLength The length of buffer, wiil be 4KB by default.
                 */
                EventEncoderStreamBuf(const Aws::Client::AWSAuthSigner* signer, size_t bufferLength = 4 * 1024);

                ~EventEncoderStreamBuf();

                /**
                 * Sets the initial signature value in hexadecimal format.
                 * This value will be used to sign the first event only. From there-on the resulting signature will
                 * be used when signing the next event and so on.
                 */
                void SetSignatureSeed(const Aws::String& seed) { m_priorSignature = seed; }

                void SetEventHeaders(aws_array_list* headers)
                {
                    aws_array_list_clean_up(&m_headers);
                    m_headers.alloc = headers->alloc;
                    m_headers.item_size = headers->item_size;
                    aws_array_list_copy(headers, &m_headers);
                }

                void SetSigner(const Aws::Client::AWSAuthSigner* signer) { m_signer = signer; }

                void SetEof();
                void SendMessage(aws_event_stream_message& message);

                /*
                 * Transforms the bytes written to the stream to an event-stream envelope and signs it.
                 * The resulting data from that operation are made available for reading.
                 * This method can block the current thread if there is not enough capacity left in the underlying
                 * buffer. The underlying buffer is cleared when the data in the stream is read.
                 *
                 * @param headers The headers to use when constructing the event-stream envelope.
                 */
                void FinalizeEvent(aws_array_list* headers);

            protected:
                std::streampos seekoff(std::streamoff off, std::ios_base::seekdir dir, std::ios_base::openmode which = std::ios_base::in | std::ios_base::out) override;
                std::streampos seekpos(std::streampos pos, std::ios_base::openmode which = std::ios_base::in | std::ios_base::out) override;

                int underflow() override;
                int overflow(int ch) override;
                int sync() override;

            private:
                Aws::Vector<char> m_getArea;
                Aws::Vector<char> m_putArea;
                Aws::Vector<char> m_backbuf; // used to shuttle data from the put area to the get area
                Aws::String m_priorSignature;
                std::mutex m_lock; // synchronize access to the common backbuffer
                std::condition_variable m_signal;
                const Aws::Client::AWSAuthSigner* m_signer;
                aws_array_list m_headers;
                bool m_eof;
            };
        }
    }
}
