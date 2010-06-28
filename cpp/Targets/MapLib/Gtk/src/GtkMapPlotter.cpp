/*
  Copyright (c) 1999 - 2010, Vodafone Group Services Ltd
  All rights reserved.

  Redistribution and use in source and binary forms, with or without modification, are permitted provided that the following conditions are met:

  * Redistributions of source code must retain the above copyright notice, this list of conditions and the following disclaimer.
  * Redistributions in binary form must reproduce the above copyright notice, this list of conditions and the following disclaimer in the documentation and/or other materials provided with the distribution.
  * Neither the name of the Vodafone Group Services Ltd nor the names of its contributors may be used to endorse or promote products derived from this software without specific prior written permission.

  THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS" AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE ARE DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT HOLDER OR CONTRIBUTORS BE LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
*/

/*
 *    GtkMapPlotter.cpp
 *
 */
#include "GtkMapPlotter.h"

#include "MC2SimpleString.h"
#include <stdio.h>
#include "PixelBox.h"
#include "GdkPixbufImageSpec.h"
#include <algorithm>
#include "MC2Point.h"
#include "Utility.h"
#include "TrigUtil.h"

#include "FontSpecInternal.h"

using namespace isab;

/// for debug
ostream &operator << (ostream &out, const isab::Rectangle &rect) {
   out<<"( "<<rect.getX()<<", "<<rect.getY()<<", "<<rect.getWidth()<<", "<<rect.getHeight()<<" )";
   return out;
}

/**
 * Get UTF-8 substring from an UTF-8 string
 * @param str utf-8 string
 * @param start start index 
 * @param length of sub string
 * @return allocated utf-8 sub string
 */
char *subStringUTF8(const char *str, int startIdx, int length) {

   gchar *start = g_utf8_offset_to_pointer( str, startIdx );
   gchar *end = g_utf8_offset_to_pointer( str, startIdx + length );

   char *subText = new char[end - start + 2]; // +2 for some extra safety :P
   memset(subText, 0, end - start + 2); 
   return g_utf8_strncpy( subText, start, length);
   
}
/**
 *
 */
static void getTextSize( PangoLayout *layout, 
                         const char *str,
                         int &width, int &height )
{
   if(layout == NULL){
      return;
   }
   // std::cout << "GET_TEXT_SIZE" << std::endl;
   pango_layout_set_text( layout, str, -1 );
   pango_layout_get_pixel_size( layout, &width, &height );
}

GtkPointVector::GtkPointVector(int size)
{
   m_points.reserve(size);
   m_allocatedSize = size;
}

GtkPointVector::~GtkPointVector()
{
   
}

void
GtkPointVector::addPoint(int x, int y)
{
   GdkPoint p;
   p.x = x;
   p.y = y;
   m_points.push_back( p );
}

class ToGDKPointConverter {
public:
   GdkPoint operator()( const pair<int, int>& inpair ) const {
      GdkPoint p;
      p.x = inpair.first;
      p.y = inpair.second;
      return p;
   }
};

void
GtkPointVector::setPoints( const vector<MC2Point>& points)
{
   m_points.clear();
   m_points.reserve( points.size() );
   GdkPoint p;
   for( vector<MC2Point>::const_iterator it = points.begin();
        it != points.end();
        ++it ) {
      p.x = it->getX();
      p.y = it->getY();
      m_points.push_back(p);
   }
           
}

int
GtkPointVector::getSize() const
{
   return m_points.size();
}

int
GtkPointVector::getAllocatedSize() const
{
   return m_allocatedSize;
}

void
GtkPointVector::reset()
{
   m_points.clear();
}

const vector<GdkPoint>&
GtkPointVector::getPoints() const
{
   return m_points;
}

// --- The GtkMapPlotter

GtkMapPlotter::GtkMapPlotter(GtkWidget* widget)
   : m_widget( widget),
     m_window( NULL ),
     m_gc( NULL ),
     m_pixmap( 0 ),
     m_plotCoordinates( false )

