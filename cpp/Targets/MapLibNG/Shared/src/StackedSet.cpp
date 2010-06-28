/*
Copyright (c) 1999 - 2010, Vodafone Group Services Ltd
All rights reserved.

Redistribution and use in source and binary forms, with or without modification, are permitted provided that the following conditions are met:

    * Redistributions of source code must retain the above copyright notice, this list of conditions and the following disclaimer.
    * Redistributions in binary form must reproduce the above copyright notice, this list of conditions and the following disclaimer in the documentation and/or other materials provided with the distribution.
    * Neither the name of the Vodafone Group Services Ltd nor the names of its contributors may be used to endorse or promote products derived from this software without specific prior written permission.

THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS" AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE ARE DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT HOLDER OR CONTRIBUTORS BE LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
*/


#include "StackedSet.h"
#include "OverlayItemInternal.h"
#include "MapProjection/MapProjection.h"
#include "OverlayItemZoomSpec.h"
#include "OverlayViewLayerInterface.h"
#include "OverlayItemVisualSpec.h"
#include "ImageSpec.h"
#include "PrecisionScalingConstants.h"
#include "OverlayItemUtil.h"
#include "MapProjection/MapProjectionAdapter.h"

// To get the distance between two coordinates
#include "GfxUtility.h"


StackedSet::StackedSet(MapProjectionAdapter& projection,
                       OverlayViewLayerInterface& layerInterface)
   : m_projection(projection),
     m_layerInterface(layerInterface),
     m_automaticHighlightMode(false)
{
   m_closest.item = NULL;
}


void StackedSet::initItem(OverlayItemInternal* item) {
   // Clears the stack of the item.
   item->m_stack.clearStack();
   // We need to reset the adjusted position, since
   // calculations will be made using this position.
   item->m_adjustedPosition = item->getPosition();

   item->m_isHighlighted = false;
   // Sets the item to its normal state.

   WFAPI::wf_uint32 curPixelScale = static_cast<WFAPI::wf_uint32>(
      m_projection.getPixelScale()) ;
   const WFAPI::OverlayItemVisualSpec* visualSpec =
      OverlayItemUtil::getCurrentVisualSpec(item, curPixelScale);
   if(!visualSpec) {
      return;
   } 
   updateItemPixelBox(item, visualSpec, item->getPosition());
}

void StackedSet::addToStackSet(OverlayItemInternal* item)
{
   // If the item is unstackable, we just add it to the set.

   if (!item->isStackable()){
      m_stacks.push_back(item);
      return;
   }
   bool overlapping = false;
   
   StackVec::iterator it = m_stacks.begin();
   StackVec::iterator end = m_stacks.end();

   PixelBox itemBox(item->getPrecisionScaledPixelBox());
   
   while (it != end && !overlapping) {
      // If an overlap is found, the item will be added to an existing
      // stack and the while loop will terminate.

      OverlayItemStackInfo& otherStack = (*it)->m_stack;
      PixelBox otherBox((*it)->getPrecisionScaledPixelBox());
         
      if ((*it)->isStackable() && itemBox.overlaps(otherBox)) {
         overlapping = true;


         // We add the item to the stack of an existing item.
         otherStack.addToStack(item);

         if (otherStack.getSize() == 2) {
            // This means that the old item was not previously stacked,
            // and now we have to update its pixelbox according to
            // its stacked specification.
            
            WFAPI::wf_uint32 curPixelScale = static_cast<WFAPI::wf_uint32>(
               m_projection.getPixelScale()) ;
            const WFAPI::OverlayItemVisualSpec* visualSpec =
               OverlayItemUtil::getCurrentVisualSpec((*it), curPixelScale);
            
            updateItemPixelBox((*it),
                               visualSpec,
                               (*it)->getPosition());            
         }

      }
      it++;
   }

   if(!overlapping) {
      // If no overlap was found, create a new stack from the item.
      m_stacks.push_back(item);
   }
}

