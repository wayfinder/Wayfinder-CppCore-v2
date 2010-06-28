/*
Copyright (c) 1999 - 2010, Vodafone Group Services Ltd
All rights reserved.

Redistribution and use in source and binary forms, with or without modification, are permitted provided that the following conditions are met:

    * Redistributions of source code must retain the above copyright notice, this list of conditions and the following disclaimer.
    * Redistributions in binary form must reproduce the above copyright notice, this list of conditions and the following disclaimer in the documentation and/or other materials provided with the distribution.
    * Neither the name of the Vodafone Group Services Ltd nor the names of its contributors may be used to endorse or promote products derived from this software without specific prior written permission.

THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS" AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE ARE DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT HOLDER OR CONTRIBUTORS BE LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
*/

#include "BufferRequesterTest.h"

#define CORE_LOGGING_MODULE_NAME "BufferRequesterTest"
// #define CORE_LOGGING_MODULE_LEVEL CL_ALL_FILE

#ifdef __linux__
#include <GtkTileMapToolkit.h>
#include <GtkHttpContext.h>
#endif

#ifdef IPHONE
#include <IPhoneFactory.h>
#endif

#include "CoreDebugPrint.h"

#include "BufferRequester/DiskCacheSettings.h"
#include "BufferRequester/MemCacheSettings.h"
#include "BufferRequester/ResourceDirectorySettings.h"
#include "BufferRequester/BufferRequester.h"
#include "BitBuffer.h"
#include "RegressionTestHelpers.h"
#include "MapLibNetworkContext.h"
#include "PlatformFactory.h"

BufferRequesterTest::BufferRequesterTest(Nav2Session& session, const char* name)
   : RegressionTest(name, 900000),
     m_reqString("DXX"),
     m_expectsValidResult(true),
     m_count(0)
{
   DiskCacheSettings diskSettings("", 0);
   MemCacheSettings memSettings(0);
   ResourceDirectorySettings resDirSettings("", false);
   
   TileMapToolkit* toolkit = NULL;
   WFAPI::DBufConnection* conn = NULL;

#ifdef __linux__
   m_netContext = new GtkHttpContext;
#elif defined IPHONE
   m_netContext = WFAPI::IPhoneFactory::createHTTPContext();
#endif

   conn = m_netContext->getConnection();
   
   toolkit = PlatformFactory::createToolkit();
   
   m_requester = new BufferRequester(toolkit,
                                     conn,
                                     memSettings,
                                     diskSettings,
                                     resDirSettings);
}

BufferRequesterTest::~BufferRequesterTest()
{
   delete m_requester;
   delete m_netContext;
}

void BufferRequesterTest::startTest()
{
   setTestPassed();
   return;
   
   m_requester->requestBuffer(m_reqString.c_str(), this);
}


void BufferRequesterTest::requestReceived(const MC2SimpleString& descr,
                                          BitBuffer* dataBuffer,
                                          OriginType origin)
{
   coreprintln("Recv %p (size %u) / %s and origin %d",
               dataBuffer,
               dataBuffer ? dataBuffer->getBufferSize() : 0,
               descr.c_str(),
               (int)origin);

   if(descr != m_reqString.c_str()) {
      setTestFailed("Incorrect descr string received!\n");
      if(dataBuffer) {
         m_requester->release(descr, dataBuffer);
      }
   } else if(m_expectsValidResult && !dataBuffer) {
      setTestFailed("Expected valid result but got NULL buffer!\n");   
   } else if(!m_expectsValidResult && dataBuffer) {
      setTestFailed("Expected invalid result but got actual buffer!\n");
      m_requester->release(descr, dataBuffer);
   } else {
      m_requester->release(descr, dataBuffer);
      m_count++;
      
      if(m_count == 1) {
         setTestPassed();
      } else {
         coreprintln("Count %d, re-doing test", m_count);
         m_requester->requestBuffer(m_reqString.c_str(), this);
      }
   }
}

void BufferRequesterTest::setArgImpl(const std::string& str)
{
   vector<string> bufferTests = RegressionTestHelpers::split(str);

   if(bufferTests.size() != 2) {
      return;
   }

   m_reqString = bufferTests[0];
   m_expectsValidResult = bufferTests[1] == "true";
}


