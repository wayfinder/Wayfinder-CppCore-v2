/*
  Copyright (c) 1999 - 2010, Vodafone Group Services Ltd
  All rights reserved.

  Redistribution and use in source and binary forms, with or without modification, are permitted provided that the following conditions are met:

  * Redistributions of source code must retain the above copyright notice, this list of conditions and the following disclaimer.
  * Redistributions in binary form must reproduce the above copyright notice, this list of conditions and the following disclaimer in the documentation and/or other materials provided with the distribution.
  * Neither the name of the Vodafone Group Services Ltd nor the names of its contributors may be used to endorse or promote products derived from this software without specific prior written permission.

  THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS" AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE ARE DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT HOLDER OR CONTRIBUTORS BE LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
*/


#include "TcpSerial.h"
#include <ctype.h>
#include "TcpThread.h"
#include "nav2util.h"
#include "DNSCache.h"
#include "LogMacros.h"
#include "NotificationCenter.h"
#include "Synchronized.h"
#include "Mutex.h"

#define CORE_LOGGING_MODULE_NAME "TcpSerial"
// #define CORE_LOGGING_MODULE_LEVEL CL_ALL_FILE

#include "CoreDebugPrint.h"

// #define SYMBIAN_DELAY_DISCONNECT
#define SYMBIAN_DELAY_AMOUNT (1000*10)
namespace {
   inline bool delayDisconnect()
   {
#ifdef SYMBIAN_DELAY_DISCONNECT
      return true;
#else
      return false;
#endif
   }
}

#define SLEEP_AT_CONNECT 1000

namespace isab {
TcpSerial::TcpSerial(const char* name, NotificationCenter* notificationCenter) :
      Module(name ? name : "TcpSerial"), m_tcpThread(NULL), 
      m_threadNum(0), m_state(CLEAR), m_keepConnectionTimeout(0),
#ifdef __WINS__
      m_connectionParam(-2),
#else
      m_connectionParam(-1),
#endif
      m_lastSent(ConnectionNotify(-1)),
      m_notificationCenter(notificationCenter),
      m_reconnectionNeeded(false),
      m_lastUsedHostName(""),
      m_lastUsedPort(-1)
   {
      DBG("TcpSerial waiting for orders");
      m_paramProvider = new ParameterProviderPublic(this);
      m_connectionTimer = m_queue->defineTimer();
      m_triggerConnectTimer = m_queue->defineTimer();
      m_triggerDisconnectTimer = m_queue->defineTimer();
      m_reconnectionNeededMutex = new isab::Mutex;
      m_dnsCache = new DNSCache();
   }


   TcpSerial::TcpSerial(const char *hostname, int port, const char* name) :
      Module(name ? name : "TcpSerial"), m_tcpThread(NULL), m_threadNum(0),
      m_state(CLEAR), 
      m_keepConnectionTimeout(0), 
#ifdef __WINS__
      m_connectionParam(-2),
#else
      m_connectionParam(-1),
#endif
      m_lastSent(ConnectionNotify(-1)),
      m_notificationCenter(NULL),
      m_reconnectionNeeded(false),
      m_lastUsedHostName(""),
      m_lastUsedPort(-1)
   {
      DBG("TcpSerial(\"%s\", %i)\n", hostname, port);
      m_paramProvider = new ParameterProviderPublic(this);
      m_connectionTimer = m_queue->defineTimer();
      m_triggerConnectTimer = m_queue->defineTimer();
      m_triggerDisconnectTimer = m_queue->defineTimer();
      m_reconnectionNeededMutex = new isab::Mutex;
      m_dnsCache = new DNSCache();

      newThread(port, hostname, 1);
      DBG("TcpSerial end");
   }
   
   TcpSerial::TcpSerial(int port, const char* name) :
      Module(name ? name : "TcpSerial"), m_tcpThread(NULL), m_threadNum(0),
      m_state(CLEAR), m_keepConnectionTimeout(0), 
#ifdef __WINS__
      m_connectionParam(-2),
#else
      m_connectionParam(-1),
#endif
      m_lastSent(ConnectionNotify(-1)),
      m_notificationCenter(NULL),
      m_reconnectionNeeded(false),
      m_lastUsedHostName(""),
      m_lastUsedPort(-1)
   {
      DBG("TcpSerial(%i)\n", port);
      m_paramProvider = new ParameterProviderPublic(this);
      m_connectionTimer = m_queue->defineTimer();
      m_triggerConnectTimer = m_queue->defineTimer();
      m_triggerDisconnectTimer = m_queue->defineTimer();
      m_reconnectionNeededMutex = new isab::Mutex;
      m_dnsCache = new DNSCache();

      newThread(port);
   }

