/*
  Copyright (c) 1999 - 2010, Vodafone Group Services Ltd
  All rights reserved.

  Redistribution and use in source and binary forms, with or without modification, are permitted provided that the following conditions are met:

  * Redistributions of source code must retain the above copyright notice, this list of conditions and the following disclaimer.
  * Redistributions in binary form must reproduce the above copyright notice, this list of conditions and the following disclaimer in the documentation and/or other materials provided with the distribution.
  * Neither the name of the Vodafone Group Services Ltd nor the names of its contributors may be used to endorse or promote products derived from this software without specific prior written permission.

  THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS" AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE ARE DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT HOLDER OR CONTRIBUTORS BE LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
*/

#ifndef WFAPI_AREA_UPDATE_INFORMATION_H
#define WFAPI_AREA_UPDATE_INFORMATION_H

#include "WFAPIConfig.h"
#include "WGS84Coordinate.h"

namespace WFAPI {


/**
 * An update of current whereabouts that is not precise enough to
 * be a point. That is represented by the area of a circle sector
 * that has a smaller radius circle sector removed from it. Like
 * a pie slice that has the pointy tip bitten of.
 *
 * Received in LocationListener call back function.
 */
class AreaUpdateInformation
{

public:

   /**
    * Default constructor. The Position will be invalid and the other
    * parameters will be 0.
    */
   AreaUpdateInformation();
   
   /**
    * Constructor.
    * 
    * @param position The center of the circle sector.
    * @param altitude The altitude in meters.
    * @param innerRadius The inner radius of the circle sector.
    * @param outerRadius The outer radius of the circle sector.
    * @param startAngle The start angle of the circle sector.
    * @param endAngle The end angle of the circle sector.
    */
   AreaUpdateInformation(const WGS84Coordinate& position, 
                         wf_int32 altitude, 
                         wf_uint32 innerRadius, 
                         wf_uint32 outerRadius, 
                         wf_uint16 startAngle, 
                         wf_uint16 endAngle);

   /**
    * Copy constructor
    */
   AreaUpdateInformation(const AreaUpdateInformation& areaUpdateInformation);
   
   /**
    * Destructor.
    */
   virtual ~AreaUpdateInformation();
   
   /**
    *   Assignment operator.
    */
   AreaUpdateInformation& operator=(const AreaUpdateInformation& rhs);
   
   /**
    * Get the altitude.
    *
    * @return The altitude in meters.
    */
   wf_int32 getAltitude() const;

   /**
    * Get the end angle of the circle sector.
    *
    * @return The end angle in degrees.
    */
   wf_uint16 getEndAngle() const;

   /**
    * Get the inner radius of the circle sector.
    *
    * @return The inner radius in meters.
    */
   wf_uint32 getInnerRadius() const;

   /**
    * Get the outer radius of the circle sector.
    *
    * @return The inner radius in meters.
    */
   wf_uint32 getOuterRadius() const;

   /**
    * Get the center coordinate of the circle sector.
    *
    * @return The center of the circle sector.
    */
   WGS84Coordinate getPosition() const;

   /**
    * Get the start angle of the circle sector.
    *
    * @return The start angle in degrees.
    */
   wf_uint16 getStartAngle() const;

private:

   /// The altitude.
   wf_int32 m_altitude;

   /// The end angle.
   wf_uint16 m_endAngle;

   /**
    * The inner radius.
    */
   wf_uint32 m_innerRadius;

   /// The outer radius.
   wf_uint32 m_outerRadius;

   /// The coordinate.
   WGS84Coordinate m_position;

   /// The start angle.
   wf_uint16 m_startAngle;

};

} // End namespace WFAPI

#endif // End WFAPI_AREA_UPDATE_INFORMATION_H



