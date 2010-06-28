/*
Copyright (c) 1999 - 2010, Vodafone Group Services Ltd
All rights reserved.

Redistribution and use in source and binary forms, with or without modification, are permitted provided that the following conditions are met:

    * Redistributions of source code must retain the above copyright notice, this list of conditions and the following disclaimer.
    * Redistributions in binary form must reproduce the above copyright notice, this list of conditions and the following disclaimer in the documentation and/or other materials provided with the distribution.
    * Neither the name of the Vodafone Group Services Ltd nor the names of its contributors may be used to endorse or promote products derived from this software without specific prior written permission.

THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS" AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE ARE DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT HOLDER OR CONTRIBUTORS BE LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
*/


#include "CoreFunctionPrologue.h"
#include <stdio.h>
#include <assert.h>
#include "Thread.h"
//#include <iostream>
#include "CoreDebugPrint.h"

#if defined NAV2_ENABLE_STRESS_ASSERTS && !defined __SYMBIAN32__

static isab::Thread* g_currentThread = NULL;

CoreFunctionPrologueImpl::CoreFunctionPrologueImpl(const char* moduleName,
                                                   const char* info,
                                                   bool tracingEnabled)
   : m_moduleName(moduleName),
     m_info(info),
     m_tracingEnabled(tracingEnabled)
{
   if (m_tracingEnabled) {
      coreprintln_internal(m_moduleName, CL_DEBUG, false, "Entering: %s",m_info);
   }
   // Check if the current thread is the correct context.
   if (g_currentThread == NULL){
      // No thread context has been set, no need to ensure thread safety then..
   } else if ( g_currentThread != isab::Thread::currentThread() ){
      printf("The WFAPI library has been called from another thread, aborting.");
      assert(false);
   }    
}

CoreFunctionPrologueImpl::~CoreFunctionPrologueImpl()
{
   if (m_tracingEnabled) {
      coreprintln_internal(m_moduleName, CL_DEBUG, false, "Exiting : %s",m_info);
   }
}

void coreFunctionPrologueSetContextImpl()
{
   g_currentThread = isab::Thread::currentThread();
}
#else

#if !defined NAV2_DISABLE_STRESS_ASSERTS && !defined __SYMBIAN32__
#error "Not synchronized enable/disable"
#endif

#endif //NAV2_ENABLE_STRESS_ASSERTS

