/*
Copyright (c) 1999 - 2010, Vodafone Group Services Ltd
All rights reserved.

Redistribution and use in source and binary forms, with or without modification, are permitted provided that the following conditions are met:

    * Redistributions of source code must retain the above copyright notice, this list of conditions and the following disclaimer.
    * Redistributions in binary form must reproduce the above copyright notice, this list of conditions and the following disclaimer in the documentation and/or other materials provided with the distribution.
    * Neither the name of the Vodafone Group Services Ltd nor the names of its contributors may be used to endorse or promote products derived from this software without specific prior written permission.

THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS" AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE ARE DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT HOLDER OR CONTRIBUTORS BE LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
*/


#ifndef STACKEDSET_H
#define STACKEDSET_H
#include "config.h"
#include <vector>
#include "MC2Point.h"
#include "MC2Coordinate.h"

class MapProjection;
class OverlayItemInternal;
class OverlayViewLayerInterface;
class MapProjectionAdapter;

namespace WFAPI{
class OverlayItemVisualSpec;
class WGS84Coordinate;
}

class StackedSet {
public:
   typedef std::vector<OverlayItemInternal*> StackVec;

   /**
    * Constructs a Stacked set.
    *
    * @param proj The projection that is used to translate between
    *             screen and world coordinates.
    * @param layerInterface The interface that supplies the OverlayItems.
    */
   StackedSet(MapProjectionAdapter& projection,
              OverlayViewLayerInterface& layerInterface);

   /**
    * Clears the current set of stacked items.
    */
   void clear();

   /**
    * @return A vector containing the stacks.
    */
   StackVec& getStackedItems();

   /**
    * Used to adjust the positions of the stacks so that they
    * are the average of the contained items' positions.
    */
   void smoothPositions();

   /**
    * Update the stacks according to the current state of the
    * layerInterface supplied at construction.
    */
   void update();

   void updateClosestItemPoint();
   
   MC2Point getClosestItemPoint();

   MC2Coordinate getClosestItemCoord();

   const OverlayItemInternal* getClosestItem();

   bool hasValidClosestItem();

   void enableAutomaticHighlight(bool enable);

   bool automaticHighlightEnabled();

private:
   /**
    * Adds an item to an existing stack, or creates a new stack for the item.
    *
    * @param item The item to add.
    */
   void addToStackSet(OverlayItemInternal* item);

   /**
    * Initializes the needed data for the item.
    *
    * @param item The item to initialize.
    */
   void initItem(OverlayItemInternal* item);

   void updateItemPixelBox(const OverlayItemInternal* item,
                           const WFAPI::OverlayItemVisualSpec* visualSpec,
                           const WFAPI::WGS84Coordinate& coord);

   void updateClosest(const OverlayItemInternal* newClosest);

   WFAPI::wf_float64 getSquareScreenDistance(MC2Point screenCenter,
                                             const OverlayItemInternal* item);

   MC2Point getPrecisionScaledScreenPosition(const OverlayItemInternal* item);

   /// The vector containing the current stacks.
   StackVec m_stacks;

   /// The projection used to translate between world and screen coordinates.
   MapProjectionAdapter& m_projection;

   /// The interface that supplies the overlay items.
   OverlayViewLayerInterface& m_layerInterface;

   /**
    * The item that is currently closest to the center of the screen,
    * the stack contents of the item and its position on the screen.
    */
   struct {
      const OverlayItemInternal* item;
      std::vector<const OverlayItemInternal*> stack;
      WFAPI::ScreenPoint point;
   } m_closest;

   bool m_automaticHighlightMode;
};


#endif // STACKEDSET_H
