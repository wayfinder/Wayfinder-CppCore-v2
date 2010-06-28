/*
  Copyright (c) 1999 - 2010, Vodafone Group Services Ltd
  All rights reserved.

  Redistribution and use in source and binary forms, with or without modification, are permitted provided that the following conditions are met:

  * Redistributions of source code must retain the above copyright notice, this list of conditions and the following disclaimer.
  * Redistributions in binary form must reproduce the above copyright notice, this list of conditions and the following disclaimer in the documentation and/or other materials provided with the distribution.
  * Neither the name of the Vodafone Group Services Ltd nor the names of its contributors may be used to endorse or promote products derived from this software without specific prior written permission.

  THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS" AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE ARE DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT HOLDER OR CONTRIBUTORS BE LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
*/

#ifndef WFAPI_OVERLAYITEM_H
#define WFAPI_OVERLAYITEM_H 

#include <vector>
#include "RefCounted.h"
#include "PALMachineTypes.h"
#include "MapObjectInfo.h"
#include "WGS84Coordinate.h"

class OverlayItemInternal;

namespace WFAPI {

// Forward declarations
class ImageSpec;
class OverlayItemZoomSpec;
class MapObjectInfo;


/**
 * Class representing an item that can be overlayed onto the map.
 * Each overlay will belong to a layer once added to the map.
 */
class OverlayItem : public RefCounted {
public:
   /**
    * Creates a new overlay item that can be placed on a layer.
    *
    * Initially the reference count will be set to 1. Once you are done
    * with this object, you will need to call removeReference to ensure
    * that the object is properly deallocated.
    *  
    * @param zoomSpec The zoom specification for the item, containing visual
    *                 specifications for different zoom ranges. The reference
    *                 count will be increased during the lifetime of this
    *                 OverlayItem.
    * @param mapObjectInfo The information object associated with this item,
    *                      containing descriptions and other properties. Will
    *                      be copied internally.
    * @param position The position of the overlay item, either in screen or
    *                 world coordinates.
    */
   static OverlayItem* allocate(const OverlayItemZoomSpec* zoomSpec,
                                const MapObjectInfo& mapObjectInfo,
                                const WGS84Coordinate& position);
   
   /**
    * Exactly the same as the previous method, but instead of copying
    * the MapObjectInfo the ownership is transferred.
    *
    * Initially the reference count will be set to 1. Once you are done
    * with this object, you will need to call removeReference to ensure
    * that the object is properly deallocated.
    */
   static OverlayItem* allocate(const OverlayItemZoomSpec* zoomSpec,
                                MapObjectInfo* mapObjectInfo,
                                const WGS84Coordinate& position);
   
   /**
    * Adds an image representation which can be used by the zoom spec
    * or other specs to represent this OverlayItem. The items are
    * numbered in the order they were added - the first image added
    * will get index 0, and so forth. For extra clarity, the id is
    * returned.
    *
    * @param imgSpec The image specification to add.
    * @return id The resulting id of the added image.
    */ 
   wf_uint32 addImageSpec(ImageSpec* imgSpec);

   /**
    * Returns an image spec corresponding to the supplied id. If no
    * such image spec exists, NULL will be returned.
    */ 
   const ImageSpec* getImageSpec(wf_uint32 id) const;

   /**
    * To remove all image spec.
    * If to change image for an overlayItem, first call this function and then
    * add new imageSpec using addImageSpec function.
    */
   void removeAllImageSpec();
   
   /**
    *  Sets the position of the overlay item. This position be
    *  used to draw the object on the map.
    *
    *  @param position The position of the overlay item, either in screen
    *                  or world coordinates.
    */ 
   virtual void setPosition(const WGS84Coordinate& position) = 0;

   /**
    * @return The zoom specification of this overlay item.
    */ 
   const OverlayItemZoomSpec* getZoomSpec() const;

   /**
    * @return The map object info associated with this overlay
    *         item.
    */
   const MapObjectInfo& getMapObjectInfo() const;

   /**
    * @return The position of this overlay item.
    */ 
   const WGS84Coordinate& getPosition() const;

   /**
    * @return True if the item is stackable.
    */
   const bool isStackable() const;

   /**
    * Sets the stackability of the item.
    * 
    * @param stackable If true, the item will be stackable.
    */
   void setStackable(bool stackable);
   
protected:
   /**
    *  Destructor.
    */ 
   virtual ~OverlayItem() = 0;

   /**
    * Constructor. @see allocate
    */ 
   OverlayItem(const OverlayItemZoomSpec* zoomSpec,
               const MapObjectInfo& mapObjectInfo,
               const WGS84Coordinate& position);
protected:

   /// Container of ImageSpec pointers.
   typedef std::vector<ImageSpec*> ImageVector;

   /// Our overlay images
   ImageVector m_images;
   
   /// Our zoom specification
   const OverlayItemZoomSpec* m_zoomSpec;

   /// Our map object info
   const MapObjectInfo m_mapObjectInfo;

   /// Our overlay position in the map
   WGS84Coordinate m_position;

   /// Determines whether the item is stackable or not.
   bool m_stackable;

};

/// Typedef for a vector containing OverlayItems.
typedef std::vector<OverlayItem*> OverlayItemVector;

} // End of namespace WFAPI

#endif // WFAPI_OVERLAYITEM_H 
