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
#include "OverlayViewInputInterface.h"
#include "OverlayView.h"
#include "SelectedMapObjectNotifier.h"
#include "MapDrawingInterface.h"
#include "SmoothMapManipulator.h"
#include "Utility.h"
#include "TileMapToolkit.h"
#include "CommonData.h"

#define CORE_LOGGING_MODULE_NAME "OverlayViewInputInterface"
//#define CORE_LOGGING_MODULE_LEVEL CL_ALL_FILE

#include "CoreDebugPrint.h"

using namespace WFAPI;

OverlayViewInputInterface::
OverlayViewInputInterface(OverlayView* view,
                          SelectedMapObjectNotifier* notifier,
                          SmoothMapManipulator& smoothManipulator,
                          AnimatedMapManipulator& animator) :
   m_currentlyBeingTapped(NULL),
   m_smoothManipulator(smoothManipulator),
   m_animator(animator)
{
   m_view = view;
   m_notifier = notifier;
   m_nonDialogPointerState.prevPoint = WFAPI::ScreenPoint(0, 0);
   m_nonDialogPointerState.accumulatedDiff = 0;
   
   m_dialogPointerState.initialized = false;
   m_dialogPointerState.scrolled = false;
   m_dialogPointerState.prevPoint = WFAPI::ScreenPoint(0, 0);
   m_dialogPointerState.prevTime = 0;
   m_dialogPointerState.item = NULL;
   m_dialogPointerState.tracking = false;
}

OverlayViewInputInterface::~OverlayViewInputInterface()
{

}

bool
OverlayViewInputInterface::handleKeyEvent(
   MapLibKeyInterface::keyType key,
   MapLibKeyInterface::kindOfPressType type)
{
   // We do not deal with key events. However, if we are in
   // dialog interaction mode, we want to suppress the input
   // to normal interaction.

   switch(m_view->getOverlayState()) {
   case OverlayView::DIALOG_INTERACTION:
      return true;
      break;
   case OverlayView::MAP_INTERACTION:
      return false;
      break;
   default:
      return false;
   }
}

