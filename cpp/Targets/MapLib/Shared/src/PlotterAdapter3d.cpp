/*
Copyright (c) 1999 - 2010, Vodafone Group Services Ltd
All rights reserved.

Redistribution and use in source and binary forms, with or without modification, are permitted provided that the following conditions are met:

    * Redistributions of source code must retain the above copyright notice, this list of conditions and the following disclaimer.
    * Redistributions in binary form must reproduce the above copyright notice, this list of conditions and the following disclaimer in the documentation and/or other materials provided with the distribution.
    * Neither the name of the Vodafone Group Services Ltd nor the names of its contributors may be used to endorse or promote products derived from this software without specific prior written permission.

THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS" AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE ARE DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT HOLDER OR CONTRIBUTORS BE LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
*/


#include "config.h"
#include "MathVec.h"
#include "PlotterAdapter3d.h"
#include <vector>
#include "MC2Point.h"

#include "ClipUtil.h"
#include "MC2BoundingBox.h"
#include "MC2PointLogging.h"
#include "PolyLine.h"
#include "InsideUtil.h"
#include <math.h>

#include "GfxUtility.h"


using namespace isab;

PlotterAdapter3d::PlotterAdapter3d( MapPlotter* plotter ) : PlotterAdapter( plotter )
{
   m_settings.m_alpha = 30.0 / 180.0 * M_PI;
   m_settings.m_delta = 66.0 / 180.0 * M_PI;
   m_settings.m_horizonPixels = 0;

   m_triangle.resize( 3, MC2Point(0,0) );
   m_polylineRect.resize( 4, MC2Point(0,0) );
}

PlotterAdapter3d::~PlotterAdapter3d()
{

}

bool
PlotterAdapter3d::isVisible() const
{
   return m_plotter->isVisible();
}

void 
PlotterAdapter3d::drawPolygon( std::vector<MC2Point>::const_iterator begin,
                               std::vector<MC2Point>::const_iterator end )
{
   project3d( begin, end, m_points3d );
   if ( m_points3d.size() < 3 ) {
      return;
   }
   PlotterAdapter::drawPolygon( m_points3d.begin(), m_points3d.end() );
}

void 
PlotterAdapter3d::drawPolygonWithColor( std::vector<MC2Point>::const_iterator begin,
                                      std::vector<MC2Point>::const_iterator end,
                                      uint32 fillcolor )
{
   project3d( begin, end, m_points3d );
   if ( m_points3d.size() < 3 ) {
      return;
   }
   PlotterAdapter::drawPolygonWithColor( m_points3d.begin(), m_points3d.end(), fillcolor );

}

void 
PlotterAdapter3d::drawPolyLine( std::vector<MC2Point>::const_iterator begin,
                              std::vector<MC2Point>::const_iterator end )
{
   setLineWidth( m_lineWidth );
   setFillColor( m_red, m_green, m_blue );
   plotterLine( begin, end, m_lineWidth );
}

void 
PlotterAdapter3d::drawPolyLineWithColor( std::vector<MC2Point>::const_iterator begin,
                                         std::vector<MC2Point>::const_iterator end,
                                         uint32 color,
                                         int lineWidth )
{
   
   if ( std::distance( begin, end ) < 2 ) {
      return;
   }

   setFillColor( uint8( color >> 16 ),
                 uint8( color >>  8 ),
                 uint8( color ) );
   
   plotterLine( begin, end, lineWidth );
}

void PlotterAdapter3d::setHorizonHeight( uint32 height )
{
   m_settings.m_horizonPixels = height;
   // Force update of members.
   m_scalefactor = -1;
   checkScreenAndUpdateMembers( 0 );
}

void PlotterAdapter3d::setPenColor( unsigned int red,
                                    unsigned int green,
                                    unsigned int blue )
{
   // Store the colors
   m_red   = red;
   m_green = green;
   m_blue  = blue;
   PlotterAdapter::setPenColor( red, green, blue );
}

void PlotterAdapter3d::setLineWidth( int lineWidth )
{
   // Store the line width
   m_lineWidth = lineWidth;
   PlotterAdapter::setLineWidth( lineWidth );
}

void
PlotterAdapter3d::makeTestVectors()
{
   if ( ! m_testTriangles.empty() ) {
      return;
   }

   int n = 20;
   m_testVector.resize( n * 2, MC2Point(0,0) );


   for ( int i = 0; i < n; ++i ) {
      MC2Point p( i*5, i*5 ); 
      m_testVector[ i ] = p;
      m_testVector[ n*2-1 -i ] = p + MC2Point( 25, 0 );
   }

   {for ( int i = 0; i < n*2; ++i ) {
      mc2dbg << " i = " << i << ", p = " << m_testVector[ i ] << endl;
   }}
//   PlotterAdapter::drawPolygon( v.begin(), v.end() );

   
   int f = 0;
   int b = m_testVector.size() - 1;

   mc2dbg << f << b << endl;
   do {
      std::vector<MC2Point> triangle;
      triangle.push_back( m_testVector[ f ] );
      triangle.push_back( m_testVector[ f + 1 ] );
      triangle.push_back( m_testVector[ b ] );
      m_testTriangles.push_back( triangle );

      triangle.clear();
      triangle.push_back( m_testVector[ f + 1 ] );
      triangle.push_back( m_testVector[ b - 1 ] );
      triangle.push_back( m_testVector[ b ] );
     
      m_testTriangles.push_back( triangle );

      ++f;
      --b;
      mc2dbg <<  f << b << endl;
   } while ( f+1 < b-1 );


   for ( uint32 j = 0; j < m_testTriangles.size(); ++j ) {
      mc2dbg << "Triangle:" << endl;
      for ( uint32 k = 0; k < m_testTriangles[ j ].size(); ++k ) {
         mc2dbg << m_testTriangles[ j ][ k ] << ", ";
      }
      mc2dbg << endl;
//      PlotterAdapter::drawPolygon( triangles[ j ].begin(), triangles[ j ].end() );
   }

}

