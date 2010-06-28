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
#include "MapLibOperationHandler.h"
#include "MapProjection/MapProjectionAdapter.h"
#include "TileMapHandler.h"

using namespace WFAPI;

MapLibOperationHandler::MapLibOperationHandler(MapProjectionAdapter* projAdapter):
   m_projectionAdapter(projAdapter)
{
   CORE_FUNCTION_PROLOGUE();
}

MapLibOperationHandler::~MapLibOperationHandler()
{
   CORE_FUNCTION_PROLOGUE();
}

WGS84Coordinate MapLibOperationHandler::getCenter() const
{
   CORE_FUNCTION_PROLOGUE();
   
   return m_projectionAdapter->getCenter(); 
}

void MapLibOperationHandler::worldToScreen(ScreenPoint& point,
                                           const WGS84Coordinate& coord) const
{
   CORE_FUNCTION_PROLOGUE();
   
   MC2Point tmp( point );
   m_projectionAdapter->transform( tmp, coord );
   point.getX() = tmp.getX();
   point.getY() = tmp.getY();
}

void MapLibOperationHandler::screenToWorld(WGS84Coordinate& coord,
                                           const ScreenPoint& point) const
{
   CORE_FUNCTION_PROLOGUE();
   
   MC2Coordinate tmpCoord( coord );
   MC2Point tmpPoint(point);
   m_projectionAdapter->inverseTransform( tmpCoord, tmpPoint );
   coord = tmpCoord;
}

void MapLibOperationHandler::setCenter(const WGS84Coordinate& newCenter)
{
   CORE_FUNCTION_PROLOGUE();
   
   if(newCenter.isValid()) {
      m_projectionAdapter->setCenter(newCenter);
   }
}

void MapLibOperationHandler::setPoint(const WGS84Coordinate& newCoord,
                                      const ScreenPoint& screenPoint )
{
   CORE_FUNCTION_PROLOGUE();
   
   m_projectionAdapter->setPoint(newCoord, screenPoint); 
}

void MapLibOperationHandler::setPoint(const WGS84Coordinate& newCoord,
                                      const ScreenPoint& screenPoint, 
                                      double angleDegrees )
{
   CORE_FUNCTION_PROLOGUE();
   
   m_projectionAdapter->setPoint(newCoord, screenPoint, angleDegrees);
}

void MapLibOperationHandler::move(wf_int32 deltaX, wf_int32 deltaY)
{
   CORE_FUNCTION_PROLOGUE();
   
   m_projectionAdapter->move(deltaX, deltaY); 
}

void MapLibOperationHandler::setAngle(double angleDegrees)
{
   CORE_FUNCTION_PROLOGUE();
   
   m_projectionAdapter->setAngle(angleDegrees); 
}

void MapLibOperationHandler::setAngle(double angleDegrees,
                                      const ScreenPoint& rotationPoint)
{
   CORE_FUNCTION_PROLOGUE();
   
   m_projectionAdapter->setAngle(angleDegrees, rotationPoint); 
}

void MapLibOperationHandler::rotateLeftDeg(wf_int32 nbrDeg)
{
   CORE_FUNCTION_PROLOGUE();
   
   m_projectionAdapter->rotateLeftDeg(nbrDeg); 
}

double MapLibOperationHandler::getAngle() const
{
   CORE_FUNCTION_PROLOGUE();
   
   return m_projectionAdapter->getAngle();
}

double MapLibOperationHandler::setZoomLevel(double zoomLevel)
{
   CORE_FUNCTION_PROLOGUE();
   
   return m_projectionAdapter->setScale(zoomLevel); 
}

double MapLibOperationHandler::getZoomLevel() const
{
   CORE_FUNCTION_PROLOGUE();
   
   return m_projectionAdapter->getScale(); 
}

double MapLibOperationHandler::zoom(double factor)
{
   CORE_FUNCTION_PROLOGUE();
   
   double res;
   m_projectionAdapter->zoom(factor, res);
   return res;
}

double MapLibOperationHandler::zoom(double factor, 
                                    const WGS84Coordinate& zoomCoord,
                                    const ScreenPoint& zoomPoint)
{
   CORE_FUNCTION_PROLOGUE();
   
   double res;
   m_projectionAdapter->zoom(factor, zoomCoord, zoomPoint, res);
   return res;
}

void MapLibOperationHandler::setScreenBox(const ScreenPoint& oneCorner,
                                          const ScreenPoint& otherCorner)
{
   CORE_FUNCTION_PROLOGUE();
   
   m_projectionAdapter->setPixelBox(oneCorner, otherCorner); 
}

void MapLibOperationHandler::setWorldBox(const WGS84Coordinate& oneCorner,
                                         const WGS84Coordinate& otherCorner) 
{
   CORE_FUNCTION_PROLOGUE();
   
   MC2BoundingBox box( oneCorner, otherCorner );
   m_projectionAdapter->setBoundingBox(box);
   // Set center so that center of box will be centered also in 3d. 
   m_projectionAdapter->setCenter(box.getCenter());
}

void MapLibOperationHandler::setMovementMode(bool moving)
{
   CORE_FUNCTION_PROLOGUE();
   
   m_projectionAdapter->setMovementMode(moving);
}

void MapLibOperationHandler::set3dMode(bool enabled)
{
   assert(!"Not implemented yet");
}

bool MapLibOperationHandler::get3dMode() const
{
   assert(!"Not implemented yet");
   return false;
}

void MapLibOperationHandler::setVariable3dMode(float step)
{
   assert(!"Not implemented yet");
}

