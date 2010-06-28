/*
Copyright (c) 1999 - 2010, Vodafone Group Services Ltd
All rights reserved.

Redistribution and use in source and binary forms, with or without modification, are permitted provided that the following conditions are met:

    * Redistributions of source code must retain the above copyright notice, this list of conditions and the following disclaimer.
    * Redistributions in binary form must reproduce the above copyright notice, this list of conditions and the following disclaimer in the documentation and/or other materials provided with the distribution.
    * Neither the name of the Vodafone Group Services Ltd nor the names of its contributors may be used to endorse or promote products derived from this software without specific prior written permission.

THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS" AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE ARE DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT HOLDER OR CONTRIBUTORS BE LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
*/


#ifndef SMOOTHMAPMANIPULATOR_H
#define SMOOTHMAPMANIPULATOR_H

class TileMapToolkit;
class TileMapPeriodicTimer;
class StressTester;
template <class T> class TileMapTimerListenerTemplate;

namespace WFAPI{
class MapOperationInterface;
class MapDrawingInterface;
}

class MapManipulator;

#include "MC2Point.h"
#include "MC2Coordinate.h"

/**
 * This class is used to create smooth movements when holding down
 * movement keys.
 */
class SmoothMapManipulator {
public:
   /**
    * Constructor.
    * @param parent The parent holding state information about the map
    *               manipulators.
    * @drawingInterface The drawing interface used to request repaints.
    * @toolkit The toolkit to use when creating timers.
    */
   SmoothMapManipulator(MapManipulator& parent,
                        WFAPI::MapDrawingInterface* drawingInterface,
                        TileMapToolkit* toolkit);

   /**
    * Sets the map operation interface to use when manipulating the map.
    *
    * @param operationInterface Handle to the operationInterface.
    */
   void
   setMapOperationInterface(WFAPI::MapOperationInterface* operationInterface);

   /**
    * Destructor.
    */
   ~SmoothMapManipulator();

   /**
    * Attempts to initiate a "MoveUp" movement.
    * @return True if the "MoveUp" movement is initiated.
    */
   bool attemptMoveUp();

   /**
    * Ends the "MoveUp" movement.
    */
   void endMoveUp();

   /**
    * Attempts to initiate a "MoveDown" movement.
    * @return True if the "MoveDown" movement is initiated.
    */
   bool attemptMoveDown();

   /**
    * Ends the "MoveDown" movement.
    */
   void endMoveDown();

   /**
    * Attempts to initiate a "MoveLeft" movement.
    * @return True if the "MoveLeft" movement is initiated.
    */
   bool attemptMoveLeft();

   /**
    * Ends the "MoveLeft" movement.
    */
   void endMoveLeft();

   /**
    * Attempts to initiate a "MoveRight" movement.
    * @return True if the "MoveRight" movement is initiated.
    */
   bool attemptMoveRight();

   /**
    * Ends the "MoveRight" movement.
    */
   void endMoveRight();

   /**
    * Attempts to initiate a "MoveUpAndRight" movement.
    * @return True if the "MoveUpAndRight" movement is initiated.
    */
   bool attemptMoveUpAndRight();

   /**
    * Ends the "MoveUpAndRight" movement.
    */
   void endMoveUpAndRight();

   /**
    * Attempts to initiate a "MoveUpAndLeft" movement.
    * @return True if the "MoveUpAndLeft" movement is initiated.
    */
   bool attemptMoveUpAndLeft();

   /**
    * Ends the "MoveUpAndLeft" movement.
    */
   void endMoveUpAndLeft();

   /**
    * Attempts to reset rotation
    * @return True if the rotation was reset.
    */
   bool attemptResetRotation();

   /**
    * Attempts to initiate a "MoveDownAndRight" movement.
    * @return True if the "MoveDownAndRight" movement is initiated.
    */
   bool attemptMoveDownAndRight();

   /**
    * Ends the "MoveDownAndRight" movement.
    */
   void endMoveDownAndRight();

   /**
    * Attempts to initiate a "MoveDownAndLeft" movement.
    * @return True if the "MoveDownAndLeft" movement is initiated.
    */
   bool attemptMoveDownAndLeft();

   /**
    * Ends the "MoveDownAndLeft" movement.
    */
   void endMoveDownAndLeft();      

   /**
    * Attempts to initiate a "RotateLeft" movement.
    * @return True if the "RotateLeft" movement is initiated.
    */
   bool attemptRotateLeft();

   /**
    * Ends the "RotateLeft" movement.
    */
   void endRotateLeft();

   /**
    * Attempts to initiate a "RotateRight" movement.
    * @return True if the "RotateRight" movement is initiated.
    */
   bool attemptRotateRight();

   /**
    * Ends the "RotateRight" movement.
    */
   void endRotateRight();

   /**
    * Attempts to initiate a "ZoomIn" movement.
    * @return True if the "ZoomIn" movement is initiated.
    */
   bool attemptZoomIn();

   /**
    * Ends the "ZoomIn" movement.
    */
   void endZoomIn();

   /**
    * Attempts to initiate a "ZoomOut" movement.
    * @return True if the "ZoomOut" movement is initiated.
    */
   bool attemptZoomOut();

   /**
    * Ends the "ZoomOut" movement.
    */
   void endZoomOut();

