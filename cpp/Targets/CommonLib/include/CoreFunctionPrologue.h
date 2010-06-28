/*
Copyright (c) 1999 - 2010, Vodafone Group Services Ltd
All rights reserved.

Redistribution and use in source and binary forms, with or without modification, are permitted provided that the following conditions are met:

    * Redistributions of source code must retain the above copyright notice, this list of conditions and the following disclaimer.
    * Redistributions in binary form must reproduce the above copyright notice, this list of conditions and the following disclaimer in the documentation and/or other materials provided with the distribution.
    * Neither the name of the Vodafone Group Services Ltd nor the names of its contributors may be used to endorse or promote products derived from this software without specific prior written permission.

THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS" AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE ARE DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT HOLDER OR CONTRIBUTORS BE LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
*/


#ifndef COREFUNCTIONPROLOGUE_H
#define COREFUNCTIONPROLOGUE_H

#if defined NAV2_ENABLE_STRESS_ASSERTS && !defined __SYMBIAN32__

#ifdef CORE_FUNCTION_PROLOGUE_ENABLE_TRACING
#define ENABLE_TRACING true
#else
#define ENABLE_TRACING false
#endif

class CoreFunctionPrologueImpl {
public:
   CoreFunctionPrologueImpl(const char* moduleName,
                            const char* info,
                            bool tracingEnabled);
   
   virtual ~CoreFunctionPrologueImpl();
private:
   const char* m_moduleName;
   const char* m_info;
   bool m_tracingEnabled;
};

void coreFunctionPrologueSetContextImpl();

#ifdef CORE_LOGGING_MODULE_NAME
#  define CORE_FUNCTION_PROLOGUE(...)\
   CoreFunctionPrologueImpl coreFunctionPrologue( \
      CORE_LOGGING_MODULE_NAME, __FUNCTION__, ENABLE_TRACING)
#else
   #define CORE_FUNCTION_PROLOGUE(...) \
      CoreFunctionPrologueImpl coreFunctionPrologue("", \
                                                    __FUNCTION__,    \
                                                    ENABLE_TRACING)
#endif

#define CORE_FUNCTION_PROLOGUE_SET_CONTEXT(...) \
   coreFunctionPrologueSetContextImpl()

#else

#define CORE_FUNCTION_PROLOGUE(...) ((void) 0)
#define CORE_FUNCTION_PROLOGUE_SET_CONTEXT(...) ((void) 0)

#endif // NAV2_ENABLE_STRESS_ASSERTS


#endif /* COREFUNCTIONPROLOGUE_H */
