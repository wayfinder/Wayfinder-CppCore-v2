/*
Copyright (c) 1999 - 2010, Vodafone Group Services Ltd
All rights reserved.

Redistribution and use in source and binary forms, with or without modification, are permitted provided that the following conditions are met:

    * Redistributions of source code must retain the above copyright notice, this list of conditions and the following disclaimer.
    * Redistributions in binary form must reproduce the above copyright notice, this list of conditions and the following disclaimer in the documentation and/or other materials provided with the distribution.
    * Neither the name of the Vodafone Group Services Ltd nor the names of its contributors may be used to endorse or promote products derived from this software without specific prior written permission.

THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS" AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE ARE DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT HOLDER OR CONTRIBUTORS BE LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
*/


#define CORE_LOGGING_MODULE_NAME "RegTestUtil"
#define CORE_LOGGING_MODULE_LEVEL CL_ALL_FILE

#include "CoreDebugPrint.h"

#include "Environment.h"
#include "Nav2Session.h"
#include "Nav2API.h"
#include "RegressionTestEnvironment.h"
#include "ReadyToRunListenerTest.h"
#include "NetworkConnectionTest.h"
#include "ImageDownloadTest.h"
#include "RouteSimulationTest.h"
#include "PositionSimulationTest.h"
#ifdef __SYMBIAN__
# include <IapSearcherTest.h>
#endif
#include "Nav2InitTest.h"
#include "MapLibInitTest.h"
#include "MapLibDeleteTest.h"
#include "Nav2ShutdownTest.h"
#include "Nav2DeleteTest.h"
#include <vector>
#include "SearchRegTest.h"
#include "RegressionTestStarter.h"
#include "SearchOverflowTest.h"
#include "SettingsTest.h"
#include "SearchImagesTest.h"
#include "CategoryTreeTest.h"
#include "OneSearchTest.h"
#include "MapDownloadTest.h"
#include "UnitTest.h"
#include "SleepTest.h"
#include "DebugTest.h"
#include "TileMapTimerTest.h"
#include "FavouriteRegTest.h"
#include "TunnelInterfaceRegTest.h"
#ifndef __SYMBIAN__ //symbian cannot run the pal threading test.
#include "PALThreadingTest.h" 
#endif
#include "PALNetworkRegTest.h"
#include "InvalidInputSearchTest.h"
#include "MapLibStandaloneTest.h"

#ifndef NAV2_CLIENT_I386
#include "LocationHardwareTest.h"
#include "LocationPositionTest.h"
#include "CellIDTest.h"
#endif
#ifdef IPHONE
# include <ProxyNetworkTest.h>
#endif
#include "RegressionTestUtil.h"
#include "ConnectionManagerTest.h"

