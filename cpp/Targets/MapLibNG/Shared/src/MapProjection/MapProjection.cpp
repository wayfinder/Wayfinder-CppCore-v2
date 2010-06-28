/*
  Copyright (c) 1999 - 2010, Vodafone Group Services Ltd
  All rights reserved.

  Redistribution and use in source and binary forms, with or without modification, are permitted provided that the following conditions are met:

  * Redistributions of source code must retain the above copyright notice, this list of conditions and the following disclaimer.
  * Redistributions in binary form must reproduce the above copyright notice, this list of conditions and the following disclaimer in the documentation and/or other materials provided with the distribution.
  * Neither the name of the Vodafone Group Services Ltd nor the names of its contributors may be used to endorse or promote products derived from this software without specific prior written permission.

  THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS" AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE ARE DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT HOLDER OR CONTRIBUTORS BE LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
*/

#define CORE_LOGGING_MODULE_NAME "MapProjection"
#define CORE_LOGGING_MODULE_LEVEL CL_ALL_FILE

#include "CoreDebugPrint.h"

#include "config.h"
#include "MapProjection/MapProjection.h"
#include "PixelBox.h"
#include "FixedPointType.h"
#include <algorithm>
#include "SphericalCamera.h"


#include "WGS84Coordinate.h"
#include "MC2Coordinate.h"

int32 MapProjection::s_topLat;
int32 MapProjection::s_bottomLat;
double MapProjection::s_maxScale;


#define MAX_SCALE_FOR_ROTATION 1800

using namespace std;

class ScopedCounter {
public:
   ScopedCounter(int& c) : m_c(c) {
      m_c++;
   }
   
   ~ScopedCounter() {
      m_c--;
   }
   
private:
   int& m_c;
};

MapProjection::
MapProjection() : TransformMatrix(),
                  m_screenSize(100,100),
                  m_dpiCorrectionFactor( 1 ),
                  m_batchedEvent(MapProjectionListener::UNKNOWN)
{
   m_usingFOVBoundingBox = false;
   m_angle = 0.0;
   m_3dOn = false;
   m_projectionNotificationCounter = 0;
   
   MC2Coordinate lower( 664609150, 157263143 );
   MC2Coordinate upper( 664689150, 157405144 );
   MC2BoundingBox bbox(lower, upper);
   setBoundingBox(bbox);
   m_camera = new SphericalCamera(*this);
   setProjectionType(MC2_PROJECTION);
}

MapProjection::
MapProjection(const MC2Coordinate& centerCoord,
              int screenX, int screenY, double scale,
              double angle) : TransformMatrix(), m_centerCoord(centerCoord),
                              m_screenSize(screenX, screenY),
                              m_scale(scale),
                              m_angle(angle),
                              m_dpiCorrectionFactor( 1 ),
                              m_batchedEvent(MapProjectionListener::UNKNOWN)
{
   m_projectionNotificationCounter = 0;
   m_usingFOVBoundingBox = false;
   m_3dOn = false;
   m_angle = 0.0;
   m_camera = new SphericalCamera(*this);
   setProjectionType(MC2_PROJECTION);
}

