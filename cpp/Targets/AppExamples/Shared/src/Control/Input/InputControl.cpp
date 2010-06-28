/*
Copyright (c) 1999 - 2010, Vodafone Group Services Ltd
All rights reserved.

Redistribution and use in source and binary forms, with or without modification, are permitted provided that the following conditions are met:

    * Redistributions of source code must retain the above copyright notice, this list of conditions and the following disclaimer.
    * Redistributions in binary form must reproduce the above copyright notice, this list of conditions and the following disclaimer in the documentation and/or other materials provided with the distribution.
    * Neither the name of the Vodafone Group Services Ltd nor the names of its contributors may be used to endorse or promote products derived from this software without specific prior written permission.

THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS" AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE ARE DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT HOLDER OR CONTRIBUTORS BE LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
*/

#define CORE_LOGGING_MODULE_NAME "InputControl"
#define CORE_LOGGING_MODULE_LEVEL CL_ALL_FILE
#include "CoreDebugPrint.h"


#include "Control/Input/InputControl.h"
#include "MapModel.h"
#include "AnimatedMapManipulator.h"
#include "MapManipulator.h"
#include "MapLibKeyHandler.h"
#include "MapProjection/MapProjection.h"
#include <map>
#include "KeyEvent.h"
#include "PointerEvent.h"
#include <math.h>
#include "MapOperationHandler.h"
#include "TileMapUtil.h"
#include "AppEventListener.h"

#include "Control/Input/GestureDetector.h"

using namespace WFAPI;

/// Key mapping of KeyCodes into map actions.
typedef std::map<KeyEvent::Code, MapLibKeyInterface::keyType> KeyMap;

#define SQUARE(a) ((a)*(a))

#define MAX_PIXEL_SCALE 15000


int calcDist(const WFAPI::ScreenPoint& p0,
             const WFAPI::ScreenPoint& p1)
{
   // Pythagoras.
   return static_cast<int>(sqrt(SQUARE(p1.getX() - p0.getX()) +
                                SQUARE(p1.getY() - p1.getY())));

}

struct InputControl::Impl {
   Impl(MapModel* model,
        AppEventListener* appListener,
        GestureListener* gestureListener) :
      model(model),
      curBackEnd(),
      appListener(appListener),
      currentlyDragging(false),
      initialPinchDist(),
      initialPinchZoom(),
      allowDragging(true),      
      gestureDetector(gestureListener)
   {}

   /// The map model.
   MapModel* model;

   /// The current map backend that is used. Represent BackendType enum.
   int curBackEnd;

   /// Key mapping of KeyCodes into map actions.
   KeyMap keyMap;

   // The listener we will issue exit requests to
   AppEventListener* appListener;

   PointerEvent prevPointerEvent;

   // If the map is currently dragged using pointer events.
   bool currentlyDragging;

   int initialPinchDist;
   
   double initialPinchZoom;

   bool allowDragging;

   // The gesture detector.
   GestureDetector gestureDetector;
};

InputControl::InputControl(MapModel* model,
                           AppEventListener* appListener)
{   
   m_impl = new Impl(model, appListener, this);

   initKeyMap();
   
   // Skip dragging
   // m_impl->allowDragging = false;
}

void InputControl::initKeyMap()
{
   m_impl->keyMap[KeyEvent::ML_x] = MapLibKeyInterface::ZOOM_OUT_KEY;
   m_impl->keyMap[KeyEvent::ML_z] = MapLibKeyInterface::ZOOM_IN_KEY;
   m_impl->keyMap[KeyEvent::ML_Minus] = MapLibKeyInterface::ZOOM_OUT_KEY;
   m_impl->keyMap[KeyEvent::ML_Plus] = MapLibKeyInterface::ZOOM_IN_KEY;
   m_impl->keyMap[KeyEvent::ML_Up] = MapLibKeyInterface::MOVE_UP_KEY;
   m_impl->keyMap[KeyEvent::ML_Down] = MapLibKeyInterface::MOVE_DOWN_KEY;
   m_impl->keyMap[KeyEvent::ML_Left] = MapLibKeyInterface::MOVE_LEFT_KEY;
   m_impl->keyMap[KeyEvent::ML_Right] = MapLibKeyInterface::MOVE_RIGHT_KEY;
   m_impl->keyMap[KeyEvent::ML_Space] = MapLibKeyInterface::RESET_ROTATION_KEY;
   m_impl->keyMap[KeyEvent::ML_PageUp] = MapLibKeyInterface::ROTATE_LEFT_KEY;
   m_impl->keyMap[KeyEvent::ML_PageDown] = MapLibKeyInterface::ROTATE_RIGHT_KEY;
}



