/*
  Copyright (c) 1999 - 2010, Vodafone Group Services Ltd
  All rights reserved.

  Redistribution and use in source and binary forms, with or without modification, are permitted provided that the following conditions are met:

  * Redistributions of source code must retain the above copyright notice, this list of conditions and the following disclaimer.
  * Redistributions in binary form must reproduce the above copyright notice, this list of conditions and the following disclaimer in the documentation and/or other materials provided with the distribution.
  * Neither the name of the Vodafone Group Services Ltd nor the names of its contributors may be used to endorse or promote products derived from this software without specific prior written permission.

  THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS" AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE ARE DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT HOLDER OR CONTRIBUTORS BE LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
*/

/* This module stores permanent parameters that needs to be 
 * kept over a system restart.
 */



#ifndef MODULE_BtManagerInternal_H
#define MODULE_BtManagerInternal_H

#include <map>
#include <list>
#include "Module.h"
#include "Buffer.h"
#include "CtrlHub.h"
#include "BtManager.h"
#include "BtIfClasses.h"

namespace isab {

   class BtManager : public Module, 
                     public CtrlHubAttachedInterface, 
                     public BtManagerProviderInterface {
   public:
      BtManager();

      /** Creates a new CtrlHubAttachedPublic object used to connect this
       * module to the CtrlHub.
       * @return a new CtrlHubAttachedPublic object connected to the queue.
       */
      CtrlHubAttachedPublic * newPublicCtrlHub();

      virtual void decodedBtRequestSerial(uint8 bda[6], uint32 src);
      virtual void decodedBtSearchForDevice(uint8 bda[6], uint32 src);


   protected:


      class MyBtIf : CBtIf, RecursiveMutex {
      public:
         MyBtIf(ModuleQueue *queue, BtManagerConsumerPublic *consumer, Log *l);

         virtual void OnDeviceResponded(
               BD_ADDR   bda,
               DEV_CLASS dev_class,
               BD_NAME   bd_name,
               BOOL      bd_connected);

         virtual void OnDiscoveryComplete();

         void startupComplete();

         class ABtDevice {
            public:
            BD_ADDR   bda;
            DEV_CLASS dev_class;
            BD_NAME   bd_name;
            BOOL      bd_connected;

            ABtDevice(BD_ADDR b, DEV_CLASS d, BD_NAME bdn, BOOL bdc) : 
               bd_connected(bdc) {
               memcpy(bda, b, sizeof(bda));
               memcpy(dev_class, d, sizeof(dev_class));
               memcpy(bd_name, bdn, sizeof(BD_NAME));
               };

            ABtDevice() {} ;

         };

         class MyBtAddr {
            public: 
               BD_ADDR bda;
               MyBtAddr(BD_ADDR b) 
               {
                  memcpy(bda, b, sizeof(bda));
               }
         };

         class AddrLess {
            public:
               bool operator()(const MyBtAddr& x, const MyBtAddr& y)
               { return memcmp(x.bda, y.bda, sizeof(BD_ADDR)) < 0; }
         };

         map <MyBtAddr, ABtDevice, AddrLess> m_knownDevs;
         
         class DiscoveryReq {
            public:
               enum ReqType { SerialReq = 0 } m_reqType;
               BD_ADDR m_bda;
               uint32 m_reqSrc;
               DiscoveryReq(ReqType rqt, BD_ADDR b, uint32 src) : 
                  m_reqType(rqt), m_reqSrc(src)
                  { memcpy(m_bda, b, sizeof(m_bda)); }
               DiscoveryReq() {}
         };

         list <DiscoveryReq> m_discoveryQueue;

         void searchForDevice(BD_ADDR bda, uint32 src);
         void requestSerial(BD_ADDR bda, uint32 src);

      protected:
         ModuleQueue *m_queue;
         BtManagerConsumerPublic * m_consumer;
         Log *m_log;

         void startDiscovery();
      };



      MyBtIf *m_btIf;

      virtual MsgBuffer * dispatch(MsgBuffer *buf);

      virtual void decodedStartupComplete();
      virtual void treeIsShutdown();

      /** Decoder for CtrlHub-messages */
      CtrlHubAttachedDecoder m_ctrlHubAttachedDecoder;
      /** Decoder for BtManager-messages */
      BtManagerProviderDecoder m_BtManagerProviderDecoder;

      /** Encoder for BtManager-messages */
      BtManagerConsumerPublic * m_BtManagerConsumer;

   };

} /* namespace isab */

#endif /* MODULE_BtManagerInternal_H */
