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

#include "UserDefinedScaleFeature.h"

#include "MC2BoundingBox.h"

#include <math.h>
#include <stdio.h>
#include "TileMapUtil.h"
#include "MapPlotter.h"
#include "GfxConstants.h"
#include "MapProjection.h"


using namespace std;

ScaleSettings
UserDefinedScaleFeature::getMilesYardsSettings()
{
   // These must be sorted. Smallest unit first.
   ScaleSettings::unitVector_t meterVect;
   meterVect.push_back(
      ScaleSettings::unitVector_t::value_type( "yds", 0.9144, 100.0 ) );
   meterVect.push_back(
      ScaleSettings::unitVector_t::value_type( "mile", 1609.344, 1.0,
                                               "miles" ) );
                                               

   return ScaleSettings( meterVect );
}

ScaleSettings
UserDefinedScaleFeature::getMilesFeetSettings()
{
   // These must be sorted. Smallest unit first.
   ScaleSettings::unitVector_t meterVect;
//     meterVect.push_back( ScaleSettings::unitVector_t::value_type( "m", 1.0 ) );
//     meterVect.push_back(
//        ScaleSettings::unitVector_t::value_type( "km", 1000.0 ) );
   meterVect.push_back(
      ScaleSettings::unitVector_t::value_type( "ft", 0.3048, 1.0 ) );
//     meterVect.push_back(
//        ScaleSettings::unitVector_t::value_type( "yds", 0.9144, 100.0 ) );
   meterVect.push_back(
      ScaleSettings::unitVector_t::value_type( "mile", 1609.344, 1.0,
                                               "miles" ) );
                                               

   return ScaleSettings( meterVect );
}

ScaleSettings
UserDefinedScaleFeature::getMeterSettings()
{
   // These must be sorted. Smallest unit first.
   ScaleSettings::unitVector_t meterVect;
   meterVect.push_back(
      ScaleSettings::unitVector_t::value_type( "m", 1.0, 1.0 ) );
   meterVect.push_back(
      ScaleSettings::unitVector_t::value_type( "km", 1000.0, 1.0 ) );

   return ScaleSettings( meterVect );
}

ScaleSettings
UserDefinedScaleFeature::getTestScaleSettings()
{
   // These must be sorted. Smallest unit first.
   ScaleSettings::unitVector_t meterVect;
   meterVect.push_back(
      ScaleSettings::unitVector_t::value_type( "aln", 0.594 , 1.0, "alnar" ) );
   meterVect.push_back(
      ScaleSettings::unitVector_t::value_type( "fjärdingsväg",
                                               2672.0 , 1.0, "fjärdingsvägar"
                                               ) );
   meterVect.push_back(
      ScaleSettings::unitVector_t::value_type( "gam. mil",
                                               10688.0 , 1.0 ) );
   
   return ScaleSettings( meterVect );
}

UserDefinedScaleFeature::
UserDefinedScaleFeature( const char* fontName,
                         int fontSize,
                         const ScaleSettings& scaleSettings,
                         int dpiCorrectionFactor) :
      UserDefinedFeature( UserDefinedFeature::scale,
                          vector<MC2Point>(1, MC2Point(0,0)),
                          MC2Point(0,0),
                          dpiCorrectionFactor),
      m_scaleSettings( scaleSettings ),
      m_point ( -1, -1 )
{
   m_fontName = TileMapUtil::newStrDup( fontName );
   m_fontSize = fontSize;
   setClickable( false );
   m_alignment = LEFT_ALIGNMENT;

   m_maxScaleWidth = 0;
   m_scaleHeight = 6;
}

void
UserDefinedScaleFeature::setScale( const ScaleSettings& scaleSettings )
{
   m_scaleSettings = scaleSettings;
}

void
UserDefinedScaleFeature::setAlignment(alignment_t alignment)
{
   m_alignment = alignment;
}

void
UserDefinedScaleFeature::setPoint( const MC2Point& point )
{
   m_point = point;
}

