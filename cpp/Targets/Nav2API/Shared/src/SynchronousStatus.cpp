/*
  Copyright (c) 1999 - 2010, Vodafone Group Services Ltd
  All rights reserved.

  Redistribution and use in source and binary forms, with or without modification, are permitted provided that the following conditions are met:

  * Redistributions of source code must retain the above copyright notice, this list of conditions and the following disclaimer.
  * Redistributions in binary form must reproduce the above copyright notice, this list of conditions and the following disclaimer in the documentation and/or other materials provided with the distribution.
  * Neither the name of the Vodafone Group Services Ltd nor the names of its contributors may be used to endorse or promote products derived from this software without specific prior written permission.

  THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS" AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE ARE DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT HOLDER OR CONTRIBUTORS BE LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
*/

#include "SynchronousStatus.h"
#include "StatusCode.h"

namespace WFAPI {

SynchronousStatus::SynchronousStatus(wf_uint32 statusCode,
                                     const WFString& statusMessage,
                                     const WFString& statusURL,
                                     const WFString& statusData)
      : m_statusCode(statusCode), m_statusMessage(statusMessage), 
        m_statusURL(statusURL), m_statusData(statusData)
{
}

SynchronousStatus::~SynchronousStatus()
{
}

wf_uint32
SynchronousStatus::getStatusCode() const
{
   return m_statusCode;
}

const WFString&
SynchronousStatus::getStatusMessage() const
{
   return m_statusMessage;
}

const WFString&
SynchronousStatus::getStatusURL() const
{
   return m_statusURL;
}

const WFString&
SynchronousStatus::getStatusData() const
{
   return m_statusData;
}

bool
SynchronousStatus::operator()() const
{
   return getStatusCode() == OK;
}

} // End namespace WFAPI
