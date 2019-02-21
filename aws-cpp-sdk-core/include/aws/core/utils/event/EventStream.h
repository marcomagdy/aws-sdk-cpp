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
                /**
                 * Used for creating event-stream messages and serializing them as bit stream
                 */
                explicit EventEncoderStream(const Aws::Client::AWSAuthSigner& signer);

                void set_signature_seed(const Aws::String& seed) { m_eventEncoderStreamBuf.SetSignatureSeed(seed); }

                void set_event_headers(aws_array_list headers) { m_eventEncoderStreamBuf.SetEventHeaders(headers); }

                /**
                 * Marks the end of the output stream. Effectively, on the reading side (input) this translates to EOF.
                 * Any writes to the stream after this call are not guaranteed to be read by another concurrent thread
                 * consuming this stream as input stream.
                 */
                void close() { m_eventEncoderStreamBuf.SetEof(); }

                /*
                 * Transforms the bytes written to the stream to an event-stream envelope and signs it.
                 * The resulting data from that operation are made available for reading.
                 * This method can block the current thread if there is not enough capacity left in the underlying
                 * buffer. The underlying buffer is cleared when the data in the stream is read.
                 *
                 * @param headers The headers to use when constructing the event-stream envelope.
                 */
                void finalize_event(aws_array_list* headers); // TODO: do not expose the event-stream library internal data structures.
                                                             // instead pass a list of key/value pairs and construct that library structure
                                                             // in the implementation

            private:
                EventEncoderStreamBuf m_eventEncoderStreamBuf;
            };
        }
    }
}