InputControl::~InputControl()
{
   delete m_impl;
}

#define ZOOM_FACTOR 1.10

void InputControl::onKeyUp(const KeyEvent& event)
{
   MapLibKeyHandler& keyHandler = m_impl->model->getKeyHandler();
   
   KeyMap::iterator itr = m_impl->keyMap.find(event.code);

   if(itr != m_impl->keyMap.end()) {
      keyHandler.handleKeyEvent(
         itr->second, MapLibKeyInterface::KEY_UP_EVENT);
      return;
   }    
}

void InputControl::onKeyDown(const KeyEvent& event)
{
   AnimatedMapManipulator& manipulator =
      m_impl->model->getMapManipulator().getAnimatedManipulator();

   MapLibKeyHandler& keyHandler = m_impl->model->getKeyHandler();
   
   KeyMap::iterator itr = m_impl->keyMap.find(event.code);

   if(itr != m_impl->keyMap.end()) {
      keyHandler.handleKeyEvent(
         itr->second, MapLibKeyInterface::KEY_DOWN_EVENT);
      return;
   }
   
   switch(event.code) {
   case KeyEvent::ML_F5:
      // Bound to toggle map backend
      m_impl->curBackEnd = (m_impl->curBackEnd + 1) % MapModel::NUM_BACKENDS;
      m_impl->model->setMapBackend(MapModel::BackendType(m_impl->curBackEnd));
      break;
   case KeyEvent::ML_F2:
      // Switch to 2D
      manipulator.attemptAnimate2d3dSwitch(false);
      break;
   case KeyEvent::ML_F3:
      // Switch to 3d
      manipulator.attemptAnimate2d3dSwitch(true);
      break;
   case KeyEvent::ML_e:
      if(m_impl->appListener) {
         m_impl->appListener->onExitRequest(0);
      }
      break;
   }
}

void InputControl::onPointerDown(const PointerEvent& event)
{   
   MapLibKeyHandler& keyHandler = m_impl->model->getKeyHandler();
   
   // printf("onPointerDown, %d touches\n",
   //        event.allStates.size());

   AnimatedMapManipulator& manipulator =
      m_impl->model->getMapManipulator().getAnimatedManipulator();
   
   // if(event.allStates.size() == 3) {
   //    static bool in3d = true;
   //    manipulator.attemptAnimate2d3dSwitch(in3d);
   //    in3d = !in3d;
   // }

   if(event.allStates.size() == 3) {
      m_impl->curBackEnd = (m_impl->curBackEnd + 1) % MapModel::NUM_BACKENDS;
      m_impl->model->setMapBackend(MapModel::BackendType(m_impl->curBackEnd));
   }

   if (event.allStates.size() == 2) {
      bool doubleTap = true;
      

      if(doubleTap) {
         printf("[InputControl]: detected double tap!\n");
      }
      
      // Second finger is placed on screen.
      // Cancel dragging map and switch to pinch zoom.

      // Move with two fingers means pinch zoom.
      // Get the diff from previous update.      
      
      m_impl->initialPinchDist  = calcDist(event.allStates[0].location,
                                           event.allStates[1].location);

      m_impl->initialPinchZoom = m_impl->model->getMapOperationHandler().getZoomLevel();

      // printf("onPointerDown, pd = %d, pz = %lf\n",
      //        m_impl->initialPinchDist,
      //        m_impl->initialPinchZoom);
      
      if(m_impl->allowDragging && m_impl->currentlyDragging) {
         // Find the other event. There should be one.
         for (unsigned int i = 0; i < event.allStates.size(); ++i) {
            if (event.allStates[i].type != PointerEvent::POINTER_DOWN) {
               keyHandler.handlePointerEvent(
                  MapLibKeyInterface::DRAG_TO,
                  MapLibKeyInterface::POINTER_UP_EVENT,
                  event.allStates[i].location, false);
               m_impl->currentlyDragging = false;
               break;
            }
         }
      }
   } else if (m_impl->allowDragging && event.allStates.size() == 1) {      
      keyHandler.handlePointerEvent(
         MapLibKeyInterface::DRAG_TO,
         MapLibKeyInterface::POINTER_DOWN_EVENT,
         event.modStates[0].location, false);
      m_impl->currentlyDragging = true;
   }

   m_impl->prevPointerEvent = event;

   m_impl->gestureDetector.onPointerDown(event);
}

