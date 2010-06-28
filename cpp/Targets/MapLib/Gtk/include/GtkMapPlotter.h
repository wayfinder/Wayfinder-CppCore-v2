/*
  Copyright (c) 1999 - 2010, Vodafone Group Services Ltd
  All rights reserved.

  Redistribution and use in source and binary forms, with or without modification, are permitted provided that the following conditions are met:

  * Redistributions of source code must retain the above copyright notice, this list of conditions and the following disclaimer.
  * Redistributions in binary form must reproduce the above copyright notice, this list of conditions and the following disclaimer in the documentation and/or other materials provided with the distribution.
  * Neither the name of the Vodafone Group Services Ltd nor the names of its contributors may be used to endorse or promote products derived from this software without specific prior written permission.

  THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS" AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE ARE DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT HOLDER OR CONTRIBUTORS BE LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
*/

#ifndef GTKMAPPLOTTER_H
#define GTKMAPPLOTTER_H

#include "config.h"

#include "MapPlotter.h"
#include "MapPlotterTypes.h"
#include "MC2SimpleString.h"
#include "FontType.h"
#include <gtk/gtk.h>
#include <cairo.h>

#include <set>

class GtkBitMap : public isab::BitMap {
public:
   GtkBitMap():m_pixMap(0), m_bitMap(0) {}

   /// The pixmap
   GdkPixmap *m_pixMap;
   /// Hopefully the mask
   GdkBitmap *m_bitMap;
};

class GtkPlotterFont : public isab::Font {
public:
   GtkPlotterFont(const char* name);

   virtual ~GtkPlotterFont();
   
   PangoFontDescription* m_fontDesc;   
};

class GtkPointVector : public isab::PointVector {
public:
   /**
    *   Allocates room for size coordinates.
    */
   GtkPointVector(int size);

   /**
    *   Deallocates allocated memory.
    */
   virtual ~GtkPointVector();
   
   /**
    *   Adds one point to the end of the vector.
    */
   void addPoint(int x, int y);

   /**
    *   Adds many points to the end of the vector.
    */
   void setPoints(const vector<MC2Point>& points);

   /**
    *   Returns the size of the vector.
    */
   int getSize() const;

   /**
    *
    */
   void reset();

   /**
    *
    */
   int getAllocatedSize() const;

   /**
    *
    */
   const vector<GdkPoint>& getPoints() const;
       
   
protected:
   /**
    *   Number of added points in vector.
    */
   int m_nbrAdded;

   /**
    *   The allocated size.
    */
   int m_allocatedSize;

   /**
    *   This is where we keep our coordinates.
    */
   vector<GdkPoint> m_points;
   
};


/**
 *    Driver for plotting in the Gtk environment.
 *
 */
class GtkMapPlotter : public isab::MapPlotter {
public:
   /**
    *    Create a new GtkMapPlotter.
    *    @param widget The widget containing the window to draw in.
    */
   explicit GtkMapPlotter(GtkWidget* widget);

   /**
    *    Destructor.
    */
   ~GtkMapPlotter();

   // - Functions that must be implemented from MapPlotter.

   /**
    *   Fills the screen with the background color.
    */
   void clearScreen();

   /**
    *   Returns if the map area / window is visible.
    *
    *   @return True if the map area is visible, false otherwise.
    */
   bool isVisible() const;
   
   /**
    *   Sets the background color.
    *   <br />
    *   NB! The server will not send impossible values once the
    *   description formats are done.
    *   @param red   Red value   0-255.
    *   @param green Green value 0-255.
    *   @param blue  Blue value  0-255.
    */
   void setBackgroundColor( unsigned int red,
                            unsigned int green,
                            unsigned int blue );

   /**
    *   Sets the color of the pen.
    *   <br />
    *   NB! The server will not send impossible values once the
    *   description formats are done.
    *   @param red   Red value   0-255.
    *   @param green Green value 0-255.
    *   @param blue  Blue value  0-255.
    */
   void setPenColor( unsigned int red,
                     unsigned int green,
                     unsigned int blue );

   // -- TEXT 

   /**
    *   Sets the font that we wants to use.
    */
    void setFont( const STRING& fontName, int size = 0 );

    /**
     *   Returns the current font name.
     */
    const char* getFontName() const;
    
