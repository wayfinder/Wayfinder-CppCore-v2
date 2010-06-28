/*
Copyright (c) 1999 - 2010, Vodafone Group Services Ltd
All rights reserved.

Redistribution and use in source and binary forms, with or without modification, are permitted provided that the following conditions are met:

    * Redistributions of source code must retain the above copyright notice, this list of conditions and the following disclaimer.
    * Redistributions in binary form must reproduce the above copyright notice, this list of conditions and the following disclaimer in the documentation and/or other materials provided with the distribution.
    * Neither the name of the Vodafone Group Services Ltd nor the names of its contributors may be used to endorse or promote products derived from this software without specific prior written permission.

THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS" AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE ARE DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT HOLDER OR CONTRIBUTORS BE LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
*/


#include "TextPlacement/OldLinePlacementPolicy.h"
#include "TextPlacement/TextPlacementPolicy.h"
#include "TextPlacement/TextPlacementData.h"
#include "TextPlacement/TextResult.h"
#include "Util/StringCache.h"
#include "ViewPortInterface.h"
#include "NativeTextInterface.h"
#include "OverlapDetector.h"
#include "MapProjection.h"
#include "TileFeature.h"
#include "TileMapTextSettings.h"
#include "InsideUtil.h"

#ifdef __SYMBIAN__
#include <e32math.h>
#endif

#define CORE_LOGGING_MODULE_NAME "OldPlacementPolicy"
// #define CORE_LOGGING_MODULE_LEVEL CL_ALL_FILE
#include "CoreDebugPrint.h"

/**
 *    Determines how much the angle between two segments that
 *    we are placing text on will affect the distance between 
 *    the substrings. A higher value will give a shorter dist.
 *    Resonable values are 1-90. Good values are around 20.
 */
#define TPL_MAGIC_CONST      30

/**
 *    The maximum sumed angles that the cost function will 
 *    alow for a text placed on_line.
 */
#define TPL_MAX_TOTAL_ANGLE  90

/**
 *    The maximum angle that two lines might have before we say 
 *    that we cant place any text here.
 */
#define TPL_MAX_ANGLE        30

/**
 *    The highest cost that we willaccept from the cost function
 *    before we say that this will probably not look good.
 */
#define TPL_MAX_TOTAL_COST   100

static inline int
Pow2( int a ) {
   return a*a;
}

inline int Pow2DistBetweenPoints(const MC2Point& p1, const MC2Point& p2) {
   return
      Pow2( p1.getX() - p2.getX() ) + 
      Pow2( p1.getY() - p2.getY() );   
}

TextPlacementDelegator::OldLinePlacementPolicy::OldLinePlacementPolicy(
   NativeTextInterface* textInterface,
   ViewPortInterface* viewPort,
   TileMapTextSettings& textSettings,
   const MapProjection& mapProjection,
   StringCache& cache,
   OverlapDetector<PixelBox>& overlapDetector) :
   TextPlacementPolicy(textInterface,
                       viewPort,
                       textSettings,
                       mapProjection,
                       cache,
                       overlapDetector)
{
    
}

int
TextPlacementDelegator::OldLinePlacementPolicy::getTextWidth( 
   STRING* nameOfFeature,
   int startIndex,
   int nbrOfChars )
{
   if( nbrOfChars == -1 ) {
      nbrOfChars = 
         m_textInterface->getStringLength( *nameOfFeature ) - 
         startIndex;
      
   }
   if( nbrOfChars <= 0 ) { return 0; }

   MC2Point p( 0, 0 );
   return m_textInterface->getStringAsRectangle(
      *nameOfFeature, p, startIndex, nbrOfChars, 0 ).getWidth();
}


const  
int TextPlacementDelegator::OldLinePlacementPolicy::getLongestSegmentInFeature( 
   const std::vector<MC2Point>& feature ) const
{
   double length = 0;
   int index = 0;
   const int featureSize = feature.size();
   if( featureSize >= 2 ) {      
      for ( int i = 0; i < featureSize - 1; ++i ) {
         // Calc the (²) distance between the endpoints of
         // the current segment in the feature.
         int xDiff = feature[ i ].getX() - feature[ i + 1 ].getX();
         int yDiff = feature[ i ].getY() - feature[ i + 1 ].getY();
         double current_length = (double)(Pow2( xDiff ) + Pow2( yDiff ));
         // If the length of this segment is longer than any that we 
         // have tested before, remember this one.
         if( current_length > length ) { 
            length = current_length;
            index = i;
         }
      }
   } else {
      return -1;
   }
   return index;
}

