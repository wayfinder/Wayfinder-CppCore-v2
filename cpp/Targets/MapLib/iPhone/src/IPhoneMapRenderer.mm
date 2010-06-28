/*
  Copyright (c) 1999 - 2010, Vodafone Group Services Ltd
  All rights reserved.

  Redistribution and use in source and binary forms, with or without modification, are permitted provided that the following conditions are met:

  * Redistributions of source code must retain the above copyright notice, this list of conditions and the following disclaimer.
  * Redistributions in binary form must reproduce the above copyright notice, this list of conditions and the following disclaimer in the documentation and/or other materials provided with the distribution.
  * Neither the name of the Vodafone Group Services Ltd nor the names of its contributors may be used to endorse or promote products derived from this software without specific prior written permission.

  THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS" AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE ARE DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT HOLDER OR CONTRIBUTORS BE LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
*/

#import "IPhoneMapRenderer.h"
#import "MC2Point.h"
#import <Foundation/Foundation.h>
#import <UIKit/UIKit.h>

using namespace WFAPI;

#define COLOR_RED 0
#define COLOR_GREEN 1
#define COLOR_BLUE 2

namespace isab {

static const size_t MAX_NAME_LENGTH = 100; //?

void IPhoneMapRenderer::setContext(CGContextRef context, size_t width, size_t height)
{
	drawingContext = context;
    contextWidth = width;
  	contextHeight = height;

    CGContextSetLineJoin(drawingContext, kCGLineJoinRound);  
    CGContextSetLineCap(drawingContext, kCGLineCapRound);
    
    CGContextSetAllowsAntialiasing(drawingContext, YES);
    CGContextSetShouldAntialias(drawingContext, YES);
}


IPhoneMapRenderer::IPhoneMapRenderer(int aWidth, int aHeight)
    : contextWidth(aWidth),
      contextHeight(aHeight),
      selectedFont([UIFont systemFontOfSize:8]),
      snapshotImage(nil),
      drawingContext(nil),
      updater(NULL), 
      isLineDrawingFinished(true),
      prevLineColor(0),
	  prevLineWidth(0),
      isPolygonDrawingFinished(true),
      prevPolygonColor(0)
{
};		

		
// -- Functions that must be implemented ---------------
		
/**
 *   Fills the screen with the background color.
 */
void IPhoneMapRenderer::clearScreen()
{
	if (drawingContext)
        CGContextFillRect(drawingContext, CGRectMake(0, 0, contextWidth,
                                                     contextHeight));
}

		
/**
 *   Sets the background color.
 *   <br />
 *   NB! The server will not send impossible values once the
 *   description formats are done.
 *   @param red   Red value   0-255.
 *   @param green Green value 0-255.
 *   @param blue  Blue value  0-255.
 */
void IPhoneMapRenderer::setBackgroundColor(unsigned int red,
										   unsigned int green,
										   unsigned int blue)
{
	if (drawingContext != nil) {
        finishPolygonDrawing();
        prevPolygonColor = ((((red << 8) | green) << 8) | blue);

        CGContextSetRGBFillColor(drawingContext, (CGFloat)red/255,
                                 (CGFloat)green/255, (CGFloat)blue/255,
                                 1.0);
	}
}
		

/**
 *   Sets the color of the pen.
 *   <br />
 *   NB! The server will not send impossible values once the
 *   description formats are done.
 *   @param red   Red value   0-255.
 *   @param green Green value 0-255.
 *   @param blue  Blue value  0-255.
 */
void IPhoneMapRenderer::setPenColor(unsigned int red,
									unsigned int green,
									unsigned int blue)
{
	if (drawingContext != nil) {
        finishLineDrawing();
        prevLineColor = ((((red << 8) | green) << 8) | blue);
        
        penColors[COLOR_RED] = red / 255.0;
        penColors[COLOR_GREEN] = green / 255.0;
        penColors[COLOR_BLUE] = blue / 255.0;

        CGContextSetRGBStrokeColor(drawingContext, penColors[COLOR_RED],
                                   penColors[COLOR_GREEN],
                                   penColors[COLOR_BLUE], 1.0);
	}
}


/**
 *   Puts the dimensions of the displayed map in the
 *   variables.
 */
void IPhoneMapRenderer::getMapSizePixels(Rectangle& size) const
{
	static isab::Rectangle bitmapSize(0, 0, contextWidth, contextHeight);
	size = bitmapSize;
}


/**
 *   Creates a pointvector of size <code>size</code>.
 */
PointVector* IPhoneMapRenderer::createPointVector(int size)
{
	return 0;
}


/**
 *   Returns the pointvector to the IPhoneMapRenderer.
 */
void IPhoneMapRenderer::deletePointVector(PointVector* vect)
{
}


// Text functions -------------------------------------------------
		
/**
 *   Creates a text string for use with the other text methods.
 *   Default implementation returns NULL.
 */
STRING* IPhoneMapRenderer::createString(const char* text)
{
	if (text == NULL) {
		return(NULL);
	}
	
	//create a Unicode string from the UTF8 string
	STRING* ptr = new STRING[1];
    *ptr = CFStringCreateWithCString(NULL, text, kCFStringEncodingUTF8);
   
	return ptr;
}


/**
 *   Deletes a text string.
 *   Default implementation does nothing.
 */   
void IPhoneMapRenderer::deleteString(STRING* text)
{
    CFRelease(*text);
    delete text;
}


/**
 *   @return The number of characters in the string.
 */
int IPhoneMapRenderer::getStringLength(const STRING& text)
{
    return CFStringGetLength(text);
}
 
 
/**
 *   Sets font. The description format in the server
 *   and the IPhoneMapRenderer must know what needs to be put
 *   here.
 *   @param fontName Name of the font in client-specific format.
 *   @param size     Size of the font in UNIT.
 */
void IPhoneMapRenderer::setFont(const STRING& fontName, int fontSize)
{
    selectedFont = [UIFont systemFontOfSize:fontSize];
}


/**
 *   Returns the rotation step possible by the plotter.
 *   <br />Default implementation returns 0.
 *   @return The minimum number of degrees that characters can
 *           be rotated.
 */
int IPhoneMapRenderer::getMinTextRotation()
{
	return 0;
}


/**
 *   Draws text on the display using the pen color and transparent
 *   background.
 *   @param text      The start of the text to draw.
 *   @param points    The center point of the text.
 *   @param startIdx  The first character to draw.
 *   @param nbrChars  The number of characters to draw.
 */
void IPhoneMapRenderer::drawText(const STRING& text, const MC2Point& point, int startIdx, int nbrChars, float angle)
{
	if (drawingContext != nil) {

		if (nbrChars == -1) {
			nbrChars = CFStringGetLength(text);
		}

        CFStringRef stringToDraw =
            CFStringCreateWithSubstring(NULL, text,
                                              CFRangeMake(startIdx, nbrChars));
        if (stringToDraw == NULL)
            return;

        CGSize sz = [(NSString *)stringToDraw sizeWithFont:selectedFont];
        CGLayerRef textLayer =
            CGLayerCreateWithContext(drawingContext, sz, NULL);

        UIGraphicsPushContext(CGLayerGetContext(textLayer));
        CGContextSetRGBFillColor(UIGraphicsGetCurrentContext(),
                                 penColors[COLOR_RED], penColors[COLOR_GREEN],
                                 penColors[COLOR_BLUE], 1.0);
        [(NSString *)stringToDraw drawAtPoint:CGPointZero withFont:selectedFont];
        UIGraphicsPopContext();

        CGContextSaveGState(drawingContext);

		float radAngle = angle / 180 * M_PI;
        float centerPosX = point.getX() - (sz.width / 2 * cos(radAngle) +
                                           sz.height / 2 * sin(radAngle));
        float centerPosY = point.getY() + (sz.width / 2 * sin(radAngle) -
                                           sz.height / 2 * cos(radAngle));
        /* Convert the coordinates to integers so that text with 0 angle gets
         * displayed without interpolation. */
        CGContextTranslateCTM(drawingContext, (int)centerPosX, (int)centerPosY);
        CGContextRotateCTM(drawingContext, -radAngle);
        CGContextDrawLayerAtPoint(drawingContext, CGPointZero, textLayer);

        CGContextRestoreGState(drawingContext);

        CGLayerRelease(textLayer);
        CFRelease(stringToDraw);
	}
}


Rectangle IPhoneMapRenderer::getStringAsRectangle(const STRING& text,
												  const MC2Point& point,
												  int startIdx,
												  int nbrChars,
												  float angle)
{
    if (nbrChars == -1)
        nbrChars = CFStringGetLength(text); 

    CFStringRef stringToDraw =
        CFStringCreateWithSubstring(NULL, text,
                                    CFRangeMake(startIdx, nbrChars));
    if (stringToDraw) {
        CGSize sz = [(NSString *)stringToDraw sizeWithFont:selectedFont];
        CFRelease(stringToDraw);

        return Rectangle(point.getX() - sz.width / 2, point.getY() - sz.height / 2,
                            sz.width, sz.height);
    }
	return Rectangle(0, 0, 0, 0);
}


/**
 *   Puts the bounding boxes of each character in str
 *   into the vector boxes.
 *   <br />
 *   Default implementation does nothing and returns 0.
 *   @param boxes  Vector to add the boxes to.
 *   @see drawText
 *   @return Number of characters in str.
 */
int IPhoneMapRenderer::getStringAsRectangles(std::vector<Rectangle>& boxes,
											 const STRING& text,
											 const MC2Point& point,
											 int startIdx,
											 int nbrChars,
											 float angle )
{
    if (!drawingContext)
        return 0;

    //when nbrChars is -1, means that they are interested in the whole string,
    //otherwise, just in a few (nbrChars) characters 
    if (nbrChars == -1) {
        nbrChars = CFStringGetLength(text); 
    }
    assert(nbrChars != 0);

    CFStringRef stringToDraw =
        CFStringCreateWithSubstring(NULL, text,
                                    CFRangeMake(startIdx, nbrChars));

    /* Calculate the top-left corner of the string (when it is rotated with
     * angle) */
    CGSize sz = [(NSString *)stringToDraw sizeWithFont:selectedFont];
    angle = M_PI * angle / 180;

    float cos_a = cos(angle);
    float sin_a = sin(angle);

    float x = point.getX() - sz.width / 2 * cos_a - sz.height / 2 * sin_a;
    float y = point.getY() + sz.width / 2 * sin_a + sz.height / 2 * cos_a;

    int drawingLength = CFStringGetLength(stringToDraw);
    for (int i = 0; i < drawingLength; ++i) {
        CGSize s = [[(NSString *)stringToDraw
            substringWithRange:NSMakeRange(i, 1)] sizeWithFont:selectedFont];

        Rectangle rect(x, y, s.width, s.height);
        boxes.push_back(rect); 

        x += s.width * cos_a;
        y -= s.width * sin_a;
    }

    CFRelease(stringToDraw);

    return drawingLength;
}


/**
 *   Get if the text rendering direction is left to right
 *   or not (arabic, hebrew is right to left) for the
 *   specified text.
 *   Default implementation returns true.
 */
bool IPhoneMapRenderer::textDirectionLeftToRight(const STRING& text) 
{
	return true;
}


// Misc ----------------------------------------------------------

/**
 *   True if the mapplotter looks at the mask of the images
 *   to create the visible boundingbox.
 */
bool IPhoneMapRenderer::returnsVisibleSizeOfBitmaps() const 
{
	return false;
}


/**
 *   Returns true if the drawing is double buffered.
 *   Default implementation returns false.
 *   @return True if the drawing is double buffered.
 */
bool IPhoneMapRenderer::doubleBuffered() const
{
	return false;
}


/**
 *   Tells the IPhoneMapRenderer that it is ok to change buffer
 *   if it is doubleBuffered. Default implementation does
 *   nothing.
 *   @return doubleBuffered().
 */
bool IPhoneMapRenderer::drawBuffer(const Rectangle& rect)
{
	return 0;
}


/**
 *   If the IPhoneMapRenderer needs to prepare the plotting in
 *   any way it should do it in this method which is called
 *   before drawing.
 *   <br />
 *   The default implementation does not do anything.
 */
void IPhoneMapRenderer::prepareDrawing()
{
}	


/**
 *   Must be called by the drawing engine everytime a drawing
 *   is completed. This is to be able to free up the resources
 *   that may have been used during the draw.
 *   <br />
 *   The default implementation does not do anything.
 */
void IPhoneMapRenderer::triggerApplicationRedraw()
{
	if (updater) {
		updater->callUpdate();
	}
}


/**
 *   Converts the bytes into a bitmap that can be drawn by the
 *   IPhoneMapRenderer.
 *   Default implementation returns NULL.
 */
void releaseData(void *info, const void *data, size_t size)
{
    free((void*)data); 
}



BitMap* IPhoneMapRenderer::createBitMap(bitMapType type,
										const uint8* bytes,
										int nbrBytes,
										int dpiCorrectionFactor)
{
	IPhoneBitMap *newBitmap = new IPhoneBitMap(); 

	NSData *imgData = [NSData dataWithBytes:bytes length:nbrBytes];
	newBitmap->image = [UIImage imageWithData:imgData];
	[newBitmap->image retain];
		
	return newBitmap;
}


/**
 *   Draws the bitmap at x and y.
 *   Default implementation draws a small rectangle.
 */
void IPhoneMapRenderer::drawBitMap(const MC2Point& center,
								   const BitMap* bitMap)
{
	if (bitMap && drawingContext) {
        finishLineDrawing();
    
        UIGraphicsPushContext(drawingContext);
        UIImage *img = ((IPhoneBitMap*)bitMap)->image;
        CGPoint pos = CGPointMake(center.getX() - img.size.width/2,
                                  center.getY() - img.size.height/2);
        [img drawAtPoint:pos];
        UIGraphicsPopContext();
	}
}


/**
 *   Gets the size of the specified BitMap in pixels
 *   Has to be implemented in derived class.
 *   @param size Rectangle which the bitmap is inside.
 *   @param bmp  Bitmap to get the size from.
 *   @param centerXY The center of the bitmap (position where it is drawn).
 */
void IPhoneMapRenderer::getBitMapAsRectangle(Rectangle& size,
											 const MC2Point& origCenter,
											 const BitMap* bmp)
{
	size = Rectangle(origCenter.getX(), origCenter.getY(), 
                        ((IPhoneBitMap*)bmp)->image.size.width, 
                        ((IPhoneBitMap*)bmp)->image.size.height);
}


/**
 *   Gets the size of the specified BitMap in pixels
 *   The other getBitMapAsRectangle has to be implemented in the subclasses.
 *   @param size Rectangle which the bitmap is inside.
 *   @param bmp  Bitmap to get the size from.
 *   @param centerXY The center of the bitmap (position where it is drawn).
 */ 
Rectangle IPhoneMapRenderer::getBitMapAsRectangle(const MC2Point& origCenter,
												  const BitMap* bmp) 
{
	Rectangle rect;
	getBitMapAsRectangle(rect, origCenter, bmp);
	
	return rect;
}


/**
 *   Deletes the supplied bitmap.
 */
void IPhoneMapRenderer::deleteBitMap(BitMap* bitMap)
{
	if (bitMap) {
		[((IPhoneBitMap*)bitMap)->image release];
		
		delete bitMap;
		bitMap = NULL;
	}
}


/**
 *   Sets the fill color for polygons.
 *   Default implementation sets the pen color.
 *   @see setPenColor.
 */
void IPhoneMapRenderer::setFillColor(unsigned int red,
									 unsigned int green,
									 unsigned int blue)
{
    setBackgroundColor(red, green, blue);
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
void IPhoneMapRenderer::drawPolygon(std::vector<MC2Point>::const_iterator begin,
									std::vector<MC2Point>::const_iterator end)
{
	if (drawingContext != nil) {
        if (isPolygonDrawingFinished) {
            CGContextBeginPath(drawingContext);
            isPolygonDrawingFinished = false;
        }

        CGContextMoveToPoint(drawingContext, (*begin).getX(), (*begin).getY());
        while (++begin != end)
            CGContextAddLineToPoint(drawingContext, (*begin).getX(),
                                    (*begin).getY());
	}		
}


/**
 *   Draws a filled polygon defined by the iterators to the screen coords
 *   and the supplied color and line width. Override this to
 *   reduce the number of virtual functions called by the
 *   TileMapHandler.
 *   @param begin     First point draw.
 *   @param end       Point after last point to draw.
 *   @param fillcolor Color to fill with in rrggbb format.
 */
void IPhoneMapRenderer::drawPolygonWithColor(std::vector<MC2Point>::const_iterator begin,
											 std::vector<MC2Point>::const_iterator end,
											 uint32 fillcolor)
{
	if (drawingContext != nil) {
       if(prevPolygonColor != fillcolor) {
            finishPolygonDrawing();
            prevPolygonColor = fillcolor;
            
            CGFloat red = ((fillcolor >> 16) & 0xff) / 255.0;
            CGFloat green = ((fillcolor >> 8) & 0xff) / 255.0;
            CGFloat blue = (fillcolor & 0xff) / 255.0;
            CGContextSetRGBFillColor(drawingContext, red, green, blue, 1.0);
        }

		drawPolygon(begin, end);
	}
}


/**
 *   Draws a polyline defined by the array of points.
 */
void IPhoneMapRenderer::drawPolyLine(std::vector<MC2Point>::const_iterator begin,
								     std::vector<MC2Point>::const_iterator end)
{
	if (drawingContext != nil) {
        if (isLineDrawingFinished) {
            CGContextBeginPath(drawingContext);
            isLineDrawingFinished = false;
        }
		
        CGContextMoveToPoint(drawingContext, (*begin).getX(), (*begin).getY());
        while (++begin != end)
            CGContextAddLineToPoint(drawingContext, (*begin).getX(),
                                    (*begin).getY());
	}
}


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
void IPhoneMapRenderer::drawPolyLineWithColor(std::vector<MC2Point>::const_iterator begin,
											  std::vector<MC2Point>::const_iterator end,
											  uint32 color,
											  int lineWidth)
{
	if (drawingContext) {
        finishPolygonDrawing();
        
        if(prevLineColor != color) {
            finishLineDrawing();
            prevLineColor = color;
            
            CGFloat red = ((color >> 16) & 0xff) / 255.0;
            CGFloat green = ((color >> 8) & 0xff) / 255.0;
            CGFloat blue = (color & 0xff) / 255.0;
            CGContextSetRGBStrokeColor(drawingContext, red, green, blue, 1.0);
        }

   		if(prevLineWidth != lineWidth) {
            finishLineDrawing();
            prevLineWidth = lineWidth;
            
            CGContextSetLineWidth(drawingContext, (CGFloat)lineWidth);
        }
        
		drawPolyLine(begin, end);
	}
}


/**
 *   Draws a spline of some sort.
 *   <br />
 *   Default implementation draws a polyline if not filled else polygon.
 */
void IPhoneMapRenderer::drawSpline(bool filled,
								   std::vector<MC2Point>::const_iterator begin,
								   std::vector<MC2Point>::const_iterator end)
{
}


/**
 *   Draws an arc.
 *   Default implementation draws a line from one corner to the other
 *   in the rectangle.
 */
void IPhoneMapRenderer::drawArc(bool filled, const Rectangle& rect, int startAngle, int stopAngle)
{
	if (drawingContext != nil) {
		
		//radius - is this ok?
		CGFloat radius = (rect.getWidth() > rect.getHeight()) ? ((CGFloat)rect.getHeight()/2) : ((CGFloat)rect.getWidth()/2); 

		CGContextBeginPath(drawingContext);
		CGContextAddArc(drawingContext, (CGFloat) rect.getX() + rect.getWidth()/2 , (CGFloat) rect.getY() + rect.getHeight()/2, 
						radius, (CGFloat)startAngle, (CGFloat) stopAngle, 1); //clockwise
		CGContextClosePath(drawingContext);
		
		if (filled) {
			CGContextFillPath(drawingContext);
		}
		else {
			CGContextStrokePath(drawingContext);
		}
	}
}


/**
 *   Draws a rectangle.
 *   @param filled     If true the rectangle is filled.
 *   @param rect       Dimensions of the rectangle.
 *   @param cornerSize Radius of corners.
 *   Default implementation cannot draw rounded corners and uses
 *   drawPolyLine or drawConvexPolygon.
 */
void IPhoneMapRenderer::drawRect(bool filled, const Rectangle& rect, int cornerSize)
{
	if (drawingContext != nil) {
		CGRect rectangle = CGRectMake(rect.getX(), rect.getY(), rect.getWidth(), rect.getHeight());
		if (filled) {
			CGContextFillRect(drawingContext, rectangle);
		}
		else {
			CGContextStrokeRect(drawingContext, rectangle);		
		}
	}
}


/**
 *   Sets the width of the lines.
 *   Default implementation does nothing.
 */
void IPhoneMapRenderer::setLineWidth(int width)
{
	if (drawingContext != nil) {
        finishLineDrawing();
		prevLineWidth = width;
        
		CGContextSetLineWidth(drawingContext, (CGFloat)width);
	}
}


/**
 *   Sets the cap style of the pen.
 *   Default implementation does nothing.
 */
void IPhoneMapRenderer::setCapStyle(capStyle penCapStyle)
{
	if (drawingContext != nil) {

		CGLineCap newStyle;
		
		switch(penCapStyle) 
		{
			case squareCap:
					newStyle = kCGLineCapSquare;
				break;
		
			case roundCap:
					newStyle = kCGLineCapRound;
				break;
			
			case flatCap:
			default:
					newStyle = kCGLineCapButt;
				break;
		}

		CGContextSetLineCap(drawingContext, newStyle);
	}
}


/**
 *   Sets the dash style of the pen.
 *   Default implementation does nothing.
 */
void IPhoneMapRenderer::setDashStyle(dashStyle penDashStyle)
{
	if (drawingContext != nil) {

		CGFloat lengths[4] = {0};  
		size_t count = 0;
		CGFloat phase = 0;
								
		switch (penDashStyle) 
		{
			case nullDash:
					lengths[0] = 0;
					lengths[1] = 5;  
					count = 2;
					phase = 0;
				break;
			
			case dottedDash:
					lengths[0] = 5;
					lengths[1] = 0;  
					count = 2;
					phase = 0;
				break;
			
			case dashedDash:
					lengths[0] = 3;
					lengths[1] = 3;  
					count = 2;
					phase = 0;
				break;
			
			case solidDash:
			default:
					lengths[0] = 1;
					lengths[1] = 3;  
					lengths[2] = 3;
					lengths[3] = 3;  
					count = 4;
					phase = 0;
				break;
		}

		CGContextSetLineDash(drawingContext, phase, lengths, count);
	}	
}


/**
 *   If the plotter can handle the screen as a bitmap snapshot, i.e.
 *   if makeScreenSnapshot(), moveScreenSnapshot() and
 *   zoomScreenSnapshot() are properly implemented.
 */
bool IPhoneMapRenderer::snapshotHandlingImplemented() const
{
#ifdef HANDLE_SCREEN_AS_BITMAP
	return true;
#else
    return false;
#endif
}


/**
 *   Creates a bitmap screen snapshot and stores it.
 *   Default implementation does nothing.
 */
void IPhoneMapRenderer::makeScreenSnapshot()
{
#ifdef HANDLE_SCREEN_AS_BITMAP
	if (snapshotImage != nil) {
		[snapshotImage release];
	}
	if (drawingContext) {

		CGSize screenSize = CGSizeMake(contextWidth, contextHeight);
		UIGraphicsBeginImageContext(screenSize);
		
        CGContextDrawLayerAtPoint(UIGraphicsGetCurrentContext(),
                                  CGPointZero, drawingLayer);
		snapshotImage = UIGraphicsGetImageFromCurrentImageContext();
		[snapshotImage retain];
		UIGraphicsEndImageContext();
	}
#endif
}


/**
 *   Moves the screen snapshot.
 */
void IPhoneMapRenderer::moveScreenSnapshot(const MC2Point& deltaPoint)
{
#ifdef HANDLE_SCREEN_AS_BITMAP
	if (snapshotImage && drawingContext) {
		if (drawingContext == UIGraphicsGetCurrentContext()) {
			[snapshotImage drawInRect: CGRectMake(0+deltaPoint.getX(), 0+deltaPoint.getY(), contextWidth, contextHeight)];
		}
		else {
			//just draw it where needed
			CGContextDrawImage(drawingContext, CGRectMake(0+deltaPoint.getX(), 0+deltaPoint.getY(), contextWidth, contextHeight), snapshotImage.CGImage);
		}
	}
#endif
}


/**
 *   Zooms the screen snapshot. Value larger than one means zoom in.
 */
void IPhoneMapRenderer::zoomScreenSnapshot(double factor, const MC2Point& zoomPoint)
{
#ifdef HANDLE_SCREEN_AS_BITMAP
	if (snapshotImage && drawingContext) {
		CGRect newRect = CGRectMake(0, 0, contextWidth*factor, contextHeight*factor);
		CGContextDrawImage(drawingContext, newRect, snapshotImage.CGImage);
	}
#endif
}


/**
 *   Locks the bitmap buffer, needed on at least the symbian
 *   platform when doing operations on the bitmaps data.
 */
void IPhoneMapRenderer::lockBitmapBuffer()
{
}


/**
 *   Unlocks the bitmap buffer. Should only be done when the buffer
 *   has been locked.
 */
void IPhoneMapRenderer::unlockBitmapBuffer()
{
}


/**
 *   Returns the data adress of the bitmap.
 */
uint32* IPhoneMapRenderer::getBitmapBuffer()
{
	return 0;
}


/**
 *   Returns the colorDepth of a bitmap.
 */
int IPhoneMapRenderer::getColorDepth()
{
	return MapPlotter::getColorDepth();
}


//to set/get drawing handler
void IPhoneMapRenderer::setCallBack(ReadyForUpdate* updObj)
{
	updater = updObj;
}


ReadyForUpdate *IPhoneMapRenderer::getCallBack()
{
	return updater;
}
				
		
void IPhoneMapRenderer::setDrawingLayer(CGLayerRef layer)  
{
	drawingLayer = layer;
}

		
void IPhoneMapRenderer::finishLineDrawing()
{
    if (!isLineDrawingFinished) {
        CGContextStrokePath(drawingContext);
        isLineDrawingFinished = true;
    }
}


void IPhoneMapRenderer::finishPolygonDrawing()
{
    if (!isPolygonDrawingFinished) {
        CGContextFillPath(drawingContext);
        isPolygonDrawingFinished = true;
    }
}


void IPhoneMapRenderer::enableLowQualityDrawing(bool status)
{
    if (status) {
        CGContextSetLineJoin(drawingContext, kCGLineJoinMiter);  
        CGContextSetLineCap(drawingContext, kCGLineCapButt);
        CGContextSetShouldAntialias(drawingContext, NO);
    }
    else {
        CGContextSetLineJoin(drawingContext, kCGLineJoinRound);  
        CGContextSetLineCap(drawingContext, kCGLineCapRound);
        CGContextSetShouldAntialias(drawingContext, YES);
    }
}

void IPhoneMapRenderer::cropString(STRING& text,
                               unsigned int maxWidth)
{
   //TODO: Implement
}

void IPhoneMapRenderer::drawOverlayText(
   const WFString& text,
   TextCachingBlock& cacheBlock,
   const OverlayItemVisualSpec::TextAlignment alignment,
   FontSpec* font,
   const Rectangle& stringRect,
   const OverlayItemVisualSpec::RGB& textColor)
{
   //TODO: Implement
}

Font* IPhoneMapRenderer::createFont(FontSpec* fontSpec)
{
   //TODO: Implement
   return NULL;
}

void IPhoneMapRenderer::deleteFont(Font* font)
{
   //TODO: Implement
}

void IPhoneMapRenderer::setFontStyle(Font* font)
{
   //TODO: Implement
}

void IPhoneMapRenderer::resetFontStyle()
{
   //TODO: Implement
}

void IPhoneMapRenderer::drawImageSpec(const WFAPI::ImageSpec* imgSpec,
                                  const MC2Point& topLeftPos)
{
   //TODO: Implement
}

   
} //isab