void
MapProjection::updateBBox()
{
   if( m_usingFOVBoundingBox ) {
      m_camera->projectionUpdated();
      m_bbox = m_camera->getBoundingBox();
      return;
   } else  {
      
//    static const double mc2scaletometer = 6378137.0*2.0*
//       3.14159265358979323846 / 4294967296.0;
//    static const float meterstomc2scale = 1.0 / mc2scaletometer;

      pair<float,float> widthAndHeight = getWidthAndHeight();
   const float width = widthAndHeight.first;
   const float height = widthAndHeight.second;
   
   {
      const int32 centerLat = getCenter().lat;
      const float halfHeight = height * 0.5;
      const int32 intHalfHeight = int32( halfHeight );
      
      if ( halfHeight + centerLat > ( MAX_INT32 / 2 ) ) {
         m_bbox.setMaxLat( MAX_INT32 / 2 );
      } else {
         m_bbox.setMaxLat( centerLat + intHalfHeight );
      }
      if ( centerLat - halfHeight < ( MIN_INT32 / 2 ) ) {
         m_bbox.setMinLat( MIN_INT32 / 2 );
      } else {
         m_bbox.setMinLat( centerLat - intHalfHeight );
      }
      m_bbox.updateCosLat();
   }
   
   {            
      if ( width >= MAX_UINT32 - 1) {
         m_bbox.setMinLon( MIN_INT32 );
         m_bbox.setMaxLon( MAX_INT32 );
      } else {
         int32 intWidth = int32(width * 0.5 );
         int32 centerLon = getCenter().lon;
         m_bbox.setMinLon( centerLon - intWidth );
         m_bbox.setMaxLon( centerLon + intWidth );
      }
   }

//    MC2Coordinate coord1;
//    inverseTranformCosLatSupplied( coord1,
//                                   m_screenSize.getX(),
//                                   0,
//                                   getCosLat(getCenter().lat));
//    MC2Coordinate coord2;
//    inverseTranformCosLatSupplied( coord2,
//                                   0,
//                                   m_screenSize.getY(),
//                                   getCosLat(getCenter().lat));
//    MC2Coordinate coord3;
//    inverseTranformCosLatSupplied( coord3,
//                                   m_screenSize.getX(),
//                                   m_screenSize.getY(),
//                                   getCosLat(getCenter().lat));
//    MC2Coordinate coord4;
//    inverseTranformCosLatSupplied( coord4,
//                                   0,
//                                   0,
//                                   getCosLat(getCenter().lat) );

#if 0
#ifdef __unix__
   mc2dbg8 << "[TMH]: m_centerCoord = " << m_centerCoord
           << "                                      and center of m_bbox = "
           << m_bbox.getCenter() << endl;
   MC2Point corner1(0,0);
   MC2Point corner2(corner1);
   MC2Point corner3(corner2);
   MC2Point corner4(corner3);

   transformPointCosLatSupplied(corner1, coord1, getCosLat(getCenter().lat));
   transformPointCosLatSupplied(corner2, coord2, getCosLat(getCenter().lat));
   transformPointCosLatSupplied(corner3, coord3, getCosLat(getCenter().lat));
   transformPointCosLatSupplied(corner4, coord4, getCosLat(getCenter().lat));
   
   mc2dbg << "[TMH]: Corners are "
          << corner1.getX() << "," << corner1.getY() << " + "
          << corner2.getX() << "," << corner2.getY() << " + "
          << corner3.getX() << "," << corner3.getY() << " + "
          << corner4.getX() << "," << corner4.getY() << " + "
          << endl;
#endif
#endif
}
}

void
MapProjection::updateTransformMatrix(MapProjectionListener::projectionEvent trigger)
{
   // Constant forever
   static const float mc2scaletometer = 6378137.0*2.0*
      3.14159265358979323846 / 4294967296.0;
   static const float meterstomc2scale = 1.0 / mc2scaletometer;

   float radAngle = getAngle() / 180 * M_PI;
   float maxScale = getCosLat( m_centerCoord.lat) * float(MAX_UINT32-1000) / (
      ( fabs( m_screenSize.getX() *
              meterstomc2scale * cos(radAngle) ) +
        fabs( m_screenSize.getY() *
              meterstomc2scale * sin(radAngle) ) ) );

   m_scale = MIN( (float)m_scale, maxScale );
   
   const double invScale = 1.0/m_scale;     
   const double mc2scale = mc2scaletometer * invScale;
   
   TransformMatrix::updateMatrix( m_angle, mc2scale, m_centerCoord,
                                  m_screenSize.getX(),
                                  m_screenSize.getY() );

   updateBBox();

   notifyListeners(trigger);
}

