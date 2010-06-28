/*
  Copyright (c) 1999 - 2010, Vodafone Group Services Ltd
  All rights reserved.

  Redistribution and use in source and binary forms, with or without modification, are permitted provided that the following conditions are met:

  * Redistributions of source code must retain the above copyright notice, this list of conditions and the following disclaimer.
  * Redistributions in binary form must reproduce the above copyright notice, this list of conditions and the following disclaimer in the documentation and/or other materials provided with the distribution.
  * Neither the name of the Vodafone Group Services Ltd nor the names of its contributors may be used to endorse or promote products derived from this software without specific prior written permission.

  THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS" AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE ARE DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT HOLDER OR CONTRIBUTORS BE LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
*/

#ifndef WFAPI_MAPLIBKEYINTERFACE_H
#define WFAPI_MAPLIBKEYINTERFACE_H

class MapLib;
class MapLibKeyInterfaceImpl;

namespace WFAPI {
// Forward declarations
class ScreenPoint;

/**
 * The interface for handling key and pointer events.
 */
class MapLibKeyInterface {
public:

   /**
    * Constructor.
    *
    * @param maplib Internal class.
    */
   MapLibKeyInterface(MapLib* maplib);
   
   /**
    * Virtual destructor.
    */ 
   virtual ~MapLibKeyInterface();
   
   /// Symbolic key action names.
   enum keyType {
      /// Select key
      SELECT_KEY = 0,
      /// Move up.
      MOVE_UP_KEY,
      /// Move down.
      MOVE_DOWN_KEY,
      /// Move left.
      MOVE_LEFT_KEY,
      /// Move right.
      MOVE_RIGHT_KEY,
      /// Zoom in.
      ZOOM_IN_KEY,
      /// Zoom out.
      ZOOM_OUT_KEY,
      /// Rotate the map counter clockwise.
      ROTATE_LEFT_KEY,
      /// Rotate the map clockwise.
      ROTATE_RIGHT_KEY,
      /// Set rotation so that north is up.
      RESET_ROTATION_KEY,
      /// Move up and right.
      MOVE_UP_RIGHT_KEY,
      /// Move down and right.
      MOVE_DOWN_RIGHT_KEY,
      /// Move up and left.
      MOVE_UP_LEFT_KEY,
      /// Move down and left.
      MOVE_DOWN_LEFT_KEY,
      /// This is solely an internal event. Will cause no action.
      INTERNAL_TRIGGER_KEY,
      /// This is solely an internal event. Will cause no action.
      INTERNAL_TRIGGER_KEY_2,
      /// Send this if nothing should be done.
      NO_KEY,
   };

   /**
    * Type if dragging pointer.
    */
   enum pointerType {
      /// Drag the map, using a pointer device.
      DRAG_TO = NO_KEY + 1,
      /// Send this if nothing should be done.
      NO_POINTER
   };
   
   /// Kind of keypress
   enum kindOfPressType {
      /// The key was pressed
      KEY_UP_EVENT,
      /// The key was released
      KEY_DOWN_EVENT,
      /// Unknown type of event
      KEY_UNKNOWN_EVENT,
   };

   /// Kind of pointer type
   enum kindOfPointerType {
      /// The pointer was pressed
      POINTER_UP_EVENT,
      /// The pointer was released
      POINTER_DOWN_EVENT,
      /// The pointer was updated
      POINTER_UPDATE_EVENT,
      /// Unknown type of event
      POINTER_UNKNOWN_EVENT,
   };
   
   /**
    * Handles the press of a key. If the type of event is
    * KEY_DOWN_EVENT, the map will move in an accelerating
    * manner until the same key is sent with a KEY_UP_EVENT.
    * 
    * @param key The key which was pressed.
    * @param type Type of event.
    * @return True if the key was consumed.
    */
   bool handleKeyEvent(keyType key, kindOfPressType type);
   
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
   bool handlePointerEvent(pointerType key,
                           kindOfPointerType type,
                           const WFAPI::ScreenPoint& dragToPoint);   
   
   /**
    * Stops the current action 
    * (i.e. in case KEY_UP_EVENT has not been received yet).
    */
   void stop();

private:

   /**
    * We do not allow copies of this interface.
    */
   MapLibKeyInterface(const MapLibKeyInterface&);
   const MapLibKeyInterface& operator=(const MapLibKeyInterface&);

   MapLibKeyInterfaceImpl* m_impl;
};

} // End of namespace WFAPI

#endif // WFAPI_MAPLIBKEYINTERFACE_H
