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
#include "MapPlotter.h"
#include "MC2Point.h"
#include "TextCachingBlock.h"
#include "WFString.h"

#include "OverlayItemVisualSpec.h"
#include "DebugDrawing.h"
#include "FontSpecInternal.h"

using namespace std;
using namespace isab;

// -- PointVector
void
PointVector::setPoints(const vector<MC2Point>& points)
{
   reset();
   for( vector<MC2Point>::const_iterator it = points.begin();
        it != points.end();
        ++it ) {
      addPoint( (*it).getX(), (*it).getY() );
   }
}

// -- MapPlotter.

/* default implementations for virtual functions */

bool
MapPlotter::isVisible() const
{
   return true;
}

int MapPlotter::getMinTextRotation()
{
	return(0);
}

void 
MapPlotter::drawTextAtBaselineLeft( const STRING& text,
                                    const MC2Point& point )
{
   // This method should be overriden by the subclasses for
   // improved efficiency.
   isab::Rectangle rect = getTextDimensions( text );
   // Convert to bottom center point.
   MC2Point centerPoint( point.getX() + rect.getWidth() / 2,
                         point.getY() + rect.getHeight() / 2 );
   
   drawText( text, centerPoint );
}


void
MapPlotter::drawText( const STRING& /*text*/,
                      const MC2Point& /*point*/,
                      int /*startIdx*/,
                      int /*nbrChars*/,
                      float /*angle*/)
{
}

bool MapPlotter::doubleBuffered() const
{
	return 0;
}

bool MapPlotter::drawBuffer( const isab::Rectangle& /*rect*/ )
{
	return 0;
}

void MapPlotter::prepareDrawing()
{
	return;
}

void MapPlotter::triggerApplicationRedraw()
{
	return;
}

BitMap* MapPlotter::createBitMap(bitMapType /*type*/,
                                 const unsigned char* /*bytes*/,
                                 int /*nbrBytes*/,
                                 int /*dpiCorrectionFactor*/)
{
	return (NULL);
}

void MapPlotter::drawBitMap( const MC2Point&,
                             const BitMap*)
{
	return;
}

void MapPlotter::deleteBitMap( BitMap* )
{
	return;
}

void
MapPlotter::getBitMapAsRectangle(isab::Rectangle&,
                                 const MC2Point&,
                                 const isab::BitMap*)
{
}

const char*
MapPlotter::getBitMapExtension() const
{
#if defined NAV2_CLIENT_SERIES80 || defined NAV2_CLIENT_SERIES90_V1
   return "gif";
#elif defined NAV2_CLIENT_SERIES60_V3
   return "mif";
#else
   return "png";
#endif
}

void MapPlotter::drawPolygon( vector<MC2Point>::const_iterator begin,
                              vector<MC2Point>::const_iterator end )
{
	drawPolyLine(begin, end);
}

void MapPlotter::drawSpline( bool /*filled*/,
                             vector<MC2Point>::const_iterator begin,
                             vector<MC2Point>::const_iterator end )
{
   drawPolyLine(begin, end);
}

void MapPlotter::drawConvexPolygon( vector<MC2Point>::const_iterator begin,
                                    vector<MC2Point>::const_iterator end )
{
   drawPolygon(begin, end);
}


void MapPlotter::drawRect( bool filled,
                           const isab::Rectangle& rect,
                           int /*cornerSize*/)
{
   // Create Rectangle from pointvector
   vector<MC2Point> points;
   points.push_back( MC2Point( rect.getX(), rect.getY() ) );
   points.push_back( MC2Point( rect.getX(), rect.getY() + rect.getHeight() ) );
   points.push_back( MC2Point( rect.getX() + rect.getWidth(),
                               rect.getY() + rect.getHeight() ) );
   points.push_back( MC2Point( rect.getX() + rect.getWidth(),
                               rect.getY() ) );
   points.push_back( MC2Point( rect.getX(), rect.getY() ) );
   // Draw filled rectangle or not.
	if ( filled ) {
      drawPolygon( points.begin(), points.end() );
   } else {
      drawPolyLine( points.begin(), points.end() );
   }   
}

void MapPlotter::drawArc( bool filled, const isab::Rectangle& rect,
                          int /*startAngle*/, int /*stopAngle*/)
{
	drawRect(filled, rect);
}

void MapPlotter::setFillColor( unsigned int /*red*/, unsigned int /*green*/,
                               unsigned int /*blue*/ )
{
	return;
}

void MapPlotter::setLineWidth( int /*width*/ )
{
	return;
}

void MapPlotter::setCapStyle( capStyle_t /*cStyle*/ )
{
	return;
}

void MapPlotter::setDashStyle( dashStyle /*dStyle*/ )
{
	return;
}

bool MapPlotter::snapshotHandlingImplemented() const
{
   // Default is that the snapshot handling is not implemented.
   return false;
}

void MapPlotter::makeScreenSnapshot()
{
   // Default implementation does nothing.
}

void MapPlotter::moveScreenSnapshot( const MC2Point& deltaPoint )
{
   // Default implementation does nothing.
}

void MapPlotter::zoomScreenSnapshot(double factor, const MC2Point& zoomPoint)
{
   // Default implementation does nothing.
}

void MapPlotter::lockBitmapBuffer()
{
   // Default implementation does nothing.
}

void MapPlotter::unlockBitmapBuffer()
{
   // Default implementation does nothing.
}

uint32* MapPlotter::getBitmapBuffer()
{
   // Default implementation returns NULL
   return NULL;
}

