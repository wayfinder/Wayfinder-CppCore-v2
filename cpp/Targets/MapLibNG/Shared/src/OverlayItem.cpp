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
#include "OverlayItem.h"
#include "WGS84Coordinate.h"
#include "ImageSpec.h"
#include "OverlayItemZoomSpec.h"
#include "OverlayItemInternal.h"
#include "RefCountHelpers.h"



namespace WFAPI {

OverlayItem::~OverlayItem() 
{
   CORE_FUNCTION_PROLOGUE();
   
   REMOVE_REF(m_zoomSpec);
   
   for(ImageVector::iterator itr = m_images.begin(); itr != m_images.end();
       itr++)
   {
      REMOVE_REF((*itr));
   }
}

OverlayItem* OverlayItem::allocate(const OverlayItemZoomSpec* zoomSpec,
                                   const MapObjectInfo& mapObjectInfo,
                                   const WGS84Coordinate& position)
{
   CORE_FUNCTION_PROLOGUE();
   
   return new OverlayItemInternal(zoomSpec, mapObjectInfo, position);
}

OverlayItem* OverlayItem::allocate(const OverlayItemZoomSpec* zoomSpec,
                                   MapObjectInfo* mapObjectInfo,
                                   const WGS84Coordinate& position)
{
   CORE_FUNCTION_PROLOGUE();
   
   /// TODO: implement real transfer of ownership
   return new OverlayItemInternal(zoomSpec, *mapObjectInfo, position);
}

wf_uint32 WFAPI::OverlayItem::addImageSpec(ImageSpec* imgSpec)
{
   CORE_FUNCTION_PROLOGUE();
   
   imgSpec->addReference();
   
   m_images.push_back(imgSpec);
   
   return m_images.size() - 1;
}

const ImageSpec* WFAPI::OverlayItem::getImageSpec(wf_uint32 id) const
{
   CORE_FUNCTION_PROLOGUE();
   
   if( id >= m_images.size() ) {
      return NULL;
   } else {
      return m_images[id];
   }
}

void
OverlayItem::removeAllImageSpec()
{
   CORE_FUNCTION_PROLOGUE();
   
   ImageVector::iterator it = m_images.begin();
   ImageVector::iterator end = m_images.end();
   for(; it != end; it++) {
      (*it)->removeReference();
   }
   m_images.clear();
}

OverlayItem::OverlayItem(const OverlayItemZoomSpec* zoomSpec,
                         const MapObjectInfo& mapObjectInfo,
                         const WGS84Coordinate& position) :
   m_zoomSpec(zoomSpec),
   m_mapObjectInfo(mapObjectInfo),
   m_position(position),
   m_stackable(true)
{
   CORE_FUNCTION_PROLOGUE();
   
   m_images.reserve(3);
   m_zoomSpec->addReference();
}

const OverlayItemZoomSpec* OverlayItem::getZoomSpec() const
{
   CORE_FUNCTION_PROLOGUE();
   
   return m_zoomSpec;
}

const MapObjectInfo& OverlayItem::getMapObjectInfo() const
{
   CORE_FUNCTION_PROLOGUE();
   
   return m_mapObjectInfo;
}

const WGS84Coordinate& OverlayItem::getPosition() const
{
   CORE_FUNCTION_PROLOGUE();
   
   return m_position;
}

const bool OverlayItem::isStackable() const
{
   CORE_FUNCTION_PROLOGUE();
   
   return m_stackable;
}

} // End of namespace WFAPI

void WFAPI::OverlayItem::setStackable(bool stackable)
{
   CORE_FUNCTION_PROLOGUE();
   
   m_stackable = stackable; 
}




