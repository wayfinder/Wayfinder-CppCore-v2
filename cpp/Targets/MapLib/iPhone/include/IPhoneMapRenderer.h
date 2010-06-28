/*
  Copyright (c) 1999 - 2010, Vodafone Group Services Ltd
  All rights reserved.

  Redistribution and use in source and binary forms, with or without modification, are permitted provided that the following conditions are met:

  * Redistributions of source code must retain the above copyright notice, this list of conditions and the following disclaimer.
  * Redistributions in binary form must reproduce the above copyright notice, this list of conditions and the following disclaimer in the documentation and/or other materials provided with the distribution.
  * Neither the name of the Vodafone Group Services Ltd nor the names of its contributors may be used to endorse or promote products derived from this software without specific prior written permission.

  THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS" AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE ARE DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT HOLDER OR CONTRIBUTORS BE LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
*/

#ifndef IPHONEMAPRENDERER_H
#define IPHONEMAPRENDERER_H

namespace WFAPI {
class ImageSpec;
}

#include "MapPlotter.h"
#include <vector>
#include <list>
#include <map>
#import <Foundation/Foundation.h>
#import <UIKit/UIKit.h>
#import <QuartzCore/QuartzCore.h> //CGContextRef

using namespace std;

namespace isab {


class IPhoneBitMap : public BitMap
{
public:
	UIImage *image;
};

class ReadyForUpdate
{
public:
	virtual void callUpdate(){};
};


class IPhoneMapRenderer : public isab::MapPlotter
{
public:
	IPhoneMapRenderer(){};	
 	virtual ~IPhoneMapRenderer(){};	 
	 
	IPhoneMapRenderer(int aWidth, int aHeight);	
	 
	//to set/get drawing handler
	void setCallBack(ReadyForUpdate* updater);

	ReadyForUpdate *getCallBack();

private:
	ReadyForUpdate *updater;
	
    /* overall draw time */
	uint32 m_lastDrawTime;

	CGContextRef drawingContext;

	size_t contextWidth;

	size_t contextHeight;

	unsigned int penColorOpacity;

	unsigned int fillColorOpacity;

	UIFont *selectedFont;

	UIImage *snapshotImage;
	  
    enum {
         /// The index of the buffer for the map
         c_mainBuffer = 0,
         /// The index of the buffer for the user-defs
         c_secondBuffer = 1,
         /// The index of the mask for the user-defs
         c_secondBufferMask = 2,
#ifdef USE_ONE_DRAW_BUFFER
         /// The number of draw buffers.
         c_nbrDrawBuffers = 1,
#else
         /// The number of draw buffers.
         c_nbrDrawBuffers = 3,
#endif
    };
      
	int iCurSize;     /* current line drawing size */

    int iCapStyle;    /* stores the type of line ending to use */

    int iDashStyle;   /* stores the type of line drawing to use */

    /* for testing drawing speeds */
    uint32 logStart, logEnd;
    uint32 m_adjustTimeDelta;

    /// Max rotated cache size constants.
    enum {
         // At most 50k can be used for the cached bitmaps.
         c_maxCacheBmpSize = 50*1024,
         // And at least 500k memory must be available before using
         // this cache.
         c_minMemFree = 500*1024
    };

     
public:   
	void clearScreen(); 

	void setBackgroundColor(unsigned int red, unsigned int green, unsigned int blue);

	void setPenColor(unsigned int red, unsigned int green, unsigned int blue);

	void setFont(const STRING& fontName, int fontSize);

	void getMapSizePixels(Rectangle& size) const;

	PointVector* createPointVector(int size);

	void deletePointVector(PointVector* vect);

	STRING* createString(const char* text);

	void deleteString(STRING* text);

    int getStringLength(const STRING& text);

	int getMinTextRotation();

	void drawText(const STRING& text, const MC2Point& point, int startIdx, int nbrChars, float angle);

	Rectangle getStringAsRectangle(const STRING& text,
								   const MC2Point& point,
								   int startIdx,
								   int nbrChars,
								   float angle);

	int getStringAsRectangles(std::vector<Rectangle>& boxes,
							  const STRING& text,
							  const MC2Point& point,
							  int startIdx,
							  int nbrChars,
							  float angle);

	bool textDirectionLeftToRight(const STRING& text) ;

	bool returnsVisibleSizeOfBitmaps() const ;

	bool doubleBuffered() const;

	bool drawBuffer(const Rectangle& rect);

	void prepareDrawing();

	void triggerApplicationRedraw();

	BitMap* createBitMap(bitMapType type,
						 const uint8* bytes,
						 int nbrBytes,
						 int dpiCorrectionFactor);

	void drawBitMap(const MC2Point& center,
					const BitMap* bitMap);

	void getBitMapAsRectangle(Rectangle& size,
							  const MC2Point& origCenter,
							  const BitMap* bmp);

	Rectangle getBitMapAsRectangle(const MC2Point& origCenter,
								   const BitMap* bmp);

	void deleteBitMap(BitMap* bitMap);

	void setFillColor(unsigned int red,
					  unsigned int green,
					  unsigned int blue);

	void drawPolygon(std::vector<MC2Point>::const_iterator begin,
					 std::vector<MC2Point>::const_iterator end);

	void drawPolygonWithColor(std::vector<MC2Point>::const_iterator begin,
							  std::vector<MC2Point>::const_iterator end,
							  uint32 fillcolor);

	void drawPolyLine(std::vector<MC2Point>::const_iterator begin,
					  std::vector<MC2Point>::const_iterator end);

	void drawPolyLineWithColor(std::vector<MC2Point>::const_iterator begin,
							   std::vector<MC2Point>::const_iterator end,
							   uint32 color,
							   int lineWidth);
	void drawSpline(bool filled,
					std::vector<MC2Point>::const_iterator begin,
					std::vector<MC2Point>::const_iterator end);

	void drawArc(bool filled,
				 const Rectangle& rect,
				 int startAngle,
				 int stopAngle);

	void drawRect(bool filled,
				  const Rectangle& rect,
				  int cornerSize);

	void setLineWidth(int width);

	void setCapStyle(enum capStyle);

	void setDashStyle(enum dashStyle);

	bool snapshotHandlingImplemented() const;

	void makeScreenSnapshot();

	void moveScreenSnapshot( const MC2Point& deltaPoint );

	void zoomScreenSnapshot(double factor, const MC2Point& zoomPoint);

	void lockBitmapBuffer();

	void unlockBitmapBuffer();

	uint32* getBitmapBuffer();

	int getColorDepth();
	   
	void setDrawingLayer(CGLayerRef layer);

    void enableLowQualityDrawing(bool status);
		  
	//helper methods
	void setContext(CGContextRef context, size_t width = 0, size_t height = 0);

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
private:
    void finishLineDrawing();
    void finishPolygonDrawing();

    bool isLineDrawingFinished;
    uint32 prevLineColor;
	int prevLineWidth;
    
    bool isPolygonDrawingFinished;
    uint32 prevPolygonColor;

	CGLayerRef drawingLayer;
    CGFloat penColors[3];
};


} //isab

#endif
