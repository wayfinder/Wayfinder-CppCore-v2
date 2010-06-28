/*
Copyright (c) 1999 - 2010, Vodafone Group Services Ltd
All rights reserved.

Redistribution and use in source and binary forms, with or without modification, are permitted provided that the following conditions are met:

    * Redistributions of source code must retain the above copyright notice, this list of conditions and the following disclaimer.
    * Redistributions in binary form must reproduce the above copyright notice, this list of conditions and the following disclaimer in the documentation and/or other materials provided with the distribution.
    * Neither the name of the Vodafone Group Services Ltd nor the names of its contributors may be used to endorse or promote products derived from this software without specific prior written permission.

THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS" AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE ARE DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT HOLDER OR CONTRIBUTORS BE LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
*/


#ifndef _COREDEBUGPRINT_H_
#define _COREDEBUGPRINT_H_


/**
 *  Printf-style util that works the way we want across multiple platforms.
 *
 *  For symbian it will output to RDebug for example.
 *  
 */
void coreprintln_internal(const char* moduleName,
                          unsigned int debugLevel,
                          bool commitToFile,
                          const char* formatStr, ...);

/**
 * Implementation of function tracer for output configs.
 */ 
class CoreFuncTracer {
public:
   CoreFuncTracer(const char* func,
                  const char* moduleName,
                  bool commitToFile);
   ~CoreFuncTracer();
private:
   const char* m_func;
   const char* m_moduleName;
   bool m_commitToFile;
};

/**
 * Various convenience macros to function above.
 */

// If no module name is set, set it here
#ifndef CORE_LOGGING_MODULE_NAME
#  define CORE_LOGGING_MODULE_NAME "XXX"
#endif

// The different debug levels
#define CL_NONE 0    // If set commit no output
#define CL_INFO 1    // If set commit coreprintln_info
#define CL_WARN 2    // If set commit coreprintln_warn
#define CL_ERROR 4   // If set commit coreprintln_err
#define CL_DEBUG 8   // If set commit coreprintln
#define CL_FILE 16   // Log to file aswell
#define CL_ALL (CL_INFO | CL_WARN | CL_ERROR | CL_DEBUG)
#define CL_ALL_FILE (CL_INFO | CL_WARN | CL_ERROR | CL_DEBUG | CL_FILE)

// If no level is set, disable all modes
#ifndef CORE_LOGGING_MODULE_LEVEL
#  define CORE_LOGGING_MODULE_LEVEL CL_NONE
#endif

// If we should commit logs to file aswell
#define CORE_LOGGING_COMMIT_TO_FILE (CL_FILE & CORE_LOGGING_MODULE_LEVEL)

// Info macro
#if defined NAV2_CLIENT_DEBUG && (CORE_LOGGING_MODULE_LEVEL & CL_INFO)
#define coreprintln_info(...) coreprintln_internal(CORE_LOGGING_MODULE_NAME, \
                                                   CL_INFO,             \
                                                   CORE_LOGGING_COMMIT_TO_FILE, \
                                                     __VA_ARGS__)
#else                     
#define coreprintln_info(...) ((void)0)
#endif

// Warning macro
#if defined NAV2_CLIENT_DEBUG && (CORE_LOGGING_MODULE_LEVEL & CL_WARN)
#define coreprintln_warn(...) coreprintln_internal(CORE_LOGGING_MODULE_NAME, \
                                                   CL_WARN,             \
                                                   CORE_LOGGING_COMMIT_TO_FILE, \
                                                   __VA_ARGS__)
#else                     
#define coreprintln_warn(...) ((void)0)
#endif

// Error macro
#if defined NAV2_CLIENT_DEBUG && (CORE_LOGGING_MODULE_LEVEL & CL_ERROR)
#define coreprintln_error(...) coreprintln_internal(CORE_LOGGING_MODULE_NAME, \
                                                    CL_ERROR,           \
                                                    CORE_LOGGING_COMMIT_TO_FILE, \
                                                    __VA_ARGS__)
#else                     
#define coreprintln_error(...) ((void)0)
#endif

// Debug macro
#if defined NAV2_CLIENT_DEBUG && (CORE_LOGGING_MODULE_LEVEL & CL_DEBUG)

// Function tracer. Call coretracefunc() at the beginning of each function to
// be traced.
#define coretracefunc(...) \
   CoreFuncTracer __core_func_tracer(__FUNCTION__, \
                                     CORE_LOGGING_MODULE_NAME, \
                                     CORE_LOGGING_COMMIT_TO_FILE)

#define coreprintln(...) coreprintln_internal(CORE_LOGGING_MODULE_NAME, \
                                              CL_DEBUG,                 \
                                              CORE_LOGGING_COMMIT_TO_FILE, \
                                              __VA_ARGS__)

#else

#define coretracefunc(...) ((void)0)
#define coreprintln(...) ((void)0)


#endif

void coreprintln_init();
void coreprintln_deinit();

#endif /* _COREDEBUGPRINT_H_ */