float64
MapProjection::getPixelScaleFromBBoxAndSize( int screenXSize, 
                                        int screenYSize,
                                        MC2BoundingBox& bbox )
{
   static const double mc2scaletometer = 6378137.0*2.0*
      3.14159265358979323846 / 4294967296.0;
   // Strech out the bbox to fit the screen size.

   // This is copied from GfxUtility. I don't want that dependency   
   // so I have copied it here. I might want to change it though.

   int32 minLat = bbox.getMinLat();
   int32 minLon = bbox.getMinLon();
   int32 maxLat = bbox.getMaxLat();
   int32 maxLon = bbox.getMaxLon();
   
   int width = screenXSize;
   int height = screenYSize;
   // width and height should have same proportions as 
   // bbox.width and bbox.height
   float64 bboxHeight = bbox.getHeight();
   float64 bboxWidth = bbox.getWidth();
   if ( bboxHeight == 0.0 ) {
      bboxHeight = 1.0;
   }
   if ( bboxWidth == 0.0 ) {
      bboxWidth = 1.0;
   }
   float64 factor = bboxHeight / bboxWidth * width / height;
   if ( factor < 1 ) {
      // Compensate for that the display is higher than the bbox
//      height = uint16( height * factor );
      int32 extraHeight = 
         int32( rint( ( (bboxHeight / factor ) - 
                        bboxHeight ) / 2 ) );
      minLat -= extraHeight;
      maxLat += extraHeight;
   } else {
      // Compensate for that the display is wider than the bbox
//      width = uint16( width / factor );
      uint32 lonDiff = bbox.getLonDiff();
      if ( lonDiff == 0 ) {
         lonDiff = 1;
      }
      int32 extraWidth = 
         int32( rint( ( (lonDiff * factor ) - 
                        lonDiff ) / 2 ) );
      minLon -= extraWidth;
      maxLon += extraWidth;
      bbox.setMinLon( minLon );
      bbox.setMaxLon( maxLon );
   }

   bbox.setMinLat( MAX( minLat, (int32) s_bottomLat ) );
   bbox.setMaxLat( MIN( maxLat, (int32) s_topLat ) );
   bbox.setMinLon( minLon );
   bbox.setMaxLon( maxLon );
   
   float64 scale = 
      double(bbox.getHeight() * mc2scaletometer) /
      screenYSize; // unit meters map / pixel
   
   // Ugglefix.
   if ( scale < 0 || scale > s_maxScale ) {
      scale = s_maxScale;
   }
   
   return scale;   
}

MC2BoundingBox
MapProjection::setBoundingBox(const MC2BoundingBox& inbbox)
{   
   MC2BoundingBox bbox(inbbox);
   // Set the scale
   m_scale = getPixelScaleFromBBoxAndSize( m_screenSize.getX(),
                                      m_screenSize.getY(),
                                      bbox );
   // Save the corner
   {
      ScopedCounter c(getNotificationCounter());
      setCenter( bbox.getCenter() );
   }

   updateTransformMatrix(MapProjectionListener::MAP_BOUNDARIES_SET);
   
   return bbox;
}

pair<int, int>
MapProjection::getScreenExtentMC2() const
{
   static const double mc2scaletometer = 6378137.0*2.0*
      3.14159265358979323846 / 4294967296.0;
   static const float meterstomc2scale = 1.0 / mc2scaletometer;

   const float width = m_screenSize.getX() * meterstomc2scale;
   const float height = m_screenSize.getY() * meterstomc2scale;
   
   return pair<int,int>( static_cast<int>( width  ),
                         static_cast<int>( height ) );
}

pair<float,float>
MapProjection::getWidthAndHeight() const
{
   static const double mc2scaletometer = 6378137.0*2.0*
      3.14159265358979323846 / 4294967296.0;
   static const float meterstomc2scale = 1.0 / mc2scaletometer;
   
   const float radAngle = ( getAngle() ) / 180.0 * M_PI;
   const float scale = getPixelScale() * meterstomc2scale;
   const float b = m_screenSize.getX() * scale;
   const float h = m_screenSize.getY() * scale;
   const float width =
      ( fabs( b * cos(radAngle) ) +
        fabs( h * sin(radAngle) ) ) / getCosLat();
   
   const float height =
      fabs( b * sin(radAngle) ) +
      fabs( h * cos(radAngle) );
   
   return pair<float,float>(width, height );
}

