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
#include "StackedDialog.h"
#include "OverlayItemVisualSpec.h"
#include "ScreenPoint.h"
#include "ImageSpec.h"
#include "MapProjection/MapProjection.h"
#include "MC2Point.h"
#include "OverlayItemUtil.h"
#include "OverlayItemInternal.h"
#include "OverlayDrawing.h"
#include "SelectedMapObjectNotifier.h"
#include "OverlayView.h"
#include "OverlayDrawer.h"
#include "MapDrawingInterface.h"
#include "Utility.h"
#include "MapDrawingInterfaceInternal.h"
#include "DebugDrawing.h"
#include "ExternalRenderer.h"
#include "MapProjection/MapProjectionAdapter.h"
#include <map>



/// The period of the timer, governs how often flicker animation is updated
#define TIMER_PERIOD_MS 32

/// If a non-exponential flicking deceleration is used, this constant
/// governs how long it will take for the animation to cease.
#define FLICKING_ANIMATION_TIME 0.600f

/// The value that governs when the exponential deceleration should cease
#define EXPONENTIAL_MIN 1.0f
/// The factor that the exponential deceleration is multiplied with on each pass
#define EXPONENTIAL_FACTOR 0.84f

/// Define this to use exponential deceleration
//#define USE_EXPONENTIAL_DECELERATION

namespace WFAPI {
class WFString;
}

StackedDialog::StackedDialog(OverlayView* view,
                             TileMapToolkit* toolkit,
                             MapDrawingInterfaceInternal* mapDrawingInterface,
                             const MapProjectionAdapter& projection) :
   m_view(view),
   m_mapDrawingInterface(mapDrawingInterface),
   m_projection(projection),
   m_item(NULL)
{
   RectWrappingState r = { 0 };
   m_rectState = r;

   m_flicking.curVelPxPs = 0.0f;

   m_timerListener  =
      new TileMapTimerListenerTemplate<StackedDialog>(this);

   m_timer =
      toolkit->createPeriodicTimer(m_timerListener,
                                   TileMapToolkit::PRIO_STANDARD);
}

StackedDialog::~StackedDialog()
{
   SpecMap::iterator itr = m_dialogSpecs.begin();
   SpecMap::iterator end = m_dialogSpecs.end();
   // Remove references on all the visual specifications
   // in the map.
   while (itr != end){
      itr->second->removeReference();
      itr++;
   }

   delete m_timer;
   delete m_timerListener;
}

void
StackedDialog::setVisualSpec(
   const WFAPI::StackedDialogVisualSpec* dialogVisualSpec,
   unsigned int numberOfItems)
{
   if(dialogVisualSpec) {
      m_dialogSpecs[numberOfItems] = dialogVisualSpec;
      m_dialogSpecs[numberOfItems]->addReference();
      // We will need a geometry state for every dialog specification.
      setupGeometry(numberOfItems);      
   }
}

bool StackedDialog::isVisible(const MC2Point& point,
                              const PixelBox& pixelBox)
{
   int curSpecIndex = getItemSpecIndex(m_item);
   // We allow drawing of partial items, hence we offset the
   // left-side position to account for an entire items width
   int leftSideX =
      pixelBox.getTopLeft().getX() - m_geometry[curSpecIndex].itemWidth;
   int rightSideX = pixelBox.getTopRight().getX();

   return
      point.getX() <= rightSideX && point.getX() >= leftSideX;
}

// extern isab::MapPlotter* g_plotter;

