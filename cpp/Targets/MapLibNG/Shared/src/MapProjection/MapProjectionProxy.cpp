/*
Copyright (c) 1999 - 2010, Vodafone Group Services Ltd
All rights reserved.

Redistribution and use in source and binary forms, with or without modification, are permitted provided that the following conditions are met:

    * Redistributions of source code must retain the above copyright notice, this list of conditions and the following disclaimer.
    * Redistributions in binary form must reproduce the above copyright notice, this list of conditions and the following disclaimer in the documentation and/or other materials provided with the distribution.
    * Neither the name of the Vodafone Group Services Ltd nor the names of its contributors may be used to endorse or promote products derived from this software without specific prior written permission.

THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS" AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE ARE DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT HOLDER OR CONTRIBUTORS BE LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
*/

#include "MapProjection/MapProjectionProxy.h"
#include "ExternalRenderer.h"

using namespace WFAPI;

MapProjectionProxy::MapProjectionProxy(MapProjection& projection,
                                       ExternalRenderer* extRenderer)
   : m_projection(projection),
     m_renderer(extRenderer)
{
    
}

WGS84Coordinate MapProjectionProxy::getCenter() const
{   
   return m_projection.getCenter(); 
}

void MapProjectionProxy::worldToScreen(ScreenPoint& point,
                                       const WGS84Coordinate& coord) const
{  
   MC2Coordinate mc2(coord);
   float x, y, z;
   
   m_renderer->transformf(mc2.lon, mc2.lat,
                          &x, &y, &z);

   point.getX() = static_cast<int>(x);
   point.getY() = static_cast<int>(y);
}

void MapProjectionProxy::screenToWorld(WGS84Coordinate& coord,
                                           const ScreenPoint& point) const
{
   MC2Coordinate tmpCoord( coord );
   MC2Point tmpPoint(point);
   m_renderer->inverseTransform(tmpCoord, tmpPoint);
   coord = tmpCoord;
}

void MapProjectionProxy::setCenter(const WGS84Coordinate& newCenter)
{
   if(newCenter.isValid()) {
      m_projection.setCenter(newCenter);
   }
}

void MapProjectionProxy::setPoint(const WGS84Coordinate& newCoord,
                                      const ScreenPoint& screenPoint )
{

   MC2Coordinate newCenter;
      
   m_renderer->calcOffsetCenterCoord(screenPoint, newCoord,
                                                   newCenter);
      

   m_projection.setCenter(newCenter);
}

void MapProjectionProxy::setPoint(const WGS84Coordinate& newCoord,
                                      const ScreenPoint& screenPoint, 
                                      double angleDegrees )
{
   
   
   m_projection.setPoint(newCoord, screenPoint, angleDegrees);
}

void MapProjectionProxy::move(wf_int32 deltaX, wf_int32 deltaY)
{
   
   
   m_projection.move(deltaX, deltaY); 
}

void MapProjectionProxy::setAngle(double angleDegrees)
{
   
   
   m_projection.setAngle(angleDegrees); 
}

void MapProjectionProxy::setAngle(double angleDegrees,
                                      const ScreenPoint& rotationPoint)
{
   
   
   m_projection.setAngle(angleDegrees, rotationPoint); 
}

void MapProjectionProxy::rotateLeftDeg(wf_int32 nbrDeg)
{
   //TODO: SetAngle
   // m_projection.rotateLeftDeg(nbrDeg); 
}

double MapProjectionProxy::getAngle() const
{
   
   
   return m_projection.getAngle();
}

double MapProjectionProxy::setZoomLevel(double zoomLevel)
{
   return m_projection.setPixelScale(zoomLevel); 
}

double MapProjectionProxy::getZoomLevel() const
{   
   return m_projection.getPixelScale(); 
}

double MapProjectionProxy::zoom(double factor)
{
   return m_projection.zoom(factor);
}

double MapProjectionProxy::zoom(double factor, 
                                    const WGS84Coordinate& zoomCoord,
                                    const ScreenPoint& zoomPoint)
{
   return m_projection.zoom(factor, zoomCoord, zoomPoint);
}

void MapProjectionProxy::setScreenBox(const ScreenPoint& oneCorner,
                                          const ScreenPoint& otherCorner)
{
   
   
   m_projection.setPixelBox(oneCorner, otherCorner); 
}

void MapProjectionProxy::setWorldBox(const WGS84Coordinate& oneCorner,
                                         const WGS84Coordinate& otherCorner) 
{
   
   
   MC2BoundingBox box( oneCorner, otherCorner );
   m_projection.setBoundingBox(box);
   // Set center so that center of box will be centered also in 3d. 
   m_projection.setCenter(box.getCenter());
}

void MapProjectionProxy::setMovementMode(bool moving)
{
   //TODO: WTF?
   // m_projection.setMovementMode(moving);
}

