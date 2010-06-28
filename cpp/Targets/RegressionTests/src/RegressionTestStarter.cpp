/*
Copyright (c) 1999 - 2010, Vodafone Group Services Ltd
All rights reserved.

Redistribution and use in source and binary forms, with or without modification, are permitted provided that the following conditions are met:

    * Redistributions of source code must retain the above copyright notice, this list of conditions and the following disclaimer.
    * Redistributions in binary form must reproduce the above copyright notice, this list of conditions and the following disclaimer in the documentation and/or other materials provided with the distribution.
    * Neither the name of the Vodafone Group Services Ltd nor the names of its contributors may be used to endorse or promote products derived from this software without specific prior written permission.

THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS" AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE ARE DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT HOLDER OR CONTRIBUTORS BE LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
*/


#define CORE_LOGGING_MODULE_NAME "RegTest"
//#define CORE_LOGGING_MODULE_LEVEL CL_ALL_FILE

#include "CoreDebugPrint.h"

#include "RegressionTestMarathon.h"

#ifndef __SYMBIAN__
# include <iostream>
#endif
#include "RegressionTestEnvironment.h"
#include "RegressionTestUtil.h"
#include "Nav2Session.h"

namespace RegressionTests {

const char* defaultSuite[] = {
                             "UnitTest",
#ifdef __linux__
                             "PALThreadingTest",
                             //"PALNetworkRegTest",
#endif
#ifndef __SYMBIAN__
                             "ReadyToRunListenerTest",
#endif
                             "Nav2InitTest",
                                 "NetworkSetupTest",
                             // "SettingsTest",
                             // "ImageDownloadTest",
                             "InvalidInputSearchTest",
                             "SearchRegTest",
                             "OneSearchTest",
                             "CategoryTreeRegTest",
                             "FavouriteRegTest",
                             "TunnelInterfaceRegTest",
                             // "SearchOverflowTest",
                             // "SearchImagesTest",
                             // "LocationHardwareTest",
                             // "CellIDTest",
                             // "LocationPositionTest",
                             // "RouteSimulationTest",
                             "MapLibInitTest",
                             "ConnectionManagerTest",
                             "TileMapTimerTest",
                             "MapDownloadTest",
                             "Nav2ShutdownTest",
                             "MapLibDeleteTest",
                             "Nav2DeleteTest",
                             };

using namespace RegressionTestUtil;

int runTests(int argc, char** argv) {

   RegressionTestEnvironment::init(argc, argv);
   
   Nav2Session nav2Session;

   RegTestSuite testSuite;
   
   if(RegressionTestEnvironment::isMarathonTest()) {
      testSuite = RegressionTestMarathon::initSuite();
   } else if(RegressionTestEnvironment::userSuppliedTests()) {
      testSuite = RegressionTestEnvironment::getUserSuppliedTests();
   } else {
      testSuite = RegTestSuite(defaultSuite, defaultSuite +
                               sizeof(defaultSuite) / sizeof(char*));
   }
   
   RegTestMap availableTests = getTests(nav2Session);
   
   RegTestVec tests = setupTestVector(availableTests, nav2Session, testSuite);
   
   coreprintln_info("** Regression testing util **");
   int result=0;
   for(RegTestVec::iterator itr = tests.begin(); itr != tests.end(); itr++)
   {
      result += doTestAndDelete(*itr, std::distance(tests.begin(), itr) + 1,
                      tests.size());
   }
   
   RegressionTestEnvironment::close();

   return result;
}

} // End of namespace RegressionTests
