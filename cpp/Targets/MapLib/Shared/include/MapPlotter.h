/*
  Copyright (c) 1999 - 2010, Vodafone Group Services Ltd
  All rights reserved.

  Redistribution and use in source and binary forms, with or without modification, are permitted provided that the following conditions are met:

  * Redistributions of source code must retain the above copyright notice, this list of conditions and the following disclaimer.
  * Redistributions in binary form must reproduce the above copyright notice, this list of conditions and the following disclaimer in the documentation and/or other materials provided with the distribution.
  * Neither the name of the Vodafone Group Services Ltd nor the names of its contributors may be used to endorse or promote products derived from this software without specific prior written permission.

  THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS" AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE ARE DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT HOLDER OR CONTRIBUTORS BE LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
*/


#ifndef MAP_PLOTTER_H
#define MAP_PLOTTER_H

#include "OverlayDrawer.h"
#include "MapPlotterConfig.h"
#include "MapPlotterTypes.h"
#include "NativeTextInterface.h"
#include "BitMapTypes.h"
#include "ViewPortInterface.h"

#include <vector>

class TextCachingBlock;
class MC2Point;

namespace WFAPI {

class ImageSpec;
class ScreenPoint;

}

namespace isab {

/**
 *   Abstract class representing a vector of coordinates.
 *   To be used as a base class for specialized classes used
 *   by the MapPlotter subclasses.
 *   Should be templated into the MapPlotter later for performance.
 */
class PointVector {
public:

   /**
    *   Virtual destructor.
    */
   virtual ~PointVector() {};
   
   /**
    *   Adds one coordinate to the vector.
    *   @param x The x coordinate.
    *   @param y The y coordinate.
    */   
   virtual void addPoint( int x, int y ) = 0;

   /**
    *   Adds many points at once to the vector.
    *   Default implementation just calls the reset mehtod and then addPoint
    *   method for each point.
    */
   virtual void setPoints(const std::vector<MC2Point>& points);
   
   /**
    *   Returns the number of coordinates in the vector.
    */
   virtual int getSize() const = 0;

   /**
    *   Resets the addposition ( and size ) back to zero.
    */
   virtual void reset() = 0;

