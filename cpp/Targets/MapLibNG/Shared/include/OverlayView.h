/*
Copyright (c) 1999 - 2010, Vodafone Group Services Ltd
All rights reserved.

Redistribution and use in source and binary forms, with or without modification, are permitted provided that the following conditions are met:

    * Redistributions of source code must retain the above copyright notice, this list of conditions and the following disclaimer.
    * Redistributions in binary form must reproduce the above copyright notice, this list of conditions and the following disclaimer in the documentation and/or other materials provided with the distribution.
    * Neither the name of the Vodafone Group Services Ltd nor the names of its contributors may be used to endorse or promote products derived from this software without specific prior written permission.

THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS" AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE ARE DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT HOLDER OR CONTRIBUTORS BE LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
*/


#ifndef OVERLAYVIEW_H
#define OVERLAYVIEW_H
#include "config.h"
#include "OverlayItemInternal.h"
#include <map>
#include "Layer.h"
#include "MapProjection/MapProjection.h"
#include "StackedSet.h"
// #include "StackedDialog.h"
#include "OverlayViewLayerInterface.h"
#include "OverlayViewInputInterface.h"
#include "OverlayViewOperationHandler.h"
#include "SelectedMapObjectNotifier.h"

class MapManipulator;
class MapDrawingInterface;
class OverlayItemInternal;
class MapProjection;
class MapDrawingInterfaceInternal;
class MapProjectionAdapter;
class OverlayDrawer;

namespace WFAPI {
class ScreenPoint;
class StackedDialogVisualSpec;
class WGS84Coordinate;
}

class TileMapToolkit;

/**
 * Class which describes an overlay view consisting of multiple layers
 * that contain overlay items. Items are stacked and 
 *
 */ 

class OverlayView : public MapProjectionListener {
public:   
   typedef WFAPI::OverlayInterface::layer_id_t layer_id_t;

   /**
    * Constructor.
    *
    * @param projection The map projection to use when calculating
    *                   bounding boxes etc.
    */ 
   OverlayView(TileMapToolkit* toolkit,
               MapProjectionAdapter& projection,
               MapManipulator& mapManipulator,
               MapDrawingInterfaceInternal* mapDrawingInterface,
               SelectedMapObjectNotifier* notifier);

   virtual ~OverlayView();
   
   /**
    * Returns our operation handler, for external parties
    */ 
   OverlayViewOperationHandler& getOperationHandler();

   /**
    * Returns our layer interface, for external parties
    */ 
   OverlayViewLayerInterface& getLayerInterface();

   /**
    * Returns our input interface, for external parties
    */
   OverlayViewInputInterface& getInputInterface();

   /**
    * Draws the entire overlay view.
    *
    * @param plotter The plotter to use when drawing.
    * @param projection The projection used when culling items outside the
    *                   screen.
    */ 
   virtual void draw(OverlayDrawer* drawer);

   /**
    * Draws our overlay dialog, which is shown when stacked items are
    * clicked.
    *
    * @param plotter The plotter to draw the dialog iwth.
    */ 
   virtual void drawDialog(OverlayDrawer* drawer);

   /**
    * Re-draws the partial part of the dialog that contains the item.
    * Used to avoid the redrawing of entire maps. Will only redraw
    * when in the DIALOG_INTERACTION state.
    */ 
   void drawPartialDialog(OverlayDrawer* drawer);
   
   /**
    * When the transform matrix has changed, we need to update our internal
    * stacking state.
    */ 
   virtual void projectionUpdated(projectionEvent trigger);

   /**
    * @param twoItemsDialogSpec The specification to use when the dialog
    *                           contains only two items.
    * @param threeItemsDialogSpec The specification to use when the dialog
    *                             contains three or more items.
    */
   virtual void setStackedDialogVisualSpecs(
      const WFAPI::StackedDialogVisualSpec* twoItemsDialogSpec,
      const WFAPI::StackedDialogVisualSpec* threeItemsDialogSpec);

   /**
    * The states the overlay view can be in,
    */ 
   enum overlayState {
      /// No dialog showing, simple map interaction. Just deal with clicks.
      MAP_INTERACTION,
      /// Dialog interaction, deal with stacked dialog input.
      DIALOG_INTERACTION
   };

   /**
    * @return The current overlay state.
    */
   overlayState getOverlayState() const;

   /**
    * Updates the overlay state to newState.
    *
    * @param newState The new state to set.
    */ 
   void setOverlayState(overlayState newState);

   /**
    * Set if automatic highlight should be enabled or not.
    * Automatic highlight means that features in the center of the
    * map area will automatically be highlighted.
    * @see DetailedConfigInterface.h
    * 
    * @param enable True if automatic highlight should be enabled,
    *               false otherwise if disabled.
    */
   void enableAutomaticHighlight(bool enable);

   /**
    * @return True if the automatic highlighting is enabled.
    */
   bool automaticHighlightEnabled();
   
   /**
    * Updates the internal stacking state.
    */ 
   void updateState();

   /**
    * Returns the item which would be selected if the user pressed
    * at point p. Returns NULL if no such item can be located.
    *
    * @param p The point to use when looking for overlap.
    */
   OverlayItemInternal* getClickedItem(const WFAPI::ScreenPoint& p);

   /**
    * Sets the overlay item parent for the stacked dialog
    *
    * @return If to open a dialog with stacked placemarkers.
    *         Will return false if there is only one item, then no dialog
    *         should be opened.
    */ 
   bool setStackedDialogItem(OverlayItemInternal* item);

   /**
    * Returns the stack dialog for this overlay view.
    */ 
   StackedDialog& getStackedDialog();

   /**
    * Called whenever an item is removed from a layer.
    */
   void overlayItemRemoved(const OverlayItemInternal* item);
   
   /**
    * 
    */ 
   void updateClosestItemPoint();

   MC2Point getClosestItemPoint();

   MC2Coordinate getClosestItemCoord();

   bool hasValidClosestItem();
   
   const WFAPI::OverlayItem* getClosestItem();
   
   void updateClosestItem();

   void setStateChanged();
   void resetStateChanged();
   bool getStateChanged() const;

   
   bool isSupportingDialog(const OverlayItemInternal* item);
   
   /**
    * To center the map around this item.
    *
    * @param item The stacked item to center the map around.
    */
   void centerOnItem(const OverlayItemInternal* item);
   
private:

   void updateClosest(OverlayItemInternal* newClosest);
   
   /// Our map projection, used for coordinate transforms and viewport params
   MapProjectionAdapter& m_projection;
   
   /// Our external layer interface, used to delegate calls to internal layers
   OverlayViewLayerInterface m_layerInterface;

   /// Our external input interface, used to delegate actions to view
   OverlayViewInputInterface m_inputInterface;

   /// Our external operation handler
   OverlayViewOperationHandler m_operationHandler;
   
   /// The set we use to detect stacks
   StackedSet m_stackSet;

   /// The stacked dialog.
   StackedDialog m_dialog;

   /// Our current state
   overlayState m_state;

   /// The item that our stacked dialog is possibly showing
   OverlayItemInternal* m_dialogItem;

   /// If our display state has changed.
   bool m_stateChanged;

   /// Used for notifying about redraws
   MapDrawingInterfaceInternal* m_mapDrawingInterface;
   
   MapManipulator& m_mapManipulator;

   /// When a stacked dialog has been opened or closed, interested parties will
   /// be notified through this interface
   SelectedMapObjectNotifier* m_notifier;
};

#endif // OVERLAYVIEW_H
