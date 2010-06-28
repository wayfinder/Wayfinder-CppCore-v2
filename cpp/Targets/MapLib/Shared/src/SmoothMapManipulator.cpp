/*
Copyright (c) 1999 - 2010, Vodafone Group Services Ltd
All rights reserved.

Redistribution and use in source and binary forms, with or without modification, are permitted provided that the following conditions are met:

    * Redistributions of source code must retain the above copyright notice, this list of conditions and the following disclaimer.
    * Redistributions in binary form must reproduce the above copyright notice, this list of conditions and the following disclaimer in the documentation and/or other materials provided with the distribution.
    * Neither the name of the Vodafone Group Services Ltd nor the names of its contributors may be used to endorse or promote products derived from this software without specific prior written permission.

THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS" AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE ARE DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT HOLDER OR CONTRIBUTORS BE LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
*/


#include "TileMapUtil.h"
#include "SmoothMapManipulator.h"
#include "TileMapToolkit.h"
#include "MapOperationInterface.h"
#include "MapDrawingInterface.h"
#include "MapManipulator.h"
#include <math.h>
#ifdef __SYMBIAN__
#include <e32math.h>
#endif
        
#include "MC2Point.h"
#include "Utility.h"


// millisecond thresholds for keypresses
#define THRESHOLD_ONE 250
#define THRESHOLD_TWO 500
#define THRESHOLD_THREE 1000

// Default delta for movement.
#define  MOVEDELTA_DEFAULT  2

// Threshold and default/max values for rotation
#define  ROTDELTA_DEFAULT  15
#define  ROTDELTA_THRESHOLD_ONE  60
#define  ROTDELTA_THRESHOLD_TWO  60
#define  ROTDELTA_THRESHOLD_THREE  60
#define  ROTDELTA_THRESHOLD_FOUR  60
#define  ROTDELTA_MAX  90

// Threshold values for the zoom
#define  ZOOMDELTA_THRESHOLD_ONE  0.40546f  // ln(1.5) = 0.40546
#define  ZOOMDELTA_THRESHOLD_TWO  0.55961f  // ln(1.75) = 0.55961f
#define  ZOOMDELTA_THRESHOLD_THREE 0.69314f // ln(2.0) = 0.69314f
#define  ZOOMDELTA_THRESHOLD_FOUR 0.91629f  // ln(2.5) = 0.91629f
#define  ZOOMDELTA_MAX           0.40546f

#define TIMER_PERIOD 10

SmoothMapManipulator::SmoothMapManipulator(
   MapManipulator& mapManipulator,
   WFAPI::MapDrawingInterface* drawingInterface,
   TileMapToolkit* toolkit) :
   m_mapManipulator(mapManipulator),
   m_operationState(STATIC),
   m_operationInterface(NULL),
   m_drawingInterface(drawingInterface)
{
   resetTimes();
   m_lastDrawTime = 0;
   m_zoomingState.inFactor = 1.0f;
   m_zoomingState.outFactor = 1.0f;
   m_movingState.delta = 0;
   m_rotationState.delta = 0;  
   
   m_timerListener  =
      new TileMapTimerListenerTemplate<SmoothMapManipulator>(this);
   
   m_timer =      
      toolkit->createPeriodicTimer( m_timerListener,
                                    TileMapToolkit::PRIO_STANDARD);

}

void SmoothMapManipulator::resetTimes()
{
   for(int i = 0; i < NUM_SMOOTH_OPERATIONS; i++) {
      m_active[i] = 0;
   }
}

bool SmoothMapManipulator::attemptOperation(OperationType operation)
{
   bool alreadyActive = m_active[operation];
      
   if(!m_mapManipulator.okayToStart(MapManipulator::SMOOTH)) {
      return false;
   } else {
      if(!alreadyActive) {
         m_mapManipulator.setIsStarting(MapManipulator::DRAGGING);         
      }
   }

   
   if(!m_active[operation]) {
      resetTimes();
      m_active[operation] = TileMapUtil::currentTimeMillis();
      m_lastDrawTime = TileMapUtil::currentTimeMillis();
      m_timer->start(TIMER_PERIOD);
   }
   
   operationState state = classify(operation);

   if( state != m_operationState ) {
      setMapState(state);
   }

   return true;
}