   TcpSerial::~TcpSerial()
   {
      delete m_reconnectionNeededMutex;
      delete m_paramProvider;
      delete m_dnsCache;
   }

   void TcpSerial::decodedStartupComplete()
   {
      Module::decodedStartupComplete();

      // Parameters we need and will subscribe to.
      ParameterEnums::ParamIds params[] = {ParameterEnums::SelectedAccessPointId2};
      //sort the list to facilitate joining
      std::sort(params, params + sizeof(params)/sizeof(*params));
      //for loop to get parameters and join parameter multicast groups
      uint16 last = 0;
      for(unsigned j = 0; j < sizeof(params)/sizeof(*params); ++j){
         uint16 group = ParameterProviderPublic::paramIdToMulticastGroup(params[j]);
         m_paramProvider->getParam(params[j]);
         if(group != last){
            m_rawRootPublic->manageMulticast(JOIN, group);
         }
         last = group;
      }
      //subscribe to navtask messages.
      m_rawRootPublic->manageMulticast(JOIN, Module::NavTaskMessages);
      if(m_tcpThread){
         DBG("TcpSerial starting TcpThread");
         if (m_tcpThread->start() == Thread::GOOD_START) {
            DBG("TcpThread start ok.");
            THREADINFO(*m_tcpThread);
         } else {
            DBG("TcpThread start NOT ok");
         }
      }
   }

   void TcpSerial::decodedShutdownPrepare( int16 upperTimeout )
   {
      coretracefunc();
      delete m_tcpThread;
      m_tcpThread = NULL;
      Module::decodedShutdownPrepare( upperTimeout );
   }

   void TcpSerial::treeIsShutdown()
   {

      coretracefunc();
      coreprintln("Deleting TCP thread");
      delete m_tcpThread;
      m_tcpThread = NULL;
      Module::treeIsShutdown();
   }

   SerialProviderPublic * TcpSerial::newPublicSerial()
   {
      DBG("newPublicSerial()\n");
      SerialProviderPublic* spp = new SerialProviderPublic(m_queue);
      return spp;
   }
   
   MsgBuffer * TcpSerial::dispatch(MsgBuffer *buf)
   {
      //DBG("TcpSerial::dispatch : %i\n",  buf->getMsgType());
      buf = m_providerDecoder.dispatch(buf, this);
      if(buf) buf = m_paramDecoder.dispatch(buf, this);
      if(buf) buf = Module::dispatch(buf);
      return buf;
   }

   void TcpSerial::decodedExpiredTimer(uint16 timerID)
   {
      DBG("decodedExpiredTimer(%u)", timerID);
#ifdef SYMBIAN_DELAY_DISCONNECT
      if(timerID == m_connectionTimer){
         if(m_tcpThread != NULL){
            closeThread();
            DBG("deleted tcpThread.");
         }
         return;
      }
#endif
      if (timerID == m_triggerConnectTimer){
         // We will replace the m_lastUsedHostName member further down the
         // call stack so we need to use a temp when passing the const char*.
         std::string tmp(m_lastUsedHostName);

         // Trigger a connection using the parameters we used last time.
         handleConnect(tmp.c_str(), m_lastUsedPort, 0);
         
      } else if (timerID == m_triggerDisconnectTimer){
         // Trigger a disconnection.
         handleDisconnect(NULL, 0);
         
      } else {
         Module::decodedExpiredTimer(timerID);         
      }
   }


   void TcpSerial::closeThread()
   {
      delete m_tcpThread;
      m_tcpThread = NULL;
   }

   void TcpSerial::newThread(int port, const char* hostname, int reconnect)
   {
      char threadName[128] = {0};
      snprintf(threadName, sizeof(threadName) - 1, 
               "%d%s_TcpThread", m_threadNum++, getName());
      DBG("New TcpThread named '%s'", threadName);
# ifdef SLEEP_AT_CONNECT
      sleep(SLEEP_AT_CONNECT, 0);
# endif
      m_tcpThread = new TcpThread(threadName, m_connectionParam, this, port, 
                                  hostname, reconnect);
   }
   
