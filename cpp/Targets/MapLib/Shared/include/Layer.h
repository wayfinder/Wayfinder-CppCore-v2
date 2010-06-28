/*
Copyright (c) 1999 - 2010, Vodafone Group Services Ltd
All rights reserved.

Redistribution and use in source and binary forms, with or without modification, are permitted provided that the following conditions are met:

    * Redistributions of source code must retain the above copyright notice, this list of conditions and the following disclaimer.
    * Redistributions in binary form must reproduce the above copyright notice, this list of conditions and the following disclaimer in the documentation and/or other materials provided with the distribution.
    * Neither the name of the Vodafone Group Services Ltd nor the names of its contributors may be used to endorse or promote products derived from this software without specific prior written permission.

THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS" AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE ARE DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT HOLDER OR CONTRIBUTORS BE LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
*/


#ifndef LAYER_H
#define LAYER_H
#include "config.h"
#include <vector>

class OverlayItemInternal;

class Layer {
public:
   typedef std::vector<OverlayItemInternal*> ItemVector;
   
   typedef ItemVector::const_iterator const_iterator;

   /**
    * Constructor.
    */ 
   Layer();
   
   virtual ~Layer();
   
   /**
    * Return whether the layer is visible or not.
    *
    * @return True if the layer is visible.
    */
   bool getVisible() const;

   /**
    * Sets the layers visible state.
    *
    * @param visible If true, the layer will be visible. 
    */
   void setVisible(bool visible);

   /**
    * Adds an overlay item to the layer, returns false if the
    * item already exists in the layer.
    *
    * @param overlayItem The item to add.
    * @return False if overlayItem already exists in the layer.
    */
   bool addOverlayItem(OverlayItemInternal* overlayItem);

   
   // This one is tricky, should get the difference of the sets and
   // increase their references before adding them to the internal
   // representation.
   // bool addOverlayItems(
   // const std::vector<OverlayItemInternal*> overlayItems);

   /**
    * Remove a specific overlay item. Returns true if the
    * item was successfully removed.
    *
    * @param overlayItem The item to remove.
    * @return True if the item was successfully removed.
    */
   bool removeItem(OverlayItemInternal* overlayItem);

   /**
    * Clears the layer of all overlay items.
    */
   void clear();

   const_iterator items_begin();

   const_iterator items_end();

   /**
    * Clears the caching blocks for all items in this layer.
    */
   void clearCachingBlocks();

private:

   /// The visible state.
   bool m_visible;

   /// The items in the layer.
   ItemVector m_items;
};

#endif // LAYER_H
