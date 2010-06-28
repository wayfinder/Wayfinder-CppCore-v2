/*
  Copyright (c) 1999 - 2010, Vodafone Group Services Ltd
  All rights reserved.

  Redistribution and use in source and binary forms, with or without modification, are permitted provided that the following conditions are met:

  * Redistributions of source code must retain the above copyright notice, this list of conditions and the following disclaimer.
  * Redistributions in binary form must reproduce the above copyright notice, this list of conditions and the following disclaimer in the documentation and/or other materials provided with the distribution.
  * Neither the name of the Vodafone Group Services Ltd nor the names of its contributors may be used to endorse or promote products derived from this software without specific prior written permission.

  THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS" AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE ARE DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT HOLDER OR CONTRIBUTORS BE LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
*/

#include "S60Nav2API.h"
#include "HardwareIDArray.h"
#include "ProgramVersion.h"
#include "WFString.h"
#include "Nav2APIImpl.h"
#include "S60Nav2APIImpl.h"
#include "Nav2APISharedImpl.h"

namespace WFAPI {

/**
 * Struct used for hiding the S60Nav2APIImpl
 */
struct S60Nav2API::S60Nav2APIImplStruct
{
   S60Nav2APIImplStruct(Nav2APIImpl* nav2APIImpl)
      : m_s60Nav2APIImpl(nav2APIImpl)
      {}

   S60Nav2APIImpl m_s60Nav2APIImpl;
};

S60Nav2API::S60Nav2API()
   : m_s60Nav2APIImplStruct(NULL)
{
   m_sharedImpl->m_httpProto = false;
   m_sharedImpl->m_programVersion = ProgramVersion(0, 9, 50);
}

S60Nav2API::~S60Nav2API()
{
   delete m_s60Nav2APIImplStruct;
}

AsynchronousStatus
S60Nav2API::start(Nav2StatusListener* statusListener, 
                  StartupData* startupData)
{
   m_s60Nav2APIImplStruct = new S60Nav2APIImplStruct(getImpl());
   return m_s60Nav2APIImplStruct->m_s60Nav2APIImpl.start(
      statusListener,
      startupData,
      m_sharedImpl->m_clientType,
      m_sharedImpl->m_clientTypeOptions,
      m_sharedImpl->m_httpProto,
      m_sharedImpl->m_httpRequestString,
      m_sharedImpl->m_programVersion);
}

} // End namespace WFAPI

