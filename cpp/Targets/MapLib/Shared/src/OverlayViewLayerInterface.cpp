/*
Copyright (c) 1999 - 2010, Vodafone Group Services Ltd
All rights reserved.

Redistribution and use in source and binary forms, with or without modification, are permitted provided that the following conditions are met:

    * Redistributions of source code must retain the above copyright notice, this list of conditions and the following disclaimer.
    * Redistributions in binary form must reproduce the above copyright notice, this list of conditions and the following disclaimer in the documentation and/or other materials provided with the distribution.
    * Neither the name of the Vodafone Group Services Ltd nor the names of its contributors may be used to endorse or promote products derived from this software without specific prior written permission.

THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS" AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE ARE DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT HOLDER OR CONTRIBUTORS BE LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
*/


#include "CoreFunctionPrologue.h"
#include "config.h"
#include "OverlayViewLayerInterface.h"
#include "Layer.h"
#include "OverlayView.h"



OverlayViewLayerInterface::~OverlayViewLayerInterface()
{
   for(LayerMap::iterator itr = m_layers.begin(); itr != m_layers.end();
       itr++)
   {
      Layer* cur = itr->second;
      delete cur;
   }

   m_layers.clear();
}

OverlayViewLayerInterface::OverlayViewLayerInterface(OverlayView* view)
{
   m_view = view; 
}

bool
OverlayViewLayerInterface::addOverlayItem(WFAPI::OverlayItem* item,
                                          layer_id_t layerID)
{
   CORE_FUNCTION_PROLOGUE();
   
   Layer& l = getOrCreateLayer(layerID);
   bool ret = l.addOverlayItem(static_cast<OverlayItemInternal*>(item));
   
   m_view->updateState();

   return ret;
}

bool
OverlayViewLayerInterface::addOverlayItems(
   const WFAPI::OverlayItemVector& overlayItems,
   layer_id_t layerID)
{
   CORE_FUNCTION_PROLOGUE();
   
   // It would be better to remove the added items if some fails.
   bool ret = true;
   Layer& l = getOrCreateLayer(layerID);
   for (unsigned int i = 0; i < overlayItems.size(); ++i) {
      bool curRetVal = l.addOverlayItem(
         static_cast<OverlayItemInternal*>(overlayItems[i]));
      if (!curRetVal) {
         ret = false;
      }
   }
   
   m_view->updateState();
   return ret;
}

bool
OverlayViewLayerInterface::removeOverlayItem(WFAPI::OverlayItem* overlayItem,
                                             layer_id_t layerID)
{
   CORE_FUNCTION_PROLOGUE();
   
   Layer& l = getOrCreateLayer(layerID);
   m_view->overlayItemRemoved(static_cast<OverlayItemInternal*>(overlayItem));
   
   bool ret = l.removeItem(static_cast<OverlayItemInternal*>(overlayItem));
   m_view->updateState();
   return ret;
}

void OverlayViewLayerInterface::clearLayer(layer_id_t layerID)
{
   CORE_FUNCTION_PROLOGUE();
   
   Layer& l = getOrCreateLayer(layerID);
   
   for(Layer::const_iterator itr = l.items_begin();
       itr != l.items_end();
       itr++)
   {
      const OverlayItemInternal* item = *itr;
      m_view->overlayItemRemoved(item);
   }
   
   l.clear();
   m_view->updateState();
}

void OverlayViewLayerInterface::hideLayer(layer_id_t layerID)
{
   CORE_FUNCTION_PROLOGUE();
   
   Layer& l = getOrCreateLayer(layerID);
   l.setVisible(false);
   m_view->updateState();
}

void OverlayViewLayerInterface::showLayer(layer_id_t layerID)
{
   CORE_FUNCTION_PROLOGUE();
   
   Layer& l = getOrCreateLayer(layerID);
   l.setVisible(true);
   m_view->updateState();
}

bool OverlayViewLayerInterface::getLayerVisible(layer_id_t layerID) const
{
   CORE_FUNCTION_PROLOGUE();
   
   LayerMap::const_iterator itr = m_layers.find(layerID);

   if(itr != m_layers.end()) {
      return itr->second->getVisible();
   } else {
      return false;
   }
}

Layer& OverlayViewLayerInterface::getOrCreateLayer(layer_id_t layerID)
{
   LayerMap::iterator itr = m_layers.find(layerID);

   if (itr!=m_layers.end()) {
      return *itr->second;
   } else {
      Layer* ret = new Layer;
      m_layers[layerID] = ret;
      return *ret;
   }
}

OverlayViewLayerInterface::LayerMap::iterator
OverlayViewLayerInterface::begin()
{
   return m_layers.begin();
}

OverlayViewLayerInterface::LayerMap::iterator
OverlayViewLayerInterface::end()
{
   return m_layers.end();
}

void OverlayViewLayerInterface::clearCachingBlocksForLayersItems()
{
   LayerMap::iterator itr = m_layers.begin();
   LayerMap::iterator itrEnd = m_layers.end();

   for(; itr != itrEnd ; itr++) {
      itr->second->clearCachingBlocks();
   }
}

const WFAPI::OverlayItem* OverlayViewLayerInterface::getCurrentHighlightedItem()
{
   if (m_view->hasValidClosestItem()){
      return m_view->getClosestItem();
   } else {
      return NULL;
   }
}
