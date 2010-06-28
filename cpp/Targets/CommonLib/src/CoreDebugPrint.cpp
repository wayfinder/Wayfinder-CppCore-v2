/*
Copyright (c) 1999 - 2010, Vodafone Group Services Ltd
All rights reserved.

Redistribution and use in source and binary forms, with or without modification, are permitted provided that the following conditions are met:

    * Redistributions of source code must retain the above copyright notice, this list of conditions and the following disclaimer.
    * Redistributions in binary form must reproduce the above copyright notice, this list of conditions and the following disclaimer in the documentation and/or other materials provided with the distribution.
    * Neither the name of the Vodafone Group Services Ltd nor the names of its contributors may be used to endorse or promote products derived from this software without specific prior written permission.

THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS" AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE ARE DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT HOLDER OR CONTRIBUTORS BE LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
*/


#define CORE_LOGGING_MODULE_NAME "CoreDebugPrint"
#define CORE_LOGGING_MODULE_LEVEL CL_ALL_FILE

#include "CoreDebugPrint.h"
#include <stdio.h>
#include <cstdarg>
#include <stdlib.h>
#include <string.h>
#include <string>
#include "FileSystem.h"
#include <set>
#include "Mutex.h"

#ifdef _WIN32_WCE
#include <windows.h>
#endif

#ifdef __SYMBIAN32__
#include <e32debug.h>
#endif 
static std::string s_outputFileName;

#define BUF_SIZE 512

// #define CORE_LOGGING_VERBOSE

#if defined __linux__
#define LOG_FILE_ROOT "/tmp"
#elif defined IPHONE
#include <IPhonePathUtil.h>
#define LOG_FILE_ROOT IPhonePathUtil::getDocumentsPath().c_str()
#else
#define LOG_FILE_ROOT NULL
#endif

static isab::Mutex* s_printMutex = NULL;

#ifdef __linux__
static unsigned int s_pid = 0;
#endif

CoreFuncTracer::CoreFuncTracer(const char* func,
                               const char* moduleName,
                               bool commitToFile)
{
   m_func = func;
   m_moduleName = moduleName;
   m_commitToFile = commitToFile;
   
   coreprintln_internal(m_moduleName,
                        CL_DEBUG,
                        m_commitToFile,
                        "+++ %s begin +++",
                        m_func);
}

CoreFuncTracer::~CoreFuncTracer() {
   coreprintln_internal(m_moduleName,
                        CL_DEBUG,
                        m_commitToFile,
                        "--- %s end ---",
                        m_func);
}




void createFormatString(char* formatBuf, const char* formatStr,
                        int debugLevel, const char* moduleName)
{
   const char* levelTag = NULL;

   switch(debugLevel) {
   case CL_INFO:
      levelTag = "INF";
      break;
   case CL_WARN:
      levelTag = "WRN";
      break;
   case CL_ERROR:
      levelTag = "ERR";
      break;
   case CL_DEBUG:
      levelTag = "DBG";
      break;
   }
   
#if defined __linux__ || defined IPHONE
   time_t tt;
   struct tm* tm;

   ::time(&tt);
   tm = localtime(&tt);

#ifdef CORE_LOGGING_VERBOSE
   sprintf(formatBuf, "WF[%d]: [%02d-%02d-%02d %02d:%02d:%02d] [%s-%4s]: %s",
           s_pid,
           tm->tm_year + 1900 - 2000, tm->tm_mon + 1, tm->tm_mday,
           tm->tm_hour, tm->tm_min, tm->tm_sec,
           moduleName, levelTag, formatStr);
#else
   sprintf(formatBuf, "[%02d:%02d:%02d] [%s-%4s]: %s",
           tm->tm_hour, tm->tm_min, tm->tm_sec,
           moduleName, levelTag, formatStr);
#endif
#elif defined _WIN32_WCE
   SYSTEMTIME st;
   GetSystemTime(&st);
   int hour = st.wHour;
   int minute = st.wMinute;
   int second = st.wSecond;

   sprintf(formatBuf, "[%02d:%02d:%02d] [%3s %-23s]: %s",
           hour, minute, second,
           levelTag, moduleName, formatStr);

   
#else
   sprintf(formatBuf, "[%s] %s: %s", moduleName, levelTag, formatStr);
#endif
}

