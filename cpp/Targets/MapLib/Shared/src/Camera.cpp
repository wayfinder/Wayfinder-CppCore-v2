/*
Copyright (c) 1999 - 2010, Vodafone Group Services Ltd
All rights reserved.

Redistribution and use in source and binary forms, with or without modification, are permitted provided that the following conditions are met:

    * Redistributions of source code must retain the above copyright notice, this list of conditions and the following disclaimer.
    * Redistributions in binary form must reproduce the above copyright notice, this list of conditions and the following disclaimer in the documentation and/or other materials provided with the distribution.
    * Neither the name of the Vodafone Group Services Ltd nor the names of its contributors may be used to endorse or promote products derived from this software without specific prior written permission.

THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS" AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE ARE DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT HOLDER OR CONTRIBUTORS BE LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
*/


#include "Camera.h"
#include "MapProjection.h"
#include "FixedPointType.h"


Camera::Camera( MapProjection& projection ) :
   m_projection( projection ),
   m_zNear(0.02f),
   m_zFar(32000.0f)
{
   m_worldOffsetX = 0;
   m_worldOffsetY = 0;
   m_backOffset = 0;
   m_arcRadius = 0.0f;
   m_arcAngle = 0.0f;
   m_fovAngle = 0.0f;
   m_horizonHeight = 0.0f;
   m_horizonDistance = 1.5f;
   m_focalPointPosition = 0.5f;

   m_2DMode.arcAngle = 90.0f;
   m_2DMode.focalPointPosition = 0.5f;
   m_2DMode.fov = 45.0f;

   // m_3DMode.arcAngle = 22.5f;
   // m_3DMode.focalPointPosition = 0.3f;
   // m_3DMode.fov = 45.0f;
   m_3DMode.arcAngle = 40.0f;
   m_3DMode.focalPointPosition = 0.2f;
   m_3DMode.fov = 90.0f;

}

float Camera::getArcRadius() const
{
   return m_arcRadius;
}

float Camera::getArcAngle() const
{
   return m_arcAngle;
}

float Camera::getWorldAngle() const
{
   return m_projection.getAngle();
}

float Camera::getFOV() const
{
   return m_fovAngle;
}

float Camera::getHorizonHeight() const
{
   return m_horizonHeight;
}

void Camera::projectionUpdated()
{
   updateState();
}

MC2BoundingBox Camera::getBoundingBox() const
{
   return m_boundingBox; 
}

void Camera::setArcAngle(float arcAngle)
{
   float prev = m_arcAngle;
   
   m_arcAngle = arcAngle;
   
   if(m_arcAngle < 1.0f) {
      m_arcAngle = 1.0f;
   } else if(m_arcAngle > 90.0f) {
      m_arcAngle = 90.0f;
   }

   if(prev != m_arcAngle) {
      m_projection.cameraUpdated();      
   }
}

void Camera::setFOV(float fovAngle)
{
   if(fovAngle != m_fovAngle) {
      m_fovAngle = fovAngle;
      m_projection.cameraUpdated();      
   }
}

int Camera::getBackOffset() const
{
   return m_backOffset; 
}

void Camera::setFocalPointPosition( float normPos )
{
   float prev = m_focalPointPosition;
   
   m_focalPointPosition = normPos;

   if( m_focalPointPosition < 0.0f ) {
      m_focalPointPosition = 0.0f;
   } else if( m_focalPointPosition > 1.0f ) {
      m_focalPointPosition = 1.0f;
   }

   if(prev != m_focalPointPosition) {
      m_projection.cameraUpdated();      
   }
}

float Camera::getFocalPointPosition() const
{
   return m_focalPointPosition; 
}

MC2Coordinate Camera::getFocalCoordinate() const
{
   return m_projection.getCenter();
}