{
   if (m_widget != NULL && m_widget->window != NULL) {
      m_window = m_widget->window;
      m_gc = gdk_gc_new(m_window);
   }

   m_defaultFont = new GtkPlotterFont(getFontName());
   
   m_cgc = NULL;
   m_layout = NULL;
}

GtkMapPlotter::~GtkMapPlotter()
{
   delete m_defaultFont;
   releaseFonts();
   
   g_object_unref( m_layout );
   if (m_gc) {
      g_object_unref( m_gc );
   }
   g_object_unref( m_pixmap );
}

void GtkMapPlotter::prepareDrawing()
{
   m_window = m_widget->window;
   // Create the pixmap
   gint width, height;
   gdk_drawable_get_size( m_window, &width, &height );
   gint depth = gdk_drawable_get_depth( m_window );

   // note: do we really need to recreate pixmap/gc each time?
   // seem like it could be done once in constructor and then 
   // recreate if window size != pixmap size
   if(m_cgc) {
      g_object_unref(m_layout);
      cairo_destroy(m_cgc);
   }

   if (m_pixmap != NULL) {
      g_object_unref( m_pixmap );      
   }
   m_pixmap = gdk_pixmap_new( m_window, width, height, depth );
   MC2_ASSERT(m_pixmap != NULL);

   if (m_gc != NULL) {
      g_object_unref( m_gc );
   }

   m_gc = gdk_gc_new( m_pixmap );
   MC2_ASSERT(m_gc != NULL);
      
   m_cgc = gdk_cairo_create(m_pixmap);
   MC2_ASSERT(m_cgc != NULL);
   
   m_layout = pango_cairo_create_layout (m_cgc);
   MC2_ASSERT(m_layout != NULL);
   
   // set front
   pango_layout_set_font_description( m_layout,
                                      m_defaultFont->m_fontDesc );
}

void GtkMapPlotter::triggerApplicationRedraw()
{
   m_window = m_widget->window;
   GdkRegion* region = gdk_drawable_get_visible_region( m_window );

   gdk_window_invalidate_region( m_window, region, true );
   
   gdk_window_process_all_updates();
}

void GtkMapPlotter::renderTo( GdkGC* gc, GdkRectangle* rect ) {
   m_window = m_widget->window;
   if ( m_pixmap != NULL ) { 
      gdk_draw_drawable( m_window, gc, m_pixmap,
                         0, 0, 0, 0, // no offset
                         -1, -1); // draw entire pixmap
     
   }
}

void GtkMapPlotter::clearScreen()
{
   // If m_gc is NULL, then we can not draw anything
   if(m_gc == NULL) {
      return;
   }
   gdk_gc_set_foreground( m_gc, &m_backColor );
   gdk_draw_rectangle( m_pixmap, m_gc, 
                       true,  // filled
                       0, 0, -1, -1 );
   gdk_gc_set_foreground( m_gc, &m_penColor );

}
   
bool 
GtkMapPlotter::isVisible() const
{
   const_cast<GtkMapPlotter*>(this)->m_window = m_widget->window;
   if (m_window == NULL) {
      return false;
   }
   

   // This macro only seems to work on device.
   // It always returns true for simulator.
   return GTK_WIDGET_VISIBLE(m_widget);

   // For simulator, this method seems to work:
//   return gdk_window_is_visible(m_window);
}

void
GtkMapPlotter::allocColor(GdkColor &theColor,
                          unsigned int red,
                          unsigned int green,
                          unsigned int blue)
{
   if(m_pixmap == NULL) {
      return;
   }
   // transform range from 0-255 to 0-65535
   theColor.red = static_cast<int>(red/ 256.0 * 0xFFFF);
   theColor.green = static_cast<int>(green / 256.0 * 0xFFFF);
   theColor.blue = static_cast<int>(blue / 256.0 * 0xFFFF);

   // allocate color
   GdkColormap *colormap = gdk_drawable_get_colormap( m_pixmap );
   gdk_colormap_alloc_color( colormap, &theColor, 
                             true, // color is writable, can be changed with gdk_color_change
                             true ); // try to match the color if allocation failed
   
}