void StackedSet::smoothPositions()
{
   for (StackVec::iterator itr = m_stacks.begin(); itr != m_stacks.end();
      itr++)
   {
      const OverlayItemInternal* curItem = *itr;
      
      MC2Point average(0, 0);
      int numItems = 0;
      
      while (curItem) {
         // average will contain the sum of all items positions,
         // it will later we divided by the number of items.
         average = average +
            getPrecisionScaledScreenPosition(curItem);
         
         curItem = curItem->m_stack.getNext();
         numItems++;
      }

      // After these divisions average will actually contain the average
      // position (in precision scaled dimensions).
      average.getX() /= numItems;
      average.getY() /= numItems;

      curItem = *itr;
      
      while (curItem) {
         // Use the offset to move pixel box.
         MC2Point offset = average -
            getPrecisionScaledScreenPosition(curItem);
         curItem->getPrecisionScaledPixelBox().move(offset);
         
         // Use the offset to move the world position.
         MC2Point tmpPoint;
         MC2Coordinate tmpCoord(curItem->m_adjustedPosition);

         WFAPI::wf_float64 x;
         WFAPI::wf_float64 y;
         
         // We get the screen position for the coordinate.
         m_projection.transformf(x,y, tmpCoord);
         // We scale the screen position into precision scaled dimensions.
         tmpPoint = MC2Point(static_cast<int>(x * SCALING_CONSTANT),
                             static_cast<int>(y * SCALING_CONSTANT));
         tmpPoint += offset;

         // We have now offset the position and want to translate it
         // back to a world coordinate. Thus we need to scale it back
         // to normal screen space.
         tmpPoint.getX() /= SCALING_CONSTANT;
         tmpPoint.getY() /= SCALING_CONSTANT;
         
         // Get the corresponding world coordinate.
         m_projection.inverseTransform(tmpCoord, tmpPoint);
         
         curItem->m_adjustedPosition = tmpCoord;
         
         // Move to next item
         curItem = curItem->m_stack.getNext();
      }

   }
}

StackedSet::StackVec& StackedSet::getStackedItems()
{
   return m_stacks; 
}

void StackedSet::clear()
{
   m_stacks.clear(); 
}

// #define USE_SMOOTHING
// #define USE_ITERATIVE_SMOOTHING
// #define ITERATIVE_SMOOTHING_CAP 2