void InputControl::onPointerMove(const PointerEvent& event)
{   
   MapLibKeyHandler& keyHandler = m_impl->model->getKeyHandler();

   if (event.allStates.size() ==  2 &&
       m_impl->prevPointerEvent.allStates.size() == 2)
   {
      assert(! m_impl->currentlyDragging);
      // Move with two fingers means pinch zoom.
      // Get the diff from previous update.      
      
      int curDist = calcDist(event.allStates[0].location,
                             event.allStates[1].location);

      // Calculate how large curDist is compared to initialDist

      float factor = (float)m_impl->initialPinchDist / curDist;
      float pixelScale = factor * m_impl->initialPinchZoom;

      // printf("onPointerMove, cd = %d, factor = %f, pz = %f\n",
      //        curDist,
      //        factor,
      //        pixelScale);

      if (pixelScale > MAX_PIXEL_SCALE) {
         return;
      }

      m_impl->model->getMapOperationHandler().setZoomLevel(pixelScale);
   } else if (m_impl->allowDragging && event.allStates.size() == 1) {
      assert(m_impl->currentlyDragging);

      
      keyHandler.handlePointerEvent(
         MapLibKeyInterface::DRAG_TO,
         MapLibKeyInterface::POINTER_UPDATE_EVENT,
         event.modStates[0].location, false);
   }

   m_impl->prevPointerEvent = event;

   m_impl->gestureDetector.onPointerMove(event);
}

void InputControl::onPointerUp(const PointerEvent& event)
{
   if (!m_impl->allowDragging) {
      return;
   }
   
   MapLibKeyHandler& keyHandler = m_impl->model->getKeyHandler();   

   if (event.allStates.size() == 2) {
      assert(!m_impl->currentlyDragging);
   }

   if (m_impl->currentlyDragging) {
      // Stop dragging, since finger up.
      assert(event.allStates.size() == 1);
      
      keyHandler.handlePointerEvent(
         MapLibKeyInterface::DRAG_TO,
         MapLibKeyInterface::POINTER_UP_EVENT,
         event.modStates[0].location, false);
      m_impl->currentlyDragging = false;
   } else {
      // Currently not dragging and finger up -> go from pinch zoom to
      // dragging.
      assert(event.allStates.size() == 2);

      // Find the event that was not pointer up and
      // register that as pointer down.
      for (unsigned int i = 0; i < event.allStates.size(); ++i) {
         if (event.allStates[i].type != PointerEvent::POINTER_UP) {
            keyHandler.handlePointerEvent(
               MapLibKeyInterface::DRAG_TO,
               MapLibKeyInterface::POINTER_DOWN_EVENT,
               event.allStates[i].location, false);
            m_impl->currentlyDragging = true;
            break;
         }
      }
   }
            
   m_impl->prevPointerEvent = event;
   
   m_impl->gestureDetector.onPointerUp(event);
}


bool
InputControl::onFling(const PointerEvent& event, 
                         float velocityX,
                         float velocityY)
{
   coreprintln("Got call to onFling, velX = %f, velY = %f",
               velocityX, velocityY);

   AnimatedMapManipulator& manipulator =
      m_impl->model->getMapManipulator().getAnimatedManipulator();
   manipulator.attemptFlicking(velocityX, velocityY);
   return true;

}
