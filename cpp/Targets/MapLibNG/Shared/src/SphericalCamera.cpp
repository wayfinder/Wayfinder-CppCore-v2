/*
Copyright (c) 1999 - 2010, Vodafone Group Services Ltd
All rights reserved.

Redistribution and use in source and binary forms, with or without modification, are permitted provided that the following conditions are met:

    * Redistributions of source code must retain the above copyright notice, this list of conditions and the following disclaimer.
    * Redistributions in binary form must reproduce the above copyright notice, this list of conditions and the following disclaimer in the documentation and/or other materials provided with the distribution.
    * Neither the name of the Vodafone Group Services Ltd nor the names of its contributors may be used to endorse or promote products derived from this software without specific prior written permission.

THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS" AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE ARE DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT HOLDER OR CONTRIBUTORS BE LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
*/


#include "SphericalCamera.h"
#include "MapProjection/MapProjection.h"
#include "MC2Point.h"
#include "FixedPointType.h"
#include "Utility.h"
#include "Vector3d.h"
#include "Vector3dMath.h"
#include "TrigUtil.h"


SphericalCamera::vec_t
SphericalCamera::rotateVecAroundZ( const vec_t& v, double angle )
{
   return vec_t( v.x * cos( angle ) - v.y * sin( angle ),
                 v.x * sin( angle ) + v.y * cos( angle ) );
}

SphericalCamera::SphericalCamera(MapProjection& projection)
   : Camera(projection)
{
   m_arcAngle = 90.0f;
   m_fovAngle = 90.0f;
   m_horizonDistance = 1.5f;
}

void SphericalCamera::updateState()
{
   calculateArcRadius();
   calculateWorldOffset();
   calculateBoundingBox();
}

void SphericalCamera::calculateArcRadius()
{
   m_arcRadius = getArcRadiusMC2();
}

SphericalCamera::vec_t SphericalCamera::getCameraPoint()
{
   const double arcAngle = TrigUtil::deg2rad(m_arcAngle);
   const double arcRadius = m_arcRadius;

   vec_t camPoint;
   // Since we want the camera to move "backwards" along the arc
   // as the angle decreases, we negate the cos factor of the arcangle.
   camPoint.x = -cos( arcAngle ) * arcRadius;
   camPoint.y = sin( arcAngle ) * arcRadius;

   return camPoint;
}