MC2BoundingBox
MapProjection::getDrawingBBox() const
{
   if( m_usingFOVBoundingBox ) {
      m_camera->projectionUpdated();
      return m_camera->getBoundingBox();
   } else {
      pair<float,float> widthAndHeight = getWidthAndHeight();
      const float height = widthAndHeight.second;
      const float width = widthAndHeight.first;
      // Height
      MC2BoundingBox bbox;
      {
         const int32 centerLat = getCenter().lat;
         const float halfHeight = height * 0.5 ;
         const int32 intHalfHeight = int32( halfHeight );
      
         if ( halfHeight + centerLat > ( MAX_INT32 / 2 ) ) {
            bbox.setMaxLat( MAX_INT32 / 2 );
         } else {
            bbox.setMaxLat( centerLat + intHalfHeight );
         }
         if ( centerLat - halfHeight < ( MIN_INT32 / 2 ) ) {
            bbox.setMinLat( MIN_INT32 / 2 );
         } else {
            bbox.setMinLat( centerLat - intHalfHeight );
         }
         bbox.updateCosLat();
      }
      //Width
      {
         const int32 centerLon = getCenter().lon;
         const float halfWidth  = width * 0.5 ;
         const int32 intHalfWidth = int32( halfWidth );

         int MIN_NUM = MIN_INT32 + 1;
         int MAX_NUM = MAX_INT32 - 1;
      
         if ( halfWidth + centerLon > ( MAX_NUM  ) ) {
            bbox.setMaxLon( MAX_NUM  );
         } else {
            bbox.setMaxLon( centerLon + intHalfWidth );
         }
         if ( centerLon - halfWidth < ( MIN_NUM  ) ) {
            bbox.setMinLon( MIN_NUM  );
         } else {
            bbox.setMinLon( centerLon - intHalfWidth );
         }
      }

      return bbox;
   }
}


void
MapProjection::getDrawingBBoxes(std::vector<MC2BoundingBox>& outBoxes) const
{
   if( m_usingFOVBoundingBox ) {
      m_camera->projectionUpdated();
            
      MC2BoundingBox camBox = m_camera->getBoundingBox();
      
      float minLon = camBox.getMinLon();
      float maxLon = camBox.getMaxLon();
      
      bool shouldSplit = minLon > maxLon;
      if (!shouldSplit){
         outBoxes.push_back(camBox);
      } else {
         
         // We split the cambox into two boxes, one on each
         // side of the datum line.
         MC2BoundingBox leftBox(MC2Coordinate(camBox.getMinLat(),
                                              camBox.getMinLon()),
                                MC2Coordinate(camBox.getMaxLat(),
                                              MAX_INT32));
         MC2BoundingBox rightBox(MC2Coordinate(camBox.getMinLat(),
                                               MIN_INT32),
                                 MC2Coordinate(camBox.getMaxLat(),
                                               camBox.getMaxLon()));
         outBoxes.push_back(leftBox);
         outBoxes.push_back(rightBox);
      }

   } else {
      pair<float,float> widthAndHeight = getWidthAndHeight();
      const float height = widthAndHeight.second;
      // Height
      MC2BoundingBox bbox;
      {
         const int32 centerLat = getCenter().lat;
         const float halfHeight = height * 0.5 ;
         const int32 intHalfHeight = int32( halfHeight );
      
         if ( halfHeight + centerLat > ( MAX_INT32 / 2 ) ) {
            bbox.setMaxLat( MAX_INT32 / 2 );
         } else {
            bbox.setMaxLat( centerLat + intHalfHeight );
         }
         if ( centerLat - halfHeight < ( MIN_INT32 / 2 ) ) {
            bbox.setMinLat( MIN_INT32 / 2 );
         } else {
            bbox.setMinLat( centerLat - intHalfHeight );
         }
         bbox.updateCosLat();
      }
      // Widths
      {
         float width = MIN( widthAndHeight.first, float(MAX_UINT32) );
         int32 centerLon = getCenter().lon;
         int32 curMinLon = int32(int64(centerLon) - int64(width / 2) );
         // Add bounding boxes until there is no width left
         while ( width > 0.0 ) {
            outBoxes.push_back( bbox );
            MC2BoundingBox& curBox = outBoxes.back();
            int32 curWidth = int32( MIN( width, float( MAX_INT32 / 4 ) ) );
            if ( curWidth == 0 ) {
               outBoxes.pop_back();
               break;
            }
            curBox.setMinLon( curMinLon );
            curBox.setMaxLon( curMinLon + curWidth );
            if ( curBox.getMinLon() > curBox.getMaxLon() ) {
               curBox.setMaxLon( MAX_INT32 );
               curMinLon = MIN_INT32;
               width -= ( curBox.getMaxLon() - curBox.getMinLon() );
            } else {            
               curMinLon += curWidth;
               width -= curWidth;
            }
         }      
      }
   }
}

