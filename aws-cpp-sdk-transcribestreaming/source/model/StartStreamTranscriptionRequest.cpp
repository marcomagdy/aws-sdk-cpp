/*
* Copyright 2010-2017 Amazon.com, Inc. or its affiliates. All Rights Reserved.
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

#include <aws/transcribestreaming/model/StartStreamTranscriptionRequest.h>
#include <aws/core/AmazonWebServiceResult.h>
#include <aws/core/utils/memory/stl/AWSStringStream.h>
#include <cstring>

#include <utility>

using namespace Aws::TranscribeStreamingService::Model;
using namespace Aws::Utils::Stream;
using namespace Aws::Utils;
using namespace Aws;

StartStreamTranscriptionRequest::StartStreamTranscriptionRequest() : 
    m_languageCode(LanguageCode::NOT_SET),
    m_languageCodeHasBeenSet(false),
    m_mediaSampleRateHertz(0),
    m_mediaSampleRateHertzHasBeenSet(false),
    m_mediaEncoding(MediaEncoding::NOT_SET),
    m_mediaEncodingHasBeenSet(false),
    m_vocabularyNameHasBeenSet(false),
    m_sessionIdHasBeenSet(false),
    m_decoder(Aws::Utils::Event::EventStreamDecoder(&m_handler))
{
}

aws_array_list StartStreamTranscriptionRequest::GetEventStreamHeaders() const
{
    aws_array_list headers;
    aws_array_list_init_dynamic(&headers, aws_default_allocator(), 3, sizeof(aws_event_stream_header_value_pair));
    aws_event_stream_add_string_header(&headers, ":content-type", strlen(":content-type"), "application/octet-stream", strlen("application/octet-stream"), 0 /*copy*/);
    aws_event_stream_add_string_header(&headers, ":event-type", strlen(":event-type"), "AudioEvent", strlen("AudioEvent"), 0 /*copy*/);
    aws_event_stream_add_string_header(&headers, ":message-type", strlen(":message-type"), "event", strlen("event"), 0 /*copy*/);
    return headers;
}

Aws::Http::HeaderValueCollection StartStreamTranscriptionRequest::GetRequestSpecificHeaders() const
{
  Aws::Http::HeaderValueCollection headers;
  Aws::StringStream ss;
  if(m_languageCodeHasBeenSet)
  {
    headers.emplace("x-amzn-transcribe-language-code", LanguageCodeMapper::GetNameForLanguageCode(m_languageCode));
  }

  if(m_mediaSampleRateHertzHasBeenSet)
  {
    ss << m_mediaSampleRateHertz;
    headers.emplace("x-amzn-transcribe-sample-rate",  ss.str());
    ss.str("");
  }

  if(m_mediaEncodingHasBeenSet)
  {
    headers.emplace("x-amzn-transcribe-media-encoding", MediaEncodingMapper::GetNameForMediaEncoding(m_mediaEncoding));
  }

  if(m_vocabularyNameHasBeenSet)
  {
    ss << m_vocabularyName;
    headers.emplace("x-amzn-transcribe-vocabulary-name",  ss.str());
    ss.str("");
  }

  if(m_sessionIdHasBeenSet)
  {
    ss << m_sessionId;
    headers.emplace("x-amzn-transcribe-session-id",  ss.str());
    ss.str("");
  }

  return headers;

}