bool
OverlayViewInputInterface::handleDialogPointerEvent(
   MapLibKeyInterface::kindOfPointerType type,
   const WFAPI::ScreenPoint& point,
   bool longPress)
{
   if(!m_dialogPointerState.tracking &&
      type != MapLibKeyInterface::POINTER_DOWN_EVENT)
   {
      return true;
   }
   
   static unsigned int prevTime = TileMapUtil::currentTimeMillis();
   
   unsigned int curTime = TileMapUtil::currentTimeMillis();

   StackedDialog& dialog = m_view->getStackedDialog();

   if(!m_dialogPointerState.initialized) {
      m_dialogPointerState.initialized = true;
      m_dialogPointerState.prevPoint = point;
      coreprintln("Resetting scrolled");
      m_dialogPointerState.scrolled = false;
      m_dialogPointerState.tracking = false;
   }

   
   switch(type) {
   case MapLibKeyInterface::POINTER_DOWN_EVENT: {      
      // If we are not inside the bounding box of the dialog, we should
      // revert into normal MAP_INTERACTION mode.
      PixelBox box = dialog.getPixelBox();

      m_dialogPointerState.tracking = true;
      
      if(!box.pointInside(point)) {
         m_view->setOverlayState(OverlayView::MAP_INTERACTION);
         handleMapPointerEvent(type, point, false);
         return false;
      }
      
      const OverlayItemInternal* item =
         static_cast<const OverlayItemInternal*>(
            dialog.getSelectedItem(point));
         
      setCurrentlyBeingTapped(item);
      
      coreprintln("POINTER_DOWN_EVENT - [%d, %d] @ DT = %d ms",
                  point.getX(), point.getY(),
                  curTime - prevTime);

      m_dialogPointerState.initialPoint = point;
      m_dialogPointerState.prevPoint = point;
   }
      break;
   case MapLibKeyInterface::POINTER_UPDATE_EVENT: {

      int deltaX = point.getX() - m_dialogPointerState.prevPoint.getX();

      dialog.scroll(deltaX);

      int distOrigPointX = point.getX() - m_dialogPointerState.initialPoint.getX();

      if(abs(distOrigPointX) > MAX_POINTER_DEVIATION_PX)
      {
         coreprintln("Setting scrolled, deviation = %d", distOrigPointX);
         m_dialogPointerState.scrolled = true;
         // setCurrentlyBeingTapped(NULL);
      }

      m_dialogPointerState.prevPoint = point;
      
      coreprintln("POINTER_UPDATE_EVENT - [%d, %d] @ DT = %d ms",
                  point.getX(), point.getY(),
                  curTime - prevTime);
   }
      break;
   case MapLibKeyInterface::POINTER_UP_EVENT:
      if(!m_dialogPointerState.scrolled) {
         handleDialogClick(longPress);         
      }

      m_dialogPointerState.initialized = false;
      m_dialogPointerState.prevPoint = point;
      m_dialogPointerState.tracking = false;
      
      coreprintln("POINTER_UP_EVENT - [%d, %d] @ DT = %d ms",
                  point.getX(), point.getY(),
                  curTime - prevTime);
      break;
   case MapLibKeyInterface::POINTER_UNKNOWN_EVENT:
      break;
   }

   prevTime = curTime;

   return true;
   
   // return true;
   
   // StackedDialog& dialog = m_view->getStackedDialog();

   // // If our pointer state is not initialized, we start by
   // // doing so. 
   // if(!m_dialogPointerState.initialized) {
   //    m_dialogPointerState.initialized = true;
   //    m_dialogPointerState.prevPoint = point;
   //    m_dialogPointerState.scrolled = false;
   // }

   // switch(type) {
   // case MapLibKeyInterface::POINTER_DOWN_EVENT:
   // {
   //    StackedDialog& dialog = m_view->getStackedDialog();
   //    const OverlayItemInternal* item =
   //       static_cast<const OverlayItemInternal*>(
   //          dialog.getSelectedItem(point));
   //    setCurrentlyBeingTapped(item);
      
   //    // We start by storing the pixel per second speed of the
   //    // flicking animation.
   //    float flickSpeed = fabs(dialog.currentFlickingSpeedPxPs());
      
   //    // If the dialog is doing a flicking animation, we always want
   //    // to stop it upon a click.
   //    if(dialog.isFlicking()) {
   //       dialog.endFlicking();
   //       //m_dialogPointerState.accelData.points.clear();
   //    }

   //    // If the speed was high enough before we stopped the animation,
   //    // we don't want to treat the click as a regular click, but rather
   //    // as an action which halted the flicking.
   //    if(flickSpeed > 30.0f) {
   //       m_dialogPointerState.scrolled = true;
   //    }

   //    // We then store the initial point in the acceleration data set.
   //    AccelData& accData = m_dialogPointerState.accelData;
      
   //    AccelData::PointSample p =
   //       { TileMapUtil::currentTimeMillis(), point };

   //    accData.points.push_back(p);
      
   //    m_dialogPointerState.prevPoint = point;
   //    m_dialogPointerState.prevTime = TileMapUtil::currentTimeMillis();

   //    // If we are not inside the bounding box of the dialog, we should
   //    // revert into normal MAP_INTERACTION mode.
   //    PixelBox box = dialog.getPixelBox();
      
   //    if(!box.pointInside(point)) {
   //       m_view->setOverlayState(OverlayView::MAP_INTERACTION);
   //       handleMapPointerEvent(type, point, false);
   //       return false;
   //    } 

      
   // }
   // break;
   // case MapLibKeyInterface::POINTER_UPDATE_EVENT:
   // {
   //    AccelData& accData = m_dialogPointerState.accelData;

   //    // If we have changed direction, i.e. going from acceleration to
   //    // deceleration or the other way around, we need to clear the
   //    // previous points in the data set (as they are no longer valid)
      
   //    if(point.getX() != m_dialogPointerState.prevPoint.getX()) {
   //       bool accelerating =
   //          point.getX() > accData.points.back().point.getX();
         
   //       if(accData.points.size() == 1) {
   //          accData.accelerating = accelerating;
   //       } else {
   //          if(accelerating != accData.accelerating) {
   //             accData.points.clear();
   //          }
   //       }
   //    }
      
   //    AccelData::PointSample p =
   //       { TileMapUtil::currentTimeMillis(), point };

   //    accData.points.push_back(p);

   //    // We do not want to store an excessive amount of previous times
   //    if(accData.points.size() > 10) {
   //       // I know that this is not optimal. However, N is small and I don't
   //       // want to involve heap allocated structures. This will not take a
   //       // long time, I promise :-)
   //       accData.points.erase(accData.points.begin());
   //    }
      
   //    int deltaX = point.getX() - m_dialogPointerState.prevPoint.getX();
   //    if(deltaX < -MIN_SCROLL_DEVIATION_PX || deltaX > MIN_SCROLL_DEVIATION_PX) {
            
   //       m_dialogPointerState.scrolled = true;
   //       setCurrentlyBeingTapped(NULL);
   //    }
         
   //    dialog.scroll(deltaX);
      
   //    m_dialogPointerState.prevPoint = point;
   //    m_dialogPointerState.prevTime = TileMapUtil::currentTimeMillis();
   // }
   // break;
   // case MapLibKeyInterface::POINTER_UP_EVENT:
   // {
   //    setCurrentlyBeingTapped(NULL);
   //    m_dialogPointerState.prevPoint = point;
      
   //    AccelData& accData = m_dialogPointerState.accelData;
      
   //    m_dialogPointerState.initialized = false;
      
   //    if(!m_dialogPointerState.scrolled || longPress) {
         
   //       handleDialogClick(longPress);            
         
   //    } else if(accData.points.size() > 3) {
   //       WFAPI::wf_uint32 elapsed =
   //          TileMapUtil::currentTimeMillis() - accData.points.front().time;

   //       int diffX = point.getX() - accData.points.front().point.getX();
         
   //       float velocityPxPs = 0.0f;
         
   //       if( elapsed > 0 && diffX != 0) {
   //          static const float millisToSec = 1.0 / 1000.0f;
            
   //          velocityPxPs = diffX / (elapsed * millisToSec);
   //          m_view->getStackedDialog().startFlicking(velocityPxPs);
   //       }            
   //    }
   // }
   //    break;
   // case MapLibKeyInterface::POINTER_UNKNOWN_EVENT:
   //    break;
   // }
      
   // return true;
}