#ifndef __unix__
   // MERGE_POLYGONS should only be defined for platforms that
   // can draw self intersecting polygons with winding fill rull rule,
   // i.e. not alternating fill rule (which will cause holes).
   // Since gtk only supports alternating fill rule, then MERGE_POLYGONS
   // cannot be defined for that platform.
   #define MERGE_POLYGONS
#endif
#ifndef MERGE_POLYGONS

#if 1
// Version of polyline that create polylines that are non selfintersecting.
// This version is typically faster than the version that draws individual
// triangles / rectangle for each segment.
void 
PlotterAdapter3d::plotterLine( 
      std::vector<MC2Point>::const_iterator begin,
      std::vector<MC2Point>::const_iterator end,
      int lineWidth )
{

#if 0
   std::vector<MC2Point> dummyData;
   dummyData.push_back(MC2Point(200, 400));
   dummyData.push_back(MC2Point(200, 300));
   dummyData.push_back(MC2Point(100, 250));

   begin = dummyData.begin();
   end = dummyData.end();

   lineWidth = 10;
#endif
   
//   cout << "Indata:" << endl;
//   for (std::vector<MC2Point>::const_iterator it = begin; it != end; ++it) {
//      cout << *it << endl; 
//   }


   it_t curIt = begin;
   it_t nextIt = curIt;
   ++nextIt;

   // First check the trend (derivative).
   // The reason is that resulting self intersecting polygons
   // are not allowed. If the trend (i.e. sign of diff between
   // two consequtive x or y values) differs then that can potentially
   // mean that the resulting polygon created out of that polyline
   // (including width) will be self intersecting.
   // Therefore we identify parts of the polyline which have the same
   // trend, store the begin and end iterators and then draw them.

   // 0 means no trend is set yet. A part from that only the
   // sign of the trend is taken into consideration.
   int trendX = 0;
   int trendY = 0;

   // Storage of begin, end iterators for parts of the polyline
   // with same trend.
   m_nonIntersectingPolylines.clear();

   it_t curBeginIt = begin;
   
   while (nextIt != end) {
      int curTrendX = nextIt->getX() - curIt->getX();
      int curTrendY = nextIt->getY() - curIt->getY();
      if (trendX == 0) {
         // No trend yet. Update trend with this.
         trendX = curTrendX;
      } else {
         // Not OK if different signs between trends.
         if (trendX > 0 && curTrendX < 0 || trendX < 0 && curTrendX > 0) {

            // Trend has breaked.
            // Store the iterators.
            m_nonIntersectingPolylines.push_back(
               make_pair(curBeginIt, nextIt));
            // Continue by resetting iterators and trend.
            curBeginIt = curIt;
            trendX = curTrendX;
            trendY = 0;
         }
      }
      if (trendY == 0) {
         // No trend yet. Update trend with this.
         trendY = curTrendY;
      } else {
         // Not OK if different signs between trends.
         if (trendY > 0 && curTrendY < 0 || trendY < 0 && curTrendY > 0) {
            // Trend has breaked..
            // Store the iterators.
            m_nonIntersectingPolylines.push_back(
               make_pair(curBeginIt, nextIt));
            // Continue by resetting iterators and trend.
            curBeginIt = curIt;
            trendY = curTrendY;
            trendX = 0;
         }
      }
      curIt = nextIt;
      ++nextIt;
   }
   
   // Add the last pair of iterators.
   m_nonIntersectingPolylines.push_back(
      make_pair(curBeginIt, nextIt));

//   cout << "Split into " << m_nonIntersectingPolylines.size() 
//        << " polylines" << endl; 

   // Each pair of begin, end iterators have the same trend, i.e.
   // we can be assured that the constructed polygons from the polylines
   // will not be selfintersecting.
   for (uint32 i = 0; i < m_nonIntersectingPolylines.size(); ++i) {
      plotterLineNonSelfIntersecting(m_nonIntersectingPolylines[i].first, 
                                     m_nonIntersectingPolylines[i].second, 
                                     lineWidth);
   }
}
#else
// Version of plotterLine that draw individual triangles and rectangles.
// This version is typically very slow.
// To be removed once the faster version has been used for a while.
void PlotterAdapter3d::plotterLine( std::vector<MC2Point>::const_iterator begin,
                                    std::vector<MC2Point>::const_iterator end,
                                    int lineWidth )
{
   int n = 0;
   n = 0;
   std::vector<MC2Point>::const_iterator curIt = begin;
   std::vector<MC2Point>::const_iterator nextIt = curIt;
   ++nextIt;

   MC2Point prevPoint = *curIt;
   MC2Point p0( -1, -1 );
   MC2Point p1( -1, -1 );
   MC2Point factor( -1, -1 );
   
  
   while ( nextIt != end ) {
      m_polylineRect.clear();
      PolyLine::calculatePolyLine(*curIt, *nextIt, lineWidth, m_polylineRect, factor);
      project3d( m_polylineRect.begin(), m_polylineRect.end(), m_points3d, n );
      PlotterAdapter::drawPolygon( m_points3d.begin(), m_points3d.end() );
      // Do not add any triangles for the first polyline, only between polylines
      if ( curIt != begin ) {
         // We do have a previous point, this is not 
         // the first polyline.
         int turn = InsideUtil::isLeft_int( prevPoint, *curIt, *nextIt );
         if ( turn > 0 ) {
            // Right turn
            m_triangle[ 0 ] = p1;
            m_triangle[ 1 ] = *curIt;
            m_triangle[ 2 ] = m_polylineRect[0];
            project3dNoCopy( m_triangle.begin(), m_triangle.end(), m_points3d, n );
            PlotterAdapter::drawPolygon( m_points3d.begin(), m_points3d.end() );
         } else if ( turn < 0 ) {
            // Left turn
            m_triangle[ 0 ] = p0;
            m_triangle[ 1 ] = *curIt;
            m_triangle[ 2 ] = m_polylineRect[1];
            project3dNoCopy( m_triangle.begin(), m_triangle.end(), m_points3d, n );
            PlotterAdapter::drawPolygon( m_points3d.begin(), m_points3d.end() );
         } else {
            // Do nothing right now!
         }
         prevPoint = *curIt;
      } else {
         // This is the first poly line in this segment
         m_triangle[ 0 ] = m_polylineRect[0];
         m_triangle[ 1 ] = (*curIt - factor);
         m_triangle[ 2 ] = m_polylineRect[1];
         project3dNoCopy( m_triangle.begin(), m_triangle.end(), m_points3d, n );
         PlotterAdapter::drawPolygon( m_points3d.begin(), m_points3d.end() );
      }
      p0 = m_polylineRect[2];
      p1 = m_polylineRect[3];
      curIt = nextIt;
      ++nextIt;
      if ( nextIt == end ) {
         // This is the last poly line in this segment
         m_triangle[ 0 ] = m_polylineRect[2];
         m_triangle[ 1 ] = ( *curIt + factor );
         m_triangle[ 2 ] = m_polylineRect[3];
         project3dNoCopy( m_triangle.begin(), m_triangle.end(), m_points3d, n );
         PlotterAdapter::drawPolygon ( m_points3d.begin(), m_points3d.end() );
      }
   }  
}
#endif
#else

