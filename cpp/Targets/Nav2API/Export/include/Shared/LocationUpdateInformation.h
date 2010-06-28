/*
  Copyright (c) 1999 - 2010, Vodafone Group Services Ltd
  All rights reserved.

  Redistribution and use in source and binary forms, with or without modification, are permitted provided that the following conditions are met:

  * Redistributions of source code must retain the above copyright notice, this list of conditions and the following disclaimer.
  * Redistributions in binary form must reproduce the above copyright notice, this list of conditions and the following disclaimer in the documentation and/or other materials provided with the distribution.
  * Neither the name of the Vodafone Group Services Ltd nor the names of its contributors may be used to endorse or promote products derived from this software without specific prior written permission.

  THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS" AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE ARE DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT HOLDER OR CONTRIBUTORS BE LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
*/

#ifndef WFAPI_LOCATION_UPDATE_INFORMATION_H
#define WFAPI_LOCATION_UPDATE_INFORMATION_H

#include "WFAPIConfig.h"
#include "WGS84Coordinate.h"
#include "GpsQualityEnum.h"

namespace WFAPI {

/**
 * An update of current whereabouts that is precise enough to
 * be a point.
 *
 * Received in LocationListener call back function.
 */
class LocationUpdateInformation
{

public:

   /**
    * Default constructor. The Positions will be invalid and the quality
    * missing. Other parameters will be 0.
    */
   LocationUpdateInformation();

   /**
    * Constructor.
    *
    * @param position The position.
    * @param altitude The altitude in meters.
    * @param heading The heading in degrees.
    * @param gpsQuality The quality of the position.
    * @param speed The speed in km/h.
    * @param routePosition The current position snapped to the route.
    * @param routeHeading The current heading snapped to the route.
    */
   LocationUpdateInformation(const WGS84Coordinate& position,
                             wf_int32 altitude,
                             wf_uint16 heading,
                             GpsQualityEnum gpsQuality,
                             wf_uint16 speed,
                             const WGS84Coordinate& routePosition,
                             wf_uint16 routeHeading);

   /**
    * Copy constructor
    */
   LocationUpdateInformation(const LocationUpdateInformation& locationUpdateInformation);
   
   /**
    * Destructor.
    */
   virtual ~LocationUpdateInformation();

   /**
    *   Assignment operator.
    */
   LocationUpdateInformation& operator=(const LocationUpdateInformation& rhs);

   /**
    * Get the altitude.
    *
    * @return The altitude in meters.
    */
   wf_int32 getAltitude() const;

   /**
    * Get the heading.
    *
    * @return The heading in degrees.
    */
   wf_uint16 getHeading() const;

   /**
    * Get the PDOP value.
    *
    * @return The PDOP.
    */
   GpsQualityEnum getGpsQuality() const;

   /**
    * Get the position.
    *
    * @return The coordinate of the current position.
    */
   WGS84Coordinate getPosition() const;

   /**
    * Get the speed.
    *
    * @return The speed in km/h.
    */
   wf_uint16 getSpeed() const;

   /**
    * Gets the snapped position. This position could be used
    * when setting current position in the map during a route.
    * The postion has been snapped to the route and follows the red
    * line in a more precise way.
    *
    * @return The snapped position.
    */
   WGS84Coordinate getRoutePosition() const;

   /**
    * Gets the snapped heading. This heading should be used if using
    * the route position when setting current position in the map during
    * a route.
    *
    * @return The snapped heading.
    */
   wf_uint16 getRouteHeading() const;

private:

   /// The altitude.
   wf_int32 m_altitude;

   /**
    * Heading in degrees.
    */
   wf_uint16 m_heading;

   /**
    * The gps quality, can be found in GpsQualityEnum.h
    */
   GpsQualityEnum m_gpsQuality;

   /**
    * The position.
    */
   WGS84Coordinate m_position;

   /**
    * The speed.
    */
   wf_uint16 m_speed;

   /// The snapped postion, the postion is snapped to the route.
   WGS84Coordinate m_routePosition;

   /// The snapped heading, snapped to the route.
   wf_uint16 m_routeHeading;
};

} // End namespace WFAPI

#endif // End WFAPI_LOCATION_UPDATE_INFORMATION_H
