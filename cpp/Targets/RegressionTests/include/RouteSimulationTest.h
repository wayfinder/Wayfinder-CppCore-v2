/*
Copyright (c) 1999 - 2010, Vodafone Group Services Ltd
All rights reserved.

Redistribution and use in source and binary forms, with or without modification, are permitted provided that the following conditions are met:

    * Redistributions of source code must retain the above copyright notice, this list of conditions and the following disclaimer.
    * Redistributions in binary form must reproduce the above copyright notice, this list of conditions and the following disclaimer in the documentation and/or other materials provided with the distribution.
    * Neither the name of the Vodafone Group Services Ltd nor the names of its contributors may be used to endorse or promote products derived from this software without specific prior written permission.

THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS" AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE ARE DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT HOLDER OR CONTRIBUTORS BE LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
*/


#ifndef ROUTESIMULATIONTEST_H
#define ROUTESIMULATIONTEST_H

#include "RouteListener.h"
#include "NavigationInfoUpdateListener.h"
#include "RequestID.h"
#include "RegressionTest.h"

namespace WFAPI {
class RouteInterface;
class NavigationInterface;
}

class RouteSimulationTest : public RegressionTest,
                            public WFAPI::NavigationInfoUpdateListener,
                            public WFAPI::RouteListener
{
public: // From base classes
   RouteSimulationTest(Nav2Session& session, const char* name);

   virtual ~RouteSimulationTest();
   
   virtual void startTest();

   /**
    * From RouteListener, called when a route has been calculated.
    *
    * @param requestID The request id for the route request, will be matched
    *                  against the request ids we have stored in our vector.
    */
   void routeReply(WFAPI::RequestID requestID);

   /**
    * From RouteListener, called when the user has reached the end of a route.
    *
    * @param requestID The request id for the route request.
    */
   void reachedEndOfRouteReply(WFAPI::RequestID requestID);

   /**
    * Called when an error has occured.
    */
   virtual void error(const WFAPI::AsynchronousStatus& status);

   /**
    * From NavigationListener, update function called during a route.
    *
    * @param info Info containing information about the route.
    */
   virtual void distanceUpdate(const WFAPI::UpdateNavigationDistanceInfo& info);

   /**
    * From NavigationListener, update function called during a route.
    *
    * @param info Info containing information about the route.
    */
   virtual void infoUpdate(const WFAPI::UpdateNavigationInfo& info);

   /**
    * From NavigationListener, called when a sound should be played.
    *
    * @param info Info containing information about the route.
    */
   virtual void playSound();

   /**
    * From NavigationListener, called before playSound so that we can
    * load sounds etc.
    *
    * @param info Info containing information about the route.
    */
   virtual void prepareSound(const WFAPI::WFStringArray& soundNames);

   /**
    * Play the current sounds.
    */
   void playCurrentSounds();
private:
   Nav2Session& m_session;

   /// Reference to the RouteInterface
   WFAPI::RouteInterface* m_routeInterface;
   WFAPI::NavigationInterface* m_navigationInterface;

   bool m_routeResponseHandled;
};

#endif /* ROUTESIMULATIONTEST_H */
