/*
Copyright (c) 1999 - 2010, Vodafone Group Services Ltd
All rights reserved.

Redistribution and use in source and binary forms, with or without modification, are permitted provided that the following conditions are met:

    * Redistributions of source code must retain the above copyright notice, this list of conditions and the following disclaimer.
    * Redistributions in binary form must reproduce the above copyright notice, this list of conditions and the following disclaimer in the documentation and/or other materials provided with the distribution.
    * Neither the name of the Vodafone Group Services Ltd nor the names of its contributors may be used to endorse or promote products derived from this software without specific prior written permission.

THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS" AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE ARE DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT HOLDER OR CONTRIBUTORS BE LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
*/


#ifndef DRAGGINGMAPMANIPULATOR_H
#define DRAGGINGMAPMANIPULATOR_H

class MapManipulator;

#include "MC2Point.h"
#include "MC2Coordinate.h"

namespace WFAPI {
class ScreenPoint;
class MapOperationInterface;
class MapDrawingInterface;
}

/**
 * The class that handles dragging manipulation of the map.
 */
class DraggingMapManipulator {
public:

   /**
    * Constructor.
    * @param mapManipulator The parent handling the map manipulation state.
    * @param drawingInterface The drawing interface used to request repaints.
    */
   DraggingMapManipulator(MapManipulator& mapManipulator,
                          WFAPI::MapDrawingInterface* drawingInterface);

   /**
    * Sets the map operation interface used to move the map.
    * Must be called to completely initialize the class.
    * 
    * @param operationInterface The map operation interface.
    */
   void setMapOperationInterface(WFAPI::MapOperationInterface* operationInterface);

   /**
    * Attempts to initiate a dragging event.
    * 
    * @param point The point of origin for the dragging event
    * @return True if the dragging event was successfully initiated.
    */
   bool attemptDrag(const WFAPI::ScreenPoint& point);

   /**
    * Ends a dragging event, supplying the last dragging point.
    * 
    * @param point The last point of the dragging event.
    */
   void endDrag(const WFAPI::ScreenPoint& point);

   /**
    * Updates the dragging event with a new point.
    *
    * @param point The current point of the dragging event.
    */
   void updateDrag(const WFAPI::ScreenPoint& point);

   /**
    * @return True if a dragging manipulation is currently
    *         ongoing.
    */
   bool isWorking();

   /**
    * Stops the current dragging manipulation.
    */
   void stopWorking();
   
private:

   /// Enum describing the possible internal states.
   enum State {
      STATIC, DRAGGING
   };

   /**
    * Sets the current state.
    * 
    * @param newState The new state.
    */
   void setState(State newState);

   /**
    * A struct containing necessary state information for a
    * dragging event.
    */
   struct DragState {
      MC2Coordinate dragCoord;
      MC2Point initialPoint;
   };

   /// The parent state controlling map manipulator.
   MapManipulator& m_mapManipulator;
   /// The map operation interface used to move the map.
   WFAPI::MapOperationInterface* m_operationInterface;
   /// The drawing interface used to request repaints.
   WFAPI::MapDrawingInterface* m_drawingInterface;

   /// The current state
   State m_state;
   /// The member containing information for needed for the current state.
   DragState m_dragState;
};

#endif // DRAGGINGMAPMANIPULATOR_H
