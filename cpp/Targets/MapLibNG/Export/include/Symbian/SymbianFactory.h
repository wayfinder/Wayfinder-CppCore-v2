/*
Copyright (c) 1999 - 2010, Vodafone Group Services Ltd
All rights reserved.

Redistribution and use in source and binary forms, with or without modification, are permitted provided that the following conditions are met:

    * Redistributions of source code must retain the above copyright notice, this list of conditions and the following disclaimer.
    * Redistributions in binary form must reproduce the above copyright notice, this list of conditions and the following disclaimer in the documentation and/or other materials provided with the distribution.
    * Neither the name of the Vodafone Group Services Ltd nor the names of its contributors may be used to endorse or promote products derived from this software without specific prior written permission.

THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS" AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE ARE DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT HOLDER OR CONTRIBUTORS BE LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
*/


#ifndef _SYMBIANFACTORY_H_
#define _SYMBIANFACTORY_H_

class CCoeControl;
class RWindow;
class CFbsBitmap;

namespace WFAPI {

// Forward declarations
class DBufConnection;
class DrawingContext;
class MapLibNetworkContext;
class MapLibInitialConfig;
class MapLibAPI;
class GtkDrawer;
class SymbianNativeDrawer;
class SymbianOpenGLESDrawer;
class ImageSpec;

/**
 *  Collection of factory methods which are used to 
 *  construct the Symbian-specific classes needed to
 *  initialize MapLib.
 */
namespace SymbianFactory {
   /**
    *   Creates and initializes a HTTP context which can be
    *   used by MapLib to download maps. Note that by calling
    *   this function you assume ownership of the resource and
    *   it must be kept alive for the duration of the MapLib
    *   session.
    *   
    */
   MapLibNetworkContext* createHTTPContext();

   /**
    *   Creates a Symbian specific map drawer using the supplied
    *   CCoeControl. Note that by calling this function you assume
    *   ownership of the resource and it must be kept alive for the
    *   duration of the MapLib session.
    */ 
   SymbianNativeDrawer* createNativeDrawer( CCoeControl& parent );

   /**
    *   Creates an OpenGL ES based renderer which works in a symbian
    *   environment. Note that by calling this function you assume
    *   ownership of the resource and it must be kept alive for the
    *   duration of the MapLib session.
    */
   SymbianOpenGLESDrawer* createOpenGLESDrawer( CCoeControl& parent,
                                                RWindow* window );

   /**
    *   Creates an image spec which can be used for overlay visuals.
    *
    *   Note that the internal image handles of image and mask will be
    *   duplicated, so it is safe to delete the image and mask once the
    *   method returns.
    *
    *   Initially the reference count of the ImageSpec resource is set to 1,
    *   so to properly deallocate it you will need to call removeReference()
    *   once you are done with the ImageSpec resouce.
    */
   ImageSpec* createSymbianImageSpec(CFbsBitmap* image,
                                     CFbsBitmap* mask);

   /**
    *   Creates an instance of MapLibAPI using the supplied connection
    *   and drawingContext.
    */ 
   MapLibAPI* createMapLib( DBufConnection* connection,
                            DrawingContext* drawingContext,
                            const MapLibInitialConfig& mapLibConfig);
}

} // End of namespace WFAPI.
#endif /* _SYMBIANFACTORY_H_ */
