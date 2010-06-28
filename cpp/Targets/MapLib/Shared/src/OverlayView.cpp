/*
Copyright (c) 1999 - 2010, Vodafone Group Services Ltd
All rights reserved.

Redistribution and use in source and binary forms, with or without modification, are permitted provided that the following conditions are met:

    * Redistributions of source code must retain the above copyright notice, this list of conditions and the following disclaimer.
    * Redistributions in binary form must reproduce the above copyright notice, this list of conditions and the following disclaimer in the documentation and/or other materials provided with the distribution.
    * Neither the name of the Vodafone Group Services Ltd nor the names of its contributors may be used to endorse or promote products derived from this software without specific prior written permission.

THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS" AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE ARE DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT HOLDER OR CONTRIBUTORS BE LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
*/


#define CORE_LOGGING_MODULE_NAME "OverlayView"
//#define CORE_LOGGING_MODULE_LEVEL CL_ALL_FILE

#include "CoreDebugPrint.h"

#include "config.h"
#include "OverlayDrawing.h"
#include "OverlayView.h"
#include "OverlayItemZoomSpec.h"
#include "OverlayItemVisualSpec.h"
#include "MapProjection.h"
#include "MapPlotter.h"
#include "ImageSpec.h"
#include "Utility.h"
#include "OverlayItemUtil.h"
#include "MapDrawingInterfaceInternal.h"
#include "MapProjectionAdapter.h"
#include "MapManipulator.h"


OverlayView::OverlayView(TileMapToolkit* toolkit,
                         MapProjectionAdapter& projection,
                         MapManipulator& mapManipulator,
                         MapDrawingInterfaceInternal* mapDrawingInterface,
                         SelectedMapObjectNotifier* notifier) :
   m_projection(projection),
   m_layerInterface(this),
   m_inputInterface(this, notifier,
                    mapManipulator.getSmoothMapManipulator(),
                    mapManipulator.getAnimatedManipulator()),
   m_operationHandler(this, &projection),
   m_stackSet(projection, m_layerInterface),
   m_dialog(this, toolkit, mapDrawingInterface, projection),
   m_state(MAP_INTERACTION),
   m_dialogItem(NULL),
   m_stateChanged(true),
   m_mapDrawingInterface(mapDrawingInterface),
   m_mapManipulator(mapManipulator),
   m_notifier(notifier)
{
   m_stateChanged = true;
}

OverlayViewLayerInterface& OverlayView::getLayerInterface()
{
   return m_layerInterface;
}

OverlayViewInputInterface& OverlayView::getInputInterface()
{
   return m_inputInterface;
}

OverlayItemInternal* OverlayView::getClickedItem(const WFAPI::ScreenPoint& p)
{
   StackedSet::StackVec& stackedItems = m_stackSet.getStackedItems();
   OverlayItemInternal* highlightedItem=NULL;

   //first check if there is a highlighted item visible   
   for( StackedSet::StackVec::reverse_iterator itr = stackedItems.rbegin();
        itr != stackedItems.rend(); itr++ )
   {
      OverlayItemInternal* curItem = *itr;
      if(curItem->m_isHighlighted) {
         highlightedItem=curItem;
         break;
      }
   }
   if(highlightedItem) {

      MC2Point screenPoint;
      m_projection.transform(screenPoint, highlightedItem->getPosition());
      WFAPI::wf_uint32 pixScale =
         static_cast<WFAPI::wf_uint32>(m_projection.getPixelScale());
      const WFAPI::OverlayItemVisualSpec* visualSpec = 
         OverlayItemUtil::getCurrentVisualSpec(highlightedItem, pixScale);

      const WFAPI::ImageSpec* background = visualSpec->getBackgroundImage();
   
      WFAPI::ScreenPoint offsetPoint = visualSpec->getFocusPoint();
   
      unsigned int width = background->getWidth();
      unsigned int height = background->getHeight();

      // Create our offset box
      PixelBox box( MC2Point(-offsetPoint.getX(), -offsetPoint.getY()),
                    MC2Point(-offsetPoint.getX() + width,
                             -offsetPoint.getY() + height));

      //Offset the box to the correct position on the screen
      box.move(screenPoint);

      //now check if we clicked the highlighted item
      if(box.pointInside(p)) {
         return highlightedItem;
      }
   }

   //if we got this far we clicked a non-highlighted item
   for( StackedSet::StackVec::reverse_iterator itr = stackedItems.rbegin();
        itr != stackedItems.rend(); itr++ )
   {
      OverlayItemInternal* curItem = *itr;
      //skip highlighted item
      if(curItem == highlightedItem) {
         continue;
      }

      PixelBox box = curItem->getPixelBox();
      //check if we clicked an item that is not the currently highlighted item
      if(box.pointInside(p)) {
         return curItem;
      }
   }

   return NULL;
}