int MapPlotter::getColorDepth()
{
   // Default implementation does nothing.
   return -1;
}

int MapPlotter::getStringAsRectangles( vector<isab::Rectangle>& boxes,
                                       const STRING& text,
                                       const MC2Point& center,
                                       int startIdx,
                                       int nbrChars,
                                       float angle )
{
   return 0;
}

void
MapPlotter::enableLowQualityDrawing( bool on )
{
   // Default implementation does nothing.   
}

isab::Rectangle MapPlotter::getStringAsRectangle( const STRING& text,
                                                  const MC2Point& center,
                                                  int startIdx,
                                                  int nbrChars,
                                                  float angle )
{
   return isab::Rectangle(0,0,0,0);    
}

/**
 *  We use this method to compare the rectangles since we are
 *  not interested in the position of rectangle but only the
 *  dimensions (because those are the only relevant parameters for
 *  the text cropping).
 */ 
bool rectDimsEqual(const isab::Rectangle& lhs,
                   const isab::Rectangle& rhs)
{
   return
      lhs.getWidth()  == rhs.getWidth() &&
      lhs.getHeight() == rhs.getHeight();
}

void isab::MapPlotter::drawOverlayText(
   const WFAPI::WFString& text,
   TextCachingBlock& cacheBlock,
   const WFAPI::OverlayItemVisualSpec::TextAlignment alignment,
   WFAPI::FontSpec* font,
   const isab::Rectangle& largeRect,
   const WFAPI::OverlayItemVisualSpec::RGB& textColor)
{
   //TODO: verify if we have to check dimensions each time
   //TODO: if we don't, we might end up with incorrect cropping etc
   //TODO: this only applies if we have different fonts / sizes and the same
   //TODO: largeRect, alignment and text since previous zoom level

   //TODO: That we cannot crop most numbers since they are less than 3
   //TODO: characters is a known "bug". The algorithm is still perfect.
   
   FontSpecInternal* fontSpec = NULL;
   
   if(font) {
      fontSpec = static_cast<FontSpecInternal*>(font);

      if(fontSpec->m_plotterFont == NULL) {
         fontSpec->m_allocator = this;
         fontSpec->m_plotterFont = createFont(fontSpec);
      }
   }

   isab::Font* plotterFont = NULL;

   if(fontSpec) {
      plotterFont = fontSpec->m_plotterFont;
      setFontStyle(plotterFont);
   }

   // Update the block if necessary
   if(cacheBlock.plotter.nameString &&
      (text != cacheBlock.source
       || !rectDimsEqual(largeRect, cacheBlock.sourceRect)))
   {
      deleteString(cacheBlock.plotter.nameString);
      cacheBlock.plotter.nameString = NULL;
      cacheBlock.sourceRect = isab::Rectangle();
   }

   
   if(!cacheBlock.plotter.nameString) {
      cacheBlock.source = text;

      cacheBlock.plotter.stringAllocator = this;
      cacheBlock.sourceRect = largeRect;
      
      STRING* tmp = createString(text.c_str());
      
      isab::Rectangle tmpRect = getTextDimensions(*tmp);
      
      if (largeRect.getWidth() > 0 &&
          tmpRect.getWidth() > largeRect.getWidth())
      {
         // Need to crop
         cropString(*tmp, largeRect.getWidth());
      } 
      
      cacheBlock.plotter.nameString = tmp;
   }

   isab::Rectangle smallRect =
      getTextDimensions(*cacheBlock.plotter.nameString);

   MC2Point centerPos;

   switch (alignment.m_horizAlign) {
   case WFAPI::OverlayItemVisualSpec::TEXT_HALIGN_LEFT:
      centerPos.getX() = largeRect.getX() + smallRect.getWidth() / 2;
      break;
   case WFAPI::OverlayItemVisualSpec::TEXT_HALIGN_CENTER:
      centerPos.getX() = largeRect.getX() + largeRect.getWidth() / 2;
      break;
   case WFAPI::OverlayItemVisualSpec::TEXT_HALIGN_RIGHT:
      centerPos.getX() = largeRect.getX() + largeRect.getWidth() -
         smallRect.getWidth() / 2;
      break;
   }

   switch (alignment.m_vertAlign) {
   case WFAPI::OverlayItemVisualSpec::TEXT_VALIGN_TOP:
      centerPos.getY() = largeRect.getY() + smallRect.getHeight() / 2;
      break;
   case WFAPI::OverlayItemVisualSpec::TEXT_VALIGN_CENTER:
      centerPos.getY() = largeRect.getY() + largeRect.getHeight() / 2;
      break;
   case WFAPI::OverlayItemVisualSpec::TEXT_VALIGN_BOTTOM:
      centerPos.getY() = largeRect.getY() + largeRect.getHeight() -
         smallRect.getHeight() / 2;
      break;
   }
   
   setPenColor(textColor.m_red, textColor.m_green, textColor.m_blue);
   drawText(*cacheBlock.plotter.nameString, centerPos);

   isab::Rectangle tmpRect(centerPos.getX() - smallRect.getWidth() / 2,
                           centerPos.getY() - smallRect.getHeight() / 2,
                           smallRect.getWidth(), smallRect.getHeight());

   resetFontStyle();
   // DebugDrawing::rectangle(largeRect, this);
   // DebugDrawing::rectangle(tmpRect, this);
}

void
MapPlotter::releaseFonts() {
   return;
}

void
MapPlotter::endFrame()
{
}
