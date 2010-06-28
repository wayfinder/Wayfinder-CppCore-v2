/*
Copyright (c) 1999 - 2010, Vodafone Group Services Ltd
All rights reserved.

Redistribution and use in source and binary forms, with or without modification, are permitted provided that the following conditions are met:

    * Redistributions of source code must retain the above copyright notice, this list of conditions and the following disclaimer.
    * Redistributions in binary form must reproduce the above copyright notice, this list of conditions and the following disclaimer in the documentation and/or other materials provided with the distribution.
    * Neither the name of the Vodafone Group Services Ltd nor the names of its contributors may be used to endorse or promote products derived from this software without specific prior written permission.

THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS" AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE ARE DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT HOLDER OR CONTRIBUTORS BE LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
*/


#ifndef _SPHERICALCAMERA_H_
#define _SPHERICALCAMERA_H_

#include "Camera.h"
#include "Vector3d.h"

class SphericalCamera : public Camera {
public:
   SphericalCamera(MapProjection& projection);

   /**
    *    Returns the arc position of the camera.
    *
    *    When in 2d mode, the arc position is 0 degrees.
    *    This means that the camera is centered directly
    *    above the focal point (o), which is the same as
    *    the map position.
    *
    *    For 3d mode, the camera will be moved along the arc to
    *    show a tilted perspective (for example, 45 degrees to
    *    the left in the example diagram below).
    *
    *
    *                 (0 degrees)    
    *                    ___
    *              -----|   |------
    *        ___ -/     |2-d|      \--
    *       \   \       |   |        \-
    *        \3 d\       /_\           |-
    *       -  \_\                      \
    *      /                             \
    *     |                               |
    *    /                                 \
    *    +----------------o-----------------+
    *
    *                                 
    */

private:
   typedef Vector3dBase<FloatStorageType> vec_t;

   /**
    * A helper to rotate a vector around the Z-axis, i.e.
    * only the X and the Y components are modified.
    *
    * @param v The vector that is to be rotated.
    * @param angle The angle to rotate the vector.
    *
    * @return The rotated vector.
    */
   vec_t rotateVecAroundZ( const vec_t& v, double angle );

   vec_t rotateVecAroundY( const vec_t& v, double angle);
      
   /**
    * Returns the position of the camera in the internal
    * coordinate space.
    *
    * @return The camera position
    */
   vec_t getCameraPoint();

   /**
    * Updates the members according to the current state of
    * the MapProjection.
    */
   virtual void updateState();

   /**
    * Calculates and updates the current bounding box.
    */
   void calculateBoundingBox();

   /**
    * Calculates and updates the current arc radius.
    */  
   void calculateArcRadius();

   /**
    * Calculates and updates the current world offset.
    */
   void calculateWorldOffset();

   /**
    * Returns the current height of the screen in MC2 units.
    *
    * @return The screen height in MC2 units.
    */
   float getScreenHeightMC2();

   /**
    * Returns the current radius of the arc in MC2 units.
    *
    * @return The arc radius in MC2 units.
    */
   float getArcRadiusMC2();
};

#endif /* _SPHERICALCAMERA_H_ */