void
UserDefinedScaleFeature::setPosition(int16 posX, int16 posY)
{
   setPoint(MC2Point(posX, posY));
}

void
UserDefinedScaleFeature::setScaleMaxWidth(uint16 scaleWidth)
{
   m_maxScaleWidth = scaleWidth;
}

int
UserDefinedScaleFeature::getScaleMaxWidth() const
{
   return m_maxScaleWidth;
}

void
UserDefinedScaleFeature::setScaleHeight(uint16 scaleHeight)
{
   m_scaleHeight = scaleHeight;
}

int
UserDefinedScaleFeature::getScaleHeight() const
{
   return m_scaleHeight;
}

UserDefinedScaleFeature::~UserDefinedScaleFeature()
{
   delete [] m_fontName;
}

static inline void
makeDrawItemParameters( uint16 width, uint16 height,
                        MC2BoundingBox& bbox,
                        float32& xFactor, 
                        float32& yFactor )
{
   bbox.updateCosLat();
   xFactor = float32(width - 1) / bbox.getLonDiff();
   yFactor = float32(height - 1) / bbox.getHeight();
   
   // Make sure that the image will not have strange proportions.
   float32 factor = float32(bbox.getHeight()) / bbox.getWidth()
      * (width - 1) / (height - 1);
   if (factor < 1) {
      // Compensate for that the image is wider than it is high
      yFactor *= factor;
   } else {
      // Compensate for that the image is higher than it is wide
      xFactor /= factor;
   }
}

//  static int getLinearX( int32 mc2Lon, MC2BoundingBox& bbox, 
//                          float64 xFactor, uint16 width ) 
//  {
//     int32 coord = int32(rint((mc2Lon - bbox.getMinLon()) * xFactor));
         
//     if (coord > (width*2))
//        coord = width*2;
//     else if (coord < (-width*2))
//        coord = -width*2 ;
               
//     return (coord);
//  }


static inline int32
getLon( int x, MC2BoundingBox& bbox, 
        float32 xFactor, uint16 /*width*/ )
{
   return int32( rint( ( (x / xFactor) + bbox.getMinLon() ) ) );
}

int32
UserDefinedScaleFeature::chooseFactor( int32 origDist, int& unitNumber )
{
   float32 dist = origDist;
   const ScaleSettings::unitVector_t& unitVect =
      m_scaleSettings.getUnitVector();
   for ( int i = unitVect.size() - 1; i >= 0; --i ) {
      float32 distByFactor = dist / unitVect[i].m_factorFromMeters;
      if ( distByFactor >= unitVect[i].m_changeValue ) {
         unitNumber = i;
         dist = distByFactor; 
         return int32(dist);
      }
   }
   unitNumber = 0;
   return int32( origDist );
}

