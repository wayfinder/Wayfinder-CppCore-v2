/*
Copyright (c) 1999 - 2010, Vodafone Group Services Ltd
All rights reserved.

Redistribution and use in source and binary forms, with or without modification, are permitted provided that the following conditions are met:

    * Redistributions of source code must retain the above copyright notice, this list of conditions and the following disclaimer.
    * Redistributions in binary form must reproduce the above copyright notice, this list of conditions and the following disclaimer in the documentation and/or other materials provided with the distribution.
    * Neither the name of the Vodafone Group Services Ltd nor the names of its contributors may be used to endorse or promote products derived from this software without specific prior written permission.

THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS" AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE ARE DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT HOLDER OR CONTRIBUTORS BE LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
*/


#ifndef REGRESSIONTESTUTIL_H
#define REGRESSIONTESTUTIL_H

#include <vector>
#include <string>
#include <map>

class RegressionTest;
class Nav2Session;

namespace RegressionTestUtil {

/// A vector of runnable regression tests
typedef std::vector<RegressionTest*> RegTestVec;

/// Function which spawns an instance of a test
typedef RegressionTest*(*SpawnFunc)(Nav2Session& session,
                                    const char* name);

/// A map containing all valid regression tests for the platform
typedef std::map<std::string, SpawnFunc> RegTestMap;

/// A string vector representing the tests that are to be run
typedef std::vector<std::string> RegTestSuite;

/**
 *  Performs the RegressionTest test and deletes it. Treats it as being number
 *  testNum out of numTests total.
 *
 *  @param test The regression test to run.  
 *  @param testNum The number of the test.
 *  @param numTests The total number of tests.
 *  @return 0 if test passed, error code otherwise.
 */ 
int doTestAndDelete(RegressionTest* test, int testNum, unsigned int numTests);

/**
 *  Returns a map of all the available tests in this compiled instance
 *  of the library.
 *
 *  @param session The nav2 session that the tests will run under.
 *  @return a map containing all of the available tests.
 */ 
RegTestMap getTests(Nav2Session& session);

/**
 *   Based on a set of string representations of tests a vector of
 *   actual regression tests are created using the available tests
 *   in availableTests.
 *   
 *   @param availableTests The map that contains all of the available tests.
 *   @param testsToRun The tests that should be run.
 *   @return The vector containing the resulting tests.
 */ 
RegTestVec setupTestVector(RegTestMap& availableTests,
                           Nav2Session& session,
                           const RegTestSuite& testsToRun);

/**
 *   This function adds all the tests available in availableTests
 *   and returns it in a vector.
 *
 *   @param availableTests The map that contains all of the available tests.
 *   @return The vector containing the resulting test.
 */ 
RegTestVec setupTestVectorWithAllTests(RegTestMap& availableTests,
                                       Nav2Session& session);

} // End of namespace RegressionTestUtil

#endif /* REGRESSIONTESTUTIL_H */
