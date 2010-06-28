/*
Copyright (c) 1999 - 2010, Vodafone Group Services Ltd
All rights reserved.

Redistribution and use in source and binary forms, with or without modification, are permitted provided that the following conditions are met:

    * Redistributions of source code must retain the above copyright notice, this list of conditions and the following disclaimer.
    * Redistributions in binary form must reproduce the above copyright notice, this list of conditions and the following disclaimer in the documentation and/or other materials provided with the distribution.
    * Neither the name of the Vodafone Group Services Ltd nor the names of its contributors may be used to endorse or promote products derived from this software without specific prior written permission.

THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS" AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE ARE DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT HOLDER OR CONTRIBUTORS BE LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
*/


#include "PositionSimulationTest.h"
#include "Nav2Session.h"
#include "LocationInterface.h"
#include "Nav2API.h"

#define CORE_LOGGING_MODULE_NAME "PositionSimulationTest"
//#define CORE_LOGGING_MODULE_LEVEL CL_ALL_FILE
#include "CoreDebugPrint.h"

using namespace WFAPI;

PositionSimulationTest::PositionSimulationTest(Nav2Session& session,
                                               const char* name)
   : RegressionTest(name, 900000),
     m_session(session),
     m_locInterface(NULL)
{
   
}

void PositionSimulationTest::locationUpdate(
   const LocationUpdateInformation& locationUpdate)
{
   WGS84Coordinate coord(locationUpdate.getRoutePosition());
   
   coreprintln("locationUpdate(%f, %f)", coord.latDeg, coord.lonDeg);
}

void PositionSimulationTest::areaUpdate(
   const AreaUpdateInformation& areaUpdateInformation)
{
    
}

void PositionSimulationTest::startedLbs()
{
    
}

void PositionSimulationTest::startTest()
{
   m_locInterface = &m_session.getNav2()->getLocationInterface();
   m_locInterface->addLocationListener(this);
   m_locInterface->startLbs();
   m_locInterface->replayNMEAData(
      "/scratchbox/users/root/home/root/nmea_logs/lbs_nmea_data_054.log");
}

PositionSimulationTest::~PositionSimulationTest()
{
   m_locInterface->stopLbs();
   m_locInterface->removeLocationListener(this);  
}

void PositionSimulationTest::error(const WFAPI::AsynchronousStatus& status)
{
   setTestFailed(status.getStatusMessage()); 
}

