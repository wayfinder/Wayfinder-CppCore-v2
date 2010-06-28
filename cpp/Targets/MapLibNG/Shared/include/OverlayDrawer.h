/*
Copyright (c) 1999 - 2010, Vodafone Group Services Ltd
All rights reserved.

Redistribution and use in source and binary forms, with or without modification, are permitted provided that the following conditions are met:

    * Redistributions of source code must retain the above copyright notice, this list of conditions and the following disclaimer.
    * Redistributions in binary form must reproduce the above copyright notice, this list of conditions and the following disclaimer in the documentation and/or other materials provided with the distribution.
    * Neither the name of the Vodafone Group Services Ltd nor the names of its contributors may be used to endorse or promote products derived from this software without specific prior written permission.

THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS" AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE ARE DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT HOLDER OR CONTRIBUTORS BE LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
*/


#ifndef OVERLAYDRAWER_H
#define OVERLAYDRAWER_H

#include "MapPlotterTypes.h"
#include "OverlayItemVisualSpec.h"

namespace WFAPI {
class ImageSpec;
class WFString;
class FontSpec;
}

namespace isab {
class Font;
}

class MC2Point;
class TextCachingBlock;

class OverlayDrawer {
public:
   /**
    * Draws an image specification for overlay.
    * 
    */ 
   virtual void drawImageSpec(const WFAPI::ImageSpec* imgSpec,
                              const MC2Point& topLeftPos) = 0;

   /**
    * Crops the string, modifies the string to fit within the
    * maxWidth with the last three chars as ...
    *
    * @param text The text to crop.
    * @param maxWidth the maximal width in pixels.
    */
   virtual void cropString(STRING& text,
                           unsigned int maxWidth) = 0;

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
      const WFAPI::OverlayItemVisualSpec::RGB& textColor) = 0;

   /**
    * Sets the clipping region of the drawer, which is the
    * region that stuff can be drawn to. Pixels outside this
    * region will not be affected. Default implementation
    * does nothing. If implementation supports clipping,
    * supportsClipping() will return true.
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
   virtual bool supportsClipping() const { return false; }

};

#endif // OVERLAYDRAWER_H
