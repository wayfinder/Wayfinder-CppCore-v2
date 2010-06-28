/*
Copyright (c) 1999 - 2010, Vodafone Group Services Ltd
All rights reserved.

Redistribution and use in source and binary forms, with or without modification, are permitted provided that the following conditions are met:

    * Redistributions of source code must retain the above copyright notice, this list of conditions and the following disclaimer.
    * Redistributions in binary form must reproduce the above copyright notice, this list of conditions and the following disclaimer in the documentation and/or other materials provided with the distribution.
    * Neither the name of the Vodafone Group Services Ltd nor the names of its contributors may be used to endorse or promote products derived from this software without specific prior written permission.

THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS" AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE ARE DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT HOLDER OR CONTRIBUTORS BE LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
*/


#ifndef OVERLAYVIEWINPUTINTERFACE_H
#define OVERLAYVIEWINPUTINTERFACE_H

#include "MapLibKeyInterface.h"
#include "ScreenPoint.h"
#include "TileMapUtil.h"
#include "AnimatedMapManipulator.h"

#include <vector>

class OverlayView;
class SelectedMapObjectNotifier;
class SmoothMapManipulator;
class OverlayItemInternal;

namespace WFAPI {
class MapDrawingInterface;
}


/**
 * The class that takes care of input related operations that is
 * relevant for the OverlayView. I.e. zooming in on the closest
 * item, dealing with scrolling in the stacked dialog and so forth.
 */ 
class OverlayViewInputInterface : public PositionAnimationListener {
public:
   /**
    * Constructor.
    *
    * @param view The view to delegate actions to according to input.
    */ 
   OverlayViewInputInterface(OverlayView* view,
                             SelectedMapObjectNotifier* notifier,
                             SmoothMapManipulator& smoothManipulator,
                             AnimatedMapManipulator& animator);
   /**
    * 
    */ 
   virtual ~OverlayViewInputInterface();
   
   /**
    * @see MapLibKeyInterface::handleKeyEvent
    */
   bool handleKeyEvent(WFAPI::MapLibKeyInterface::keyType key,
                       WFAPI::MapLibKeyInterface::kindOfPressType type);
   
   /**
    * @see MapLibKeyInterface::handlePointerEvent
    */
   bool handlePointerEvent(WFAPI::MapLibKeyInterface::pointerType key,
                           WFAPI::MapLibKeyInterface::kindOfPointerType type,
                           const WFAPI::ScreenPoint& point,
                           bool longPress);
   
   /**
    * @see MapLibKeyInterface::getCursorPosition
    */ 
   virtual WFAPI::ScreenPoint getCursorPosition() const;
   
   /**
    * @see MapLibKeyInterface::stop
    */
   virtual void stop();

   /**
    * @see PositionAnimationListener::animationFinished
    */ 
   void animationFinished();

   /**
    * Called whenever an item is removed from a layer.
    */
   void overlayItemRemoved(const OverlayItemInternal* item);
private:
   /**
    * Handles a pointer event which occured inside the dialog when
    * the OverlayView was in a DIALOG_INTERACTION state.
    *
    * The strategy of the method is to detect a flicking motion,
    * i.e. when the user scrolled the dialog with force. If such
    * a motion occurs, the velocity of the motion is calculated
    * using previous point samples and a flicking motion is
    * triggered in the dialog.
    *
    * It also detects input which cancels flicking motions and
    * regular clicks inside the dialog.
    *
    * @param type The pointer event which triggered the action
    * @param point The point where the click occured, should be
    *              within the dialog.
    * @param longPress Whether or not the press was the result of a
    *                  long press or not.
    */ 
   bool handleDialogPointerEvent(WFAPI::MapLibKeyInterface::kindOfPointerType type,
                                 const WFAPI::ScreenPoint& point,
                                 bool longPress);

   /**
    * Deals with a pointer click which occured in the map view.
    *
    * @param longPress Whether or not the press was the result of a
    *                  long press or not.
    * @return True if the click resulted in a selected item.                 
    */ 
   bool handleMapClick(bool longPress);

   /**
    * Deals with a dialog click which occured in a stacked dialog,
    *
    * @param longPress Whether or not the press was the result of a
    *                  long press or not.
    * @return True if the click resulted in a selected item.                 
    */ 
   bool handleDialogClick(bool longPress);
   
   /**
    * Handles a pointer event which occured in the map view.
    *
    * @param type The type of the pointer event which triggered the
    *             action.
    * @param point The point where the click occured, should be within
    *              the map area.
    *
    * @param longPress Whether or not the pointer event is a long press event.
    */   
   bool handleMapPointerEvent(WFAPI::MapLibKeyInterface::kindOfPointerType type,
                              const WFAPI::ScreenPoint& point,
                              bool longPress);


   void setCurrentlyBeingTapped(const OverlayItemInternal* item);

   /**
    *  The state information for when we are not in dialog state, i.e.
    *  MAP_INTERACTION state for OverlayView
    */ 
   struct {
      int accumulatedDiff;
      WFAPI::ScreenPoint prevPoint;
   } m_nonDialogPointerState;

   /**
    * Data for the acceleration calculations which are used for
    * triggering flicking motions in the stacked dialog.
    * 
    */ 
   struct AccelData {
      /// A discrete sample of input movement. 
      struct PointSample {
         WFAPI::wf_uint32 time;
         WFAPI::ScreenPoint point;
      };

      /// Are we accelerating, i.e. going to the left, or decelerating,
      /// (going to the right)
      bool accelerating;

      /// Our set of input samples
      std::vector<PointSample> points;
   };

   /**
    * The state of the pointer input when we are in the stacked
    * dialog state. 
    */ 
   struct {
      /// Acceleration data used for triggering flicking motions.
      AccelData accelData; 

      /// The previous input point in the state. 
      WFAPI::ScreenPoint prevPoint;

      /// The previous input point in the state. 
      WFAPI::ScreenPoint initialPoint;

      /// The time the previous input point occured
      WFAPI::wf_uint32 prevTime;

      /// True if we have scrolled the dialog. If so, the pointer up
      /// event should not result in the selection of an item.
      bool scrolled;

      /// Whether or not we are properly initialized.
      bool initialized;

      /// To deal with errant POINTER_UPDATE's between DOWN and UPS, we
      /// keep track of our tracking state.
      bool tracking;
      
      /// The item that serves as the base of the dialog
      OverlayItemInternal* item;

   } m_dialogPointerState;

   const OverlayItemInternal* m_currentlyBeingTapped;
   
   /// The parent view that we delegate actions to.
   OverlayView* m_view;

   /// When an object has been selected, interested parties will
   /// be notified through this interface
   SelectedMapObjectNotifier* m_notifier;

   /// We use this interface to achieve smooth map scrolling
   SmoothMapManipulator& m_smoothManipulator;

   /// We use this interface to achieve smooth transitions
   AnimatedMapManipulator& m_animator;

};


#endif // OVERLAYVIEWINPUTINTERFACE_H