   /**
    *   Creates and returns a copy of the char* text.
    */
   STRING* createString(const char* text);

   /**
    * Crops the string, modifies the string to fit within the
    * maxWidth with the last three chars as ...
    *
    * @param text The text to crop.
    * @param maxWidth the maximal width in pixels.
    */
   void cropString(STRING& text, unsigned int maxWidth);
   
   /**
    *   Deletes a text string.
    *   Default implementation does nothing.
    */   
   void deleteString(STRING* text);
  
   /**
    *   @return The number of characters in the string.
    */
   int getStringLength( const STRING& text );
 
   /**
    *   Sets the fill color for polygons.
    *   Default implementation sets the pen color.
    *   @see setPenColor.
    */
   void setFillColor( unsigned int red,
                      unsigned int green,
                      unsigned int blue );

   /**
    *   Draws text on the display using the pen color and transparent
    *   background.
    *   Note that indeces and rotation not taken into account.
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
    *   True if the mapplotter looks at the mask of the images
    *   to create the visible boundingbox.
    */
   virtual bool returnsVisibleSizeOfBitmaps() const;

   /**
    *   Puts the bounding boxes of each character in str
    *   into the vector boxes.
    *   Note that indeces and rotation is not taken into account.
    *   @param boxes  Vector to add the boxes to.
    *   @see drawText
    *   @return Number of characters in str.
    */
   virtual isab::Rectangle getStringAsRectangle( const STRING& text,
                                                 const MC2Point& point,
                                                 int startIdx =  0,
                                                 int nbrChars = -1,
                                                 float angle  = 0.0 );
   
   /**
    * Returns the dimensions of the text in pixels.
    *
    * @param text The text to get the dimensions for.
    * @return A rectangle containing the width and height in pixels
    *         for the text.
    */
   virtual isab::Rectangle getTextDimensions(const STRING& text);
   
   /**
    *
    */
   virtual int getStringAsRectangles( 
            vector<isab::Rectangle>& boxes,
            const STRING& text,
            const MC2Point& center,
            int startIdx,
            int nbrChars,
            float angle );
    
   /**
    *   Draws a polyline defined by the array of points.
    */
   void drawPolyLine( const vector<MC2Point>::const_iterator begin,
                      const vector<MC2Point>::const_iterator end);

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
   void drawPolyLineWithColor( vector<MC2Point>::const_iterator begin,
                               vector<MC2Point>::const_iterator end,
                               uint32 color,
                               int lineWidth ) {
      setLineWidth( lineWidth );
      MapPlotter::setPenColor( color );
      drawPolyLine( begin, end );
   }
   
   /**
    *   Draws a polygon filled with the fill color and with a border
    *   with set with setPenColor and linewith as set by setLineWidth
    *   and dashstyle set by setDashStyle.
    *   If borderColor == penColor or lineWidth == 0 no border should
    *   be drawn.
    *   <br />
     */
   void drawPolygon( const vector<MC2Point>::const_iterator begin,
                     const vector<MC2Point>::const_iterator end);

   void drawPolygonWithColor( vector<MC2Point>::const_iterator begin,
                              vector<MC2Point>::const_iterator end,
                              uint32 fillcolor ) {
      MapPlotter::setFillColor( fillcolor );
      drawPolygon( begin, end );
   }

   /**
    *   Puts the dimensions of the displayed map in the
    *   variables.
    */
   void getMapSizePixels(isab::Rectangle& size) const;

   /**
    *   Creates a pointvector of size <code>size</code>.
    */
   isab::PointVector* createPointVector(int size);

   /**
    *   Returns the pointvector to the MapPlotter.
    */
   void deletePointVector(isab::PointVector* vect);
   
   /**
    *   Sets the width of the lines.
    *   Default implementation does nothing.
    */
   void setLineWidth( int width );

   /**
    *   Sets the cap style of the pen.
    */
   void setCapStyle( capStyle_t capStyle );

   /**
    *   Creates a bitmap from an xpm. Type is ignored.
    */
   isab::BitMap* createBitMap(isab::bitMapType type,
                              const uint8* bytes,
                              int nbrBytes,
                              int dpiCorrectionFactor = 1);

   /**
    *   Draws a bitmap at the center coordinate.
    */
   void drawBitMap( const MC2Point& center,
                    const isab::BitMap* bitMap);

