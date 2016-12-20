﻿/*
* Copyright 2010-2016 Amazon.com, Inc. or its affiliates. All Rights Reserved.
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
#include <aws/devicefarm/DeviceFarm_EXPORTS.h>
#include <aws/core/utils/memory/stl/AWSVector.h>
#include <aws/core/utils/memory/stl/AWSString.h>
#include <aws/devicefarm/model/Job.h>

namespace Aws
{
template<typename RESULT_TYPE>
class AmazonWebServiceResult;

namespace Utils
{
namespace Json
{
  class JsonValue;
} // namespace Json
} // namespace Utils
namespace DeviceFarm
{
namespace Model
{
  /**
   * <p>Represents the result of a list jobs request.</p><p><h3>See Also:</h3>   <a
   * href="http://docs.aws.amazon.com/goto/WebAPI/devicefarm-2015-06-23/ListJobsResult">AWS
   * API Reference</a></p>
   */
  class AWS_DEVICEFARM_API ListJobsResult
  {
  public:
    ListJobsResult();
    ListJobsResult(const AmazonWebServiceResult<Aws::Utils::Json::JsonValue>& result);
    ListJobsResult& operator=(const AmazonWebServiceResult<Aws::Utils::Json::JsonValue>& result);

    /**
     * <p>Information about the jobs.</p>
     */
    inline const Aws::Vector<Job>& GetJobs() const{ return m_jobs; }

    /**
     * <p>Information about the jobs.</p>
     */
    inline void SetJobs(const Aws::Vector<Job>& value) { m_jobs = value; }

    /**
     * <p>Information about the jobs.</p>
     */
    inline void SetJobs(Aws::Vector<Job>&& value) { m_jobs = value; }

    /**
     * <p>Information about the jobs.</p>
     */
    inline ListJobsResult& WithJobs(const Aws::Vector<Job>& value) { SetJobs(value); return *this;}

    /**
     * <p>Information about the jobs.</p>
     */
    inline ListJobsResult& WithJobs(Aws::Vector<Job>&& value) { SetJobs(value); return *this;}

    /**
     * <p>Information about the jobs.</p>
     */
    inline ListJobsResult& AddJobs(const Job& value) { m_jobs.push_back(value); return *this; }

    /**
     * <p>Information about the jobs.</p>
     */
    inline ListJobsResult& AddJobs(Job&& value) { m_jobs.push_back(value); return *this; }

    /**
     * <p>If the number of items that are returned is significantly large, this is an
     * identifier that is also returned, which can be used in a subsequent call to this
     * operation to return the next set of items in the list.</p>
     */
    inline const Aws::String& GetNextToken() const{ return m_nextToken; }

    /**
     * <p>If the number of items that are returned is significantly large, this is an
     * identifier that is also returned, which can be used in a subsequent call to this
     * operation to return the next set of items in the list.</p>
     */
    inline void SetNextToken(const Aws::String& value) { m_nextToken = value; }

    /**
     * <p>If the number of items that are returned is significantly large, this is an
     * identifier that is also returned, which can be used in a subsequent call to this
     * operation to return the next set of items in the list.</p>
     */
    inline void SetNextToken(Aws::String&& value) { m_nextToken = value; }

    /**
     * <p>If the number of items that are returned is significantly large, this is an
     * identifier that is also returned, which can be used in a subsequent call to this
     * operation to return the next set of items in the list.</p>
     */
    inline void SetNextToken(const char* value) { m_nextToken.assign(value); }

    /**
     * <p>If the number of items that are returned is significantly large, this is an
     * identifier that is also returned, which can be used in a subsequent call to this
     * operation to return the next set of items in the list.</p>
     */
    inline ListJobsResult& WithNextToken(const Aws::String& value) { SetNextToken(value); return *this;}

    /**
     * <p>If the number of items that are returned is significantly large, this is an
     * identifier that is also returned, which can be used in a subsequent call to this
     * operation to return the next set of items in the list.</p>
     */
    inline ListJobsResult& WithNextToken(Aws::String&& value) { SetNextToken(value); return *this;}

    /**
     * <p>If the number of items that are returned is significantly large, this is an
     * identifier that is also returned, which can be used in a subsequent call to this
     * operation to return the next set of items in the list.</p>
     */
    inline ListJobsResult& WithNextToken(const char* value) { SetNextToken(value); return *this;}

  private:
    Aws::Vector<Job> m_jobs;
    Aws::String m_nextToken;
  };

} // namespace Model
} // namespace DeviceFarm
} // namespace Aws
