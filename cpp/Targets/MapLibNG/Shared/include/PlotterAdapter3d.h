/*
  Copyright (c) 1999 - 2010, Vodafone Group Services Ltd
  All rights reserved.

  Redistribution and use in source and binary forms, with or without modification, are permitted provided that the following conditions are met:

  * Redistributions of source code must retain the above copyright notice, this list of conditions and the following disclaimer.
  * Redistributions in binary form must reproduce the above copyright notice, this list of conditions and the following disclaimer in the documentation and/or other materials provided with the distribution.
  * Neither the name of the Vodafone Group Services Ltd nor the names of its contributors may be used to endorse or promote products derived from this software without specific prior written permission.

  THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS" AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE ARE DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT HOLDER OR CONTRIBUTORS BE LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
*/


#ifndef PLOTTER_ADAPTER_3D_H 
#define PLOTTER_ADAPTER_3D_H

#include "PlotterAdapter.h"
#include "MC2Point.h"
#include <vector>
#include "MC2BoundingBox.h"
#include "ClipUtil.h"

typedef float vec_t;

struct Settings3D {
   float  m_delta;
   float  m_alpha;
   uint32 m_horizonPixels;
};

namespace isab {
/**
 *   PlotterAdapter that projects the data in 3d.
 */
class PlotterAdapter3d : public isab::PlotterAdapter {   
public:
   
   /// Constructor.
   PlotterAdapter3d( isab::MapPlotter* plotter );

   /**
    *   Virtual destructor.
    */
   virtual ~PlotterAdapter3d();
  
   /**
    *   Returns if the map area / window is visible.
    *
    *   @return True if the map area is visible, false otherwise.
    */
   bool isVisible() const;

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
    *   Draws a polyline defined by the array of points.
    */
   void drawPolyLine( std::vector<MC2Point>::const_iterator begin,
                              std::vector<MC2Point>::const_iterator end );

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
    *   Draws the bitmap at x and y.
    *   Default implementation draws a small rectangle.
    */
   void drawBitMap( const MC2Point& center,
                            const BitMap* bitMap);

   /**
    * Sets the horizon height, the pixel height.
    */
   void setHorizonHeight( uint32 height );

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
    *   Sets the width of the lines.
    */
   void setLineWidth( int width );


   /// --- Other methods.
   
   /// Transform point in 2D to 3D.
   MC2Point transform2DTo3D( const MC2Point& point2D );

   /// Transform point in 2D to 3D.
   MC2Pointf transform2DTo3Df( const MC2Pointf& point2D );
   
   /// Transform 3D point to 2D (slow):
   MC2Point transform3DTo2D_slow( const MC2Point& point3D );

   MC2Pointf transform3DTo2D_slowf( const MC2Point& point3D );
   
   virtual void drawImageSpec(const WFAPI::ImageSpec* imgSpec,
                              const MC2Point& point);

   /**
    * Crops the string, modifies the string to fit within the
    * maxWidth with the last three chars as ...
    *
    * @param text The text to crop.
    * @param maxWidth the maximal width in pixels.
    */
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

   /**
    *   Creates a platform specific font using the specification fontSpec.
    */ 
   virtual isab::Font* createFont(WFAPI::FontSpec* fontSpec);

   /**
    * Release font data for all fonts used.
    */
   virtual void releaseFonts();

   /**
    *   Deallocates a platform specific font.
    */ 
   virtual void deleteFont(isab::Font* font);
private:

   /**
    *    Transform the points to 3d projection.
    *    Will modify the original data to project.
    *    @param   begin Begin iterator for the points to project.
    *    @param   end   End iterator for the points to project.
    *    @param   scalefactor Scalefactor that the resulting points will
    *                         be rightshifted with. Default 0.
    */
   void project3dNoCopy( std::vector<MC2Point>::iterator begin,
                   std::vector<MC2Point>::iterator end,
                   std::vector<MC2Point>& res, 
                   int scalefactor = 0 );

   void project3dNoCopyf( std::vector<MC2Pointf>::iterator begin,
                          std::vector<MC2Pointf>::iterator end,
                          std::vector<MC2Pointf>& res, 
                          int scalefactor = 0 );
   
   /**
    *    Transform the points to 3d projection.
    *    Will not modify the original data to project.
    *    @param   begin Begin iterator for the points to project.
    *    @param   end   End iterator for the points to project.
    *    @param   scalefactor Scalefactor that the resulting points will
    *                         be rightshifted with. Default 0.
    */
   void project3d( std::vector<MC2Point>::const_iterator begin,
                   std::vector<MC2Point>::const_iterator end,
                   std::vector<MC2Point>& res, 
                   int scalefactor = 0 );

