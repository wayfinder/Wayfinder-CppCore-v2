/*
Copyright (c) 1999 - 2010, Vodafone Group Services Ltd
All rights reserved.

Redistribution and use in source and binary forms, with or without modification, are permitted provided that the following conditions are met:

    * Redistributions of source code must retain the above copyright notice, this list of conditions and the following disclaimer.
    * Redistributions in binary form must reproduce the above copyright notice, this list of conditions and the following disclaimer in the documentation and/or other materials provided with the distribution.
    * Neither the name of the Vodafone Group Services Ltd nor the names of its contributors may be used to endorse or promote products derived from this software without specific prior written permission.

THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS" AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE ARE DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT HOLDER OR CONTRIBUTORS BE LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
*/


#include "WinMobileFactory.h"
#include "MapLibAPI.h"
#include "WinTileMapToolkit.h"
#include "WinMapLib.h"
#include "WinMobileGDIDrawer.h"
#include "CoreFunctionPrologue.h"
#include "WinMobileOpenGLESDrawer.h"

namespace WFAPI {

namespace WinMobileFactory {

WinMobileGDIDrawer* createGDIDrawer(HWND hWnd)
{
   CORE_FUNCTION_PROLOGUE();
   
   return new WinMobileGDIDrawer(hWnd);
}

MapLibAPI* createMapLib(DBufConnection* connection,
                        DrawingContext* drawingContext,
                        const MapLibInitialConfig& mapLibConfig,
                        HWND hWnd,
                        HINSTANCE appInst)
{
   CORE_FUNCTION_PROLOGUE();   
   
   return new MapLibAPI(new WinMapLib(drawingContext,
                                      connection,
                                      mapLibConfig,
                                      hWnd,
                                      appInst));
                                      
}


WinMobileOpenGLESDrawer* WFAPI::WinMobileFactory::createOpenGlEsDrawer(HWND hWnd)
{
   return new WinMobileOpenGLESDrawer(hWnd);
}

} // End of namespace WinMobileFactory

} // End of namespace WFAPI

