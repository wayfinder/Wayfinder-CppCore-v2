/*
Copyright (c) 1999 - 2010, Vodafone Group Services Ltd
All rights reserved.

Redistribution and use in source and binary forms, with or without modification, are permitted provided that the following conditions are met:

    * Redistributions of source code must retain the above copyright notice, this list of conditions and the following disclaimer.
    * Redistributions in binary form must reproduce the above copyright notice, this list of conditions and the following disclaimer in the documentation and/or other materials provided with the distribution.
    * Neither the name of the Vodafone Group Services Ltd nor the names of its contributors may be used to endorse or promote products derived from this software without specific prior written permission.

THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS" AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE ARE DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT HOLDER OR CONTRIBUTORS BE LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
*/


#ifndef _UNITTESTFRAMEWORK_H_
#define _UNITTESTFRAMEWORK_H_
#include <cstdarg>
#include <vector>

class UnitTestFramework;

typedef void (*TestFunc)(UnitTestFramework&);

struct UnitTestStruct {
   TestFunc func;
   const char* name;
};


class UnitTestFramework {
public:
   typedef std::vector<UnitTestStruct> TestVector;
   
   static UnitTestFramework& instance();

   void addTest(UnitTestStruct t);

   TestVector& getTests();

   unsigned int getNumTests() const;

   unsigned int getNumFailedTests() const;

   void addNewTest();

   void setTestFailed(const char* file, unsigned int line,
                      const char* expression, ...);  
private:
   UnitTestFramework();

   TestVector m_tests;

   unsigned int m_numTests;
   unsigned int m_numFailedTests;
};

struct TestAdder {
   TestAdder(TestFunc f, const char* name, UnitTestFramework& env) {
      UnitTestStruct unitTest = {f, name };

      env.addTest(unitTest);
   }
};

// struct UnitTestFrameworkInit {
//    UnitTestFramework() {
//       UnitTestFramework::instance().addNewTest();
//    }
// };

#define UT_NEW_TEST(NAME) \
   void UnitTest_##NAME(UnitTestFramework& env);     \
   TestAdder NAME##Adder(UnitTest_##NAME, #NAME, UnitTestFramework::instance()); \
   void UnitTest_##NAME(UnitTestFramework& env) 

#define UT_ASSERT(condition)                         \
   if(!(condition)) {                                \
      env.setTestFailed(__FILE__, __LINE__, #condition);  \
      return;                                           \
      }                                                 \

#define UT_FAIL(...)                            \
   env.setTestFailed(__FILE__, __LINE__, __VA_ARGS__);     \
   return;

#define UT_ADD_TEST_FILE(NAME)                  \
   void NAME##__REG() {}

#define UT_REFERENCE_TEST_FILE(NAME)            \
   void NAME##__REG();                          \
   void NAME##__REF() {  NAME##__REG(); }

#endif /* _UNITTESTFRAMEWORK_H_ */
