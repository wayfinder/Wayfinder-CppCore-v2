/*
Copyright (c) 1999 - 2010, Vodafone Group Services Ltd
All rights reserved.

Redistribution and use in source and binary forms, with or without modification, are permitted provided that the following conditions are met:

    * Redistributions of source code must retain the above copyright notice, this list of conditions and the following disclaimer.
    * Redistributions in binary form must reproduce the above copyright notice, this list of conditions and the following disclaimer in the documentation and/or other materials provided with the distribution.
    * Neither the name of the Vodafone Group Services Ltd nor the names of its contributors may be used to endorse or promote products derived from this software without specific prior written permission.

THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS" AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE ARE DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT HOLDER OR CONTRIBUTORS BE LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
*/


#ifndef _WINMAPPLOTTER_H_
#define _WINMAPPLOTTER_H_

/* includes */
#include "MC2BoundingBox.h"
#include "TransformMatrix.h"
#include "MapPlotter.h"
#include "WinPaintCallback.h"
#include "TileMapHandler.h"
#include "VisibilityAdapterBase.h"
#include "MapRectInterface.h"
#include "DxDraw.h"
#include "WinMobileGDIDrawer.h"
#include <vector>
#include <map>

namespace isab {

   /* array for points */
   class PointArray : public std::vector<MC2Point>,
                      public PointVector
   {
   public:
      /* constructor */
      PointArray(int maxSize);

      /* destructor */
      ~PointArray();
   
      /* adds a point to the vector */
      void addPoint( int x, int y );

      /* Returns the number of coordinates in the vector. */
      int getSize() const;

      /* Resets the addposition ( and size ) back to zero. */
      void reset();

      /* Returns the number of allocated points. */
      int getAllocatedSize() const;
   };

   struct PenAttribs{
      int Style, Width;
      COLORREF Color;

      PenAttribs()
         :Style(0),Width(0),Color(0){}
      PenAttribs(int pStyle,int pWidth,COLORREF pColor)
         :Style(pStyle),Width(pWidth),Color(pColor){}
      PenAttribs(const PenAttribs& a)
         :Style(a.Style),Width(a.Width),Color(a.Color){}

      PenAttribs& operator=(const PenAttribs& a);
   };

   typedef std::pair<PenAttribs,HPEN> penMapVal;

   typedef std::map<PenAttribs,HPEN> penMap;

   typedef std::pair<COLORREF,HBRUSH> brushMapVal;

   typedef std::map<COLORREF,HBRUSH> brushMap;

   /* Win32 implementation of MapPlotter */
   class WinMapPlotter : public MapPlotter, public MapRectInterface
   {
      /** data **/
   private:
      /* the parent window */
      //      HWND m_parentWindow;

      /* the bitmap to draw to */
      //      HBITMAP m_backBuffer;
      //      HBITMAP m_tempBuffer;
      //       HBITMAP m_oldBitmap;
      //       HBITMAP m_oldtBitmap;

      // here is methods, needed for GDI painting
      HDC DC();
      void unDC();
      
      HDC m_backDC;
      //      HDC m_tempDC;
      DxDraw *m_ddraw;
      //      IDirectDrawSurface *m_tempSurf; // the surface that the map is drawn to, might not be needed

      /* width and height of the buffer */
      int m_width, m_height;

      /* the blit position */
      //      int m_dX, m_dY;

      /* state variables */
      /* pen stuff */
      int m_penWidth, m_penStyle;
      COLORREF m_penColor;
      HPEN m_pen;
      penMap m_penCache;

      /* brush stuff */
      COLORREF m_brushColor;
      HBRUSH m_brush;
      brushMap m_brushCache;

      /* the filling color */
      COLORREF m_fillColor;
      HBRUSH m_fillBrush;

      /* the NULL pen for avoiding drawing outlines */
      HPEN m_nullPen;
      /* the NULL brush for avoiding filling interiors */
      HBRUSH m_nullBrush;

