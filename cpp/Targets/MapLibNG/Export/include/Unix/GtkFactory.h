/*
  Copyright (c) 1999 - 2010, Vodafone Group Services Ltd
  All rights reserved.

  Redistribution and use in source and binary forms, with or without modification, are permitted provided that the following conditions are met:

  * Redistributions of source code must retain the above copyright notice, this list of conditions and the following disclaimer.
  * Redistributions in binary form must reproduce the above copyright notice, this list of conditions and the following disclaimer in the documentation and/or other materials provided with the distribution.
  * Neither the name of the Vodafone Group Services Ltd nor the names of its contributors may be used to endorse or promote products derived from this software without specific prior written permission.

  THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS" AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE ARE DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT HOLDER OR CONTRIBUTORS BE LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
*/

#ifndef WFAPI_GTKFACTORY_H
#define WFAPI_GTKFACTORY_H

#include <gtk/gtk.h>

namespace WFAPI {

// Forward declarations

class DBufConnection;
class DrawingContext;
class MapLibNetworkContext;
class GtkDrawer;
class GtkGLESDrawer;
class MapLibAPI;
class MapLibInitialConfig;
class ImageSpec;

/**
 * Collection of factory methods which are used to 
 * construct the Gtk-specific classes needed to
 * initialize MapLib.
 */
class GtkFactory {
public:
   /**
    * Creates and initializes a HTTP context which can be
    * used by MapLib to download maps. Note that by calling
    * this function you assume ownership of the resource and
    * it must be kept alive for the duration of the MapLib
    * session.
    *
    * @return The created MapLibNetworkContext.
    */
   static MapLibNetworkContext* createHTTPContext();

   /**
    * Creates a GTK based drawer using the supplied window. Note
    * that by calling this function you assume ownership of the
    * resource and it must be kept alive for the duration of the
    * MapLib session.
    *
    * @param widget The widget that the drawer should draw upon. 
    * @return The created GtkDrawer.
    */ 
   static GtkDrawer* createGtkDrawer(GtkWidget* widget);
   
   /**
    * Creates an image object which can be used as backgrounds or
    * icons for OverlayItems. Will increase the internal GTK reference
    * count of the gdkPixBuf. It is therefore safe to remove a reference
    * to the original pixbuf once allocated.
    *
    * Initially the reference count of the ImageSpec resource is set to 1,
    * so to properly deallocate it you will need to call removeReference()
    * once you are done with the ImageSpec resouce.
    *
    * @param gdkPixBuf The GdkPixBuf to use as source for the image.
    */ 
   
   static ImageSpec* createGdkImageSpec(GdkPixbuf* gdkPixBuf);
   
   /**
    * Creates an instance of MapLibAPI using the supplied connection,
    * drawingContext and initial config. 
    * The DBufConnection to supply can either be retrieved from Nav2API
    * or from GtkFactory::createHTTPContext().
    *
    * @param connection The connection that MapLib will use.
    * @param drawingContext The drawing context that MapLib will use.
    * @param mapLibConfig The initial configuration for MapLib.
    * @return A pointer to the MapLibAPI instance.
    */ 
   static MapLibAPI* createMapLib(DBufConnection* connection,
                                  DrawingContext* drawingContext,
                                  const MapLibInitialConfig& mapLibConfig);

};
  
} // End of namespace WFAPI

#endif // WFAPI_GTKFACTORY_H