void StackedSet::update()
{
   clear();

   // Since overlap should be calculated using the precision scaled dimensions
   // we need to precision scale the bounding box for the screen.
   PixelBox screenBox = m_projection.getPixelBoundingBox();

   MC2Point topLeft(screenBox.getTopLeft().getX() * SCALING_CONSTANT,
                    screenBox.getTopLeft().getY() * SCALING_CONSTANT);
   MC2Point bottomRight(screenBox.getBottomRight().getX() * SCALING_CONSTANT,
                        screenBox.getBottomRight().getY() * SCALING_CONSTANT);
   PixelBox screenBoxScaled(topLeft, bottomRight);
   
   for( OverlayViewLayerInterface::LayerMap::iterator itr =
           m_layerInterface.begin();
        itr != m_layerInterface.end(); itr++)
   {
      Layer& l = *itr->second;

      if(!l.getVisible()) {
         // We don't need to detect any overlaps with invisible layers.
         continue;
      }

      for(Layer::const_iterator itemItr = l.items_begin();
          itemItr != l.items_end(); itemItr++)
      {
         OverlayItemInternal* item = *itemItr;
         
         initItem(item);
         
         if (!screenBoxScaled.overlaps(item->getPrecisionScaledPixelBox())) {
            // The item lies outside the screen, do NOT add it to the stacks.
            continue;
         }
         
         addToStackSet(item);
      }
   }

   
#ifdef USE_SMOOTHING
   // Smooth the positions of the stacks to represent the average
   // of the contained items' positions.
   smoothPositions();
#endif 
   
#ifdef USE_SMOOTHING
#ifdef USE_ITERATIVE_SMOOTHING
   // Since we have changed the position of the stacks, we run another
   // pass of overlap detection.
   unsigned int prevSize = 0; 
   unsigned int iterationCap = ITERATIVE_SMOOTHING_CAP;

   // If the number of stacks has not changed, no stacks overlapped
   // so the iteration should terminate.
   while (prevSize != m_stacks.size() && iterationCap > 0) {
      StackVec tempVec = m_stacks;
      prevSize = m_stacks.size();

      clear();
   
      for(StackVec::iterator itr = tempVec.begin();
          itr != tempVec.end(); itr++) {
         addToStackSet(*itr);
         
      }
   
      smoothPositions();
      --iterationCap;
   }
#endif
#endif
   
   OverlayItemInternal* currClosest = NULL;
   if ( m_stacks.size() != 0 && m_automaticHighlightMode){
      // Find out which item is closest to the center of the screen
      
      MC2Point screenCenter;
      m_projection.transform(screenCenter, m_projection.getCenter());
   
      WFAPI::wf_float64 closestDistSq = 0;

      currClosest = *m_stacks.begin();
      closestDistSq = getSquareScreenDistance(screenCenter, currClosest);
         
      for(StackedSet::StackVec::const_iterator itr = m_stacks.begin();
          itr != m_stacks.end(); itr++)
      {
         WFAPI::wf_float64 currDistSq = getSquareScreenDistance(screenCenter,
                                                                *itr);
         if (currDistSq < closestDistSq){
            currClosest = *itr;
            closestDistSq = currDistSq;
         }
      }
      // If the currently closest item is further away than RADIUS pixels,
      // disregard it.
      const unsigned int RADIUS = 75;
      if (closestDistSq > RADIUS * RADIUS){
         currClosest = NULL;
         closestDistSq = 0;
      }
   }
   updateClosest(currClosest);
   
   if (m_closest.item && automaticHighlightEnabled()){
      m_closest.item->m_isHighlighted = true;
      // Since we don't care pixel scale the item is on, we
      // use 0 as the current pixel scale.
      const WFAPI::OverlayItemVisualSpec* highlighted =
         OverlayItemUtil::getCurrentVisualSpec(m_closest.item, 0);
      
      if(highlighted) {
         updateItemPixelBox(m_closest.item, highlighted,
                            m_closest.item->m_adjustedPosition);
      }
   }
   
}



void
StackedSet::updateItemPixelBox(const OverlayItemInternal* item,
                               const WFAPI::OverlayItemVisualSpec* visualSpec,
                               const WFAPI::WGS84Coordinate& coord)
{
   MC2Point screenPoint;

   WFAPI::wf_float64 x;
   WFAPI::wf_float64 y;
   m_projection.transformf(x,y, coord);
   screenPoint = MC2Point(static_cast<int>(x * SCALING_CONSTANT),
                          static_cast<int>(y * SCALING_CONSTANT));

   if(!visualSpec) {
      item->getPrecisionScaledPixelBox() = PixelBox(MC2Point(0, 0),
                                                    MC2Point(0, 0));
      return;
   }
   
   const WFAPI::ImageSpec* background = visualSpec->getBackgroundImage();

   if(!background) {
      item->getPrecisionScaledPixelBox() = PixelBox(MC2Point(0, 0),
                                                    MC2Point(0, 0));
      return;
   }
   
   WFAPI::ScreenPoint offsetPoint = visualSpec->getFocusPoint();
   
   // The pixel box needs to be scaled in order to gain sufficient
   // precision.
   offsetPoint.getX() *= SCALING_CONSTANT;
   offsetPoint.getY() *= SCALING_CONSTANT;
   unsigned int width = background->getWidth() * SCALING_CONSTANT;
   unsigned int height = background->getHeight() * SCALING_CONSTANT;

   // Create our offset box
   PixelBox box( MC2Point(-offsetPoint.getX(), -offsetPoint.getY()),
                 MC2Point(-offsetPoint.getX() + width,
                          -offsetPoint.getY() + height));

   //Offset the box to the correct position on the screen
   box.move(screenPoint);

   //Assign pixel box to item so that other items can check for overlap
   item->getPrecisionScaledPixelBox() = box;    
}