// Version of plotterLine that draws the line as one (self intersecting) polygon.
#if 1
void 
PlotterAdapter3d::plotterLine( std::vector<MC2Point>::const_iterator begin,
                               std::vector<MC2Point>::const_iterator end,
                               int lineWidth )
{

   int n = 0;
   n = 0;
   std::vector<MC2Point>::const_iterator curIt = begin;
   std::vector<MC2Point>::const_iterator nextIt = curIt;
   ++nextIt;

   MC2Point prevPoint = *curIt;
   MC2Point p0( -1, -1 );
   MC2Point p1( -1, -1 );
   MC2Point factor( -1, -1 );
   
   // m is the size of the resulting polygon.
   // The polygon consists of 4 points for each pair of coordinates,
   // plus 2 extra points for the triangle at the start and end
   // of the polygon.
   int m = 4*(std::distance(begin, end) - 1 ) + 2;
   m_linePolygon.resize( m, MC2Point( 0, 0 ) );
  
   // Counter.
   int j = 1;
   
   while ( nextIt != end ) {
      m_polylineRect.clear();
      PolyLine::calculatePolyLine(*curIt, *nextIt, lineWidth, m_polylineRect, factor);

      m_linePolygon[ j ] = m_polylineRect[1];
      m_linePolygon[ m-j ] = m_polylineRect[0];
      m_linePolygon[ j+1 ] = m_polylineRect[2];
      m_linePolygon[ m-(j+1) ] = m_polylineRect[3]; 
      if ( curIt != begin ) {
         // We do have a previous point, this is not 
         // the first polyline.
         prevPoint = *curIt;
      } else {
         // This is the first poly line in this segment
         m_linePolygon[ 0 ] = (*curIt - factor);
      }
      p0 = m_polylineRect[2];
      p1 = m_polylineRect[3];
      curIt = nextIt;
      ++nextIt;
      if ( nextIt == end ) {
         // This is the last poly line in this segment
         m_linePolygon[ j+2 ] = (*curIt + factor);
         MC2_ASSERT( j+2 == m-(j+2) );
      }
      // Move to next pair of coordinates.
      j += 2;
   }  

   project3d( m_linePolygon.begin(), m_linePolygon.end(), m_points3d, n );
   PlotterAdapter::drawPolygon( m_points3d.begin(), m_points3d.end() );
}
#endif
#endif

