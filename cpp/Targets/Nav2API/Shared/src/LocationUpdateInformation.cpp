/*
  Copyright (c) 1999 - 2010, Vodafone Group Services Ltd
  All rights reserved.

  Redistribution and use in source and binary forms, with or without modification, are permitted provided that the following conditions are met:

  * Redistributions of source code must retain the above copyright notice, this list of conditions and the following disclaimer.
  * Redistributions in binary form must reproduce the above copyright notice, this list of conditions and the following disclaimer in the documentation and/or other materials provided with the distribution.
  * Neither the name of the Vodafone Group Services Ltd nor the names of its contributors may be used to endorse or promote products derived from this software without specific prior written permission.

  THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS" AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE ARE DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT HOLDER OR CONTRIBUTORS BE LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
*/

#include "LocationUpdateInformation.h"

namespace WFAPI {

LocationUpdateInformation::LocationUpdateInformation()
   : m_altitude(0),
     m_heading(0),
     m_gpsQuality(QUALITY_MISSING),
     m_speed(0),
     m_routeHeading(0)
{

}

LocationUpdateInformation::LocationUpdateInformation(
   const WGS84Coordinate& position,
   wf_int32 altitude,
   wf_uint16 heading,
   GpsQualityEnum gpsQuality,
   wf_uint16 speed,
   const WGS84Coordinate& routePosition,
   wf_uint16 routeHeading)
   : m_altitude(altitude), m_heading(heading),
     m_gpsQuality(gpsQuality), m_position(position), m_speed(speed),
     m_routePosition(routePosition), m_routeHeading(routeHeading)
{
}

LocationUpdateInformation::LocationUpdateInformation(
   const LocationUpdateInformation& locationUpdateInformation)
   : m_altitude(locationUpdateInformation.m_altitude),
     m_heading(locationUpdateInformation.m_heading),
     m_gpsQuality(locationUpdateInformation.m_gpsQuality),
     m_position(locationUpdateInformation.m_position),
     m_speed(locationUpdateInformation.m_speed),
     m_routePosition(locationUpdateInformation.m_routePosition),
     m_routeHeading(locationUpdateInformation.m_routeHeading)
{
}


LocationUpdateInformation::~LocationUpdateInformation()
{
}

LocationUpdateInformation&
LocationUpdateInformation::operator=(const LocationUpdateInformation& rhs)
{
   if (this == &rhs) {
      // Handle self assignment
      return *this;
   }

   m_altitude = rhs.m_altitude;
   m_heading = rhs.m_heading;
   m_gpsQuality = rhs.m_gpsQuality;
   m_position = rhs.m_position;
   m_speed = rhs.m_speed;
   m_routePosition = rhs.m_routePosition;
   m_routeHeading = rhs.m_routeHeading;
      
   return *this;
}

wf_int32
LocationUpdateInformation::getAltitude() const
{
   return m_altitude;
}

wf_uint16
LocationUpdateInformation::getHeading() const
{
   return m_heading;
}

GpsQualityEnum
LocationUpdateInformation::getGpsQuality() const
{
   return m_gpsQuality;
}

WGS84Coordinate
LocationUpdateInformation::getPosition() const
{
   return m_position;
}

wf_uint16
LocationUpdateInformation::getSpeed() const
{
   return m_speed;
}

WGS84Coordinate
LocationUpdateInformation::getRoutePosition() const
{
   return m_routePosition;    
}

wf_uint16
LocationUpdateInformation::getRouteHeading() const
{
   return m_routeHeading;    
}

} // End namespace WFAPI


