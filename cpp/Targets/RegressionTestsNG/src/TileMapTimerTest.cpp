/*
Copyright (c) 1999 - 2010, Vodafone Group Services Ltd
All rights reserved.

Redistribution and use in source and binary forms, with or without modification, are permitted provided that the following conditions are met:

    * Redistributions of source code must retain the above copyright notice, this list of conditions and the following disclaimer.
    * Redistributions in binary form must reproduce the above copyright notice, this list of conditions and the following disclaimer in the documentation and/or other materials provided with the distribution.
    * Neither the name of the Vodafone Group Services Ltd nor the names of its contributors may be used to endorse or promote products derived from this software without specific prior written permission.

THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS" AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE ARE DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT HOLDER OR CONTRIBUTORS BE LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
*/


#define CORE_LOGGING_MODULE_NAME "TILEMAPTIMERTEST"
//#define CORE_LOGGING_MODULE_LEVEL CL_ALL_FILE

#include "CoreDebugPrint.h"
#include "TileMapTimerTest.h"
#include "MapLibAPI.h"
#include "Nav2Session.h"
#include "MapLib.h"

using namespace WFAPI;

TileMapTimerTest::TileMapTimerTest(Nav2Session& session, const char* name)
   : RegressionTest(name),
     m_session(session),
     m_firstTimerId(0),
     m_firstTimerFired(false),
     m_secondTimerId(0),
     m_secondTimerFired(false),
     m_thirdTimerId(0),
     m_thirdTimerFired(false),
     m_idleTimerId(0)
{
    
}

void TileMapTimerTest::startTest()
{
   MapLibAPI* mapLibAPI = m_session.getMapLib();
   if (mapLibAPI == NULL){
      setTestFailed("No MapLibAPI available.");
      return;
   }
   
   MapLib* mapLib = mapLibAPI->getMapLib();
   if (mapLib == NULL){
      setTestFailed("No MapLib available.");
      return;
   }
   
   TileMapToolkit* toolkit = mapLib->getToolkit();
   
   // The order of requestTimer calls is intentionally in "wrong" order
   // considering the member names. A crash has previously been observed
   // with this setup.
   m_thirdTimerId = toolkit->requestTimer(this, 1000);
   m_firstTimerId = toolkit->requestTimer(this, 100);
   m_secondTimerId = toolkit->requestTimer(this, 500);
   coreprintln("m_firstTimerId=%d, m_secondTimerId=%d, m_thirdTimerId=%d",
               m_firstTimerId, m_secondTimerId, m_thirdTimerId);

}

void TileMapTimerTest::timerExpired(uint32 id)
{
   MapLibAPI* mapLibAPI = m_session.getMapLib();
   MapLib* mapLib = mapLibAPI->getMapLib();
   TileMapToolkit* toolkit = mapLib->getToolkit();

   coreprintln("timerExpired with id=%d", id);
   
   if (id == m_firstTimerId){
      m_firstTimerFired = true;
      // We wait for the second timer to expire
   } else if (id == m_secondTimerId) {
      m_secondTimerFired = true;
      cancelRemainingTimers();
      m_idleTimerId = toolkit->requestIdle(this);
      coreprintln("first and second timer expired, canceling remaining timers and requesting an idle timer, id=%d",
                  m_idleTimerId);
   } else if (id == m_thirdTimerId) {
      m_thirdTimerFired = true;
      cancelRemainingTimers();
      setTestFailed("Timers fired in wrong order!");
   }
}

void TileMapTimerTest::cancelRemainingTimers()
{
   MapLibAPI* mapLibAPI = m_session.getMapLib();
   MapLib* mapLib = mapLibAPI->getMapLib();
   TileMapToolkit* toolkit = mapLib->getToolkit();

   if (!m_firstTimerFired){
      toolkit->cancelTimer(this, m_firstTimerId);
   }
   if (!m_secondTimerFired){
      toolkit->cancelTimer(this, m_secondTimerId);
   }
   if (!m_thirdTimerFired){
      toolkit->cancelTimer(this, m_thirdTimerId);
   }
}

void TileMapTimerTest::runIdleTask(uint32 id)
{
   coreprintln("in runIdleTask, id=%d", id);
   if (id == m_idleTimerId){
      setTestPassed();       
   } else {
      setTestFailed("Idle timer expired with wrong id!");
   }

}
