/*
Copyright (c) 1999 - 2010, Vodafone Group Services Ltd
All rights reserved.

Redistribution and use in source and binary forms, with or without modification, are permitted provided that the following conditions are met:

    * Redistributions of source code must retain the above copyright notice, this list of conditions and the following disclaimer.
    * Redistributions in binary form must reproduce the above copyright notice, this list of conditions and the following disclaimer in the documentation and/or other materials provided with the distribution.
    * Neither the name of the Vodafone Group Services Ltd nor the names of its contributors may be used to endorse or promote products derived from this software without specific prior written permission.

THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS" AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE ARE DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT HOLDER OR CONTRIBUTORS BE LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
*/


#define CORE_LOGGING_MODULE_NAME "UnitTestFramework"
#define CORE_LOGGING_MODULE_LEVEL CL_ALL_FILE
#include "CoreDebugPrint.h"
#include "UnitTestFramework.h"
#include <cstdarg>
#include <string.h>
#include <cstdio>

#ifdef ANDROID
# include <stdio.h>
#endif

void UnitTestFramework::addTest(UnitTestStruct f)
{
   m_tests.push_back(f); 
}

UnitTestFramework& UnitTestFramework::instance()
{
   static UnitTestFramework ret;

   return ret;
}

UnitTestFramework::TestVector& UnitTestFramework::getTests()
{
   return m_tests; 
}

UnitTestFramework::UnitTestFramework()
{
   m_numTests = 0;
   m_numFailedTests = 0;
}

unsigned int UnitTestFramework::getNumTests() const
{
   return m_numTests; 
}

unsigned int UnitTestFramework::getNumFailedTests() const
{
   return m_numFailedTests; 
}

void UnitTestFramework::addNewTest()
{
   m_numTests++; 
}

void UnitTestFramework::setTestFailed(const char* file, unsigned int line,
                                      const char* expression, ...)
{
   char buf[255];
   va_list args;
   
   va_start(args, expression);
   vsprintf(buf, expression, args);
   va_end(args);
   
   coreprintln("%s:%u: %s", file, line, buf);
   m_numFailedTests++; 
}