int
TextPlacementDelegator::OldLinePlacementPolicy::placeOnLine(
   TextPlacementData& data,
   TextResultVec& output)
{
   std::vector<MC2Point> pointsInFeature;
   
   for( int i=0; i < (int)data.pointsInFeature.size(); i++ ) {
      pointsInFeature.push_back( data.pointsInFeature[ i ] );
   }

//   int isInside=0;
//   for( int i=0; i < (int)proxy_pointsInFeature.size(); i++ ) {
//      /*
//       * This clipping is not done in a correct way but it is probably
//       * as good as we need it to be. It will remove any segments
//       * that dosent have any points inside the screen.
//       */
//      int m_screenCoords_pointInside_proxy_pointsInFeature_i_1 = 0;
//      if( i > 1 && 
//            m_screenCoords.pointInside(proxy_pointsInFeature[i-1]) ) {
//         m_screenCoords_pointInside_proxy_pointsInFeature_i_1 = 1;
//      }
//      if( (m_screenCoords.pointInside(proxy_pointsInFeature[i]) ||
//           m_screenCoords.pointInside(proxy_pointsInFeature[i+1])) ||
//          (m_screenCoords.pointInside(proxy_pointsInFeature[i]) ||
//           m_screenCoords_pointInside_proxy_pointsInFeature_i_1)) {
//         pointsInFeature.push_back( proxy_pointsInFeature[ i ] );
//         isInside = 1;
//      } else if( isInside ) {
//         i = (int)proxy_pointsInFeature.size();    
//      }
//   }

   std::vector<int> segmentWidths;
   int longestSegmentInFeature =
      getLongestSegmentInFeature( pointsInFeature );

   if( longestSegmentInFeature == -1 ) {
      return 0;
   }

   int lsifLength = Pow2DistBetweenPoints( 
      pointsInFeature[ longestSegmentInFeature ],
      pointsInFeature[ longestSegmentInFeature + 1 ] ); 
 
   const STRING* font = m_stringCache.getOrCreateString(
      m_textSettings.getLineFont( data.polyLineWidth ).first );

   const int fontSize =
      m_textSettings.getLineFont( data.polyLineWidth ).second;
   
   m_textInterface->setFont( *font, fontSize );
   
   int stringWidth = getTextWidth(
      data.nameOfFeature, 0, -1 );
 
   int lengthOfString = m_textInterface->getStringLength( *data.nameOfFeature );
   std::vector<int> charsOnSegment( (int)pointsInFeature.size() - 1 );
   for( int j=0; j < (int)pointsInFeature.size() - 1; j++) {
      charsOnSegment[ j ] = 0;
   }
 
   stringWidth = Pow2( stringWidth );
   if( lsifLength > stringWidth ) {
      // Trying to place on the longest segment in the feature.
      charsOnSegment[ longestSegmentInFeature ] = lengthOfString;
   } else {
      for( int i = 0; i < (int)pointsInFeature.size() - 1; i++ ) {
         segmentWidths.push_back(
            Pow2DistBetweenPoints(
               pointsInFeature[ i ],
               pointsInFeature[ i + 1 ] ) ); 
      }
      
      int bestCostSoFar = INT_MAX;

      for( int segment = 0; 
           segment < (int)segmentWidths.size();         
           segment++ ) {
         std::vector<int> tempCharsOnSegment;
         tempCharsOnSegment.resize( (int)segmentWidths.size() );

         int nbrOfCharsPossitioned = fillRestOfCharsOnSegment(
            data.nameOfFeature,
            segmentWidths, 
            tempCharsOnSegment, 
            lengthOfString, 
            segment, 0 );
         
         if( nbrOfCharsPossitioned == lengthOfString ) {
            int cost = polyCost( 
               data.nameOfFeature, 
               segmentWidths,
               tempCharsOnSegment,
               pointsInFeature ); 
            if( nbrOfCharsPossitioned != lengthOfString ) {
               cost = INT_MAX;
            }
            
            if( cost < bestCostSoFar ) {
               bestCostSoFar = cost;
               tempCharsOnSegment.swap( charsOnSegment );
            } 
         } else {
            segment = (int)segmentWidths.size();
         }
      }
   }

   int firstSegment = -1;
   for( int k = 0; k < (int)charsOnSegment.size(); k++ ) {
      if( firstSegment == -1 && charsOnSegment[ k ] != 0 ) {
         firstSegment = k;
         k = (int)charsOnSegment.size();
      }
   }
   
   if ( firstSegment < 0 ) {
      return 0;
   }
   
   int reverseText = isTextReversed( pointsInFeature, firstSegment );
   
   return placeOnSegments( data,
                           font,
                           fontSize,
                           pointsInFeature,
                           data.coordsBegin,
                           charsOnSegment,
                           reverseText,
                           output);
}