void 
PlotterAdapter3d::plotterLineNonSelfIntersecting( 
      std::vector<MC2Point>::const_iterator begin,
      std::vector<MC2Point>::const_iterator end,
      int lineWidth )
{                               

   // Reset iterators.
   it_t curIt = begin;
   it_t nextIt = curIt;
   ++nextIt;

   MC2Point prevPoint = *curIt;
   MC2Point p0( -1, -1 );
   MC2Point p1( -1, -1 );
   MC2Point factor( -1, -1 );
   

   m_leftSide.clear();
   m_rightSide.clear();

   static const int LOWER_RIGHT = 1;
   static const int LOWER_LEFT = 0;
   static const int UPPER_LEFT = 3;
   static const int UPPER_RIGHT = 2;

   m_polylineRect.clear();
   m_prevPolylineRect.clear();

   while ( nextIt != end ) {
//      cout << "Iteration." << endl;
      m_prevPolylineRect = m_polylineRect;
      m_polylineRect.clear();
      PolyLine::calculatePolyLine(*curIt, *nextIt, lineWidth, 
                                  m_polylineRect, factor);
     
      if (curIt == begin) {
         // First point. 
         // Add the initial triangle point.
         m_leftSide.push_back(prevPoint - factor);
         // Simply add the first set of points to left and right side.
         m_leftSide.push_back(m_polylineRect[LOWER_LEFT]);
         m_rightSide.push_back(m_polylineRect[LOWER_RIGHT]);
      } else {
         // Check if we turn to the right or two the left.
         int turn = InsideUtil::isLeft_int( prevPoint, *curIt, *nextIt );
         if ( turn > 0 ) {
            // Right turn
//            cout << "Right turn." << endl;
            MC2Point intersection;
            // Calculate intersection on the right hand side.
            GfxUtility::IntersectionResult res = GfxUtility::getIntersection(
                  m_prevPolylineRect[LOWER_RIGHT],
                  m_prevPolylineRect[UPPER_RIGHT],
                  m_polylineRect[LOWER_RIGHT],
                  m_polylineRect[UPPER_RIGHT],
                  intersection);
         
            if (res != GfxUtility::LINE_INTERSECTING) {
//               cout << "Not intersecting" << endl;
               // Not intersecting. A bit strange actually.
               m_leftSide.push_back(m_polylineRect[LOWER_LEFT]);
               m_rightSide.push_back(m_polylineRect[LOWER_RIGHT]);
            } else {
//               cout << "Intersecting." << endl;
               // Lines intersecting as expected.
               m_rightSide.push_back(intersection);
               m_leftSide.push_back(m_prevPolylineRect[UPPER_LEFT]);
               m_leftSide.push_back(m_polylineRect[LOWER_LEFT]);
            }
            
         } else if ( turn < 0 ) {
            // Left turn
//            cout << "Left turn." << endl;
            MC2Point intersection;
            // Calculate intersection on the left hand side.
            GfxUtility::IntersectionResult res = GfxUtility::getIntersection(
                  m_prevPolylineRect[LOWER_LEFT],
                  m_prevPolylineRect[UPPER_LEFT],
                  m_polylineRect[LOWER_LEFT],
                  m_polylineRect[UPPER_LEFT],
                  intersection);
         
            if (res != GfxUtility::LINE_INTERSECTING) {
//               cout << "Not intersecting" << endl;
               // Not intersecting. A bit strange actually.
               m_leftSide.push_back(m_polylineRect[LOWER_LEFT]);
               m_rightSide.push_back(m_polylineRect[LOWER_RIGHT]);
            } else {
//               cout << "Intersecting" << endl;
               m_leftSide.push_back(intersection);
               m_rightSide.push_back(m_prevPolylineRect[UPPER_RIGHT]);
               m_rightSide.push_back(m_polylineRect[LOWER_RIGHT]);
            }

         } else {
            // No turn.
            // Just add the next two points.
            m_leftSide.push_back(m_polylineRect[LOWER_LEFT]);
            m_rightSide.push_back(m_polylineRect[LOWER_RIGHT]);
         }
      }

      prevPoint = *curIt;
      curIt = nextIt;
      ++nextIt;
   }  

   // Add the last points.
   m_leftSide.push_back(m_polylineRect[UPPER_LEFT]);
   m_rightSide.push_back(m_polylineRect[UPPER_RIGHT]);
   
   // Add the last triangle point.
   m_leftSide.push_back(*curIt + factor);

   // Add the left and right hand side of the polygon to m_linePolygon.
   m_linePolygon.clear();
   m_linePolygon.reserve(m_leftSide.size() + m_rightSide.size());
   for (uint32 i = 0; i < m_leftSide.size(); ++i) {
      m_linePolygon.push_back(m_leftSide[i]);
   }

   for (int j = m_rightSide.size() - 1; j >= 0; --j) {
      m_linePolygon.push_back(m_rightSide[j]);
   }
  
//   cout << "Outdata:" << endl;
//   {for (std::vector<MC2Point>::const_iterator it = m_linePolygon.begin(); it != m_linePolygon.end(); ++it) {
//      cout << *it << endl; 
//   }}

   project3d( m_linePolygon.begin(), m_linePolygon.end(), m_points3d );
   PlotterAdapter::drawPolygon( m_points3d.begin(), m_points3d.end() );
//   PlotterAdapter::drawPolygon( m_linePolygon.begin(), m_linePolygon.end() );
//   PlotterAdapter::drawPolygon( begin, end );
}