namespace RegressionTestUtil {


bool doTestAndDelete(RegressionTest* test, int testNum, unsigned int numTests)
{
   const char* name = test->getTestName();
   
   coreprintln_info("Starting test %d/%d: %s(%s)",
                    testNum, numTests, name,
                    test->getArg().c_str());

   test->startTest();

   int ok = 0;
   bool timedOut = false;

   unsigned int allowedTimeOutMs = test->getTimeOutMs();
   
   // Some tests are not time bound, for example unit tests. These
   // will complete within the startTest() call. Even if they were
   // to lock the system, they could not be aborted anyhow.
   
   if(test->isTestFinished()) {
      //ok = test->testPassed();
      ok = 0;
   } else {
      timedOut =
         !Environment::conditionalWait(test->isTestFinished(), allowedTimeOutMs);
      //ok = !timedOut && test->testPassed();
   }
   
   if(timedOut) {
      coreprintln_info(" * TIMED-OUT. ");
      ok = 1;
   } else {
      if(test->testPassed()) {
         coreprintln_info(" * PASSED.");
      } else {                          
         coreprintln_info(" * FAILED: %s", test->getFailedReason().c_str());
         ok = 2;
      }
   }

   delete test;
   test = NULL;

   return ok;   
}

template<typename DerivedClass>
RegressionTest* spawnTest(Nav2Session& session, const char* name) {
   return new DerivedClass(session, name);
}

RegTestMap getTests(Nav2Session& session)
{   
   RegTestMap tests;
   
#ifdef __SYMBIAN__
   tests["NetworkSetupTest"]       = spawnTest<IapSearcherTest>;
#elif defined IPHONE
   tests["NetworkSetupTest"]       = spawnTest<ProxyNetworkTest>;
#else
   tests["NetworkSetupTest"]       = spawnTest<NetworkConnectionTest>;
#endif

   tests["DebugTest"]              = spawnTest<DebugTest>;
   tests["UnitTest"]               = spawnTest<UnitTest>;
   tests["SleepTest"]              = spawnTest<SleepTest>;
   tests["Nav2InitTest"]           = spawnTest<Nav2InitTest>;
   tests["Nav2ShutdownTest"]       = spawnTest<Nav2ShutdownTest>;
   tests["Nav2DeleteTest"]         = spawnTest<Nav2DeleteTest>;
   tests["ConnectionManagerTest"]  = spawnTest<ConnectionManagerTest>;
   tests["SettingsTest"]           = spawnTest<SettingsTest>;
   tests["InvalidInputSearchTest"] = spawnTest<InvalidInputSearchTest>;
   tests["SearchRegTest"]          = spawnTest<SearchRegTest>;
   tests["SearchImagesTest"]       = spawnTest<SearchImagesTest>;
   tests["ImageDownloadTest"]      = spawnTest<ImageDownloadTest>;
   tests["RouteSimulationTest"]    = spawnTest<RouteSimulationTest>;
   tests["PositionSimulationTest"] = spawnTest<PositionSimulationTest>;
   tests["SearchOverflowTest"]     = spawnTest<SearchOverflowTest>;
   tests["ReadyToRunListenerTest"] = spawnTest<ReadyToRunListenerTest>;
   tests["FavouriteRegTest"]       = spawnTest<FavouriteRegTest>;
   tests["TunnelInterfaceRegTest"] = spawnTest<TunnelInterfaceRegTest>;
#ifndef NAV2_CLIENT_I386
   tests["CellIDTest"]             = spawnTest<CellIDTest>;
   tests["LocationHardwareTest"]   = spawnTest<LocationHardwareTest>;
   tests["LocationPositionTest"]   = spawnTest<LocationPositionTest>;
#endif
#ifndef __SYMBIAN__   
   tests["PALThreadingTest"]       = spawnTest<PALThreadingTest>;
   tests["PALNetworkRegTest"]       = spawnTest<PALNetworkRegTest>;
#endif
   tests["MapLibInitTest"]         = spawnTest<MapLibInitTest>;
   tests["MapLibDeleteTest"]       = spawnTest<MapLibDeleteTest>;
   tests["TileMapTimerTest"]       = spawnTest<TileMapTimerTest>;
   tests["MapDownloadTest"]        = spawnTest<MapDownloadTest>;
   tests["CategoryTreeRegTest"]    = spawnTest<CategoryTreeTest>;
   tests["OneSearchTest"]          = spawnTest<OneSearchTest>;

   
   return tests;
}

using namespace std;

RegTestVec setupTestVector(RegTestMap& availableTests,
                           Nav2Session& session,
                           const RegTestSuite& testsToRun)
{
   RegTestVec res;
   
   for(RegTestSuite::const_iterator itr = testsToRun.begin();
       itr != testsToRun.end();
       itr++)
   {
      const std::string& curTest = *itr;

      std::size_t delim = curTest.find_first_of(' ');

      std::string name;
      std::string arg;

      if(delim == string::npos) {
         name = curTest;
      } else {
         arg = curTest.substr(delim+1);
         name = curTest.substr(0, delim);
      }
      
      RegTestMap::iterator regItr = availableTests.find(name);

      if(regItr != availableTests.end()) {
         RegressionTest* reg =
            regItr->second(session, regItr->first.c_str());

         reg->setArg(arg);
         res.push_back(reg);
      } else {
         coreprintln_warn("WARNING: Test not found: %s", name.c_str());
      }
   }
   
   return res;
}

} // End of namespace RegressionTestUtil

