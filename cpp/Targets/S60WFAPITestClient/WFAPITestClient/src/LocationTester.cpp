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
#include "MapOperationInterface.h"
#include <e32debug.h>
#include <map>

using namespace std;

LocationTester::LocationTester(
   LocationInterface& locationInterface,
   WFAPI::MapOperationInterface& operationInterface)
   : m_locationInterface(locationInterface),
     m_operationInterface(operationInterface)
{
   m_locationInterface.addLocationListener(this);
}

LocationTester::~LocationTester()
{
   m_locationInterface.removeLocationListener(this);
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
LocationTester::areaUpdate(const AreaUpdateInformation& areaUpdateInformation)
{
   // Print header
   // cout << "*******************************************************" << endl;
   // cout << "******************** Area Update **********************" << endl;
   // cout << "*******************************************************" << endl;

   // Print data
   // cout << "Position: " << areaUpdateInformation.getPosition() << endl;
   // cout << "InnerRadius: " << areaUpdateInformation.getInnerRadius() << endl;
   // cout << "OuterRadius: " << areaUpdateInformation.getOuterRadius() << endl;
   // cout << "StartAngle: " << areaUpdateInformation.getStartAngle() << endl;
   // cout << "EndAngle: " << areaUpdateInformation.getEndAngle() << endl;
   // cout << "Altitude: " << areaUpdateInformation.getAltitude() << endl;
}

void
LocationTester::locationUpdate(const LocationUpdateInformation& locationUpdate)
{
   // Print header
   // cout << "*******************************************************" << endl;
   // cout << "****************** Location Update ********************" << endl;
   // cout << "*******************************************************" << endl;

   // Print data
   // cout << "Position: " << locationUpdate.getPosition() << endl;
   // cout << "heading: " << locationUpdate.getHeading() << endl;
   // cout << "Speed: " << locationUpdate.getSpeed() << endl;
   // cout << "Altitude: " << locationUpdate.getAltitude() << endl;
   // cout << "Quality: " << locationUpdate.getGpsQuality() << endl;

   // RDebug::Print(_L("LocTester::POS: lat: %f lon: %f\n"),
   //               locationUpdate.getPosition().latDeg,
   //               locationUpdate.getPosition().lonDeg);
   m_operationInterface.setCenter(locationUpdate.getPosition());
}

void
LocationTester::startedLbs()
{
   // Print
   // cout << "******************** LBS Started **********************" << endl;
}

void LocationTester::error(const AsynchronousStatus& status)
{
   // cout << "Error for request " << status.getRequestID().getID() 
   //      << " code " << status.getStatusCode() << " message " 
   //      << status.getStatusMessage() << endl;
   
}

bool LocationTester::handleStartLbs()
{
   m_locationInterface.addLocationListener(this);
   m_locationInterface.startLbs();
   return true;
}

bool LocationTester::handleStopLbs()
{
   m_locationInterface.removeLocationListener(this);
   m_locationInterface.stopLbs();
   return true;
}

