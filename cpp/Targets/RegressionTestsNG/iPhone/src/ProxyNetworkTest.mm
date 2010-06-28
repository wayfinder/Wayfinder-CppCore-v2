/*
  Copyright (c) 1999 - 2010, Vodafone Group Services Ltd
  All rights reserved.

  Redistribution and use in source and binary forms, with or without modification, are permitted provided that the following conditions are met:

  * Redistributions of source code must retain the above copyright notice, this list of conditions and the following disclaimer.
  * Redistributions in binary form must reproduce the above copyright notice, this list of conditions and the following disclaimer in the documentation and/or other materials provided with the distribution.
  * Neither the name of the Vodafone Group Services Ltd nor the names of its contributors may be used to endorse or promote products derived from this software without specific prior written permission.

  THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS" AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE ARE DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT HOLDER OR CONTRIBUTORS BE LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
*/

#include "ProxyNetworkTest.h"
#include "Nav2Session.h"
#include "Nav2API.h"
#include "CFProxySupport.h"
#include "NetworkInterface.h"
#include "CoreDebugPrint.h"
#include "StatusCode.h"

#import <CoreFoundation/CoreFoundation.h>
#import <Foundation/Foundation.h>

ProxyNetworkTest::ProxyNetworkTest(Nav2Session& session, const char* name)
: RegressionTest(name, 3000),
   m_networkInterface(session.getNav2()->getNetworkInterface())
{
   
}

ProxyNetworkTest::~ProxyNetworkTest()
{
   m_networkInterface.removeNetworkListener(this);
}

void
ProxyNetworkTest::startTest()
{
   CFStringRef googleurl =
      (CFStringRef) [NSString stringWithCString:"www.google.com"];
   CFURLRef url = CFURLCreateWithString(NULL, googleurl, NULL);
   CFArrayRef proxies = CFNetworkCopyProxiesForURL(url, NULL);
   CFIndex nbr = CFArrayGetCount(proxies);
   
   NSMutableDictionary *dictionary;
   if (nbr != 1){
      setTestFailed("Proxy query returned several choices, currently not handled in ProxyNetworkTest");
   }

   WFAPI::SynchronousStatus res = m_networkInterface.clearNetworkLinkID();
   if (res.getStatusCode() != WFAPI::OK){
      setTestFailed("Failed to clear network link ID");
   }

   WFAPI::NetworkLinkID network;
   res = m_networkInterface.setNetworkLinkID(network);
   if (res.getStatusCode() != WFAPI::OK){
      setTestFailed("Failed to set network link ID");
   }
   
   m_networkInterface.addNetworkListener(this);
   m_networkInterface.testServerConnection();
}

void
ProxyNetworkTest::testServerConnectionReply(WFAPI::RequestID requestId)
{
   setTestPassed();
}

void
ProxyNetworkTest::error(const WFAPI::AsynchronousStatus& status)
{
   setTestFailed(status.getStatusMessage());
}

