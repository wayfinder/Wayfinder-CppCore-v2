/*
Copyright (c) 1999 - 2010, Vodafone Group Services Ltd
All rights reserved.

Redistribution and use in source and binary forms, with or without modification, are permitted provided that the following conditions are met:

    * Redistributions of source code must retain the above copyright notice, this list of conditions and the following disclaimer.
    * Redistributions in binary form must reproduce the above copyright notice, this list of conditions and the following disclaimer in the documentation and/or other materials provided with the distribution.
    * Neither the name of the Vodafone Group Services Ltd nor the names of its contributors may be used to endorse or promote products derived from this software without specific prior written permission.

THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS" AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE ARE DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT HOLDER OR CONTRIBUTORS BE LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
*/


#ifndef OLDLINEPLACEMENTPOLICY_H
#define OLDLINEPLACEMENTPOLICY_H

#include "TextPlacement/TextPlacementPolicy.h"
#include "MapPlotterConfig.h"
#include "TileMapCoord.h"
#include <vector>

class MC2Point;

class TextPlacementDelegator::OldLinePlacementPolicy :
   public TextPlacementDelegator::TextPlacementPolicy
{
public:
   OldLinePlacementPolicy(NativeTextInterface* textInterface,
                          ViewPortInterface* viewPort,
                          TileMapTextSettings& textSettings,
                          const MapProjection& mapProjection,
                          StringCache& cache,
                          OverlapDetector<PixelBox>& overlapDetector);

   /**
    *  
    */ 
   virtual int placeOnLine(TextPlacementData& data, TextResultVec& output);

   virtual const char* getName() {
      return "Old style";
   }
private:
   /**
    *    Calculates the angle used to draw text. Caluclations
    *    are based on the line made up by p1 and p2. The angle
    *    should be between -180 and 180.
    *    @param p1x The x value for the first point.
    *    @param p1y The y value for the first point.
    *    @param p2x The x value for the second point.
    *    @param p2y The y value for the second point.
    *    @return the angle.
    */
   int16 calcAngleForTextOnLine(int p1x, 
                                int p1y, 
                                int p2x, 
                                int p2y );

   /**
    *    Tests if the text that we wants to place on the polygon
    *    should be reversed in order to be drawn correcty or not.
    *    @ param pointsInFeature the polygon.
    *    @return 0 if normal and 1 if we need to possition the text
    *    in revers order.
    */
   int isTextReversed(const std::vector<MC2Point>& pointsInFeature,
                      int firstSegment );

   /**
    *    Putts some text on a polygon using NbrOfCharsThatFitsInSeg.
    *    @param nameOfFeature the string.
    *    @param segmentWidth the width of the segment we are trying
    *    to place the string on.
    *    @param tempCharsOnSegment number of chars on each linesegment.
    *    @param stringLength the length of the string in chars.
    *    @param startSegment the index in the polygon telling us
    *    at what segment we should start to place text.
    *    @param startChar the indes in the string that we wants to 
    *    start to place text from.
    *    @return number of chars that we could possition.
    *
    */
   int fillRestOfCharsOnSegment(STRING* nameOfFeature,
                                const std::vector<int>& segmentWidths,
                                std::vector<int>& tempCharsOnSegment,
                                int stringLength,
                                int startSegment,
                                int startChar );

   /**
    *    Given some info this function tells us how many characters 
    *    we can fitt on a line. 
    *    @param nameOfFeature the string that we wants to place on
    *    the line.
    *    @param stringLength the length of the string in chars.
    *    @param segmentWidth the width of the segment we are trying
    *    to place the string on.
    *    @param startChar tells us what char in the string that 
    *    should be the first on the line.
    *    @return the number of chars that could be fitted on the
    *    line.
    */
   int nbrOfCharsThatFitsInSeg(STRING* nameOfFeature,
                               int stringLength,
                               int segmentWidth,
                               int startChar );

   /**
    *    An estimation of how good a given placment of some text on
    *    a polygon is.
    *    @param nameOfFeature the string.
    *    @param charsOnSegment number of chars on each linesegment.
    *    @param the polygon.
    *    @return the total cost fo this indata.
    */
   int polyCost(STRING* nameOfFeature,
                const std::vector<int>& segmentWidths, 
                const std::vector<int>& charsOnSegment,
                const std::vector<MC2Point>& pointsInFeature );

   /**
    *    Returns the width in pixels of a given string.
    *    @param nameOfFeature The text that we wants to draw.
    *    @param startIndex the index that we will start at in the
    *    string. That is if we are getting tha width for a 
    *    substring of the string.
    *    @param nbrOfChars number of character that we will be
    *    conserned with in the string, starting at startIndex.
    *    @return the widht of the String/Substring in pixels.
    */
   int getTextWidth(STRING* nameOfFeature,
                    int startIndex,
                    int nbrOfChars );

   /**
    *    Returns the index of the start of the longest segment 
    *    in feature.
    *    @param feature A list of points describing the feature.
    *    @param widthOfTextInPixels An int telling us how wide
    *    the string that we are trying to place is.
    */
   const int getLongestSegmentInFeature(
      const std::vector<MC2Point>& feature ) const;

   /**
    *    Placec a substring of a text on a given segment of 
    *    that texts feature.
    *    @param nameOfFeature The text that we wants to draw.
    *    @param font The font that we wants to draw the text with.
    *    @param pointsInFeature A vector containing points that
    *    describe the feature that we wants to put a name on.
    *    @param segmentNbr Tellls us what segment we will be 
    *    working on. 
    *    @param LeftCenterRight Tells us if we want to possition the
    *    substring close to the left edge or the right or the center
    *    of the segment, ei the begining or end or center.
    *    @return tells the caling method if things went well or bad.
    */
   int placeOnSegments(TextPlacementData& data,
                       const STRING* font,
                       int fontSize,
                       const std::vector<MC2Point>& pointsInFeature,
                       TileMapCoords::const_iterator coordsBegin,
                       std::vector<int>& charsOnSegment,
                       int reverseText,
                       TextResultVec& output );
};


#endif /* OLDLINEPLACEMENTPOLICY_H */