      /* the current font */
      int32 m_fontWidth, m_fontHeight;      
      /* for caching fonts */
      STRING m_curFontName;
      int32 m_curFontSize;

      /* structure to store font information */
      LOGFONT m_fontData;

      /* structure to store text metrics */
      TEXTMETRIC m_textMetrics;

      /* the drag rectangle */
      LPRECT m_dragRect;

      /* backgroung filling color. */
      COLORREF m_backColor;
      HBRUSH m_backFillingBrush;

      /* the frame rectangle */
      TransformMatrix* m_xform;
      MC2BoundingBox* m_frameRect;
      int m_frameWidth;
      int m_frameRed, m_frameGreen, m_frameBlue;

      /* WinPaintCallback */
      WinPaintCallback* m_pWinPaintCallback;      
      
      int m_nTime;
      int m_nCountRedraw;
      int m_nPrimitivesDrawed;
      int m_nPenCached;

      BOOL m_bStatic;
      double m_dScale;

      /* Screen snapshot device context and bitmap. */
      HDC m_screenSnapshot;
      HBITMAP m_screenBitmap;
      HBITMAP m_oldScreenBitmap;

      bool m_afterBlitNotOnBackBuffer;

      //IDirectDrawSurface *m_snapshotSurface;
      DxSurf m_snapshotSurf;
      DxSurf m_cacheSurf;
   
   private:
      /* private constructor */
      WinMapPlotter();

      /* second-phase constructor */
      int construct(int width, int height);

      /* creates a pen using the current pen parameters, deletes the
         old pen if required */
      void updateCurrentPen();

      /* creates a brush using the current brush parameters, deletes the
         old brush if required */
      void updateCurrentBrush();

      /* creates a fill brush using the current brush parameters, 
         deletes the old brush if required */
      void updateFillingBrush();

      /* creates a Win32 specific font - supports rotated fonts */
      HFONT createFont(float angle);

      /* rotates a point around another by the specified angle */
      void rotate2D(int x, int y,
                    float rotAngle, 
                    int* outx, int* outy,
                    int cx = 0, int cy = 0)
      {
#define DEG2RAD(ang) (0.01745329252*ang)

         float sine = sin(DEG2RAD(rotAngle));
         float cosine = cos(DEG2RAD(rotAngle));

         /* calculate the points and translate back to given centre */
         *outx = int( float(y)*sine   + float(x)*cosine ) + cx;
         *outy = int( float(y)*cosine - float(x)*sine ) + cy;

         return;
      }

      /** 
       * This method does not seem to work.
       * It should however do this:
       * 
       * gets the rotated bounding rectangle 
       * for the specified angle and points 
       */
      isab::Rectangle rotatedRect(int x1, int y1, int x2, int y2, 
                                  float angle);

      
      /**
       *    Convert the desired part of the utf8 string to unicode.
       *
       *    @param   text     The utf8 string.
       *    @param   startIdx The index of the first char of the string.
       *    @param   nbrChars The number of chars in the string.
       *                      If -1, use max length of the string.
       *    @param   uniStr   Prealloced unicode string to place result in.
       *    @param   uniStrLen   The length of the prealloced unicode 
       *                         string.
       *    @return  Number of characters in the returned string.
       */
      int utf8ToUnicode( const STRING& text,
                         int startIdx,
                         int nbrChars,
                         wchar_t* uniStr,
                         int uniStrLen );

   public:

      /* allocates a new WinMapPlotter */
      static WinMapPlotter* allocate(int width, int height);

      /* destructor */
      ~WinMapPlotter();

      /**
       *	 Sets static flag - map is not moving/zooming.
       */
      void setStatic(BOOL bStatic);

      /**
       *	 Set map scale.
       */
      void setScale(double dScale);

      // -- Functions that must be implemented ---------------
   
