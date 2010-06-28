/*
Copyright (c) 1999 - 2010, Vodafone Group Services Ltd
All rights reserved.

Redistribution and use in source and binary forms, with or without modification, are permitted provided that the following conditions are met:

    * Redistributions of source code must retain the above copyright notice, this list of conditions and the following disclaimer.
    * Redistributions in binary form must reproduce the above copyright notice, this list of conditions and the following disclaimer in the documentation and/or other materials provided with the distribution.
    * Neither the name of the Vodafone Group Services Ltd nor the names of its contributors may be used to endorse or promote products derived from this software without specific prior written permission.

THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS" AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE ARE DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT HOLDER OR CONTRIBUTORS BE LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
*/

#include "BufferRequestChainTest.h"

#define CORE_LOGGING_MODULE_NAME "BufferRequestChainTest"
// #define CORE_LOGGING_MODULE_LEVEL CL_ALL_FILE
#include "CoreDebugPrint.h"

#ifdef __linux__
#include <GtkTileMapToolkit.h>
#include <GtkHttpContext.h>
#endif

#ifdef IPHONE
#include <IPhonePathUtil.h>
#include <IPhoneFactory.h>
#endif

#include "MapLibNetworkContext.h"

#include "BufferRequester/ResourceDirectorySettings.h"
#include "BufferRequester/DiskCacheSettings.h"
#include "BufferRequester/MemCacheSettings.h"
#include "BufferRequester/BufferRequester.h"
#include "BitBuffer.h"
#include "RegressionTestHelpers.h"
#include "PlatformFactory.h"

#define REQ_STR "DXX"
#define REQ_STR2 "DXX-1"

BufferRequestChainTest::BufferRequestChainTest(Nav2Session& session, const
                                               char* name) :
   RegressionTest(name, 9000000)
{
   DiskCacheSettings diskSettings("", 0);
   MemCacheSettings memSettings(100*1024);
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
   m_requester->printChain();
   m_state = UNINITIALIZED;
}

BufferRequestChainTest::~BufferRequestChainTest()
{
   
}

void BufferRequestChainTest::startTest()
{
   m_state = MEM_CHECK_FROM_NET;
   m_requester->requestBuffer(REQ_STR, this);
}

std::string getDiskCachePath() {
#ifdef IPHONE
   // return "/var/root/Documents";
   return IPhonePathUtil::getDocumentsPath();
#else
   return "/tmp/tilemaps";
#endif
}

void BufferRequestChainTest::requestReceived(const MC2SimpleString& descr,
                                             BitBuffer* dataBuffer,
                                             OriginType origin)
{
   coreprintln("Recv %p (size %u) / %s and origin %s",
               dataBuffer,
               dataBuffer ? dataBuffer->getBufferSize() : 0,
               descr.c_str(),
               OriginType::toString(origin));

   if(!dataBuffer) {
      setTestFailed("Receieved NULL buffer");
      return;
   }

   switch(m_state) {
   case MEM_CHECK_FROM_NET:
      if(origin != OriginType::EXTERNAL) {
         setTestFailed("Expected the buffer to come from EXTERNAL, it did not.");
         return;
      } else {
         m_state = MEM_CHECK_FROM_MEM;
         m_requester->release(REQ_STR, dataBuffer);
         m_requester->requestBuffer(REQ_STR, this);
      }
      break;
   case MEM_CHECK_FROM_MEM:
      if(origin != OriginType::MEMORY) {
         setTestFailed("Expected the buffer to come from MEMORY, it did not.");
         return;
      } else {
         m_requester->release(REQ_STR, dataBuffer);
         m_state = DISK_CHECK_FROM_NET;
         
         // Remove mem cache.
         coreprintln("Done testing memory, removing mem cache and adding disk. Chain before:");
         MemCacheSettings emptyMemCacheSettings(0);         
         m_requester->setMemCacheSettings(emptyMemCacheSettings);
         
         DiskCacheSettings diskSettings(getDiskCachePath().c_str(),
                                        1024*1024*10);
         m_requester->setDiskCacheSettings(diskSettings);
         
         // Clear rw cache.
         m_requester->clearReadWriteCache();
         m_requester->requestBuffer(REQ_STR2, this);

         coreprintln("Changing state to DISK_CHECK_FROM_NET");
         m_requester->printChain();
      }
      break;
   case DISK_CHECK_FROM_NET:
      if(origin != OriginType::EXTERNAL) {
         setTestFailed("Expected the buffer to come from EXTERNAL, it did not.");
         return;
      } else {
         coreprintln("Changing state to DISK_CHECK_FROM_DISK");
         m_requester->printChain();
         m_state = DISK_CHECK_FROM_DISK;
         m_requester->release(descr, dataBuffer);
         m_requester->requestBuffer(REQ_STR2, this);
      }
      break;
   case DISK_CHECK_FROM_DISK:
      if(origin != OriginType::READ_WRITE_CACHE) {
         setTestFailed("Expected the buffer to come from READ_WRITE_CACHE, it did not.");
         return;
      } else {
         setTestPassed();
      }
      break;
      break;
   }
}

void BufferRequestChainTest::setArgImpl(const std::string& str)
{
   
}






