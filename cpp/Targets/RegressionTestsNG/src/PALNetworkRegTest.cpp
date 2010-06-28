/*
Copyright (c) 1999 - 2010, Vodafone Group Services Ltd
All rights reserved.

Redistribution and use in source and binary forms, with or without modification, are permitted provided that the following conditions are met:

    * Redistributions of source code must retain the above copyright notice, this list of conditions and the following disclaimer.
    * Redistributions in binary form must reproduce the above copyright notice, this list of conditions and the following disclaimer in the documentation and/or other materials provided with the distribution.
    * Neither the name of the Vodafone Group Services Ltd nor the names of its contributors may be used to endorse or promote products derived from this software without specific prior written permission.

THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS" AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE ARE DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT HOLDER OR CONTRIBUTORS BE LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
*/


#define CORE_LOGGING_MODULE_NAME "PALNETWORK_REG_TEST"
//#define CORE_LOGGING_MODULE_LEVEL CL_ALL 

#include "PALNetworkRegTest.h"
#include "CoreDebugPrint.h"
#include "Nav2Session.h"
#include "Nav2API.h"
#include "RegressionTestErrorPrinting.h"

#include <string.h>

PALNetworkRegTest::PALNetworkRegTest(Nav2Session& session, const char* name)
   : RegressionTest(name, 50000),
     m_networkContext(NULL),
     m_socketContext(NULL),
     m_connectionState(pal::UNINITIALIZED)
{
   m_state = UNINITIALIZED;
}

PALNetworkRegTest::~PALNetworkRegTest()
{

   
}

void PALNetworkRegTest::startTest()
{
   coreprintln("STARTING TEST!");
   pal::pstatus status = pal::PAL_OK;

   status = pal::createNetworkContext(&m_networkContext);
   coreprintln("createNetworkContext status: %d", status);
   if(status != pal::PAL_OK){
      m_state = ERROR;
      setTestFailed("Unable to create networkContext");
      return;
   }

   status = pal::addNetworkCallbackFunc(m_networkContext,
                                        &CnetworkCallback,
                                        this);
   coreprintln("addCallbackFunc status: %d", status);
   if(status != pal::PAL_OK){
      m_state = ERROR;
      setTestFailed("Unable to add callback function.");
      return;
   }
   
   status = pal::setNetworkContextParameters(m_networkContext,
                                             NULL);

   status = pal::initNetworkContext(m_networkContext);
   
   coreprintln("addCallbackFunc status: %d", status);
   if(status != pal::PAL_OK){
      m_state = ERROR;
      setTestFailed("Unable to add callback function.");
      return;
   }

   if(m_connectionState != pal::INITIALIZED) {
      setTestFailed("Network is not initilized");
      return;
   }
   status = pal::createSocket(&m_socketContext);
   coreprintln("createSocket status: %d", status);
   if(status != pal::PAL_OK){
      m_state = ERROR;
      setTestFailed("Unable to create socket.");
      return;
   }

   struct pal::ServerParameters serverAddress;
   serverAddress.serverAddress = "localhost";
   serverAddress.serverPort = 5555;
   
   status = pal::setSocketContextParameters(m_socketContext,
                                            serverAddress);
   coreprintln("setConnectionParams status: %d", status);
   if(status != pal::PAL_OK){
      m_state = ERROR;
      setTestFailed("Unable to set connection parameters.");
      return;
   }
   
   status = pal::connect(m_socketContext);
   coreprintln("connectSocket status: %d", status);
   if(status != pal::PAL_OK){
      m_state = ERROR;
      setTestFailed("Unable to connect to server.");
      return;
   }

   if(!(writeToSocket("First message!"))){
      return;
   }

   if(!readFromSocket()) {
      return;
   }

   if(!(writeToSocket("Second message!"))){
      return;
   }

   if(!readFromSocket()) {
      return;
   }

   status = pal::disconnect(m_socketContext);
   if(status != pal::PAL_OK){
      m_state = ERROR;
      setTestFailed("Disconnect Failed.");
      return;
   }

   // status = pal::createSocket(m_networkContext, &m_socketContext);
   // coreprintln("createSocket status: %d", status);
   // if(status != pal::PAL_OK){
   //    m_state = ERROR;
   //    setTestFailed("Unable to create socket.");
   // }

   // status = pal::connect(m_networkContext, m_socketContext);
   // coreprintln("connectSocket status: %d", status);
   // if(status != pal::PAL_OK){
   //    m_state = ERROR;
   //    setTestFailed("Unable to connect to server.");
   // }

   // if(!(writeToSocket("Third message!"))){
   //    return;
   // }

   // if(!readFromSocket()) {
   //    return;
   // }
   
   // status = pal::removeNetworkCallbackFunc(m_networkContext,
   //                                         &CnetworkCallback,
   //                                         this);
   // coreprintln("removeCallbackFunc status: %d", status);
   // if(status != pal::PAL_OK){
   //    m_state = ERROR;
   //    setTestFailed("Unable to remove callback function.");
   //    return;
   // }
   
   
   status = pal::destroyNetworkContext(m_networkContext);
   if(status != pal::PAL_OK){
      m_state = ERROR;
      setTestFailed("Failed to destroy network context.");
      return;
   }
   
   setTestPassed();
}




