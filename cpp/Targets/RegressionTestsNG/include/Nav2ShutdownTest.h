/*
Copyright (c) 1999 - 2010, Vodafone Group Services Ltd
All rights reserved.

Redistribution and use in source and binary forms, with or without modification, are permitted provided that the following conditions are met:

    * Redistributions of source code must retain the above copyright notice, this list of conditions and the following disclaimer.
    * Redistributions in binary form must reproduce the above copyright notice, this list of conditions and the following disclaimer in the documentation and/or other materials provided with the distribution.
    * Neither the name of the Vodafone Group Services Ltd nor the names of its contributors may be used to endorse or promote products derived from this software without specific prior written permission.

THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS" AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE ARE DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT HOLDER OR CONTRIBUTORS BE LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
*/


#ifndef _NAV2SHUTDOWNTEST_H_
#define _NAV2SHUTDOWNTEST_H_

#include "Nav2Session.h"
#include "RegressionTest.h"
#include "Nav2API.h"

class Nav2ShutdownTest : public RegressionTest,
                         public WFAPI::Nav2StatusListener {
public:
   Nav2ShutdownTest(Nav2Session& session, const char* name) :
      RegressionTest(name),
      m_session(session)
   {
      
   }

   virtual ~Nav2ShutdownTest() {
      // We give the session back the control
      m_session.resetStatusNotification(this);
   }
   
   virtual void startTest() {
      m_session.redirectStatusNotification(this);
      m_session.getNav2()->stop();
   }

   virtual void stopComplete() {
      setTestPassed();

   }
   
   virtual void mapLibStartupComplete() {
      setTestFailed("mapLibStartupComplete called inside Nav2ShutdownTest - "
                    "should never be possible since we do not start MapLib "
                    "from there.");
   }

   virtual void startupComplete() {
      setTestFailed("startupComplete called inside Nav2InitTest - "
                    "should never be possible since we do not start Nav2 "
                    "from there.");
   }

   virtual void error(const WFAPI::AsynchronousStatus& status);
   
private:
   Nav2Session& m_session;
};

#endif /* _NAV2SHUTDOWNTEST_H_ */
