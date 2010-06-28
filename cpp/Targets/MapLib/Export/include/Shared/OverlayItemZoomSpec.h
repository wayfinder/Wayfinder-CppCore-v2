/*
  Copyright (c) 1999 - 2010, Vodafone Group Services Ltd
  All rights reserved.

  Redistribution and use in source and binary forms, with or without modification, are permitted provided that the following conditions are met:

  * Redistributions of source code must retain the above copyright notice, this list of conditions and the following disclaimer.
  * Redistributions in binary form must reproduce the above copyright notice, this list of conditions and the following disclaimer in the documentation and/or other materials provided with the distribution.
  * Neither the name of the Vodafone Group Services Ltd nor the names of its contributors may be used to endorse or promote products derived from this software without specific prior written permission.

  THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS" AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE ARE DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT HOLDER OR CONTRIBUTORS BE LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
*/

#ifndef WFAPI_OVERLAYITEMZOOMSPEC_H
#define WFAPI_OVERLAYITEMZOOMSPEC_H

#include "PALMachineTypes.h"
#include "RefCounted.h"
#include <vector>

namespace WFAPI {

// Forward declarations
class OverlayItemVisualSpec;
   
/**
 * Zoom specification class for overlay items.
 *
 * Used to configure the zoom specific visual representation
 * for overlay items. Used when creating an OverlayItem.
 */
class OverlayItemZoomSpec : public RefCounted {
public:
   /**
    * Allocates a new zoom specification. All instantiation is taken
    * care of here so that it will not be stack allocated.
    *
    * Initially the reference count will be set to 1. Once you are done
    * with this object, you will need to call removeReference to ensure
    * that the object is properly deallocated.
    */ 
   static OverlayItemZoomSpec* allocate();
   
   /**
    * Add zoom level range with the given OverlayItemVisualSpec.
    * If several calls are made with overlapping ranges, which
    * specification to be used at such a range is undefined. If
    * the overlay item is highlighted and a highlight specification
    * has been set, it will be used instead. It is possible to use
    * the same visual specification for the normal and
    * stacked states. 
    *
    * @param minZoom The lower level of zoom of which
    *                rangeSpec will be used.
    * @param maxZoom The upper level of zoom of which
    *                rangeSpec will be used.

    * @param normalSpec The visual specification that will be used
    *                   for the normal state.
    * @param stackedSpec The visual specification that will be used
    *                    for the stacked state.
    */
   void addZoomLevelRange(wf_uint32 minZoom, wf_uint32 maxZoom,
                          const OverlayItemVisualSpec* normalSpec,
                          const OverlayItemVisualSpec* stackedSpec);

   /**
    * Add zoom level range with the given OverlayItemVisualSpecs.
    * If several calls are made with overlapping ranges, which
    * specification to be used at such a range is undefined. If
    * the overlay item is highlighted and a highlight specification
    * has been set, it will be used instead. If an item is being tapped and a
    * set of tapped visual specifications are supplied, it will be used
    * instead. It is possible to use the same visual specification for all
    * different states. 
    *
    * @param minZoom The lower level of zoom of which
    *                rangeSpec will be used.
    * @param maxZoom The upper level of zoom of which
    *                rangeSpec will be used.

    * @param normalSpec The visual specification that will be used
    *                   for the normal state.
    * @param stackedSpec The visual specification that will be used
    *                    for the stacked state.
    * @param normalTappedSpec The visual specification that will be used
    *                         for the normal state when being tapped.
    * @param stackedTappedSpec The visual specification that will be used
    *                          for the stacked state when being tapped.
    */
   void addZoomLevelRange(wf_uint32 minZoom, wf_uint32 maxZoom,
                          const OverlayItemVisualSpec* normalSpec,
                          const OverlayItemVisualSpec* stackedSpec,
                          const OverlayItemVisualSpec* normalTappedSpec,
                          const OverlayItemVisualSpec* stackedTappedSpec);

   /**
    * Set the visual specification for highlighted items for normal and
    * stacked states.
    *
    * @param normalSpec The visual spec that is used for the normal state.
    * @param stackedSpec The visual spec that is used for the stacked state.
    */
   void setHighlightedSpecs(const OverlayItemVisualSpec* normalSpec,
                            const OverlayItemVisualSpec* stackedSpec);

