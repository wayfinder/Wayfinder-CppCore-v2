/*
  Copyright (c) 1999 - 2010, Vodafone Group Services Ltd
  All rights reserved.

  Redistribution and use in source and binary forms, with or without modification, are permitted provided that the following conditions are met:

  * Redistributions of source code must retain the above copyright notice, this list of conditions and the following disclaimer.
  * Redistributions in binary form must reproduce the above copyright notice, this list of conditions and the following disclaimer in the documentation and/or other materials provided with the distribution.
  * Neither the name of the Vodafone Group Services Ltd nor the names of its contributors may be used to endorse or promote products derived from this software without specific prior written permission.

  THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS" AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE ARE DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT HOLDER OR CONTRIBUTORS BE LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
*/

#ifndef _GTKGLESDRAWER_H_
#define _GTKGLESDRAWER_H_

#include <gtk/gtk.h>
#include "PALMachineTypes.h"

namespace WFAPI {
class DrawingContext;
class ImageSpec;

/**
 *   GTK based OpenGL ES drawer. Assumes that the underlying OpenGL ES
 *   implementation uses X11 as its native windowing system and that GTK
 *   is implemented on top of this. For optimal visuals, disable double
 *   buffering for the GTK window that is used for drawing.
 */ 

class GtkGLESDrawer {
public:
   GtkGLESDrawer( GtkWidget* widget );
   ~GtkGLESDrawer();

   WFAPI::DrawingContext* getDrawingContext();

   bool init();

   void render();

   /**
    *   Creates a drawable image from a byte buffer.
    *
    *   @param image The buffer to base the internal texture upon.
    */
   WFAPI::ImageSpec* createImageSpec(const pal::wf_uint8* rgba8Buffer,
                                     pal::wf_uint32 widthPixels,
                                     pal::wf_uint32 heightPixels);

   /**
    *   Creates an image representation from a block of text.
    */ 
   WFAPI::ImageSpec* createTextSpec(const char* utf8Sring);

private:

   // Prevent copy construction
   GtkGLESDrawer( const GtkGLESDrawer& other );

   // Prevent copy assignment operator
   GtkGLESDrawer& operator=( const GtkGLESDrawer& other );
   
   struct Impl;
   Impl* m_impl;
   
};

} // End of namespace WFAPI

#endif /* _GTKGLESDRAWER_H_ */
