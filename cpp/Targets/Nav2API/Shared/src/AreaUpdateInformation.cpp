/*
  Copyright (c) 1999 - 2010, Vodafone Group Services Ltd
  All rights reserved.

  Redistribution and use in source and binary forms, with or without modification, are permitted provided that the following conditions are met:

  * Redistributions of source code must retain the above copyright notice, this list of conditions and the following disclaimer.
  * Redistributions in binary form must reproduce the above copyright notice, this list of conditions and the following disclaimer in the documentation and/or other materials provided with the distribution.
  * Neither the name of the Vodafone Group Services Ltd nor the names of its contributors may be used to endorse or promote products derived from this software without specific prior written permission.

  THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS" AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE ARE DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT HOLDER OR CONTRIBUTORS BE LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
*/

#include "AreaUpdateInformation.h"

namespace WFAPI {


AreaUpdateInformation::AreaUpdateInformation()
   : m_altitude(0),
     m_endAngle(0),
     m_innerRadius(0),
     m_outerRadius(0),
     m_startAngle(0)
{

}

AreaUpdateInformation::AreaUpdateInformation(const WGS84Coordinate& position, 
                                             wf_int32 altitude, 
                                             wf_uint32 innerRadius, 
                                             wf_uint32 outerRadius, 
                                             wf_uint16 startAngle, 
                                             wf_uint16 endAngle)
   : m_altitude(altitude), m_endAngle(endAngle),
     m_innerRadius(innerRadius), m_outerRadius(outerRadius),
     m_position(position), m_startAngle(startAngle)
{
}

AreaUpdateInformation::AreaUpdateInformation(
   const AreaUpdateInformation& areaUpdateInformation)
   : m_altitude(areaUpdateInformation.m_altitude),
     m_endAngle(areaUpdateInformation.m_endAngle),
     m_innerRadius(areaUpdateInformation.m_innerRadius),
     m_outerRadius(areaUpdateInformation.m_outerRadius),
     m_position(areaUpdateInformation.m_position),
     m_startAngle(areaUpdateInformation.m_startAngle)
{
}

AreaUpdateInformation::~AreaUpdateInformation()
{
}

AreaUpdateInformation&
AreaUpdateInformation::operator=(const AreaUpdateInformation& rhs)
{
   if (this == &rhs) {
      // Handle self assignment
      return *this;
   }
   m_altitude = rhs.m_altitude;
   m_endAngle = rhs.m_endAngle;
   m_innerRadius = rhs.m_innerRadius;
   m_outerRadius = rhs.m_outerRadius;
   m_position = rhs.m_position;
   m_startAngle = rhs.m_startAngle;
      
   return *this;
}

wf_int32
AreaUpdateInformation::getAltitude() const
{
   return m_altitude;
}

wf_uint16
AreaUpdateInformation::getEndAngle() const
{
   return m_endAngle;
}

wf_uint32
AreaUpdateInformation::getInnerRadius() const
{
   return m_innerRadius;
}

wf_uint32
AreaUpdateInformation::getOuterRadius() const
{
   return m_outerRadius;
}

WGS84Coordinate
AreaUpdateInformation::getPosition() const
{
   return m_position;
}

wf_uint16
AreaUpdateInformation::getStartAngle() const
{
   return m_startAngle;
}

} // End namespace WFAPI
