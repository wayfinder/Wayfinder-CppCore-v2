/*
  Copyright (c) 1999 - 2010, Vodafone Group Services Ltd
  All rights reserved.

  Redistribution and use in source and binary forms, with or without modification, are permitted provided that the following conditions are met:

  * Redistributions of source code must retain the above copyright notice, this list of conditions and the following disclaimer.
  * Redistributions in binary form must reproduce the above copyright notice, this list of conditions and the following disclaimer in the documentation and/or other materials provided with the distribution.
  * Neither the name of the Vodafone Group Services Ltd nor the names of its contributors may be used to endorse or promote products derived from this software without specific prior written permission.

  THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS" AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE ARE DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT HOLDER OR CONTRIBUTORS BE LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
*/

#ifndef WFAPI_MAPOPERATIONINTERFACE_H
#define WFAPI_MAPOPERATIONINTERFACE_H

#include "PALMachineTypes.h"

namespace WFAPI {

// Forward declaration.
class ScreenPoint;
class WGS84Coordinate;

/**
 * Interface that supports various operations on a map. By using this
 * class you can alter the visual state of the map by moving, zooming
 * and rotating it.
 *
 * It also supports operations such as coordinate projection. The class
 * uses only uses WGS84 coordinates. 
 *
 * Definitions:
 * ScreenPoints are relative to the upper left corner of the screen.
 * I.e. ScreenPoint(10,10) refers to the position 10 pixels down and 
 * 10 pixels to the right of the upper left corner of the screen.
 */
class MapOperationInterface {
public:
   
   /**
    * Virtual destructor.
    */
   virtual ~MapOperationInterface(){};
      
   /**
    * Returns the center coordinate of the map in WGS84 coordinates.
    *
    * @return The center coordinate of the map.
    */
   virtual WGS84Coordinate getCenter() const = 0;

   /**
    * Transforms a real world coordinate to a screen coordinate.
    *   
    * @param point The point parameter will be set to the screen point
    *              that the world coordinate is shown at.
    * @param coord The world coordinate.
    */
   virtual void worldToScreen(ScreenPoint& point,
                              const WGS84Coordinate& coord) const = 0;
   
   /**
    * Transforms the point on the screen to a real world
    * coordinate.
    *
    * @param coord The coord parameter will be set to the world coordinate
    *              that the screen point is at.
    * @param point The screen point.
    */
   virtual void screenToWorld(WGS84Coordinate& coord,
                              const ScreenPoint& point) const = 0;
   
   /**
    * Sets the center coordinate to newCenter.
    *
    * @param newCenter The new center coordinate of the map.
    */
   virtual void setCenter(const WGS84Coordinate& newCenter) = 0;
   
   /**
    * Sets the specified point on the screen to the specified
    * world coordinate.
    * 
    * @param newCoord The new world coordinate to move the specified
    *                 screen point to.
    * @param screenPoint The screen point on the screen to set to the
    *                    specified world coordinate.
    */
   virtual void setPoint(const WGS84Coordinate& newCoord,
                         const ScreenPoint& screenPoint ) = 0;

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
   virtual void setPoint(const WGS84Coordinate& newCoord,
                         const ScreenPoint& screenPoint, 
                         double angleDegrees ) = 0;

   /**
    * Moves the screen. Unit pixels.
    *
    * @param deltaX Positive value means the map is moved 
    *               that amount of pixels to the right.
    * @param deltaY Positive value means the map is moved 
    *               that amount of pixels upwards.
    */
   virtual void move(wf_int32 deltaX, wf_int32 deltaY) = 0;
   
   /**
    * Sets the angle to the number of degrees in the
    * parameter. Rotates around the center.
    *
    * @param angleDegrees The angle in degrees to rotate.
    */
   virtual void setAngle(double angleDegrees) = 0;

   /**
    * Sets the angle to the number of degrees in the
    * parameter. Rotates around the rotation point.
    * 
    * @param angleDegrees  Angle in degrees.
    * @param rotationPoint Point to rotate around.
    */
   virtual void setAngle(double angleDegrees,
                         const ScreenPoint& rotationPoint) = 0;

   /**
    * Rotates the display the supplied number of degrees 
    * counter clock wise.
    *
    * @param nbrDeg Amount of degrees to rotate ccw.
    */
   virtual void rotateLeftDeg(wf_int32 nbrDeg) = 0;
   
   /**
    * Returns the current angle in degrees.
    *
    * @return The current angle in degrees of the map heading.
    */
   virtual double getAngle() const = 0;

   /**
    * Sets zoom level. 
    * 
    * @param zoomLevel The new zoom level.
    * @return The zoom level that was set.
    */
   virtual double setZoomLevel(double zoomLevel) = 0;

   /**
    * Returns the current zoom level.
    *
    * @return The zoom level.
    */
   virtual double getZoomLevel() const = 0;

   /**
    * Zooms the display. Value larger than one means zoom in.
    * Corresponds to setZoomLevel( factor * getZoomLevel() )
    *
    * @param factor The factor to zoom in.
    * @return The resulting zoom level. 
    */
   virtual double zoom(double factor) = 0;
   
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
                       const WGS84Coordinate& zoomCoord,
                       const ScreenPoint& zoomPoint) = 0;

   /**
    * Zooms the display to the supplied rectangle defined by the
    * two screen points.
    *   
    * @param oneCorner One of the diagonal corners to zoom to.
    * @param otherCorner The other diagonal corner to zoom to.
    */
   virtual void setScreenBox(const ScreenPoint& oneCorner,
                             const ScreenPoint& otherCorner) = 0;

   /**
    * Zooms the display to the supplied world box, i.e. rectangle defined
    * by the two real world coordinates.
    *
    * @param oneCorner One of the diagonal world coordinate corners to zoom to.
    * @param otherCorner The other diagonal world coordinate corner to zoom to.
    */
   virtual void setWorldBox(const WGS84Coordinate& oneCorner,
                            const WGS84Coordinate& otherCorner)  = 0;

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
   virtual void setMovementMode(bool moving) = 0;

   /**
    * Set if to enable 3d mode for the map. Calling this with true
    * is equal to calling setVariable3dMode(1.0), calling this with
    * false is equal to calling setVariable3dMode(0.0).
    * 
    * @param enabled True if enable 3d, false otherwise (2d).
    */
   virtual void set3dMode(bool enabled) = 0;

   /**
    * Get if 3d mode is enabled.
    * 
    * @return True if 3d mode is enabled, false if otherwise (2d).
    */
   virtual bool get3dMode() const = 0;

   /**
    * Sets the map state to a value between 2D and 3D when using a camera
    * based map renderer (like the OpenGL ES renderer).
    * 
    * Can be used to animate from 2D to 3D (step = 0.0 -> 1.0)
    * and vice versa (step = 1.0 -> 0.0).
    * Calling this function with step == 0.0 is equal to calling
    * set3dMode with false. Calling this function with 1.0 is equal
    * to calling set3dMode with true.
    *
    * @param step The current state between 2D and 3D to use.
    */
   virtual void setVariable3dMode(float step) = 0;
};

} // End of namespace WFAPI

#endif // WFAPI_MAPOPERATIONINTERFACE_H
