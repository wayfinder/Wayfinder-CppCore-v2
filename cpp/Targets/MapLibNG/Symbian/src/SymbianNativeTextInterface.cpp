/*
Copyright (c) 1999 - 2010, Vodafone Group Services Ltd
All rights reserved.

Redistribution and use in source and binary forms, with or without modification, are permitted provided that the following conditions are met:

    * Redistributions of source code must retain the above copyright notice, this list of conditions and the following disclaimer.
    * Redistributions in binary form must reproduce the above copyright notice, this list of conditions and the following disclaimer in the documentation and/or other materials provided with the distribution.
    * Neither the name of the Vodafone Group Services Ltd nor the names of its contributors may be used to endorse or promote products derived from this software without specific prior written permission.

THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS" AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE ARE DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT HOLDER OR CONTRIBUTORS BE LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
*/


#include "SymbianNativeTextInterface.h"
#include <e32std.h>
#include <fbs.h>
#include "sincos.h"
#include "Surface.h"
#include "MapUtility.h"
#include <biditext.h>
#include <eikenv.h>
#include <gdi.h>
#include <e32math.h>
#include <coecntrl.h>

_LIT(KWfAntiAlias, "_wfantialias");

SymbianNativeTextInterface::SymbianNativeTextInterface( CCoeControl& aParent,
                                                        TInt aColorDepth )
   : BitMapTextLoader(),
     iColorDepth( aColorDepth ),
     iParent( aParent )
{
   iFontQueryBuffer = CSurface::NewL(iParent.ControlEnv()->WsSession(),
                                     64, 64, aColorDepth);
   iCurFont = NULL;
}

int SymbianNativeTextInterface::getStringAsRectangles(
   std::vector<isab::Rectangle>& boxes,
   const STRING& text,
   const MC2Point& center,
   int startIdx,
   int nbrChars,
   float angle )
{
   /* return quietly if no font is set */
   if(iCurFont == NULL) return(0);
   
   int i;
   
   /* Loop thru the text and set the bounding rectangle for each character 
      Each rectangle holds absolute coords for the begininning of 
      each character and their width and height */

   if(nbrChars == -1) {
      nbrChars = text.Length();
   }

   int textH = iCurFont->HeightInPixels();
   int halfTextH = textH >> 1;

   /* number of characters to be printed */
   int numChars = nbrChars - startIdx;
   int maxTextLen = iCurFont->TextWidthInPixels(text);
   int halfTextLen = maxTextLen >> 1;

   if(angle == 0.0) {
      /* get the center points of all the characters */
      RArray<MC2Point> origArray;

      int startX = center.getX() - halfTextLen;
      int textW;

      for(i = 0; i < numChars; i++) {
         textW = iCurFont->CharWidthInPixels(text[startIdx+i]);
         origArray.Append(MC2Point(startX + (textW >> 1),
                                   center.getY()));
         startX += textW;
      }

      /* get the rectangles */
      int curOrig = 0;
      for(i = startIdx; i < nbrChars; i++) {

         textW = iCurFont->CharWidthInPixels(text[startIdx+i]);
         boxes.push_back(isab::Rectangle(origArray[curOrig].getX()-(textW>>1),
                                         origArray[curOrig].getY()-halfTextH,
                                         textW, textH));
         curOrig++;

      }
      origArray.Reset();
   }
   else {
      /* get the center points of all the characters */
      RArray<MC2Point> origArray;

      int startX = center.getX() - halfTextLen;
      int textW;

      for(i = 0; i < numChars; i++) {
         textW = iCurFont->CharWidthInPixels(text[startIdx+i]);

         /* get the relative coord for the center */
         float origX = (startX + (textW >> 1)) - center.getX();
         float origY = 0;

         /* get the rotated center */
         float rotOrigX, rotOrigY;
         rotatePoint(origX, origY, angle, &rotOrigX, &rotOrigY);
         rotOrigX += center.getX();
         rotOrigY += center.getY();

         origArray.Append(MC2Point((int)rotOrigX,
                                   (int)rotOrigY));
         startX += textW;
      }

      /* get the rectangles */
      int curOrig = 0;
      for(i = startIdx; i < nbrChars; i++) {
         textW = iCurFont->CharWidthInPixels(text[startIdx+i]);

         isab::Rectangle rotBox = getRotatedBBox(MC2Point(origArray[curOrig].getX(),
                                           origArray[curOrig].getY()),
                                           textW, textH, angle);

         boxes.push_back(rotBox);
         curOrig++;
      }
      origArray.Reset();
   }

   /* returns the number of characters processed */
   return(numChars); 
}