   /**
    *   Draws the bitmap at x and y.
    */
   virtual void drawImageSpec(const WFAPI::ImageSpec* spec,
                              const MC2Point& point);

   /**
    * Sets the clipping region of the drawer, which is the
    * region that stuff can be drawn to. Pixels outside this
    * region will not be affected. 
    */ 
   virtual void setClippingRectangle(int x, int y,
                                     int width, int height);

   /**
    * Resets the clipping region of the drawer to the entire screen.
    */ 
   virtual void resetClippingRectangle();

   /**
    * Drawers the support clipping will return true if this method
    * is called.
    */ 
   virtual bool supportsClipping() const { return true; }
   
   /**
    *   Gets the size of the specified BitMap in pixels
    *   Has to be implemented in derived class.
    *   @param size Rectangle which the bitmap is inside.
    *   @param bmp  Bitmap to get the size from.
    *   @param centerXY The center of the bitmap (position where it is drawn).
    */
   void getBitMapAsRectangle( isab::Rectangle& size,
                              const MC2Point& origCenter,
                              const isab::BitMap* bmp );
   
   /**
    *   Deletes the supplied bitmap.
    *   @see MapPlotter
    */
   void deleteBitMap( isab::BitMap* bitMap );

   /**
    *   Returns xpm.
    */
   const char* getBitMapExtension() const { return "png"; }
  
   /**
    *   Prepares drawing.
    */
   void prepareDrawing();

   /**
    *   Invalidates our window, allowing it to redraw using
    *   renderTo
    */
   void triggerApplicationRedraw();

   /**
    *  Draws our internal pixmap buffer to the supplied gc
    */
   void renderTo( GdkGC* gc, GdkRectangle* rect );
   
   /** 
    *  Set the drawing mode to be low quality and fast.
    *  Such as disabling antialiasing, or other tricks.
    */
   void enableLowQualityDrawing( bool on );


   /**
    *   Creates a platform specific font using the specification fontSpec.
    */ 
   virtual isab::Font* createFont(WFAPI::FontSpec* fontSpec);

   /**
    *   Deallocates a platform specific font.
    */ 
   virtual void deleteFont(isab::Font* font);

   /**
    * Resets the cairo font to the default font.
    */
   virtual void resetFontStyle();

   /**
    * Updates the cairo font to the specified font.
    */ 
   virtual void setFontStyle(isab::Font* font);

   /**
    * Releases all font data that has been used.
    */
   virtual void releaseFonts();

private:

   /**
    *   Plots some stuff at every coordinate.
    */
   void plotCoordinates( vector<MC2Point>::const_iterator begin,
                         vector<MC2Point>::const_iterator end );
   
   /**
    *    allocates color in the colormap
    */
   void allocColor(GdkColor& theColor,
                   unsigned int red,
                   unsigned int green,
                   unsigned int blue );

   /**
    *
    */
   void drawPolySomething( vector<MC2Point>::const_iterator begin,
                           vector<MC2Point>::const_iterator end,
                           bool filled);
   
   
   /**
    * The widget containing the window. 
    */
   GtkWidget* m_widget;

   /**
    *    The window where the drawables will be drawn.
    *    Comes from m_widget->window.
    */
   GdkDrawable *m_window;

   /**
    *    Graphic context
    */
   GdkGC *m_gc;

   /**
    * Cairo GC.
    */ 
   cairo_t* m_cgc;
 
   /**
    *    The color of the pen.
    */
   GdkColor m_penColor;

   /**
    *    The color of the fill.
    */
   GdkColor m_fillColor;

   /**
    *    The color of the background.
    */
   GdkColor m_backColor;

   /**
    *    The color of the foreground.
    */
   GdkColor m_foregroundColor;
      
   /**
    *    Temporary backstore.
    */
   GdkPixmap *m_pixmap;

   /** 
    *    True if all coordinates should be shown.
    */
   bool m_plotCoordinates;
   
   /**
    * Pango layout used for text drawing
    */
   PangoLayout *m_layout;

   /// Current fontname.
   MC2SimpleString m_fontName;

   /// The default font
   GtkPlotterFont* m_defaultFont;

   /// Holds all the fonts used.
   set<WFAPI::FontSpec*> m_fonts;
};


#endif

