/*
Copyright (c) 1999 - 2010, Vodafone Group Services Ltd
All rights reserved.

Redistribution and use in source and binary forms, with or without modification, are permitted provided that the following conditions are met:

    * Redistributions of source code must retain the above copyright notice, this list of conditions and the following disclaimer.
    * Redistributions in binary form must reproduce the above copyright notice, this list of conditions and the following disclaimer in the documentation and/or other materials provided with the distribution.
    * Neither the name of the Vodafone Group Services Ltd nor the names of its contributors may be used to endorse or promote products derived from this software without specific prior written permission.

THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS" AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE ARE DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT HOLDER OR CONTRIBUTORS BE LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
*/

#include "BufferRequester/BufferRequester.h"

#define CORE_LOGGING_MODULE_NAME "BufferRequester"
// #define CORE_LOGGING_MODULE_LEVEL CL_ALL_FILE
#include "CoreDebugPrint.h"

#include "MemoryDBufRequester.h"
#include "BufferRequester/DiskCacheSettings.h"
#include "BufferRequester/MemCacheSettings.h"
#include "BufferRequester/BufferRequesterListener.h"
#include "BufferRequester/BufferRequesterChain.h"
#include "TileMapToolkit.h"
#include "TileMapUtil.h"

/**
 *   Data for one request.
 */
class ReqData {
public:
   /// Should only be used internally in map.      
   ReqData() {}
   ReqData(uint32 timep, BufferRequesterListener* reqListener,
           CachePolicy whereFrom )
      : m_time(timep), m_reqListener(reqListener),
        m_whereFrom( whereFrom ) 
      {
         m_block  = true;
         m_failedTime = 0;
      }
   /// Time of request
   uint32 m_time;
   /// Listener of request.
   BufferRequesterListener* m_reqListener;
   /// Where the request may be requested from.
   CachePolicy m_whereFrom;
   /// True if there should be no more requests for the buffer
   bool m_block;
   /// Time for last failuer.
   uint32 m_failedTime;
};

/// Type of map to store data in
typedef map<MC2SimpleString, ReqData> storage_t;

struct BufferRequester::Impl {
   Impl(TileMapToolkit* toolkit,
        WFAPI::DBufConnection* netConnection,
        const MemCacheSettings& memCacheSettings,
        const DiskCacheSettings& diskCacheSettings,
        const ResourceDirectorySettings& resDirSettings) :
      toolkit(toolkit),
      netConnection(netConnection),
      defaultListener(NULL),
      reRequestTimeSec(5),
      minReRequestTimeSec(5),
      maxReRequestTimeSec(60),
      firstFailedTimeSec (0), // No failures yet
      reqChain(toolkit,
               netConnection,
               memCacheSettings,
               diskCacheSettings,
               resDirSettings),
      reqAmountLimitEnabled(true)
   {
      
   }

   // The toolkit used to request timers
   TileMapToolkit* toolkit;

   // The topmost connection to the internet
   WFAPI::DBufConnection* netConnection;
   
   /// Map containing descriptions and last request time in ms.
   storage_t requested;

   /**
    *   Minimum time to wait between re-requests in ms.
    */
   uint32 reRequestTimeSec;

   /**
    *   The absolute minimum time to wait before re-requesting.
    */
   uint32 minReRequestTimeSec;
   
   /**   
    *   The absolute minimum time to wait before re-requesting.
    */
   uint32 maxReRequestTimeSec;

   /**
    *   Time of first failed request.
    */
   uint32 firstFailedTimeSec;

   /**
    *   Default listener for requests with no recipient.
    */  
   BufferRequesterListener* defaultListener;

   /** 
    *   The class that organizes the chain of caches.
    */
   BufferRequesterChain reqChain;

   /**
    *   Whether or not we enforce a request amount limit.
    */ 
   bool reqAmountLimitEnabled;
};

BufferRequester::BufferRequester(TileMapToolkit* toolkit,
                                 WFAPI::DBufConnection* netConnection,
                                 const MemCacheSettings& memCacheSettings,
                                 const DiskCacheSettings& diskCacheSettings,
                                 const ResourceDirectorySettings& resDirSettings)
{
   m_impl = new Impl(toolkit,
                     netConnection,
                     memCacheSettings,
                     diskCacheSettings,
                     resDirSettings);
   printChain();
}


BufferRequester::~BufferRequester()
{
   delete m_impl; 
}

