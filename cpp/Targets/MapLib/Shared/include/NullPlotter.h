/*
Copyright (c) 1999 - 2010, Vodafone Group Services Ltd
All rights reserved.

Redistribution and use in source and binary forms, with or without modification, are permitted provided that the following conditions are met:

    * Redistributions of source code must retain the above copyright notice, this list of conditions and the following disclaimer.
    * Redistributions in binary form must reproduce the above copyright notice, this list of conditions and the following disclaimer in the documentation and/or other materials provided with the distribution.
    * Neither the name of the Vodafone Group Services Ltd nor the names of its contributors may be used to endorse or promote products derived from this software without specific prior written permission.

THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS" AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE ARE DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT HOLDER OR CONTRIBUTORS BE LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
*/


#ifndef NULL_PLOTTER_H
#define NULL_PLOTTER_H

/*
 *
 *   /dev/null for plotting.
 *
 */ 

#include "config.h"

#include "MapPlotter.h"
#include "MapPlotterTypes.h"
#include "MC2SimpleString.h"
#include "DrawingContextsConcrete.h"

namespace isab {
   class Rectangle;
   class PointVector;
}

/**
 *   Abstract class representing a vector of coordinates.
 *   To be used as a base class for specialized classes used
 *   by the MapPlotter subclasses.
 *   Should be templated into the MapPlotter later for performance.
 */
class NullPointVector : public isab::PointVector {
public:

   /**
    *   Virtual destructor.
    */
   virtual ~NullPointVector() {};
   
   /**
    *   Adds one coordinate to the vector.
    *   @param x The x coordinate.
    *   @param y The y coordinate.
    */   
   virtual void addPoint( int x, int y ) {}

   /**
    *   Adds many points at once to the vector.
    *   Default implementation just calls the reset mehtod and then addPoint
    *   method for each point.
    */
   virtual void setPoints(const std::vector<MC2Point>& points) {}
   
   /**
    *   Returns the number of coordinates in the vector.
    */
   virtual int getSize() const {
      return 0;
   }

   /**
    *   Resets the addposition ( and size ) back to zero.
    */
   virtual void reset() {
      
   }

   /**
    *   Returns the number of allocated points.
    */
   virtual int getAllocatedSize() const {
      return 0;
   }
};


class NullPlotter : public isab::MapPlotter {
public:
   static WFAPI::DrawingContext* createDrawingContext() {
      PlotterContextConcrete* ret = new PlotterContextConcrete;
      NullPlotter* plotter = new NullPlotter;
      ret->plotter = plotter;
      
      return ret;
   }
   
   virtual ~NullPlotter() {}
   
   virtual void clearScreen() {}
   
   virtual void setBackgroundColor( unsigned int red,
                                    unsigned int green,
                                    unsigned int blue ) {}
   
   virtual void setPenColor( unsigned int red,
                             unsigned int green,
                             unsigned int blue ) {};
   
   virtual void getMapSizePixels(isab::Rectangle& size) const {
      size = isab::Rectangle( 0, 0, 480, 320 );
   };
   
   virtual isab::PointVector* createPointVector(int size) {
      return new NullPointVector;
   };
   
   virtual void deletePointVector(isab::PointVector* vect) {
      delete vect;
   }

   virtual void drawPolygonWithColor( std::vector<MC2Point>::const_iterator begin,
                                      std::vector<MC2Point>::const_iterator end,
                                      uint32 fillcolor ) {};

   virtual bool returnsVisibleSizeOfBitmaps() const {
      return true;
   }

   virtual void setFont( const STRING& fontName, int size ) {};

   virtual STRING* createString( const char* text ) { return NULL; };

   virtual void deleteString( STRING* text ) {};

   virtual int getStringLength( const STRING& text ) { return 0; };
   
   /**
    *   Draws a polyline defined by the array of points.
    */
   virtual void drawPolyLine( std::vector<MC2Point>::const_iterator begin,
                              std::vector<MC2Point>::const_iterator end ) {};
   
   virtual void drawPolyLineWithColor( std::vector<MC2Point>::const_iterator begin,
                                       std::vector<MC2Point>::const_iterator end,
                                       uint32 color,
                                       int lineWidth ) {};

   virtual void drawImageSpec(const WFAPI::ImageSpec*, const MC2Point&)
   {
      
   }
   
   virtual void cropString(STRING& text, unsigned int) {
      
   }
   
   isab::Font* createFont(WFAPI::FontSpec*) {
      return NULL;
   }
   
   virtual void deleteFont(isab::Font*) {
      
   }
   virtual void setFontStyle(isab::Font*) {
      
   }
   
   virtual void resetFontStyle() {
      
   }
};

#endif