double
MapProjection::setPixelScale(double scale)
{
   if(scale > MAX_SCALE_FOR_ROTATION && getAngle() != 0.0) {
      setAngle(0.0);
   }
   
   double oldScale = m_scale;
#ifdef ENABLE_INTEGER_SCALE_STEPS

   if ( scale != m_scale ) {
      // Truncate the scale so it will be easier to 
      // reproduce the same scale level.
      double rintScale = rint( scale );
      if ( rintScale != rint( m_scale ) ) {
         m_scale = rintScale;
      } else if ( scale < m_scale ) {
         m_scale = rintScale - 1;
      } else {
         m_scale = rintScale + 1;
      }
   }
#else
   m_scale = scale;
#endif   
   
   const double minScale = 0.1;
   const double maxScale = s_maxScale;
   if ( m_scale < minScale ) {
      m_scale = minScale;
   } else if ( m_scale > maxScale ) {
      m_scale = maxScale;
   }

   if ( m_scale != oldScale ) {
      updateTransformMatrix(MapProjectionListener::SET_ZOOM);
   }
   return m_scale;
}

double
MapProjection::getPixelScale() const
{
   return m_scale;
}

void 
MapProjection::setDPICorrectionFactor( uint32 factor )
{
   factor = 1;
   m_dpiCorrectionFactor = factor;
   updateTransformMatrix(MapProjectionListener::DPI_CORRECTION_FACTOR);
}
   
uint32 
MapProjection::getDPICorrectionFactor() const 
{
   return m_dpiCorrectionFactor;
}
   
double 
MapProjection::setDPICorrectedScale( double scale )
{
   setPixelScale( scale / double( getDPICorrectionFactor() ) );
   return getDPICorrectedScale();
}
   
double 
MapProjection::getDPICorrectedScale() const 
{
   double scale = getPixelScale() * double( getDPICorrectionFactor() );
   if ( scale > s_maxScale ) {
      scale = s_maxScale;
   }
   return scale;
}

double
MapProjection::zoom(double factor)
{
   // Zoom is a higher level command which uses setPixelScale internally.
   // Since setPixelScale notifies that a pixel scale has been set, we need
   // to mute that event so that we can send the proper higher level event.

   {
      ScopedCounter c(getNotificationCounter());
      setPixelScale( factor * getPixelScale() );
   }
   
   updateTransformMatrix(MapProjectionListener::ZOOM);
   
   return m_scale;
}

double
MapProjection::zoom(double factor,
                    const MC2Coordinate& zoomCoord,
                    const MC2Point& zoomPoint )
{
   double newScale = 0.0;
   
   // Zooming to point is an ever higher level command than zooming, so
   // we need to use the same procedure here

   {
      ScopedCounter c(getNotificationCounter());

      newScale = zoom( factor );
   
      setPoint( zoomCoord, zoomPoint );
   }

   updateTransformMatrix(MapProjectionListener::ZOOM_POINT);
   
   return newScale;
}