int
TextPlacementDelegator::OldLinePlacementPolicy::placeOnSegments(
   TextPlacementData& data,
   const STRING* font,
   int fontSize,
   const std::vector<MC2Point>& pointsInFeature,
   TileMapCoords::const_iterator coordsBegin,
   std::vector<int>& charsOnSegment,
   int reverseText,
   TextResultVec& output)
{
   TextResult* tr = new TextResult();

   m_textInterface->setFont( *font, fontSize );
   std::vector<PixelBox> stringPixelBoxes;
   int textStartIdx = 0; 
   // let the direction of the text be controled by the line
   // drawn by pointsInFeature[ 0 ] and 
   // pointsInFeature[ (int)pointsInFeature.size()-1 ].
   int first=-1;
   int last=-1;
   for( int i = 0; i < (int)charsOnSegment.size(); i++ ) {
      if( first == -1 && charsOnSegment[ i ] != 0 ) {
         first = i;
      }
      if( charsOnSegment[ i ] != 0 ) {
         last = i;
      }
   }
   if( first == last) {
      first = -1;
      last = -1;
   }
   int lengthOfString = m_textInterface->getStringLength( *data.nameOfFeature );
   for( int segmentNbr = 0; 
        segmentNbr < (int)charsOnSegment.size(); 
        segmentNbr++ ) {
      if( charsOnSegment[ segmentNbr ] != 0 ){ 
         MC2Point lineBegin = pointsInFeature[ segmentNbr ];
         MC2Point lineEnd = pointsInFeature[ segmentNbr + 1 ];

         MC2Coordinate lineBeginCoord = *(coordsBegin + segmentNbr);
         MC2Coordinate lineEndCoord = *(coordsBegin + segmentNbr + 1);
         
         int p1x = lineBegin.getX();  
         int p1y = lineBegin.getY();
         int p2x = lineEnd.getX();
         int p2y = lineEnd.getY();

         int16 angle = calcAngleForTextOnLine( p1x, p1y, p2x, p2y );
         if( angle == 9999 ) {
            mc2dbg << "angle == 9999" << endl;
            delete( tr );
            return 0;
         }
         
         // Align al texts on a segment in the same way as the first one.
         if ( reverseText == 0 ) {
            angle += 180;
         }
         // Varun's angle increase ccw.
         angle = -angle;
         if ( angle < 0 ) {
            angle += 360; 
         }
         
         // Collect and send the results to our textresults vector.
         tr->setString( data.nameOfFeature );
         tr->setFontName( font );

         uint32 textColor = m_textSettings.getTextColor();
         
         tr->setFontColor( (textColor >> 16 ) & 0xff,
                           (textColor >> 8 ) & 0xff,
                           (textColor >> 0 ) & 0xff );
         tr->setTypeOfString( TileMapNameSettings::on_line );
         tr->setFontSize( fontSize );
         std::vector<isab::Rectangle> stringRectangles;
         int posX = 0;
         int posY = 0;
         int textFirstIdx = 0;
         for( int i = 0; i < first; i++ ) {
            textFirstIdx += charsOnSegment[ i ];
         }
         int textLastIdx = 0;
         for( int j = 0; j < last; j++ ) {
            textLastIdx += charsOnSegment[ j ];
         }
         
         // Put text to the left.
         if( segmentNbr == first ) {
            mc2dbg4 << "left" << endl;
            // Compensate for the angle between line 1 and 2.
            double angleDiff = 0;
            if(  segmentNbr + 2 < (int)pointsInFeature.size() ) {
               int fp1x = pointsInFeature[ segmentNbr ].getX();
               int fp1y = pointsInFeature[ segmentNbr ].getY();
               int fp2x = pointsInFeature[ segmentNbr + 1 ].getX();
               int fp2y = pointsInFeature[ segmentNbr + 1 ].getY();
               int fp3x = pointsInFeature[ segmentNbr + 2 ].getX();
               int fp3y = pointsInFeature[ segmentNbr + 2 ].getY();

               angleDiff =
                  calcAngleForTextOnLine( fp1x, fp1y, fp2x, fp2y ) -
                  calcAngleForTextOnLine( fp2x, fp2y, fp3x, fp3y );
               angleDiff = abs(int( (int)angleDiff ) );
               if ( angleDiff > 180 ) { 
                  angleDiff = 360 - angleDiff; 
               }
            } 
            double subTextWidth = getTextWidth( data.nameOfFeature,
                                                textFirstIdx, charsOnSegment[ segmentNbr ] ); 

            // 15 is a magic constant.
            double faktor = ( subTextWidth + ( angleDiff /
                                               TPL_MAGIC_CONST) ) / ( sqrt( (double) 
                                                                          Pow2DistBetweenPoints( lineBegin, lineEnd ) ) * 2.0 );
            posX = (int)( ( lineBegin.getX() * faktor ) + 
                          ( lineEnd.getX() * ( 1 - faktor ) ) + 0.0 );
            posY = (int)( ( lineBegin.getY() * faktor ) + 
                          ( lineEnd.getY() * ( 1 - faktor ) ) + 0.0 );

            // Put text to the right.
         } else if( segmentNbr == last ) {
            mc2dbg4 << "right" << endl;
            double angleDiff = 0;
            int lp1x = pointsInFeature[ segmentNbr - 1 ].getX();
            int lp1y = pointsInFeature[ segmentNbr - 1 ].getY();
            int lp2x = pointsInFeature[ segmentNbr ].getX();
            int lp2y = pointsInFeature[ segmentNbr ].getY();
            int lp3x = pointsInFeature[ segmentNbr + 1 ].getX();
            int lp3y = pointsInFeature[ segmentNbr + 1 ].getY();

            angleDiff =
               calcAngleForTextOnLine( lp1x, lp1y, lp2x, lp2y ) -
               calcAngleForTextOnLine( lp2x, lp2y, lp3x, lp3y );
            angleDiff = abs(int( (int)angleDiff ) );
            if ( angleDiff > 180 ) { 
               angleDiff = 360 - angleDiff; 
            }

            double subTextWidth = getTextWidth( data.nameOfFeature,
                                                textLastIdx, charsOnSegment[ segmentNbr ] ); 

            // 15 is a magic constant.
            double faktor = (subTextWidth + ( angleDiff / 
                                              TPL_MAGIC_CONST ) ) / ( sqrt( (double) 
                                                                         Pow2DistBetweenPoints( lineBegin, lineEnd ) ) * 2.0 );

            posX = (int)( ( lineBegin.getX() * ( 1 - faktor ) ) +
                          ( lineEnd.getX() * faktor ) + 0.0 );
            posY = (int)( ( lineBegin.getY() *  ( 1 - faktor ) ) +
                          ( lineEnd.getY() * faktor ) + 0.0 );

            // Put text in center.
         } else {
            mc2dbg4 << "center" << endl;
            posX = (int)( ( ( lineBegin.getX() +
                              lineEnd.getX() ) / 2 ) + 0.0 );
            posY = (int)( ( ( lineBegin.getY() + 
                              lineEnd.getY() ) / 2 ) + 0.0 );
         }
         MC2Point pos( posX, posY );
         if( reverseText == 0 ) {
            m_textInterface->getStringAsRectangles(
               stringRectangles,
               *data.nameOfFeature,
               pos,
               textStartIdx, 
               charsOnSegment[ segmentNbr ],
               angle );
         } else {
            m_textInterface->getStringAsRectangles(
               stringRectangles,
               *data.nameOfFeature,
               pos,
               lengthOfString - textStartIdx -
               charsOnSegment[ segmentNbr ],
               charsOnSegment[ segmentNbr ],
               angle );
         }
         for( int k = 0; k < (int)stringRectangles.size(); k++ ) { 
            stringPixelBoxes.push_back( stringRectangles[ k ] );
         }
         
         MC2Point posOfString( posX, posY );
         MC2Coordinate worldCoord;
         toWorld( worldCoord, posOfString );
         
         if( reverseText == 0 ) {
            TextPlacementNotice tpn( posOfString,
                                     worldCoord,
                                     textStartIdx, 
                                     textStartIdx + charsOnSegment[ segmentNbr ], 
                                     angle,
                                     lineEndCoord,
                                     lineBeginCoord);
            tr->addTPN( tpn );
         } else {
            TextPlacementNotice tpn( posOfString,
                                     worldCoord,
                                     lengthOfString - textStartIdx - 
                                     charsOnSegment[ segmentNbr ], 
                                     lengthOfString - textStartIdx,
                                     angle,
                                     lineBeginCoord,
                                     lineEndCoord);
            tr->addTPN( tpn );
         }
         textStartIdx += charsOnSegment[ segmentNbr ];
      }
      mc2dbg4 << endl;
   }
   int nbrOfstringRectangles_inside_m_screenCoords = 0;
   for( int k = 0; k < (int)stringPixelBoxes.size(); k++ ) {
      if( !( stringPixelBoxes[k].inside(m_screenCoords) ) ) {
         nbrOfstringRectangles_inside_m_screenCoords++;
      }
   }
  
   int halfNbrOfRectangles = (int)stringPixelBoxes.size() / 2;

   // This should be fetched from the MapPlotter for this actual
   // text. Right to left text means "wrong" drawing order, i.e.
   // arabic, hebrew or similar. We only support placing text in
   // the wrong drawing order with one text segment (since ABC DEF
   // -> CBA FED instead of FEDCBA)
   bool leftToRightText = 
      m_textInterface->textDirectionLeftToRight( *data.nameOfFeature );
   

   if( tr->getTPN().size() > 0 &&
       nbrOfstringRectangles_inside_m_screenCoords < 
       halfNbrOfRectangles && 
       m_overlapDetector.addIfNotOverlapping( stringPixelBoxes ) &&
       // OK, if wrong text drawing order and only one string
       (!leftToRightText && tr->getTPN().size() == 1 ||
        // Also OK if normal text drawing order and many strings
        leftToRightText ))
   {
      coreprintln("Failed to place %s", data.mc2String.c_str());
      output.push_back( tr );
   } else {
      coreprintln("Successfully placed %s", data.mc2String.c_str());
      delete tr;
      return 0;
   }

   return 1;
} 