void 
PlotterAdapter3d::drawBitMap( const MC2Point& center,
                            const BitMap* bitMap)
{
   // Let's reuse the clipped points member for the point.
   m_bitmapCenterPoint.clear();
   m_bitmapCenterPoint.push_back( center );
   project3dNoCopy( m_bitmapCenterPoint.begin(),
                    m_bitmapCenterPoint.end(), m_points3d );
   
   if ( m_points3d.empty() ) {
      return;
   }
   const MC2Point& p = m_points3d.front();
   if ( p.getX() < 0 || p.getX() > m_width ||
        p.getY() > m_height || p.getY() < (int)m_settings.m_horizonPixels ) {
      // Outside the map area, don't draw.
      return;
   }
   m_plotter->drawBitMap( p, bitMap );
}

void PlotterAdapter3d::checkScreenAndUpdateMembers( int scalefactor )
{   
   isab::Rectangle sizePixels;
   m_plotter->getMapSizePixels( sizePixels );

   if ( sizePixels == m_screenSizeInPixels && scalefactor == m_scalefactor) {
      return;
   }

   m_screenSizeInPixels = sizePixels;
   m_scalefactor = scalefactor;

   m_width = m_screenSizeInPixels.getWidth() << m_scalefactor;
   m_height = m_screenSizeInPixels.getHeight() << m_scalefactor;
   m_horizHeight = m_settings.m_horizonPixels << m_scalefactor;

   m_intWidth = int(m_width);
   m_intHeight = int(m_height);
   m_intHorizHeight = int(m_horizHeight);

   
   mc2dbg << "width = " << m_width << ", height = " << m_height << endl;

   // m_width = k * m_height
   float k = m_width / m_height;
   float a = m_height*sin(m_settings.m_alpha);
   float b = m_height*cos(m_settings.m_alpha);

   // The distance from the reference point (rotation point) and the eye.
   float d = ( m_width/2 ) / tan( m_settings.m_delta );
   float tanbeta = ( m_width/2 ) / ( b + d );
   float tan_phiy_by_two = sin(m_settings.m_alpha)*tanbeta/k;
   
   // The eye.
   MathVec<3> E;
   E[0] = m_width / 2;
   E[1] = m_height + d * cos( m_settings.m_alpha );
   E[2] = d * sin( m_settings.m_alpha );

   // The reference point is at the bottom of the screen.
   MathVec<3> R;
   R[0] = m_width / 2;
   R[1] = m_height;
   R[2] = 0;
   
   // Should point upwards in the desired image.
   MathVec<3> V;
   V[0] = 0;
   V[1] = -1;
   V[2] = 0;

   // Inverse view direction => Z
   MathVec<3> n = (E - R) / ( E - R ).norm();

   // Points to the right, perpendicular to n => X
   MathVec<3> u = V.cross( n );
   u /= u.norm();

   // Points up like V, but is perpendicular to n and u.
   MathVec<3> v = n.cross( u );

   m_Va = u(0);
   m_Vb = u(1);
   m_Vc = u(2);
   m_Vd = -u(0)*E(0)-u(1)*E(1)-u(2)*E(2);
   m_Ve = v(0);
   m_Vf = v(1);
   m_Vg = v(2);
   m_Vh = -v(0)*E(0)-v(1)*E(1)-v(2)*E(2);
   m_Vi = n(0);
   m_Vj = n(1);
   m_Vk = n(2);
   m_Vl = -n(0)*E(0)-n(1)*E(1)-n(2)*E(2);
   
   vec_t zfront = -d;
   vec_t zback = -(d + b);
   vec_t r = m_width/a;
   
   m_Pa = 1 / (r* tan_phiy_by_two); 
   m_Pb = 1 / tan_phiy_by_two;

   m_Pc = (zfront + zback)/(zfront - zback);
   m_Pd = -2*zfront*zback/(zfront - zback);


   m_Pa_times_m_Vd = m_Pa*m_Vd;
   m_Pb_times_m_Vf = m_Pb*m_Vf;
   m_Pb_times_m_Vh = m_Pb*m_Vh;

   // Check if we need decrease the quality by reducing nbr of bits,
   // but avoiding overflow. Typically needed for hires screens.
   int bits = 12;
   if ( m_screenSizeInPixels.getWidth() > 450 ||
        m_screenSizeInPixels.getHeight() > 450 ) {
        --bits;
   }
   if ( m_screenSizeInPixels.getWidth() > 680 ||
        m_screenSizeInPixels.getHeight() > 680 ) {
        --bits;
   }
   mc2log << "Nbr bits " << bits << endl;
//   mc2log_real << "w = " << m_screenSizeInPixels.getWidth() << ", h = " << m_screenSizeInPixels.getHeight() << endl;

   int factor = 1 << bits;
   m_intPa = int(m_Pa*factor);
   m_intPa_times_m_Vd = int(m_Pa_times_m_Vd*factor);
   m_intPb_times_m_Vf = int(m_Pb_times_m_Vf*factor);
   m_intPb_times_m_Vh = int(m_Pb_times_m_Vh*factor);
   m_intVj = int(m_Vj*factor);
   m_intVl = int(m_Vl*factor);


   
#if 0
   mc2dbg << "m_Va " << m_Va<< endl;
   mc2dbg << "m_Vb " << m_Vb<< endl;
   mc2dbg << "m_Vc " << m_Vc<< endl;
   mc2dbg << "m_Vd " << m_Vd<< endl;
   mc2dbg << "m_Ve " << m_Ve<< endl;
   mc2dbg << "m_Vf " << m_Vf<< endl;
   mc2dbg << "m_Vg " << m_Vg<< endl;
   mc2dbg << "m_Vh " << m_Vh<< endl;
   mc2dbg << "m_Vi " << m_Vi<< endl;
   mc2dbg << "m_Vj " << m_Vj<< endl;
   mc2dbg << "m_Vk " << m_Vk<< endl;
   mc2dbg << "m_Vl " << m_Vl<< endl;
   mc2dbg << "m_Pa " << m_Pa << endl;
   mc2dbg << "m_Pb " << m_Pb << endl;
   mc2dbg << "m_Pc " << m_Pc << endl;
   mc2dbg << "m_Pd " << m_Pd << endl;
#endif

   MC2BoundingBox clipBox;
   clipBox.update( 0, 0 );
   clipBox.update( int(m_height), int(m_width) );
   m_clipBox = clipBox;

}

