/*
Copyright (c) 1999 - 2010, Vodafone Group Services Ltd
All rights reserved.

Redistribution and use in source and binary forms, with or without modification, are permitted provided that the following conditions are met:

    * Redistributions of source code must retain the above copyright notice, this list of conditions and the following disclaimer.
    * Redistributions in binary form must reproduce the above copyright notice, this list of conditions and the following disclaimer in the documentation and/or other materials provided with the distribution.
    * Neither the name of the Vodafone Group Services Ltd nor the names of its contributors may be used to endorse or promote products derived from this software without specific prior written permission.

THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS" AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE ARE DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT HOLDER OR CONTRIBUTORS BE LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
*/


#include "S60ImageUtil.h"

void S60ImageUtil::calculateVisibleRect(Image* aImage)
{
   if (aImage->iHasMask) {
      CFbsBitmap* mask = aImage->GetMask();
      TInt xmin = aImage->iWidth;
      TInt xmax = 0;
      TInt ymin = aImage->iHeight;
      TInt ymax = 0;
#if defined USE_AKN_LIB
      //Iterating the pixels with GetPixel() works for sure on all symbian,
      //but it's pretty slow and we have to look at every one pixel.
      //On a 800 x 800 image we do 640 000 GetPixel() iterations.
      TRgb color;
      TRgb white(255, 255, 255);
      for (TInt y = 0; y < aImage->iHeight; y++) {
         for (TInt x = 0; x < aImage->iWidth; x++) {
            mask->GetPixel(color, TPoint(x, y));
            if (color == white) {
               if (xmin > x) {
                  xmin = x;
               }
               if (xmax < x) {
                  xmax = x;
               }
               if (ymin > y) {
                  ymin = y;
               }
               if (ymax < y) {
                  ymax = y;
               }
            }
         }
      }
#else
      //Iterating the pixels by hand in memory probably works on all symbian,
      //it's much faster since we look at 8 pixels at a time, 
      //It needs some testing and verification.
      //On a 800 x 800 image we do roughly 83 000 iterations.
      TDisplayMode dMode = mask->DisplayMode();
      if (dMode == EGray2) {
# ifndef NAV2_CLIENT_SERIES60_V1
         mask->LockHeap();
# endif
         TUint32* imgPtr = mask->DataAddress();
         TSize imgSize = mask->SizeInPixels();
         TInt imgByteWidth = imgSize.iWidth >> 3;
         TInt imgBitsLeft = imgSize.iWidth % 8;
         TInt lineLength = CFbsBitmap::ScanLineLength(imgSize.iWidth, dMode);
         TUint8* pCurrByteLine = (TUint8*) imgPtr;
         TUint8 currByte;
         TInt currXPixelOffset;
         TInt currXPixel;
         for (TInt y = 0; y < imgSize.iHeight; y++) {
            for (TInt x = 0; x < imgByteWidth; x++) {
               currByte = pCurrByteLine[x];
               //If currByte is != 0, it contains at least one white pixel.
               if (currByte) {
                  if (ymin > y) {
                     ymin = y;
                  }
                  if (ymax < y) {
                     ymax = y;
                  }
                  currXPixelOffset = x << 3;
                  //Check if this byte of pixels might contain xmin or xmax.
                  if ((currXPixelOffset < xmin) || 
                      ((currXPixelOffset + 7) > xmax)) {
                     for (TInt b = 0; b < 8; b++) {
                        //Some of the 8 pixels in the byte are visible.
                        //Find which ones that mather for the x-axis.
                        if  (currByte & (1 << b)) {
                           currXPixel = currXPixelOffset + b;
                           if (xmin > currXPixel) {
                              xmin = currXPixel;
                           }
                           if (xmax < currXPixel) {
                              xmax = currXPixel;
                           }
                        }
                     }
                  }
               }
            }
            //Here we take care of bit padded bytes when the
            //image width is not evenly dividable by a byte.
            if (imgBitsLeft != 0) {
               currByte = pCurrByteLine[imgByteWidth];
               currXPixelOffset = imgByteWidth << 3;
               for (TInt b = 0; b < imgBitsLeft; b++) {
                  if  (currByte & (1 << b)) {
                     currXPixel = currXPixelOffset + b;
                     if (xmax < currXPixel) {
                        xmax = currXPixel;
                     }
                  }
               }
            }
            //Move to next line in image.
            pCurrByteLine = pCurrByteLine + lineLength;
         }
      }
# ifndef NAV2_CLIENT_SERIES60_V1
      mask->UnlockHeap();
# endif
#endif
      //Set the image visible rectangle.
      //We need to add +1 to xmax and ymax since the image 
      //pixels go from 0 to image width-1 but we're interested
      //in the the actual rectangular size of the image.
      //Default rect set to 0 size and we only set it if we've
      //detected a visible rectangle in the mask.
      if ((xmin != aImage->iWidth) && (ymin != aImage->iHeight)) {
         aImage->iVisibleRect = TRect(xmin, ymin, 
                                      xmax+1, ymax+1);
      }
   } else {
      aImage->iVisibleRect = TRect(0, 0, 
                                   aImage->iWidth, aImage->iHeight);
   }
}
