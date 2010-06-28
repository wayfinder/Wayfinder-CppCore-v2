/*
Copyright (c) 1999 - 2010, Vodafone Group Services Ltd
All rights reserved.

Redistribution and use in source and binary forms, with or without modification, are permitted provided that the following conditions are met:

    * Redistributions of source code must retain the above copyright notice, this list of conditions and the following disclaimer.
    * Redistributions in binary form must reproduce the above copyright notice, this list of conditions and the following disclaimer in the documentation and/or other materials provided with the distribution.
    * Neither the name of the Vodafone Group Services Ltd nor the names of its contributors may be used to endorse or promote products derived from this software without specific prior written permission.

THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS" AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE ARE DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT HOLDER OR CONTRIBUTORS BE LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
*/

#include "MapProjection/MapProjectionAdapter.h"
#include "TileMapHandler.h"
#include "PlotterAdapter3d.h"
#include "ExternalRenderer.h"


MapProjectionAdapter::MapProjectionAdapter(TileMapHandler* handler,
                                               MapProjection& projection) :
   m_handler(handler),
   m_projection(projection)
{
   
}

void MapProjectionAdapter::setScreenSize(const MC2Point& size)
{
   m_projection.setScreenSize( size );
}

const MC2BoundingBox& MapProjectionAdapter::getBoundingBox() const
{
   return m_projection.getBoundingBox();
}

double MapProjectionAdapter::getPixelScale() const
{
   return m_projection.getPixelScale();
}

MC2Point
MapProjectionAdapter::getCenterPoint() const
{
   isab::Rectangle sizePixels;
   m_handler->getMapSizeFromRenderer( sizePixels );

   MC2Point centerPoint = MC2Point( sizePixels.getWidth() / 2,
                                    sizePixels.getHeight() / 2 );

   return centerPoint;
}

MC2Point 
MapProjectionAdapter::getPoint3DFrom2D( const MC2Point& screenPoint ) const
{
   MC2Point point3D = screenPoint;
   
   if ( m_handler->getDrawingContext() == DrawingContextProxy::PLOTTER &&
        m_handler->get3dMode() )
   {
      // Get the 2D point corresponding to the 3d point.
      point3D = m_handler->m_plotter3d->transform2DTo3D( point3D );
   }

   return point3D;
}

MC2Point 
MapProjectionAdapter::getPoint2D( const MC2Point& screenPoint ) const
{
   MC2Point point2D = screenPoint;
   
   if ( m_handler->getDrawingContext() == DrawingContextProxy::PLOTTER &&
        m_handler->get3dMode() )
   {
      // Get the 2D point corresponding to the 3d point.
      point2D = m_handler->m_plotter3d->transform3DTo2D_slow( point2D );
   }

   return point2D;
}

void 
MapProjectionAdapter::transform( MC2Point& point,
                                 const MC2Coordinate& coord ) const 
{
   if ( m_handler->getDrawingContext() == DrawingContextProxy::PLOTTER) {
      m_projection.transformPointInternalCosLat( point, coord );
      
      if(m_handler->get3dMode()) {
         // Transform 2d point to 3d.
         point = m_handler->m_plotter3d->transform2DTo3D( point );
      }
   } else if(m_handler->getDrawingContext() == DrawingContextProxy::EXTERNAL) {
      float x, y, z;
   
      m_handler->m_extRenderer->transformf(coord.lon, coord.lat,
                                           &x, &y, &z);

      point.getX() = static_cast<int>(x);
      point.getY() = static_cast<int>(y);
   }
}

void MapProjectionAdapter::transformf(float64& x, float64&y,
                                      const MC2Coordinate& coord ) const
{
   if ( m_handler->getDrawingContext() == DrawingContextProxy::PLOTTER) {
      m_projection.transformPointInternalCosLatf( x, y, coord );
      
      if( m_handler->get3dMode() ) {
         // Transform 2d point to 3d.
         MC2Pointf p = {x, y};
         MC2Pointf tmp;
      
         tmp = m_handler->m_plotter3d->transform2DTo3Df( p );
         x = tmp.x;
         y = tmp.y;         
      }
   } else if(m_handler->getDrawingContext() == DrawingContextProxy::EXTERNAL) {
      float outX, outY, outZ;
      
      m_handler->m_extRenderer->transformf(coord.lon, coord.lat,
                                           &outX, &outY, &outZ);

      x = outX;
      y = outY;
   }
}

void 
MapProjectionAdapter::inverseTransform( MC2Coordinate& coord,
                                        const MC2Point& point ) const 
{
   if(m_handler->getDrawingContext() == DrawingContextProxy::PLOTTER ) {
      m_projection.inverseTranformUsingCosLat( coord, getPoint2D( point ) );
   } else if(m_handler->getDrawingContext() != DrawingContextProxy::NONE ) {
      m_handler->m_extRenderer->inverseTransform(coord, point);
   }
}

const MC2Coordinate& 
MapProjectionAdapter::getCenter() const 
{
   if ( m_handler->get3dMode() &&
        m_handler->getDrawingContext() == DrawingContextProxy::PLOTTER )
   {
      // Going to 2D from 3D. Keep center coordinate.
      MC2Coordinate centerCoord;
      inverseTransform( centerCoord, getCenterPoint() );

      const_cast<TileMapHandler*> (m_handler)->m_tmpCoord = centerCoord;
      return m_handler->m_tmpCoord;
   } else {
      return m_projection.getCenter();
   }
}

