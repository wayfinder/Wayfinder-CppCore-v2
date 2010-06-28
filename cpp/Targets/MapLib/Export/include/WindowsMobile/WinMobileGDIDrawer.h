/*
Copyright (c) 1999 - 2010, Vodafone Group Services Ltd
All rights reserved.

Redistribution and use in source and binary forms, with or without modification, are permitted provided that the following conditions are met:

    * Redistributions of source code must retain the above copyright notice, this list of conditions and the following disclaimer.
    * Redistributions in binary form must reproduce the above copyright notice, this list of conditions and the following disclaimer in the documentation and/or other materials provided with the distribution.
    * Neither the name of the Vodafone Group Services Ltd nor the names of its contributors may be used to endorse or promote products derived from this software without specific prior written permission.

THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS" AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE ARE DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT HOLDER OR CONTRIBUTORS BE LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
*/


#ifndef _WINMOBILEGDIDRAWER_H_
#define _WINMOBILEGDIDRAWER_H_

#include <windows.h>
#include "PALMachineTypes.h"

namespace WFAPI {

class DrawingContext;

/**
 * Represents a WinMobile GDI drawing interface.
 */
class WinMobileGDIDrawer {
public:
   /**
    * Constructor
    *
    * @param hWnd The window to use for drawing
    */ 
   WinMobileGDIDrawer(HWND hWnd);

   ~WinMobileGDIDrawer();
   
   /**
    *   Change the window that the context should draw upon.
    */
   void changeWindow(HWND newWindow);
   
   /**
    *   Returns the drawing context associated with the drawer.
    */
   DrawingContext* getDrawingContext();

   /**
    *   Blits the map unto the specified DC.
    */ 
   void doSwap(HDC destDC);

   /**
    * Use this call in conjunction with the the callback
    * function mapIsReadyToBeDrawn to draw stuff on top
    * of the map before the map is flipped to the screen.
    *
    * @return The backbuffer that will be flipped to the screen.
    */
   HDC getBackBuffer();
   
private:
   void createContext();
   
   /// Private to avoid direct usage.
   WinMobileGDIDrawer( const WinMobileGDIDrawer& );

   /// Private to avoid direct usage.
   const WinMobileGDIDrawer& operator=( const WinMobileGDIDrawer& );
   
   struct Impl;
   
   Impl* m_impl;
};

} // End of namespace WFAPI

#endif /* _WINMOBILEGDIDRAWER_H_ */