void StackedSet::updateClosestItemPoint()
{
   if (m_closest.item == NULL){
      return;
   }
   MC2Point screenPoint;
   m_projection.transform(screenPoint,
                          m_closest.item->m_adjustedPosition);
   m_closest.point = screenPoint;
}

MC2Point StackedSet::getClosestItemPoint()
{
   if(m_closest.item == NULL){
      return MC2Point(0,0);
   } else {
      return m_closest.point;
   }
}

MC2Coordinate StackedSet::getClosestItemCoord()
{
   if(m_closest.item == NULL){
      return MC2Coordinate(0,0);
   } else {
      return m_closest.item->m_adjustedPosition; 
   }
}

bool StackedSet::hasValidClosestItem()
{
   return m_closest.item != NULL; 
}

void StackedSet::updateClosest(const OverlayItemInternal* newClosest)
{
   if (newClosest == NULL){
      // Clear the m_closest struct
      m_closest.item = NULL;
      m_closest.stack.clear();
      m_closest.point = WFAPI::ScreenPoint();
      return;
   } 

   // To determine whether the m_closest struct should be updated or not,
   // we examine the pointer itself, the size of it's stack - and if
   // both are the same, we examine the contents of the stack.
   bool shouldUpdate = false;
   if (newClosest != m_closest.item ||
       m_closest.stack.size() != newClosest->m_stack.getSize()-1){
      // Either the pointer differs, or the size of the stack.
      shouldUpdate = true;
   } else {
      // The pointer and the size is the same as previously,
      // so we examine the contents of the stack.
      const OverlayItemInternal* cur = newClosest->m_stack.getNext();
      unsigned int curIndex = 0;
      while (cur && !shouldUpdate){
         if (cur != m_closest.stack[curIndex]){
            shouldUpdate = true;
         }
         cur = cur->m_stack.getNext();
         curIndex++;
      }
   }

   if (!shouldUpdate){
      return;
   } else {
      // Now we update the m_closest struct.
      m_closest.item = newClosest;
      m_closest.stack.clear();
      const OverlayItemInternal* cur = newClosest->m_stack.getNext();
      while (cur){
         m_closest.stack.push_back(cur);
         cur = cur->m_stack.getNext();
      }
      // This call sets the m_closest.point correctly for the current
      // projection state.
      updateClosestItemPoint();
   }
   
}


WFAPI::wf_float64
StackedSet::getSquareScreenDistance(MC2Point screenCenter,
                                    const OverlayItemInternal* item)
{
   MC2Point screenItem;
   m_projection.transform(screenItem, item->m_adjustedPosition);
   
   return (screenItem.getX() - screenCenter.getX()) *
      (screenItem.getX() - screenCenter.getX()) +
      (screenItem.getY() - screenCenter.getY()) *
      (screenItem.getY() - screenCenter.getY());
   

}

MC2Point
StackedSet::getPrecisionScaledScreenPosition(const OverlayItemInternal* item)
{
   WFAPI::wf_float64 x;
   WFAPI::wf_float64 y;
         
   // We get the screen position for the coordinate.
   m_projection.transformf(x,y, item->m_adjustedPosition);
   // We scale the screen position into precision scaled dimensions.
   return MC2Point(static_cast<int>(x * SCALING_CONSTANT),
                   static_cast<int>(y * SCALING_CONSTANT));

   // return item->getPrecisionScaledPixelBox().getTopLeft();
}

void StackedSet::enableAutomaticHighlight(bool enable)
{
   m_automaticHighlightMode = enable;
   if (!enable){
      updateClosest(NULL);
   }
   updateClosestItemPoint();

}

bool StackedSet::automaticHighlightEnabled()
{
   return m_automaticHighlightMode;
}

const OverlayItemInternal* StackedSet::getClosestItem()
{
   return m_closest.item;
}

