/*
Copyright (c) 1999 - 2010, Vodafone Group Services Ltd
All rights reserved.

Redistribution and use in source and binary forms, with or without modification, are permitted provided that the following conditions are met:

    * Redistributions of source code must retain the above copyright notice, this list of conditions and the following disclaimer.
    * Redistributions in binary form must reproduce the above copyright notice, this list of conditions and the following disclaimer in the documentation and/or other materials provided with the distribution.
    * Neither the name of the Vodafone Group Services Ltd nor the names of its contributors may be used to endorse or promote products derived from this software without specific prior written permission.

THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS" AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE ARE DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT HOLDER OR CONTRIBUTORS BE LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
*/


#define CORE_LOGGING_MODULE_NAME "TUNNEL_INTERFACE_REG_TEST"
//#define CORE_LOGGING_MODULE_LEVEL CL_ALL 
#include "TunnelInterfaceRegTest.h"
#include "CoreDebugPrint.h"
#include "Nav2Session.h"
#include "Nav2API.h"
#include "RegressionTestErrorPrinting.h"

#include <string>

using namespace WFAPI;
using namespace std;



TunnelInterfaceRegTest::TunnelInterfaceRegTest(Nav2Session& session, const char* name)
   : RegressionTest(name, 50000),
     m_tunnelInterface(session.getNav2()->getTunnelInterface())

{
   m_state = UNINITIALIZED;
}

TunnelInterfaceRegTest::~TunnelInterfaceRegTest()
{
   m_tunnelInterface.removeTunnelListener(this); 
}

void TunnelInterfaceRegTest::startTest()
{
   m_tunnelInterface.addTunnelListener(this);

   WFString URL = "http://firstpage/";
   WFString parameter = "successLink=wf://retry";
   WFString clientVersion = "CoreRegTestClient";
   AsynchronousStatus status =
      m_tunnelInterface.requestData(URL, parameter, clientVersion);
   m_state = WEB_REQ;
   if(status.getStatusCode() != OK) {
      m_state = ERROR;
      setTestFailed(status.getStatusMessage());
   }


}

bool TunnelInterfaceRegTest::finished()
{
   return (m_state == DONE || m_state == ERROR);
}

void
TunnelInterfaceRegTest::dataReceived(RequestID requestID, ReplyData replyData)
{
   if(replyData.getTotalDataSize() == 0) {
      m_state = ERROR;
      setTestFailed("ReplyData did not contain any data");
   } else {
      coreprintln("dataReceived!");
      
      if(m_state == WEB_REQ) {
         bool res = writeFile(replyData, "webpage.txt");
         if(!res) {
            m_state = ERROR;
            setTestFailed("Where not able to write to file");
         }
         if(replyData.isComplete()) {
            WFString URL = "http://TMap/B;cat_vf_vodafoneshop.png";
            AsynchronousStatus status =
               m_tunnelInterface.requestData(URL);
            m_state = PIC_REQ;
            if(status.getStatusCode() != OK) {
               m_state = ERROR;
               setTestFailed(status.getStatusMessage());
            }
         }
      } else if(m_state == PIC_REQ) {
         bool res = writeFile(replyData, "picture.png");
         if(!res) {
            m_state = ERROR;
            setTestFailed("Where not able to write to file");
         }
         if(replyData.isComplete()) {
            m_state = DONE;
            setTestPassed();
         }
      }
   }


}

void TunnelInterfaceRegTest::error(const WFAPI::AsynchronousStatus& status)
{
   m_state = ERROR;
   setTestFailed("ERROR!!!");
}

bool
TunnelInterfaceRegTest::writeFile(ReplyData replyData, WFString fileName)
{
   // Create the file, if file already exist data will be appended.
   FILE* pSaveFile = fopen(fileName.c_str(), "ab");
   if (pSaveFile == NULL) {
      return false;
   }
   // Get the buffer for writing
   if (replyData.getData() != NULL) {
      // Write the buffer to the file
      fwrite(replyData.getData(),
             replyData.getSize(),
             1, pSaveFile);
      fclose(pSaveFile);
      return true;
   }
   // No buffer, this should never happen
   fclose(pSaveFile);
   return false;
}