void
GtkMapPlotter::setBackgroundColor( unsigned int red,
                                   unsigned int green,
                                   unsigned int blue )
{
   // If m_gc is NULL, then we can not draw anything
   if(m_gc == NULL) {
      return;
   }
   allocColor(m_backColor, red, green, blue);
   gdk_gc_set_background(m_gc, &m_backColor);
}

void
GtkMapPlotter::setFillColor( unsigned int red,
                             unsigned int green,
                             unsigned int blue )
{
   allocColor(m_fillColor, red, green, blue);
}

void
GtkMapPlotter::setPenColor( unsigned int red,
                            unsigned int green,
                            unsigned int blue )
{
   allocColor(m_penColor, red, green, blue);
}

void 
GtkMapPlotter::setFont( const STRING& fontName, int size )
{
   m_fontName = fontName;
}

const char*
GtkMapPlotter::getFontName() const
{
   // override default font

#ifdef __linux__
   static const char font[] = "Times Slant 10";
#else
   static const char font[] = "Times Slant 8";
#endif
   return font;
   //return m_fontName.c_str();
}

STRING* 
GtkMapPlotter::createString( const char* text)
{
   char** ptr = new char*[1];
   ptr[ 0 ] = new char[ strlen( text ) + 1 ];
   strcpy( ptr[ 0 ], text );
   return ptr;
}

void
GtkMapPlotter::cropString(STRING& text, unsigned int maxWidth)
{
   char* start = text;
   // We assumes that the characater period is one byte in utf8
   int numChars = g_utf8_strlen(text, -1);
   if (numChars <= 3) {
      // To small to crop
      return;
   }
   char* lastChar = g_utf8_offset_to_pointer(text, numChars - 1);
   char* end = NULL;

   // Go back two chars
   char* prev = g_utf8_prev_char(lastChar);
   prev = g_utf8_prev_char(prev);

   // Get a pointer to the prev
   char* tmp = NULL;

   isab::Rectangle strRect;
   do {
      prev = g_utf8_prev_char(prev);
      if (prev == start) {
         // We are already on the first char in the string, cant continue.
         return;
      }
      tmp = prev;

      *tmp++ = '.';
      *tmp++ = '.';
      *tmp++ = '.';

      // Set the end to point to three chars after prev and add null terminator
      end = prev + 3;
      *end = '\0';

      // Get the new width of the string and see if it is small enough
      strRect = getTextDimensions(text);
   } while (strRect.getWidth() > maxWidth);
}

void
GtkMapPlotter::deleteString( STRING* str )
{
   delete [] *str;
   delete [] str;
}

int
GtkMapPlotter::getStringLength( const STRING& text )
{
   return g_utf8_strlen( text, -1 ); 
}



isab::Rectangle 
GtkMapPlotter::getStringAsRectangle( const STRING& text,
                                     const MC2Point& center,
                                     int startIdx,
                                     int nbrChars,
                                     float angle )
{
 
   if( nbrChars == -1 ) {
      nbrChars = getStringLength( text ); 
   }

   MC2_ASSERT( nbrChars!=0 );
   
   // get sub string from utf-8
   char *subText = subStringUTF8(text, startIdx, nbrChars);

   int width, height;
   getTextSize( m_layout, subText, width, height );

   int safeSize =
      static_cast<int>( sqrt( width * width + height * height ) + 0.5 );
   
   delete[] subText;

   return isab::Rectangle( center.getX() - safeSize / 2,
                           center.getY() - safeSize / 2,
                           safeSize, safeSize );  
}