void
MapProjection::setPixelBox( const MC2Point& oneCorner,
                            const MC2Point& otherCorner )
{
   {
      ScopedCounter c(getNotificationCounter());
      
      PixelBox pixBox( oneCorner, otherCorner );
      MC2BoundingBox bbox;
      for( int i = 0; i < 4; ++i ) {
         MC2Coordinate tmpCoord;
         inverseTranformUsingCosLat( tmpCoord, pixBox.getCorner(i) );
         bbox.update( tmpCoord );
      }
      double oldangle = m_angle;
      setAngle(0);
      setBoundingBox( bbox );
      setAngle( oldangle );
   }

   updateTransformMatrix(MapProjectionListener::SCREEN_BOX_SET);
}

void
MapProjection::move(int deltaX,
                    int deltaY )
{

   {      
      ScopedCounter c(getNotificationCounter());
      
      // Translate the screen coordinates into lat/lon.
      MC2Coordinate center;
      inverseTranformCosLatSupplied(
         center,
         deltaX + (m_screenSize.getX() >> 1),
         deltaY + (m_screenSize.getY() >> 1),
         getCosLat( m_centerCoord.lat ) );
      setCenter( center );
   }

   updateTransformMatrix(MapProjectionListener::MOVE);
}

void
MapProjection::setCenter(const MC2Coordinate& newCenter)
{
   MC2Coordinate prev = m_centerCoord;

   
   m_centerCoord = newCenter;
   
   if(m_centerCoord.lat + (m_bbox.getHeight()/2) > s_topLat) {
      m_centerCoord.lat = s_topLat - (m_bbox.getHeight()/2);
   } else if(m_centerCoord.lat - (m_bbox.getHeight()/2) < s_bottomLat) {
      m_centerCoord.lat = s_bottomLat + (m_bbox.getHeight()/2);
   }
   
   
   if(prev != m_centerCoord) {
      updateTransformMatrix(MapProjectionListener::SET_CENTER);
   }
}

void
MapProjection::setPoint(const MC2Coordinate& newCoord,
                        const MC2Point& screenPoint )
{
   
   // Now set the center to that center coordinate. Will update
   // all the members.
   {
      ScopedCounter c(getNotificationCounter());

      // Translate the center to a screen coord.
      MC2Point centerPoint(0,0);
      transformPointInternalCosLat( centerPoint,
                                    m_centerCoord );

      MC2Coordinate newCenter;
   
      getOffsetCenterCoordinate(screenPoint.getX(),
                                screenPoint.getY(),
                                centerPoint.getX(),
                                centerPoint.getY(),
                                newCoord,
                                newCenter);
      
      setCenter( newCenter );
   }

   updateTransformMatrix(MapProjectionListener::SET_POINT);
}

void
MapProjection::setAngle(double angleDeg)
{
   m_angle = angleDeg;

   if(m_scale > MAX_SCALE_FOR_ROTATION) {
      m_angle = 0.0;
   }
   
   if(m_angle > 360) {
      m_angle -= 360;
   }

   if(m_angle < 0) {
      m_angle += 360;
   }
   
   updateTransformMatrix(MapProjectionListener::SET_ANGLE);
}

void
MapProjection::setAngle(double angleDeg, const MC2Point& rotationPoint )
{
   {
      // Translate the center to a screen coord.
      MC2Point centerPoint(0,0);
      transformPointInternalCosLat( centerPoint,
                                    m_centerCoord );

      int deltaX = centerPoint.getX() - rotationPoint.getX();
      int deltaY = centerPoint.getY() - rotationPoint.getY();
      
      ScopedCounter c(getNotificationCounter());
      move(-deltaX, -deltaY);
      setAngle( angleDeg );
      move(deltaX, deltaY);
   }

   updateTransformMatrix(MapProjectionListener::ROTATE_POINT);
}

double
MapProjection::getAngle() const
{
   return m_angle;
}