void SymbianNativeTextInterface::rotatePoint(float inX, float inY, 
                                             float angle, 
                                             float* outX, float* outY)
{
   // get the trig ratios
   register float sine = SIN(angle);
   register float cosine = COS(angle);

   // calculate the points
   *outX = inY * sine   + inX * cosine;
   *outY = inY * cosine - inX * sine;

   return; 
}

isab::Rectangle
SymbianNativeTextInterface::getRotatedBBox( const MC2Point& center, 
                                            int width, int height,
                                            float angle)
{
   // get the corner vertices
   int halfW = width >> 1;
   int halfH = height >> 1;
   float x1 = -halfW; float y1 = -halfH; 
   float x2 = halfW; float y2 = -halfH; 
   float x3 = halfW; float y3 = halfH; 
   float x4 = -halfW; float y4 = halfH; 

   // rotate the points
   float newx[4], newy[4];
   rotatePoint(x1, y1, angle, &newx[0], &newy[0]);
   rotatePoint(x2, y2, angle, &newx[1], &newy[1]);
   rotatePoint(x3, y3, angle, &newx[2], &newy[2]);
   rotatePoint(x4, y4, angle, &newx[3], &newy[3]);   

   // shift the rotated coords to the center
   for(int i = 0; i < 4; i++) {
      newx[i] += center.getX();
      newy[i] += center.getY();
   }

   // get the max and min of the coords
   float xMax = MAX(MAX(newx[0],newx[1]),MAX(newx[2],newx[3]));
   float yMax = MAX(MAX(newy[0],newy[1]),MAX(newy[2],newy[3]));

   float xMin = MIN(MIN(newx[0],newx[1]),MIN(newx[2],newx[3]));
   float yMin = MIN(MIN(newy[0],newy[1]),MIN(newy[2],newy[3]));

   // calculate the resulting rectangle 
   int finalWidth  = (int)(xMax-xMin);
   int finalHeight = (int)(yMax-yMin);
   int topleftX = (int)xMin;
   int topleftY = (int)yMin;

   return(isab::Rectangle(topleftX, topleftY, finalWidth, finalHeight));
}

isab::Rectangle
SymbianNativeTextInterface::getStringAsRectangle( const STRING& text,
                                                  const MC2Point& point,
                                                  int startIdx,
                                                  int nbrChars,
                                                  float angle )
{
   /* return quietly with a null rectangle, if no font is set */
   if( iCurFont == NULL ) return( isab::Rectangle(0,0,0,0) );
   /* get the width and height of the string */
   if(nbrChars == -1) {
      nbrChars = text.Length();
   }
   int textWidth = iCurFont->TextWidthInPixels(text.Mid(startIdx, nbrChars));
   int halfWidth = textWidth >> 1;
   int textHeight = iCurFont->HeightInPixels();
   int halfHeight = textHeight >> 1;

   /* get the basic text bounding box */
   isab::Rectangle baseRect(point.getX() - halfWidth, 
                            point.getY() - halfHeight,
                            textWidth, textHeight);

   if(angle == 0.0) {
      // zero angle
      return(baseRect);
   }
   else {
      /* return the rotated bounding box */
      return(getRotatedBBox(point, baseRect.getWidth(), 
                            baseRect.getHeight(), angle));
   }
}


void SymbianNativeTextInterface::setFont( const STRING& fontName, int size )
{

   FontNotice notice( (STRING*)&fontName, size );
   fontCache_t::iterator findit = m_fontCache.find( notice );
   if ( findit != m_fontCache.end() ) {
      // Found the font.
      iCurFont = (*findit).second;
      /* set the specified font as the default font */
   } else {
      // Font not found.
      // Create the font.
      TFontSpec fontspec;
      TTypeface fonttype;
      TFontStyle fontstyle;
      TInt pos = fontName.Find(KWfAntiAlias);
      // See if we want to draw it with anti aliasing.
      if (pos != KErrNotFound) {
         // Found wfantialias in name so set the fontstyle to anti alias.
         // The name of the font to load has to be stripped from wfantialias.
         fonttype.iName = fontName.Left(pos);
         fontstyle.SetBitmapType(EAntiAliasedGlyphBitmap);
      } else {
         fonttype.iName = fontName;
      }
      fontspec.iTypeface = fonttype;
      fontspec.iHeight = size;
      fontspec.iFontStyle = fontstyle;
      CFbsFont* theFont = NULL;
      iFontQueryBuffer->Device().GetNearestFontInPixels(theFont,
                                                        fontspec);
      MC2_ASSERT( theFont != NULL );

      // Add the font to the font cache.
      FontNotice noticeToStore( fontName.Alloc(), size );
      m_fontCache[ noticeToStore ] = theFont;
      iCurFont = theFont;
      /* set the specified font as the default font */
   }
}