WFAPI::WFString getNbrItems(const OverlayItemInternal* item)
{
   char num[32];
   
   if(item->m_stack.getSize() > 1) {
      sprintf(num, "%d", item->m_stack.getSize());

      return WFAPI::WFString(num);
   }
   return WFAPI::WFString();
}

WFAPI::WFString getName(const OverlayItemInternal* item)
{   
   const WFAPI::MapObjectInfo& objInfo = item->getMapObjectInfo();
   
   return objInfo.getName();
}

void OverlayView::draw(OverlayDrawer* drawer)
{
   bool highlightEnabled = m_stackSet.automaticHighlightEnabled();
   
   if (m_mapManipulator.getAnimatedManipulator().isWorking()) {
      m_stackSet.enableAutomaticHighlight(false);
   }
   
   m_stackSet.update();      

   StackedSet::StackVec& stackedItems = m_stackSet.getStackedItems();

   const OverlayItemInternal* highlighted = NULL;

   WFAPI::wf_uint32 pixScale =
      static_cast<WFAPI::wf_uint32>(m_projection.getPixelScale());

   for( StackedSet::StackVec::const_iterator itr = stackedItems.begin();
        itr != stackedItems.end(); itr++ )
   {
      const OverlayItemInternal* curItem = *itr;

      if(curItem == m_dialogItem) {
         continue;
      }      

      if (!curItem->m_isHighlighted){

         MC2Point topLeft(curItem->getPixelBox().getTopLeft());
         
         // Get our visual spec
         const WFAPI::OverlayItemVisualSpec* visualSpec =
            OverlayItemUtil::getCurrentVisualSpec(curItem, pixScale);

         const WFAPI::WFString& nbrItems(getNbrItems(curItem));
      
         const WFAPI::WFString& itemName(getName(curItem));
         
         OverlayDrawing::drawOverlayItem(curItem,
                                         visualSpec,
                                         itemName,
                                         nbrItems,
                                         topLeft,
                                         drawer);
         // OverlayDrawing::drawPixelBox(curItem->getPixelBox(),
         //                              drawer);
      } else {
         highlighted = curItem;
      }
   }
         
      
   if (highlighted){
      // OverlayDrawing::drawPixelBox(closest->getPixelBox(),
      //                              drawer);
      const WFAPI::OverlayItemVisualSpec* visualSpec =
         OverlayItemUtil::getCurrentVisualSpec(highlighted, pixScale);

      const WFAPI::WFString& nbrItems(getNbrItems(highlighted));
      
      const WFAPI::WFString& itemName(getName(highlighted));

      MC2Point topLeft(highlighted->getPixelBox().getTopLeft());
      
      OverlayDrawing::drawOverlayItem(highlighted, visualSpec,
                                      itemName,
                                      nbrItems,
                                      topLeft,
                                      drawer);
   }

   if( getOverlayState() == DIALOG_INTERACTION ) {
      drawDialog(drawer);      
   }

   m_stackSet.enableAutomaticHighlight(highlightEnabled);
}