void
PlotterAdapter3d::project3dNoCopyf( std::vector<MC2Pointf>::iterator begin,
                                    std::vector<MC2Pointf>::iterator end,
                                    std::vector<MC2Pointf>& res,
                                    int scalefactor )
{
   // Do the needful.

#ifdef __unix__
#undef mc2dbg
   //#define mc2dbg cout
#define mc2dbg mc2dbg8
#endif
   
   res.clear();
   res.reserve( std::distance( begin, end ) );

   checkScreenAndUpdateMembers( scalefactor );
   
   mc2dbg << "M_Height: " << m_height << " m_width: " << m_width << endl;
   
   mc2dbg << "3D: tmppoly = [];" << endl;
   for (std::vector<MC2Pointf>::const_iterator it = begin; it != end; ++it ) {

      MathVec<3> origcoord;
      origcoord[ 0 ] = it->x;
      origcoord[ 1 ] = it->y;
      origcoord[ 2 ] = 0;

      vec_t x = -m_Pa*origcoord(0) + m_Pa_times_m_Vd;
      vec_t y = m_Pb_times_m_Vf*origcoord(1) + m_Pb_times_m_Vh;
      vec_t w = -m_Vj*origcoord(1) -m_Vl;

      // Fix points.
      
      if ( w == 0 ) {
         MC2_ASSERT( false );
         continue;
      }

      x = x/w;
      y = y/w;
//      z = z/w;
      
      float xw = m_width*(-x+1)/2;
      float yw = m_horizHeight + (m_height - m_horizHeight)*(-y+2)/2;

      MC2Pointf p = { xw, yw };

      res.push_back( p ); 
   }
}

