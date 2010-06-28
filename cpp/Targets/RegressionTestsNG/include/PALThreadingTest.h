/*
Copyright (c) 1999 - 2010, Vodafone Group Services Ltd
All rights reserved.

Redistribution and use in source and binary forms, with or without modification, are permitted provided that the following conditions are met:

    * Redistributions of source code must retain the above copyright notice, this list of conditions and the following disclaimer.
    * Redistributions in binary form must reproduce the above copyright notice, this list of conditions and the following disclaimer in the documentation and/or other materials provided with the distribution.
    * Neither the name of the Vodafone Group Services Ltd nor the names of its contributors may be used to endorse or promote products derived from this software without specific prior written permission.

THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS" AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE ARE DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT HOLDER OR CONTRIBUTORS BE LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
*/


#ifndef PALTHREADINGTEST_H
#define PALTHREADINGTEST_H

#include "RegressionTest.h"
#include "PALThreading.h"

class PALThreadingTest : public RegressionTest {
public:

   PALThreadingTest(Nav2Session& session, const char* name);

   virtual ~PALThreadingTest();

   virtual void startTest();

   void threadJobfunction(void* arg);

   void threadJobfunctionRecursive(void* arg);

   void wait_for_data_to_process();

   void prepare_data_for_processing();

   void process_data();

private:

   pal::Mutex m_mutex;

   pal::RecursiveMutex m_recursiveMutex;
   
   int m_res;

   pal::Conditional m_cond;
   pal::Mutex m_condmut;
   bool m_data_ready;

};

void threadFunc(void* arg);
void threadFunc2(void* arg);
void threadFunc3(void* arg);

#endif /* PALTHREADINGTEST_H */
