/*
Copyright (c) 1999 - 2010, Vodafone Group Services Ltd
All rights reserved.

Redistribution and use in source and binary forms, with or without modification, are permitted provided that the following conditions are met:

    * Redistributions of source code must retain the above copyright notice, this list of conditions and the following disclaimer.
    * Redistributions in binary form must reproduce the above copyright notice, this list of conditions and the following disclaimer in the documentation and/or other materials provided with the distribution.
    * Neither the name of the Vodafone Group Services Ltd nor the names of its contributors may be used to endorse or promote products derived from this software without specific prior written permission.

THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS" AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE ARE DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT HOLDER OR CONTRIBUTORS BE LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
*/


#include "config.h"
#include "OverlayItemInternal.h"
#include "MC2Point.h"
#include "MapPlotter.h"
#include "PrecisionScalingConstants.h"


OverlayItemStackInfo::OverlayItemStackInfo() :
   size(1),
   next(NULL)
{
   
}

void OverlayItemStackInfo::clearStack()
{
   next = NULL;
   size = 1;
}

OverlayItemInternal::~OverlayItemInternal()
{

}

void OverlayItemStackInfo::addToStack(const OverlayItemInternal* item)
{   

   size += item->m_stack.size;
   
   item->m_stack.size = 1;

   // Since item can contain a stack itself, we find the end of
   // the stack first then set it to point to our own "next".
   const OverlayItemInternal* last = item;
   while (last->m_stack.next != NULL){
      last = last->m_stack.next;
   }
   
   // The last item should point to the current next.
   last->m_stack.next = next;
   
   // And our next should instead point to the input item (the first item
   // in the incoming stack).
   next = item;
}

const OverlayItemInternal* OverlayItemStackInfo::getNext()
{
   return next;
}

unsigned int OverlayItemStackInfo::getSize() const
{
   return size;
}

void OverlayItemInternal::setPosition(const WFAPI::WGS84Coordinate& position)
{
   m_position = position;
}

WFAPI::WGS84Coordinate OverlayItemInternal::getPosition() const
{
   return m_position; 
}

OverlayItemInternal::OverlayItemInternal(
   const WFAPI::OverlayItemZoomSpec* zoomSpec,
   const WFAPI::MapObjectInfo& mapObjectInfo,
   const WFAPI::WGS84Coordinate& position) :
   OverlayItem(zoomSpec, mapObjectInfo, position),
   m_isHighlighted(false),
   m_isBeingTapped(false)
{

}

PixelBox OverlayItemInternal::getPixelBox() const
{
   MC2Point topLeft(m_box.getTopLeft().getX() / SCALING_CONSTANT,
                    m_box.getTopLeft().getY() / SCALING_CONSTANT);
   MC2Point bottomRight(m_box.getBottomRight().getX() / SCALING_CONSTANT,
                        m_box.getBottomRight().getY() / SCALING_CONSTANT);
   
   return PixelBox(topLeft, bottomRight);
}

PixelBox& OverlayItemInternal::getPrecisionScaledPixelBox() const
{
   return m_box; 
}
