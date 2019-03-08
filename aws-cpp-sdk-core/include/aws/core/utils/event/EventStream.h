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
#pragma once

#include <aws/core/Core_EXPORTS.h>
#include <aws/core/utils/event/EventStreamBuf.h>
#include <aws/core/utils/event/EventEncoderStreamBuf.h>
#include <aws/core/utils/memory/stl/AWSStreamFwd.h>
#include <atomic>

namespace Aws
{
    namespace Client
    {
        class AWSAuthSigner;
    }
    namespace Utils
    {
        namespace Event
        {
            /**
             * It's only used for output stream right now.
             */
            class AWS_CORE_API EventStream : public Aws::IOStream
            {
            public:
                /**
                 * @param decoder decodes the stream from server side, so as to invoke related callback functions.
                 * @param eventStreamBufLength The length of the underlying buffer.
                 */
                EventStream(EventStreamDecoder& decoder, size_t eventStreamBufLength = Aws::Utils::Event::DEFAULT_BUF_SIZE);

            private:
                EventStream(const EventStream&) = delete;
                EventStream(EventStream&&) = delete;
                EventStream& operator=(const EventStream&) = delete;
                EventStream& operator=(EventStream&&) = delete;

                EventStreamBuf m_eventStreamBuf;
            };

            class AWS_CORE_API EventEncoderStream : public Aws::IOStream
            {
            public:

                EventEncoderStream();

                /**
                 * Sets the signature seed used by event-stream events.
                 * Every event uses its previous event's signature to calculate its own signature.
                 * Setting this value affects the signature calculation of the first event.
                 */
                void set_signature_seed(const Aws::String& seed) { m_eventEncoderStreamBuf.SetSignatureSeed(seed); }

                /**
                 * Sets the headers wrapped with each event sent on the stream.
                 */
                void set_event_headers(aws_array_list* headers) { m_eventEncoderStreamBuf.SetEventHeaders(headers); }

                /**
                 * Sets the signer implementation used for every event.
                 */
                void set_signer(Aws::Client::AWSAuthSigner* signer) { m_eventEncoderStreamBuf.SetSigner(signer); }

                /**
                 * Allows a stream writer to communicate the end of the stream to a stream reader.
                 *
                 * Any writes to the stream after this call are not guaranteed to be read by another concurrent
                 * read thread.
                 */
                void close() { m_eventEncoderStreamBuf.SetEof(); }

                /**
                 * Tests if the stream is ready to be written to.
                 * The stream is ready when all of the following are true:
                 * 1- It has been seeded with an initial signature v4 value
                 * 2- It has event-streams headers set.
                 * 3- It has a signer set.
                 *
                 * This method is thread-safe
                 */
                bool is_ready_for_streaming() const
                {
                    return m_readyForStreaming.load(std::memory_order_acquire);
                }

                /**
                 * Sets the stream write-readiness state.
                 *
                 * This method is thread-safe
                 */
                void set_ready_for_streaming(bool ready)
                {
                    m_readyForStreaming.store(ready, std::memory_order_release);
                }

            private:
                std::atomic<bool> m_readyForStreaming;
                EventEncoderStreamBuf m_eventEncoderStreamBuf;
            };
        }
    }
}
