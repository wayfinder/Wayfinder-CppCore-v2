/*
Copyright (c) 1999 - 2010, Vodafone Group Services Ltd
All rights reserved.

Redistribution and use in source and binary forms, with or without modification, are permitted provided that the following conditions are met:

    * Redistributions of source code must retain the above copyright notice, this list of conditions and the following disclaimer.
    * Redistributions in binary form must reproduce the above copyright notice, this list of conditions and the following disclaimer in the documentation and/or other materials provided with the distribution.
    * Neither the name of the Vodafone Group Services Ltd nor the names of its contributors may be used to endorse or promote products derived from this software without specific prior written permission.

THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS" AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE ARE DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT HOLDER OR CONTRIBUTORS BE LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
*/


#ifndef _NAV2INITTEST_H_
#define _NAV2INITTEST_H_

#include "StartupData.h"
#include "RegressionTest.h"
#include "Nav2Session.h"
#include "VoiceLanguage.h"
#include "TextLanguage.h"
#include "Nav2StatusListener.h"
#include "Nav2API.h"
#include "RegressionTestConnectionManager.h"
#include "ImageDimension.h"
#include "ImageExtension.h"

class Nav2InitTest : public RegressionTest,
                     public WFAPI::Nav2StatusListener {
public:
   Nav2InitTest(Nav2Session& session, const char* name) :
      RegressionTest(name, 60000 ),
      m_session(session)
   {

   }

   virtual ~Nav2InitTest() {
      // We give the session back the control
      m_session.resetStatusNotification(this);
   }
   
   void error(const WFAPI::AsynchronousStatus& s);

   virtual void startTest() {
      
      WFAPI::StartupData startupData(m_session.getParamSeedPath(),
                                     m_session.getResourceStoragePath(),
                                     WFAPI::ENGLISH,
                                     WFAPI::VoiceLanguage::ENGLISH,
                                     m_session.getImageStoragePath(),
                                     m_session.getCategoryImageStoragePath(),
                                     m_session.getCategoryTreeImageStoragePath(),
                                     m_session.getParameterStoragePath());
      startupData.setImageDimension(WFAPI::DIMENSION_50X50);
      startupData.setImageExtension(WFAPI::PNG);
      startupData.setConnectionManager(new RegressionTestConnectionManager);
      m_session.getNav2()->start(this, &startupData);
   }

   virtual void startupComplete() {
      setTestPassed();
   }

   virtual void mapLibStartupComplete() {
      setTestFailed("mapLibStartupComplete called inside Nav2InitTest - "
                    "should never be possible since we do not start MapLib "
                    "from there.");
   }

   virtual void stopComplete() {
      setTestFailed("stopComplete called inside Nav2InitTest - "
                    "should never be possible since we do not stop Nav2 "
                    "from there.");
   }
private:
   Nav2Session& m_session;
};

#endif /* _NAV2INITTEST_H_ */