void 
MapProjectionAdapter::setCenter( const MC2Coordinate& newCenter ) 
{
   if ( m_handler->get3dMode() &&
        m_handler->getDrawingContext() == DrawingContextProxy::PLOTTER )
   {
      m_projection.setPoint( newCenter,
                             getPoint2D( getCenterPoint() ) );
   } else {
      m_projection.setCenter( newCenter );
   }
}

void 
MapProjectionAdapter::setPoint(const MC2Coordinate& newCoord,
                               const MC2Point& screenPoint ) 
{
   if ( m_handler->getDrawingContext() == DrawingContextProxy::PLOTTER )
   {
      if (m_handler->get3dMode()) {
         m_projection.setPoint( newCoord,
                                getPoint2D( screenPoint ) );
      } else {
         m_projection.setPoint( newCoord, screenPoint );         
      }
   } else if (m_handler->getDrawingContext() != DrawingContextProxy::NONE ){
      MC2Coordinate newCenter;
      
      m_handler->m_extRenderer->calcOffsetCenterCoord(screenPoint, newCoord,
                                                      newCenter);
      

      m_projection.setCenter(newCenter);
   }
}

void 
MapProjectionAdapter::setPoint(const MC2Coordinate& newCoord,
                         const MC2Point& screenPoint,
                         double angleDegrees ) 
{
   if ( m_handler->get3dMode() &&
        m_handler->getDrawingContext() == DrawingContextProxy::PLOTTER )
   {
      m_projection.setPoint( newCoord,
                             getPoint2D( screenPoint ), angleDegrees );
   } else {
      m_projection.setPoint( newCoord, screenPoint, angleDegrees );
   }
}

void 
MapProjectionAdapter::setAngle( double angleDegrees,
                          const MC2Point& rotationPoint ) 
{
   if ( m_handler->get3dMode() &&
        m_handler->getDrawingContext() == DrawingContextProxy::PLOTTER )
   {
      m_projection.setAngle(angleDegrees,
                            getPoint2D( rotationPoint ));
   } else {
      m_projection.setAngle(angleDegrees, rotationPoint);
   }
}

void 
MapProjectionAdapter::setAngle(double angleDegrees) 
{
   // Means set angle of the center.
   if ( m_handler->get3dMode() &&
        m_handler->getDrawingContext() == DrawingContextProxy::PLOTTER )
   {
      setAngle( angleDegrees, getCenterPoint() );      
   } else {
      m_projection.setAngle( angleDegrees );
   }
}

double
MapProjectionAdapter::zoom( double factor, 
                      const MC2Coordinate& zoomCoord,
                      const MC2Point& zoomPoint ) {

   if ( m_handler->get3dMode() &&
        m_handler->getDrawingContext() == DrawingContextProxy::PLOTTER )
   {
      return m_projection.zoom( factor,
                                zoomCoord,
                                getPoint2D( zoomPoint ) );
   } else {
      return m_projection.zoom( factor, zoomCoord, zoomPoint );
   }
}

bool MapProjectionAdapter::zoom(double factor, 
                          const MC2Coordinate& zoomCoord,
                          const MC2Point& zoomPoint,
                          double& resultFactor)
{
   resultFactor = zoom(factor, zoomCoord, zoomPoint);
   return true;
}

double 
MapProjectionAdapter::zoom( double factor ) 
{
   if ( m_handler->get3dMode() &&
        m_handler->getDrawingContext() == DrawingContextProxy::PLOTTER )
   {
      // Zoom means zooming in the center of the screen.
      return zoom( factor, getCenter(), getCenterPoint() );
   } else {
      return m_projection.zoom( factor );
   }
}

bool MapProjectionAdapter::zoom(double factor, double& resultFactor)
{
   resultFactor = zoom(factor);
   return true;
}

void 
MapProjectionAdapter::setPixelBox( const MC2Point& oneCorner,
                             const MC2Point& otherCorner ) 
{
   m_projection.setPixelBox( getPoint2D( oneCorner ), 
                             getPoint2D( otherCorner ) );
}
      
void 
MapProjectionAdapter::setWorldBox( const MC2Coordinate& oneCorner,
                             const MC2Coordinate& otherCorner ) 
{
   MC2BoundingBox box( oneCorner, otherCorner );
   m_projection.setBoundingBox( box );
   // Set center so that center of box will be centered also in 3d. 
   setCenter( box.getCenter() );
}

void
MapProjectionAdapter::rotateLeftDeg(int angleDeg)
{
   setAngle(getAngle() + angleDeg);
}

const PixelBox& MapProjectionAdapter::getPixelBoundingBox() const
{
   return m_projection.getPixelBoundingBox(); 
}

void MapProjectionAdapter::setBoundingBox(const MC2BoundingBox& bbox) {
   m_projection.setBoundingBox(bbox);   
}

void MapProjectionAdapter::move(int deltaX, int deltaY)
{
   // if ( m_handler->getDrawingContext() == DrawingContextProxy::PLOTTER )
   // {
      m_projection.move(deltaX, deltaY);
   // } else {
   //    std::cout << "MPA--MOVE" << std::endl;
      
   //    MC2Coordinate coord;

   //    MC2Point centerPoint(0,0);
      
   //    m_handler->m_extRenderer->transform(centerPoint,
   //                                        m_projection.getCenter());

   //    centerPoint.getX() += deltaX;
   //    centerPoint.getY() += deltaY;
      
   //    m_handler->m_extRenderer->inverseTransform(coord, centerPoint);
   //    m_projection.setCenter(coord);
   // }    
}

void MapProjectionAdapter::setMovementMode(bool moving)
{
   m_handler->setMoving(moving); 
}

