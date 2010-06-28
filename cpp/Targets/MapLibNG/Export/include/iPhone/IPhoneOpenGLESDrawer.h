/*
Copyright (c) 1999 - 2010, Vodafone Group Services Ltd
All rights reserved.

Redistribution and use in source and binary forms, with or without modification, are permitted provided that the following conditions are met:

    * Redistributions of source code must retain the above copyright notice, this list of conditions and the following disclaimer.
    * Redistributions in binary form must reproduce the above copyright notice, this list of conditions and the following disclaimer in the documentation and/or other materials provided with the distribution.
    * Neither the name of the Vodafone Group Services Ltd nor the names of its contributors may be used to endorse or promote products derived from this software without specific prior written permission.

THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS" AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE ARE DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT HOLDER OR CONTRIBUTORS BE LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
*/


#ifndef IPHONEOPENGLESDRAWER_H
#define IPHONEOPENGLESDRAWER_H

#import <UIKit/UIKit.h>
#import <OpenGLES/EAGL.h>
#import <OpenGLES/ES1/gl.h>
#import <OpenGLES/ES1/glext.h>

namespace WFAPI {

class DrawingContext;

/**
 * This class encapsulates an OpenGL ES drawer. It can supply
 * a DrawingContext for MapLib.
 */ 
class IPhoneOpenGLESDrawer {
public:
   IPhoneOpenGLESDrawer();
   
   IPhoneOpenGLESDrawer(UIView* view,
                        GLuint viewRenderBuffer,
                        GLuint viewFrameBuffer,
                        EAGLContext* eaglContext);

   /**
    * Sets the internal frame buffers that the drawer uses.
    */ 
   void updateContext(GLuint viewRenderBuffer,
                      GLuint viewFrameBuffer,
                      EAGLContext* eaglContext);

   /**
    * Changes the screen size of the map. The bottom left of the map will
    * be fixed, and the top right corner will change according to the new size.
    *
    * @param newWidth The new width for the map, in pixels.
    * @param newHeight The new height for the map, in pixels.
    */
   void setScreenSize(unsigned int newWidth,
                      unsigned int newHeight);

   /**
    * Returns a drawing context which can be used by MapLib
    * to draw the maps.
    */ 
   DrawingContext* getDrawingContext();
private:
   struct Impl;
   
   Impl* m_impl;
};


} // End of namespace WFAPI

#endif /* IPHONEOPENGLESDRAWER_H */
