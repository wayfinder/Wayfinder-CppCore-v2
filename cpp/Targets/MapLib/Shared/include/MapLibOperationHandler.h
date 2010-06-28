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
    * Returns the center coordinate of the map in WGS84 coordinates.
    *
    * @return The center coordinate of the map.
    */
   virtual WFAPI::WGS84Coordinate getCenter() const;

   /**
    * Transforms a real world coordinate to a screen coordinate.
    *   
    * @param point The point parameter will be set to the screen point
    *              that the world coordinate is shown at.
    * @param coord The world coordinate.
    */
   virtual void worldToScreen(WFAPI::ScreenPoint& point,
                              const WFAPI::WGS84Coordinate& coord) const;
   
   /**
    * Transforms the point on the screen to a real world
    * coordinate.
    *
    * @param coord The coord parameter will be set to the world coordinate
    *              that the screen point is at.
    * @param point The screen point.
    */
   virtual void screenToWorld(WFAPI::WGS84Coordinate& coord,
                              const WFAPI::ScreenPoint& point) const;
   
   /**
    * Sets the center coordinate to newCenter.
    *
    * @param newCenter The new center coordinate of the map.
    */
   virtual void setCenter(const WFAPI::WGS84Coordinate& newCenter);
   
   /**
    * Sets the specified point on the screen to the specified
    * world coordinate.
    * 
    * @param newCoord The new world coordinate to move the specified
    *                 screen point to.
    * @param screenPoint The screen point on the screen to set to the
    *                    specified world coordinate.
    */
   virtual void setPoint(const WFAPI::WGS84Coordinate& newCoord,
                         const WFAPI::ScreenPoint& screenPoint );

   /**
    * Sets the specified point on the screen to the
    * specified coordinate and rotate around that point.
    *
    * This method call results in the same thing as below, 
    * but more efficiently implemented:
    *
    * setPoint( newCoord, screenPoint );
    * setAngle( angleDegrees, screenPoint );
    *   
    * @param newCoord The new world coordinate to move the specified
    *                 screen point to.
    * @param screenPoint The screen point on the screen to set to the
    *                    specified world coordinate.
    * @param angleDegrees The angle in degrees to rotate around 
    *                     the screenPoint(in degrees).
    */
   virtual void setPoint(const WFAPI::WGS84Coordinate& newCoord,
                         const WFAPI::ScreenPoint& screenPoint, 
                         double angleDegrees );

   /**
    * Moves the screen. Unit pixels.
    *
    * @param deltaX Positive value means the map is moved 
    *               that amount of pixels to the right.
    * @param deltaY Positive value means the map is moved 
    *               that amount of pixels upwards.
    */
   virtual void move(WFAPI::wf_int32 deltaX, WFAPI::wf_int32 deltaY);
   
   /**
    * Sets the angle to the number of degrees in the
    * parameter. Rotates around the center.
    *
    * @param angleDegrees The angle in degrees to rotate.
    */
   virtual void setAngle(double angleDegrees);

   /**
    * Sets the angle to the number of degrees in the
    * parameter. Rotates around the rotation point.
    * 
    * @param angleDegrees  Angle in degrees.
    * @param rotationPoint Point to rotate around.
    */
   virtual void setAngle(double angleDegrees,
                         const WFAPI::ScreenPoint& rotationPoint);

   /**
    * Rotates the display the supplied number of degrees 
    * counter clock wise.
    *
    * @param nbrDeg Amount of degrees to rotate ccw.
    */
   virtual void rotateLeftDeg(WFAPI::wf_int32 nbrDeg);
   
   /**
    * Returns the current angle in degrees.
    *
    * @return The current angle in degrees of the map heading.
    */
   virtual double getAngle() const;

   /**
    * Sets zoom level. 
    * 
    * @param zoomLevel The new zoom level.
    * @return The zoom level that was set.
    */
   virtual double setZoomLevel(double zoomLevel);

   /**
    * Returns the current zoom level.
    *
    * @return The zoom level.
    */
   virtual double getZoomLevel() const;

   /**
    * Zooms the display. Value larger than one means zoom in.
    * Corresponds to setZoomLevel( factor * getZoomLevel() )
    *
    * @param factor The factor to zoom in.
    * @return The resulting zoom level. 
    */
   virtual double zoom(double factor);
   
   /**
    * Zooms in so that a specific point at the screen is located at a 
    * a certain coordinate.
    *    
    * @param factor The factor to zoom in. Value larger than one means
    *               zoom in.
    * @param zoomCoord The coordinate to zoom in to.
    * @param zoomPoint The point on the screen where the zoomCoord should
    *                  be located.
    * @return The resulting zoom level.
    */
   virtual double zoom(double factor, 
                       const WFAPI::WGS84Coordinate& zoomCoord,
                       const WFAPI::ScreenPoint& zoomPoint);

   /**
    * Zooms the display to the supplied rectangle defined by the
    * two screen points.
    *   
    * @param oneCorner One of the diagonal corners to zoom to.
    * @param otherCorner The other diagonal corner to zoom to.
    */
   virtual void setScreenBox(const WFAPI::ScreenPoint& oneCorner,
                             const WFAPI::ScreenPoint& otherCorner);

   /**
    * Zooms the display to the supplied world box, i.e. rectangle defined
    * by the two real world coordinates.
    *
    * @param oneCorner One of the diagonal world coordinate corners to zoom to.
    * @param otherCorner The other diagonal world coordinate corner to zoom to.
    */
   virtual void setWorldBox(const WFAPI::WGS84Coordinate& oneCorner,
                            const WFAPI::WGS84Coordinate& otherCorner) ;

   /**
    * Method to indicate if the map is moving continously in order to be able
    * to redraw faster.
    *   
    * Should be called with the value true during movement of the map because
    * of a key being held pressed. Should be called with the value false when
    * movement ends. Can be used to draw the map quicker and uglier when moving
    * and then drawing a nice one when it stops.
    * 
    * @param moving True if moving, false if not.
    */
   virtual void setMovementMode(bool moving);

private:
   MapProjectionAdapter* m_projectionAdapter;
};

#endif /* _MAPLIBOPERATIONHANDLER_H_ */