      /**
       *   Fills the screen with the background color.
       */
      void clearScreen();
   
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
       *	  Set background filling color.
       */
      void setBackFillColor(COLORREF cBackColor);

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
       *	  Draws line.
       *   @param nSx  Start point X coord.
       *   @param nSy  Start point Y coord.
       *   @param nEx  Start point X coord.
       *   @param nEy  Start point Y coord.
       */
      void drawLine(int nSx, int nSy, int nEx, int nEy); 

      /**
       *   Draws a polyline defined by the array of points.
       */
      void drawPolyLine( std::vector<MC2Point>::const_iterator begin,
                         std::vector<MC2Point>::const_iterator end );

      /**
       *	  Gets an inflated ellipse size.
       *   @param nBaseSize Base ellipse size.
       */
      int getInflatedEllipseSize(int nBaseSize);

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
      void drawPolyLineWithColor( std::vector<MC2Point>::const_iterator begin,
                                  std::vector<MC2Point>::const_iterator end,
                                  uint32 color,
                                  int lineWidth );

      /**
       *   Puts the dimensions of the displayed map in the
       *   variables.
       */
      void getMapSizePixels(Rectangle& size) const;

      /**
       *   Creates a pointvector of size <code>size</code>.
       */
      PointVector* createPointVector(int size);

      /**
       *   Returns the pointvector to the MapPlotter.
       */
      void deletePointVector(PointVector* vect);

      // Functions that should be re-implemented. ---------------------->

      // Text functions -------------------------------------------------

      /**
       *   Creates a text string for use with the other text methods.
       *   Default implementation returns NULL.
       */
      STRING* createString(const char* text);

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
       *   Sets font. The description format in the server
       *   and the MapPlotter must know what needs to be put
       *   here.
       *   @param fontName Name of the font in client-specific format.
       *   @param size     Size of the font in UNIT.
       */
      void setFont( const STRING& fontName, int size );
   
      /**
       *   Returns the rotation step possible by the plotter.
       *   <br />Default implementation returns 0.
       *   @return The minimum number of degrees that characters can
       *           be rotated.
       */
      int getMinTextRotation();

      /**
       *   Draws text on the display using the pen color and transparent
       *   background.
       *   @param text      The start of the text to draw.
       *   @param points    The center point of the text.
       *   @param startIdx  The first character to draw.
       *   @param nbrChars  The number of characters to draw.
       */
      void drawText( const STRING& text,
                     const MC2Point& point,
                     int startIdx =  0,
                     int nbrChars = -1,
                     float angle  =  0.0 );

      /**
       *   Puts the bounding boxes of each character in str
       *   into the vector boxes.
       *   <br />
       *   Default implementation does nothing and returns 0.
       *   @param boxes  Vector to add the boxes to.
       *   @see drawText
       *   @return Number of characters in str.
       */
      Rectangle getStringAsRectangle( const STRING& text,
                                      const MC2Point& point,
                                      int startIdx =  0,
                                      int nbrChars = -1,
                                      float angle  = 0.0 );

   
      /**
       *   Puts the bounding boxes of each character in str
       *   into the vector boxes.
       *   <br />
       *   Default implementation does nothing and returns 0.
       *   @param boxes  Vector to add the boxes to.
       *   @see drawText
       *   @return Number of characters in str.
       */
      int getStringAsRectangles( std::vector<Rectangle>& boxes,
                                 const STRING& text,
                                 const MC2Point& point,
                                 int startIdx =  0,
                                 int nbrChars = -1,
                                 float angle  = 0.0 );

      // Misc ----------------------------------------------------------
   
      /**
       *   Returns true if the drawing is double buffered.
       *   Default implementation returns false.
       *   @return True if the drawing is double buffered.
       */
      bool doubleBuffered() const;
   
      /**
       *   Tells the MapPlotter that it is ok to change buffer
       *   if it is doubleBuffered. Default implementation does
       *   nothing.
       *   @return doubleBuffered().
       */
      bool drawBuffer( const Rectangle& rect );