int 
GtkMapPlotter::getStringAsRectangles( vector<isab::Rectangle>& boxes,
                                      const STRING& text,
                                      const MC2Point& center,
                                      int startIdx,
                                      int nbrChars,
                                      float angle )
{

   if( nbrChars == -1 ){
      nbrChars = getStringLength( text );
   }
   
   MC2_ASSERT( nbrChars!=0 );

   // Get pixel size of substring 
   char *subText = subStringUTF8( text, startIdx, nbrChars );
   int widthOfString, heightOfString;
   getTextSize( m_layout, subText, widthOfString, heightOfString );

   delete[] subText;

   int halfWidthOfString = widthOfString >> 1;
   // waruns angle goes ccw and we wants it cw so thats why we do -.
   angle = -angle;

   double radAngle = ( angle / 180.0 ) * M_PI;
   double xDiff = cos( radAngle ) * halfWidthOfString; 
   double yDiff = sin( radAngle ) * halfWidthOfString;
   int p1x = (int)(center.getX() - xDiff );
   int p1y = (int)(center.getY() - yDiff );
   int p2x = (int)(center.getX() + xDiff );
   int p2y = (int)(center.getY() + yDiff );
   for ( int i = startIdx; i < startIdx + nbrChars; i++ ) {
      double diffFactor = 
         ( ( i - startIdx + 0.5 ) / (double)nbrChars );
      mc2dbg4 << "diffFactor " << diffFactor << endl;
      MC2Point posOfSubString( 
         (int)(p1x * (1 - diffFactor) + p2x * diffFactor),
         (int)(p1y * (1 - diffFactor) + p2y * diffFactor) );
      
      boxes.push_back( getStringAsRectangle( text,
                                             posOfSubString, i, 1, angle ) );
   }
   return nbrChars;
}

void 
GtkMapPlotter::drawText( const STRING& text,
                         const MC2Point& center,
                         int startIdx,
                         int nbrChars,
                         float angle )
{
   // If m_cgc, m_gc or m_layout is NULL, then we can not draw anything.
   if(m_cgc == NULL || m_gc == NULL || m_layout == NULL) {
      return;
   }
   
   int char_length = 0;

   int startIndex = startIdx;
   int endIndex = startIdx + nbrChars;
   
   if( startIndex == endIndex || endIndex == -1) {   
      char_length = g_utf8_strlen( text, -1 );
      pango_layout_set_text( m_layout, text, - 1 );
   } else {
      // text iterator, get start
      char *text_i = g_utf8_offset_to_pointer( text, startIndex );
      
      const char *endOfString = g_utf8_offset_to_pointer( text_i,
                                                          endIndex - startIndex );
      // length of first utf-8 character
      char_length = g_utf8_strlen( text_i, endOfString - text_i );
   
      pango_layout_set_text( m_layout,
                             text_i,
                             endOfString - text_i );
   }

   int width, height;
   
   cairo_save(m_cgc);
   // cairo_set_source_rgb (m_cgc, 0.0, 0.0, 0.0);
   cairo_set_source_rgb (m_cgc,
                         double(m_penColor.red) / 65535,
                         double(m_penColor.green) / 65535,
                         double(m_penColor.blue) / 65535);
   pango_cairo_update_layout (m_cgc, m_layout);
   pango_layout_get_pixel_size (m_layout, &width, &height);
      
   cairo_identity_matrix(m_cgc);

   cairo_translate(m_cgc, center.getX(), center.getY());
   cairo_rotate(m_cgc, TrigUtil::deg2rad(-angle));
   cairo_translate(m_cgc, -width/2, -height/2);
      
   pango_cairo_show_layout (m_cgc, m_layout);
   cairo_restore(m_cgc);
}

bool 
GtkMapPlotter::returnsVisibleSizeOfBitmaps() const
{
   // Actually, the visible size of bitmaps are not used,
   // however we really want the tip of the bitmap to be pointing at
   // the correct spot, at the expense of using too large overlap boxes.
   return true;
}

void
GtkMapPlotter::plotCoordinates( vector<MC2Point>::const_iterator begin,
                                vector<MC2Point>::const_iterator end )
{
   // If m_gc is NULL, then we can not draw anything
   if(m_gc == NULL) {
      return;
   }
   GdkColor red;
   allocColor(red, 0xFF, 0, 0);
   gdk_gc_set_foreground( m_gc, &red );

   int offset = 2;
   int rectSize = offset << 1;
   for ( vector<MC2Point>::const_iterator it = begin;
         it != end;
         ++it ) {
      gdk_draw_rectangle( m_pixmap, m_gc,
                          true, // filled
                          it->getX()-offset,
                          it->getY()-offset,
                          rectSize, rectSize );
   }

   gdk_gc_set_foreground( m_gc, &m_penColor );
}