void
PlotterAdapter3d::project3dNoCopy( std::vector<MC2Point>::iterator begin,
                                   std::vector<MC2Point>::iterator end,
                                   std::vector<MC2Point>& res,
                                   int scalefactor )
{
   // Do the needful.

#ifdef __unix__
   #undef mc2dbg
   //#define mc2dbg cout
   #define mc2dbg mc2dbg8
#endif
   
   res.clear();
   res.reserve( std::distance( begin, end ) );

   checkScreenAndUpdateMembers( scalefactor );
   
   m_clippedPoints.clear();
   m_clippedPoints.insert( m_clippedPoints.end(), begin, end );

   m_clipUtil.clipPolyToBBoxFast( m_clipBox, m_clippedPoints );
   
   begin = m_clippedPoints.begin();
   end = m_clippedPoints.end();

   mc2dbg << "M_Height: " << m_height << " m_width: " << m_width << endl;
   
   mc2dbg << "3D: tmppoly = [];" << endl;
   for (std::vector<MC2Point>::const_iterator it = begin; it != end; ++it ) {


      int intorigx = it->getX();
      int intorigy = it->getY();

      int intx = -m_intPa*intorigx + m_intPa_times_m_Vd;
      int inty = m_intPb_times_m_Vf*intorigy + m_intPb_times_m_Vh;
      int intw = -m_intVj*intorigy -m_intVl;




//      if ( fabs(x) >= 1 || fabs(y) >= 1 || fabs(z) >= 1 ) {
//         mc2dbg << "Outside [-1,1]^3 :  x " << x << ",y " << y << ",z " << z << endl;
//      }

      
//      int intxw = intwidth*(-intx/intw+1)/2;
//      int intyw = inthorizHeight + (intheight - inthorizHeight)*(-inty/intw+2)/2;
     

      int nominatorX = (m_intWidth*intx);
      int denominatorX = intw*2;
      
      int tmpIntxw = nominatorX / denominatorX;
      if ((nominatorX % denominatorX) >= (denominatorX/2)) {
         ++ tmpIntxw;
      }

      int intxw = int(-tmpIntxw + m_intWidth/2);

      // Old
//      int intxwold = int(-(m_intWidth*intx)/intw/2 + m_intWidth/2);
      
      
      int nominatorY = ((m_intHeight - m_intHorizHeight)*(inty));
      int denominatorY = intw*2;
      
      int tmpIntyw = (nominatorY / denominatorY);
      if ((nominatorY % denominatorY) >= (denominatorY/2)) {
         ++ tmpIntyw;
      }

      int intyw = int(m_intHorizHeight 
         -tmpIntyw + (m_intHeight - m_intHorizHeight));
         // Old version.
//      int intywold = int(m_intHorizHeight + 
//         ((m_intHeight - m_intHorizHeight)*(-inty))/intw/2 + (m_intHeight - m_intHorizHeight));

#if 0
//      if ( rint(xw) != intxw || rint(yw) != intyw) {

         cout << "xw = " << xw << ", rint(xw) = " << rint(xw) << ", intxw = " << intxw << ", intxwold = " << intxwold
              << ", yw = " << yw << ", rint(yw) = " << rint(yw) << ", intyw = " << intyw << ", intywold = " << intywold << endl;
         if ( w < 0 ) {
            mc2dbg << "w " << w << endl;
         }

//      }
#endif
      MC2Point p( intxw, intyw );
//      MC2Point p( static_cast<int>(rint(xw)), static_cast<int>(rint(yw)) );
      mc2dbg << "orig: " << *it << ", trans: " << p << endl;
//      mc2dbg << "3D: " << "tmppoly = [ tmppoly " << p.getX() << " " << p.getY() << " " << (z+1)/2 << " ];" << endl;

      res.push_back( p ); 
   }
   
   mc2dbg << "3D: newpolys = append( newpolys, tmppoly );" << endl;  
}

void
PlotterAdapter3d::project3d( std::vector<MC2Point>::const_iterator begin,
                             std::vector<MC2Point>::const_iterator end,
                             std::vector<MC2Point>& res,
                             int scalefactor )
{
   std::vector<MC2Point> copiedPoints( begin, end );
   project3dNoCopy( copiedPoints.begin(), copiedPoints.end(), 
                    res, scalefactor );
}

MC2Point
PlotterAdapter3d::transform2DTo3D( const MC2Point& point2D )
{
   
   std::vector<MC2Point> points2D;
   std::vector<MC2Point> points3D;
   points2D.push_back( point2D );

   project3dNoCopy( points2D.begin(), points2D.end(), 
                    points3D );
  
   return ( points3D.front() );
}

MC2Pointf
PlotterAdapter3d::transform2DTo3Df( const MC2Pointf& point2D )
{
   
   std::vector<MC2Pointf> points2D;
   std::vector<MC2Pointf> points3D;
   points2D.push_back( point2D );

   project3dNoCopyf( points2D.begin(), points2D.end(), 
                     points3D );
  
   return ( points3D.front() );
}

MC2Point 
PlotterAdapter3d::transform3DTo2D_slow( const MC2Point& point3D )
{
   checkScreenAndUpdateMembers( 0 );
   
   // Initial value.
   MC2Point point2D = point3D;

#ifdef __unix__
// #undef mc2dbg
// #define mc2dbg cout 
#endif
   mc2dbg << "[PA3D]: transform3DTo2D_slow( " << point3D << " )" << endl;

   MC2Point minPoint = MC2Point( 0, 0 );
   MC2Point maxPoint = MC2Point( m_intWidth, m_intHeight );

   
   MC2Point prevclosestPoint3D = MC2Point(MAX_INT32, MAX_INT32);
   MC2Point closestPoint3D = transform2DTo3D( point2D );
   while ( closestPoint3D != point3D && closestPoint3D != prevclosestPoint3D) {

      // mc2dbg << "minPoint = " << minPoint 
      //        << ", maxPoint = " << maxPoint
      //        << ", prevclosestPoint3D = " << prevclosestPoint3D 
      //        << ", closestPoint3D " << closestPoint3D 
      //        << ", point2D = " << point2D << endl;

      if ( closestPoint3D.getX() > point3D.getX() ) {
         // X too big.
         mc2dbg << "X too big " << endl;
         maxPoint.getX() = point2D.getX();
         point2D.getX() -= (point2D.getX() - minPoint.getX())/2;
      } else if ( closestPoint3D.getX() <  point3D.getX() ) {
         mc2dbg << "X too small " << endl;
         // X too small.
         minPoint.getX() = point2D.getX();
         point2D.getX() += (maxPoint.getX() - point2D.getX())/2;
      } else {
         // X is correct.
      }
    
      if ( closestPoint3D.getY() > point3D.getY() ) {
         // Y too big.
         mc2dbg << "Y too big " << endl;
         maxPoint.getY() = point2D.getY();
         point2D.getY() -= (point2D.getY() - minPoint.getY())/2;
      } else if ( closestPoint3D.getY() <  point3D.getY() ) {
         // Y too small.
         mc2dbg << "Y too small " << endl;
         minPoint.getY() = point2D.getY();
         point2D.getY() += (maxPoint.getY() - point2D.getY())/2;
      } else {
         // Y is correct.
      }
    
      prevclosestPoint3D = closestPoint3D;
      closestPoint3D = transform2DTo3D( point2D );
      // mc2dbg << "Updated point2D = " << point2D
      //        << ", prevclosestPoint3D " << prevclosestPoint3D
      //        << ", closestPoint3D = " << closestPoint3D 
      //        << endl;
   }
 
   if ( closestPoint3D != point3D ) {
      mc2dbg << "[PA3D]: transform3DTo2D_slow failed. 2d point " << point2D
             << ", expected 3D: " << point3D 
             << ", actual 3D: " << closestPoint3D
             << endl;
    }
 
   return point2D;
}

