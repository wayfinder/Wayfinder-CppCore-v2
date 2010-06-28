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
#include "StressTester.h"
#include "SmoothMapManipulator.h"
#include "MapLibKeyInterface.h"
#include "TileMapToolkit.h"
#include "MapOperationInterface.h"
#include "MC2Coordinate.h"


using namespace WFAPI;

WGS84Coordinate cityCenters[] =
{
   WGS84Coordinate(55.6057,12.9991), 
   WGS84Coordinate(56.6694,16.3468), 
   WGS84Coordinate(59.3327,18.066), 
   WGS84Coordinate(55.6828,12.5652), 
   WGS84Coordinate(56.1578,10.2193), 
   WGS84Coordinate(53.556,9.98905), 
   WGS84Coordinate(53.0534,8.79369), 
   WGS84Coordinate(52.5232,13.4096), 
   WGS84Coordinate(50.8417,4.34471), 
   WGS84Coordinate(48.8544,2.35432), 
   WGS84Coordinate(51.5062,-0.131634), 
   WGS84Coordinate(52.4868,-1.88902), 
   WGS84Coordinate(53.384,-1.46339), 
   WGS84Coordinate(55.9521,-3.1887), 
   WGS84Coordinate(45.4378,-75.6923), 
   WGS84Coordinate(38.8769,-77.0083), 
   WGS84Coordinate(39.9555,-75.1671), 
   WGS84Coordinate(40.7655,-73.9817), 
   WGS84Coordinate(42.3637,-71.0622), 
   WGS84Coordinate(42.3259,-83.0296), 
   WGS84Coordinate(41.8846,-87.6445), 
   WGS84Coordinate(39.1131,-94.586), 
   WGS84Coordinate(34.0826,-118.234), 
   WGS84Coordinate(37.7825,-122.201), 
   WGS84Coordinate(19.4448,-99.079), 
   WGS84Coordinate(18.0102,-76.7901)
};

#define MIN_TICK_AMOUNT 100
#define MAX_TICK_AMOUNT 1000

//
// returns random integer from 1 to lim
//

#define MODULUS    2147483647 /* DON'T CHANGE THIS VALUE                  */
#define MULTIPLIER 48271      /* DON'T CHANGE THIS VALUE                  */

uint32 StressTester::rand()
{
   const unsigned long Q = MODULUS / MULTIPLIER;
   const unsigned long R = MODULUS % MULTIPLIER;
   unsigned long t;

   t = MULTIPLIER * (m_curSeed % Q) - R * (m_curSeed / Q);
   if (t > 0) 
      m_curSeed = t;
   else 
      m_curSeed = t + MODULUS;

   return m_curSeed;
}

StressTester::StressTester(TileMapToolkit* toolkit,
                           WFAPI::MapOperationInterface* operationInterface):
   m_mapOperator(NULL),
   m_operationInterface(operationInterface),
   m_curSeed(123456789),
   m_remainingTicks(0),
   m_isActive(false),
   m_curOperation(0),
   m_curIndex(0),
   m_curPhase(LOADING)
{
   m_timerListener  =
      new TileMapTimerListenerTemplate<StressTester>(this);

   m_timer =      
      toolkit->createPeriodicTimer( m_timerListener,
                                    TileMapToolkit::PRIO_STANDARD);
}

StressTester::~StressTester()
{
   delete m_timerListener;
   delete m_timer;
}


void StressTester::start()
{
#if 0
   if (m_mapOperator != NULL && !m_isActive){
      m_isActive = true;
      beginNewOperation();
   }
#endif
   
   m_timer->start(10000);
}

void StressTester::stop()
{
#if 0
   if (m_isActive){
      m_isActive = false;
      endCurrentOperation();
   }
#endif
}

void StressTester::setMapOperator(SmoothMapManipulator* mapOperator)
{
   m_mapOperator = mapOperator;
}

void StressTester::updateIteration()
{
#if 0
   if (m_isActive){
      m_remainingTicks--;
      if (m_remainingTicks == 0) {
         stop();
         start();
      }
   }
#endif
}

