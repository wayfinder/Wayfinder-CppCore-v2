/*
Copyright (c) 1999 - 2010, Vodafone Group Services Ltd
All rights reserved.

Redistribution and use in source and binary forms, with or without modification, are permitted provided that the following conditions are met:

    * Redistributions of source code must retain the above copyright notice, this list of conditions and the following disclaimer.
    * Redistributions in binary form must reproduce the above copyright notice, this list of conditions and the following disclaimer in the documentation and/or other materials provided with the distribution.
    * Neither the name of the Vodafone Group Services Ltd nor the names of its contributors may be used to endorse or promote products derived from this software without specific prior written permission.

THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS" AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE ARE DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT HOLDER OR CONTRIBUTORS BE LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
*/


#ifndef OVERLAYITEMINTERNAL_H
#define OVERLAYITEMINTERNAL_H

#include "MC2BoundingBox.h"
#include "PALMachineTypes.h"
#include "OverlayItem.h"
#include "PixelBox.h"
#include "WGS84Coordinate.h"
#include "TextCachingBlock.h"
#include "FontCachingBlock.h"

namespace WFAPI {
class OverlayItemZoomSpec;
class MapObjectInfo;
}

class OverlayItemInternal;

namespace isab {
class MapPlotter;
}


/**
 * This struct is used to keep the stacking information of the
 * OverlayItems. The stack is implemented as a NULL-terminated
 * single linked list.
 */
struct OverlayItemStackInfo {
public:
   OverlayItemStackInfo();

   /**
    * Clears the stack information.
    */
   void clearStack();

   /**
    * Modifies the stack to include the supplied item.
    *
    * @param item The item to add to the stack.
    */
   void addToStack(const OverlayItemInternal* item);

   /**
    * A getter to the next item in the linked list.
    */
   const OverlayItemInternal* getNext();

   /**
    * @return The size of the stack (including the item that owns the stack).
    */
   unsigned int getSize() const;

private:
   /// The size of the stack.
   unsigned int size;

   /// The next item in the linked list.
   const OverlayItemInternal* next;
};

/**
 * The internal representation of an OverlayItem.
 * Contains mutable stacking information, string caching and adjusted position
 * information. All of these are subject to change in order to draw the item
 * correctly on screen, hence the mutability of these members.
 */
class OverlayItemInternal : public WFAPI::OverlayItem {
public:

   /**
    * Constructs a new internal representation of an OverlayItem.
    * @see OverlayItem::OverlayItem for explanation of the parameters.
    */ 
   OverlayItemInternal(const WFAPI::OverlayItemZoomSpec* zoomSpec,
                       const WFAPI::MapObjectInfo& mapObjectInfo,
                       const WFAPI::WGS84Coordinate& position);

   /**
    * Sets the position of the item.
    * 
    * @param position The position to set.
    */
   virtual void setPosition(const WFAPI::WGS84Coordinate& position);

   /**
    * @return The position of the item.
    */
   WFAPI::WGS84Coordinate getPosition() const;

   /**
    * @return The pixelbox for the item, expressed in screen coordinates.
    */
   PixelBox getPixelBox() const;

   /**
    * @return The pixelbox used to overlap detect with other items
    *         and other pixelboxes in the same scale of precision.
    */
   PixelBox& getPrecisionScaledPixelBox() const;

   /// A struct containing all stacking information
   mutable OverlayItemStackInfo m_stack;

   /// A struct containing cached internal string representations.
   mutable TextCachingBlock m_numberBoxCache;

   /// A struct containing cached internal string representations.
   mutable TextCachingBlock m_textBoxCache;
   
   // /// A struct containing cached font information used to speed up drawing
   // mutable FontCachingBlock m_fontInfo;
   
   /// The position that will be used when drawing the overlayitem.
   mutable WFAPI::WGS84Coordinate m_adjustedPosition;

   /// Whether the item is currently highlighted or not.
   mutable bool m_isHighlighted;

   /// Whether the item is currently being tapped or not.
   mutable bool m_isBeingTapped;
private:
   /// The pixelbox used to detect overlaps with other items etc.
   mutable PixelBox m_box;

   /// Private destructor.
   virtual ~OverlayItemInternal();
};

typedef std::vector<OverlayItemInternal*> OverlayItemInternalVector;

#endif // OVERLAYITEMINTERNAL_H