   /**
    *   Returns the number of allocated points.
    */
   virtual int getAllocatedSize() const = 0;
   
};

/**
 *   Abstract class that should be used to describe a device
 *   that is able to draw a map. See it as a driver.
 *   <br />
 *   <b>NB!</b> It should be possible to use this file in
 *              many environments so the default implementations
 *              must not use anything other than the functions in
 *              MapPlotter.
 */
class MapPlotter : public NativeTextInterface,
                   public ViewPortInterface,
                   public OverlayDrawer {   
public:

   /**
    *   Virtual destructor.
    */
   virtual ~MapPlotter() {}
   
   // -- Functions that must be implemented ---------------
   
   /**
    *   Fills the screen with the background color.
    */
   virtual void clearScreen() = 0;
   
   /**
    *   Returns if the map area / window is visible.
    *
    *   @return True if the map area is visible, false otherwise.
    */
   virtual bool isVisible() const;
   
   /**
    *   Sets the background color.
    *   <br />
    *   NB! The server will not send impossible values once the
    *   description formats are done.
    *   @param red   Red value   0-255.
    *   @param green Green value 0-255.
    *   @param blue  Blue value  0-255.
    */
   virtual void setBackgroundColor( unsigned int red,
                                    unsigned int green,
                                    unsigned int blue ) = 0;

   /**
    *   Sets the color of the pen.
    *   <br />
    *   NB! The server will not send impossible values once the
    *   description formats are done.
    *   @param red   Red value   0-255.
    *   @param green Green value 0-255.
    *   @param blue  Blue value  0-255.
    */
   virtual void setPenColor( unsigned int red,
                             unsigned int green,
                             unsigned int blue ) = 0;

   /**
    *   Sets the color of the pen using one 24bit color value.
    *   @param rgb24bits Red in bits 23-16 etc.
    */
   inline void setPenColor(unsigned int  rgb24bits ) {
      setPenColor( ( rgb24bits >> 16) & 0xff,
                   ( rgb24bits >>  8) & 0xff,
                   ( rgb24bits >>  0) & 0xff );
   }

   /**
    *   Creates a pointvector of size <code>size</code>.
    */
   virtual PointVector* createPointVector(int size) = 0;

   /**
    *   Returns the pointvector to the MapPlotter.
    */
   virtual void deletePointVector(PointVector* vect) = 0;

   // Functions that should be re-implemented. ---------------------->

   virtual int nbrMaskedExtraBitmaps() const { return 0; }

   virtual int switchBitmap( int nbr, int mask ) {
      return nbr == 0 && mask == false;
   }
   
   // Text functions -------------------------------------------------

   
   /**
    *   Returns the rotation step possible by the plotter.
    *   <br />Default implementation returns 0.
    *   @return The minimum number of degrees that characters can
    *           be rotated.
    */
   virtual int getMinTextRotation();

   /**
    *    Draw straight text. The specified point is at the baseline, 
    *    left of the text.
    */
   virtual void drawTextAtBaselineLeft( const STRING& text,
                                        const MC2Point& point );
   
   /**
    *   Draws text on the display using the pen color and transparent
    *   background.
    *   @param text      The start of the text to draw.
    *   @param points    The center point of the text.
    *   @param startIdx  The first character to draw.
    *   @param nbrChars  The number of characters to draw.
    */
   virtual void drawText( const STRING& text,
                          const MC2Point& point,
                          int startIdx =  0,
                          int nbrChars = -1,
                          float angle  =  0.0 );

   /**
    * Sets the font that should be used in drawText.
    *
    * @param font The font to use when drawing the text.
    */
   virtual void setFontStyle(isab::Font* font) = 0;

   /**
    * Resets the font for drawing text back to default font.
    */
   virtual void resetFontStyle() = 0;

   /**
    * Releases data for all fonts used.
    */
   virtual void releaseFonts();

   // Misc ----------------------------------------------------------

   /**
    *   True if the mapplotter looks at the mask of the images
    *   to create the visible boundingbox.
    */
   virtual bool returnsVisibleSizeOfBitmaps() const {
      return false;
   }
   
   /**
    *   Returns true if the drawing is double buffered.
    *   Default implementation returns false.
    *   @return True if the drawing is double buffered.
    */
   virtual bool doubleBuffered() const;
   
   /**
    *   Tells the MapPlotter that it is ok to change buffer
    *   if it is doubleBuffered. Default implementation does
    *   nothing.
    *   @return doubleBuffered().
    */
   virtual bool drawBuffer( const Rectangle& rect );

   /**
    *   If the MapPlotter needs to prepare the plotting in
    *   any way it should do it in this method which is called
    *   before drawing.
    *   <br />
    *   The default implementation does not do anything.
    */
   virtual void prepareDrawing();
   
   /**
    *   Must be called by the drawing engine everytime a drawing
    *   is completed. This is to be able to free up the resources
    *   that may have been used during the draw.
    *   <br />
    *   The default implementation does not do anything.
    */
   virtual void triggerApplicationRedraw();
   
   /**
    *   Converts the bytes into a bitmap that can be drawn by the
    *   MapPlotter.
    *   Default implementation returns NULL.
    *   Perhaps we should add the type of image too, e.g. PNG etc.?
    */
   virtual BitMap* createBitMap(bitMapType type,
                                const uint8* bytes,
                                int nbrBytes,
                                int dpiCorrectionFactor = 1 );

   /**
    *   Draws the bitmap at x and y.
    *   Default implementation draws a small rectangle.
    */
   virtual void drawBitMap( const MC2Point& center,
                            const BitMap* bitMap);

   /**
    * Draws a string of text as a texture block. The string
    * will be centered at point.
    *
    * @param text The text to be drawn.
    * @param cacheBlock Contains a cached text, if the text and the cached
    *                   text is the same there is no need for reallocation.
    * @param alignment The horizontal and vertical alignment of the text.
    * @param largeRect The complete text area to draw in.
    * @param textColor The color of the text.
    */
   virtual void drawOverlayText(
      const WFAPI::WFString& text,
      TextCachingBlock& cacheBlock,
      const WFAPI::OverlayItemVisualSpec::TextAlignment alignment,
      WFAPI::FontSpec* font,
      const isab::Rectangle& stringRect,
      const WFAPI::OverlayItemVisualSpec::RGB& textColor);

   /**
    *   Gets the size of the specified BitMap in pixels
    *   Has to be implemented in derived class.
    *   @param size Rectangle which the bitmap is inside.
    *   @param bmp  Bitmap to get the size from.
    *   @param centerXY The center of the bitmap (position where it is drawn).
    */
   virtual void getBitMapAsRectangle(Rectangle& size,
                                     const MC2Point& origCenter,
                                     const BitMap* bmp);

   /**
    *   Gets the size of the specified BitMap in pixels
    *   The other getBitMapAsRectangle has to be implemented in the subclasses.
    *   @param size Rectangle which the bitmap is inside.
    *   @param bmp  Bitmap to get the size from.
    *   @param centerXY The center of the bitmap (position where it is drawn).
    */ 
   Rectangle getBitMapAsRectangle( const MC2Point& origCenter,
                                   const BitMap* bmp) {
      Rectangle rect;
      getBitMapAsRectangle(rect, origCenter, bmp);
      return rect;
   }


   virtual int getStringAsRectangles( std::vector<isab::Rectangle>& boxes,
                                      const STRING& text,
                                      const MC2Point& center,
                                      int startIdx = 0,
                                      int nbrChars = -1,
                                      float angle = 0.0 );
   
   virtual isab::Rectangle getStringAsRectangle( const STRING& text,
                                                 const MC2Point& center,
                                                 int startIdx = 0,
                                                 int nbrChars = -1,
                                                 float angle = 0.0 );


   
   /**
    *   Deletes the supplied bitmap.
    */
   virtual void deleteBitMap( BitMap* bitMap );

   /**
    *   Returns the wanted bitmap extension.
    *   Default implementation returns png. Dot will be appended
    *   by TileMapHandler.
    */
   virtual const char* getBitMapExtension() const;

   /**
    *   Sets the fill color for polygons.
    *   Default implementation sets the pen color.
    *   @see setPenColor.
    */
   virtual void setFillColor( unsigned int red,
                              unsigned int green,
                              unsigned int blue );
   /**
    *   Sets the color of the pen using one 24bit color value.
    *   @param rgb24bits Red in bits 23-16 etc.
    */
   inline void setFillColor(unsigned int  rgb24bits ) {
      setFillColor( ( rgb24bits >> 16) & 0xff,
                   ( rgb24bits >>  8) & 0xff,
                   ( rgb24bits >>  0) & 0xff );
   }

   
   /**
    *   Draws a polygon filled with the fill color and with a border
    *   with set with setPenColor and linewith as set by setLineWidth
    *   and dashstyle set by setDashStyle.
    *   If borderColor == penColor or lineWidth == 0 no border should
    *   be drawn.
    *   <br />
    *   Default implementation draws a polyline using the fill color.
    */
   virtual void drawPolygon( std::vector<MC2Point>::const_iterator begin,
                             std::vector<MC2Point>::const_iterator end );

   /**
    *   Draws a filled polygon defined by the iterators to the screen coords
    *   and the supplied color and line width. Override this to
    *   reduce the number of virtual functions called by the
    *   TileMapHandler.
    *   @param begin     First point draw.
    *   @param end       Point after last point to draw.
    *   @param fillcolor Color to fill with in rrggbb format.
    */
   virtual void drawPolygonWithColor( std::vector<MC2Point>::const_iterator begin,
                                      std::vector<MC2Point>::const_iterator end,
                                      uint32 fillcolor ) = 0;

   /**
    *   Draws a polyline defined by the array of points.
    */
   virtual void drawPolyLine( std::vector<MC2Point>::const_iterator begin,
                              std::vector<MC2Point>::const_iterator end ) = 0;
   
   /**
    *   Draws a polyline defined by the iterators to the screen coords
    *   and the supplied color and line width. Override this to
    *   reduce the number of virtual functions called by the
    *   TileMapHandler.
    *   @param begin     First point draw.
    *   @param end       Point after last point to draw.
    *   @param color     Color in rrggbb format.
    *   @param lineWidth Line width in pixels.
    */
   virtual void drawPolyLineWithColor( std::vector<MC2Point>::const_iterator begin,
                                       std::vector<MC2Point>::const_iterator end,
                                       uint32 color,
                                       int lineWidth ) = 0;

   
   /**
    *   Default implementation draws a polyline using the fill color.
    */
   //virtual void drawPolyPolygon( const PointVectorVector& polygons );
                        

   /**
    *   Draws a spline of some sort.
    *   <br />
    *   Default implementation draws a polyline if not filled else polygon.
    */
   virtual void drawSpline( bool filled,
                            std::vector<MC2Point>::const_iterator begin,
                            std::vector<MC2Point>::const_iterator end );
                    

   /**
    *   Same as drawPolygons, but the polygons must be convex. Can be
    *   implemented to speed things up.
    *   <br />
    *   Default implementation calls drawPolygons.
    */
   virtual void drawConvexPolygon( std::vector<MC2Point>::const_iterator begin,
                                   std::vector<MC2Point>::const_iterator end );
   
   /**
    *   Draws an arc.
    *   Default implementation draws a line from one corner to the other
    *   in the rectangle.
    */
   virtual void drawArc( bool filled,
                         const Rectangle& rect,
                         int startAngle,
                         int stopAngle);

   /**
    *   Draws a rectangle.
    *   @param filled     If true the rectangle is filled.
    *   @param rect       Dimensions of the rectangle.
    *   @param cornerSize Radius of corners.
    *   Default implementation cannot draw rounded corners and uses
    *   drawPolyLine or drawConvexPolygon.
    */
   virtual void drawRect( bool filled,
                          const Rectangle& rect,
                          int cornerSize = 0);
     
   /**
    *   Sets the width of the lines.
    *   Default implementation does nothing.
    */
   virtual void setLineWidth( int width );
   
   /**
    * Cap style enum.
    */
   enum capStyle_t {
      /// No cap, standard line.
      NO_CAP,
      /// Round cap, extending half of the width.
      ROUND_CAP,
      /// Flat cap, extending half of the width.
      FLAT_CAP
   };

   /**
    *   Sets the cap style of the pen.
    *   Default implementation does nothing.
    */
   virtual void setCapStyle( capStyle_t capStyle );

   /**
    *   Sets the dash style of the pen.
    *   Default implementation does nothing.
    */
   virtual void setDashStyle( enum dashStyle );

   /**
    *   If the plotter can handle the screen as a bitmap snapshot, i.e.
    *   if makeScreenSnapshot(), moveScreenSnapshot() and
    *   zoomScreenSnapshot() are properly implemented.
    */
   virtual bool snapshotHandlingImplemented() const;
   
   /**
    *   Creates a bitmap screen snapshot and stores it.
    *   Default implementation does nothing.
    */
   virtual void makeScreenSnapshot();

   /**
    *   Moves the screen snapshot.
    */
   virtual void moveScreenSnapshot( const MC2Point& deltaPoint );

   /**
    *   Zooms the screen snapshot. Value larger than one means zoom in.
    */
   virtual void zoomScreenSnapshot(double factor, const MC2Point& zoomPoint);

   /**
    *   Locks the bitmap buffer, needed on at least the symbian
    *   platform when doing operations on the bitmaps data.
    */
   virtual void lockBitmapBuffer();

   /**
    *   Unlocks the bitmap buffer. Should only be done when the buffer
    *   has been locked.
    */
   virtual void unlockBitmapBuffer();

   /**
    *   Returns the data adress of the bitmap.
    */
   virtual uint32* getBitmapBuffer();

   /**
    *   Returns the colorDepth of a bitmap.
    */
   virtual int getColorDepth();

   /** 
    *  Set the drawing mode to be low quality and fast.
    *  Such as disabling antialiasing, or other tricks.
    */
   virtual void enableLowQualityDrawing( bool on );

   /**
    *  This is called after all drawing has been completed,
    *  override this if you need to release any resources
    *  before the flip takes place.
    */
   virtual void endFrame();

};
} // namespace
#endif
