/*
Copyright (c) 1999 - 2010, Vodafone Group Services Ltd
All rights reserved.

Redistribution and use in source and binary forms, with or without modification, are permitted provided that the following conditions are met:

    * Redistributions of source code must retain the above copyright notice, this list of conditions and the following disclaimer.
    * Redistributions in binary form must reproduce the above copyright notice, this list of conditions and the following disclaimer in the documentation and/or other materials provided with the distribution.
    * Neither the name of the Vodafone Group Services Ltd nor the names of its contributors may be used to endorse or promote products derived from this software without specific prior written permission.

THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS" AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE ARE DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT HOLDER OR CONTRIBUTORS BE LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
*/


#ifndef _MAPLIBKEYINTERFACEIMPL_H_
#define _MAPLIBKEYINTERFACEIMPL_H_

#include "PALMachineTypes.h"
#include "MapLibKeyInterface.h"
#include "ScreenPoint.h"

class OverlayViewInputInterface;
class MapLibKeyHandler;
class TileMapToolkit;

namespace WFAPI {
class InternalConfig;
class ScreenPoint;
}

template <class T> class TileMapTimerListenerTemplate;

class MapLibKeyInterfaceImpl {
public:
   MapLibKeyInterfaceImpl(MapLib* mapLib);

   virtual ~MapLibKeyInterfaceImpl();

   /**
    * Handles the press of a key. If the type of event is
    * KEY_DOWN_EVENT, the map will move in an accelerating
    * manner until the same key is sent with a KEY_UP_EVENT.
    * 
    * @param key The key which was pressed.
    * @param type Type of event.
    * @return True if the key was consumed.
    */
   bool handleKeyEvent(WFAPI::MapLibKeyInterface::keyType key,
                       WFAPI::MapLibKeyInterface::kindOfPressType type);
   
   /**
    * Handles the press of a key. If the type of event is
    * KEY_DOWN_EVENT, the map will move in an accelerating
    * manner until the same key is sent with a KEY_UP_EVENT.
    * Use this method when supplying a DRAG_TO key event.
    * 
    * @param key The key which was pressed.
    * @param type Type of event.
    * @param dragToPoint Contains the current position 
    *                    of the pointer device.
    * @return True if the key was consumed.
    */
   bool handlePointerEvent(WFAPI::MapLibKeyInterface::pointerType key,
                           WFAPI::MapLibKeyInterface::kindOfPointerType type,
                           const WFAPI::ScreenPoint& dragToPoint);

   bool handleLongPressPointerEvent(WFAPI::MapLibKeyInterface::pointerType key,
                                    const WFAPI::ScreenPoint& point);   
   
   /**
    * Stops the current action 
    * (i.e. in case KEY_UP_EVENT has not been received yet).
    */
   void stop();

   void timerExpired(WFAPI::wf_uint32 id);

   void cancelLongPressTimer();

   void requestLongPressTimer();
private:
   
   OverlayViewInputInterface* m_overlayHandler;
   MapLibKeyHandler* m_maplibHandler;
   WFAPI::InternalConfig* m_internalConfig;
   TileMapToolkit* m_toolkit;
   WFAPI::wf_uint32 m_longPressTimerId;
   TileMapTimerListenerTemplate<MapLibKeyInterfaceImpl>* m_timerListener;
   WFAPI::ScreenPoint m_longPressScreenPoint;
   WFAPI::MapLibKeyInterface::pointerType m_longPressPointerType;
   bool m_issuedLongPress;
};


#endif /* _MAPLIBKEYINTERFACEIMPL_H_ */
