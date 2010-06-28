/*
Copyright (c) 1999 - 2010, Vodafone Group Services Ltd
All rights reserved.

Redistribution and use in source and binary forms, with or without modification, are permitted provided that the following conditions are met:

    * Redistributions of source code must retain the above copyright notice, this list of conditions and the following disclaimer.
    * Redistributions in binary form must reproduce the above copyright notice, this list of conditions and the following disclaimer in the documentation and/or other materials provided with the distribution.
    * Neither the name of the Vodafone Group Services Ltd nor the names of its contributors may be used to endorse or promote products derived from this software without specific prior written permission.

THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS" AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE ARE DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT HOLDER OR CONTRIBUTORS BE LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
*/


#ifndef _MAPLIBOPERATIONHANDLER_H_
#define _MAPLIBOPERATIONHANDLER_H_

#include "PALMachineTypes.h"
#include "MapOperationInterface.h"
#include "WGS84Coordinate.h"

class MapProjectionAdapter;
class TileMapHandler;

class MapLibOperationHandler : public WFAPI::MapOperationInterface {
public:
   MapLibOperationHandler(MapProjectionAdapter* projAdapter);

   /**
    * Virtual destructor.
    */
   virtual ~MapLibOperationHandler();
      
   /**
    *  @see MapOperationInterface::getCenter
    */
   virtual WFAPI::WGS84Coordinate getCenter() const;

   /**
    *  @see MapOperationInterface::worldToScreen
    */
   virtual void worldToScreen(WFAPI::ScreenPoint& point,
                              const WFAPI::WGS84Coordinate& coord) const;
   
   /**
    *  @see MapOperationInterface::screenToWorld
    */
   virtual void screenToWorld(WFAPI::WGS84Coordinate& coord,
                              const WFAPI::ScreenPoint& point) const;
   
   /**
    *  @see MapOperationInterface::setCenter
    */
   virtual void setCenter(const WFAPI::WGS84Coordinate& newCenter);
   
   /**
    *  @see MapOperationInterface::setPoint
    */
   virtual void setPoint(const WFAPI::WGS84Coordinate& newCoord,
                         const WFAPI::ScreenPoint& screenPoint );

   /**
    *  @see MapOperationInterface::setPoint
    */
   virtual void setPoint(const WFAPI::WGS84Coordinate& newCoord,
                         const WFAPI::ScreenPoint& screenPoint, 
                         double angleDegrees );

   /**
    *  @see MapOperationInterface::move
    */
   virtual void move(WFAPI::wf_int32 deltaX, WFAPI::wf_int32 deltaY);
   
   /**
    *  @see MapOperationInterface::setAngle
    */
   virtual void setAngle(double angleDegrees);

   /**
    *  @see MapOperationInterface::setAngle
    */
   virtual void setAngle(double angleDegrees,
                         const WFAPI::ScreenPoint& rotationPoint);

   /**
    *  @see MapOperationInterface::rotateLeftDeg
    */
   virtual void rotateLeftDeg(WFAPI::wf_int32 nbrDeg);
   
   /**
    *  @see MapOperationInterface::getAngle
    */
   virtual double getAngle() const;

   /**
    *  @see MapOperationInterface::setZoomLevel
    */
   virtual double setZoomLevel(double zoomLevel);

   /**
    *  @see MapOperationInterface::getZoomLevel
    */
   virtual double getZoomLevel() const;

   /**
    *  @see MapOperationInterface::zoom
    */
   virtual double zoom(double factor);
   
   /**
    *  @see MapOperationInterface::zoom
    */
   virtual double zoom(double factor, 
                       const WFAPI::WGS84Coordinate& zoomCoord,
                       const WFAPI::ScreenPoint& zoomPoint);

   /**
    *  @see MapOperationInterface::setScreenBox
    */
   virtual void setScreenBox(const WFAPI::ScreenPoint& oneCorner,
                             const WFAPI::ScreenPoint& otherCorner);

   /**
    *  @see MapOperationInterface::setWorldBox
    */
   virtual void setWorldBox(const WFAPI::WGS84Coordinate& oneCorner,
                            const WFAPI::WGS84Coordinate& otherCorner) ;

   /**
    *  @see MapOperationInterface::setMovementMode
    */
   virtual void setMovementMode(bool moving);

   /**
    *  @see MapOperationInterface::set3dMode
    */
   void set3dMode(bool enabled);

   /**
    *  @see MapOperationInterface::get3dMode
    */
   bool get3dMode() const;

   /**
    *  @see MapOperationInterface::setVariable3dMode
    */  
   void setVariable3dMode(float step);
   
private:
   MapProjectionAdapter* m_projectionAdapter;
};

#endif /* _MAPLIBOPERATIONHANDLER_H_ */
