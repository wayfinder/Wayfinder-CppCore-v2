/*
  Copyright (c) 1999 - 2010, Vodafone Group Services Ltd
  All rights reserved.

  Redistribution and use in source and binary forms, with or without modification, are permitted provided that the following conditions are met:

  * Redistributions of source code must retain the above copyright notice, this list of conditions and the following disclaimer.
  * Redistributions in binary form must reproduce the above copyright notice, this list of conditions and the following disclaimer in the documentation and/or other materials provided with the distribution.
  * Neither the name of the Vodafone Group Services Ltd nor the names of its contributors may be used to endorse or promote products derived from this software without specific prior written permission.

  THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS" AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE ARE DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT HOLDER OR CONTRIBUTORS BE LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
*/

#include "WFGfxUtil.h"
#include "WGS84Coordinate.h"
#include <math.h>


#undef SQUARE
#define SQUARE(x) ((x)*(x))

WFAPI::wf_float64 
WFGfxUtil::squareDistBetweenCoords(const WFAPI::WGS84Coordinate& coord1,
                                   const WFAPI::WGS84Coordinate& coord2)
{
   WFAPI::wf_float64 cos_lat= 
      cos( (coord1.latDeg/2+coord2.latDeg/2) * M_PI / 180 );

   WFAPI::wf_float64 delta_lat_sq =  
      SQUARE( (coord2.latDeg - coord1.latDeg) );
   WFAPI::wf_float64 delta_lon_sq =  
      SQUARE( (coord2.lonDeg - coord1.lonDeg) * cos_lat);

   return (delta_lat_sq + delta_lon_sq) * SQUARE(6378137.0*M_PI*2/360); 
}