int SymbianNativeTextInterface::getStringLength( const STRING& text )
{
   return text.Length(); 
}

STRING* SymbianNativeTextInterface::createString( const char* text )
{
   if(text == NULL){
      return(NULL);
   }
   /* convert the UTF-8 string to Unicode */
   TBuf<256> uniBuf;
   const int length = strlen(text);
   MapUtility::ConvertUTF8ToUnicode((const unsigned char*)text,
                                    uniBuf, length);
   /* allocate a heap descriptor from the buffer and return it */
   return((STRING*)uniBuf.Alloc()); 
}

void SymbianNativeTextInterface::deleteString( STRING* text )
{
   delete (static_cast<HBufC16*>(text));
   text = NULL;
   return; 
}

int SymbianNativeTextInterface::load( RGBA32BitMap& bitMap )
{
   
   /* if no font is specified, exit quietly */
   if(iCurFont == NULL) return 1;
   
   int nbrChars = m_endIndex - m_startIndex;

   MC2Point point( 0, 0 );
   
   /* if nbrChars > max string length, the set it to the same */
   if(nbrChars > m_string->Length() || nbrChars <= 0) {
      nbrChars = m_string->Length();
   }
   
   // Get a pointer to the text to draw.
   TPtrC textToDraw = m_string->Mid(m_startIndex, nbrChars);
   
   /* get basic info */
   int textWidth = iCurFont->TextWidthInPixels(textToDraw);
   int textHeight = iCurFont->HeightInPixels()+iCurFont->DescentInPixels();
   TRgb penColor = TRgb( m_r, m_g, m_b );

   CSurface* drawBuffer = CSurface::NewL(iParent.ControlEnv()->WsSession(),
                                textWidth, textHeight, iColorDepth);

   drawBuffer->Gc().UseFont( iCurFont );
   drawBuffer->Gc().SetPenColor( penColor );
   
   //MC2Point adjPoint(point.getX() - (textWidth >> 1),
   //                  point.getY() + (textHeight >> 1));
   isab::Rectangle rect ( getStringAsRectangle( *m_string, point,
                                                m_startIndex, nbrChars,
                                                0.0f ) );
   MC2Point adjPoint( 0,
                      0 + rect.getHeight() );

   CGraphicsContext::TDrawTextExtendedParam txtParam;
   if ( ! textDirectionLeftToRight( *m_string ) ) {
      txtParam.iParRightToLeft = ETrue;
   }
   
   drawBuffer->Gc().Clear();
   
   /* write the text to the bitmap */
   drawBuffer->Gc().DrawTextExtended(*m_string, 
                                      adjPoint, 
                                      txtParam);
   //drawBuffer->Gc().DrawText(text, adjPoint);
   
   bitMap.imgWidth = bitMap.memWidth =
      drawBuffer->Bitmap().SizeInPixels().iWidth;
   bitMap.imgHeight = bitMap.memHeight =
      drawBuffer->Bitmap().SizeInPixels().iHeight;
   for ( unsigned int y = 0; y < bitMap.imgHeight; y++ ) {
      for ( unsigned int x = 0; x < bitMap.imgWidth; x++ ) {
         unsigned int col = drawBuffer->ReadPixel( x, y ).Internal();
         if( col == 0xFFFFFFFF ) {
            bitMap.data.push_back( 0 );
         } else {
            bitMap.data.push_back( col );
         }
      }
   }

   delete drawBuffer;
   
   return 0;  
}

bool SymbianNativeTextInterface::textDirectionLeftToRight( const STRING& text )
{
   TBidiText *bidiText = TBidiText::NewL(text, 1);
   TBidiText::TDirectionality txtDir = bidiText->Directionality();
   delete bidiText;
   return (txtDir == TBidiText::ELeftToRight); 
}

isab::Font* SymbianNativeTextInterface::createFont(WFAPI::FontSpec* fontSpec)
{
   // TODO: Implement
   return NULL;   
}

void SymbianNativeTextInterface::deleteFont(isab::Font* font) 
{
   // TODO: Implement
}

