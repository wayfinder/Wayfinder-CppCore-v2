/*
Copyright (c) 1999 - 2010, Vodafone Group Services Ltd
All rights reserved.

Redistribution and use in source and binary forms, with or without modification, are permitted provided that the following conditions are met:

    * Redistributions of source code must retain the above copyright notice, this list of conditions and the following disclaimer.
    * Redistributions in binary form must reproduce the above copyright notice, this list of conditions and the following disclaimer in the documentation and/or other materials provided with the distribution.
    * Neither the name of the Vodafone Group Services Ltd nor the names of its contributors may be used to endorse or promote products derived from this software without specific prior written permission.

THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS" AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE ARE DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT HOLDER OR CONTRIBUTORS BE LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
*/


#include "config.h"
#include "GfxConstants.h"
#include "WGS84Coordinate.h"


// -- WGS84Coordinate.

namespace WFAPI {

WGS84Coordinate::WGS84Coordinate()
   : latDeg(180.0), lonDeg(180.0)
{
   
}

WGS84Coordinate::WGS84Coordinate(double latDeg, double lonDeg)
   : latDeg(latDeg), lonDeg(lonDeg)
{
   
}

WGS84Coordinate::WGS84Coordinate(const WGS84Coordinate& coord)
   : latDeg(coord.latDeg), lonDeg(coord.lonDeg)
{
   
}

bool WFAPI::WGS84Coordinate::isValid() const
{
   // Coordinate is only valid if latDeg is with -90 to 90 degrees
   // and lonDeg is within -180 to 180.
   if (latDeg <= 90.0 && latDeg >= -90.0
       && lonDeg <= 180.0 && lonDeg >= -180.0){
      return true;
   } else {
      return false;
   }
}



WGS84Coordinate&
WGS84Coordinate::operator=(const WGS84Coordinate& coord)
{
   latDeg = coord.latDeg;
   lonDeg = coord.lonDeg;
   
   return *this;
}

bool
WGS84Coordinate::operator==(const WGS84Coordinate& rhs)
{
   if(latDeg == rhs.latDeg && lonDeg == rhs.lonDeg) {
      return true;
   } else {
      return false;
   }
}


#ifdef WFAPI_IOSTREAM_AVAILABLE

std::ostream&
operator<< (std::ostream& stream,
           const WGS84Coordinate& coord)
{
   return stream << "(" << coord.latDeg << "," << coord.lonDeg << ")";
}

#endif //WFAPI_IOSTREAM_AVAILABLE
}