void StackedDialog::draw(OverlayDrawer* drawer)
{   
   int curSpecIndex = getItemSpecIndex(m_item);
   if (curSpecIndex==-1){
      return;
   }

   const WFAPI::StackedDialogVisualSpec* curSpec = m_dialogSpecs[curSpecIndex];

   if(!curSpec) {
      return;
   }
   
   MC2Point dialogPos;
   m_projection.transform(dialogPos, m_position);

   WFAPI::ScreenPoint dialogFocusPoint =
      curSpec->getFocusPoint();

   dialogPos.getX() -= dialogFocusPoint.getX();
   dialogPos.getY() -= dialogFocusPoint.getY();
   WFAPI::ScreenPoint offset = dialogPos;

   // Start by drawing the background image of the dialog
   drawer->drawImageSpec(
      curSpec->getDialogBackgroundImage(), dialogPos);
   
   // Draw the arrows for the dialog
   int curStackSize = static_cast<int>(m_item->m_stack.getSize());

   const WFAPI::ImageSpec* leftArrowImage =
      curSpec->getLeftArrowImage();
   
   // Only draw the arrow if it has a visual spec, and the current
   // set of items is larger than the visible amount.
   if (leftArrowImage && curStackSize > curSpecIndex){
      WFAPI::ScreenPoint leftPos =
         curSpec->getLeftArrowCenterPos();
      leftPos.getX() -= leftArrowImage->getWidth() / 2;
      leftPos.getX() += offset.getX();
      leftPos.getY() -= leftArrowImage->getHeight() / 2;
      leftPos.getY() += offset.getY();

      drawer->drawImageSpec(leftArrowImage, leftPos);
      
   }
   
   const WFAPI::ImageSpec* rightArrowImage =
      curSpec->getRightArrowImage();
   
   // Only draw the arrow if it has a visual spec, and the current
   // set of items is larger than the visible amount.   
   if(rightArrowImage && curStackSize > curSpecIndex){
      WFAPI::ScreenPoint rightPos =
         curSpec->getRightArrowCenterPos();
      rightPos.getX() -= rightArrowImage->getWidth() / 2;
      rightPos.getX() += offset.getX();
      rightPos.getY() -= rightArrowImage->getHeight() / 2;
      rightPos.getY() += offset.getY();

      drawer->drawImageSpec(rightArrowImage, rightPos);

   }

   WFAPI::ScreenPoint itemsUpperLeft(
      curSpec->getItemsUpperLeftPoint());
   WFAPI::ScreenPoint itemsLowerRight(
      curSpec->getItemsLowerRightPoint());

   PixelBox p(itemsUpperLeft, itemsLowerRight);

   p.move(offset);

   // DebugDrawing::pixelBox(p, (isab::MapPlotter*)g_plotter);
   
   offset.getX() += itemsUpperLeft.getX();
   offset.getY() += itemsUpperLeft.getY();

   isab::Rectangle r(offset.getX(), offset.getY(),
                     m_geometry[curSpecIndex].itemDialogWidth,
                     m_geometry[curSpecIndex].itemDialogHeight);
   
   // DebugDrawing::rectangle(r, (isab::MapPlotter*)drawer);
   // DebugDrawing::rectangle(p, (isab::MapPlotter*)drawer);
   
   drawer->setClippingRectangle(offset.getX(),
                                 offset.getY(),
                                 m_geometry[curSpecIndex].itemDialogWidth,
                                 m_geometry[curSpecIndex].itemDialogHeight);

   const OverlayItemInternal* curItem = m_item;
   
   for (unsigned int i = 0; i < m_itemPositions.size(); i++) {
      MC2Point posLeft = m_itemPositions[i] + MC2Point(m_rectState.leftX, 0);
      MC2Point posRight = m_itemPositions[i] + MC2Point(m_rectState.rightX, 0);

      posLeft += MC2Point(offset);
      posRight += MC2Point(offset);

      const WFAPI::MapObjectInfo& objInfo = curItem->getMapObjectInfo();
      const WFAPI::WFString& itemName = objInfo.getName();
      const WFAPI::OverlayItemVisualSpec* itemVisualSpec;
      if (curItem->m_isBeingTapped){
         itemVisualSpec = curSpec->getTappedItemVisualSpec();
      } else {
         itemVisualSpec = curSpec->getItemVisualSpec();
      }
      if(isVisible(posLeft, p)) {

         
         // Draw item inside left rectangle
         OverlayDrawing::drawOverlayItem(
            curItem,
            itemVisualSpec,
            itemName,
            "",
            posLeft,
            drawer);

         // DebugDrawing::point(posLeft, (isab::MapPlotter*)drawer);

      } else if(isVisible(posRight, p)) {

         
         // Draw item inside right rectangle
         OverlayDrawing::drawOverlayItem(
            curItem,
            itemVisualSpec,
            itemName,
            "",
            posRight,
            drawer);
         
         // DebugDrawing::point(posRight, (isab::MapPlotter*)drawer);

      } else {
         // Simply continue
      }

      curItem = curItem->m_stack.getNext();
   }

   drawer->resetClippingRectangle();
}

