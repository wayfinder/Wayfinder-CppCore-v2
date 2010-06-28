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
#include "DraggingMapManipulator.h"
#include "MapOperationInterface.h"
#include "MapManipulator.h"
#include "MapDrawingInterface.h"

//#include <valarray>
#include <cstdlib>

DraggingMapManipulator::DraggingMapManipulator(
   MapManipulator& mapManipulator,
   WFAPI::MapDrawingInterface* drawingInterface) :
   m_mapManipulator(mapManipulator),
   m_operationInterface(NULL),
   m_drawingInterface(drawingInterface),
   m_state(STATIC)
{
   DragState s = { MC2Coordinate(0, 0), MC2Point(0, 0) };
   m_dragState = s;    
}

bool DraggingMapManipulator::attemptDrag(const WFAPI::ScreenPoint& point)
{
   if(!m_mapManipulator.okayToStart(MapManipulator::DRAGGING)) {
      return false;
   } else {
      m_mapManipulator.setIsStarting(MapManipulator::DRAGGING);      
   }
   
   m_dragState.initialPoint = point;
   WFAPI::WGS84Coordinate tmpCoord;
   m_operationInterface->screenToWorld(tmpCoord, point);
   m_dragState.dragCoord = tmpCoord;
   setState(DRAGGING);

   return true;
}

void DraggingMapManipulator::endDrag(const WFAPI::ScreenPoint& point)
{   
   setState(STATIC);

   // if( abs(point.getX() - m_dragState.initialPoint.getX()) > 3  ||
   //     abs(point.getY() - m_dragState.initialPoint.getY()) > 3 )
   // {
      m_operationInterface->setPoint(m_dragState.dragCoord, point);
      m_drawingInterface->requestRepaint();
   // }
}

void DraggingMapManipulator::updateDrag(const WFAPI::ScreenPoint& point)
{   
   if( m_state == STATIC ) {
      return;
   }

   // if( abs(point.getX() - m_dragState.initialPoint.getX()) > 3  ||
   //     abs(point.getY() - m_dragState.initialPoint.getY()) > 3 )
   // {
      m_operationInterface->setPoint(m_dragState.dragCoord, point);
      m_drawingInterface->requestRepaint();
   // }
}

void DraggingMapManipulator::setState(State newState)
{
   if(newState == DRAGGING) {
      m_operationInterface->setMovementMode(true);
   } else {
      m_operationInterface->setMovementMode(false);
   }
   m_state = newState; 
}

void DraggingMapManipulator::stopWorking()
{
   m_operationInterface->setMovementMode(false);
   m_state = STATIC;
}

bool DraggingMapManipulator::isWorking()
{
   return m_state != STATIC; 
}

void
DraggingMapManipulator::setMapOperationInterface(
   WFAPI::MapOperationInterface* operationInterface)
{
   m_operationInterface = operationInterface;
}
