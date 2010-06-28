/*
Copyright (c) 1999 - 2010, Vodafone Group Services Ltd
All rights reserved.

Redistribution and use in source and binary forms, with or without modification, are permitted provided that the following conditions are met:

    * Redistributions of source code must retain the above copyright notice, this list of conditions and the following disclaimer.
    * Redistributions in binary form must reproduce the above copyright notice, this list of conditions and the following disclaimer in the documentation and/or other materials provided with the distribution.
    * Neither the name of the Vodafone Group Services Ltd nor the names of its contributors may be used to endorse or promote products derived from this software without specific prior written permission.

THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS" AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE ARE DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT HOLDER OR CONTRIBUTORS BE LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
*/


#include "NetworkTester.h"
#include "NetworkInterface.h"
#include "WGS84Coordinate.h"
#include "NetworkLinkID.h"
#include "WFString.h"
#include <iostream>

using namespace std;
using namespace WFAPI;

NetworkTester::NetworkTester(NetworkInterface& networkInterface)
   : m_networkInterface(networkInterface)
{
   m_networkInterface.addNetworkListener(this);
   addCommandOptions();
}
                   
NetworkTester::~NetworkTester()
{

}

void
NetworkTester::error(const AsynchronousStatus& status)
{
   printErrorStatus(&status, "NetworkTester");
}

bool 
NetworkTester::handleInput(const char* input, bool& goBack)
{
   StringMap::const_iterator it = m_commandOptions.find(input);

   if (it == m_commandOptions.end()) {
      return false;
   }
   switch((*it).second.m_commandType) {
      case help:  
         printHelp();
         break;
      case back:
         goBack = true;
         return false;
         break;
      case cmdSetNetworkLinkID:
         setNetworkLinkID();
         break;
      case cmdGetNetworkLinkID:
         getNetworkLinkID();
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
NetworkTester::addCommandOptions()
{
   m_commandOptions.
      insert(make_pair("h",
                       CommandHolder("Displays this help menu.", help)));
    m_commandOptions.
      insert(make_pair("b", 
                       CommandHolder("Back", 
                                     back)));
    m_commandOptions.
       insert(make_pair("s",
                        CommandHolder("Set network link id",
                                      cmdSetNetworkLinkID)));

    m_commandOptions.
       insert(make_pair("g",
                        CommandHolder("Get network link id",
                                      cmdGetNetworkLinkID)));
}

void 
NetworkTester::testServerConnectionReply(RequestID requestID)
{

}

void
NetworkTester::getNetworkLinkID()
{
   NetworkLinkID networkLinkID;
   SynchronousStatus status =
      m_networkInterface.getNetworkLinkID(networkLinkID);
   if (status.getStatusCode() == OK) {
      cout << "-------------------------------------------" << endl;
      cout << "Got networkLinkID: " << endl;
      cout << "Id: " << networkLinkID.getID() << endl;
      cout << "Port: " << networkLinkID.getProxyPort() << endl;
      cout << "Host: " << networkLinkID.getProxyHost() << endl;
      cout << "-------------------------------------------" << endl;
   }
}

void
NetworkTester::setNetworkLinkID()
{
   NetworkLinkID networkLinkID(10, 1337, "din-mamma.wayfinder.com");
   
   SynchronousStatus status =
      m_networkInterface.setNetworkLinkID(networkLinkID);

   cout << "-------------------------------------------" << endl;
   cout << "NetworkTester::setNetworkLinkID returned status code: "
        << status.getStatusCode() << endl;
}