      /**
       *   If the MapPlotter needs to prepare the plotting in
       *   any way it should do it in this method which is called
       *   before drawing.
       *   <br />
       *   The default implementation does not do anything.
       */
      void prepareDrawing();
   
      /**
       *   Must be called by the drawing engine everytime a drawing
       *   is completed. This is to be able to free up the resources
       *   that may have been used during the draw.
       *   <br />
       *   The default implementation does not do anything.
       */
      void triggerApplicationRedraw();
   
      /**
       *   Converts the bytes into a bitmap that can be drawn by the
       *   MapPlotter.
       *   Default implementation returns NULL.
       *   Future improvement could be to  add the type of image too,
       *   e.g. PNG etc.
       */
      BitMap* createBitMap(bitMapType type,
                           const uint8* bytes,
                           int nbrBytes,
                           int dpiCorrectionFactor);
      /**
       *   Draws the bitmap at x and y.
       *   Default implementation draws a small rectangle.
       */
      void drawBitMap( const MC2Point& center,
                       const BitMap* bitMap);

      /**
       *   Gets the size of the specified BitMap in pixels
       *   Has to be implemented in derived class.
       *   @param size Rectangle which the bitmap is inside.
       *   @param bmp  Bitmap to get the size from.
       *   @param centerXY The center of the bitmap (position where it is drawn).
       */
      void getBitMapAsRectangle(Rectangle& size,
                                const MC2Point& origCenter,
                                const BitMap* bmp);

      /**
       *   Deletes the supplied bitmap.
       */
      void deleteBitMap( BitMap* bitMap );

      /**
       *   Returns the wanted bitmap extension.
       *   Default implementation returns png. Dot will be appended
       *   by TileMapHandler.
       */
      const char* getBitMapExtension() const;
   