void
MapProjection::setPoint(const MC2Coordinate& newCoord,
                        const MC2Point& screenPoint,
                        double angleDeg )
{
   {
      ScopedCounter c(getNotificationCounter());
      setPoint( newCoord, screenPoint );
      setAngle( angleDeg, screenPoint );
   }

   updateTransformMatrix(MapProjectionListener::SET_POINT);
}

void
MapProjection::setScreenSize(const MC2Point& size)
{
   if ( size != m_screenSize && size != MC2Point(0,0) ) {
      m_screenSize = size;

      m_pixelBoundingBox = PixelBox( MC2Point(0, 0),
                                     MC2Point(size.getX(), size.getY()) );

      updateTransformMatrix(MapProjectionListener::SET_SCREEN_SIZE);
   }
}

MC2Coordinate
MapProjection::calcLowerLeft() const
{
   // Constant forever
   static const double mc2scaletometer = 6378137.0*2.0*
      3.14159265358979323846 / 4294967296.0;
   const double invScale = 1.0/m_scale;
   const double mc2scale = mc2scaletometer * invScale;

   const int screenWidth  = m_screenSize.getX();
   const int screenHeight = m_screenSize.getY();
   
   return MC2Coordinate( int32(double(m_centerCoord.lat) -
                               (1/mc2scale * screenHeight * 0.5)),
                         int32(double(m_centerCoord.lon) -
                               (1/mc2scale/getCosLat(m_centerCoord.lat) *
                                screenWidth * 0.5 ) ) );
}

bool MapProjection::get3dMode() const
{
   return m_3dOn;
}

void MapProjection::set3dMode( bool on )
{
   m_3dOn = on;

   if( m_3dOn ) {
      m_camera->setVariable3DMode(1.0f);
   } else {
      m_camera->setVariable3DMode(0.0f);
   }

}

void MapProjection::setVariable3dMode(float step)
{
   if (step == 0.0f){
      m_3dOn = false;
   } else {
      m_3dOn = true;  
   }
   
   m_camera->setVariable3DMode(step);      
}

MC2BoundingBox MapProjection::getCenterBox() const
{
   return MC2BoundingBox( getCenter(), 7 ); 
}

MC2Coordinate MapProjection::getCameraCenter() const
{
   if( m_3dOn ) {
      MC2Coordinate center;
      
      inverseTranformCosLatSupplied( center,
                                     (m_screenSize.getX() >> 1),
                                     165 + (m_screenSize.getY() >> 1),
                                     getCosLat( m_centerCoord.lat ) );
   
      return center;      
   } else {
      return m_centerCoord;
   }
}

MC2BoundingBox MapProjection::getCameraCenterBox() const
{
   return MC2BoundingBox( getCameraCenter(), 7 ); 
}

double MapProjection::getAspectRatio() const
{
   const double w = m_screenSize.getX();
   const double h = m_screenSize.getY();
   
   return w / h;
}

void MapProjection::setUsingFOVBoundingBox( bool enabled )
{
   m_usingFOVBoundingBox = enabled;
   updateBBox();
}

bool MapProjection::usingFOVBoundingBox() const
{
   return m_usingFOVBoundingBox; 
}

Camera& MapProjection::getCamera()
{
   return *m_camera;
}

const Camera& MapProjection::getCamera() const
{
   return *m_camera; 
}

void MapProjection::cameraUpdated()
{
   updateTransformMatrix(MapProjectionListener::CAMERA_CHANGED);
}

void MapProjection::setProjectionType(projection_t projectionType)
{
   m_projectionType = projectionType;
   
   if (projectionType == MC2_PROJECTION) {
      s_maxScale = 24000.0;
      s_topLat = 912909609 ;
      s_bottomLat = -912909609;
      setCosLatEnabled(true);
   } else if (projectionType == MERCATOR_PROJECTION) {
      s_maxScale = 100000.0;
      WFAPI::WGS84Coordinate topLatWgs(85.05112, 0);
      MC2Coordinate topLatMC2(topLatWgs);
      MercatorCoord topLatMerc(topLatMC2);
      s_topLat = topLatMerc.lat;
      s_bottomLat = -topLatMerc.lat;
      setCosLatEnabled(false);
   } else {
      // Cannot happen!
//      MC2_ASSERT(false);
   }
}