bool StackedDialog::updatePositionsDelta(int deltaX) {
   if(m_itemPositions.empty()) {
      return false;
   }

   m_rectState.leftX += deltaX;
   m_rectState.rightX += deltaX;

   /*                                                    |
    *                                                    |
    *   +-----------------------++-----------------------+
    *   |                       ||                       |
    *   |                       ||                       |
    *   |                       ||                       |
    *   +-----------------------++-----------------------+
    *   +                                                | 0
    *                                                    |
    *   |   
    *   |
    *   +-----------------------++-----------------------+
    *   |                       ||                       |
    *   |                       ||                       |
    *   +-----------------------++-----------------------+
    *   |
    *   + 0
    *
    *   The two imaginary rectangles should wrap around and be
    *   swapped when:
    *
    *   1) The left rectangle is completely to the left of 0.
    *   2) The right rectangle is completely to the right of 0.
    */
   if(m_rectState.rightX < 0) {
      m_rectState.leftX += m_rectState.width * 2;
      std::swap(m_rectState.leftX, m_rectState.rightX);
   } else if(m_rectState.leftX > 0) {
      m_rectState.rightX -= m_rectState.width * 2;
      std::swap(m_rectState.leftX, m_rectState.rightX);
   }

   return deltaX != 0;
}

int
StackedDialog::getItemSpecIndex(const OverlayItemInternal* item) const{
   unsigned int stackSize = item->m_stack.getSize();
   if (stackSize == 1) {
      
      // We should not show unstacked items in a dialog.
       return -1;
   }
   SpecMap::const_iterator it = m_dialogSpecs.begin();
   SpecMap::const_iterator end = m_dialogSpecs.end();
   SpecMap::const_iterator prev = it;
   
   if (it==end){
      return -1;
   }
   // We want to return the index for the largest spec that can contain
   // all the items in the stack without any empty gaps.
   while(it != end){
      if (stackSize >= prev->first && stackSize < it->first){
         break;
      } else {
         prev = it;
         it++;
      }
   }
   return prev->first;
}

#define PR(val) std::cout << #val << " : " << val << std::endl;

void StackedDialog::updatePositions()
{
   const OverlayItemInternal* curItem = m_item;
   MC2Point curPos(0, 0);

   int curItemSpecIndex = getItemSpecIndex(curItem);
   if (curItemSpecIndex == -1){
      // No item specifications where found.
      return;
   }
   
   unsigned int index = 0;

   m_itemPositions.clear();

   OverlayItemUtil::verifyStack(curItem);

   unsigned int numItems = m_item->m_stack.getSize();

   // PR(m_geometry[curItemSpecIndex].itemSpacing);
   // PR(m_geometry[curItemSpecIndex].itemWidth);
   
   // We have
   m_rectState.width =
      static_cast<int>((numItems) *
                       m_geometry[curItemSpecIndex].itemSpacing);
   m_rectState.leftX = 0;
   m_rectState.rightX = m_rectState.width;

   while(curItem) {
      curPos.getX() = index * m_geometry[curItemSpecIndex].itemSpacing;
      curPos.getY() = m_geometry[curItemSpecIndex].itemPosY;

      m_itemPositions.push_back(curPos);

      curItem = curItem->m_stack.getNext();
      index++;
   }

   m_geometry[curItemSpecIndex].lowerBoundX =
      1 * m_geometry[curItemSpecIndex].itemSpacing; 

   m_geometry[curItemSpecIndex].upperBoundX =
      (curItemSpecIndex - 2) * m_geometry[curItemSpecIndex].itemSpacing;
}

void StackedDialog::enableDialog(OverlayItemInternal* item)
{
   m_position = item->m_adjustedPosition;

   m_item = item;

   setupItemOffset();
   updatePositions();
   m_view->setStateChanged();
   m_mapDrawingInterface->requestRepaint();
}