SmoothMapManipulator::operationState
SmoothMapManipulator::classify(OperationType type)
{
   switch(type) {
   case MOVE_UP:
   case MOVE_DOWN:
   case MOVE_LEFT:
   case MOVE_RIGHT:
   case MOVE_UP_AND_RIGHT:
   case MOVE_DOWN_AND_RIGHT:
   case MOVE_UP_AND_LEFT:
   case MOVE_DOWN_AND_LEFT:
      return MOVING;
   case ZOOM_IN_POINT:
   case ZOOM_IN:
   case ZOOM_OUT:
      return ZOOMING;
   case ROTATE_LEFT:
   case ROTATE_RIGHT:
      return ROTATING;
   default:
      return STATIC;
   }
}

void SmoothMapManipulator::endOperation(OperationType operation)
{
   // Currently, ending one operation will stop all operations.
   // Just as beginning one will stop all.
   
   m_active[operation] = 0;
   resetTimes();
   m_timer->stop();
   
   setMapState(STATIC);
}

void SmoothMapManipulator::setMapState(operationState group)
{
   if(group != STATIC) {
      m_operationInterface->setMovementMode(true);
   } else {
      m_operationInterface->setMovementMode(false);  
   }
   
   m_operationState = group;
}

void SmoothMapManipulator::updateState() {   
   switch(m_operationState) {
   case MOVING:
      updateMovingState();
      break;
   case ROTATING:
      updateRotatingState();
      break;
   case STATIC:
      updateStaticState();
      break;
   case ZOOMING:
      updateZoomingState();
      break;
   }
}

void SmoothMapManipulator::updateMovingState()
{
   MC2Point moveDelta;
   
    if( m_active[ MOVE_DOWN ]) {
       updateMovingState(m_active[ MOVE_DOWN ]);
       moveDelta = MC2Point(0, m_movingState.delta);
    }
    else if( m_active[ MOVE_UP ] ) {
       updateMovingState( m_active[ MOVE_UP ] );
       moveDelta = MC2Point(0, -m_movingState.delta);
    }
    else if( m_active[ MOVE_LEFT ] ) {
       updateMovingState( m_active[ MOVE_LEFT ] );
       moveDelta = MC2Point(-m_movingState.delta, 0);
    }
    else if( m_active[ MOVE_RIGHT ] ) {
       updateMovingState( m_active[ MOVE_RIGHT ] );
       moveDelta = MC2Point(m_movingState.delta, 0);
    }
    else if( m_active[ MOVE_DOWN_AND_RIGHT ]) {
       updateMovingState(m_active[ MOVE_DOWN_AND_RIGHT ]);
       moveDelta = MC2Point(m_movingState.delta, m_movingState.delta);
    }
    else if( m_active[ MOVE_UP_AND_RIGHT ] ) {
       updateMovingState( m_active[ MOVE_UP_AND_RIGHT ] );
       moveDelta = MC2Point(m_movingState.delta, -m_movingState.delta);
    }
    else if( m_active[ MOVE_DOWN_AND_LEFT ]) {
       updateMovingState(m_active[ MOVE_DOWN_AND_LEFT ]);
       moveDelta = MC2Point(-m_movingState.delta, m_movingState.delta);
    }
    else if( m_active[ MOVE_UP_AND_LEFT ] ) {
       updateMovingState( m_active[ MOVE_UP_AND_LEFT ] );
       moveDelta = MC2Point(-m_movingState.delta, -m_movingState.delta);
    }
    
    if(moveDelta.getX() || moveDelta.getY()) {
       m_operationInterface->move(moveDelta.getX(), moveDelta.getY());
       m_drawingInterface->requestRepaint();
    }
}

