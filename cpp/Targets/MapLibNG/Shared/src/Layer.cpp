/*
Copyright (c) 1999 - 2010, Vodafone Group Services Ltd
All rights reserved.

Redistribution and use in source and binary forms, with or without modification, are permitted provided that the following conditions are met:

    * Redistributions of source code must retain the above copyright notice, this list of conditions and the following disclaimer.
    * Redistributions in binary form must reproduce the above copyright notice, this list of conditions and the following disclaimer in the documentation and/or other materials provided with the distribution.
    * Neither the name of the Vodafone Group Services Ltd nor the names of its contributors may be used to endorse or promote products derived from this software without specific prior written permission.

THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS" AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE ARE DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT HOLDER OR CONTRIBUTORS BE LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
*/


#include "Layer.h"
#include "OverlayItemInternal.h"
#include "RefCountHelpers.h"
#include <algorithm>


// Helper function for for_each.
void referenceRemover(OverlayItemInternal* item) {
   REMOVE_REF(item);
}

// Helper function for for_each.
void referenceAdder(OverlayItemInternal* item) {
   item->addReference();
}

Layer::Layer()
{
   m_visible = true;
}

bool Layer::getVisible() const
{
   return m_visible;
}

Layer::~Layer()
{
   clear();
}

void Layer::setVisible(bool visible)
{
   m_visible = visible; 
}

void Layer::clear()
{
   // We must decrease the reference count before clearing the internal
   // vector.
   std::for_each(m_items.begin(), m_items.end(), referenceRemover );
   // This should make the vector shrink after removing the objects,
   // returning memory.
   std::vector<OverlayItemInternal*>().swap(m_items);
//   m_items.clear();
}

bool Layer::removeItem(OverlayItemInternal* overlayItem)
{
   std::vector<OverlayItemInternal*>::iterator res =
      std::find(m_items.begin(), m_items.end(), overlayItem);
   
   if (res != m_items.end()){
      m_items.erase(res);
      REMOVE_REF(overlayItem);
      return true;
   } else {
      return false;
   }
}

bool Layer::addOverlayItem(OverlayItemInternal* overlayItem)
{
   std::vector<OverlayItemInternal*>::iterator res =
      std::find(m_items.begin(), m_items.end(), overlayItem);
   
   if (res == m_items.end()) {
      // The element was not found, so we add 
      overlayItem->addReference();
      m_items.push_back(overlayItem);
      
      return true;
      
   } else {
      // The element was found, so we do nothing.
      return false;
      
   }
}

Layer::const_iterator Layer::items_begin()
{
   return m_items.begin();
}

Layer::const_iterator Layer::items_end()
{
   return m_items.end();
}

void
Layer::clearCachingBlocks() {
   
   std::vector<OverlayItemInternal*>::iterator it = m_items.begin();
   std::vector<OverlayItemInternal*>::iterator itEnd = m_items.end();
   for(; it != itEnd; it++) {
      (*it)->m_numberBoxCache.releaseData();
      (*it)->m_textBoxCache.releaseData();
   }
   
}