int
TextPlacementDelegator::OldLinePlacementPolicy::fillRestOfCharsOnSegment(
   STRING* nameOfFeature,
   const std::vector<int>& segmentWidths,
   std::vector<int>& tempCharsOnSegment,
   int stringLength,
   int startSegment,
   int startChar )
{
   for( int i = startSegment; i < (int)segmentWidths.size(); i++ ) {
      if( startChar >= stringLength ) {
         return startChar;
      }
      tempCharsOnSegment[ i ] = 
         nbrOfCharsThatFitsInSeg(
            nameOfFeature, stringLength,
            segmentWidths[ i ], startChar );
      startChar += tempCharsOnSegment[ i ];
      if( tempCharsOnSegment[ i ] == 0 ) {
         return -1;
      }
   }
   return startChar;
}

int
TextPlacementDelegator::OldLinePlacementPolicy::nbrOfCharsThatFitsInSeg(
   STRING* nameOfFeature,
   int stringLength,
   int segmentWidth,
   int startChar )
{  
   if( startChar >= stringLength ) {
      return 0;
   }
   if( segmentWidth <= 0 ) {
      return 0;
   }
   int nbrOfCharsUsedSoFar = 0;
   int charsWidth = getTextWidth( nameOfFeature, 
                                  startChar, nbrOfCharsUsedSoFar );
   
   int firstCharWidth = getTextWidth( nameOfFeature,
                                      startChar, 1 );
   int lastCharWidth = getTextWidth( nameOfFeature,
                                     startChar + nbrOfCharsUsedSoFar, 1 );

   if( startChar + nbrOfCharsUsedSoFar >= stringLength ) {
      return stringLength - startChar;
   }

   while( segmentWidth > Pow2( charsWidth + 
                               ( firstCharWidth + lastCharWidth ) / 2 ) ) {

      nbrOfCharsUsedSoFar++;
      if( startChar + nbrOfCharsUsedSoFar >= stringLength ) {
         return stringLength - startChar;
      }
      charsWidth = getTextWidth( nameOfFeature, 
                                 startChar, nbrOfCharsUsedSoFar );
      lastCharWidth = getTextWidth( nameOfFeature,
                                    startChar + nbrOfCharsUsedSoFar, 1 );
   }
   return nbrOfCharsUsedSoFar;
}

