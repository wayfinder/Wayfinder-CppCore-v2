/*
Copyright (c) 1999 - 2010, Vodafone Group Services Ltd
All rights reserved.

Redistribution and use in source and binary forms, with or without modification, are permitted provided that the following conditions are met:

    * Redistributions of source code must retain the above copyright notice, this list of conditions and the following disclaimer.
    * Redistributions in binary form must reproduce the above copyright notice, this list of conditions and the following disclaimer in the documentation and/or other materials provided with the distribution.
    * Neither the name of the Vodafone Group Services Ltd nor the names of its contributors may be used to endorse or promote products derived from this software without specific prior written permission.

THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS" AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE ARE DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT HOLDER OR CONTRIBUTORS BE LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
*/


#include "Triangulation/WFTriangulation.h"
#include "PolygonTriangulation.h"
#include "PolylineTriangulation.h"

namespace Triangulation {

namespace WF {

int triangulatePolygon(CoordsArg::const_iterator begin,
                       CoordsArg::const_iterator end,
                       const MC2Coordinate& refCoord,
                       int color,
                       vec4x* output)
{   
   static vbuf3f contour, polygon;
   contour.clear();
   polygon.clear();

   for(CoordsArg::const_iterator itr = begin; itr != end; itr++)
   {
      int x = itr->lon - refCoord.lon;
      int y = itr->lat - refCoord.lat;

      contour.push_back(vec3f((float)x, (float)y));
   }
      
   if( !PolygonTriangulation::triangulate(contour, polygon) ) {
      return 0;
   }

   // Copy the triangulated floating point polygon to fixed point storage
   
   for(vbuf3f::iterator vBI = polygon.begin(); vBI != polygon.end(); vBI++)
   {
      vec3f& curCoord = *vBI;

      vec4x ins = { vec3x( fixedp(fixedp::RAW,  (int)curCoord.x ),
                           fixedp(fixedp::RAW,  (int)curCoord.y ) )
#ifdef VEC_3D_USE_COLOR_BUFFER
                    , color
#endif
      };

      *output++ = ins;
   }

   return 1;
}


int triangulateLine(CoordsArg::const_iterator begin,
                    CoordsArg::const_iterator end,
                    const MC2Coordinate& refCoord,
                    float cosLat,
                    float widthMC2,
                    int color,
                    vec4x* output)
{
   static vbuf3f buf, res;

   buf.clear();
   res.clear();
   
   TileMapCoords::const_iterator coordIt = begin;
   
   while( coordIt != end ) {
      buf.push_back(vec3f(coordIt->lon - refCoord.lon,
                          coordIt->lat - refCoord.lat ) );
         
      coordIt++;
   }
   
   PolylineTriangulation::triangulate(buf, res, cosLat, widthMC2);

   // Copy the triangulated floating point line to fixed point storage
   
   for(vbuf3f::iterator vBI = res.begin(); vBI != res.end(); vBI++)
   {
      vec3f& curCoord = *vBI;

      vec4x ins = { vec3x( fixedp(fixedp::RAW,  (int)curCoord.x ),
                           fixedp(fixedp::RAW,  (int)curCoord.y ) )
#ifdef VEC_3D_USE_COLOR_BUFFER
                    , color
#endif
      };

      *output++ = ins;
   }
   
   return 1;
}

} // End of namespace WF

} // End of namespace Triangulation

