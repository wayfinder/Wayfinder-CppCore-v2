/*
Copyright (c) 1999 - 2010, Vodafone Group Services Ltd
All rights reserved.

Redistribution and use in source and binary forms, with or without modification, are permitted provided that the following conditions are met:

    * Redistributions of source code must retain the above copyright notice, this list of conditions and the following disclaimer.
    * Redistributions in binary form must reproduce the above copyright notice, this list of conditions and the following disclaimer in the documentation and/or other materials provided with the distribution.
    * Neither the name of the Vodafone Group Services Ltd nor the names of its contributors may be used to endorse or promote products derived from this software without specific prior written permission.

THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS" AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE ARE DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT HOLDER OR CONTRIBUTORS BE LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
*/


#ifndef MAPPROJECTIONADAPTER_H
#define MAPPROJECTIONADAPTER_H

#include "MapProjection.h"

class TileMapHandler;
class MapProjection;

/**
 * This class bridges the gap between the internal drawing
 * representations (2d/3d, plotter/external) in TileMapHandler
 * and straight map operations. It acts as a filter. All operations
 * should go through this.
 *
 * It is also a MapOperationHandler so that it can be the "last stop"
 * along the way of a delegation hierarchy.
 */ 
class MapProjectionAdapter {
public:
   MapProjectionAdapter(TileMapHandler* handler,
                        MapProjection& projection);

   /**
    *   Returns the current bounding box of the map display.
    *   
    */
   const MC2BoundingBox& getBoundingBox() const;

   /**
    * Sets the current bounding box.
    *
    * @param bbox The bounding box to use.
    */
   void setBoundingBox(const MC2BoundingBox& bbox);

   /**
    * @return the current center position of the map.
    */
   const MC2Coordinate& getCenter() const;

   /**
    * Transforms a coordinate to a screenPoint.
    * 
    * @param point The transformed point.
    * @param coord The coord that should be transformed.
    */
   void transform( MC2Point& point,
                   const MC2Coordinate& coord ) const;

   /**
    * Transforms a coordinate to a screenPoint using floating
    * point.
    * 
    * @param x The x-coord of the transformed point.
    * @param y The y-coord of the transformed point.
    * @param coord The coord that should be transformed.
    */
   void transformf(float64& x, float64&y,
                   const MC2Coordinate& coord ) const;

   /**
    * Transforms a screen point to world coordinates.
    * 
    * @coord The transformed coordinate.
    * @point The point to transform into world coordinates.
    */
   void inverseTransform( MC2Coordinate& coord,
                          const MC2Point& point ) const;

   /**
    * Sets the center of the map to a coordinate.
    *
    * @param newCenter The new center to use.
    */
   void setCenter( const MC2Coordinate& newCenter ); 

   /**
    * Moves the map so that the coordinate newCoord lies on the screen
    * position screenPoint.
    *
    * @param newCoord The coordinate to use.
    * @param screenPoint The screen point to lock set the coordinate to.
    */
   void setPoint(const MC2Coordinate& newCoord,
                 const MC2Point& screenPoint );

   /**
    * Moves the map a certain amount of pixels.
    * 
    * @param deltaX The amount to move in x-space.
    * @param deltaY The amount to move in y-space.
    */
   void move(int deltaX, int deltaY);
   
   /**
    *   Sets the specified point on the screen to the
    *   specified coordinate and rotate around that point.
    *
    *   This method call result in the same thing as below, 
    *   but more efficiently implemented:
    *
    *   setPoint( newCoord, screenPoint );
    *   setAngle( angleDegrees, screenPoint );
    *   
    *   
    *   @param newCoord    The new coordinate to move the specified
    *                      point to.
    *   @param screenPoint The point on the screen to set to the
    *                      specified coordinate.
    *   @param angle       The angle in degrees to rotate around 
    *                      the screenPoint.
    */
   void setPoint(const MC2Coordinate& newCoord,
                 const MC2Point& screenPoint, 
                 double angleDegrees );

   /**
    * Sets the current angle of the map to angleDegrees.
    *
    * @param angleDegrees The new angle to use.
    */
   void setAngle(double angleDegrees);

   /**
    * Sets the angle to angleDegrees, rotating around the
    * supplied screen point.
    *
    * @param angleDegrees The angle to use.
    * @param rotationPoint The point to rotate around.
    */
   void setAngle( double angleDegrees,
                  const MC2Point& rotationPoint );
   
   /**
    * Rotates the display the supplied number of degrees to the left.
    *
    * @param nbrDeg The number of degrees to rotate.
    */
   void rotateLeftDeg(int nbrDeg);

   /**
    * @return The current angle of the map.
    */
   inline double getAngle() const {
      return m_projection.getAngle();
   }

   /**
    * Sets the current scale of the map.
    * 
    * @param scale The scale to use for the map.
    */
   inline double setScale( double scale ) {
      return m_projection.setDPICorrectedScale( scale );
   }

   /**
    * @return The current scale of the map.
    */
   inline double getScale() const {
      return m_projection.getDPICorrectedScale();
   }

   /**
    *   Returns the current scale in meters per pixel.
    */
   double getPixelScale() const;

   /**
    *   Refactored to MapProjection member, still
    *   here for external access.
    */  
   double zoom( double factor );
   bool zoom(double factor, double& resultFactor);
   
   
   /**
    *   Refactored to MapProjection member, still
    *   here for external access.
    */  

   double zoom( double factor, 
                const MC2Coordinate& zoomCoord,
                const MC2Point& zoomPoint );
   bool zoom(double factor, 
             const MC2Coordinate& zoomCoord,
             const MC2Point& zoomPoint,
             double& resultFactor);

   /**
    *   Refactored to MapProjection member, still
    *   here for external access.
    */  
   void setPixelBox( const MC2Point& oneCorner,
                     const MC2Point& otherCorner );

   /**
    *   Refactored to MapProjection member, still
    *   here for external access.
    */  
   void setScreenSize(const MC2Point& size);

   /**
    * Sets the current bounding box.
    *
    * @param oneCorner One of the corners in the bounding box to use.
    * @param otherCorner Another corner in the bounding box to use
    *                    (preferrably diagonally opposite to the first one).
    */
   void setWorldBox( const MC2Coordinate& oneCorner,
                     const MC2Coordinate& otherCorner );
   
   /**
    * @return The center point of the screen.
    */
   MC2Point getCenterPoint() const;


   /**
    * Returns the bounding box in pixels - basically defined
    * by the two diagonal points (0, 0) and (screenWidth, screenHeight)
    *
    * @return The bounding box in pixels.
    */ 
   const PixelBox& getPixelBoundingBox() const;

   /**
    * Get the 3D point from the supplied 2D screen point.
    * In case not in 3D mode, the 2D screen point will be returned.
    *
    * @param screenPoint The 2D point.
    * @return The resulting 3D point.
    */
   MC2Point getPoint3DFrom2D( const MC2Point& screenPoint ) const;

   /**
    * Sets the current movement mode.
    *
    * @param moving Set to true if the map is currently being moved.
    */
   void setMovementMode(bool moving);
   
private:
   /**
    * Get 2D point from screen point, i.e. a point for map projection usage. 
    */
   MC2Point getPoint2D( const MC2Point& screenPoint ) const;
   
   TileMapHandler* m_handler;
   MapProjection& m_projection;
};


#endif // MAPPROJECTIONADAPTER_H