int
TextPlacementDelegator::OldLinePlacementPolicy::polyCost(
   STRING* nameOfFeature,
   const std::vector<int>& segmentWidths, 
   const std::vector<int>& charsOnSegment,
   const std::vector<MC2Point>& pointsInFeature )
{
   int startIdx = 0;
   int cost = 0;
   int totalAngle = 0;
   int atLeastOneSegmentHaveCharsThatNeedsPlacment = 0;
   // For all segments.
   for( int i = 0; i < (int)segmentWidths.size()-1; i++ ) {
      if( charsOnSegment[ i ] != 0 ) {
         atLeastOneSegmentHaveCharsThatNeedsPlacment = 1;
      }
      
      if( i > 0 && i < (int)segmentWidths.size() - 1 ) {
         if( ( charsOnSegment[ i - 1 ] != 0 ) &&
             ( charsOnSegment[ i ] == 0 ) &&
             ( charsOnSegment[ i + 1 ] != 0 ) ) {
            mc2dbg << "We have atleast one segment that are empty within "
                   << "the segments that we are placing text on." << endl;
            return INT_MAX;
         }
         int subStringOnSegWidth = getTextWidth(
            nameOfFeature, 
            startIdx, 
            charsOnSegment[ i ] );
         startIdx += charsOnSegment[ i ];
         if( ( charsOnSegment[ i ] != 0 ) &&
             ( charsOnSegment[ i - 1 ] != 0 ) &&
             ( charsOnSegment[ i + 1 ] != 0 ) ) {
            cost += abs(int( Pow2( subStringOnSegWidth ) - 
                             segmentWidths[ i ] ) );
         }
      }
      if( i < (int)segmentWidths.size() - 1 ) {
         if( ( charsOnSegment[ i ] != 0 ) &&
             ( charsOnSegment[ i + 1 ] != 0 ) ) {
            int p1x = pointsInFeature[ i ].getX();
            int p1y = pointsInFeature[ i ].getY();
            int p2x = pointsInFeature[ i + 1 ].getX();
            int p2y = pointsInFeature[ i + 1 ].getY();
            int a12Cost = calcAngleForTextOnLine( p1x, p1y, p2x, p2y );
            int p3x = pointsInFeature[ i + 2 ].getX();
            int p3y = pointsInFeature[ i + 2 ].getY();
            int a23Cost = calcAngleForTextOnLine( p2x, p2y, p3x, p3y );

            int a13Cost = abs(int( a12Cost - a23Cost ) );
            if ( a13Cost > 180 ) { a13Cost = 360 - a13Cost; }
            totalAngle += a13Cost;
            if ( a13Cost > TPL_MAX_ANGLE || totalAngle > TPL_MAX_TOTAL_ANGLE ) {
               return INT_MAX;
            }
            cost += a13Cost;
         }
      }
      if( cost > TPL_MAX_TOTAL_COST ) {
         return INT_MAX;
      }
   }
   if( atLeastOneSegmentHaveCharsThatNeedsPlacment ) {
      return cost;
   } else {
      return INT_MAX;
   }
}

