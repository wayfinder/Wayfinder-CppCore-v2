/*
Copyright (c) 1999 - 2010, Vodafone Group Services Ltd
All rights reserved.

Redistribution and use in source and binary forms, with or without modification, are permitted provided that the following conditions are met:

    * Redistributions of source code must retain the above copyright notice, this list of conditions and the following disclaimer.
    * Redistributions in binary form must reproduce the above copyright notice, this list of conditions and the following disclaimer in the documentation and/or other materials provided with the distribution.
    * Neither the name of the Vodafone Group Services Ltd nor the names of its contributors may be used to endorse or promote products derived from this software without specific prior written permission.

THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS" AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE ARE DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT HOLDER OR CONTRIBUTORS BE LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
*/


#define CORE_LOGGING_MODULE_NAME "MapLibDownloadTest"
// #define CORE_LOGGING_MODULE_LEVEL CL_ALL_FILE

#include "CoreDebugPrint.h"
#include "Nav2Session.h"
#include "Environment.h"
#include "MapDownloadTest.h"
#include "InternalConfig.h"
#include "MapLibAPI.h"
#include <cstdlib>
#include "WGS84Coordinate.h"
#include "MapOperationInterface.h"
#include "TileMapUtil.h"
#include "Thread.h"

using namespace WFAPI;

MapDownloadTest::MapDownloadTest(Nav2Session& session,
                                 const char* name)
   : RegressionTest(name, 30000),
     m_session(session),
     m_useRandomPos(false)
{
    
}

void MapDownloadTest::startTest()
{
   MapLibAPI* mapLib = m_session.getMapLib();

   if (mapLib == NULL){
      setTestFailed("No MapLib available.");
      return;
   }
   
   if(m_useRandomPos) {      
      double normRand1 = (rand() / (double)RAND_MAX) - 0.5f;
      double normRand2 = (rand() / (double)RAND_MAX) - 0.5f;
      
      WGS84Coordinate randCoord(normRand1 * 360.0f, normRand2 * 360.0f);
      mapLib->getMapOperationInterface()->setCenter(randCoord);
   }

   // int timeSlice = getTimeOutMs() / 10;
   // timeSlice = std::max(timeSlice, 500);
   // int totalTime = 0;

   InternalConfig* internalConf = mapLib->getInternalConfig();

   unsigned int startTimeMs = TileMapUtil::currentTimeMillis();
   
   while(TileMapUtil::currentTimeMillis() - startTimeMs < getTimeOutMs()) {

      isab::Thread::sleep(50, 0);
      Environment::sleep(0);
      isab::Thread::sleep(50, 0);
      Environment::sleep(0);
      isab::Thread::sleep(50, 0);
      Environment::sleep(0);
      isab::Thread::sleep(50, 0);
      Environment::sleep(0);
      isab::Thread::sleep(50, 0);
      Environment::sleep(0);
      isab::Thread::sleep(50, 0);
      Environment::sleep(0);
      
      if(internalConf->isMapFullyLoaded()) {
         setTestPassed();
         return;
      }
   }
   
   setTestFailed("The map failed to download during the specified timeout.");
}

void MapDownloadTest::setUseRandomPos(bool useRandomPos)
{
   m_useRandomPos = useRandomPos; 
}

void MapDownloadTest::setArgImpl(const std::string& str)
{
   if(str.compare("randomPos") == 0) {
      setUseRandomPos(true); 
   }
}

