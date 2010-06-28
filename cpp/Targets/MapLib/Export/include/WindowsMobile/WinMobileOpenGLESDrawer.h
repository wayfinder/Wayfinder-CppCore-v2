/*
Copyright (c) 1999 - 2010, Vodafone Group Services Ltd
All rights reserved.

Redistribution and use in source and binary forms, with or without modification, are permitted provided that the following conditions are met:

    * Redistributions of source code must retain the above copyright notice, this list of conditions and the following disclaimer.
    * Redistributions in binary form must reproduce the above copyright notice, this list of conditions and the following disclaimer in the documentation and/or other materials provided with the distribution.
    * Neither the name of the Vodafone Group Services Ltd nor the names of its contributors may be used to endorse or promote products derived from this software without specific prior written permission.

THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS" AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE ARE DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT HOLDER OR CONTRIBUTORS BE LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
*/


#ifndef WINMOBILEOPENGLESDRAWER_H
#define WINMOBILEOPENGLESDRAWER_H

#include <windows.h>
#include "PALMachineTypes.h"

namespace WFAPI {

class DrawingContext;
class ImageSpec;

/**
 * Represents a WinMobile OpenGLES drawing interface.
 */
class WinMobileOpenGLESDrawer {
public:
   
   /**
    * Constructor
    */
   WinMobileOpenGLESDrawer(HWND parentWindow);

   ~WinMobileOpenGLESDrawer();

   /**
    *   Returns the drawing context associated with the drawer.
    */
   DrawingContext* getDrawingContext();

   /**
    *   Creates a drawable image from a byte buffer.
    *
    *   @param buffer The buffer to base the internal texture upon.
    *   @param widthPixels The width of the texture
    *   @param heightPixels The height of the texture
    */
   WFAPI::ImageSpec* createImageSpec(const pal::wf_uint8* buffer,
                                     pal::wf_uint32 widthPixels,
                                     pal::wf_uint32 heightPixels);

   /**
    *   Flips the buffer to the screen.
    */ 
   void render();
private:
   /// Private to avoid direct usage.
   WinMobileOpenGLESDrawer(const WinMobileOpenGLESDrawer&);

   /// Private to avoid direct usage.
   const WinMobileOpenGLESDrawer& operator=( const WinMobileOpenGLESDrawer&);

   struct Impl;

   Impl* m_impl;
   
};

} // End of namespace WFAPI.


#endif /* WINMOBILEOPENGLESDRAWER_H */