int
TextPlacementDelegator::OldLinePlacementPolicy::isTextReversed(
   const std::vector<MC2Point>& pointsInFeature,
   int firstSegment )
{
   const MC2Point &lineBegin = pointsInFeature[ firstSegment ];
   const MC2Point &lineEnd = pointsInFeature[ firstSegment + 1 ];
   int p1x = lineBegin.getX();  
   int p1y = lineBegin.getY();
   int p2x = lineEnd.getX();
   int p2y = lineEnd.getY();

   int16 angle = calcAngleForTextOnLine( p1x, p1y, p2x, p2y );
   if( angle == 9999 ) {
      mc2dbg << "angle == 9999" << endl;
   }
   if( ( angle > -90 ) && ( angle < 90 ) ) {
      return 1;
   }
   return 0;
}

int16 
TextPlacementDelegator::OldLinePlacementPolicy::calcAngleForTextOnLine( 
   int p1x, 
   int p1y, 
   int p2x, 
   int p2y )
{
   double xDiff = p1x - p2x;
   double yDiff = p1y - p2y;
   
   if ( xDiff == 0 && yDiff == 0 ) {
      // x and y diff is 0 means no angle.
      return 9999;
   }
  
   // Calculate at what angle the line p1-p2 is.
#ifdef __SYMBIAN__
   TReal atanRes;
   Math::ATan( atanRes, yDiff, xDiff );
#else
   double atanRes = atan2( yDiff, xDiff );
#endif
   int16 angle = (int16)( ( atanRes / M_PI ) * 180 );
   return angle;
}
 
