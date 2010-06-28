/*
  Copyright (c) 1999 - 2010, Vodafone Group Services Ltd
  All rights reserved.

  Redistribution and use in source and binary forms, with or without modification, are permitted provided that the following conditions are met:

  * Redistributions of source code must retain the above copyright notice, this list of conditions and the following disclaimer.
  * Redistributions in binary form must reproduce the above copyright notice, this list of conditions and the following disclaimer in the documentation and/or other materials provided with the distribution.
  * Neither the name of the Vodafone Group Services Ltd nor the names of its contributors may be used to endorse or promote products derived from this software without specific prior written permission.

  THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS" AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE ARE DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT HOLDER OR CONTRIBUTORS BE LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
*/

#ifndef WFAPI_OVERLAYINTERFACE_H
#define WFAPI_OVERLAYINTERFACE_H 

#include "PALMachineTypes.h"
#include "OverlayItem.h"

class MapLib;

namespace WFAPI {

class ImageSpec;
class OverlayItemZoomSpec;
class OverlayPos;

/**
 * Class which encapsulates the functionality of adding
 * overlay information onto the map.
 *
 * Layers are automatically created(if they do not exist) when
 * referenced in overlay item adding operations. Layers are drawn
 * in ascending order, where a larger layer number will result in
 * the layer being drawn later.
 *
 */
class OverlayInterface {
public:
   /**
    * Constructor, not to be used directly.
    */ 
   OverlayInterface();

   /**
    * Destructor.
    */ 
   virtual ~OverlayInterface();
   
   /**
    * The layer id type.
    */
   typedef wf_uint32 layer_id_t;
   
   /**
    * Add the overlay item to the layer.
    * 
    * Increases the reference count of the overlay item for the duration
    * of its lifetime within the layer.
    *
    * @param overlayItem The item to be added.
    * @param layerID The id of the layer that the overlay item should be
    *                placed on.
    * @return Will return false if the overlayItem already exists within
    *         the layer.               
    */
   virtual bool addOverlayItem(OverlayItem* overlayItem,
                               layer_id_t layerID) = 0;

   /**
    * Add multiple overlay items to a layer. More efficient than adding
    * them one by one.
    * 
    * Increases the reference count for all the overlay items for the
    * duration of its lifetime within the layer.
    *
    * @param overlayItems Vector of items to be added.
    * @param layerID The id of the layer that the overlay item should be
    *                placed on.
    * @return Will return false if the overlayItem already exists within
    *         the layer.               
    */
   virtual bool addOverlayItems(const OverlayItemVector& overlayItems,
                                layer_id_t layerID) = 0;

   /**
    * Remove a specific overlay item from a specified layer.
    * Returns true if the item was successfully removed.
    *
    * @param overlayItem The OverlayItem to remove.
    * @param layerID The id of the layer containing the OverlayItem.
    * @return True if the item was successfully removed.
    */
   virtual bool removeOverlayItem(OverlayItem* overlayItem,
                                  layer_id_t layerID) = 0;
   
   /**
    * Clear the layer and remove all associated content.
    *
    * @param layerID The layer to clear.
    */
   virtual void clearLayer(layer_id_t layerID) = 0;

   /**
    * Hide a layer specified by the layerid.
    *
    * @param layerID The layer to hide.
    */
   virtual void hideLayer(layer_id_t layerID) = 0;

   /**
    * Show a layer specified by the layer id.
    *
    * @param layerID The layer to show.
    */
   virtual void showLayer(layer_id_t layerID) = 0;

   /**
    * Get if the layer is visible.
    *
    * @param layerID The layer to query visibility information from.
    * @return True if the layer is visible.
    */
   virtual bool getLayerVisible(layer_id_t layerID) const = 0;

   /**
    * Get the current highlighted item. If no item is currently highlighted,
    * NULL will be returned.
    *
    * @return The current highlighted item, NULL if no item is highlighted.
    */
   virtual const OverlayItem* getCurrentHighlightedItem() = 0;
   
private:
   struct Impl;
   Impl* m_impl;
};

} // End of namespace WFAPI

#endif // WFAPI_OVERLAY_INTERFACE_H 
