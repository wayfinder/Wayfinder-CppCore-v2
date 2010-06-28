/*
Copyright (c) 1999 - 2010, Vodafone Group Services Ltd
All rights reserved.

Redistribution and use in source and binary forms, with or without modification, are permitted provided that the following conditions are met:

    * Redistributions of source code must retain the above copyright notice, this list of conditions and the following disclaimer.
    * Redistributions in binary form must reproduce the above copyright notice, this list of conditions and the following disclaimer in the documentation and/or other materials provided with the distribution.
    * Neither the name of the Vodafone Group Services Ltd nor the names of its contributors may be used to endorse or promote products derived from this software without specific prior written permission.

THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS" AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE ARE DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT HOLDER OR CONTRIBUTORS BE LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
*/


#ifndef TEXTPLACEMENTPOLICY_H
#define TEXTPLACEMENTPOLICY_H

#include "TextPlacement/TextPlacementDelegator.h"
#include "PixelBox.h"


struct TextPlacementData;
class PixelBox;
class MC2Coordinate;
class MC2Point;

template <class T> class OverlapDetector;

class TextPlacementDelegator::TextPlacementPolicy {
public:
   /**
    *  Since the text interface can change during the execution
    *  of the program, we must include a setter function for this
    *  member.
    *
    *  @param textInterface The new text interface to use.
    */ 
   void setTextInterface(NativeTextInterface* textInterface);

   /**
    *  Since the viewport interface can change during the
    *  execution of the program, we must include a setter
    *  function for this member.
    *
    *  @param viewPort The new viewport to use.
    */ 
   void setViewPortInterface(ViewPortInterface* viewPort);

   /**    
    *    Place some text horizontal and puts the results in
    *    output.
    *
    *    @param data The data to base the placement upon,
    *                @see TextPlacementData.
    *                
    *    @param output The output will be placed at the end of this vector.
    *    
    *    @return -1 If the data was invalid, 0 if unable to place, 1
    *               if successfully placed.
    */
   int placeHorizontal(TextPlacementData& data, TextResultVec& output);


   /**    
    *    Place the text on the feature, but use a "round rect" as
    *    background. This is used for highways for instance (E22..).
    *
    *    @param data The data to base the placement upon,
    *                @see TextPlacementData.
    *                
    *    @param output The output will be placed at the end of this vector.
    *    
    *    @return -1 If the data was invalid, 0 if unable to place, 1
    *               if successfully placed.
    */
   int placeOnRoundRect(TextPlacementData& data, TextResultVec& output);

   /**    
    *    Place the text inside a polygon feature. 
    *
    *    @param data The data to base the placement upon,
    *                @see TextPlacementData.
    *                
    *    @param output The output will be placed at the end of this vector.
    *    
    *    @return -1 If the data was invalid, 0 if unable to place, 1
    *               if successfully placed.
    */
   int placeInsidePolygon(TextPlacementData& data, TextResultVec& output);

   /**
    *  The line implementation differs so implement this by derived classes.
    */ 
   virtual int placeOnLine(TextPlacementData& data, TextResultVec& output) = 0;

   /**
    *   Returns a string representation of the policy. Should be implemented
    *   by each base class that provides a custom line placing strategy.
    *
    *   @return The string representation of the placement policy.
    */ 
   virtual const char* getName() = 0;

   /**
    *   Resets the internal viewport dimensions by querying the
    *   viewport interface.
    */ 
   void resetViewport();

protected:   
   TextPlacementPolicy(NativeTextInterface* textInterface,
                       ViewPortInterface* viewPort,
                       TileMapTextSettings& textSettings,
                       const MapProjection& mapProjection,
                       StringCache& cache,
                       OverlapDetector<PixelBox>& overlapDetector);
   
   /**
    *    Converts the dest coordinate to a world coordinate
    *    using the transformMatrix and returns a reference to dest.
    */
   MC2Coordinate& toWorld(MC2Coordinate& dest, const MC2Point& src);
   
   /**
    *    A pixel box that tells us how big the screen is.
    *    Is used to determine if a text will be drawn or not.
    */
   PixelBox m_screenCoords;
   
   NativeTextInterface* m_textInterface;
   ViewPortInterface* m_viewPort;
   TileMapTextSettings& m_textSettings;
   const MapProjection& m_mapProjection;
   StringCache& m_stringCache;
   OverlapDetector<PixelBox>& m_overlapDetector;
private:
   enum {
      c_heightDisplacmentOftextOverCityCenter = 12,
   };
};

#endif /* TEXTPLACEMENTPOLICY_H */
