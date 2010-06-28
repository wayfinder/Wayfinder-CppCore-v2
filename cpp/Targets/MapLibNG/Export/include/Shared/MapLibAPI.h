/*
  Copyright (c) 1999 - 2010, Vodafone Group Services Ltd
  All rights reserved.

  Redistribution and use in source and binary forms, with or without modification, are permitted provided that the following conditions are met:

  * Redistributions of source code must retain the above copyright notice, this list of conditions and the following disclaimer.
  * Redistributions in binary form must reproduce the above copyright notice, this list of conditions and the following disclaimer in the documentation and/or other materials provided with the distribution.
  * Neither the name of the Vodafone Group Services Ltd nor the names of its contributors may be used to endorse or promote products derived from this software without specific prior written permission.

  THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS" AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE ARE DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT HOLDER OR CONTRIBUTORS BE LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
*/

#ifndef WFAPI_MAPLIBAPI_H
#define WFAPI_MAPLIBAPI_H

#include "PALMachineTypes.h"

class MapLib;

namespace WFAPI {

// Forward declarations.
class ConfigInterface;
class DrawingContext;
class DynamicConfig;
class MapDrawingInterface;
class MapLibKeyInterface;
class MapObjectInterface;
class MapLibStartupListener;
class MapOperationInterface;
class OverlayInterface;
class InternalConfig;

/**
 * Main interface class for MapLib API.
 * The interfaces returned by the MapLib API are valid during the lifetime
 * of the MapLib API.
 */
class MapLibAPI {
public:
   /**
    * Constructor.
    */ 
   MapLibAPI( MapLib* maplib );
   
   /**
    * Virtual destructor.
    */
   virtual ~MapLibAPI();

   /**
    * Register a callback which will be notified
    * when the startup is completed.
    * Note that MapLib will automatically be started after construction.
    * 
    * @param startupListener The listener to receive callback when startup
    *                        is completed.
    */
   void setStartupListener(MapLibStartupListener* startupListener);

   /**
    * Used to check if MapLib is succesfully started.
    * 
    * @return True if Maplib is succesfully started.
    */
   bool isMapLibStarted() const;
   
   /**
    * Returns the interface used to handle repaint requests.
    *
    * @return The MapDrawingInterface.
    */
   MapDrawingInterface* getMapDrawingInterface() const;

   /**
    * Returns the interface used for key handling.
    *
    * @return The MapLibKeyInterface.
    */
   MapLibKeyInterface* getKeyInterface();

   /**
    * Returns the interface used for moving, zooming etc.
    *
    * @return The MapOperationInterface.
    */
   MapOperationInterface* getMapOperationInterface();

   /**
    * Returns the interface used for configuration
    * settings of the MapLib instance.
    *
    * @return The ConfigInterface.
    */
   ConfigInterface* getConfigInterface();

   /**
    * Returns the interface used for receiving information
    * when map objects are selected.
    *
    * @return The MapObjectInterface.
    */
   MapObjectInterface* getMapObjectInterface();

   /**
    * Returns the interface used for adding overlays
    * on to the map.
    *
    * @return The MapOverlayInterface.
    */
   OverlayInterface* getOverlayInterface();

   /**
    * Returns an internal config that is not usable by
    * 3rd party developers. Used internally for debugging
    * and integration purposes.
    *
    * @return An internal configuration interface.
    */
   InternalConfig* getInternalConfig();

   /**
    * Set the drawing context for MapLib. The map is drawn 
    * onto the context.
    *
    * @param context An external context that should
    *                be used for drawing.   
    */
   void setDrawingContext(DrawingContext* context);

   /**
    * Terminates the running context. 
    *
    */ 
   void stop();
public:

   /**
    * Internal function.
    */
   MapLib* getMapLib();
private:
   
   /**
    * Private to avoid direct usage.
    */
   MapLibAPI(const MapLibAPI&);
   
   /**
    * Private to avoid direct usage.
    */
   const MapLibAPI& operator=(const MapLibAPI&);

   struct Impl;
   Impl* m_impl;

};

} // End of namespace WFAPI

#endif // WFAPI_MAPLIBAPI_H
