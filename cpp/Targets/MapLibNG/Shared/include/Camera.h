/*
Copyright (c) 1999 - 2010, Vodafone Group Services Ltd
All rights reserved.

Redistribution and use in source and binary forms, with or without modification, are permitted provided that the following conditions are met:

    * Redistributions of source code must retain the above copyright notice, this list of conditions and the following disclaimer.
    * Redistributions in binary form must reproduce the above copyright notice, this list of conditions and the following disclaimer in the documentation and/or other materials provided with the distribution.
    * Neither the name of the Vodafone Group Services Ltd nor the names of its contributors may be used to endorse or promote products derived from this software without specific prior written permission.

THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS" AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE ARE DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT HOLDER OR CONTRIBUTORS BE LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
*/


#ifndef _CAMERA_H_
#define _CAMERA_H_

#include "MC2BoundingBox.h"
#include "Vector3d.h"
#include "CameraParameters.h"

class MapProjection;

/**
 * Camera
 *
 * Describes a 3D camera. The camera is placed along a sphere around a
 * focal coordinate in the world.
 *
 *                              --------+--------
 *                       ------/        |        \------
 *                   ---/               |               \---
 *                --/                   |                   \--
 *             CAM--                    |                      \--
 *           -/ |-\ \---                |                         \-
 *         -/   |  -\   \---            |                           \-
 *        /     |    -\     \----       |                             \
 *      -/       \     -\        \---   |                              \-
 *     /         |       -\          \--+-                               \
 *    /          |         \            | \---                            \
 *   /           |          -\          |     \---                         \
 *   |           |            -\        |         \----                    |
 *  /            |              -\      |              \---                 \
 *  |             \               -\    |                  \----            |
 * /              |                 -\  |                       \---         \
 * |              |                   -\|                           \---     |
 * +-------------------------------------------------------------------------+
 *                                 FOCAL POINT
 */

class Camera {
public:
   virtual ~Camera() {}
   
   /**
    * Creates a new camera with the supplied projection as
    * parent.
    * 
    * @param projection The projection that the camera should use.
    */ 
   explicit Camera( MapProjection& projection );

   /**
    * Returns the arc radius of the camera projection.
    * 
    * @ret The arc radius of the camera.
    */
   float getArcRadius() const;

   /**
    * Returns the focal world coordinate.
    *
    * @return The focal coordinate the camera is observing.
    */
   MC2Coordinate getFocalCoordinate() const;

   /**
    * Returns the arc angle of the camera.
    *
    * @return The arc angle of the camera.
    */ 
   float getArcAngle() const;

   /**
    * Sets the arc angle of the camera. Input values are
    * clamped between 90.0 and 1.0.
    * 
    * @param arcAngle The supplied arcAngle.
    */ 
   void setArcAngle(float arcAngle);

   /**
    * Sets the distance to the horizon.
    *
    * @param distance The distance expressed as a factor of the screen height.
    */ 
   void setHorizonDistance( float distance );

   /**
    *   Returns the distance to the horizon.
    */
   float getHorizonDistance();
   
   /**
    * Returns the angle between the current "forward" direction and
    * the direction to north.
    *
    * @return The angle between "forward" and north.
    */
   float getWorldAngle() const;

   /**
    * Returns the current Field Of View. I.e the angle between the top
    * and bottom of the screen.
    *
    * @return The current Field Of View in degrees.
    */
   float getFOV() const;

   /**
    * Sets the current Field of View.
    *
    * @param fovAngle The field of view angle to use, in degrees.
    */
   void setFOV(float fovAngle);

   /**
    * Returns the current height of the horizon. Expressed in normalized
    * screen coordinates between 0.0 and 1.0 (the whole screen).
    *
    * @return The height of the horizon.
    */
   float getHorizonHeight() const;

   /**
    * Notifies the camera to update it's state according to it's MapProjection.
    */
   void projectionUpdated();

   /**
    * Returns the distance the camera is to be moved backwards.
    *
    * @return The distance the camera is to be moved backwards, in MC2-space.
    */
   int getBackOffset() const;

   /**
    * Returns the bounding box that non-rendering
    * parties can use to determine map fetching
    * etc.
    *
    * @return The current bounding box.
    */
   MC2BoundingBox getBoundingBox() const;

   /**
    * Sets the focal point of the camera to a normalized screen coordinate.
    * If normPos is 0.0, the focal point will be positioned at the bottom
    * of the screen. If normPos is 1.0, the focal point will be positioned at
    * the top of the screen.
    *
    * @param normPos The desired position for the focal point on screen.
    */
   void setFocalPointPosition( float normPos );

   /**
    * Gets the current position of the focal point of the camera. Expressed
    * in a normalized screen coordinate. 0.0 is the bottom of the screen and
    * 1.0 is the top of the screen.
    *
    * @return The current position of the focal point on screen.
    */
   float getFocalPointPosition() const;

   /**
    * Returns the necessary parameters for setting up a matrix transforming
    * 3D renderer. See @CameraParameters for more information.
    */ 
   CameraParameters getCameraParams() const;

   /**
    * Sets the linear interpolated state between 2d and 3d mode.
    * 0.0 means 2d mode and 1.0 means 3d mode. This can be used to animate
    * between 2d and 3d mode.
    *
    * @param step Which state the camera should be in. Between 2d and 3d mode.
    */
   void setVariable3DMode(float step);
   
   int getDPICorrectionFactor() const;

   float getZoom() const;
   
protected:
   MapProjection& m_projection;
   
   float m_fovAngle;
   int m_backOffset;
   int m_worldOffsetX;
   int m_worldOffsetY;
   const float m_zNear;
   const float m_zFar;
   float m_arcRadius;
   float m_arcAngle;
   float m_worldAngle;
   float m_horizonHeight;
   float m_horizonDistance;
   float m_focalPointPosition;
   MC2BoundingBox m_boundingBox;
   CameraParameters m_camParams;

   struct FixedState {
      float arcAngle;
      float focalPointPosition;
      float fov;
   };

   FixedState m_2DMode;
   FixedState m_3DMode;
   
private:
   virtual void updateState() = 0;
};

#endif /* _CAMERA_H_ */
