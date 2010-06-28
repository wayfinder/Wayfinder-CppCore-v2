/*
Copyright (c) 1999 - 2010, Vodafone Group Services Ltd
All rights reserved.

Redistribution and use in source and binary forms, with or without modification, are permitted provided that the following conditions are met:

    * Redistributions of source code must retain the above copyright notice, this list of conditions and the following disclaimer.
    * Redistributions in binary form must reproduce the above copyright notice, this list of conditions and the following disclaimer in the documentation and/or other materials provided with the distribution.
    * Neither the name of the Vodafone Group Services Ltd nor the names of its contributors may be used to endorse or promote products derived from this software without specific prior written permission.

THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS" AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE ARE DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT HOLDER OR CONTRIBUTORS BE LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
*/


#include "config.h"
#include "MapManipulator.h"
#include "DraggingMapManipulator.h"
#include "SmoothMapManipulator.h"
#include "AnimatedMapManipulator.h"

MapManipulator::MapManipulator(WFAPI::MapDrawingInterface* drawingInterface,
                               TileMapToolkit* platform)
{
   m_dragging = new DraggingMapManipulator(*this,
                                           drawingInterface);
         
   m_smooth = new SmoothMapManipulator(*this,
                                       drawingInterface,
                                       platform);

   m_animated = new AnimatedMapManipulator(*this,
                                           drawingInterface,
                                           platform);
}

AnimatedMapManipulator& MapManipulator::getAnimatedManipulator()
{
   return *m_animated;
}


DraggingMapManipulator& MapManipulator::getDraggingMapManipulator()
{
   return *m_dragging;
}

SmoothMapManipulator& MapManipulator::getSmoothMapManipulator()
{
   return *m_smooth;
}

MapManipulator::~MapManipulator()
{
   delete m_dragging;
   delete m_smooth;
   delete m_animated;
}

bool MapManipulator::okayToStop()
{
   return !m_animated->isWorking(); 
}

bool MapManipulator::okayToStart(ManipulatorTag tag)
{

   switch(tag)  {
   case ANIMATED:
      // It is always ok to start an animation unless an animation
      // is already in progress.
      return !m_animated->isWorking();
      break;
   case SMOOTH:
      // OK to start only if there is no animation in progress
      return !m_animated->isWorking();
      break;
   case STATIC:
      // OK to start only if there is no animation in progress
      return !m_animated->isWorking();
      break;
   case DRAGGING:
      // OK to start only if there is no animation in progress
      return !m_animated->isWorking();
      break;
   case EXTERNAL:
      return false;
      break;
   }

   return false;
}

void MapManipulator::stopAllManipulators()
{
   m_dragging->stopWorking();
   m_animated->stopWorking();
   m_smooth->stopWorking();
}

void MapManipulator::setIsStarting(ManipulatorTag tag)
{
   stopAllManipulators();    
}

void
MapManipulator::setMapOperationInterface(
   WFAPI::MapOperationInterface* operationInterface)
{
   m_dragging->setMapOperationInterface(operationInterface);
   m_smooth->setMapOperationInterface(operationInterface);
   m_animated->setMapOperationInterface(operationInterface);
}
