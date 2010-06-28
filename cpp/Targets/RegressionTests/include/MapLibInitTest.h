/*
Copyright (c) 1999 - 2010, Vodafone Group Services Ltd
All rights reserved.

Redistribution and use in source and binary forms, with or without modification, are permitted provided that the following conditions are met:

    * Redistributions of source code must retain the above copyright notice, this list of conditions and the following disclaimer.
    * Redistributions in binary form must reproduce the above copyright notice, this list of conditions and the following disclaimer in the documentation and/or other materials provided with the distribution.
    * Neither the name of the Vodafone Group Services Ltd nor the names of its contributors may be used to endorse or promote products derived from this software without specific prior written permission.

THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS" AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE ARE DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT HOLDER OR CONTRIBUTORS BE LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
*/


#ifndef _MAPLIBINITTEST_H_
#define _MAPLIBINITTEST_H_

#include "RegressionTest.h"
#include "Nav2StatusListener.h"
#include "CoreDebugPrint.h"
#include "Nav2Session.h"
#include "Nav2API.h"

namespace WFAPI {
   class MapLibAPI;
} // End of namespace WFAPI

class MapLibInitTest : public RegressionTest,
                       public WFAPI::Nav2StatusListener {

public:
   MapLibInitTest(Nav2Session& session, const char* name) :
      RegressionTest(name, 90000), m_session(session)
   {
      m_mapLib = NULL;
   }

   virtual ~MapLibInitTest() {
      m_session.resetStatusNotification(this);
   }
   
   virtual void startTest() {
      m_session.redirectStatusNotification(this);
      createMapLib();
   }

   virtual void startupComplete() {
      setTestFailed("startupComplete called inside MapLibInitTest - "
                    "should never be possible since we do not start Nav2 "
                    "from there.");
   }

   virtual void stopComplete() {
      setTestFailed("stopComplete called inside MapLibInitTest - "
                    "should never be possible since we do not stop Nav2 "
                    "from there.");
   }
   
   virtual void mapLibStartupComplete() {
      m_session.setMapLib(m_mapLib);
      setTestPassed();
   }

   virtual void error(const WFAPI::AsynchronousStatus& status);
private:
   void createMapLib();
   
   Nav2Session& m_session;

   WFAPI::MapLibAPI* m_mapLib;
};

#endif /* _MAPLIBINITTEST_H_ */
