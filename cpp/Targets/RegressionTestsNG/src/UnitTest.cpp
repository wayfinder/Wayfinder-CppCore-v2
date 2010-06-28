/*
Copyright (c) 1999 - 2010, Vodafone Group Services Ltd
All rights reserved.

Redistribution and use in source and binary forms, with or without modification, are permitted provided that the following conditions are met:

    * Redistributions of source code must retain the above copyright notice, this list of conditions and the following disclaimer.
    * Redistributions in binary form must reproduce the above copyright notice, this list of conditions and the following disclaimer in the documentation and/or other materials provided with the distribution.
    * Neither the name of the Vodafone Group Services Ltd nor the names of its contributors may be used to endorse or promote products derived from this software without specific prior written permission.

THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS" AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE ARE DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT HOLDER OR CONTRIBUTORS BE LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
*/


#define CORE_LOGGING_MODULE_NAME "UnitTest"
#define CORE_LOGGING_MODULE_LEVEL CL_ALL_FILE

#include "CoreDebugPrint.h"
#include "UnitTest.h"
#include "UnitTestFramework.h"

// Add all reference invocations here
#if !defined(__SYMBIAN__) && !defined(_WIN32_WCE)
UT_REFERENCE_TEST_FILE(JTCTestTryLock);
UT_REFERENCE_TEST_FILE(JTCUncaughtException);
UT_REFERENCE_TEST_FILE(JTCStaticMonitorTest);
UT_REFERENCE_TEST_FILE(JTCReentrantMonitorTest);
UT_REFERENCE_TEST_FILE(JTCMonitorTest2);
UT_REFERENCE_TEST_FILE(JTCMonitorTest);
UT_REFERENCE_TEST_FILE(JTCDeadMonitorTest);
UT_REFERENCE_TEST_FILE(JTCTestRWLock);
UT_REFERENCE_TEST_FILE(JTCWaitTest);
UT_REFERENCE_TEST_FILE(JTCThreadCreateTest);
UT_REFERENCE_TEST_FILE(JTCThreadStateTest);
UT_REFERENCE_TEST_FILE(JTCTSSTest);
UT_REFERENCE_TEST_FILE(JTCThreadExitTest);
UT_REFERENCE_TEST_FILE(JTCThreadIDTest);
#endif

#if defined(_WIN32_WCE)
UT_REFERENCE_TEST_FILE(WinTileMapToolkitTests);
#endif

UnitTest::UnitTest(Nav2Session& session, const char* name)
   : RegressionTest(name)
{
    
}

void UnitTest::startTest()
{   
   typedef UnitTestFramework::TestVector TestVector; 
   
   UnitTestFramework& framework = UnitTestFramework::instance();

   TestVector& tests = framework.getTests();
   
   for(TestVector::iterator itr = tests.begin(); itr != tests.end(); itr++)
   {
      framework.addNewTest();
      UnitTestStruct& t = *itr;
      coreprintln("+ Starting unit test %s", t.name);
      t.func(framework);
   }

   if(framework.getNumTests() == 0) {
      coreprintln("No unit tests registered.");
      setTestPassed();
   } else {      
      if (framework.getNumFailedTests() > 0) {
         setTestFailed("One or more unit tests did not pass.");
      } else {
         setTestPassed();      
      }
   }
}