CameraParameters Camera::getCameraParams() const
{
   // The following ASCII art is an example of how you can use the
   // parameters that this function returns. Start by translating
   // the elevation. This will place you at the top of the arc,
   // currently centered at origo.
   //
   //                                       Y
   //                               ---------
   //                         -----/        |
   //                      --/              |
   //                   --/                 |      Point of focus
   //                 -/                    |      /
   //               -/                      |     /
   //              /                        |    /
   //             /                         |   /
   //             |                         |  /
   //            /                          ||/_
   //     Camera |-----------------------(Origo)- - - - - - - - - - - - Z
   //            \                          |
   //             |                         |
   //             \                         |
   //              \                        |
   //               -\                      |
   //                 -\                    |
   //                   --\                 |
   //                      --\              |
   //                         -----\        |
   //                               ---------
   //                                      -Y
   //
   // Now perform the arc rotation. Since we are translated to the elevation
   // offset, this will in effect move us along the arc. In the example
   // depicted here, the camera will be offset along the Z-axis and then
   // rotated along the X-axis (which points outwards from the screen).
   //
   //                                       Y
   //                               ---------
   //                         -----/        |
   //                      --/              |
   //                   --/                 |      Point of focus
   //                 -/                    |      /
   //               -/                      |     /
   //              /                        |    /
   //             /                         |   /
   //             |                         |  /
   //            /                          ||/_
   //            |- - - - - - - - - - - -(Origo)- - - - - - - - - - - - Z
   //            \                      --/ |
   //             |                  --/    |
   //             \                -/       |
   //              \            --/         |
   //               -\       --/            |
   //                 -\  --/               |
   //            Camera -/\                 |
   //                      --\              |
   //                         -----\        |
   //                               ---------
   //                                      -Y
   //
   //
   // After the rotation, the camera will have the desired position
   // depicted above.
   // 
   // After we have positioned the camera on it's arc, we want to
   // offset it's position so that the point of focus of the map can
   // appear on a preset position of the screen instead of always in the
   // middle. This is especially desired in route-mode, when the only
   // interesting information is the information in front of the current
   // position.
   //
   //                                       Y
   //                               ---------
   //                         -----/        |
   //                      --/              |
   //                   --/                 |
   //                 -/                    |
   //               -/                      |
   //              /                        |
   //             /                         |
   //             |                         |
   //            /                          |
   //         -Z |- - - - - - - - - - - -(Origo)- - - - - - - - - - - - Z
   //            \                      --/ |
   //             |                  --/    |   Point of focus
   //             \                -/       |    /
   //              \            --/         |   /
   //               -\       --/            |  /
   //                 -\  --/               ||/_
   //            Camera -/\                 |
   //                      --\              |
   //                         -----\        |
   //                               ---------
   //                                      -Y


   CameraParameters ret;
   MC2Coordinate focal(m_projection.getCenter());
   MC2Point screenSize(m_projection.getScreenSize());
   
   ret.proj.zNear = m_zNear;
   ret.proj.zFar = m_zFar;
   ret.proj.aspectRatio =
      static_cast<float>(screenSize.getX()) /
      static_cast<float>(screenSize.getY());
   ret.proj.FOVDegrees = getFOV();
   
   // The translatedAngle is the orientation angle of the map, i.e. the angle
   // between the "forward" direction and the direction to north.

   // The elevation is the distance that we translate the camera
   // before applying the rotation.
   ret.mv.translationZ =
      -m_projection.getCamera().getArcRadius() * FixedPointHelpers::FXFACTOR;

   // In this calculation context, we have defined the
   // arcPositionDegrees to be 90.0 degrees when looking at the map from
   // directly above. The camera parameters work under the assumption that
   // without any rotation, the map will be seen from directly above. 
   // Because of this, we subtract by 90 degrees to compensate when setting
   // the variable.
   ret.mv.rotationXDeg = m_projection.getCamera().getArcAngle() - 90.0;
   
   ret.mv.translationY = -m_backOffset * FixedPointHelpers::FXFACTOR;

   ret.mv.rotationZDeg = m_projection.getAngle();

   ret.mv.scalingX = m_projection.getCosLat();
   
   ret.mv.worldTranslationX = fixedp(fixedp::RAW, -focal.lon);
   ret.mv.worldTranslationY = fixedp(fixedp::RAW, -focal.lat);

   return ret;
}

int Camera::getDPICorrectionFactor() const
{
   return m_projection.getDPICorrectionFactor(); 
}

float Camera::getZoom() const
{
   return m_projection.getPixelScale(); 
}

void Camera::setHorizonDistance( float distance )
{
   m_horizonDistance = distance; 
}

float Camera::getHorizonDistance()
{
   return m_horizonDistance; 
}

void Camera::setVariable3DMode(float step)
{
   // We only allow the step to be between 0.0 and 1.0,
   // so we clamp it to these values.
   float clampedStep = std::min(step, 1.0f);
   clampedStep = std::max(step, 0.0f);

   // LERP of the 2d and 3d mode parameters
   setArcAngle(m_3DMode.arcAngle * clampedStep +
               m_2DMode.arcAngle * (1.0f - clampedStep));
   setFocalPointPosition(m_3DMode.focalPointPosition * clampedStep +
                         m_2DMode.focalPointPosition * (1.0f - clampedStep));
   setFOV(m_3DMode.fov * clampedStep +
          m_2DMode.fov * (1.0f - clampedStep));
}