PixelBox StackedDialog::getPixelBox()
{
   MC2Point dialogPos;
   int curItemSpecIndex = getItemSpecIndex(m_item);
   if (curItemSpecIndex == -1){
      return PixelBox();
   }
   m_projection.transform(dialogPos, m_position);
   WFAPI::ScreenPoint offsetPoint =
      m_dialogSpecs[curItemSpecIndex]->getFocusPoint();

   const WFAPI::ImageSpec* itemBackground =
      m_dialogSpecs[curItemSpecIndex]->getDialogBackgroundImage();

   int width = itemBackground->getWidth();
   int height = itemBackground->getHeight();

   PixelBox box( MC2Point(-offsetPoint.getX(), -offsetPoint.getY()),
                 MC2Point(-offsetPoint.getX() + width,
                          -offsetPoint.getY() + height) );

   //Offset the box to the correct position on the screen
   box.move(dialogPos);

   return box;
}

bool StackedDialog::scroll(int deltaX)
{
   int curItemIndex = getItemSpecIndex(m_item);
   if (static_cast<int>(m_item->m_stack.getSize()) == curItemIndex){
      // If the size of the stack matches the amount of visible
      // items, we don't want the dialog to be scrollable.
      return false;
   }

   if(updatePositionsDelta(deltaX)) {
      m_view->setStateChanged();
      m_mapDrawingInterface->repaintOverlayDialogs();
      return true;
   } else {
      return false;
   }
}

bool StackedDialog::clickOverlapsItem(int itemPosX,
                                      const MC2Point& clickPosition)
{

   int itemSpecIndex = getItemSpecIndex(m_item);
   if (itemSpecIndex==-1){
      // No specs where found.
      return false;
   }
   
   MC2Point dialogPos;
   m_projection.transform(dialogPos, m_position);

   MC2Point leftPos =
      MC2Point(itemPosX + m_rectState.leftX, 0) +
      dialogPos - m_dialogSpecs[itemSpecIndex]->getFocusPoint()
      + m_dialogSpecs[itemSpecIndex]->getItemsUpperLeftPoint();

   MC2Point rightPos =
      MC2Point(itemPosX + m_rectState.rightX, 0) +
      dialogPos - m_dialogSpecs[itemSpecIndex]->getFocusPoint()
      + m_dialogSpecs[itemSpecIndex]->getItemsUpperLeftPoint();

   PixelBox pBox(MC2Point(0, 0),
                 MC2Point(m_geometry[itemSpecIndex].itemWidth,
                          m_geometry[itemSpecIndex].itemHeight));

   pBox.move(leftPos);

   if(pBox.pointInside(clickPosition)) {
      return true;
   }

   pBox.move(MC2Point(0,0) - leftPos);
   pBox.move(rightPos);

   if(pBox.pointInside(clickPosition)) {
      return true;
   } else {
      return false;
   }
}

const WFAPI::OverlayItem*
StackedDialog::getSelectedItem(const WFAPI::ScreenPoint& point)
{
   const OverlayItemInternal* curItem = m_item;

   for(unsigned int i = 0; i < m_itemPositions.size(); i++) {

      if(clickOverlapsItem(m_itemPositions[i].getX(), point)) {
         return curItem;
      }

      curItem = curItem->m_stack.getNext();
   }

   return NULL;
}

void StackedDialog::setupGeometry(unsigned int numOfItems)
{

   const WFAPI::OverlayItemVisualSpec* visualSpec =
      m_dialogSpecs[numOfItems]->getItemVisualSpec();

   const WFAPI::ImageSpec* dialogBackground =
      m_dialogSpecs[numOfItems]->getDialogBackgroundImage();

   const WFAPI::ImageSpec* itemBackground = visualSpec->getBackgroundImage();

   MC2Point upperLeft(m_dialogSpecs[numOfItems]->getItemsUpperLeftPoint());
   MC2Point lowerRight(m_dialogSpecs[numOfItems]->getItemsLowerRightPoint());
   
   m_geometry[numOfItems].dialogWidth = dialogBackground->getWidth();
   m_geometry[numOfItems].dialogHeight = dialogBackground->getHeight();
   m_geometry[numOfItems].itemDialogWidth = lowerRight.getX() - upperLeft.getX();
   m_geometry[numOfItems].itemDialogHeight = lowerRight.getY() - upperLeft.getY();
   m_geometry[numOfItems].itemPosY =
      m_geometry[numOfItems].itemDialogHeight / 2
      - itemBackground->getHeight() / 2;
   m_geometry[numOfItems].itemWidth = itemBackground->getWidth();
   m_geometry[numOfItems].itemHeight = itemBackground->getHeight();

   m_geometry[numOfItems].itemSpacing = 0;
}

