/*
Copyright (c) 1999 - 2010, Vodafone Group Services Ltd
All rights reserved.

Redistribution and use in source and binary forms, with or without modification, are permitted provided that the following conditions are met:

    * Redistributions of source code must retain the above copyright notice, this list of conditions and the following disclaimer.
    * Redistributions in binary form must reproduce the above copyright notice, this list of conditions and the following disclaimer in the documentation and/or other materials provided with the distribution.
    * Neither the name of the Vodafone Group Services Ltd nor the names of its contributors may be used to endorse or promote products derived from this software without specific prior written permission.

THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS" AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE ARE DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT HOLDER OR CONTRIBUTORS BE LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
*/


#include "ServiceWindowTester.h"
#include "TunnelInterface.h"
#include "WFString.h"
#include <iostream>
#include <stdlib.h>

using namespace std;
using namespace WFAPI;

ServiceWindowTester::ServiceWindowTester(TunnelInterface& tunnelInterface)
   : m_tunnelInterface(tunnelInterface)
{
   m_tunnelInterface.addTunnelListener(this);
   addCommandOptions();
}
                   
ServiceWindowTester::~ServiceWindowTester()
{

}
   
void
ServiceWindowTester::requestData()
{
   cout << "SERVICE WINDOW TESTER REQUEST DATA" << endl;
   WFString destURL2 = "http://TMap/B;cat_vf_vodafoneshop.png";
   WFString destURL = "http://firstpage/";
   
   AsynchronousStatus status = m_tunnelInterface.requestData(destURL,
                                                             "successLink=retry",
                                                             "WFAPITestClient");

   
   cout << "RequestData::RequestID first: " << status.getRequestID().getID()
        << endl;

   status = m_tunnelInterface.requestData(destURL2);

   cout << "RequestData::RequestID second: " << status.getRequestID().getID()
        << endl;
   
}

void
ServiceWindowTester::error(const AsynchronousStatus& status)
{
   cout << "some kind of error" << endl;
   cout << "ReqID: " << status.getRequestID().getID() << endl;
   printErrorStatus(&status, "ServiceWindowTester");
}

bool 
ServiceWindowTester::handleInput(const char* input, bool& goBack)
{
   StringMap::const_iterator it = m_commandOptions.find(input);

   if (it == m_commandOptions.end()) {
      return false;
   }
   switch((*it).second.m_commandType) {
   case help:  
      printHelp();
      break;
   case cmdRequestData:
      requestData();
      break;
   case back:
      goBack = true;
      return false;
      break;
   default:
      cout << "Command not handled here, please try again or select B to "
           << "go back." << endl;
      return false;
      break;
   }

   // If we get here, the key has been consumed.
   return true;
}   

void 
ServiceWindowTester::addCommandOptions()
{
   m_commandOptions.
      insert(make_pair("h",
                       CommandHolder("Displays this help menu.", help)));
   m_commandOptions.
      insert(make_pair("r", 
                       CommandHolder("requestData", 
                                     cmdRequestData)));
   m_commandOptions.
      insert(make_pair("b", 
                       CommandHolder("Back", 
                                     back)));
}

void ServiceWindowTester::dataReceived(RequestID requestID, ReplyData replyData)
{
   cout << "dataReceived" << endl;
   cout << "RequestID: " << requestID.getID() << endl;
   cout << "Total data size: " << replyData.getTotalDataSize() << endl;
//   writeToFile(replyData);
   cout << "This data length: " << replyData.getSize() << endl;
   cout << "Complete: " << replyData.isComplete() << endl;
   cout << "Delete!" << endl;

//   cout << replyData.getData() << endl;

}

bool
ServiceWindowTester::writeToFile(const ReplyData& replyData)
{
   WFString imageName = "tunnelData.txt";
   // Create the file, if file already exists the content is erased.
   FILE* pSaveFile = fopen(imageName.c_str(), "a");
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