void isab::PlotterAdapter3d::cropString(STRING& text,
                                        unsigned int maxWidth)
{
   m_plotter->cropString(text, maxWidth);
}

void isab::PlotterAdapter3d::drawOverlayText(
   const WFAPI::WFString& text,
   TextCachingBlock& cacheBlock,
   const WFAPI::OverlayItemVisualSpec::TextAlignment alignment,
   WFAPI::FontSpec* font,
   const isab::Rectangle& stringRect,
   const WFAPI::OverlayItemVisualSpec::RGB& textColor)
{
   m_plotter->drawOverlayText(text, cacheBlock, alignment, font,
                              stringRect, textColor);
}

void isab::PlotterAdapter3d::drawImageSpec(const WFAPI::ImageSpec* imgSpec,
                                           const MC2Point& point)
{
   m_plotter->drawImageSpec(imgSpec, point);
}

bool eq(const MC2Pointf& lhs, const MC2Pointf& rhs) {
   static const float EPSILON = 0.00001f;

   return
      fabs(lhs.x - rhs.x) < EPSILON &&
      fabs(lhs.y - rhs.y) < EPSILON;
}

MC2Pointf isab::PlotterAdapter3d::transform3DTo2D_slowf( const MC2Point& point3D )
{
   checkScreenAndUpdateMembers( 0 );
   
   // Initial value.
   MC2Pointf point2D = { point3D.getX(), point3D.getY() };

   MC2Pointf point3Df = { point3D.getX(), point3D.getY() };
   
#ifdef __unix__
// #undef mc2dbg
// #define mc2dbg cout 
#endif
   // mc2dbg << "[PA3D]: transform3DTo2D_slow( " << point3Df << " )" << endl;

   MC2Pointf minPoint = { 0, 0 };
   MC2Pointf maxPoint = { m_intWidth, m_intHeight };
   
   MC2Pointf prevclosestPoint3D = { (float)MAX_INT32, (float)MAX_INT32 };
   
   MC2Pointf closestPoint3D = transform2DTo3Df( point2D );
   
   while ( !eq(closestPoint3D, point3Df) &&
           !eq(closestPoint3D, prevclosestPoint3D) )
   {

      // mc2dbg << "minPoint = " << minPoint 
      //        << ", maxPoint = " << maxPoint
      //        << ", prevclosestPoint3D = " << prevclosestPoint3D 
      //        << ", closestPoint3D " << closestPoint3D 
      //        << ", point2D = " << point2D << endl;

      if ( closestPoint3D.x > point3Df.x ) {
         // X too big.
         mc2dbg << "X too big " << endl;
         maxPoint.x = point2D.x;
         point2D.x -= (point2D.x - minPoint.x)/2.0;
      } else if ( closestPoint3D.x <  point3Df.x ) {
         mc2dbg << "X too small " << endl;
         // X too small.
         minPoint.x = point2D.x;
         point2D.x += (maxPoint.x - point2D.x)/2.0;
      } else {
         // X is correct.
      }
    
      if ( closestPoint3D.y > point3Df.y ) {
         // Y too big.
         mc2dbg << "Y too big " << endl;
         maxPoint.y = point2D.y;
         point2D.y -= (point2D.y - minPoint.y)/2.0;
      } else if ( closestPoint3D.y <  point3Df.y ) {
         // Y too small.
         mc2dbg << "Y too small " << endl;
         minPoint.y = point2D.y;
         point2D.y += (maxPoint.y - point2D.y)/2.0;
      } else {
         // Y is correct.
      }
    
      prevclosestPoint3D = closestPoint3D;
      closestPoint3D = transform2DTo3Df( point2D );
   }

   return point2D;    
}

isab::Font* isab::PlotterAdapter3d::createFont(WFAPI::FontSpec* fontSpec)
{
   return m_plotter->createFont(fontSpec); 
}

void isab::PlotterAdapter3d::deleteFont(isab::Font* font)
{
   return m_plotter->deleteFont(font);
}

void isab::PlotterAdapter3d::releaseFonts()
{
   m_plotter->releaseFonts();
}