   /**
    * Set the visual specification for highlighted items for normal and
    * stacked states, including specific visual specifications for when the
    * items are being tapped.
    *
    * @param normalSpec The visual spec that is used for the normal state.
    * @param stackedSpec The visual spec that is used for the stacked state.
    * @param normalTappedSpec The visual spec that is used for the normal state
    *                         when the item is being tapped.
    * @param stackedTappedSpec The visual spec that is used for the stacked
    *                          state when the item is being tapped.
    */
   void setHighlightedSpecs(const OverlayItemVisualSpec* normalSpec,
                            const OverlayItemVisualSpec* stackedSpec,
                            const OverlayItemVisualSpec* normalTappedSpec,
                            const OverlayItemVisualSpec* stackedTappedSpec);

   /**
    *  @param zoomLevel The zoom level to use. 
    *  @return The visual spec that is used for the normal state.
    */ 
   const OverlayItemVisualSpec* getNormalSpec(wf_uint32 zoomLevel) const;

   /**
    * @param zoomLevel The zoom level to use. 
    * @return The visual spec that is used for the stacked state.
    */ 
   const OverlayItemVisualSpec* getStackedSpec(wf_uint32 zoomLevel) const;

   /**
    *  @param zoomLevel The zoom level to use. 
    *  @return The visual spec that is used for the normal state when tapped.
    */ 
   const OverlayItemVisualSpec* getNormalTappedSpec(wf_uint32 zoomLevel) const;

   /**
    * @param zoomLevel The zoom level to use. 
    * @return The visual spec that is used for the stacked state when tapped.
    */ 
   const OverlayItemVisualSpec* getStackedTappedSpec(wf_uint32 zoomLevel) const;

   /**
    *  @return The visual spec that is used for the normal state
    *          when the item is highlighted.
    */ 
   const OverlayItemVisualSpec* getNormalHighlightedSpec() const;

   /**
    * @return The visual spec that is used for the stacked state
    *         when the item is highlighted.
    */ 
   const OverlayItemVisualSpec* getStackedHighlightedSpec() const;

   /**
    *  @return The visual spec that is used for the normal state
    *          when the item is highlighted.
    */ 
   const OverlayItemVisualSpec* getNormalTappedHighlightedSpec() const;

   /**
    * @return The visual spec that is used for the stacked state
    *         when the item is highlighted.
    */ 
   const OverlayItemVisualSpec* getStackedTappedHighlightedSpec() const;

protected:
   virtual ~OverlayItemZoomSpec();
private:
   /**
    * Constructor.
    */
   OverlayItemZoomSpec();
   
   /**
    * Internal class used to store our different zoom ranges.
    */ 
   struct ZoomRange {
      /// The start zoom level for this range
      wf_uint32 startZoomLevel;

      /// The end zoom level for this range
      wf_uint32 endZoomLevel;
      
      /// Our visual spec for the normal state
      const OverlayItemVisualSpec* normalSpec;

      /// Our visual spec for the stacked state
      const OverlayItemVisualSpec* stackedSpec;

      /// Our visual spec for the normal state when tapped
      const OverlayItemVisualSpec* normalTappedSpec;

      /// Our visual spec for the stacked state when tapped
      const OverlayItemVisualSpec* stackedTappedSpec;
   };
   
   /// Comparison function used for sorting
   friend bool zoomRangeCmp(const ZoomRange& lhs, const ZoomRange& rhs);
   
   /**
   * @return the zoom range for a given zoom level.
   */ 
   const ZoomRange* getZoomRange(wf_uint32 zoomLevel) const;
   
   /// Please note that the use of a vector here instead of a map
   /// is due to a variety of reasons. Consider that the number of
   /// different zoom ranges will most likely be small (i.e. n < 10)
   /// and for this case vectors are suitable efficient.
   typedef std::vector<ZoomRange> ZoomRangeVector;
   
   ZoomRangeVector m_zoomRanges;

   /// The visual specification for a normal (unstacked) item when highlighted.
   const OverlayItemVisualSpec* m_normalHighlighted;
   /// The visual specification for a stacked item when highlighted.
   const OverlayItemVisualSpec* m_stackedHighlighted;
   
   /**
    * The visual specification for a normal (unstacked) item when highlighted
    * and tapped.
    */
   const OverlayItemVisualSpec* m_normalTappedHighlighted;
   
   /**
    * The visual specification for a stacked item when highlighted
    * and tapped.
    */
   const OverlayItemVisualSpec* m_stackedTappedHighlighted; 

};
   
} // End of namespace WFAPI

#endif // WFAPI_OVERLAYITEMZOOMSPEC_H