void SphericalCamera::calculateBoundingBox()
{
   const double arcAngle = TrigUtil::deg2rad(m_arcAngle);
   const double aspectRatio = m_projection.getAspectRatio();

   // fovAngleY is the angle of view extent between the top
   // and the bottom of the screen.
   const double fovAngleY = TrigUtil::deg2rad(m_fovAngle);
   
   // This variable is the maximum allowed distance for the bounding box
   // to extent from the focus point. I.e. the distance where we will
   // put a horizon.
   const double horizonDistanceMC2 = getScreenHeightMC2() * m_horizonDistance;

   // std::cout << std::setiosflags( std::ios::fixed );

   vec_t camPoint = getCameraPoint();

   vec_t centerVec = camPoint;
   centerVec.x *= -1.0;
   centerVec.y *= -1.0;

   vmath::normalize( centerVec );

   // We construct vectors that represent the field of view.
   // The backDirVec can be described as shooting out from the
   // bottom of the screen, and will always be behind the center point.
   vec_t backDirVec = rotateVecAroundZ( centerVec, -fovAngleY / 2 );
   // The frontDirVec can be described as shooting out from the
   // top of the screen, and will always be in front of the center point.
   vec_t frontDirVec = rotateVecAroundZ( centerVec, fovAngleY / 2 );

   // We want to know where these vectors intersect with the map,
   // so we calculate the slope of each vector and insert the camera point
   // to get line equations instead of vectors.
   //
   // Line equation:  y = K * x + M
   //
   double frontK = frontDirVec.y / frontDirVec.x;

   double backK = backDirVec.y / backDirVec.x;

   double frontM = camPoint.y - frontK * camPoint.x;

   double backM = camPoint.y - backK * camPoint.x;

   // We have now found the unknowns K and M in the line equations.
   // We can now find their intersection points on the map (when y = 0).
   // 0 = K * x + M -> x = -M / K

   double frontX = -frontM / frontK;
   double backX = -backM / backK;

   // frontX now denote the distance on the map between the center point
   // and the frontmost visible point of the map.
   // Similarly, backX denote the distance on the map between the center point
   // and the the backmost visible point of the map.

   // If the arc angle is too low, the upper vector will
   // not hit the ground, but point to the sky.
   // In this case, using frontX would yield wrong behaviour.
   bool invalidFrontX = frontX < 0.0;

   double clampedFrontX = 0.0;
   if( invalidFrontX ) {
      // Since frontX is invalid, we use horizonDistanceMC2 instead of
      // frontX everywhere.
      clampedFrontX = horizonDistanceMC2;
      frontX = horizonDistanceMC2;
   } else {
      // frontX is valid, but we don't want to extend our bounding
      // box further than the distance to the horizon.
      clampedFrontX = std::min( frontX, horizonDistanceMC2 );
   }

   // Calculate the horizon height expressed in normalized screen
   // height units

   /*
    *                               -------+-------
    *                         -----/       |       \-----
    *                      --/             |             \--
    *                   --/                |                \--
    *                 -/                   |                   \-
    *              --/                     |                     \--
    *       Camera/                        |                        \
    *           ---                        |                         \-
    *          / |\\---                    |                           \
    *         /  |  \-\----                |                            \
    *       -/   |    \  \-----            |                             \-
    *      /     |     \-   \--\---        |                               \
    *      |     | alpha \-    \-- \---    |                               |
    *     /      |         \ beta \--  \---|                                \
    *    /       |          \-       \--   \---                              \
    *   /        |            \-        \--|   \--                            \
    *   | camPoint.y            \          \--    \---                        |
    *  /         |               |-        |  \--     \---                     \
    *  |         |              /  \-      |     \--      \---                 |
    *  |         |    arcAngle/      \     |        \--       \---             |
    *  |         |           |        \-   |           \--        \---          \
    * |          |---        |          \- |              \--         \---      |
    * |          |  |        \            \|                 \-           \-    |
    * +----------+-------------------------o------------------------------------+
    *            <---------camPoint.x-----> <-----horizDist--->
    *                                       <----------------frontX---------->
    *
    * In order to calculate the height of the horizon, we need the angle
    * denoted beta in the illustration above.
    */
   if( clampedFrontX != frontX || invalidFrontX ) {

      // We begin by calculating alpha + beta since we know two sides of
      // that triangle. camPoint.y is the height, and camPoint.x + horizDist
      // is the width. We can then get the angle by using the arc tangent.
      // Note that camPoint.x is negative.
      double alphaPlusBeta =
         atan((horizonDistanceMC2 - camPoint.x) / camPoint.y);

      // Since we know the arcAngle and that the triangle is right, we can
      // get the alpha angle through the sum of triangles:
      // alpha + pi/2 + arcAngle = pi
      // => alpha = pi - pi/2 - arcAngle = pi/2 - arcAngle
      double alphaRadians = M_PI / 2.0f - arcAngle;

      // Aquiring the beta angle is now quite straight forward.
      double betaRadians = alphaPlusBeta - alphaRadians;



      //                                                         /+    ^
      //                                                       /- |    |
      //                                                     /-   |    |
      //                                                   /-     |    |
      //                                                 /-       |    |
      //                                               /-         |    |
      //                                             /-           |    |
      //                                           /-             |    |
      //                                         /-               |    |
      //                                       /-                 |    |
      //                                     /-                   |    |
      //                                   /-                     |    |
      //                                 /-                       |    |
      //                               /-                        /+^   |A
      //                             /-                      /--- ||   |
      //                           /-                    /---     ||   |
      //                         /-                  /---         ||   |
      //                       /-                 /--             ||   |
      //                     /-               /---                ||   |
      //                   /-  \-         /---                    ||B  |
      //                 /-      \ theta--                        ||   |
      //               /-         \/--                            ||   |
      //             /-        /---\                              ||   |
      //           /-      /+--     |                             ||   |
      //         /-     /-- |       |                             ||   |
      //       /-   /---     \      |                             ||   |
      //     /- /---     beta|      |                             ||   |
      //   //---              \     /                             |\/ \/
      // +--------------------+-----------------------------------+
      // <--------------------irrelevant side length C----------->
      //
      // In the illustration above, the following applies:
      //
      // tan(theta) = A / C
      //
      // and similarly:
      //
      // tan(beta) = B / C
      //
      // We can eliminate C and use the fact that:
      //
      // tan(theta)    A
      // ---------- = ---
      // tan(beta)     B
      //
      // Consider the line A to depict the top half of the screen.
      // Then the angle theta will be fovAngle / 2.
      // The line A will have the length 0.5 in normalized screen
      // coordinates.
      //
      // The angle beta is the angle we previously calculated.
      // What we want to calculate is the length B, or actually
      // the length A - B, since this is the desired height
      // of the horizon in normalized screen coordinates.
      //
      // We begin by solving B:
      //       tan(beta)
      // B = ------------ * A
      //      tan(theta)
      //
      double B = (tan(betaRadians) / tan(fovAngleY / 2.0f)) * 0.5f;

      // The horizon height is then simply A - B.
      m_horizonHeight = 0.5 - B;

   } else {
      // The horizon is currently not visible.
      m_horizonHeight = 0.0f;
   }

   //
   // The widest part of the viewable area will be the area at the
   // top of the screen, so we will calculate the distance to that
   // point.
   // To get this distance, we use Pythagoras theorem.
   // We know that the height of the triangle is camPoint.y,
   // and the width is the sum of camPoint.x and clampedFrontx.
   double heightAlongMap = -camPoint.x + clampedFrontX;

   double distance = sqrt(heightAlongMap * heightAlongMap +
                          camPoint.y * camPoint.y);
   
   // Using the distance and the angle, the width at that distance
   // is easily calculated using the tangens function.
   double halfWidth = sin( fovAngleY / 2.0 ) * distance * aspectRatio;

   // halfWidth = (clampedFrontX - backX) / 2.0f;
   
   double cosLat = m_projection.getCosLat();

   //
   // Now we need to compensate for rotation. We create vectors
   // pointing from origo to the four corners of the desired bounding box.
   // The vectors are first expressed in the current coordinate system,
   // then they are offset to adjust the position of the focal point
   // on screen. Lastly they are rotated to reflect the current
   // rotation of the map (the angle that north points to).
   const float rotatedAngle = TrigUtil::deg2rad(m_projection.getAngle());

   vec_t offsetVecs[] = { vec_t( -halfWidth, clampedFrontX ),
                          vec_t( halfWidth, clampedFrontX ),
                          vec_t( -halfWidth, backX ),
                          vec_t( halfWidth, backX ) };

   const int NUM_OFFSETS = sizeof( offsetVecs ) / sizeof( vec_t );

   for( int i = 0; i < NUM_OFFSETS; i++ ) {
      // offsetVecs[ i ].x*=0.95;
      // offsetVecs[ i ].y*=0.95;
      offsetVecs[ i ].y += m_backOffset;
      offsetVecs[ i ] = rotateVecAroundZ( offsetVecs[i], -rotatedAngle );
   }

   // We create a center coordinate for the bounding box, then
   // create corners by adding the vectors.
   MC2Coordinate c(static_cast<int>(m_worldOffsetY),
                   static_cast<int>(m_worldOffsetX));

   MC2Coordinate corners[NUM_OFFSETS];

   for( int i = 0; i < NUM_OFFSETS; i++ ) {
      corners[i] = MC2Coordinate( c.lat + static_cast<int>( offsetVecs[i].y ),
                                  c.lon + static_cast<int>( offsetVecs[i].x /
                                                            cosLat ) );
   }

   // We create a bounding box using two of the corners
   MC2BoundingBox bbox( corners[0], corners[1] );

   // And then we update the bounding box to contain the last
   // two corners.
   bbox.update( corners[2] );
   bbox.update( corners[3] );


   m_boundingBox = bbox;
}