      /**
       *   Draws a polygon filled with the fill color and with a border
       *   with set with setPenColor and linewith as set by setLineWidth
       *   and dashstyle set by setDashStyle.
       *   If borderColor == penColor or lineWidth == 0 no border should
       *   be drawn.
       *   <br />
       *   Default implementation draws a polyline using the fill color.
       */
      void drawPolygon( std::vector<MC2Point>::const_iterator begin,
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
      void drawPolygonWithColor( std::vector<MC2Point>::const_iterator begin,
                                 std::vector<MC2Point>::const_iterator end,
                                 uint32 fillcolor );
      
      /**
       *   Default implementation draws a polyline using the fill color.
       */
      //void drawPolyPolygon( const PointVectorVector& polygons );
                        

      /**
       *   Draws a spline of some sort.
       *   <br />
       *   Default implementation draws a polyline if not filled else polygon.
       */
      void drawSpline( bool filled,
                       std::vector<MC2Point>::const_iterator begin,
                       std::vector<MC2Point>::const_iterator end );
                    

      /**
       *   Same as drawPolygons, but the polygons must be convex. Can be
       *   implemented to speed things up.
       *   <br />
       *   Default implementation calls drawPolygons.
       */
      void drawConvexPolygon( std::vector<MC2Point>::const_iterator begin,
                              std::vector<MC2Point>::const_iterator end );
   
      /**
       *   Draws an arc.
       *   Default implementation draws a line from one corner to the other
       *   in the rectangle.
       */
      void drawArc( bool filled,
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
      void drawRect( bool filled,
                     const Rectangle& rect,
                     int cornerSize = 0);
   
      /**
       *   Sets the fill color for polygons.
       *   Default implementation sets the pen color.
       *   @see setPenColor.
       */
      void setFillColor( unsigned int red,
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
       *   Sets the width of the lines.
       *   Default implementation does nothing.
       */
      void setLineWidth( int width );
   
      /**
       *   Sets the cap style of the pen.
       *   Default implementation does nothing.
       */
      void setCapStyle( enum capStyle );

      /**
       *   Sets the dash style of the pen.
       *   Default implementation does nothing.
       */
      void setDashStyle( enum dashStyle );

      /**
       *   Puts the dimensions of the displayed map in the
       *   variables. Must be overridden if the drawing units
       *   of the map is not pixels.
       *   <br />
       *   Default implementation returns getMapSizePixels.
       */
      void getMapSizeDrawingUnits(Rectangle& size) const;


      // -- Windows Specific 
      
      /* blits the internal back buffer to the specified DC */
      void blitTo(HDC destDC, int32 dX, int32 dY);

      /** 
       *   Gets the back buffer.
       */
      HDC getBuffer()
      {
         return DC();
      }

      /* resizes the plotter to the specified size */
      /* Returns true on success, false on error */
      bool resize(int32 newWidth, int32 newHeight);

      void setBlitPoint(int x, int y)
      {
         // ignored for now
//           m_dX = x;
//           m_dY = y;
          return;
      }

      /* Draws a frame on the spcified DC. It uses a Win32 RECT 
         and the specified frame width and color */
      void drawFrame(HDC dc,
                     LPRECT frame, 
                     int fWidth,
                     int r, int g, int b);

      /* Sets a drag rectangle to be drawn on the target DC after 
         blitting the backbuffer to it.
         Pass NULL to disable the drag rectangle */
      void setDragRect(LPRECT dragPtr);

      /* Sets a frame to be drawn on the map. 
         Uses given TransformMatrix to do the MC2Coord to 
         screen-point translation. Uses given width and color.
         Pass NULL to disable the frame rectangle */
      void setFrameRect(TransformMatrix* tMatrix,
                        MC2BoundingBox* frame,
                        int fWidth = 0,
                        int r = 0, int g = 0, int b = 0);

      /**
       *    Setting listener for painting events
       *    @param pWinPaintCallback listener
       */
      void setWinPaintCallback( WinPaintCallback* pWinPaintCallback = NULL );

      /**
       *	  Creates a bitmap screen snapshot and stores it.
       */
      virtual void makeScreenSnapshot();

      /**
       *   Moves the screen snapshot.
       */
      virtual void moveScreenSnapshot(const MC2Point& deltaPoint);

      /**
       *   Zooms the screen snapshot. Value larger than one means zoom in.
       */
      virtual void zoomScreenSnapshot(double factor, const MC2Point& zoomPoint);

      virtual bool snapshotHandlingImplemented() const;

      /**
       *    Get the rectangle of the actual map, relative to
       *    the control containing the map.
       */
      PixelBox getMapRect() const;

      /**
       *   True if the mapplotter looks at the mask of the images
       *   to create the visible boundingbox.
       */

      virtual bool returnsVisibleSizeOfBitmaps() const {
         return true;
      }


      virtual void cropString(STRING& text,
                              unsigned int maxWidth);

      /**
       * Draws a string of text as a texture block. The string
       * will be centered at point.
       */ 
      virtual void drawOverlayText(
         const WFAPI::WFString& text,
         TextCachingBlock& cacheBlock,
         const WFAPI::OverlayItemVisualSpec::TextAlignment alignment,
         WFAPI::FontSpec* font,
         const isab::Rectangle& stringRect,
         const WFAPI::OverlayItemVisualSpec::RGB& textColor);

      /*
      *   Creates a platform specific font using the specification fontSpec.
      */ 
      virtual isab::Font* createFont(WFAPI::FontSpec* fontSpec);

      /**
       *   Deallocates a platform specific font.
       */ 
      virtual void deleteFont(isab::Font* font);

      /**
       * Sets the font that should be used in drawText.
       *
       * @param font The font to use when drawing the text.
       */
      virtual void setFontStyle(isab::Font* font);

      /**
       * Resets the font for drawing text back to default font.
       */
      virtual void resetFontStyle();

      virtual void drawImageSpec(const WFAPI::ImageSpec* imgSpec,
                                 const MC2Point& topLeftPos);

      /**
       * @see MapPlotter::endFrame
       */
      virtual void endFrame();

   };




}; /* namespace isab */

#endif
