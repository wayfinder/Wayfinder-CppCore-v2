/*
Copyright (c) 1999 - 2010, Vodafone Group Services Ltd
All rights reserved.

Redistribution and use in source and binary forms, with or without modification, are permitted provided that the following conditions are met:

    * Redistributions of source code must retain the above copyright notice, this list of conditions and the following disclaimer.
    * Redistributions in binary form must reproduce the above copyright notice, this list of conditions and the following disclaimer in the documentation and/or other materials provided with the distribution.
    * Neither the name of the Vodafone Group Services Ltd nor the names of its contributors may be used to endorse or promote products derived from this software without specific prior written permission.

THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS" AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE ARE DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT HOLDER OR CONTRIBUTORS BE LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
*/


#ifndef _NETWORKCONNECTIONSTRESSTEST_H_
#define _NETWORKCONNECTIONSTRESSTEST_H_

#include "NetworkListener.h"
#include "NetworkInterface.h"
#include "RegressionTest.h"
#include "Nav2Session.h"
#include "Nav2API.h"
#include "PALMachineTypes.h"
#include "RequestID.h"

#include <vector>

class NetworkConnectionStressTest : public RegressionTest,
                              public WFAPI::NetworkListener {
public:
   NetworkConnectionStressTest(Nav2Session& session, const char* name) :
      RegressionTest(name, 50000),
      m_netInterface(session.getNav2()->getNetworkInterface())
   {
      
   }

   virtual ~NetworkConnectionStressTest() {
      m_netInterface.removeNetworkListener(this);
   }

   virtual void startTest();
   virtual void testServerConnectionReply(WFAPI::RequestID requestId);


   virtual void error(const WFAPI::AsynchronousStatus& status);
   
private:
   WFAPI::NetworkInterface& m_netInterface;

   typedef std::vector<WFAPI::RequestID> t_reqIdVec;
   t_reqIdVec m_requestList;
};

#endif /* _NETWORKCONNECTIONTEST_H_ */
