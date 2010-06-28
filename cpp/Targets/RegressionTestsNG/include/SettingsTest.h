/*
Copyright (c) 1999 - 2010, Vodafone Group Services Ltd
All rights reserved.

Redistribution and use in source and binary forms, with or without modification, are permitted provided that the following conditions are met:

    * Redistributions of source code must retain the above copyright notice, this list of conditions and the following disclaimer.
    * Redistributions in binary form must reproduce the above copyright notice, this list of conditions and the following disclaimer in the documentation and/or other materials provided with the distribution.
    * Neither the name of the Vodafone Group Services Ltd nor the names of its contributors may be used to endorse or promote products derived from this software without specific prior written permission.

THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS" AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE ARE DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT HOLDER OR CONTRIBUTORS BE LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
*/


#ifndef _SETTINGSTEST_H_
#define _SETTINGSTEST_H_

#include "SettingsListener.h"
#include "SettingsInterface.h"
#include "RegressionTest.h"
#include "Nav2Session.h"
#include "WFString.h"
#include "Nav2API.h"

class SettingsTest : public RegressionTest, public WFAPI::SettingsListener {
public:
   SettingsTest(Nav2Session& session, const char* name) :
      RegressionTest(name),
      m_settingsInterface(session.getNav2()->getSettingsInterface())
   {

   }

   virtual ~SettingsTest() {
      m_settingsInterface.removeSettingsListener(this);
   }
   
   virtual void startTest() {
      // m_settingsInterface.isEULAUpdated("eula_test.txt");

      m_settingsInterface.addSettingsListener(this);
      
      bool autoReroute = false;
      bool avoidHighway = false;
      bool avoidTollRoad = false;      
      bool checkRouteForTrafficChanges = false;
      bool usePos = false;
      
      WFAPI::DistanceUnit distanceUnit;
      WFAPI::RouteCost routeCost;
      WFAPI::VoiceVerbosity voiceVerbosity;
      WFAPI::wf_uint32 trafficInformationUpdateTime = 0;
      WFAPI::TransportationType transportationType;
      
      WFAPI::SynchronousStatus res = m_settingsInterface.getAutoReroute(autoReroute);

      if(res.getStatusCode() != WFAPI::OK) {
         setTestFailed(res.getStatusMessage());
         return;
      }
      
      res = m_settingsInterface.getAvoidHighway(avoidHighway);
      
      if(res.getStatusCode() != WFAPI::OK) {
         setTestFailed(res.getStatusMessage());
         return;
      }      

      res = m_settingsInterface.getAvoidTollRoad(avoidTollRoad);
      
      if(res.getStatusCode() != WFAPI::OK) {
         setTestFailed(res.getStatusMessage());
         return;
      }      

      res = m_settingsInterface.getCheckRouteForTrafficChanges(
         checkRouteForTrafficChanges);
      
      if(res.getStatusCode() != WFAPI::OK) {
         setTestFailed(res.getStatusMessage());
         return;
      }

      res = m_settingsInterface.getDistanceUnit(distanceUnit);
      
      if(res.getStatusCode() != WFAPI::OK) {
         setTestFailed(res.getStatusMessage());
         return;
      }

      res = m_settingsInterface.getRouteCost(routeCost);
      
      if(res.getStatusCode() != WFAPI::OK) {
         setTestFailed(res.getStatusMessage());
         return;
      }

      res = m_settingsInterface.getVoiceVerbosity(voiceVerbosity);
      
      if(res.getStatusCode() != WFAPI::OK) {
         setTestFailed(res.getStatusMessage());
         return;
      }

      res = m_settingsInterface.getTrafficInformationUpdateTime(
         trafficInformationUpdateTime);
      
      if(res.getStatusCode() != WFAPI::OK) {
         setTestFailed(res.getStatusMessage());
         return;
      }

      res = m_settingsInterface.getCurrentTransportationType(transportationType);

      if(res.getStatusCode() != WFAPI::OK) {
         setTestFailed(res.getStatusMessage());
         return;
      }

      res = m_settingsInterface.getUsePositionForSearchCategoriesRequests(usePos);
      
      if(res.getStatusCode() != WFAPI::OK) {
         setTestFailed(res.getStatusMessage());
         return;
      }

      setTestPassed();
   }

   virtual void error(const WFAPI::AsynchronousStatus& status);

   virtual void eulaAvailable(bool updated, WFAPI::WFString filename) {
      //std::cout << "eulaAvailable" << std::endl;
      setTestPassed();
   }

   virtual void newVersionAvailable(const WFAPI::WFString& versionNumber,
                                    const WFAPI::WFString& versionUrl,
                                    const WFAPI::WFString& versionString,
                                    bool force) {
      
   }
   
private:
   WFAPI::SettingsInterface& m_settingsInterface;
};

#endif /* _SETTINGSTEST_H_ */