#define QUICK_AND_DIRTY

void
GtkMapPlotter::drawPolySomething( vector<MC2Point>::const_iterator begin,
                                  vector<MC2Point>::const_iterator end,
                                  bool filled)
{
   // If m_gc is NULL, then we can not draw anything
   if(m_gc == NULL) {
      return;
   }
   int npoints = end - begin;

#ifdef QUICK_AND_DIRTY
   MC2Point* tmpPtr = const_cast<MC2Point*>(&(*begin));
   GdkPoint* gdkPoints = reinterpret_cast<GdkPoint*>(tmpPtr);
#else
   GdkPoint* gdkPoints = new GdkPoint[npoints];
   int i = 0;
   GdkPoint tmppoint;
   for ( vector<MC2Point>::const_iterator it = begin;
         it != end;
         ++it ) {
      tmppoint.x = it->getX();
      tmppoint.y = it->getY();
      gdkPoints[i++] = tmppoint;
   }
#endif
   
   if ( ! filled ) {
      gdk_draw_lines( m_pixmap, m_gc, gdkPoints, npoints );
   } else {
      gdk_draw_polygon( m_pixmap, m_gc,
                        true, // filled
                        gdkPoints, npoints );
   }

#ifndef QUICK_AND_DIRTY
    delete[] gdkPoints;
#endif
    
   if ( m_plotCoordinates ) {
      plotCoordinates( begin, end );
   }

}

void
GtkMapPlotter::drawPolyLine( vector<MC2Point>::const_iterator begin,
                             vector<MC2Point>::const_iterator end )
   
{
   // If m_gc is NULL, then we can not draw anything
   if(m_gc == NULL) {
      return;
   }
   gdk_gc_set_foreground( m_gc, &m_penColor);

   drawPolySomething( begin, end, false );
}

void
GtkMapPlotter::drawPolygon( vector<MC2Point>::const_iterator begin,
                            vector<MC2Point>::const_iterator end )
{
   // If m_gc is NULL, then we can not draw anything
   if(m_gc == NULL) {
      return;
   }
   gdk_gc_set_foreground( m_gc, &m_fillColor);

   drawPolySomething( begin, end, true );
}


void
GtkMapPlotter::getMapSizePixels(isab::Rectangle& size) const
{
   const_cast<GtkMapPlotter*>(this)->m_window = m_widget->window;
   if (m_window == NULL) {
      // note: Investigate when and if this happens
      size = Rectangle( 0, 0, 300, 300 );
      return;
   }
   gint width, height;
   gdk_drawable_get_size(m_window, &width, &height);

   
   size = Rectangle( 0, 0, width, height );
}

isab::PointVector*
GtkMapPlotter::createPointVector(int size)
{
   // Must implement
   return new GtkPointVector(size);
}

void
GtkMapPlotter::deletePointVector(isab::PointVector* vect)
{
   // Must implement
   delete static_cast<GtkPointVector*>(vect);
}

void
GtkMapPlotter::setLineWidth( int width )
{
   // If m_gc is NULL, then we can not draw anything
   if(m_gc == NULL) {
      return;
   }

   // get old attributes and change the width
   // Set the line width and other properties.
   gdk_gc_set_line_attributes( m_gc,
                               width,
                               GDK_LINE_SOLID,
                               GDK_CAP_ROUND,
                               GDK_JOIN_ROUND);
}
   