   Thread::startStatus TcpSerial::startNewThread(int port, const char* hostname, 
                                                 int reconnect)
   {
      m_lastUsedHostName = std::string(hostname);
      m_lastUsedPort = port;

      newThread(port, hostname, reconnect);
      enum startStatus status = startStatus(m_tcpThread->start());
      if(status == Thread::GOOD_START){
         THREADINFO(*m_tcpThread);
      }
      return status;
   }

   void TcpSerial::decodedConnectionCtrl(enum ConnectionCtrl ctrl, 
                                         const char *method, uint32 src)
   {
      if (!rootPublic() || m_shutdownPrepared) {
         return;
      }

      switch(ctrl){
      case CONNECT:
      {            
            coreprintln_info("CONNECT %s", method);
            char* tmp = strdup_new(method);
            char* host = NULL;
            uint16 port = 0;
#ifdef SYMBIAN_DELAY_DISCONNECT
            if(m_queue->isTimerSet(m_connectionTimer)){
               DBG("stopping timer %u", m_connectionTimer);
               m_queue->stopTimer(m_connectionTimer);
            }
#endif
            if(!parseHost(tmp, &host, &port)){
               coreprintln_warn("parsing of %s failed.", method);
               sendConnectionNotify(DISCONNECTING, METHOD_ERROR, src);
               // this look suspicious
               //rootPublic()->connectionNotify(CLEAR, UNSPECIFIED, src);
            } else {
               handleConnect(host, port, src);
               
            }
            delete[] tmp;
         }
         break;
      case DISCONNECT:
         handleDisconnect(method, src);
         coreprintln_info("DISCONNECTION COMPLETE");
         break;
      case QUERY:
         coreprintln_info("QUERY %s", method);
         {
            enum ConnectionNotifyReason reason = UNSPECIFIED;
            if(m_tcpThread){
               reason = m_tcpThread->getDisconnectReason();
            }
            //no lastsent check here. This is after all an explicit request.
            rootPublic()->connectionNotify(m_state, reason, src);
         }
         break;
      default:
         coreprintln_error("ConnectionCtrl: Unknown ctrl value: %d", ctrl);
      }
      coreprintln("end of decodedConnectionCtrl");
   }

#define CNSTRING(ctrl)                                                    \
      ((ctrl == CONNECTING) ? "CONNECTING" :                              \
       ((ctrl == CONNECTED) ? "CONNECTED" :                               \
        ((ctrl == DISCONNECTING) ? "DISCONNECTING" :                      \
         ((ctrl == CLEAR) ? "CLEAR" :                                     \
          ((ctrl == WAITING_FOR_USER ? "WAITING_FOR_USER" : "?????"))))))

   void TcpSerial::connectionNotify(enum ConnectionNotify ctrl)
   {
      coreprintln_info("%s", CNSTRING(ctrl));
      m_state = ctrl;

      switch (m_state) {
      case CONNECTED:
         if (getReconnectionNeeded()){
            
            postNotification("RECONNECTED");
            setReconnectionNeeded(false);
         } else {
            
            postNotification("CONNECTED");
         }
         break;
         
      case CLEAR:
         postNotification("DISCONNECTED");
         
         if (getReconnectionNeeded()){
            m_queue->setTimer(m_triggerConnectTimer, 1);
         }
         break;

      case CONNECTING:
      case DISCONNECTING:
      case  WAITING_FOR_USER:
         break;
                  
      }
      
      ConnectionNotifyReason reason = UNSPECIFIED;
      if(m_tcpThread){
         reason = m_tcpThread->getDisconnectReason();
      }
      DBG("Sending connection notify upwards");
      sendConnectionNotify(m_state, reason, 
                           uint32(MsgBufferEnums::ADDR_LINK_LOCAL));
   }

   void TcpSerial::decodedSendData(int length, const uint8 *data, uint32 /*src*/)
   {
      DBG("TcpSerial::decodedSendData(%i, %p)\n", length, data);
      if(m_tcpThread){
         int ret = 0;
         if(0 > (ret = m_tcpThread->write(data, ssize_t(length)))){
            coreprintln_error("No data sent. %d", ret);
         }
      } else {
         coreprintln_warn("Cannot send data while disconnected!");
      }
   }

