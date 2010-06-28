/*
Copyright (c) 1999 - 2010, Vodafone Group Services Ltd
All rights reserved.

Redistribution and use in source and binary forms, with or without modification, are permitted provided that the following conditions are met:

    * Redistributions of source code must retain the above copyright notice, this list of conditions and the following disclaimer.
    * Redistributions in binary form must reproduce the above copyright notice, this list of conditions and the following disclaimer in the documentation and/or other materials provided with the distribution.
    * Neither the name of the Vodafone Group Services Ltd nor the names of its contributors may be used to endorse or promote products derived from this software without specific prior written permission.

THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS" AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE ARE DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT HOLDER OR CONTRIBUTORS BE LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
*/


//#define CORE_LOGGING_MODULE_LEVEL CL_ALL_FILE
#define CORE_LOGGING_MODULE_NAME "AnimatedMapManipulator"

#include "config.h"
#include "AnimatedMapManipulator.h"
#include "TileMapToolkit.h"
#include "TileMapUtil.h"
#include "MapOperationInterface.h"
#include "MapDrawingInterface.h"
#include "MapManipulator.h"

#include "CoreDebugPrint.h"

#define TIMER_PERIOD 16

using namespace WFAPI;

AnimatedMapManipulator::AnimatedMapManipulator(MapManipulator& parent,
                                               MapDrawingInterface* drawer,
                                               TileMapToolkit* toolkit) :
   m_parent(parent),
   m_operationInterface(NULL),
   m_drawingInterface(drawer),
   m_curState(INACTIVE)
{
   m_timerListener  =
      new TileMapTimerListenerTemplate<AnimatedMapManipulator>(this);
   
   m_timer =      
      toolkit->createPeriodicTimer( m_timerListener,
                                    TileMapToolkit::PRIO_STANDARD);

   m_timerStarted = false;
}


AnimatedMapManipulator::~AnimatedMapManipulator()
{
   delete m_timerListener;
}

void AnimatedMapManipulator::stopWorking()
{
   
}

bool AnimatedMapManipulator::isWorking() const
{
   return m_curState != INACTIVE; 
}

void AnimatedMapManipulator::updatePositionState()
{
   MC2Coordinate curPos;
   float curTime = (TileMapUtil::currentTimeMillis() / 1000.0f
                    - m_posState.startTime) / m_posState.period;
   
   if (curTime >= 1.0f){
      // We set the last position and terminate the animation.
      curPos = m_posState.target;
      m_curState = INACTIVE;

      m_operationInterface->setCenter(curPos);
      m_drawingInterface->requestRepaint();

      if(m_posState.listener) {
         m_posState.listener->animationFinished();
         m_posState.listener = NULL;         
      }

      // We need to stop the timer since we are done moving the map
      m_timer->stop();
      m_timerStarted = false;
   } else {
      // Update to an interpolated position.
      float curValue = m_posState.interpolator.getValue(curTime);
      curPos.lat =
         static_cast<wf_int32>(m_posState.source.lat +
                               m_posState.targetVec.x * curValue);
      curPos.lon =
         static_cast<wf_int32>(m_posState.source.lon +
                               m_posState.targetVec.y * curValue);

      m_operationInterface->setCenter(curPos);
      m_drawingInterface->requestRepaint();
   }


}

bool
AnimatedMapManipulator::attemptAnimatePosition(const MC2Coordinate& targetPosition,
                                               PositionAnimationListener* listener)
{
   if (!m_parent.okayToStart(MapManipulator::ANIMATED)){
      return false;
   }

   WFAPI::ScreenPoint centerPoint;
   WFAPI::ScreenPoint targetPoint;

   m_operationInterface->worldToScreen(centerPoint,
                                       m_operationInterface->getCenter());

   m_operationInterface->worldToScreen(targetPoint, targetPosition);

   int diffX = centerPoint.getX() - targetPoint.getX();
   int diffY = centerPoint.getY() - targetPoint.getY();
   
   unsigned int distSquared = diffX * diffX + diffY * diffY;
   
   static const unsigned int maxDistSquared = 5 * 5;
   
   if(distSquared < maxDistSquared) {   
      m_operationInterface->setCenter(targetPosition);
      if(listener) {
         listener->animationFinished();
      }
      return true;
   }
   
   if(!m_timerStarted) {
      m_timer->start(TIMER_PERIOD);
      m_timerStarted = true;
   }
   
   m_posState.listener = listener;
   m_posState.source = m_operationInterface->getCenter();
   m_posState.target = targetPosition;
   m_posState.period = 0.5f;
   m_posState.startTime = TileMapUtil::currentTimeMillis() / 1000.0f;
   m_posState.targetVec = vec3f(targetPosition.lat - m_posState.source.lat,
                                     targetPosition.lon - m_posState.source.lon);
   m_posState.interpolator = IntervalInterpolator(0.0f, 0.55f);
   
   m_curState = ANIMATING_POSITION;
   return true;
}

void AnimatedMapManipulator::timerExpired(uint32 id)
{
   coreprintln("%s, %d", __FUNCTION__, __LINE__);
   switch(m_curState) {
   case ANIMATING_POSITION:
      coreprintln("%s, %d, m_curState = ANIMATING_POSITION ", __FUNCTION__, __LINE__);
      updatePositionState();
      break;
   case INACTIVE:
      coreprintln("%s, %d, m_curState = INACTIVE ", __FUNCTION__, __LINE__);
      break;
   default:
      break;
   }
}

void
AnimatedMapManipulator::setMapOperationInterface(
   MapOperationInterface* operationInterface)
{
   m_operationInterface = operationInterface;
}