void 
GtkMapPlotter::setCapStyle( capStyle_t capStyle )
{
   // If m_gc is NULL, then we can not draw anything
   if(m_gc == NULL) {
      return;
   }
   // get old attributes and change the cap style.
    
   GdkGCValues gcv;
   gdk_gc_get_values( m_gc, &gcv);

   GdkCapStyle gdkCapStyle = GDK_CAP_BUTT;
   GdkJoinStyle gdkJoinStyle = GDK_JOIN_MITER;
   switch (capStyle) {
      case (NO_CAP):
         gdkCapStyle = GDK_CAP_BUTT;
         gdkJoinStyle = GDK_JOIN_MITER;
         break;
      case (ROUND_CAP):
         gdkCapStyle = GDK_CAP_ROUND;
         gdkJoinStyle = GDK_JOIN_ROUND;
         break;
      case (FLAT_CAP):
         gdkCapStyle = GDK_CAP_PROJECTING;
         gdkJoinStyle = GDK_JOIN_MITER;
         break;
   }

   gdk_gc_set_line_attributes( m_gc,
                               gcv.line_width,
                               gcv.line_style,
                               gdkCapStyle,
                               gdkJoinStyle );

}

isab::BitMap*
GtkMapPlotter::createBitMap(isab::bitMapType type,
                            const uint8* bytes,
                            int nbrBytes,
                            int dpiCorrectionFactor )
{

   GtkBitMap* bitMap = new GtkBitMap;
   // Make temporary file to store the bitmap in
   char tempTemplate[1024];   
   sprintf(tempTemplate, "/tmp/tmpbitmapXXXXXX");
   int tmpDesc = mkstemp(tempTemplate);

   FILE* file = fdopen(tmpDesc, "w");
   if ( file ) {
      if ( fwrite( bytes, nbrBytes, 1, file ) == 1 ) {
         fclose(file);

         // Get "transparent" color
         mc2dbg << "[GTMP]: Filename is " << MC2CITE(tempTemplate)
                << endl;

         GdkColor white;
         allocColor(white, 0xFF, 0xFF, 0xFF);
         
         bitMap->m_pixMap = gdk_pixmap_create_from_xpm( 
                           m_pixmap,                           
                           &(bitMap->m_bitMap),
                           &white,
                           tempTemplate);

      } else {
         fclose(file);
         delete bitMap;
         bitMap = NULL;
      }
      // Remove the temp file.
      remove(tempTemplate);
   } else {
      delete bitMap;
      bitMap = NULL;
   }

   return bitMap;
}


void
GtkMapPlotter::getBitMapAsRectangle( isab::Rectangle& size,
                                     const MC2Point& origCenter,
                                     const isab::BitMap* bmp )

{
   
   const GtkBitMap* gtkBitMap = static_cast<const GtkBitMap*>(bmp);
   assert(gtkBitMap);
   assert(gtkBitMap->m_bitMap);

   int x = origCenter.getX();
   int y = origCenter.getY();
   int width, height;
   gdk_drawable_get_size(gtkBitMap->m_pixMap, &width, &height);
   
   size = isab::Rectangle( x - ( width >> 1), y - ( height >> 1),
                           width, height );

   
}

void
GtkMapPlotter::drawBitMap( const MC2Point& center,
                           const isab::BitMap* bitMap )
{
   // If m_gc is NULL, then we can not draw anything
   if(m_gc == NULL) {
      return;
   }
   const GtkBitMap* gtkBitMap = static_cast<const GtkBitMap*>(bitMap);
   
   isab::Rectangle rect;
   getBitMapAsRectangle(rect, center, bitMap);

   gdk_gc_set_clip_origin( m_gc, rect.getX(), rect.getY() );
   gdk_gc_set_clip_mask( m_gc, ((GtkBitMap*)gtkBitMap)->m_bitMap );

   gdk_draw_drawable( m_pixmap, m_gc, 
                      gtkBitMap->m_pixMap,
                      0, 0, 
                      rect.getX(), rect.getY(), 
                      rect.getWidth(), rect.getHeight());

   // clear clip origin and mask
   gdk_gc_set_clip_origin( m_gc, 0, 0);
   gdk_gc_set_clip_mask( m_gc, NULL );

   // Draw center coordinate
   if ( m_plotCoordinates ) {
      vector<MC2Point> points;
      points.push_back(center);
      plotCoordinates(points.begin(), points.end());
   }
   
}