bool OverlayViewInputInterface::handleMapPointerEvent(
   MapLibKeyInterface::kindOfPointerType type,
   const WFAPI::ScreenPoint& point,
   bool longPress)
{   
   // Algorithm explanation:
   //
   // We start by checking if the user has clicked on the area of an item.
   // If so, we are interested in tracking the further pointer updates
   // that will arrive.
   //
   // When the pointer up event occurs, we check the deviation in pixels
   // from the original point of click. Since touch devices use large input
   // tools(fingers) pixel precision can be hard to achieve. We therefore
   // allow some deviation (MAX_POINTER_DEVIATION pixels) from the point of origin.
   //
   // During the time we are tracking, we consume all pointer events. This
   // is to avoid tricky simultaneous drag/pointer tracking.
   
   switch(type) {
   case MapLibKeyInterface::POINTER_DOWN_EVENT: {
      m_nonDialogPointerState.prevPoint = point;
      m_nonDialogPointerState.accumulatedDiff = 0;

      setCurrentlyBeingTapped(
         m_view->getClickedItem(m_nonDialogPointerState.prevPoint));
      return false;
   }
      break;
   case MapLibKeyInterface::POINTER_UP_EVENT:

      setCurrentlyBeingTapped(NULL);
      m_nonDialogPointerState.prevPoint = point;
      
      return handleMapClick(longPress);
   break;
   case MapLibKeyInterface::POINTER_UPDATE_EVENT: {
      m_nonDialogPointerState.accumulatedDiff +=
         abs(point.getX() - m_nonDialogPointerState.prevPoint.getX());

      m_nonDialogPointerState.accumulatedDiff +=
         abs(point.getY() - m_nonDialogPointerState.prevPoint.getY());
      
      m_nonDialogPointerState.prevPoint = point;
      if (m_nonDialogPointerState.accumulatedDiff > MAX_POINTER_DEVIATION_PX){
         setCurrentlyBeingTapped(NULL);
      }
   }
      return false;
      break; 
   case MapLibKeyInterface::POINTER_UNKNOWN_EVENT:
      break;
   }

   return false;
}