bool PALNetworkRegTest::finished()
{
   if(m_state == DONE)
      return true;
   return false;
}

void
PALNetworkRegTest::CnetworkCallback(pal::NetworkContext networkContext,
                                    pal::NetConnectionState connectionState,
                                    void* extendedErrorData,
                                    unsigned int platformStatus,
                                    void* data)
{
   (static_cast<PALNetworkRegTest*>(data))
      ->networkCallback(networkContext, connectionState, extendedErrorData, platformStatus);
}

void
PALNetworkRegTest::networkCallback(pal::NetworkContext networkContext,
                                   pal::NetConnectionState connectionState,
                                   void* extendedErrorData,
                                   unsigned int platformStatus)
{
   coreprintln("connectionState: %d", connectionState);
   m_connectionState = connectionState;
   
}

bool PALNetworkRegTest::writeToSocket(WFAPI::WFString buffer) {
   pal::pstatus status = pal::PAL_OK;
   bool writeReady = true;
   int timeout = 0;
   // Check if the socket is ready to be written to
   status = pal::select(m_socketContext,
                        &timeout,
                        NULL,
                        &writeReady);
   if(status == pal::PAL_OK){
      if(writeReady) {
         coreprintln("The socket is ready to be written to");

         unsigned int res = 0;
         status = pal::write(m_socketContext,
                             buffer.c_str(),
                             strlen(buffer.c_str()),
                             &res);
         coreprintln("Data written: %d", res);
         if(status != pal::PAL_OK){
            m_state = ERROR;
            setTestFailed("Write failed.");
            return false;
         }
         
      }
      
   } else {
      m_state = ERROR;
      setTestFailed("Select returned some error!");
      return false;
   }

   return true;
}

bool PALNetworkRegTest::readFromSocket() {
   pal::pstatus status = pal::PAL_OK;
   bool readReady = true;
   int timeout = 5;
   // Check if the socket is ready to be written to
   status = pal::select(m_socketContext,
                        &timeout,
                        &readReady,
                        NULL);
   if(status == pal::PAL_OK){
      coreprintln("PALOK");
      if(readReady) {
         coreprintln("The socket is ready to be read from");

         char readbuf[200];
         unsigned int bytesRead = 0;
         status = pal::read(m_socketContext,
                            readbuf,
                            199,
                            &bytesRead);

         coreprintln("Read result: %s", readbuf);
         if(status != pal::PAL_OK){
            m_state = ERROR;
            setTestFailed("Read Failed");
            return false;
         }
         WFAPI::WFString result(readbuf);
         
      } else {
         coreprintln("The socket is not ready");
      }
      
   } else {
      m_state = ERROR;
      setTestFailed("Select returned some error!");
      return false;
   }

   return true;
}
