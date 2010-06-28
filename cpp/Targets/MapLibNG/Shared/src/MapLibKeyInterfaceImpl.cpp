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
#define CORE_LOGGING_MODULE_NAME "MapLibKeyInterfaceImpl"

#include "config.h"
#include "MapLibKeyInterface.h"
#include "InternalConfig.h"
#include "MapLibKeyHandler.h"
#include "OverlayViewInputInterface.h"
#include "CoreDebugPrint.h"
#include "MapLib.h"
#include "TileMapToolkit.h"
#include "MapLibKeyInterfaceImpl.h"
#include "CommonData.h"

using namespace WFAPI;

MapLibKeyInterfaceImpl::MapLibKeyInterfaceImpl(MapLib* mapLib)
{
   m_overlayHandler = mapLib->getOverlayInputHandler();
   m_maplibHandler = mapLib->getMapLibInputHandler();
   m_internalConfig = mapLib->getInternalConfig();
   m_toolkit = mapLib->getToolkit();
   m_longPressTimerId = 0;
   m_issuedLongPress = false;

   m_timerListener =
      new TileMapTimerListenerTemplate<MapLibKeyInterfaceImpl>(this);
}

MapLibKeyInterfaceImpl::~MapLibKeyInterfaceImpl()
{
   delete m_timerListener;
}

bool MapLibKeyInterfaceImpl::handleKeyEvent(MapLibKeyInterface::keyType key,
                                            MapLibKeyInterface::kindOfPressType type)
{
   if (m_internalConfig->get3dMode()){
      // Currently we don't allow any input when in 3d mode.
      return false;
   }

   if(m_overlayHandler->handleKeyEvent(key, type)) {
      return true;
   }

   if(m_maplibHandler->handleKeyEvent(key, type)) {
      return true;
   }

   return false; 
}

bool MapLibKeyInterfaceImpl::handlePointerEvent(
   MapLibKeyInterface::pointerType key,
   MapLibKeyInterface::kindOfPointerType type,
   const ScreenPoint& dragToPoint)
{
   if (m_internalConfig->get3dMode()){
      // Currently we don't allow any input when in 3d mode.
      return false;
   }

   switch(type) {
   case MapLibKeyInterface::POINTER_DOWN_EVENT: {
      m_longPressScreenPoint = dragToPoint;
      m_longPressPointerType = key;
      cancelLongPressTimer();
      requestLongPressTimer();
      m_issuedLongPress = false;
      break;
   }
   case MapLibKeyInterface::POINTER_UP_EVENT: {
      cancelLongPressTimer();      
      if(m_issuedLongPress) {
         return false;
      }
      break;
   }
   case MapLibKeyInterface::POINTER_UPDATE_EVENT: {
      int diff = abs(m_longPressScreenPoint.getX() - dragToPoint.getX()) +
         abs(m_longPressScreenPoint.getY() - dragToPoint.getY());
      if(diff > MAX_POINTER_DEVIATION_PX){
         cancelLongPressTimer();
      }
      if(m_issuedLongPress) {
         return false;
      }
      break;
   }
   case MapLibKeyInterface::POINTER_UNKNOWN_EVENT: {
      break;
   }
   }
   
   if(m_overlayHandler->handlePointerEvent(key, type, dragToPoint, false)) {
      return true;
   }    
   
   if(m_maplibHandler->handlePointerEvent(key, type, dragToPoint, false)) {
      return true;
   }
   
   return false;      
}

void MapLibKeyInterfaceImpl::stop()
{


   m_overlayHandler->stop();
   m_maplibHandler->stop();
}

void
MapLibKeyInterfaceImpl::requestLongPressTimer()
{
   cancelLongPressTimer();

   m_longPressTimerId =
      m_toolkit->requestTimer(m_timerListener, LONGPRESS_TIME_MS);
}

void
MapLibKeyInterfaceImpl::cancelLongPressTimer()
{
   if(m_longPressTimerId) {
      m_toolkit->cancelTimer(m_timerListener, m_longPressTimerId);
      m_longPressTimerId = 0;
   }
}

void
MapLibKeyInterfaceImpl::timerExpired(wf_uint32 id)
{
   handleLongPressPointerEvent(m_longPressPointerType, m_longPressScreenPoint);
   m_longPressTimerId = 0;
}

bool
MapLibKeyInterfaceImpl::handleLongPressPointerEvent(
   MapLibKeyInterface::pointerType key,
   const ScreenPoint& point)
{
   m_issuedLongPress = true;
   if(m_overlayHandler->handlePointerEvent(key,
                                           MapLibKeyInterface::POINTER_UP_EVENT,
                                           point,
                                           true)) {
      return true;
   }
   if(m_maplibHandler->handlePointerEvent(key,
                                          MapLibKeyInterface::POINTER_UP_EVENT,
                                          point,
                                          true)) {
      return true;
   }
   return false; 
}
