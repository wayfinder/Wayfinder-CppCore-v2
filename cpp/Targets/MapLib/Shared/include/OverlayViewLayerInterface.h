/*
Copyright (c) 1999 - 2010, Vodafone Group Services Ltd
All rights reserved.

Redistribution and use in source and binary forms, with or without modification, are permitted provided that the following conditions are met:

    * Redistributions of source code must retain the above copyright notice, this list of conditions and the following disclaimer.
    * Redistributions in binary form must reproduce the above copyright notice, this list of conditions and the following disclaimer in the documentation and/or other materials provided with the distribution.
    * Neither the name of the Vodafone Group Services Ltd nor the names of its contributors may be used to endorse or promote products derived from this software without specific prior written permission.

THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS" AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE ARE DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT HOLDER OR CONTRIBUTORS BE LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
*/


#ifndef OVERLAYVIEWLAYERINTERFACE_H
#define OVERLAYVIEWLAYERINTERFACE_H

#include "OverlayInterface.h"
#include "Layer.h"
#include <map>

class OverlayView;

class OverlayViewLayerInterface : public WFAPI::OverlayInterface {
public:
   typedef std::map<layer_id_t, Layer*> LayerMap;
   
   OverlayViewLayerInterface(OverlayView* m_view);

   virtual ~OverlayViewLayerInterface();

   /**
    * @see OverlayInterface::addOverlayItem
    */ 
   virtual bool addOverlayItem(WFAPI::OverlayItem* item, layer_id_t layerID);


   /**
    * @see OverlayInterface::addOverlayItems
    */
   virtual bool addOverlayItems(const WFAPI::OverlayItemVector& overlayItems,
                                layer_id_t layerID);

   /**
    * @see OverlayInterface::removeOverlayItem
    */
   virtual bool removeOverlayItem(WFAPI::OverlayItem* overlayItem,
                                  layer_id_t layerID);
   
   /**
    * @see OverlayInterface::clearLayer
    */
   virtual void clearLayer(layer_id_t layerID);

   /**
    * @see OverlayInterface::hideLayer
    */
   virtual void hideLayer(layer_id_t layerID);

   /**
    * @see OverlayInterface::showLayer
    */
   virtual void showLayer(layer_id_t layerID);

   /**
    * @see OverlayInterface::getLayerVisible
    */
   virtual bool getLayerVisible(layer_id_t layerID) const;

   /**
    * @see OverlayInterface::getCurrentHighlightedItem
    */
   virtual const WFAPI::OverlayItem* getCurrentHighlightedItem();

   /**
    * Returns an iterator that can be used to iterate
    * over the current layers.
    *
    * @return Iterator for layers.
    */ 
   LayerMap::iterator begin();

   /**
    * Returns an iterator that can be used to iterate
    * over the current layers.
    *
    * @return Iterator for layers.
    */ 
   LayerMap::iterator end();

   /**
    * Clear all items cacheblocks for every layer.
    */
   void clearCachingBlocksForLayersItems();
   
private:
   /**
    * Returns or creates an internal layer. If it is created it will
    * be associated with id layerID.
    *
    * @param layerId The id of the layer to create and return or merely
    *                return.
    */ 
   Layer& getOrCreateLayer(layer_id_t layerID);
   
   /// The layers of the map
   LayerMap m_layers;
   
   OverlayView* m_view;
};

#endif // OVERLAYVIEWLAYERINTERFACE_H