bool
OverlayViewInputInterface::handlePointerEvent(
   MapLibKeyInterface::pointerType key,
   MapLibKeyInterface::kindOfPointerType type,
   const WFAPI::ScreenPoint& point,
   bool longPress)
{
   if (type == MapLibKeyInterface::POINTER_UP_EVENT){
      setCurrentlyBeingTapped(NULL);
   }
   switch(m_view->getOverlayState()) {
   case OverlayView::DIALOG_INTERACTION:
      return handleDialogPointerEvent(type, point, longPress);
      break;
   case OverlayView::MAP_INTERACTION:
      return handleMapPointerEvent(type, point, longPress);
      break;
   default:
      return false;
   }
}

WFAPI::ScreenPoint OverlayViewInputInterface::getCursorPosition() const
{
   return WFAPI::ScreenPoint(0, 0);
}

void OverlayViewInputInterface::stop()
{
   m_smoothManipulator.stopWorking();
}

void OverlayViewInputInterface::overlayItemRemoved(const OverlayItemInternal* item)
{
   coreprintln("overlayItemRemoved = %p", item);
   
   if(m_dialogPointerState.item == item) {
      coreprintln("Matching target, disabling animation");
      m_dialogPointerState.item = NULL;
   }
}

void OverlayViewInputInterface::animationFinished()
{
   //Stacked item
   /**
    * returns false if there is only one item,
    * then no stacked dialog should be opened.
    */
   if(m_dialogPointerState.item &&
      m_view->setStackedDialogItem(m_dialogPointerState.item))
   {
      m_view->setOverlayState(OverlayView::DIALOG_INTERACTION);
   }
   m_dialogPointerState.item = NULL;
}

bool OverlayViewInputInterface::handleDialogClick(bool longPress)
{
   StackedDialog& dialog = m_view->getStackedDialog();

   WFAPI::OverlayItem* item =
      const_cast<WFAPI::OverlayItem*>(
         dialog.getSelectedItem(m_dialogPointerState.prevPoint));

   if(item) {
      m_notifier->notifyListeners(item->getMapObjectInfo(),
                                  item,
                                  item->getPosition(),
                                  longPress);

      setCurrentlyBeingTapped(NULL);
      
      // Previously we only disabled the stacked dialog on non-longpress
      // if(!longPress) {
      m_view->setOverlayState(OverlayView::MAP_INTERACTION);
      // }

      return true;
   } else {
      return false;
   }
}

bool OverlayViewInputInterface::handleMapClick(bool longPress)
{

   
   if(m_nonDialogPointerState.accumulatedDiff > MAX_POINTER_DEVIATION_PX) {
      return false;
   }
   
   OverlayItemInternal* clicked =
      m_view->getClickedItem(m_nonDialogPointerState.prevPoint);

   if(!clicked) {
      // No clicked item found so we don't consume the event.
      return false;
   } 

   if(clicked->m_stack.getSize() > 1 && m_view->isSupportingDialog(clicked) ) {
      // if(longPress) {
      //    return false;
      // }
      
      //Now, we should display a stacked item. We want to
      //animate a transition so that the screen is centered
      //at its coordinate

      m_dialogPointerState.item = clicked;

      m_animator.attemptAnimatePosition(clicked->getPosition(),
                                        this);
   } else {      
      m_notifier->notifyListeners(clicked->getMapObjectInfo(),
                                  clicked,
                                  clicked->getPosition(),
                                  longPress);
      //Non-stacked item
   }
   setCurrentlyBeingTapped(NULL);
   return true;
}

void
OverlayViewInputInterface::setCurrentlyBeingTapped(
   const OverlayItemInternal* item)
{
   bool itemChanged = false;
   if (m_currentlyBeingTapped != item){
      itemChanged = true;
   }
   
   if(m_currentlyBeingTapped != NULL) {
      m_currentlyBeingTapped->m_isBeingTapped = false;
   }
   
   m_currentlyBeingTapped = item;

   if(m_currentlyBeingTapped != NULL){
      m_currentlyBeingTapped->m_isBeingTapped = true;
   }
   
   if (itemChanged){
      // We trigger a repaint event if the item was actually changed.
      m_view->setOverlayState(m_view->getOverlayState());
   }


}