// double MapProjection::getHorizonHeight() const

float SphericalCamera::getArcRadiusMC2()
{
   const float fovAngle = getFOV();

   const float halfHeight = getScreenHeightMC2() / 2.0f;
   
   const float arcRadius = halfHeight / tan( TrigUtil::deg2rad( fovAngle / 2.0 ) );

   return arcRadius;
}

void SphericalCamera::calculateWorldOffset()
{
   const vec_t camPoint = getCameraPoint();
   const double arcAngle = TrigUtil::deg2rad( getArcAngle() );
   const double fovAngle = TrigUtil::deg2rad( getFOV() );
   const MC2Coordinate& center = m_projection.getCenter();

   m_worldOffsetX = center.lon;
   m_worldOffsetY = center.lat;


   // Calculations of the backOffset.
   // The backOffset is the distance in MC2 space we need to
   // move the camera, in order for the focal point to have the
   // desired position.

   //                                                         /+    ^
   //                                                       /- |    |
   //                                                     /-   |    |
   //                                                   /-     |    |
   //                                                 /-       |    |
   //                                               /-         |    |
   //                                             /-           |    |
   //                                           /-             |    |
   //                                         /-               |    |
   //                                       /-                 |    |
   //                                     /-                   |    |
   //                                   /-                     |    |
   //                                 /-                       |    |
   //                               /-                        /+^   |A
   //                             /-                      /--- ||   |
   //                           /-                    /---     ||   |
   //                         /-                  /---         ||   |
   //                       /-                 /--             ||   |
   //                     /-               /---                ||   |
   //                   /-  \-         /---                    ||B  |
   //                 /-      \ theta--                        ||   |
   //               /-         \/--                            ||   |
   //             /-        /---\                              ||   |
   //           /-      /+--     |                             ||   |
   //         /-     /-- |       |                             ||   |
   //       /-   /---     \      |                             ||   |
   //     /- /---     beta|      |                             ||   |
   //   //---              \     /                             |\/ \/
   // +--------------------+-----------------------------------+
   // <--------------------irrelevant side length C----------->
   //
   // In the illustration above, the following applies:
   //
   // tan(theta) = A / C
   //
   // and similarly:
   //
   // tan(beta) = B / C
   //
   // We can eliminate C and use the fact that:
   //
   // tan(theta)    A
   // ---------- = ---
   // tan(beta)     B
   //
   // Consider the line A to depict the top half of the screen.
   // Then the angle theta will be fovAngle / 2.
   // The line A will have the length 0.5 in normalized screen
   // coordinates.
   //
   // The line B is the desired position m_focalPointPosition
   // (actually 0.5 - m_focalPointPosition) where we want the
   // focal position to appear.
   // To find what this distance represents on the map, we first
   // find the angle beta:
   //                  B
   //  beta = arctan( --- * tan(theta) )
   //                  A
   //
   float beta = atan(((0.5 - m_focalPointPosition) / 0.5) *
                     tan(fovAngle / 2.0f));
   //
   // 
   //                                                  -----------+
   //                                         --------/           |
   //                                   -----/                    |
   //                               ---/                          |
   //                           ---/                              |
   //                        --\                                  |
   //                     --/  |\\                                |
   //                   -/     | \-\                              |
   //                --/       |  \ -\                            |
   //              -/          | rho (phi + beta)                 |
   //             /            |    \   -\                        |
   //           -/             |     \    \                       |
   //          /               | phi  \beta-\                     |
   //        -/                |       \     -\                   |
   //       /                  |        \      -\                 |
   //      /                   |        |        -\               |
   //     /            camPoint.y        \         -\             |
   //     |                    |          \          -\           |
   //    /                     |           \           \          |
   //   /                      |            \           -\        |
   //   |                      |             \            -\      |
   //  /                       |              \             -\    |
   //  |                       |               \       arcAngle\  |
   //  |                       |                \               -\|
   //  |-----------------------+-----------------\----------------+
   //                          <-----diffDist----><---backOffs---->
   //                          <-----------camPoint.x------------->
   //
   // To get backOffs, we can use diffDist. To get difffDist, we need
   // the angle rho (phi + beta). Since the triangle is right and
   // the last angle is arcAngle, rho is easy to get using the sum
   // of angles:
   const float rho = M_PI / 2.0f - arcAngle;
   
   // phi is now simply rho - beta.
   const float phi = rho - beta;

   // And diffDist can now be calculated from phi and camPoint.y
   const float diffDist = tan( phi ) * camPoint.y;

   // The offset is now the difference between camPoint.x and diffDist.
   // Note that camPoint.x is negative.
   const float offset = -camPoint.x - diffDist;

   m_backOffset = static_cast<int>(offset);
}

float SphericalCamera::getScreenHeightMC2()
{
   static const double mc2scaletometer = 6378137.0*2.0*
      3.14159265358979323846 / 4294967296.0;
   static const float meterstomc2scale = 1.0 / mc2scaletometer;

   const float scale = m_projection.getPixelScale() * meterstomc2scale;

   const MC2Point screenSize( m_projection.getScreenSize() );

   return screenSize.getY() * scale;
}
