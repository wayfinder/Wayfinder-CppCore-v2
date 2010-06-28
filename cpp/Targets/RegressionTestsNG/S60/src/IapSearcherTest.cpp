/*
Copyright (c) 1999 - 2010, Vodafone Group Services Ltd
All rights reserved.

Redistribution and use in source and binary forms, with or without modification, are permitted provided that the following conditions are met:

    * Redistributions of source code must retain the above copyright notice, this list of conditions and the following disclaimer.
    * Redistributions in binary form must reproduce the above copyright notice, this list of conditions and the following disclaimer in the documentation and/or other materials provided with the distribution.
    * Neither the name of the Vodafone Group Services Ltd nor the names of its contributors may be used to endorse or promote products derived from this software without specific prior written permission.

THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS" AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE ARE DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT HOLDER OR CONTRIBUTORS BE LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
*/


#include "IapSearcherTest.h"

#include "NetworkLinkID.h"
#include "RequestID.h"
#ifdef SOFT_DEBUG_PRINTS
#include "CoreDebugPrint.h"
#endif
#include "Nav2Session.h"
#include "Nav2API.h"
#include "IAPSearcher.h"
#include "RegressionTestErrorPrinting.h"

//#define SOFT_DEBUG_PRINTS

using namespace WFAPI;

IapSearcherTest::IapSearcherTest(Nav2Session& session, const char* name)
   : RegressionTest(name, 1000*60*10),
     m_networkInterface(session.getNav2()->getNetworkInterface()),
     m_requestID(RequestID::INVALID_REQUEST_ID)
{
   m_iapSearcher = NULL;
}

IapSearcherTest::~IapSearcherTest()
{
    
}

void IapSearcherTest::startTest()
{
   m_iapSearcher = new IAPSearcher(this);
   m_networkInterface.addNetworkListener(this);

   // Start the iap search
   m_iapSearcher->StartSearch();
}

void IapSearcherTest::testServerConnectionReply(RequestID requestID)
{
#ifdef SOFT_DEBUG_PRINTS
   coreprintln("[%s]", __FUNCTION__);
#endif
   if (requestID == m_requestID) {
      // The tested network id worked, we got a reply for our
      // nop request.
      m_iapSearcher->Reply();      
   }
}

void IapSearcherTest::error(const WFAPI::AsynchronousStatus& status)
{
   dumpError(status);
   
#ifdef SOFT_DEBUG_PRINTS
   coreprintln("[%s]", __FUNCTION__);
#endif
   if (m_requestID == status.getRequestID()) {
#ifdef SOFT_DEBUG_PRINTS
      coreprintln("[%s], status code = %d", __FUNCTION__,
                  status.getStatusCode());
#endif
                  

      // Report error to the iap searcher
      m_iapSearcher->ReplyFailed();  
   }
}

void IapSearcherTest::ReportFailureIAP(TInt error, TBool temporary )
{
   setTestFailed("No working APN:s");
}

void IapSearcherTest::SendIAP(int32 aIAPid,
                              const char* proxyHost,
                              uint16 proxyPort,
                              TBool isReal, TBool aShow )
{
#ifdef SOFT_DEBUG_PRINTS
   coreprintln("[%s]", __FUNCTION__);
#endif
   m_networkLinkID.setID(aIAPid);
   if (proxyHost != NULL && proxyPort != 0) {
      m_networkLinkID.setProxyPort(proxyPort);
      m_networkLinkID.setProxyHost(proxyHost);
   }

   if (!isReal) {
      // Not working, this call will generate a call to SendNop.
      m_iapSearcher->iapid2Set(aIAPid);
   } else {
      // It is actually working.
      // Continue with rest of startup.
      setTestPassed();
   }    
}

void IapSearcherTest::SendNop()
{
#ifdef SOFT_DEBUG_PRINTS
   coreprintln("[%s]", __FUNCTION__);
#endif
   m_networkInterface.setNetworkLinkID(m_networkLinkID);
   AsynchronousStatus status =  m_networkInterface.testServerConnection();
   if (status.getStatusCode() != OK) {
      // No good!
      // Report error to the iap searcher
      m_iapSearcher->ReplyFailed();
   } else {
      // Store the request id for this request
      m_requestID = status.getRequestID();
   }
}