void
GtkMapPlotter::deleteBitMap( isab::BitMap* bitMap )
{
   GtkBitMap* gtkBitMap = static_cast<GtkBitMap*>(bitMap);
   
   delete gtkBitMap;
}

void 
GtkMapPlotter::enableLowQualityDrawing( bool on )
{ 
#if 0
   // Debug stuff to see what's going on.
   mc2dbg << "GtkMapPlotter::enableLowQualityDrawing" << on << endl;
   m_plotCoordinates = on;
#endif
}

void
GtkMapPlotter::drawImageSpec(const WFAPI::ImageSpec* spec,
                             const MC2Point& point)
{
   // If m_gc is NULL, then we can not draw anything
   if(m_gc == NULL) {
      return;
   }
   const GdkPixbufImageSpec* pbuf = static_cast<const GdkPixbufImageSpec*>(spec);

   if( m_pixmap ) {
      gdk_draw_pixbuf( m_pixmap,
                       m_gc, pbuf->m_pixBuf,
                       0, 0,
                       point.getX(), point.getY(), 
                       pbuf->m_width, pbuf->m_height,
                       GDK_RGB_DITHER_NONE,
                       0, 0);      
   }
   
}

void GtkMapPlotter::setClippingRectangle(int x, int y,
                                         int width, int height)
{
   // If m_cgc or m_gc is NULL, then we can not draw anything.
   if(m_cgc == NULL || m_gc == NULL) {
      return;
   }
   GdkRectangle clip_rect;
   clip_rect.x = x;
   clip_rect.y = y;
   clip_rect.width = width;
   clip_rect.height = height;
   
   gdk_gc_set_clip_rectangle(m_gc, &clip_rect);
   cairo_new_path(m_cgc);
   cairo_rectangle(m_cgc, x, y, width, height);
   cairo_clip(m_cgc);
}

void GtkMapPlotter::resetClippingRectangle()
{
   // If m_cgc or m_gc is NULL, then we can not draw anything.
   if(m_cgc == NULL || m_gc == NULL) {
      return;
   }
   gdk_gc_set_clip_rectangle(m_gc, NULL);
   cairo_reset_clip(m_cgc);
}


void GtkMapPlotter::resetFontStyle()
{
   setFontStyle(m_defaultFont); 
}

void GtkMapPlotter::setFontStyle(isab::Font* font)
{
   if(m_layout == NULL) {
      return;
   }
   GtkPlotterFont* plotterFont = static_cast<GtkPlotterFont*>(font);
   pango_layout_set_font_description( m_layout, plotterFont->m_fontDesc );    
}

isab::Rectangle GtkMapPlotter::getTextDimensions(const STRING& text)
{   
   int width, height;
   getTextSize( m_layout, text, width, height );

   return isab::Rectangle( 0, 0, width, height);
}

isab::Font* GtkMapPlotter::createFont(WFAPI::FontSpec* fontSpec)
{
   const WFAPI::WFString& fontName = fontSpec->getFontString();
   m_fonts.insert(fontSpec);
   fontSpec->addReference();
   
   
   if(fontName.empty()) {
      return new GtkPlotterFont(getFontName());
   } else {
      return new GtkPlotterFont(fontName.c_str());
   }
}

void GtkMapPlotter::deleteFont(isab::Font* font)
{
   delete font;
}

GtkPlotterFont::GtkPlotterFont(const char* name)
{
   if(name) {
      m_fontDesc = pango_font_description_from_string( name );      
   } else {
      m_fontDesc = pango_font_description_from_string( "" );      
   }
}

GtkPlotterFont::~GtkPlotterFont()
{
   if(m_fontDesc) {
      pango_font_description_free(m_fontDesc);
   } 
}

void
GtkMapPlotter::releaseFonts()
{
   set<WFAPI::FontSpec*>::iterator it = m_fonts.begin();
   set<WFAPI::FontSpec*>::iterator itEnd = m_fonts.end();

   for(; it != itEnd; it++) {
      if((*it) != NULL) {
         static_cast<FontSpecInternal*>((*it))->releaseData();
         
        (*it)->removeReference();
      }
      m_fonts.erase(it);
   }
   
}