void StressTester::beginNewOperation()
{
#if 0
   
   // MOVE_DOWN_LEFT_KEY is last in the enum
   m_curOperation = MapLibKeyInterface::SELECT_KEY;
   
   while (m_curOperation == MapLibKeyInterface::SELECT_KEY ||
          m_curOperation == MapLibKeyInterface::RESET_ROTATION_KEY) {
      m_curOperation = rand(1 + MapLibKeyInterface::MOVE_DOWN_LEFT_KEY) - 1;
   }
   m_remainingTicks =
      MIN_TICK_AMOUNT + rand(MAX_TICK_AMOUNT - MIN_TICK_AMOUNT);
   
   switch(m_curOperation) {
   case MapLibKeyInterface::MOVE_UP_KEY:
      m_mapOperator->beginMoveUp();
      break;
   case MapLibKeyInterface::MOVE_DOWN_KEY:
      m_mapOperator->beginMoveDown();
      break;
   case MapLibKeyInterface::MOVE_LEFT_KEY:
      m_mapOperator->beginMoveLeft();
      break;
   case MapLibKeyInterface::MOVE_RIGHT_KEY:
      m_mapOperator->beginMoveRight();
      break;
   case MapLibKeyInterface::ZOOM_IN_KEY:
      m_mapOperator->beginZoomIn();
      break;
   case MapLibKeyInterface::ZOOM_OUT_KEY:
      m_mapOperator->beginZoomOut();
      break;
   case MapLibKeyInterface::ROTATE_LEFT_KEY:
      m_mapOperator->beginRotateLeft();
      break;
   case MapLibKeyInterface::ROTATE_RIGHT_KEY:
      m_mapOperator->beginRotateRight();
      break;
   case MapLibKeyInterface::MOVE_UP_RIGHT_KEY:
      m_mapOperator->beginMoveUpAndRight();
      break;
   case MapLibKeyInterface::MOVE_DOWN_RIGHT_KEY:
      m_mapOperator->beginMoveDownAndRight();
      break;
   case MapLibKeyInterface::MOVE_UP_LEFT_KEY:
      m_mapOperator->beginMoveUpAndLeft();
      break;
   case MapLibKeyInterface::MOVE_DOWN_LEFT_KEY:
      m_mapOperator->beginMoveDownAndLeft();
      break;
   }

#endif
}

void StressTester::endCurrentOperation()
{
#if 0
   switch(m_curOperation) {
   case MapLibKeyInterface::MOVE_UP_KEY:
      m_mapOperator->endMoveUp();
      break;
   case MapLibKeyInterface::MOVE_DOWN_KEY:
      m_mapOperator->endMoveDown();
      break;
   case MapLibKeyInterface::MOVE_LEFT_KEY:
      m_mapOperator->endMoveLeft();
      break;
   case MapLibKeyInterface::MOVE_RIGHT_KEY:
      m_mapOperator->endMoveRight();
      break;
   case MapLibKeyInterface::ZOOM_IN_KEY:
      m_mapOperator->endZoomIn();
      break;
   case MapLibKeyInterface::ZOOM_OUT_KEY:
      m_mapOperator->endZoomOut();
      break;
   case MapLibKeyInterface::ROTATE_LEFT_KEY:
      m_mapOperator->endRotateLeft();
      break;
   case MapLibKeyInterface::ROTATE_RIGHT_KEY:
      m_mapOperator->endRotateRight();
      break;
   case MapLibKeyInterface::MOVE_UP_RIGHT_KEY:
      m_mapOperator->endMoveUpAndRight();
      break;
   case MapLibKeyInterface::MOVE_DOWN_RIGHT_KEY:
      m_mapOperator->endMoveDownAndRight();
      break;
   case MapLibKeyInterface::MOVE_UP_LEFT_KEY:
      m_mapOperator->endMoveUpAndLeft();
      break;
   case MapLibKeyInterface::MOVE_DOWN_LEFT_KEY:
      m_mapOperator->endMoveDownAndLeft();
      break;
   }
#endif
}

bool boundCheck1(int lon) {
   MC2Coordinate leftPoint(WGS84Coordinate(19.2565,-15.9642));
   MC2Coordinate rightPoint(WGS84Coordinate(31.9278,67.4978));
   return lon > leftPoint.lon && lon < rightPoint.lon;
}

void StressTester::timerExpired(uint32 id)
{
   if(!m_mapOperator) {
      return;
   }
   
   static const int numCities =
      sizeof(cityCenters) / sizeof(WGS84Coordinate);

   if(m_curPhase == LOADING) {
      m_mapOperator->endZoomOut();
      
      WGS84Coordinate curCity = cityCenters[m_curIndex];
      m_operationInterface->setCenter(curCity);
      m_operationInterface->setZoomLevel(5.80f);
   
      m_curIndex = (m_curIndex + 1) % numCities;

      m_curPhase = ROTATING;
   } else if(m_curPhase == ROTATING) {
      m_mapOperator->attemptRotateLeft();
      m_curPhase = ZOOMING;
   }   else if(m_curPhase == ZOOMING) {
      m_mapOperator->endRotateLeft();
      m_mapOperator->attemptZoomOut();
      
      m_curPhase = LOADING;
   }

   
   // int lat = getRandomCoord();
   // int lon = getRandomCoord();

   // while(!boundCheck1(lon)) {
   //    lon = getRandomCoord();
   // }

   // float randScale = (rand() / (float)MODULUS) * 24000.0f;
   
   // MC2Coordinate c(lat, lon);
   

}

int StressTester::getRandomCoord()
{
   return (int)(rand() * 2.0f - (float)MAX_INT32);
}