void SmoothMapManipulator::updateMovingState(uint32 keyTime)
{
   // create threshold and delta arrays
   static const float32
      thresholds[] = { 125,
                       THRESHOLD_ONE,   // 250
                       THRESHOLD_TWO,   // 500
                       THRESHOLD_THREE, // 1000
                       2000,
                       2500,
                       float(MAX_UINT32) };
   
   static const float32 deltas[] = { 0, 50, 100, 150, 150, 150, 150 };
      
#ifndef ENABLE_MAPPERFORMANCE_DEBUGGING 
   // Production values.
   static const float32 maxVals[] = { 0, 7, 7, 10, 15, 50, 50 };
#else   
   // Test values.
   static const float32 maxVals[] = { 0, 1, 1, 1, 1, 1, 1 };
#endif
   // get delta based on time
   float32 delta = getDelta(thresholds,
                            deltas,
                            maxVals, 
                            sizeof(thresholds) / sizeof( thresholds[0] ),
                            keyTime);
   
   /* adjust the value, since it will be truncated to zero
      on conversion to integer */
   if(delta < 1.0f) {
      delta = 1.0f;
   }

   // set movement delta to calculated value
   m_movingState.delta = (uint32) delta;
}

void SmoothMapManipulator::updateRotatingState() {

   int rotateDeg = 0;
   
   
   if(m_active[ROTATE_LEFT]) {
      updateRotatingState(m_active[ROTATE_LEFT]);
      rotateDeg = m_rotationState.delta;
   }
   else if(m_active[ROTATE_RIGHT]) {
      updateRotatingState(m_active[ROTATE_RIGHT]);
      rotateDeg = -m_rotationState.delta;
   }

   if(rotateDeg) {
      m_operationInterface->rotateLeftDeg(rotateDeg);
      m_drawingInterface->requestRepaint();
   }
}
   
void SmoothMapManipulator::updateRotatingState(uint32 keyTime)
{

   
   // create threshold and delta arrays
   static const float32
      thresholds[4] = { THRESHOLD_ONE, THRESHOLD_TWO, THRESHOLD_THREE,
                        float(MAX_UINT32) };
   
   static const float32 deltas[4] = { ROTDELTA_THRESHOLD_ONE, 
                                      ROTDELTA_THRESHOLD_TWO, 
                                      ROTDELTA_THRESHOLD_THREE,
                                      ROTDELTA_THRESHOLD_FOUR };

   static const float32 maxVals[] = { ROTDELTA_MAX, ROTDELTA_MAX, 
                                      ROTDELTA_MAX, ROTDELTA_MAX };

   
   
   // get delta based on time
   float32 delta = getDelta(thresholds, deltas, maxVals,
                            sizeof(thresholds) / sizeof( thresholds[0] ),
                            keyTime);
   
   /* adjust the value, since it will be truncated to zero
      on conversion to integer */
   if(delta < 1.0f) {
      delta = 1.0f;
   }

   // set rotation delta to calculated value
   m_rotationState.delta = (uint32) delta;

   return;    
}


void SmoothMapManipulator::updateStaticState()
{
    
}

void SmoothMapManipulator::updateZoomingToPointState(uint32 keyTime)
{
    
}

void SmoothMapManipulator::updateZoomingState()
{

   
   float zoomFactor = 1.0f;
   
   if(m_active[ZOOM_IN]) {
      updateZoomingState(m_active[ZOOM_IN]);
      zoomFactor = m_zoomingState.inFactor;
   } else if(m_active[ZOOM_OUT]) {
      updateZoomingState(m_active[ZOOM_OUT]);
      zoomFactor = m_zoomingState.outFactor;   
   } else if(m_active[ZOOM_IN_POINT]) {
      updateZoomingState(m_active[ZOOM_IN_POINT]);
      zoomFactor = m_zoomingState.inFactor;
   }
   
   if(zoomFactor != 1.0f) {
      if(m_zoomingState.zoomingToPoint) {
         m_operationInterface->zoom(zoomFactor,
                                       m_zoomingState.coord,
                                       m_zoomingState.point);
      } else {
         m_operationInterface->zoom(zoomFactor);
      }

      m_drawingInterface->requestRepaint();
   }
}