   /**
    * Calculates a polygon from a polyline with the given line width.
    *
    * @param begin Begin iterator for the points to draw.
    * @param end   End iterator for the points to draw.
    * @param lineWidth The polygon width.
    */
   void plotterLine( std::vector<MC2Point>::const_iterator begin,
                     std::vector<MC2Point>::const_iterator end,
                     int lineWidth );
   
   /**
    * Calculates a polygon from a polyline with the given line width.
    * This method requires that the resulting polygon is non self
    * intersecting.
    *
    * @param begin Begin iterator for the points to draw.
    * @param end   End iterator for the points to draw.
    * @param lineWidth The polygon width.
    */
   void plotterLineNonSelfIntersecting( 
      std::vector<MC2Point>::const_iterator begin,
      std::vector<MC2Point>::const_iterator end,
      int lineWidth );

   /**
    * Calculates/updates members needed for 3d projection,
    * this is done when either screen size or scale factor
    * has changed.
    * @param scalefactor The wanted scalefactor.
    */
   void checkScreenAndUpdateMembers( int scalefactor );
   

    /**
    * Holds the red value for setting the pen color (also used
    * when as fill color, when drawing a drawPolyLine
    */
   int m_red;

    /**
    * Holds the green value for setting the pen color (also used
    * when as fill color, when drawing a drawPolyLine
    */
   int m_green;

    /**
    * Holds the green value for setting the pen color (also used
    * when as fill color, when drawing a drawPolyLine
    */
   int m_blue;

   /**
    * Stores the line width.
    */
   int m_lineWidth;

   isab::Rectangle m_screenSizeInPixels;



   vec_t m_width;
   vec_t m_height;
   vec_t m_horizHeight;
   int m_scalefactor;

   vec_t m_Va;
   vec_t m_Vb;
   vec_t m_Vc;
   vec_t m_Vd;
   vec_t m_Ve;
   vec_t m_Vf;
   vec_t m_Vg;
   vec_t m_Vh;
   vec_t m_Vi;
   vec_t m_Vj;
   vec_t m_Vk;
   vec_t m_Vl;
   vec_t m_sz;
   vec_t m_tz;
   vec_t m_r;
   vec_t m_Pa; 
   vec_t m_Pb;
   vec_t m_Pc;
   vec_t m_Pd;


   vec_t m_Pa_times_m_Vd;
   vec_t m_Pb_times_m_Vf;
   vec_t m_Pb_times_m_Vh;

   int m_intPa;
   int m_intPa_times_m_Vd;
   int m_intPb_times_m_Vf;
   int m_intPb_times_m_Vh;
   int m_intVj;
   int m_intVl;
  
   int m_intWidth;
   int m_intHeight;
   int m_intHorizHeight;
   
   std::vector<MC2Point> m_points3d;
   std::vector<MC2Point> m_polylineRect;
   std::vector<MC2Point> m_prevPolylineRect;
   std::vector<MC2Point> m_linePolygon;
   std::vector<MC2Point> m_triangle;
   std::vector<MC2Point> m_clippedPoints;
   // This is a temp variable used in the drawBitMap function, in order
   // to supply correct iterators to project3dNoCopy.
   std::vector<MC2Point> m_bitmapCenterPoint;

   /**
    * Temporary vector containing the left hand side points when
    * creating polygon out of a polyline.
    */
   std::vector<MC2Point> m_leftSide;
   
   /**
    * Temporary vector containing the left hand side points when
    * creating polygon out of a polyline.
    */
   std::vector<MC2Point> m_rightSide;
   
   void makeTestVectors();

   std::vector<MC2Point> m_testVector;
   std::vector<std::vector<MC2Point> > m_testTriangles;

 
   MC2BoundingBox m_clipBox;
   ClipUtil m_clipUtil;
  

   /// Typedef for vector of begin, end MC2Point iterators.
   typedef std::vector<MC2Point>::const_iterator  it_t; 

   /**
    * Temporary vector containing (begin,end) for non intersecting
    * polylines.
    */
   std::vector<std::pair<it_t, it_t> > m_nonIntersectingPolylines;

   // make it easy to modify the settings from inside the client.
public:
   Settings3D m_settings;
};
} // namespace
#endif