void BufferRequester::requestBuffer(const MC2SimpleString& descr,
                                    BufferRequesterListener* caller,
                                    const CachePolicy& whereFrom)
{
   coreprintln(__FUNCTION__);

   uint32 curTimeSec = TileMapUtil::currentTimeMillis() / 1000;
   
   mc2dbg8 << "[THM]: m_impl->firstFailedTimeSec = "
           << m_impl->firstFailedTimeSec << endl;
   mc2dbg8 << "[TMH]: m_requested.size() = " << m_impl->requested.size()
           << endl;

   if(m_impl->reqAmountLimitEnabled) {
      if ( whereFrom != CachePolicy::onlyCache && m_impl->requested.size() >= 30 ) {
         if ( m_impl->firstFailedTimeSec == 0 ) {
            return;
         } else if ( (curTimeSec - m_impl->firstFailedTimeSec) > m_impl->reRequestTimeSec ) {
            // Don't return
         } else {
            return;
         }
      }      
   }
   
   storage_t::iterator it = m_impl->requested.find(descr);
   // Only request maps if not waiting for it.
   // Later we will add timeout that removes old stuff.
   bool update = false;
   
   if ( it == m_impl->requested.end() ) {
      update = true;
   } else if ( it->second.m_block == false ) {
      update = true;
   } else if ( it->second.m_whereFrom != whereFrom ) {
      mc2dbg8 << "[TMHBBR]: whereFrom changed." << endl;
      update = true;
   } else if ( (curTimeSec - it->second.m_time) > m_impl->reRequestTimeSec ) {
      mc2dbg8 << "[TMHBBR]: Ok to request map again - timediff "
              << (curTimeSec - it->second.m_time) << " s" << endl;
      // Waited too long for the map.      
      update = true;
      // Wait one more second for next one.
      m_impl->reRequestTimeSec = MIN(m_impl->reRequestTimeSec, m_impl->maxReRequestTimeSec);
      m_impl->firstFailedTimeSec = 0;
      for ( storage_t::iterator it = m_impl->requested.begin();
            it != m_impl->requested.end();
            ++it ) {
         m_impl->firstFailedTimeSec = MAX(m_impl->firstFailedTimeSec,
                                    it->second.m_failedTime);
      }
   }
   if ( update ) {
      mc2dbg << info << "[TMH]: Requested " << descr << endl;
      m_impl->requested[descr] = ReqData(
         curTimeSec,
         caller, 
         whereFrom );
      getRoot()->request(descr, this, whereFrom );
   } else {
      // TODO: Make a requester that fails after x seconds.
      // m_impl->timerRequester->request(descr, this);
   }
}

void BufferRequester::cancelAll()
{
   coreprintln(__FUNCTION__);

   // Allow all requests to be resent.
   m_impl->requested.clear();
   getRoot()->cancelAll();
}

void BufferRequester::requestReceived(const MC2SimpleString& descr,
                                      BitBuffer* dataBuffer,
                                      const DBufRequester& origin)
{
   coreprintln(__FUNCTION__);

   //TODO: Move to policy
   // if ( origin.getType() & OriginType::EXTERNAL ) {
   //    // Now we must fixup the traffic maps since they are from the
   //    // in ter net.
   //    if ( m_handler->m_mapFormatDesc ) {
   //       dataBuffer =
   //          TileMap::writeReceiveTime( *m_handler->m_mapFormatDesc,
   //                                     descr,
   //                                     dataBuffer );
   //    }
   // }      
   
   storage_t::iterator it = m_impl->requested.find(descr);
   if ( it != m_impl->requested.end() ) {
      mc2dbg8 << "[TMH]: Received " << descr << endl;
      BufferRequesterListener* listener = it->second.m_reqListener;
      if ( dataBuffer == NULL ) {
         // Failed - release the buffer.
         // But first wait some time.
         it->second.m_failedTime = TileMapUtil::currentTimeMillis() / 1000;
         if ( m_impl->firstFailedTimeSec == 0 ) {
            m_impl->firstFailedTimeSec = it->second.m_failedTime;               
         }
      } else {
         // Decrease the waiting time
         m_impl->reRequestTimeSec = MAX(m_impl->reRequestTimeSec -1 ,
                                  m_impl->minReRequestTimeSec);         
         m_impl->requested.erase(it);
      }
      listener->requestReceived(descr, dataBuffer, origin.getType());
   } else {
      // No recipient, relay to default
      if(m_impl->defaultListener) {
         m_impl->defaultListener->requestReceived( descr, dataBuffer, origin.getType() );         
      } else {
         release(descr, dataBuffer);   
      }
   }
}

void BufferRequester::release(const MC2SimpleString& descr, BitBuffer* dataBuffer)
{
   if ( dataBuffer == NULL ) {
      return;
   }
   m_impl->requested.erase(descr);
   
   getRoot()->release(descr, dataBuffer); 
}

void BufferRequester::setBufferReleasePolicy(BufferReleasePolicy* policy)
{
   //TODO: Figure out ownership of policy
   getRoot()->setBufferReleasePolicy(policy); 
}

DBufRequester* BufferRequester::getRoot()
{
   return m_impl->reqChain.getRoot();
}

void 
BufferRequester::clearReadWriteCache()
{
   m_impl->reqChain.clearReadWriteCache();
}

void 
BufferRequester::setDiskCacheSettings(const DiskCacheSettings& diskCacheSettings)
{
   m_impl->reqChain.setDiskCacheSettings(diskCacheSettings);
}

void 
BufferRequester::setMemCacheSettings(const MemCacheSettings& memCacheSettings)
{
   m_impl->reqChain.setMemCacheSettings(memCacheSettings);
}

void BufferRequester::setHttpReqSettings(const HttpReqSettings& reqSettings)
{
   m_impl->reqChain.setHttpReqSettings(reqSettings); 
}

void
BufferRequester::printChain()
{
   m_impl->reqChain.printChain(); 
}

void BufferRequester::setReqAmountLimitEnabled(bool enabled)
{
   m_impl->reqAmountLimitEnabled = enabled; 
}

void BufferRequester::cancelRequest(const MC2SimpleString& descr,
                                    BufferRequesterListener* callback)
{
   getRoot()->cancelRequest(descr, this);
   m_impl->requested.erase(descr);
}
