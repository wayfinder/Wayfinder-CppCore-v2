/*
Copyright (c) 1999 - 2010, Vodafone Group Services Ltd
All rights reserved.

Redistribution and use in source and binary forms, with or without modification, are permitted provided that the following conditions are met:

    * Redistributions of source code must retain the above copyright notice, this list of conditions and the following disclaimer.
    * Redistributions in binary form must reproduce the above copyright notice, this list of conditions and the following disclaimer in the documentation and/or other materials provided with the distribution.
    * Neither the name of the Vodafone Group Services Ltd nor the names of its contributors may be used to endorse or promote products derived from this software without specific prior written permission.

THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS" AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE ARE DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT HOLDER OR CONTRIBUTORS BE LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
*/


#ifndef IPHONENATIVETEXTINTERFACE_H
#define IPHONENATIVETEXTINTERFACE_H

#include "MapPlotter.h"
#include "NativeTextInterface.h"
#include "BitMapTextLoader.h"

#import <UIKit/UIKit.h>
#include <vector>
#include <map>
#import <QuartzCore/QuartzCore.h> //CGContextRef


/**
 *  This class serves as the platform specific text interface for
 *  an ExternalRenderer on the iPhone platform. It uses UIKit fonts
 *  to calculate dimensinos and draw text upon buffers.
 */  
class IPhoneNativeTextInterface : public NativeTextInterface,
                                  public BitMapTextLoader {
public:
   /**
    * Constructs a new text interface.
    */ 
   IPhoneNativeTextInterface();
   
    /**
    *   @see NativeTextInterface::getStringAsRectangles
    */
   virtual int getStringAsRectangles(std::vector<isab::Rectangle>& boxes,
                                     const STRING& text,
                                     const MC2Point& center,
                                     int startIdx,
                                     int nbrChars,
                                     float angle);
   
   /**
    *   @see NativeTextInterface::getStringAsRectangle
    */
   virtual isab::Rectangle getStringAsRectangle(const STRING& text,
                                                const MC2Point& point,
                                                int startIdx =  0,
                                                int nbrChars = -1,
                                                float angle  = 0.0);

   /**
    *   @see NativeTextInterface::setFont
    */
   virtual void setFont(const STRING& fontName, int size);

   /**
    *   @see NativeTextInterface::getStringLength
    */
   virtual int getStringLength(const STRING& text);

   /**
    *   @see NativeTextInterface::createString
    */
   virtual STRING* createString(const char* text);

   /**
    *   @see NativeTextInterface::deleteString
    */

   virtual void deleteString(STRING* text);
   
   /**
    *   @see BitMapLoader::load
    */
   virtual int load(RGBA32BitMap& bitMap);

   /**
    *   Creates a platform specific font using the specification fontSpec.
    */ 
   virtual isab::Font* createFont(WFAPI::FontSpec* fontSpec);

   /**
    *   Deallocates a platform specific font.
    */ 
   virtual void deleteFont(isab::Font* font) ;
private:
   
   /// The UIKit font we are using for drawing
   UIFont *selectedFont;
};

#endif /* IPHONENATIVETEXTINTERFACE_H */



