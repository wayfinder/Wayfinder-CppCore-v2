/*
  Copyright (c) 1999 - 2010, Vodafone Group Services Ltd
  All rights reserved.

  Redistribution and use in source and binary forms, with or without modification, are permitted provided that the following conditions are met:

  * Redistributions of source code must retain the above copyright notice, this list of conditions and the following disclaimer.
  * Redistributions in binary form must reproduce the above copyright notice, this list of conditions and the following disclaimer in the documentation and/or other materials provided with the distribution.
  * Neither the name of the Vodafone Group Services Ltd nor the names of its contributors may be used to endorse or promote products derived from this software without specific prior written permission.

  THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS" AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE ARE DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT HOLDER OR CONTRIBUTORS BE LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
*/

#define CORE_LOGGING_MODULE_NAME "IPhoneNativeTextInterface"
// #define CORE_LOGGING_ENABLE_MODULE

#include "CoreDebugPrint.h"
#include "IPhoneNativeTextInterface.h"
#include "BitMapUtil.h"

using namespace isab;


IPhoneNativeTextInterface::IPhoneNativeTextInterface()
  : selectedFont([UIFont systemFontOfSize:12])
{
    
}

int
IPhoneNativeTextInterface::getStringAsRectangles(
   std::vector<Rectangle>& boxes,
   const STRING& text,
   const MC2Point& point,
   int startIdx,
   int nbrChars,
   float angle )
{
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

Rectangle
IPhoneNativeTextInterface::getStringAsRectangle( const STRING& text,
                                                 const MC2Point& point,
                                                 int startIdx,
                                                 int nbrChars,
                                                 float angle )
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

void
IPhoneNativeTextInterface::setFont( const STRING& fontName, int size )
{
   selectedFont = [UIFont systemFontOfSize:12];
}

int
IPhoneNativeTextInterface::getStringLength( const STRING& text )
{
   return CFStringGetLength(text);
}

STRING*
IPhoneNativeTextInterface::createString( const char* text )
{
   if (text == NULL) {
		return(NULL);
	}
	
	//create a Unicode string from the UTF8 string
	STRING* ptr = new STRING[1];
   *ptr = CFStringCreateWithCString(NULL, text, kCFStringEncodingUTF8);
   
	return ptr;
}

void
IPhoneNativeTextInterface::deleteString( STRING* text )
{
   CFRelease(*text);
   delete text;
}

// This method is based upon the Texture2D class in the CrashLander
// SDK sample from Apple.
int
IPhoneNativeTextInterface::load( RGBA32BitMap& bitMap )
{
   if (!m_string) {
      return 0;
   }

   int widthAdjust = 0;
   int heightAdjust = 0;
   
   CFStringRef wholeString = *m_string;
   CFRange r;
   r.location = m_startIndex;
   r.length = m_endIndex-m_startIndex;
   
   if (m_endIndex == -1) {
      r.location = 0;
      r.length = CFStringGetLength(wholeString); 
   } 

   CFStringRef refString =
      CFStringCreateWithSubstring(NULL, wholeString, r);
   NSString* nsString = (NSString*) refString;

   // Start by calculating the dimension for the given string and
   // current fnt
   CGSize dimensions = [nsString sizeWithFont:selectedFont];

   NSUInteger width, height, i;
   CGContextRef context;
   CGColorSpaceRef colorSpace;

   width = dimensions.width;
   height = dimensions.height;
   
   if(m_hasBackgroundBox) {
      widthAdjust = 4;
      heightAdjust = 2;

      //size enlargement pills, for debugging your length
      // width *= 4;
      // height *= 4;
   }

   width += widthAdjust;
   height += heightAdjust;
   bitMap.imgWidth = width;
   bitMap.imgHeight = height;
   bitMap.memWidth = bitMap.imgWidth;
   bitMap.memHeight = bitMap.imgHeight;
   
   // Make room for the entire buffer
   bitMap.data.resize(width * height);

   if(m_hasBackgroundBox) {
      BitMapUtil::applyRoadsignBackground(bitMap);
   }
   // This is wasteful - text textures do not need full 32 RGBA format. Fix
   // this in optimization pass.
   colorSpace = CGColorSpaceCreateDeviceRGB();

   // Initialize a suitable bitmap context based upon a 32 bit RGBA
   // buffer format that uses the calculated dimensions.
   context = CGBitmapContextCreate(&bitMap.data[0],
                                   width, height,
                                   8, 4*width,
                                   colorSpace,
                                   kCGImageAlphaPremultipliedLast |
                                   kCGBitmapByteOrder32Big);
   
   CGColorSpaceRelease(colorSpace);

   // Setup the color using normalized values
   CGContextSetRGBFillColor(context,
                            m_r / 255.0f, m_g / 255.0f, m_b / 255.0f, 1.0f);

   // Translate (move) the string so that it will be located in the
   // top left corner of the buffer.
   CGContextTranslateCTM(context, 0.0, height);
   
   // NOTE: NSString draws in UIKit referential i.e. renders upside-down
   // compared to CGBitmapContext referential
   CGContextScaleCTM(context, 1.0, -1.0); 
   UIGraphicsPushContext(context);

   // Finally, draw the string unto the buffer.
   [nsString
               drawInRect:CGRectMake(widthAdjust/2, heightAdjust/2, dimensions.width, dimensions.height)
               withFont:selectedFont
               lineBreakMode:UILineBreakModeWordWrap];
   UIGraphicsPopContext();
        
   CGContextRelease(context);
   CFRelease(refString);
   return 0;
}

Font*
IPhoneNativeTextInterface::createFont(WFAPI::FontSpec* fontSpec)
{
   //TODO: Implement this
   return NULL;
}

void
IPhoneNativeTextInterface::deleteFont(Font* font) 
{
   //TODO: Implement this   
}