   /**
    * Attempts to zoom in while locking a coordinate to a specific
    * screen point.
    *
    * @param point The screen point to lock the coordinate to.
    * @param coord The screen coord to lock to the point.
    * @return True if the movement is initiated.
    */
   bool attemptZoomInPoint(const MC2Point& point,
                         const MC2Coordinate& coord);

   /**
    * @return True if the manipulator is currently zooming in on a point.
    */
   bool currentlyZoomingInPoint() const;

   /**
    * Ends the "zoomInPoint" movement.
    */
   void endZoomInPoint(const MC2Point& point, const MC2Coordinate& coord);

   /**
    * @return True if the manipulator is currently working.
    */
   bool isWorking();

   /**
    * Stops the manipulator.
    */
   void stopWorking();

   /**
    * This is used to handle timers.
    */
   void timerExpired(uint32);
private:

   /**
    * Internal enum that describes the different movement types.
    */
   enum OperationType {
      /// Move up.
      MOVE_UP = 0,
      /// Move down.
      MOVE_DOWN,
      /// Move left.
      MOVE_LEFT,
      /// Move right.
      MOVE_RIGHT,
      /// Move up and right.
      MOVE_UP_AND_RIGHT,
      /// Move down and right.
      MOVE_DOWN_AND_RIGHT,
      /// Move up and left.
      MOVE_UP_AND_LEFT,
      /// Move down and left.
      MOVE_DOWN_AND_LEFT,
      /// Zoom in.
      ZOOM_IN,
      /// Zoom out.
      ZOOM_OUT,
      /// Zoom in.
      ZOOM_IN_POINT,
      /// Rotate the map counter clockwise.
      ROTATE_LEFT,
      /// Rotate the map clockwise.
      ROTATE_RIGHT,
      ///
      NUM_SMOOTH_OPERATIONS 
   };

   /**
    * This function reflects the long list of attempt"Operation" previously
    * described.
    * 
    * @param operation The type of operation.
    * @return True if the operation successfully initiated.
    */
   bool attemptOperation(OperationType operation);

   /**
    * Ends the operation.
    *
    * @param operation The type of operation to end.
    */
   void endOperation(OperationType operation);
   
   /// State of the operator
   enum operationState {
      MOVING,
      ROTATING,
      STATIC,
      ZOOMING
   };
   
   /// Moves the map by x, y pixels
   void move(int32 moveDeltaX, int32 moveDeltaY);
   
   /// Sets the current operation state
   void setMapState(operationState group);

   /// Classifies which state a certain operation belongs to
   operationState classify(OperationType type);

   /// Resets all active times
   void resetTimes();

   /// Updates the current state, to be run periodically
   void updateState();

   /// Updates the moving state.
   void updateMovingState();
   /// Updates the moving state according to a specific time.
   void updateMovingState(uint32 keyTime);
   
   /// Updates the rotating state
   void updateRotatingState();
   /// Updates the rotating state according to a specific time.
   void updateRotatingState(uint32 keyTime);

   /// Updates the static state.
   void updateStaticState();

   /// Updates the zooming state
   void updateZoomingState();
   /// Updates the zooming state according to a specific time.
   void updateZoomingState(uint32 keyTime);

   /// Updates the zoomingtopoint state
   void updateZoomingToPointState();
   /// Updates the zoomingtopoint state according to a specific time.
   void updateZoomingToPointState(uint32 keyTime);
   
   /**
    *  Calculates a delta for movement, rotation or zooming etc.
    *
    *  @param threshold The thresholds, which are time intervals that
    *                   dictate which deltas should be used.
    *  @param deltas The deltas. Directly corresponds to the intervals
    *                in threshold.
    *  @param maxValues The maximum values for the deltas. Directly corresponds
    *                   to the values in deltas.
    *  @param numTresholds The number of elements in the thresholds,
    *                      deltas and maxValues arrays.
    *  @param firstKeyPress The time of the first operation.
    *
    *  @return A delta suitable for movement, rotation etc.
    */  
   float32 getDelta(const float32* thresholds,
                    const float32* deltas,
                    const float32* maxValues,
                    int nbrThresholds,
                    uint32 firstKeyPress);
   
   /// The state variables for the rotation
   struct {
      uint32 delta;
   } m_rotationState;

   /// The state variables for moving
   struct {
      uint32 delta;
   } m_movingState;

   /// The state variables for zooming
   struct {
      float inFactor;
      float outFactor;
      bool zoomingToPoint;
      MC2Point point;
      MC2Coordinate coord;
   } m_zoomingState;

   /// The parent map manipulator.
   MapManipulator& m_mapManipulator;
   
   /// Periodic timer for key-repeats
   TileMapPeriodicTimer* m_timer;

   /// Listener
   TileMapTimerListenerTemplate<SmoothMapManipulator>* m_timerListener;
   
   /// The current operation state
   operationState m_operationState;

   /// The registered times for the different keys
   unsigned int m_active[NUM_SMOOTH_OPERATIONS];
   
   /// The time of the last drawn frame.
   uint32 m_lastDrawTime;

   /// Our map operation interface, used to notify when moving
   WFAPI::MapOperationInterface* m_operationInterface;

   /// Our drawing interface, used to notify of redraws
   WFAPI::MapDrawingInterface* m_drawingInterface;

   /// Used for stresstesting.
   StressTester* m_stressTester;
};

#endif // SMOOTHMAPMANIPULATOR_H
