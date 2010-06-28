/*
Copyright (c) 1999 - 2010, Vodafone Group Services Ltd
All rights reserved.

Redistribution and use in source and binary forms, with or without modification, are permitted provided that the following conditions are met:

    * Redistributions of source code must retain the above copyright notice, this list of conditions and the following disclaimer.
    * Redistributions in binary form must reproduce the above copyright notice, this list of conditions and the following disclaimer in the documentation and/or other materials provided with the distribution.
    * Neither the name of the Vodafone Group Services Ltd nor the names of its contributors may be used to endorse or promote products derived from this software without specific prior written permission.

THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS" AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE ARE DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT HOLDER OR CONTRIBUTORS BE LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
*/


#ifndef _WINOPENGLESCONTEXTUTIL_H_
#define _WINOPENGLESCONTEXTUTIL_H_

#include <gles/egl.h>

class DrawingContext;

namespace WinOpenGlEsContextUtil {

   /**
    *   Creates an OpenGL ES based drawing context.
    *
    *   @param   display   See EGL documentation.
    *   @param   surface   See EGL documentation.
    *
    *   @return  The created drawing context.
    */
   
   DrawingContext* createContext( EGLDisplay display,
                                  EGLSurface surface );

   /**
    *   Creates an OpenGL ES based drawing context.
    *
    *   @param   hWnd   The window to draw in.
    *
    *   @return  The created drawing context.
    */

   DrawingContext* createContext( HWND hWnd );

   /**
    *   Change the window that a context should draw upon.
    *
    *   @param   context   The context that should change window.
    *   @param   display   See EGL documentation.
    *   @param   surface   See EGL documentation.
    */
   
   void changeWindow( DrawingContext* context,
                      EGLDisplay display,
                      EGLSurface surface );

   /**
    *   Change the window that the context should draw upon.
    *
    *   @param   context   The context that should change window.
    *   @param   hWnd      The window that the context should draw upon
    *                      from now.
    */
   
   void changeWindow( DrawingContext* context,
                      HWND hWnd );

   /**
    *   Destroys a context previously created by calling createContext.
    *
    *   @param   context   The context to destroy.
    */

   void destroyContext( DrawingContext* context );
   
};

#endif /* _WINOPENGLESCONTEXTUTIL_H_ */
