/*
Copyright (c) 1999 - 2010, Vodafone Group Services Ltd
All rights reserved.

Redistribution and use in source and binary forms, with or without modification, are permitted provided that the following conditions are met:

    * Redistributions of source code must retain the above copyright notice, this list of conditions and the following disclaimer.
    * Redistributions in binary form must reproduce the above copyright notice, this list of conditions and the following disclaimer in the documentation and/or other materials provided with the distribution.
    * Neither the name of the Vodafone Group Services Ltd nor the names of its contributors may be used to endorse or promote products derived from this software without specific prior written permission.

THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS" AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE ARE DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT HOLDER OR CONTRIBUTORS BE LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
*/


#include "LocationTester.h"
#include "LocationInterface.h"
#include "LocationStatusCode.h"
#include "GeocodingListener.h"
#include <map>

// #define PRINT_VERBOSE

using namespace std;

LocationTester::LocationTester(LocationInterface& locationInterface)
      : m_locationInterface(locationInterface)
{
   m_locationInterface.addLocationListener(this);
   m_locationInterface.addGeocodingListener(this);
   // Add command for this.
   m_commandOptions.
      insert(make_pair("h",
                       CommandHolder("Displays this help menu.", help)));
   m_commandOptions.
      insert(make_pair("s", 
                       CommandHolder("Start LBS", 
                                     cmdStartLBS)));
   m_commandOptions.
      insert(make_pair("t", 
                       CommandHolder("Stop LBS", 
                                     cmdStopLBS)));
   m_commandOptions.
      insert(make_pair("c", 
                       CommandHolder("Print current LBS status", 
                                     cmdIslbsStarted)));
   m_commandOptions.
      insert(make_pair("r", 
                       CommandHolder("Request reverse geocoding",
                                     cmdRevGeocoding)));

   m_commandOptions.
      insert(make_pair("b", 
                       CommandHolder("Back", 
                                     back)));   
}

LocationTester::~LocationTester()
{
   m_locationInterface.removeLocationListener(this);
   m_locationInterface.removeGeocodingListener(this);
}

void 
LocationTester::addLocationListener(LocationListener* listener)
{
   m_locationInterface.addLocationListener(listener);
}

void
LocationTester::removeLocationListener(LocationListener* listener)
{
   m_locationInterface.removeLocationListener(listener);
}

void
LocationTester::error(const AsynchronousStatus& status)
{
   printErrorStatus(&status, "LocationTester");
}

void
LocationTester::areaUpdate(const AreaUpdateInformation& areaUpdateInformation)
{
#ifdef PRINT_VERBOSE
   // Print header
   cout << "*******************************************************" << endl;
   cout << "******************** Area Update **********************" << endl;
   cout << "*******************************************************" << endl;

   // Print data
   cout << "Position: " << areaUpdateInformation.getPosition() << endl;
   cout << "InnerRadius: " << areaUpdateInformation.getInnerRadius() << endl;
   cout << "OuterRadius: " << areaUpdateInformation.getOuterRadius() << endl;
   cout << "StartAngle: " << areaUpdateInformation.getStartAngle() << endl;
   cout << "EndAngle: " << areaUpdateInformation.getEndAngle() << endl;
   cout << "Altitude: " << areaUpdateInformation.getAltitude() << endl;
#else
   cout << "Area-update: " << areaUpdateInformation.getPosition()
        << " inner/outer radius: " << areaUpdateInformation.getInnerRadius()
        << " / " << areaUpdateInformation.getOuterRadius() << endl;
#endif   
}