void appendNewSessionInfo(FILE* f) {
#ifdef __linux__
   // Append processs id.
   pid_t pid = getpid();

   char msg[512];

   sprintf(msg, "\n*** NEW PROCESS STARTED, pid = %d\n", pid);
   
   FileSystem::corefwrite(f, msg);
#else
   FileSystem::corefwrite(f, "\n");
#endif
}

void commitLogToFile(const char* fileName, char* str)
{
   // typedef std::set<std::string> LogSet;
   // static LogSet g_startedLogs;

   static bool s_firstLog = true;
   
   FILE* f = FileSystem::openFile(fileName);
      
   if(f) {
      // LogSet::iterator itr = g_startedLogs.find(fileName);
      // if(itr == g_startedLogs.end()) {
      if(s_firstLog) {
         appendNewSessionInfo(f);

         s_firstLog = false;
         // g_startedLogs.insert(fileName);
      } 
         
      FileSystem::corefwrite(f, str);
      FileSystem::closeFile(f);
   }
}

void coreprintln_internal(const char* moduleName,
                          unsigned int debugLevel,
                          bool commitToFile,
                          const char* formatStr, ...)
{
   if(s_printMutex) {
      s_printMutex->lock();
   }
   
   char formatBuf[BUF_SIZE];
   
   memset(formatBuf, 0, BUF_SIZE * sizeof(char));
   
   // Empty lines are output as empty lines.
   if(strlen(formatStr) > 0) {
      if(moduleName) {
         createFormatString(formatBuf, formatStr, debugLevel, moduleName);
      } else {
         sprintf(formatBuf, "%s", formatStr);
      }
   }
   
   char buf[BUF_SIZE];
   memset(buf, 0, BUF_SIZE * sizeof(char));
   
   va_list args;
   va_start(args, formatStr);
   vsprintf(buf, formatBuf, args);
   va_end(args);

   if(commitToFile && LOG_FILE_ROOT != NULL) {
      char fileBuf[BUF_SIZE];
      // sprintf(fileBuf, "%s/CORE_LOG_%s.txt", LOG_FILE_ROOT, moduleName);
      // commitLogToFile(fileBuf, buf);
      sprintf(fileBuf, "%s/CORE_LOG_ALL.txt", LOG_FILE_ROOT);
      commitLogToFile(fileBuf, buf);
   }
   
#ifdef __SYMBIAN32__
   // Allocate a symbian specific string that will work with RDebug.
   
   TPtrC8 ptr(reinterpret_cast < const TUint8 * > (buf));

   HBufC*  buffer = HBufC::NewL(ptr.Length());
   
   buffer->Des().Copy(ptr);

   RDebug::Print(_L("%S"), buffer);

   delete buffer;      

   // printf("%s\n", buf);
   
#elif defined(__linux__) || defined (IPHONE) || defined(_WIN32_WCE)
   printf("%s\n", buf);
#endif

   if(s_printMutex) {
      s_printMutex->unlock();
   }
}

#ifndef __SYMBIAN32__
// This can not be allowed for symbian since no CleanupStack has
// been initialized. We need to change in two places:
// 1 - createFormatString, here we can not use sprintf to format
//     the string.
// 2 - coreprintln_internal, here we can't use HBufC, we should
//     instead use TBuf<BUF_SIZE>
class ProgramSentry {
public:
   ProgramSentry() {
#ifdef __linux__
      if(s_pid == 0) {
         s_pid = getpid();
      }
#endif
      coreprintln_info("Program starting..");
   }

   ~ProgramSentry() {
      coreprintln_info("Program exiting..");
   }
   
} s_programSentry;
#endif

void coreprintln_init()
{
   s_printMutex = new isab::Mutex;

#ifdef __linux__
   s_pid = getpid();
#endif
}

void coreprintln_deinit()
{
   delete s_printMutex;
   s_printMutex = NULL;
}


