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

#define CORE_LOGGING_MODULE_NAME "NetworkReqTest"
#define CORE_LOGGING_MODULE_LEVEL CL_ALL_FILE
#include "CoreDebugPrint.h"

#ifdef __linux__
#include <GtkTileMapToolkit.h>
#include <GtkHttpContext.h>
#endif

#ifdef IPHONE
#include <IPhoneFactory.h>
#endif

#include "MapLibNetworkContext.h"
#include "NetDBufRequester.h"
#include "BitBuffer.h"
#include "RegressionTestHelpers.h"
#include "PlatformFactory.h"
#include "NetworkReqTest.h"
#include "Environment.h"
#include "HttpDBufConnection.h"

#define REQ_STR "DXX"

#include <string>
#include <map>

using namespace std;

typedef map<string, int> sizemap;

struct NetworkReqTest::Impl {
   WFAPI::MapLibNetworkContext* netContext;
   int pending;
   NetDBufRequester* req;
   sizemap reqSizes;
   sizemap reqSizesWF;
   sizemap reqSizesOSM;
   sizemap::iterator reqItr;
   bool forceDisconnect;
   bool osm;
};

NetworkReqTest::NetworkReqTest(Nav2Session& session, const char* name) : 
   RegressionTest(name, 30000)
{
   m_impl = new Impl;
   m_impl->pending = 0;

   m_impl->reqSizesWF["Btat_restaurant.png"] = 493;
   m_impl->reqSizesWF["btat_restaurant.png"] = 659;
   m_impl->reqSizesWF["Btat_hotel.png"]      = 519;
   m_impl->reqSizesWF["btat_hotel.png"]      = 689;
   m_impl->reqSizesWF["Btat_pharmacy.png"]   = 721;
   m_impl->reqSizesWF["btat_pharmacy.png"]   = 748;
   m_impl->reqSizesWF["btat_asdf.png"]       = 187;
   m_impl->reqSizesWF["Btat_railwaystation.png"] = 495;
   m_impl->reqSizesWF["btat_railwaystation.png"] = 658;

   m_impl->reqSizesOSM["0/0/0.png"] = 8571;
   m_impl->reqSizesOSM["1/0/0.png"] = 9094;
   m_impl->reqSizesOSM["1/1/0.png"] = 7796;
   m_impl->reqSizesOSM["1/1/1.png"] = 4222;
   m_impl->reqSizesOSM["2/0/0.png"] = 5435;
   m_impl->reqSizesOSM["2/1/0.png"] = 8018;
   m_impl->reqSizesOSM["2/2/0.png"] = 6498;
   m_impl->reqSizesOSM["2/2/1.png"] = 23577;
   m_impl->reqSizesOSM["2/2/2.png"] = 9758;

//   m_impl->forceDisconnect = true;
   m_impl->osm = true;
}

void NetworkReqTest::startTest() {

   coreprintln("Note, test is not doing anything.");

   setTestPassed();

   return;
   
#ifdef __linux__
   m_impl->netContext = new GtkHttpContext;
#elif defined IPHONE
   m_impl->netContext = WFAPI::IPhoneFactory::createHTTPContext();
#endif
   
   m_impl->req =
      new NetDBufRequester(m_impl->netContext->getConnection());

   m_impl->req->setForceGet(true);

   if (m_impl->osm) {
      HttpDBufConnection* conn =
         dynamic_cast<HttpDBufConnection*>(
            m_impl->req->getDBufConnection());
         
      if(conn) {
         conn->setPostDirectory("/");
         conn->setHostAndPort("b.tile.openstreetmap.org:80");
      }
      m_impl->reqSizes = m_impl->reqSizesOSM;
   } else {
      m_impl->reqSizes = m_impl->reqSizesWF;
   }

   int numFirstRun = 5;
   m_impl->pending = numFirstRun;
   
   for(m_impl->reqItr = m_impl->reqSizes.begin();
       m_impl->reqItr != m_impl->reqSizes.end() && numFirstRun > 0;
       m_impl->reqItr++)
   {
      m_impl->req->request(m_impl->reqItr->first.c_str(), this,
                           CachePolicy::cacheOrInternet);
      
      numFirstRun--;
   }
}

NetworkReqTest::~NetworkReqTest() {
   delete m_impl;
}

void NetworkReqTest::requestReceived(const MC2SimpleString& descr,
                                     BitBuffer* dataBuffer,
                                     const DBufRequester& origin )
{
   sizemap::iterator itr = m_impl->reqSizes.find(descr.c_str());

   m_impl->pending--;
   
   if(dataBuffer == NULL) {
      setTestFailed("Receieved NULL buffer");
      return;
   } else {
      coreprintln("Recv %s / %d bytes (%d remaining)",
                  descr.c_str(),
                  dataBuffer->getBufferSize(),
                  m_impl->pending);
   }
   
   if(itr != m_impl->reqSizes.end()) {
      if(itr->second != dataBuffer->getBufferSize()) {
         coreprintln("Expected %d for %s, got %d",
                     itr->second, itr->first.c_str(), dataBuffer->getBufferSize());
         
         setTestFailed("Unexpected size!");
      }
   } else {
      setTestFailed("Non-requested reply received!");
   }
   
   if(m_impl->forceDisconnect && m_impl->reqSizes.begin()->first == descr.c_str()) {
      // Simulate disconnect after first reply.
      coreprintln("Simulating disconnect");
      m_impl->req->forceDisconnect();
      coreprintln("Done simulating disconnect");
   }

   if(m_impl->pending == 0) {

      if(m_impl->reqItr==m_impl->reqSizes.end()) {
         setTestPassed();
      } else {
         // Wait for 5s, then req more..
         coreprintln("SLEEPING FOR 6S");
         Environment::sleep(6000);
         coreprintln("DONE SLEEPING FOR 6S");
         
         for(;m_impl->reqItr != m_impl->reqSizes.end(); m_impl->reqItr++)
         {
            m_impl->req->request(m_impl->reqItr->first.c_str(), this,
                                 CachePolicy::cacheOrInternet);
            m_impl->pending++;
         }
      }
   } else {
      // printf("still waiting for %d requests\n", m_impl->pending);
   }
}