void
LocationTester::locationUpdate(const LocationUpdateInformation& locationUpdate)
{
#ifdef PRINT_VERBOSE
   // Print header
   cout << "*******************************************************" << endl;
   cout << "****************** Location Update ********************" << endl;
   cout << "*******************************************************" << endl;

   // Print data
   cout << "Position: " << locationUpdate.getPosition() << endl;
   cout << "heading: " << locationUpdate.getHeading() << endl;
   cout << "Speed: " << locationUpdate.getSpeed() << endl;
   cout << "Altitude: " << locationUpdate.getAltitude() << endl;
   cout << "Quality: " << locationUpdate.getGpsQuality() << endl;
#else
   cout << "Location-update: " << locationUpdate.getPosition()
        << " @ " << locationUpdate.getSpeed() << " km/h @ "
        << locationUpdate.getHeading() << " deg" << endl;
#endif
}

void
LocationTester::startedLbs()
{
   // Print
   cout << "******************** LBS Started **********************" << endl;
}

bool
LocationTester::handleInput(const char* input, bool& goBack)
{
   // Hmm, could use functionpointer/object in m_commandOptions and 
   // implement this once and for all in super class.
   StringMap::const_iterator it = m_commandOptions.find(input);

   if (it == m_commandOptions.end()) {
      return false;
   }
   switch((*it).second.m_commandType) {
      case help:  
         printHelp();
         break;
      case cmdStartLBS:
         cout << "starting LBS." << endl;
         m_locationInterface.startLbs();
         break;
      case cmdStopLBS:
         m_locationInterface.stopLbs();
         break;
      case cmdIslbsStarted:
         printLbsStarted();
         break;
      case back:
         goBack = true;
         break;
      case cmdRevGeocoding:
         requestReverseGeocoding();
         break;
      default:
         cout << "Command not handled here, please try again or select "
              << "B to go back." << endl;
         return false;
         break;
   }

   // If we get here, the key has been consumed.
   return true;
}

void
LocationTester::printLbsStarted()
{
   SynchronousStatus status = m_locationInterface.lbsStarted();

   if (status.getStatusCode() == LBS_STATUS_STARTED) {
      cout << "LBS is started." << endl;
   } else if (status.getStatusCode() == LBS_STATUS_NOT_STARTED) {
      cout << "LBS is NOT started." << endl;
   } else {
      // This should not happen.
      cout << "LBS Error: code " << status.getStatusCode() << " message " 
           << status.getStatusMessage() << endl;
   }
}

void LocationTester::reverseGeocodingReply(RequestID requestID,
                                           GeocodingInformation info)
{
   std::cout << "Got a reply from the reverse geocoding request:" << std::endl;
   std::cout << "Outstanding requests: " << m_revGeoReq.size() << std::endl;
   ReverseGeocodingRequests::iterator it = m_revGeoReq.find(requestID);
   if(it != m_revGeoReq.end()) {
      std::cout << "Found request in set!" << std::endl;
      std::cout << "Country name: " << info.countryName << std::endl;
      std::cout << "TopRegionID: " << info.topRegionID << std::endl;
      std::cout << "Municipal name: " << info.municipalName << std::endl;
      std::cout << "City name: " << info.cityName << std::endl;
      std::cout << "District name: " << info.districtName << std::endl;
      std::cout << "Address name: " << info.addressName << std::endl;

      m_revGeoReq.erase(it);
      std::cout << "Outstanding requests after reply: " << m_revGeoReq.size()
                << std::endl;
      
   } else {
      std::cout << "No valid corresponding request found." << std::endl;
   }
   
}

void LocationTester::requestReverseGeocoding()
{
   WGS84Coordinate pos(55.5971, 13.009);
   AreaUpdateInformation loc(pos, 0, 0, 0, 0, 0);
   AsynchronousStatus status =
      m_locationInterface.requestReverseGeocoding(loc);
   m_revGeoReq.insert(status.getRequestID());
   
   WGS84Coordinate pos2(51.5129,7.46519);
   LocationUpdateInformation loc2(pos, 0, 0, QUALITY_EXCELLENT, 0, pos, 0);
   status = m_locationInterface.requestReverseGeocoding(loc2);
   m_revGeoReq.insert(status.getRequestID());

   

}