void SmoothMapManipulator::updateZoomingState(uint32 keyTime) {

   // create threshold and delta arrays for zooming
   static const float32
      thresholds[4] = { THRESHOLD_ONE, THRESHOLD_TWO, THRESHOLD_THREE,
                        float(MAX_UINT32) };
   static const float32
      deltas[4] = { ZOOMDELTA_THRESHOLD_ONE, 
                    ZOOMDELTA_THRESHOLD_TWO, 
                    ZOOMDELTA_THRESHOLD_THREE,
                    ZOOMDELTA_THRESHOLD_FOUR };
   
   static const float32 maxVals[] = { ZOOMDELTA_MAX, 
                                      ZOOMDELTA_MAX, 
                                      ZOOMDELTA_MAX, 
                                      ZOOMDELTA_MAX };
   
   // get zoom-in delta based on time
   const float32 delta = getDelta(thresholds,
                                  deltas,
                                  maxVals, 
                                  sizeof(thresholds) / sizeof( thresholds[0] ),
                                  keyTime);

   // set zoom-in delta to calculated value
#ifdef __SYMBIAN32__
   TReal expDelta = 1.0;
   Math::Exp( expDelta, delta );
#else
   double expDelta = exp( delta );
#endif
   
   m_zoomingState.inFactor = 1 / expDelta;
  
   // set zoom-out delta to calculated value
   m_zoomingState.outFactor = expDelta;

   return;
}

float32 SmoothMapManipulator::getDelta(const float32* thresholds,
                                    const float32* deltas,
                                    const float32* maxValues,
                                    int nbrThresholds,
                                    uint32 firstKeyPress)
{
   float32 curDelta = 0;

   uint32 curKeyPress = TileMapUtil::currentTimeMillis();
   
   // calculate the time between keypresses
   uint32 curTime = curKeyPress - firstKeyPress;

   // time since last drawn.
   uint32 timeSinceLastDrawn = curKeyPress - m_lastDrawTime;
   
   float32 maxValue = 0.0;
   
   // check the curTime against the thresholds 
   // (currently, hardcoded for nbr values)
   for ( int i = 0; i < nbrThresholds; ++i ) {
      if ( curTime < thresholds[i] ) {
         curDelta = deltas[i] * ((float32) timeSinceLastDrawn / 1000.0f);
#ifdef ARCH_OS_WINDOWS
         // Until the deltas can be sent into TMKH, 
         // hardcode that windows needs 5 times larger deltas.
         curDelta *= 5;
#endif         
         maxValue = maxValues[ i ];
         break;
      }
   }
   
   // check against maxValue 
   if(curDelta > maxValue) {
      curDelta = maxValue;
   }

   if ( curDelta > 0 ) {
      m_lastDrawTime = curKeyPress;
   }
   
   return(curDelta); 
}

bool SmoothMapManipulator::attemptMoveUp()
{
   return attemptOperation(MOVE_UP);
}

void SmoothMapManipulator::endMoveUp()
{
   endOperation(MOVE_UP);
}

bool SmoothMapManipulator::attemptMoveDown()
{
   return attemptOperation(MOVE_DOWN);
}

void SmoothMapManipulator::endMoveDown()
{
   endOperation(MOVE_DOWN);
}

bool SmoothMapManipulator::attemptMoveLeft()
{
   return attemptOperation(MOVE_LEFT);
}

void SmoothMapManipulator::endMoveLeft()
{
   endOperation(MOVE_LEFT);
}