   SerialConsumerPublic * TcpSerial::rootPublic()
   {

      // This is actually safe since the connect message is only sent 
      // by a directly attached public class to the module. Since we
      // created the public interface (of type SerialProviderPublic)
      // we know that that is the class that created the connect message.
      // SerialProviderPublic in turn only accepts a 
      // SerialConsumerPublic as parameter.
      return static_cast<SerialConsumerPublic *>(m_rawRootPublic);
   }


#ifdef __SYMBIAN32__
   void TcpSerial::Kill(int reason, Thread* originator)
   {
      if(m_tcpThread){
         m_tcpThread->Kill(reason, originator);
      }
      Module::Kill(reason, originator);
   }
#endif

   void TcpSerial::decodedParamNoValue(uint32 paramId, uint32 /*src*/, uint32 /*dst*/)
   {
      switch(paramId){
      case ParameterEnums::SelectedAccessPointId2:
#ifdef __WINS__
         m_connectionParam = -2;
#else
         m_connectionParam = -1;
#endif
         if(m_tcpThread){
            m_tcpThread->setConnectionParam(m_connectionParam);
         }
         break;
      default:
         break;
      }
   }

   void TcpSerial::decodedParamValue(uint32 paramId, const int32 * data,
                                     int32 /*numEntries*/, uint32 /*src*/, 
                                     uint32 /*dst*/)
   {
      switch(paramId){
      case ParameterEnums::SelectedAccessPointId2:
         m_connectionParam = *data;
         if(m_tcpThread){
            m_tcpThread->setConnectionParam(m_connectionParam);
         }
         break;
      default:
         break;
      }
   }


   void TcpSerial::decodedParamValue( uint32 param, const char*const* data,
                                      int32 /*numEntries*/, uint32 /*src*/, 
                                      uint32 /*dst*/)
   {
      switch ( param ) {
         case ParameterEnums::SelectedAccessPointId2: {
            char* tmpStr = NULL;
            int32 p = strtol( data[ 0 ], &tmpStr, 10 );
            if ( tmpStr != data[ 0 ] ) {
               m_connectionParam = p;
               if ( m_tcpThread != NULL ) {
                  m_tcpThread->setConnectionParam( m_connectionParam );
               }
            }
         }  break;
            
         default:
            break;
      }
   }


   void TcpSerial::sendConnectionNotify(enum ConnectionNotify msg, 
                                        enum ConnectionNotifyReason rsn, 
                                        uint32 dst)
   {
      if(rootPublic() && m_lastSent != msg && 
           !(m_lastSent == CLEAR && msg == DISCONNECTING)){
         DBG("Really sending %s upward", CNSTRING(msg));
         rootPublic()->connectionNotify(msg, rsn, dst);
         DBG("connext notify sent");
         m_lastSent = msg;
         DBG("m_lastSent set");
      }
      DBG("Leaving sendConnectionNotify");
   }