const PixelBox& MapProjection::getPixelBoundingBox() const
{
   return m_pixelBoundingBox; 
}

void MapProjection::addListener( MapProjectionListener* listener )
{
   m_listeners.push_back(listener);
}

void MapProjection::notifyListeners(MapProjectionListener::projectionEvent event)
{
   if(!projectionNotificationEnabled()) {
      return;
   }
   
   for(ProjectionListenerVec::iterator itr = m_listeners.begin();
       itr != m_listeners.end(); itr++)
   {
      (*itr)->projectionUpdated(event);
   }
}


bool MapProjection::projectionNotificationEnabled() const
{
   return m_projectionNotificationCounter == 0; 
}

int& MapProjection::getNotificationCounter()
{
   return m_projectionNotificationCounter; 
}

MapProjection::~MapProjection()
{
   delete m_camera; 
}

const char* MapProjectionListener::toString(projectionEvent event)
{
   switch(event) {

   case UNKNOWN:
      return "UNKNOWN";
      break;
   case MOVE:
      return "MOVE";
      break;
   case DPI_CORRECTION_FACTOR:
      return "DPI_CORRECTION_FACTOR";
      break;
   case SET_CENTER:
      return "SET_CENTER";
      break;
   case ZOOM:
      return "ZOOM";
      break;
   case ZOOM_POINT:
      return "ZOOM_POINT";
      break;
   case SET_ANGLE:
      return "SET_ANGLE";
      break;
   case ROTATE_POINT:
      return "ROTATE_POINT";
      break;
   case SET_ZOOM:
      return "SET_ZOOM";
      break;
   case SET_POINT:
      return "SET_POINT";
      break;
   case SCREEN_BOX_SET:
      return "SCREEN_BOX_SET";
      break;
   case SET_SCREEN_SIZE:
      return "SET_SCREEN_SIZE";
      break;
   case CAMERA_CHANGED:
      return "CAMERA_CHANGED";
      break;
   case MAP_BOUNDARIES_SET:
      return "MAP_BOUNDARIES_SET";
      break;
   default:
      return "";
   }
}

void MapProjection::printState() const
{

   coreprintln("Current state:");

   coreprintln("Center-coord: (%d, %d)",
               m_centerCoord.lat, m_centerCoord.lon);

   coreprintln("Scale: %lf", m_scale);

   coreprintln("Angle: %lf", m_angle);
   
   coreprintln("BBox: [(%d %d), (%d, %d)]",
               m_bbox.getMaxLat(), m_bbox.getMinLon(),
               m_bbox.getMinLat(), m_bbox.getMaxLon());

   coreprintln("cosLat: %lf", getCosLat());
   
   coreprintln("PixelBox: [(%d %d), (%d, %d)]",
               m_pixelBoundingBox.getTopLeft().getX(),
               m_pixelBoundingBox.getTopLeft().getY(),
               m_pixelBoundingBox.getBottomRight().getX(),
               m_pixelBoundingBox.getBottomRight().getY());

   coreprintln("Screen-size: %dx%d",
               m_screenSize.getX(), m_screenSize.getY());

   
   // coreprintln("DPI correction factor: %u", m_dpiCorrectionFactor);
   
   // coreprintln("3D: %s", m_3dOn ? : "on" : "off");

   // coreprintln("Use FOV bounding box: %d", m_usingFOVBoundingBox);

   // coreprintln("Camera: %p", m_camera);
}

void MapProjection::beginBatchedEvent(MapProjectionListener::projectionEvent event)
{
   getNotificationCounter()++;
   m_batchedEvent = event; 
}

void MapProjection::endBatchedEvent()
{
   getNotificationCounter()--;

   notifyListeners(m_batchedEvent);
      
   m_batchedEvent = MapProjectionListener::UNKNOWN;
}

MapProjection::projection_t MapProjection::getProjectionType() const
{
   return m_projectionType; 
}