bool SmoothMapManipulator::attemptMoveRight()
{
   return attemptOperation(MOVE_RIGHT);
}

void SmoothMapManipulator::endMoveRight()
{
   endOperation(MOVE_RIGHT);
}

bool SmoothMapManipulator::attemptMoveUpAndRight()
{
   return attemptOperation(MOVE_UP_AND_RIGHT);
}

void SmoothMapManipulator::endMoveUpAndRight()
{
   endOperation(MOVE_UP_AND_RIGHT);
}

bool SmoothMapManipulator::attemptMoveUpAndLeft()
{
   return attemptOperation(MOVE_UP_AND_LEFT);
}

void SmoothMapManipulator::endMoveUpAndLeft()
{
   endOperation(MOVE_UP_AND_LEFT);
}

bool SmoothMapManipulator::attemptMoveDownAndRight()
{
   return attemptOperation(MOVE_DOWN_AND_RIGHT);
}

void SmoothMapManipulator::endMoveDownAndRight()
{
   endOperation(MOVE_DOWN_AND_RIGHT);
}

bool SmoothMapManipulator::attemptMoveDownAndLeft()
{
   return attemptOperation(MOVE_DOWN_AND_LEFT);
}

void SmoothMapManipulator::endMoveDownAndLeft()
{
   endOperation(MOVE_DOWN_AND_LEFT);
}

bool SmoothMapManipulator::attemptRotateLeft()
{
   return attemptOperation(ROTATE_LEFT);
}

void SmoothMapManipulator::endRotateLeft()
{
   endOperation(ROTATE_LEFT);
}

bool SmoothMapManipulator::attemptRotateRight()
{
   return attemptOperation(ROTATE_RIGHT);
}

void SmoothMapManipulator::endRotateRight()
{
   endOperation(ROTATE_RIGHT);
}

bool SmoothMapManipulator::attemptZoomIn()
{
   m_zoomingState.zoomingToPoint = false;
   return attemptOperation(ZOOM_IN);
}

void SmoothMapManipulator::endZoomIn()
{
   endOperation(ZOOM_IN);
}

bool SmoothMapManipulator::isWorking()
{
   return m_operationState != STATIC;
}

void SmoothMapManipulator::stopWorking()
{
   resetTimes(); 
}

bool SmoothMapManipulator::attemptZoomOut()
{   
   m_zoomingState.zoomingToPoint = false;
   return attemptOperation(ZOOM_OUT);
}

void SmoothMapManipulator::endZoomOut()
{
   endOperation(ZOOM_OUT);
}

void SmoothMapManipulator::timerExpired(uint32)
{
   updateState();

}

bool SmoothMapManipulator::attemptZoomInPoint(const MC2Point& point,
                                              const MC2Coordinate& coord)
{
   m_zoomingState.coord = coord;
   m_zoomingState.point = point;
   m_zoomingState.zoomingToPoint = true;
   return attemptOperation(ZOOM_IN_POINT);
}

void SmoothMapManipulator::endZoomInPoint(const MC2Point& point,
                                       const MC2Coordinate& coord)
{
   endOperation(ZOOM_IN_POINT);
}

bool SmoothMapManipulator::currentlyZoomingInPoint() const
{
   return m_active[ZOOM_IN_POINT] != 0;
}


bool SmoothMapManipulator::attemptResetRotation(){
   if(!m_mapManipulator.okayToStart(MapManipulator::SMOOTH)) {
      return false;
   } else {
      m_mapManipulator.setIsStarting(MapManipulator::DRAGGING);
   }

   m_operationInterface->setAngle(0);
   m_drawingInterface->requestRepaint();

   return true;   
}

SmoothMapManipulator::~SmoothMapManipulator()
{
   delete m_timerListener;
   delete m_timer;
}

void
SmoothMapManipulator::setMapOperationInterface(
   WFAPI::MapOperationInterface* operationInterface)
{
   m_operationInterface = operationInterface; 
}
