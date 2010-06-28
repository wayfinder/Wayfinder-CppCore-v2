/*
Copyright (c) 1999 - 2010, Vodafone Group Services Ltd
All rights reserved.

Redistribution and use in source and binary forms, with or without modification, are permitted provided that the following conditions are met:

    * Redistributions of source code must retain the above copyright notice, this list of conditions and the following disclaimer.
    * Redistributions in binary form must reproduce the above copyright notice, this list of conditions and the following disclaimer in the documentation and/or other materials provided with the distribution.
    * Neither the name of the Vodafone Group Services Ltd nor the names of its contributors may be used to endorse or promote products derived from this software without specific prior written permission.

THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS" AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE ARE DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT HOLDER OR CONTRIBUTORS BE LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
*/


// #define CORE_LOGGING_MODULE_LEVEL CL_ALL_FILE
#define CORE_LOGGING_MODULE_NAME "MapLibKeyHandler"

#include "config.h"
#include "MapLibKeyHandler.h"
#include "SmoothMapManipulator.h"
#include "MapProjection.h"
#include "DraggingMapManipulator.h"
#include "AnimatedMapManipulator.h"
#include "TileMapHandler.h"
#include "SelectedMapObjectNotifier.h"
#include "MapObjectInfo.h"
#include "MapManipulator.h"
#include "ExternalRenderer.h"
#include "StressTester.h"

#include "MapProjectionAdapter.h"
#include "CommonData.h"

#include "CoreDebugPrint.h"

using namespace WFAPI;

MapLibKeyHandler::MapLibKeyHandler(MapManipulator& mapManipulator,
                                   TileMapHandler* handler,
                                   SelectedMapObjectNotifier* notifier)
   :      m_smoothManipulator(mapManipulator.getSmoothMapManipulator()),
          m_draggingManipulator(mapManipulator.getDraggingMapManipulator()),
          m_animatedManipulator(mapManipulator.getAnimatedManipulator()),
          m_handler(handler),
          m_notifier(notifier)
{
   m_longPressRequest = false;
   m_validRequest = false;
   m_clickTracking.accumulatedDiff = 0;
}

bool MapLibKeyHandler::handleKeyEvent(MapLibKeyInterface::keyType key,
                                      MapLibKeyInterface::kindOfPressType type)
{   
   bool isDown = type == MapLibKeyInterface::KEY_DOWN_EVENT;

   if(!isDown && !m_smoothManipulator.isWorking()) {
      return true;
   }

   switch(key) {
   case MapLibKeyInterface::SELECT_KEY:
      break;
   case MapLibKeyInterface::MOVE_UP_KEY:
      if(isDown) {
         m_smoothManipulator.attemptMoveUp();
      } else {
         m_smoothManipulator.endMoveUp();
      }
      break;
   case MapLibKeyInterface::MOVE_DOWN_KEY:
      if(isDown) {
         m_smoothManipulator.attemptMoveDown();
      } else {
         m_smoothManipulator.endMoveDown();
      }
      break;
   case MapLibKeyInterface::MOVE_LEFT_KEY:
      if(isDown) {
         m_smoothManipulator.attemptMoveLeft();
      } else {
         m_smoothManipulator.endMoveLeft();
      }
      break;
   case MapLibKeyInterface::MOVE_RIGHT_KEY:
      if(isDown) {
         m_smoothManipulator.attemptMoveRight();
      } else {
         m_smoothManipulator.endMoveRight();
      }
      break;
   case MapLibKeyInterface::ZOOM_IN_KEY:
      if(isDown) {
         m_smoothManipulator.attemptZoomIn();
      } else {
         m_smoothManipulator.endZoomIn();
      }
      break;
   case MapLibKeyInterface::ZOOM_OUT_KEY:
      if(isDown) {
         m_smoothManipulator.attemptZoomOut();
      } else {
         m_smoothManipulator.endZoomOut();
      }
      break;
   case MapLibKeyInterface::ROTATE_LEFT_KEY:
      if(isDown) {
         m_smoothManipulator.attemptRotateLeft();
      } else {
         m_smoothManipulator.endRotateLeft();
      }
      break;
   case MapLibKeyInterface::ROTATE_RIGHT_KEY:
      if(isDown) {
         m_smoothManipulator.attemptRotateRight();
      } else {
         m_smoothManipulator.endRotateRight();
      }
      break;
   case MapLibKeyInterface::RESET_ROTATION_KEY:
      if(isDown) {
         
      } else {
         m_smoothManipulator.attemptResetRotation();
      }
      break;
   case MapLibKeyInterface::MOVE_UP_RIGHT_KEY:
      if(isDown) {
         m_smoothManipulator.attemptMoveUpAndRight();
      } else {
         m_smoothManipulator.endMoveUpAndRight();
      }
      break;
   case MapLibKeyInterface::MOVE_DOWN_RIGHT_KEY:
      if(isDown) {
         m_smoothManipulator.attemptMoveDownAndRight();
      } else {
         m_smoothManipulator.endMoveDownAndRight();
      }
      break;
   case MapLibKeyInterface::MOVE_UP_LEFT_KEY:
      if(isDown) {
         m_smoothManipulator.attemptMoveUpAndLeft();
      } else {
         m_smoothManipulator.endMoveUpAndLeft();
      }
      break;
   case MapLibKeyInterface::MOVE_DOWN_LEFT_KEY:
      if(isDown) {
         m_smoothManipulator.attemptMoveDownAndLeft();
      } else {
         m_smoothManipulator.endMoveDownAndLeft();
      }
      break;
   case MapLibKeyInterface::INTERNAL_TRIGGER_KEY:

      break;
   case MapLibKeyInterface::INTERNAL_TRIGGER_KEY_2:
      if(isDown) {
         m_handler->getStressTester().start();
         m_handler->repaintNow();
      }
      break;
   case MapLibKeyInterface::NO_KEY:
      break;

   }

   return true;
}

