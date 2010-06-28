/*
Copyright (c) 1999 - 2010, Vodafone Group Services Ltd
All rights reserved.

Redistribution and use in source and binary forms, with or without modification, are permitted provided that the following conditions are met:

    * Redistributions of source code must retain the above copyright notice, this list of conditions and the following disclaimer.
    * Redistributions in binary form must reproduce the above copyright notice, this list of conditions and the following disclaimer in the documentation and/or other materials provided with the distribution.
    * Neither the name of the Vodafone Group Services Ltd nor the names of its contributors may be used to endorse or promote products derived from this software without specific prior written permission.

THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS" AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE ARE DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT HOLDER OR CONTRIBUTORS BE LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
*/


#ifndef IAP_SEARCHER_TEST_H
#define IAP_SEARCHER_TEST_H

#include "NetworkListener.h"
#include "IAPObserver.h"
#include "RegressionTest.h"
#include "NetworkInterface.h"
//#include "RequestID.h"
#include "RegressionTest.h"

namespace WFAPI {
class NetworkLinkID;
class RequestID;
}

class Nav2Session;
class IAPSearcher;

class IapSearcherTest : public WFAPI::NetworkListener,
                        public IAPObserver,
                        public RegressionTest
{
public:

   /**
    * Constructor.
    *
    * @param session Reference to the Nav2Session.
    */
   IapSearcherTest(Nav2Session& session, const char* name);

   /**
    * Destructor.
    */
   virtual ~IapSearcherTest();

   /**
    * Starts the iap search test.
    */
   virtual void startTest();

   /**
    * This function gets called when getting a reply for the
    * test server connection call.
    *
    * @requestID The request id for the request.
    */
   virtual void testServerConnectionReply(WFAPI::RequestID requestID);

   /**
    * Callback functinon from Interface, called when an error occures
    * in the core. In this case for the server connection request.
    *
    * @param status The status object containing the error params.
    */
   virtual void error(const WFAPI::AsynchronousStatus& status);

public: // From IAPObserver

   /**
    * Called when no working iaps could be found.
    * 
    * @param error The error for this.
    * @param temporary This is true when having done the manual selection
    *                  dialog, not vaild for this test class.
    */
   virtual void ReportFailureIAP(TInt error, TBool temporary = EFalse);

   /**
    * Gets called by the iap searcher when we should test an iap or
    * when we found a working iap.
    *
    * @param aIAPid The id for the iap.
    * @param proxyHost The proxy host, if present.
    * @param proxyPort The proxy port, if present.
    * @param isReal True if we found a working iap.
    * @param aShow Not used.
    */
   virtual void SendIAP(int32 aIAPid,
                        const char* proxyHost = NULL,
                        uint16 proxyPort = 0,
                        TBool isReal = EFalse, TBool aShow = ETrue);

   /**
    * Sends a test server connection request to the core.
    */
   virtual void SendNop();

private:

   /// The networking interface which we can request a server connection test.
   WFAPI::NetworkInterface& m_networkInterface;

   /// The request id for the test server connection request, we will match
   /// this request id with the one we get in the callbacks from the core.
   WFAPI::RequestID m_requestID;

   /// The network link id containing the information about the apn to test.
   WFAPI::NetworkLinkID m_networkLinkID;

   /// The iap searcher, the class that does the actual search in the
   /// comms database.
   IAPSearcher* m_iapSearcher;
};

#endif // IAP_SEARCHER_TEST_H
