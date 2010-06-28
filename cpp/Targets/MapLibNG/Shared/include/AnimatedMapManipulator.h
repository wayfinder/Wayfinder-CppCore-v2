/*
Copyright (c) 1999 - 2010, Vodafone Group Services Ltd
All rights reserved.

Redistribution and use in source and binary forms, with or without modification, are permitted provided that the following conditions are met:

    * Redistributions of source code must retain the above copyright notice, this list of conditions and the following disclaimer.
    * Redistributions in binary form must reproduce the above copyright notice, this list of conditions and the following disclaimer in the documentation and/or other materials provided with the distribution.
    * Neither the name of the Vodafone Group Services Ltd nor the names of its contributors may be used to endorse or promote products derived from this software without specific prior written permission.

THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS" AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE ARE DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT HOLDER OR CONTRIBUTORS BE LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
*/


#ifndef _ANIMATEDMAPMANIPULATOR_H_
#define _ANIMATEDMAPMANIPULATOR_H_

#include "IntervalInterpolator.h"
#include "MC2Coordinate.h"
#include "Vector3d.h"

namespace WFAPI{
class MapOperationInterface;
class MapDrawingInterface;
}
class MapManipulator;
class TileMapToolkit;
class TileMapPeriodicTimer;
template <class T> class TileMapTimerListenerTemplate;

/**
 * The interface that classes can implement if they want to be notified when
 * an animation is completed.
 */
class PositionAnimationListener {
public:
   virtual ~PositionAnimationListener() {}
   
   virtual void animationFinished() = 0;
};


/**
 * This class handles animated map manipulations such as centering
 * the map on a specific position smoothly.
 */
class AnimatedMapManipulator {
public:
   /**
    * Constructor.
    *
    * @param parent The map manipulation state handling parent.
    * @param drawingInterface The drawing interface, used to call repaint
    *                         during animations.
    * @param toolkit The toolkit used to create and handle timers.
    */
   AnimatedMapManipulator(MapManipulator& parent,
                          WFAPI::MapDrawingInterface* drawingInterface,
                          TileMapToolkit* toolkit);

   virtual ~AnimatedMapManipulator();
   
   /**
    * Sets the map operation interface used to move the map.
    * Must be called to completely initialize the class.
    * 
    * @param operationInterface The map operation interface.
    */
   void
   setMapOperationInterface(WFAPI::MapOperationInterface* operationInterface);

   /**
    * Attempts to initiate an animation that smoothly moves the map to
    * center on targetPosition.
    *
    * @param targetPosition The position to move towards.
    * @param listener A handle to a listener that will be called
    *                 when the animation is complete.
    * @return True if the animation is started.
    */
   bool attemptAnimatePosition(const MC2Coordinate& targetPosition,
                               PositionAnimationListener* listener);

   /**
    * TODO: Not yet implemented
    */
   void endAnimatePosition();

   /**
    *
    */ 
   bool attemptAnimate2d3dSwitch(bool to3d);

   /**
    *  Attempt to start flicking.
    */
   bool attemptFlicking(float velocityX, float velocityY);

   /**
    *  Stop flicking.
    */
   void endFlicking();
   
   /**
    * @return True if an the manipulator is working.
    */
   bool isWorking() const;

   /**
    * @return True if an animation is ongoing.
    */
   bool isAnimating() const;
   
   /**
    * TODO: Not yet implemented
    */
   void stopWorking();

   /**
    * Timer related callback function.
    */ 
   void timerExpired(uint32 id);
   
private:

   /**
    * Updates the position according to the current progress of the
    * animation.
    */
   void updatePositionState();

   /**
    * Updates the switch between 2d and 3d.
    */ 
   void update2d3dState();

   /**
    * Update flicking state.
    */
   void updateFlickingState();
   
   void killTimer();
   
   /**
    * Used to keep track of the internal state.
    */
   enum State { ANIMATING_POSITION, ANIMATING_2D_3D, FLICKING, INACTIVE };

   /**
    * Internal data storage needed for positon animation.
    */
   struct PositionData {
      MC2Coordinate source;
      MC2Coordinate target;
      float period;
      float startTime;
      vec3f targetVec;
      IntervalInterpolator interpolator;
      PositionAnimationListener* listener;
   } m_posState;

   struct Switch2dTo3dData {
      bool to3d;
      float curStep;
   } m_switchState;
   
   struct ZoomStepData {
      
   } m_zoomStepData; 

   /**
    * The state of the flicking animation.
    */ 
   struct FlickingState {
      /// Velocity in x direction. Unit pixels per millisecond.
      float curVelX;
      /// Velocity in y direction. Unit pixels per millisecond.
      float curVelY;
      
      unsigned int lastTime;
      float decelerationConstantX;
      float decelerationConstantY;
      
   } m_flickingState;
   
   /// The state handling parent.
   MapManipulator& m_parent;
   /// The map operation interface used to move the map.
   WFAPI::MapOperationInterface* m_operationInterface;
   /// The map drawing interface used to redraw the map.
   WFAPI::MapDrawingInterface* m_drawingInterface;
   /// Periodic timer for logic updates
   TileMapPeriodicTimer* m_timer;
   /// Listener
   TileMapTimerListenerTemplate<AnimatedMapManipulator>* m_timerListener;
   /// The current state
   State m_curState;
   /// State member indicating if the timer has yet been started.
   bool m_timerStarted;
};


#endif /* _ANIMATEDMAPMANIPULATOR_H_ */
