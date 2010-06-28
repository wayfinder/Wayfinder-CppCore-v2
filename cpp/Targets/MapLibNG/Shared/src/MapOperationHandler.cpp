/*
Copyright (c) 1999 - 2010, Vodafone Group Services Ltd
All rights reserved.

Redistribution and use in source and binary forms, with or without modification, are permitted provided that the following conditions are met:

    * Redistributions of source code must retain the above copyright notice, this list of conditions and the following disclaimer.
    * Redistributions in binary form must reproduce the above copyright notice, this list of conditions and the following disclaimer in the documentation and/or other materials provided with the distribution.
    * Neither the name of the Vodafone Group Services Ltd nor the names of its contributors may be used to endorse or promote products derived from this software without specific prior written permission.

THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS" AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE ARE DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT HOLDER OR CONTRIBUTORS BE LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
*/

#include "CoreFunctionPrologue.h"
#include "config.h"
#include "MapOperationHandler.h"
#include "MapProjection/MapProjection.h"
#include "ExternalRenderer.h"
#include "MC2BoundingBox.h"

using namespace WFAPI;

MapOperationHandler::MapOperationHandler(MapProjection* projection,
                                         ExternalRenderer* extRenderer):
   m_projection(projection),
   m_extRenderer(extRenderer)
{
   CORE_FUNCTION_PROLOGUE();
}

MapOperationHandler::~MapOperationHandler()
{
   CORE_FUNCTION_PROLOGUE();
}

WGS84Coordinate MapOperationHandler::getCenter() const
{
   CORE_FUNCTION_PROLOGUE();
   
   return m_projection->getCenter(); 
}

void MapOperationHandler::worldToScreen(ScreenPoint& point,
                                        const WGS84Coordinate& coord) const
{
   CORE_FUNCTION_PROLOGUE();
   
   MC2Coordinate mc2(coord);
   float x, y, z;
   
   m_extRenderer->transformf(mc2.lon, mc2.lat,
                             &x, &y, &z);

   point.getX() = static_cast<int>(x);
   point.getY() = static_cast<int>(y);
}

void MapOperationHandler::screenToWorld(WGS84Coordinate& coord,
                                        const ScreenPoint& point) const
{
   CORE_FUNCTION_PROLOGUE();
   
   MC2Coordinate tmpCoord( coord );
   MC2Point tmpPoint(point);
   m_extRenderer->inverseTransform(tmpCoord, tmpPoint);
   coord = tmpCoord;
}

void MapOperationHandler::setCenter(const WGS84Coordinate& newCenter)
{
   CORE_FUNCTION_PROLOGUE();
   
   if(newCenter.isValid()) {
      m_projection->setCenter(MC2Coordinate(newCenter));
   }
}

void MapOperationHandler::setPoint(const WGS84Coordinate& newCoord,
                                      const ScreenPoint& screenPoint )
{
   CORE_FUNCTION_PROLOGUE();

   {
      m_projection->beginBatchedEvent(MapProjectionListener::SET_POINT);
      m_projection->setPoint(MC2Coordinate(newCoord), screenPoint); 
      MC2Coordinate newCenter;
      
      m_extRenderer->calcOffsetCenterCoord(MC2Point(screenPoint), 
                                           MC2Coordinate(newCoord),
                                           newCenter);
      

      m_projection->setCenter(newCenter);
      m_projection->endBatchedEvent();
   }

}

void MapOperationHandler::setPoint(const WGS84Coordinate& newCoord,
                                      const ScreenPoint& screenPoint, 
                                      double angleDegrees )
{
   CORE_FUNCTION_PROLOGUE();

   ///XXX: Not implemented..
}

void MapOperationHandler::move(wf_int32 deltaX, wf_int32 deltaY)
{
   m_projection->move(deltaX, deltaY); 
}

void MapOperationHandler::setAngle(double angleDegrees)
{
   m_projection->setAngle(angleDegrees); 
}

void MapOperationHandler::setAngle(double angleDegrees,
                                      const ScreenPoint& rotationPoint)
{
   m_projection->setAngle(angleDegrees, MC2Point(rotationPoint)); 
}

void MapOperationHandler::rotateLeftDeg(wf_int32 nbrDeg)
{
   setAngle(getAngle() + nbrDeg);
}

double MapOperationHandler::getAngle() const
{
   return m_projection->getAngle();
}

double MapOperationHandler::setZoomLevel(double zoomLevel)
{
   return m_projection->setPixelScale(zoomLevel); 
}

double MapOperationHandler::getZoomLevel() const
{
   return m_projection->getPixelScale(); 
}

double MapOperationHandler::zoom(double factor)
{
   return m_projection->zoom(factor);
}

double MapOperationHandler::zoom(double factor, 
                                    const WGS84Coordinate& zoomCoord,
                                    const ScreenPoint& zoomPoint)
{
   return m_projection->zoom(factor, MC2Coordinate(zoomCoord), zoomPoint);
}

void MapOperationHandler::setScreenBox(const ScreenPoint& oneCorner,
                                          const ScreenPoint& otherCorner)
{
   m_projection->setPixelBox(oneCorner, otherCorner); 
}

void MapOperationHandler::setWorldBox(const WGS84Coordinate& oneCorner,
                                      const WGS84Coordinate& otherCorner) 
{
   MC2Coordinate c1(oneCorner);
   MC2Coordinate c2(otherCorner);
   MC2BoundingBox box( c1, c2 );

   {
      m_projection->beginBatchedEvent(MapProjectionListener::WORLD_BOX_SET);
      m_projection->setBoundingBox(box);
      // Set center so that center of box will be centered also in 3d. 
      m_projection->setCenter(box.getCenter());
      m_projection->endBatchedEvent();
   }

}

void MapOperationHandler::setMovementMode(bool moving)
{
   // XXX Not supported.
}

void MapOperationHandler::set3dMode(bool enabled)
{
   m_projection->set3dMode(enabled);
}

bool MapOperationHandler::get3dMode() const
{
   return m_projection->get3dMode();
}

void MapOperationHandler::setVariable3dMode(float step)
{
   m_projection->setVariable3dMode(step);
}

