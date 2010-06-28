/*
Copyright (c) 1999 - 2010, Vodafone Group Services Ltd
All rights reserved.

Redistribution and use in source and binary forms, with or without modification, are permitted provided that the following conditions are met:

    * Redistributions of source code must retain the above copyright notice, this list of conditions and the following disclaimer.
    * Redistributions in binary form must reproduce the above copyright notice, this list of conditions and the following disclaimer in the documentation and/or other materials provided with the distribution.
    * Neither the name of the Vodafone Group Services Ltd nor the names of its contributors may be used to endorse or promote products derived from this software without specific prior written permission.

THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS" AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE ARE DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT HOLDER OR CONTRIBUTORS BE LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
*/


#ifndef _STRESSTESTER_H_
#define _STRESSTESTER_H_

class SmoothMapManipulator;
class TileMapToolkit;
class TileMapPeriodicTimer;

template <class T> class TileMapTimerListenerTemplate;

namespace WFAPI {
class MapOperationInterface;
}

/**
 *  This class will perform random movement in order to
 *  test the stability of the system.
 *
 */ 
class StressTester {
public:
   StressTester(TileMapToolkit* toolkit,
                WFAPI::MapOperationInterface* operationInterface);

   ~StressTester();

   void setMapOperator(SmoothMapManipulator* mapOperator);
   
   void start();
   void stop();

   void updateIteration();

   void timerExpired(uint32 id);   
private:
   int getRandomCoord();

   enum StressState {
      LOADING, ROTATING, ZOOMING
   };

   
   /// Periodic timer for random position updates
   TileMapPeriodicTimer* m_timer;
   
   /// Listener
   TileMapTimerListenerTemplate<StressTester>* m_timerListener;
   
   uint32 rand();

   void beginNewOperation();
   void endCurrentOperation();

   TileMapToolkit* toolkit;

   SmoothMapManipulator* m_mapOperator;
   WFAPI::MapOperationInterface* m_operationInterface;
   
   uint32 m_curSeed;
   uint32 m_remainingTicks;
   bool m_isActive;
   uint32 m_curOperation;

   unsigned int m_curIndex;

   StressState m_curPhase;
};

#endif /* _STRESSTESTER_H_ */