void
UserDefinedScaleFeature::draw( const MapProjection& matrix,
                               isab::MapPlotter& plotter,
                               uint32 textColor )
{
   if ( ! isVisible() ) {
      return;
   }
   
   MC2BoundingBox bbox = matrix.getBoundingBox();
   // Shamelessly copied from GDImageDraw
   MC2Point screenSize = matrix.getScreenSize();
   uint16 width  = screenSize.getX();
   uint16 height = screenSize.getY();
   const float32 scalePartOfWidth = 0.40;
   int bottomMargin = 8;
   int sideMargin = 8;
   int barWidth = 5;
   int textMargin = 2;
   float32 fontSize = 10.0;
   if ( width > 250 ) { // > 250 pxl
      fontSize = 10.0;
      bottomMargin = sideMargin = 6;
   } else if ( width > 150 ) { // 250-151 pxl
      fontSize = 8.0;
      bottomMargin = sideMargin = 4;
      barWidth = 4;
   } else { // less than 150 pxl
      fontSize = 6.0;
      bottomMargin = sideMargin = 2;
      textMargin = 1;
      barWidth = 3;
   }
   
   if(m_maxScaleWidth == 0) {
      m_maxScaleWidth = int( rint(scalePartOfWidth * width));
   }
   int leftPosX = 0;
   int rightPosX = 0;
   switch(m_alignment) {
      case RIGHT_ALIGNMENT:
         rightPosX = m_point.getX();
         leftPosX = rightPosX - m_maxScaleWidth;
         break;
      case LEFT_ALIGNMENT:
         leftPosX = m_point.getX();
         rightPosX = leftPosX + m_maxScaleWidth;
         break;
   }
   int posY = height - m_point.getY();

   float32 xFactor = 0;
   float32 yFactor = 0;
   makeDrawItemParameters( width, height,
                           bbox, xFactor, yFactor );

   MC2Coordinate coord;
   matrix.inverseTranformUsingCosLat(coord, leftPosX, posY);
   double coordCosLat = matrix.getCosLat(coord.lat);

   // Determine leftLon from leftPosX
   int32 leftLon = coord.lon;

   
   if (coord.lat > MAX_INT32 / 2 || coord.lat < MIN_INT32 / 2) {
      /*
       * Then we are above or below the poles, and we do not want to draw a
       * scale beacuse it is not valid.
       */
      return;
   }

   // Determine rightLon from rightPosX
   matrix.inverseTranformUsingCosLat(coord, rightPosX, posY);
   int32 rightLon = coord.lon;
   
   int32 distance = 0;
   
   if(rightLon < leftLon) {
      // Crossing date line. leftlon -> MAX_INT32, MIN_INT32 -> rightLon.
      int leftDistance = static_cast<int>( (MAX_INT32 - leftLon) *
                                           GfxConstants::MC2SCALE_TO_METER *
                                           coordCosLat + 0.5);

      int rightDistance = static_cast<int>( (rightLon - MIN_INT32) *
                                            GfxConstants::MC2SCALE_TO_METER *
                                            coordCosLat + 0.5);

      // Sum leftDistance and rightDistance to get the total distance.
      distance = leftDistance + rightDistance;
      
      
   } else {
      // Normal scale calculation
      distance = static_cast<int>( (rightLon - leftLon) *
                                   GfxConstants::MC2SCALE_TO_METER *
                                   coordCosLat + 0.5);

   }
   // Save distance to use it later.
   int32 orgDist = distance;
   
   // Get unit from unit vector.
   int unitIdx;
   distance = chooseFactor( distance, unitIdx );


   char distStr[128];
   uint32 black = 0x000000;

   //** Make distance nice length
   // First number zeros
   int32 logDist = 10;
   while( distance > logDist ) {
      logDist *= 10;
   }
   logDist /= 10;
   
   // Then some fine tuning
   int32 niceDist = 0;
   // Should be even ^2 but 1 is allowed too
   if ( 2*logDist <= distance ) {
      niceDist = 2*logDist;
   } else {
      niceDist = logDist;
   }
   while ( niceDist + 2*logDist <= distance ) {
      niceDist += 2*logDist;
   }

   using namespace std;

   //** Nice distance as string
   distStr[ 0 ] = '\0';
   sprintf(distStr, "%u %s",
           (unsigned int)niceDist,
           (niceDist == 1) ?
           m_scaleSettings.getUnitVector()[unitIdx].m_name :
           m_scaleSettings.getUnitVector()[unitIdx].m_pluralName );

   // Convert the nice distance back to meters
   niceDist = int32(
      niceDist *
      m_scaleSettings.getUnitVector()[unitIdx].m_factorFromMeters );

   int distanceInPx = static_cast<int>(((double)niceDist / (double)orgDist) *
                                       m_maxScaleWidth + 0.5);
   
   
   switch(m_alignment) {
      case RIGHT_ALIGNMENT:
         leftPosX = rightPosX - distanceInPx;
         break;
      case LEFT_ALIGNMENT:
         rightPosX = leftPosX + distanceInPx;
         break;
         }
  
   // Set the font
   {
      STRING* plotterFont = plotter.createString( m_fontName );   
      plotter.setFont( *plotterFont, m_fontSize );
      plotter.deleteString( plotterFont );
   }

   // Get size of distStr
   STRING* str = plotter.createString( distStr );
   isab::Rectangle strRect =
      plotter.getStringAsRectangle( *str, MC2Point(0,0) );
   
   // Use textArea for textbox
   int textWidth = strRect.getWidth();
   
   // Put textbox right aligned over scale
   int textXCenter = rightPosX - (textWidth >> 1);

   // Make sure that text fits in image
   if ( textXCenter + textWidth/2 + textMargin + sideMargin >= width ) {
      textXCenter = width - textWidth/2 - textMargin - sideMargin;
   }

   // Draw scale
   plotter.setLineWidth(2);
   plotter.setPenColor(black);
   plotter.setCapStyle(isab::MapPlotter::NO_CAP);
   std::vector<MC2Point> points;

   
   // Draw left base line.
   points.push_back(MC2Point(leftPosX, posY));
   int totalLength = rightPosX - leftPosX;
   int lineLeftRightPosX = leftPosX + totalLength/2 - textWidth/2 - 4;
   if(lineLeftRightPosX < leftPosX) {
      lineLeftRightPosX = leftPosX;
   }
#if defined __SYMBIAN32__
   points.push_back(MC2Point(lineLeftRightPosX-2, posY));
   if(lineLeftRightPosX > leftPosX +1)
      plotter.drawPolyLine(points.begin(), points.end());
#else
   points.push_back(MC2Point(lineLeftRightPosX, posY));
   plotter.drawPolyLine(points.begin(), points.end());
#endif
   
   points.clear();


   // Draw right base line
   int lineRightLeftPosX = rightPosX - totalLength/2 + textWidth/2 + 4;
   if(lineRightLeftPosX > rightPosX) {
      lineRightLeftPosX = rightPosX;
   }
   points.push_back(MC2Point(lineRightLeftPosX, posY));   
   
#if defined __SYMBIAN32__
   points.push_back(MC2Point(rightPosX-2, posY));
   if(lineRightLeftPosX < rightPosX - 1)
      plotter.drawPolyLine(points.begin(), points.end());
#else
   points.push_back(MC2Point(rightPosX, posY));
   plotter.drawPolyLine(points.begin(), points.end());
#endif

   points.clear();

   // Draw stop lines in line width 1.
   plotter.setLineWidth(1);

    // Draw left stop line
#if defined __SYMBIAN32__  
   points.push_back(MC2Point(leftPosX, posY + m_scaleHeight/2));
   points.push_back(MC2Point(leftPosX, posY - m_scaleHeight/2+1));
#else
   points.push_back(MC2Point(leftPosX, posY + m_scaleHeight/2 - 1));
   points.push_back(MC2Point(leftPosX, posY - m_scaleHeight/2));
#endif
   plotter.drawPolyLine(points.begin(), points.end());
   points.clear();

   // Draw right stop line
#if defined __SYMBIAN32__
   points.push_back(MC2Point(rightPosX-1, posY + m_scaleHeight/2));
   points.push_back(MC2Point(rightPosX-1, posY - m_scaleHeight/2+1));
#else
   points.push_back(MC2Point(rightPosX-1, posY + m_scaleHeight/2 - 1));
   points.push_back(MC2Point(rightPosX-1, posY - m_scaleHeight/2));
#endif
   plotter.drawPolyLine(points.begin(), points.end());
   points.clear();

   // Draw text.
   plotter.drawText(*str, MC2Point(leftPosX + totalLength/2, posY-1));
   plotter.deleteString( str );
}

int32 UserDefinedScaleFeature::getPositionXLed() const
{
   return m_point.getX(); 
}

int32 UserDefinedScaleFeature::getPositionYLed() const
{
   return m_point.getY(); 
}