   class DNSCache& TcpSerial::getDNSCache()
   {
      return *m_dnsCache;
   }

void
TcpSerial::handleConnect(const char* host, uint16 port, uint32 src)
{
   coreprintln("handleConnect: %s, %d, %d", host, port, src);
   if(m_tcpThread && !m_tcpThread->isAlive()){
      coreprintln("Closing TcpThread");
      closeThread();
   }
   if(!m_tcpThread){ //not connected before.
      coreprintln("CONNECT: creating TcpThread, was NULL before.");
                  
      coreprintln("Not alredy connected");
      startStatus start = startNewThread(port, host);
      if(GOOD_START != start){
         coreprintln_error("TcpThread didn't start. Errorcode %d", start);
         closeThread();
         coreprintln_error("TcpThread deleted");
         if (src != 0){
            // This function can be called from a ConnectionManager
            // notification, then we have no source to notify.
            // TODO: postNotification in else!
            sendConnectionNotify(DISCONNECTING, INTERNAL_ERROR,src);            
         }

         coreprintln("Notified Consumer %p", rootPublic());
      } else {
         coreprintln("TcpThread started.");
      }
      coreprintln("Started or not, here i GO....");
   } else if(host != NULL){
      coreprintln("already connected, new connection initiated");
      const char* hostname = m_tcpThread->getHostname();
      if(hostname == NULL || strcmp(host, hostname) != 0 || 
         port != m_tcpThread->getPort()){
         // disconnect current and connect to a new server.
         coreprintln_info("disconnect %s:%d and connect to %s:%d",
                          m_tcpThread->getHostname(), m_tcpThread->getPort(),
                          host, port);
         closeThread();
         startStatus start = startNewThread(port, host);
         if(GOOD_START != start){
            coreprintln_error("TcpThread didn't start. Errorcode %d", start);
            closeThread();
            if (src != 0){
               // This function can be called from a ConnectionManager
               // notification, then we have no source to notify.
               // TODO: postNotification in else!
               sendConnectionNotify(DISCONNECTING, INTERNAL_ERROR, 
                                    src);               
            }

         } else {
            coreprintln("New tcpthread sterted.");
         }
      } else {
         coreprintln_info("Already connected to host. Reuse and notify.");
         if (src != 0){
            // This function can be called from a ConnectionManager
            // notification, then we have no source to notify.
            // TODO: postNotification in else!
            sendConnectionNotify(CONNECTING, UNSPECIFIED, src);
         }
         
         if ( m_tcpThread->socketConnected() ) {
            if (src != 0){
               // This function can be called from a ConnectionManager
               // notification, then we have no source to notify.
               sendConnectionNotify( CONNECTED, UNSPECIFIED, 
                                     src );
      
            } 
         }
      }
   } else if(port != m_tcpThread->getPort()){
      coreprintln("Alredy connected, listening connection initiated");
      closeThread();
      startNewThread(port);
   }
}

void
TcpSerial::handleDisconnect(const char* method, uint32 src) {
   coreprintln_info("DISCONNECT %s", method);

   m_keepConnectionTimeout = 0;
   if(method && isdigit(*method)){
      uint32 timeoutTime = 0;
      char* endp = NULL;
      timeoutTime = strtoul(method, &endp, 10);
      if(endp != method){
         m_keepConnectionTimeout = timeoutTime;
      }
   } else if(method && strequ(method, "FAKETIMEOUT")){
      if(m_tcpThread){
         coreprintln_info("disconnectreason set to NoResponse");
         m_tcpThread->setDisconnectReason(TcpThread::NoResponse);
      }
   }
   DBG("KeepConnectionTimeout parsed to %"PRId32,m_keepConnectionTimeout);
   if(m_tcpThread){
      coreprintln("method: %p, delay: %s, *method: %s", method, 
                  delayDisconnect() ? "true" : "false", method);
      if(method && delayDisconnect() && 
         !strequ(method, "FAKETIMEOUT")){
         //#ifdef SYMBIAN_DELAY_DISCONNECT
         coreprintln("Fake disconnect");

         // This function can be called from a ConnectionManager
         // (when src == 0) notification, then we have no source to notify.
         if (src != 0){
            sendConnectionNotify(DISCONNECTING, REQUESTED, src);
            sendConnectionNotify(CLEAR, UNSPECIFIED, src);            
         }
         m_queue->setTimer(m_connectionTimer, SYMBIAN_DELAY_AMOUNT);
      } else {
         DBG("Real disconnect, deleting TcpThread");
         //#else
         closeThread();
         coreprintln("deleted tcpThread.");
      }
      //#endif
   } else {
      DBG("m_state = CLEAR");
      m_state = CLEAR;
   }

}

void TcpSerial::readyForConnection(){
   coreprintln("ConnectionListener::readyForConnection");
   
   // Create a timer that triggers a connection.
   m_queue->setTimer(m_triggerConnectTimer, 1);

}

void TcpSerial::setReconnectionNeeded(bool needed){
   isab::Synchronized lock(*m_reconnectionNeededMutex);
   m_reconnectionNeeded = needed;
}

bool TcpSerial::getReconnectionNeeded(){
   isab::Synchronized lock(*m_reconnectionNeededMutex);
   return m_reconnectionNeeded;
}

void TcpSerial::reconnectionNeeded(){
   setReconnectionNeeded(true);
   m_queue->setTimer(m_triggerDisconnectTimer, 1);
   
   coreprintln("ConnectionListener::reconnectionNeeded");
}

void TcpSerial::noConnectionAvailable(){

   // Create a timer that triggers a disconnection.
   m_queue->setTimer(m_triggerDisconnectTimer, 1);

   coreprintln("ConnectionListener::noConnectionAvailable");
}

void TcpSerial::postNotification(const std::string& info){
   if (m_notificationCenter != NULL){
      m_notificationCenter->postNotification(CORE_LOGGING_MODULE_NAME,info);
   }

}

} /* namespace isab */


