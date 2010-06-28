/*
Copyright (c) 1999 - 2010, Vodafone Group Services Ltd
All rights reserved.

Redistribution and use in source and binary forms, with or without modification, are permitted provided that the following conditions are met:

    * Redistributions of source code must retain the above copyright notice, this list of conditions and the following disclaimer.
    * Redistributions in binary form must reproduce the above copyright notice, this list of conditions and the following disclaimer in the documentation and/or other materials provided with the distribution.
    * Neither the name of the Vodafone Group Services Ltd nor the names of its contributors may be used to endorse or promote products derived from this software without specific prior written permission.

THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS" AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE ARE DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT HOLDER OR CONTRIBUTORS BE LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
*/


#define CORE_LOGGING_MODULE_NAME "LOCATIONHARDWARETEST"
//#define CORE_LOGGING_MODULE_LEVEL CL_ALL_FILE

#include "CoreDebugPrint.h"
#include "LocationHardwareTest.h"
#include "LocationInterface.h"
#include "LocationStatusCode.h"
#include "Nav2Session.h"
#include "Nav2API.h"
#include "GpsQualityEnum.h"
#include "AsynchronousStatus.h"
#include "StatusCode.h"
#include "CoreDebugPrint.h"
#include <map>
#include "RegressionTestErrorPrinting.h"

using namespace std;

LocationHardwareTest::LocationHardwareTest(Nav2Session& session,
                                           const char* name)  :
   RegressionTest(name, 10000),
   m_locationInterface(session.getNav2()->getLocationInterface())
{
   m_state = LOC_UNINITIALIZED;
   m_errorString = NULL;
   m_goodGpsPositions = 0;
}

LocationHardwareTest::~LocationHardwareTest()
{
   m_locationInterface.stopLbs();
   m_locationInterface.removeLocationListener(this);
}

void LocationHardwareTest::startTest()
{
#ifdef SOFT_DEBUG_PRINTS
   coreprintln("LocationHardwareTest::startTest()");
#endif
   m_locationInterface.addLocationListener(this);

   WFAPI::AsynchronousStatus startLbsStatus = m_locationInterface.startLbs();
   if (startLbsStatus.getStatusCode() != WFAPI::OK) {
      coreprintln("startLbsStatus.getStatusCode() == %d",
                  startLbsStatus.getStatusCode());
      setTestFailed(startLbsStatus.getStatusMessage());
   } else {
      m_state = LOC_RUNNING;
   }
}

bool LocationHardwareTest::finished()
{
   return m_state == LOC_DONE || m_state == LOC_ERROR; 
}

void
LocationHardwareTest::error(const WFAPI::AsynchronousStatus& status)
{
   dumpError(status);
#ifdef SOFT_DEBUG_PRINTS
   coreprintln("SearchOverflowTest::error()");
#endif
   setTestFailed(status.getStatusMessage());
   m_state = LOC_ERROR;
}

void
LocationHardwareTest::locationUpdate(const WFAPI::LocationUpdateInformation& locationUpdate)
{
   // Print header
   coreprintln("*******************************************************");
   coreprintln("****************** Location Update ********************");
   coreprintln("*******************************************************");

   // Print data
   coreprintln("Position: %f,%f", locationUpdate.getPosition().latDeg, locationUpdate.getPosition().lonDeg);
   coreprintln("heading: %d", locationUpdate.getHeading());
   coreprintln("Speed: %d", locationUpdate.getSpeed());
   coreprintln("Altitude: %d", locationUpdate.getAltitude());
   coreprintln("Quality: %d", locationUpdate.getGpsQuality());

   //changed my mind about this strategy below. Getting 180,180
   //locationUpdate's 5 times is not good enough, since that's nav2 doing that
   //as soon as we start that subsystem. It says nothing about the gps
   //hardware. Instead, we now trust the LocationInterfaceImpl to correctly
   //implement checks that verify that the lbs IS actually started when it says
   //it is.
   
   
   // // getting location updates is enough to consider the gps hardware to
   // // exist, since QUALITY_SEARCHING can't be trusted to be reported
   // // correctly right now.
   // m_goodGpsPositions++;

   // if(m_goodGpsPositions > 4) {
   //    // five good location updates is considered enough for test to succeed.
   //    m_state = LOC_DONE;
   //    setTestPassed();
   //    coreprintln("Test passed!");
   // }

}

void
LocationHardwareTest::areaUpdate(const WFAPI::AreaUpdateInformation& areaUpdate)
{
   return;
}
void
LocationHardwareTest::startedLbs()
{
   // Print
   coreprintln("******************** LBS Started **********************");
   // the startedLbs callback is considered enough for test to succeed.
   m_state = LOC_DONE;
   setTestPassed();
   coreprintln("Test passed!");

}

void
LocationHardwareTest::printLbsStarted()
{
   WFAPI::SynchronousStatus status = m_locationInterface.lbsStarted();
   
   if (status.getStatusCode() == WFAPI::LBS_STATUS_STARTED) {
      coreprintln("LBS is started.");
   } else if (status.getStatusCode() == WFAPI::LBS_STATUS_NOT_STARTED) {
      coreprintln("LBS is NOT started.");
   } else {
      // This should not happen.
      coreprintln("LBS Error: code %d message %s", status.getStatusCode(),
                  status.getStatusMessage().c_str());
   }
}

