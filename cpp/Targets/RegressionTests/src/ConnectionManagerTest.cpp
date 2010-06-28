/*
Copyright (c) 1999 - 2010, Vodafone Group Services Ltd
All rights reserved.

Redistribution and use in source and binary forms, with or without modification, are permitted provided that the following conditions are met:

    * Redistributions of source code must retain the above copyright notice, this list of conditions and the following disclaimer.
    * Redistributions in binary form must reproduce the above copyright notice, this list of conditions and the following disclaimer in the documentation and/or other materials provided with the distribution.
    * Neither the name of the Vodafone Group Services Ltd nor the names of its contributors may be used to endorse or promote products derived from this software without specific prior written permission.

THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS" AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE ARE DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT HOLDER OR CONTRIBUTORS BE LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
*/


#define CORE_LOGGING_MODULE_NAME "ConnMgrTest"
// #define CORE_LOGGING_MODULE_LEVEL CL_ALL_FILE

#include "ConnectionManagerTest.h"
#include "CoreDebugPrint.h"
#include "RegressionTestConnectionManager.h"
#include "Nav2Session.h"
#include "Nav2API.h"
#include "Nav2APIImpl.h"
#include "NetworkInterface.h"
#include "NetworkInterfaceImpl.h"
#include "Environment.h"


#define NETWORK_MODULE "TcpSerial"

using namespace Environment;

ConnectionManagerTest::ConnectionManagerTest(Nav2Session& session, const char* name)
   : RegressionTest(name, 20000),
     m_session(session)
{
   
}

ConnectionManagerTest::~ConnectionManagerTest()
{
   m_session.getNav2()->getImpl()->getNotificationCenter()->removeListener(
      NETWORK_MODULE, this);
}

void ConnectionManagerTest::startTest()
{
   ConnectionManager* connMgr =
      m_session.getNav2()->getNetworkInterface().getImpl()->getConnectionManager();
   
   if (connMgr == NULL){
      setTestFailed("No connection manager available.");
   }

   
   RegressionTestConnectionManager* regConnMgr =
      static_cast<RegressionTestConnectionManager*>(connMgr);

   // (!m_states.connected
   // setTestFailed("The connection is not initiated!");
   
   m_session.getNav2()->getImpl()->getNotificationCenter()->addListener(
      NETWORK_MODULE, this);
   
   clearState();
   m_states.disconnectIssued = true;
   regConnMgr->triggerNoConnectionAvailable();
   if (!conditionalWait(m_states.disconnected, getTimeOutMs()/4)){
      setTestFailed(
         "Issued noConnectionAvailable, but no disconnection done.");
      return;
   }

   clearState();
   m_states.connectIssued = true;
   regConnMgr->triggerReadyForConnection();
   if (!conditionalWait(m_states.connected, getTimeOutMs()/4)){
      setTestFailed(
         "Issued ready for connection, but no connection established");
      return;
   } 

   clearState();
   m_states.reconnectIssued = true;
   regConnMgr->triggerReconnectionNeeded();
   if (!conditionalWait(m_states.reconnected, getTimeOutMs()/4)){
      setTestFailed(
         "Issued reconnection needed, but no reconnection done.");
      return;
   }

   
   setTestPassed();
}

void ConnectionManagerTest::clearState(){
   m_states.disconnectIssued   = false;
   m_states.disconnected       = false;
   m_states.connectIssued      = false;
   m_states.connected          = false;
   m_states.reconnectIssued    = false;
   m_states.reconnected        = false;
}

void ConnectionManagerTest::notificationReceived(const std::string& source,
                                                 const std::string& info)
{

   if (m_states.disconnectIssued){
      // Determine if we got disconnection status.
      if (info == "DISCONNECTED"){
         m_states.disconnected = true;
         coreprintln("disconnected");
      }
   } else if (m_states.connectIssued){
      if (info == "CONNECTED"){
         m_states.connected = true;
         coreprintln("connected");
      }
   } else if (m_states.reconnectIssued){
      // Determine if we got reconnected status.
      if (info == "RECONNECTED"){
         m_states.reconnected = true;
         coreprintln("reconnected");
      }
   } else {
      // The test is not in a listening state at the moment.
   }
   
}