void OverlayView::updateState()
{
   setOverlayState(MAP_INTERACTION);
   
   m_stackSet.update();
}

void OverlayView::projectionUpdated(projectionEvent trigger)
{
   updateState();
}

void OverlayView::setStackedDialogVisualSpecs(
   const WFAPI::StackedDialogVisualSpec* twoItemsDialogSpec,
   const WFAPI::StackedDialogVisualSpec* threeItemsDialogSpec)
{
   m_dialog.setVisualSpec(twoItemsDialogSpec, 2);
   m_dialog.setVisualSpec(threeItemsDialogSpec, 3);
}

void OverlayView::drawDialog(OverlayDrawer* drawer)
{
   m_dialog.draw(drawer);
}


void OverlayView::drawPartialDialog(OverlayDrawer* drawer)
{
   if(m_state == DIALOG_INTERACTION) {
      m_dialog.drawPartial(drawer);      
   }
}

OverlayView::overlayState OverlayView::getOverlayState() const
{
   return m_state; 
}

void OverlayView::setOverlayState(overlayState newState)
{
   setStateChanged();

   if(m_state == DIALOG_INTERACTION && newState == MAP_INTERACTION) {
      m_notifier->notifyListenersStackedDialogClosed();
   } else if(m_state == MAP_INTERACTION && newState == DIALOG_INTERACTION) {
      m_notifier->notifyListenersStackedDialogOpened();
   }
      
   
   if(m_state == DIALOG_INTERACTION && newState != DIALOG_INTERACTION) {     
      // The following line causes a sigsegv so it is disabled.
      //m_dialog.disableDialog();
      m_dialogItem = NULL;

   }
   
   m_state = newState;
   m_mapDrawingInterface->requestRepaint();
}

bool OverlayView::setStackedDialogItem(OverlayItemInternal* item)
{
   if(item == NULL) {
      return false;
   }
   
   if(item->m_stack.getSize() == 1) {
      // A dialog should never be opened with 1 item.
      return false;
   }
   m_dialogItem = item;
   m_dialog.enableDialog(item);
   return true;
}

StackedDialog& OverlayView::getStackedDialog()
{
   return m_dialog;
}


void OverlayView::enableAutomaticHighlight(bool enable)
{
   m_stackSet.enableAutomaticHighlight(enable);
}

bool OverlayView::automaticHighlightEnabled()
{
   return m_stackSet.automaticHighlightEnabled();
}

OverlayViewOperationHandler& OverlayView::getOperationHandler()
{
   return m_operationHandler; 
}

bool OverlayView::hasValidClosestItem()
{
   return m_stackSet.hasValidClosestItem(); 
}

void OverlayView::updateClosestItemPoint()
{
   m_stackSet.updateClosestItemPoint(); 
}

MC2Point OverlayView::getClosestItemPoint()
{
   return m_stackSet.getClosestItemPoint(); 
}

MC2Coordinate OverlayView::getClosestItemCoord()
{
   return m_stackSet.getClosestItemCoord(); 
}

void OverlayView::setStateChanged()
{
   m_stateChanged = true; 
}

void OverlayView::resetStateChanged()
{
   m_stateChanged = false; 
}

bool OverlayView::getStateChanged() const
{
   return m_stateChanged; 
}

OverlayView::~OverlayView() {
   m_stackSet.clear();    
}

void OverlayView::centerOnItem(const OverlayItemInternal* item)
{
   m_projection.setCenter(item->getPosition());
}

bool OverlayView::isSupportingDialog(const OverlayItemInternal* item)
{
   return m_dialog.isValidFor(item);
}

void OverlayView::overlayItemRemoved(const OverlayItemInternal* item)
{
   coreprintln("Note: OV-item @ %p removed.", item);

   m_inputInterface.overlayItemRemoved(item);
}

const WFAPI::OverlayItem* OverlayView::getClosestItem()
{
   return m_stackSet.getClosestItem(); 
}