void StackedDialog::setupItemOffset()
{
   // The item spacing is based on the number of items to display,
   // which is equal to the index of the current specification.
   int itemSpecIndex = getItemSpecIndex(m_item);
   unsigned int numItems = itemSpecIndex;

   m_geometry[itemSpecIndex].itemSpacing =
      m_geometry[itemSpecIndex].itemDialogWidth / numItems;
}

void StackedDialog::startFlicking(float initialVelocityPxPs)
{
   int curItemIndex = getItemSpecIndex(m_item);
   if (static_cast<int>(m_item->m_stack.getSize()) == curItemIndex){
      // If the size of the stack matches the amount of visible
      // items, we don't want the dialog to be scrollable.
      return;
   }

   static const float FREQUENCY_TICKS_PER_SEC = 1000.0f / TIMER_PERIOD_MS;

   m_flicking.curVelPxPs = initialVelocityPxPs;
   m_flicking.decelerationConstant =
      (initialVelocityPxPs / FLICKING_ANIMATION_TIME) / FREQUENCY_TICKS_PER_SEC;

   m_flicking.lastTime = TileMapUtil::currentTimeMillis();

   if(m_timer->active()) {
      m_timer->stop();
   }

   m_timer->start(TIMER_PERIOD_MS);
}

void StackedDialog::disableDialog()
{
   // std::cout << "Disabling stacked dialog" << std::endl;
   endFlicking();
   m_view->setStateChanged();
   m_mapDrawingInterface->repaintNow();
}

void StackedDialog::endFlicking()
{
   m_flicking.curVelPxPs = 0.0f;
   m_timer->stop();
}

void StackedDialog::timerExpired(uint32)
{
   unsigned int now = TileMapUtil::currentTimeMillis();
   unsigned int elapsedMs = now - m_flicking.lastTime;

   float inc = m_flicking.curVelPxPs * (elapsedMs) / 1000.0f;

#ifdef USE_EXPONENTIAL_DECELERATION
   m_flicking.curVelPxPs*=EXPONENTIAL_FACTOR;
   if(fabs(m_flicking.curVelPxPs) < EXPONENTIAL_MIN) {
      m_flicking.curVelPxPs = 0.0f;
      endFlicking();
   }
#else
   if(fabs(m_flicking.curVelPxPs) < fabs(m_flicking.decelerationConstant)) {
      m_flicking.curVelPxPs = 0.0f;
      endFlicking();
   } else {
      m_flicking.curVelPxPs-=m_flicking.decelerationConstant;
   }
#endif

   if(!scroll(static_cast<int>(inc))) {
      endFlicking();
   } else {

   }

   m_flicking.lastTime = now;
}

bool StackedDialog::isFlicking()
{
   return m_timer->active();
}

float StackedDialog::currentFlickingSpeedPxPs()
{
   return m_flicking.curVelPxPs;
}

void StackedDialog::drawPartial(OverlayDrawer* drawer)
{
   int curSpecIndex = getItemSpecIndex(m_item);
   if (curSpecIndex==-1 || m_dialogSpecs[curSpecIndex] == NULL){
      return;
   }

   MC2Point dialogPos;
   m_projection.transform(dialogPos, m_position);

   WFAPI::ScreenPoint dialogFocusPoint =
   m_dialogSpecs[curSpecIndex]->getFocusPoint();

   dialogPos.getX() -= dialogFocusPoint.getX();
   dialogPos.getY() -= dialogFocusPoint.getY();

   WFAPI::ScreenPoint offset = dialogPos;
   WFAPI::ScreenPoint itemsUpperLeft(
      m_dialogSpecs[curSpecIndex]->getItemsUpperLeftPoint());

   offset.getX() += itemsUpperLeft.getX();
   offset.getY() += itemsUpperLeft.getY();

   drawer->setClippingRectangle(offset.getX(),
                                offset.getY(),
                                m_geometry[curSpecIndex].itemDialogWidth,
                                m_geometry[curSpecIndex].itemDialogHeight);
   
   draw(drawer);

   drawer->resetClippingRectangle(); 
}

bool StackedDialog::isValidFor(const OverlayItemInternal* item)
{
   return getItemSpecIndex(item) != -1; 
}

