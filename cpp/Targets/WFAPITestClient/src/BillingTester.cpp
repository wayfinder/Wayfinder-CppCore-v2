/*
Copyright (c) 1999 - 2010, Vodafone Group Services Ltd
All rights reserved.

Redistribution and use in source and binary forms, with or without modification, are permitted provided that the following conditions are met:

    * Redistributions of source code must retain the above copyright notice, this list of conditions and the following disclaimer.
    * Redistributions in binary form must reproduce the above copyright notice, this list of conditions and the following disclaimer in the documentation and/or other materials provided with the distribution.
    * Neither the name of the Vodafone Group Services Ltd nor the names of its contributors may be used to endorse or promote products derived from this software without specific prior written permission.

THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS" AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE ARE DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT HOLDER OR CONTRIBUTORS BE LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
*/


#include "BillingTester.h"
#include "BillingInterface.h"
#include "WFString.h"
#include <iostream>
#include <stdlib.h>

using namespace std;
using namespace WFAPI;

BillingTester::BillingTester(BillingInterface& billingInterface)
   : m_billingInterface(billingInterface)
{
   m_billingInterface.addBillingListener(this);
   addCommandOptions();
}
                   
BillingTester::~BillingTester()
{

}
   
void
BillingTester::verifyThirdPartyTransaction()
{
   cout << "Enter transaction id: ";
   string transactionID = "";
   cin.clear();
   getline(cin, transactionID);

   cout << "Enter selction string: ";
   string selectionString;
   cin.clear();
   getline(cin, selectionString);
   
   m_billingInterface.verifyThirdPartyTransaction(WFString(transactionID.c_str()),
                                                  WFString(selectionString.c_str()));
}

void
BillingTester::error(const AsynchronousStatus& status)
{
   printErrorStatus(&status, "BillingTester");
}

void 
BillingTester::thirdPartyTransactionVerified(RequestID requestID)
{
   cout << "BillingTester::thirdPartyTransactionVerified OK" << endl;
}

bool 
BillingTester::handleInput(const char* input, bool& goBack)
{
   StringMap::const_iterator it = m_commandOptions.find(input);

   if (it == m_commandOptions.end()) {
      return false;
   }
   switch((*it).second.m_commandType) {
      case help:  
         printHelp();
         break;
      case cmdVerifyThirdPartyTransaction:
         verifyThirdPartyTransaction();
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
BillingTester::addCommandOptions()
{
   m_commandOptions.
      insert(make_pair("h",
                       CommandHolder("Displays this help menu.", help)));
   m_commandOptions.
      insert(make_pair("v", 
                       CommandHolder("Verify a third party transaction with the server", 
                                     cmdVerifyThirdPartyTransaction)));
   m_commandOptions.
      insert(make_pair("b", 
                       CommandHolder("Back", 
                                     back)));
}


