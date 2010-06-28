/*
Copyright (c) 1999 - 2010, Vodafone Group Services Ltd
All rights reserved.

Redistribution and use in source and binary forms, with or without modification, are permitted provided that the following conditions are met:

    * Redistributions of source code must retain the above copyright notice, this list of conditions and the following disclaimer.
    * Redistributions in binary form must reproduce the above copyright notice, this list of conditions and the following disclaimer in the documentation and/or other materials provided with the distribution.
    * Neither the name of the Vodafone Group Services Ltd nor the names of its contributors may be used to endorse or promote products derived from this software without specific prior written permission.

THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS" AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE ARE DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT HOLDER OR CONTRIBUTORS BE LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
*/


#include "RouteSimulationTest.h"

#define CORE_LOGGING_MODULE_NAME "RouteSimulationTest"
//#define CORE_LOGGING_MODULE_LEVEL CL_ALL_FILE

#include "CoreDebugPrint.h"
#include "Nav2Session.h"
#include "Nav2API.h"
#include "RouteInterface.h"
#include "NavigationInterface.h"

using namespace WFAPI;

void RouteSimulationTest::startTest()
{
   m_routeInterface = &m_session.getNav2()->getRouteInterface();
   m_navigationInterface = &m_session.getNav2()->getNavigationInterface();

   m_routeInterface->addRouteListener(this);
   m_navigationInterface->addNavigationInfoUpdateListener(this);
   
   /// Reference to the RouteInterface
   WGS84Coordinate startCoord(55.5901, 13.0134);
   WGS84Coordinate destCoord(55.5916, 13.0183);

   AsynchronousStatus status =
      m_routeInterface->routeBetweenCoordinates(startCoord, destCoord, CAR);
}

void RouteSimulationTest::routeReply(RequestID requestID)
{
   if(!m_routeResponseHandled) {
      // We get this twice.
      m_routeResponseHandled = true;
      coreprintln("Received routeReply");
      m_routeInterface->routeSimulationStart();
      m_routeInterface->routeSimulationIncreaseSpeed();
      m_routeInterface->routeSimulationIncreaseSpeed();
      m_routeInterface->routeSimulationIncreaseSpeed();
      m_routeInterface->routeSimulationIncreaseSpeed();
      m_routeInterface->routeSimulationIncreaseSpeed();
      m_routeInterface->routeSimulationIncreaseSpeed();
      m_routeInterface->routeSimulationIncreaseSpeed();
      m_routeInterface->routeSimulationIncreaseSpeed();
      m_routeInterface->routeSimulationIncreaseSpeed();
      m_routeInterface->routeSimulationIncreaseSpeed();
      m_routeInterface->routeSimulationIncreaseSpeed();
      m_routeInterface->routeSimulationIncreaseSpeed();
   }
}

void RouteSimulationTest::reachedEndOfRouteReply(RequestID requestID)
{
   coreprintln("Reached end of route!");
   setTestPassed();
}

void RouteSimulationTest::error(const AsynchronousStatus& status)
{
   setTestFailed(status.getStatusMessage()); 
}

void RouteSimulationTest::distanceUpdate(const UpdateNavigationDistanceInfo& info)
{
   coreprintln("DistanceUpdate: %d", info.getDistanceToGoal());
}

void RouteSimulationTest::infoUpdate(const UpdateNavigationInfo& info)
{
   
}

void RouteSimulationTest::playSound()
{
   
}

void RouteSimulationTest::prepareSound(const WFStringArray& soundNames)
{
   // for(WFStringArray::iterator itr 
}

void RouteSimulationTest::playCurrentSounds()
{
    
}

RouteSimulationTest::~RouteSimulationTest()
{
   m_routeInterface->removeRouteListener(this);
   m_navigationInterface->removeNavigationInfoUpdateListener(this);
}

RouteSimulationTest::RouteSimulationTest(Nav2Session& session,
                                         const char* name) :
   RegressionTest(name, 90000),
   m_session(session),
   m_routeInterface(NULL),
   m_navigationInterface(NULL),
   m_routeResponseHandled(false)
{
}
