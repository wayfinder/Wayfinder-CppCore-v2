/*
Copyright (c) 1999 - 2010, Vodafone Group Services Ltd
All rights reserved.

Redistribution and use in source and binary forms, with or without modification, are permitted provided that the following conditions are met:

    * Redistributions of source code must retain the above copyright notice, this list of conditions and the following disclaimer.
    * Redistributions in binary form must reproduce the above copyright notice, this list of conditions and the following disclaimer in the documentation and/or other materials provided with the distribution.
    * Neither the name of the Vodafone Group Services Ltd nor the names of its contributors may be used to endorse or promote products derived from this software without specific prior written permission.

THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS" AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE ARE DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT HOLDER OR CONTRIBUTORS BE LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
*/

#include "MapProjection/ProjectionInterface.h"
#include "MapProjection/MapProjection.h"
#include "MapProjection/ProjectionCoordinate.h"
#include "ExternalRenderer.h"

ProjectionInterface::ProjectionInterface(MapProjection& projection,
                                         ExternalRenderer& renderer) : 
   m_projection(projection),
   m_renderer(renderer)
{
   
}

void
ProjectionInterface::getProjectionCoordinate(float wgs84lat,
                                             float wgs84lon,
                                             ProjectionCoordinate* outCoord)
{   
   ProjectionCoordinate ret;

   WFAPI::WGS84Coordinate wgs(wgs84lat, wgs84lon);
   MC2Coordinate mc2(wgs);
   
   switch(m_projection.getProjectionType()) {
   case MapProjection::MC2_PROJECTION:
   {
      ret.x.i = mc2.lon;
      ret.y.i = mc2.lat;
      break;
   }
   case MapProjection::MERCATOR_PROJECTION:
   {
      MercatorCoord merc(mc2);

      ret.x.i = merc.lon;
      ret.y.i = merc.lat;
      break;
   }
   default:
      break;
   };

   *outCoord = ret;
}

void ProjectionInterface::projectf(const ProjectionCoordinate& coord,
                                   float* x,
                                   float* y,
                                   float* z)
{
   m_renderer.transformf(coord.x.i, coord.y.i, x, y, z);
}
