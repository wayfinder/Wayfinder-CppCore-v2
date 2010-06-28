/*
  Copyright (c) 1999 - 2010, Vodafone Group Services Ltd
  All rights reserved.

  Redistribution and use in source and binary forms, with or without modification, are permitted provided that the following conditions are met:

  * Redistributions of source code must retain the above copyright notice, this list of conditions and the following disclaimer.
  * Redistributions in binary form must reproduce the above copyright notice, this list of conditions and the following disclaimer in the documentation and/or other materials provided with the distribution.
  * Neither the name of the Vodafone Group Services Ltd nor the names of its contributors may be used to endorse or promote products derived from this software without specific prior written permission.

  THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS" AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE ARE DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT HOLDER OR CONTRIBUTORS BE LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
*/

#define CORE_LOGGING_MODULE_NAME "IPWFLOCATIONHANDLER"
#define CORE_LOGGING_ENABLE_MODULE

#include "IPWFLocationHandler.h"
#include "LocationInterface.h"
#include "LocationStatusCode.h"
#include "Nav2API.h"
#include "GpsQualityEnum.h"
#include "AsynchronousStatus.h"
#include "StatusCode.h"
#include "WGS84Coordinate.h"
#include <map>

#include "MapOperationInterface.h"
#include "MapDrawingInterface.h"
#include "MapObjectInfo.h"
#include "MapObjectInterface.h"


#define coreprintln printf


// helper function floatAlmostEqualULPs version - fast and simple, but
// some limitations. the maximum error is passwd in terms of "Units in the Last Place".
// This should probably not be used in the product delivered to customers.
bool floatAlmostEqualUlps(float A, float B) {
    #define maxUlps 0.00000001
    assert(sizeof(float) == sizeof(int));
    if (A == B)
        return true;
    int intDiff = abs(*(int*)&A - *(int*)&B);
    if (intDiff <= maxUlps)
        return true;
    return false;
}

using namespace std;

IPWFLocationHandler::IPWFLocationHandler(WFAPI::LocationInterface& locationInterface, WFAPI::MapLibAPI& mapLib)
: m_locationInterface(locationInterface),
   m_mapLib(mapLib)
{
   m_locationInterface.addLocationListener(this);
   
   WFAPI::AsynchronousStatus startLbsStatus = m_locationInterface.startLbs();
   if (startLbsStatus.getStatusCode() != WFAPI::OK) {
      coreprintln("startLbsStatus.getStatusCode() == %d",
                  startLbsStatus.getStatusCode());
   }
   WFAPI::MapOperationInterface* operationInterface =
      m_mapLib.getMapOperationInterface();

   operationInterface->setZoomLevel(1);
   m_lastGoodLat=180;
   m_lastGoodLon=180;
}

IPWFLocationHandler::~IPWFLocationHandler()
{
   m_locationInterface.stopLbs();
   m_locationInterface.removeLocationListener(this);
}


void
IPWFLocationHandler::error(const WFAPI::AsynchronousStatus& status)
{
   coreprintln("error()");
}

void
IPWFLocationHandler::locationUpdate(const WFAPI::LocationUpdateInformation& locationUpdate)
{
   // Print header
   coreprintln("*******************************************************\n");
   coreprintln("****************** Location Update ********************\n");
   coreprintln("*******************************************************\n");

   // Print data
   coreprintln("Position: %f,%f\n", locationUpdate.getPosition().latDeg, locationUpdate.getPosition().lonDeg);
   coreprintln("heading: %d\n", locationUpdate.getHeading());
   coreprintln("Speed: %d\n", locationUpdate.getSpeed());
   coreprintln("Altitude: %d\n", locationUpdate.getAltitude());
   coreprintln("Quality: %d\n", locationUpdate.getGpsQuality());

   //first check if the current position is invalid (no gps data available). if so, dont setCenter:
   if( ! floatAlmostEqualUlps(locationUpdate.getPosition().latDeg,180) && 
       ! floatAlmostEqualUlps(locationUpdate.getPosition().lonDeg,180) ) { 

      //then check if the current position is the same as our last good position. 
      // if so, dont setCenter (to let the developer 
      // scroll around in the map freely while not moving the device):
      if( ! floatAlmostEqualUlps(locationUpdate.getPosition().latDeg,m_lastGoodLat) && 
          ! floatAlmostEqualUlps(locationUpdate.getPosition().lonDeg,m_lastGoodLon) ) { 

            m_lastGoodLat=locationUpdate.getPosition().latDeg;
            m_lastGoodLon=locationUpdate.getPosition().lonDeg;

         WFAPI::MapOperationInterface* operationInterface =
            m_mapLib.getMapOperationInterface();
         operationInterface->setCenter(locationUpdate.getPosition());
         operationInterface->setAngle(locationUpdate.getHeading());
      }
   }
}

void
IPWFLocationHandler::areaUpdate(const WFAPI::AreaUpdateInformation& areaUpdate)
{
	// Print header
	coreprintln("*******************************************************\n");
	coreprintln("****************** Area Update ************************\n");
	coreprintln("*******************************************************\n");
	
	// Print data
	coreprintln("Position: %f,%f\n", areaUpdate.getPosition().latDeg, areaUpdate.getPosition().lonDeg);
	coreprintln("InnerRadius: %d\n", areaUpdate.getInnerRadius());
	coreprintln("OuterRadius: %d\n", areaUpdate.getOuterRadius());
	coreprintln("StartAngle: %d\n", areaUpdate.getStartAngle());
	coreprintln("EndAngle: %d\n", areaUpdate.getEndAngle());
	coreprintln("Altitude: %d\n", areaUpdate.getAltitude());
	
	
	//first check if the current position is invalid (no gps data available). if so, dont setCenter:
	if( ! floatAlmostEqualUlps(areaUpdate.getPosition().latDeg,180) && 
       ! floatAlmostEqualUlps(areaUpdate.getPosition().lonDeg,180) ) { 
		
		//then check if the current position is the same as our last good position. 
		// if so, dont setCenter (to let the developer 
		// scroll around in the map freely while not moving the device):
		if( ! floatAlmostEqualUlps(areaUpdate.getPosition().latDeg,m_lastGoodLat) && 
		   ! floatAlmostEqualUlps(areaUpdate.getPosition().lonDeg,m_lastGoodLon) ) { 
			
            m_lastGoodLat=areaUpdate.getPosition().latDeg;
            m_lastGoodLon=areaUpdate.getPosition().lonDeg;
			
			WFAPI::MapOperationInterface* operationInterface =
            m_mapLib.getMapOperationInterface();
			operationInterface->setCenter(areaUpdate.getPosition());
		}
	}
	
	
   return;
}
void
IPWFLocationHandler::startedLbs()
{
   // Print
   coreprintln("******************** LBS Started **********************");
}