bool MapLibKeyHandler::handlePointerEvent(MapLibKeyInterface::pointerType key,
                                          MapLibKeyInterface::kindOfPointerType type,
                                          const WFAPI::ScreenPoint& point,
                                          bool longPress)
{
   // if(type != POINTER_DOWN_EVENT) {
   //    if(!m_draggingManipulator.isWorking()) {
   //       return true;
   //    }
   // }
   
   switch(type) {
   case MapLibKeyInterface::POINTER_DOWN_EVENT: {
      m_clickTracking.accumulatedDiff = 0;
      m_clickTracking.startPressPoint = point;
      m_draggingManipulator.attemptDrag(point);
   }
      break;
   case MapLibKeyInterface::POINTER_UP_EVENT: {
      
      m_draggingManipulator.endDrag(point);
      int diff = abs(m_clickTracking.startPressPoint.getX() - point.getX()) +
         abs(m_clickTracking.startPressPoint.getY() - point.getY());
      if(diff <= MAX_POINTER_DEVIATION_PX) {
         TileMapHandlerClickResult res;

         setRequest(point, false);
      
         m_handler->getInfoForFeatureAt(res, point, false, this);

         // MC2Coordinate targetCoord;
         // m_handler->getMapProjectionAdapter().inverseTransform(targetCoord,
         //                                                        point);
         
         // m_animatedManipulator.attemptAnimatePosition(targetCoord, NULL);

         m_validRequest = false;
         notifySelectedMapObject(res, longPress);
         
      }
   }
      
      break;
   case MapLibKeyInterface::POINTER_UPDATE_EVENT:
      m_clickTracking.accumulatedDiff +=
         abs(point.getX() - m_clickTracking.prevPoint.getX());

      m_clickTracking.accumulatedDiff +=
         abs(point.getY() - m_clickTracking.prevPoint.getY());
      
      m_clickTracking.prevPoint = point;

      m_draggingManipulator.updateDrag(point);
      break;
   case MapLibKeyInterface::POINTER_UNKNOWN_EVENT:
      break;
   }

   return true;
}

void
MapLibKeyHandler::notifySelectedMapObject(const TileMapHandlerClickResult& res,
                                          bool longPress)
{

   MapProjectionAdapter& adapter = m_handler->getMapProjectionAdapter();

   MC2Coordinate coord;
   
   adapter.inverseTransform(coord, m_reqPoint);

   WGS84Coordinate wgsCoord(coord);

   if(!res.getNameAsString().empty()) {  
      WFAPI::MapObjectInfo objInfo(res.getName(),
                                   WFAPI::MapObjectInfo::INTEGRATED_MAP_FEATURE,
                                   0);
   
      m_notifier->notifyListeners(objInfo, NULL, wgsCoord, longPress);
   } else {
      WFAPI::MapObjectInfo objInfo("",
                                   WFAPI::MapObjectInfo::INVALID_FEATURE,
                                   0);
   
      m_notifier->notifyListeners(objInfo, NULL, wgsCoord, longPress);
   }
}

void MapLibKeyHandler::stop()
{
   m_smoothManipulator.stopWorking();
   m_draggingManipulator.stopWorking();
}

void MapLibKeyHandler::setRequest(MC2Point reqPoint,
                                  bool longPress)
{
   m_reqPoint = reqPoint;
   m_validRequest = true;
   m_longPressRequest = longPress;
}

void MapLibKeyHandler::projectionUpdated(projectionEvent trigger)
{
   m_validRequest = false;
}

void MapLibKeyHandler::notifyInfoNamesAvailable()
{   
   if(m_validRequest) {
      TileMapHandlerClickResult res;
      m_handler->getInfoForFeatureAt(res,
                                     m_reqPoint,
                                     true,
                                     NULL);

      notifySelectedMapObject(res, m_longPressRequest);
      
      m_validRequest = false;
   }
}



