/*
Copyright (c) 1999 - 2010, Vodafone Group Services Ltd
All rights reserved.

Redistribution and use in source and binary forms, with or without modification, are permitted provided that the following conditions are met:

    * Redistributions of source code must retain the above copyright notice, this list of conditions and the following disclaimer.
    * Redistributions in binary form must reproduce the above copyright notice, this list of conditions and the following disclaimer in the documentation and/or other materials provided with the distribution.
    * Neither the name of the Vodafone Group Services Ltd nor the names of its contributors may be used to endorse or promote products derived from this software without specific prior written permission.

THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS" AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE ARE DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT HOLDER OR CONTRIBUTORS BE LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
*/


#ifndef MAPLIBKEYHANDLER_H
#define MAPLIBKEYHANDLER_H

#include "MapLibKeyInterface.h"
#include "MC2Point.h"
#include "MC2Coordinate.h"
#include "TileMapInfoCallback.h"
#include "MapProjection.h"

class MapManipulator;
class SmoothMapManipulator;
class DraggingMapManipulator;
class AnimatedMapManipulator;
class TileMapHandler;
class SelectedMapObjectNotifier;
class TileMapHandlerClickResult;

/**
 * The class that handles the default behaviour for keypresses.
 */
class MapLibKeyHandler : public TileMapInfoCallback,
                         public MapProjectionListener {
public:
   /**
    * Constructor.
    * @param mapManipulator The map manipulator to use for moving the map etc.
    * @param tileMapHandler Used to fetch names for clicked objects etc.
    * @param notifier A notifier containing listeners that are interested in
    *                 selection of objects.
    */
   MapLibKeyHandler(MapManipulator& mapManipulator,
                    TileMapHandler* tileMapHandler,
                    SelectedMapObjectNotifier* notifier);

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
    * @see MapLibKeyInterface::stop
    */ 
   void stop();

   virtual void projectionUpdated(projectionEvent trigger);
   
   virtual void notifyInfoNamesAvailable();

private:

   struct {
      int accumulatedDiff;
      WFAPI::ScreenPoint startPressPoint;
      WFAPI::ScreenPoint prevPoint;
   } m_clickTracking;

   /**
    * Sets the point of selection for an map object info request.
    *
    * @param reqPoint The screen point for the request.
    * @param longPress Whether or not the request is from a long press
    */
   void setRequest(MC2Point reqPoint, bool longPress);

   /**
    * Notifies listeners that information for a requested map object is
    * available.
    *
   * @param longPress Whether or not the request is from a long press
    */
   void notifySelectedMapObject(const TileMapHandlerClickResult& res,
                                bool longPress);

   /// Indicates whether the current info request is valid.
   bool m_validRequest;
   /// Indicates whether the current info request is from a long press
   bool m_longPressRequest;

   /// The point of the current info request.
   MC2Point m_reqPoint;
   /// The map manipulator used for smooth manipulation.
   SmoothMapManipulator& m_smoothManipulator;
   /// The map manipulator used for dragging manipulation.
   DraggingMapManipulator& m_draggingManipulator;
   /// The map manipulator used for animated manipulation.
   AnimatedMapManipulator& m_animatedManipulator;
   /// Used to fetch names for clicked objects etc.
   TileMapHandler* m_handler;
   /// The notifier that holds and notifies all of the listeners.
   SelectedMapObjectNotifier* m_notifier;
};

#endif // MAPLIBKEYHANDLER_H
