/*
  Copyright (c) 1999 - 2010, Vodafone Group Services Ltd
  All rights reserved.

  Redistribution and use in source and binary forms, with or without modification, are permitted provided that the following conditions are met:

  * Redistributions of source code must retain the above copyright notice, this list of conditions and the following disclaimer.
  * Redistributions in binary form must reproduce the above copyright notice, this list of conditions and the following disclaimer in the documentation and/or other materials provided with the distribution.
  * Neither the name of the Vodafone Group Services Ltd nor the names of its contributors may be used to endorse or promote products derived from this software without specific prior written permission.

  THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS" AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE ARE DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT HOLDER OR CONTRIBUTORS BE LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
*/


#ifndef POCKETPC_OUTPUT_FACTORY_H
#define POCKETPC_OUTPUT_FACTORY_H
#include "machine.h"
#include <string.h>
#include "Output.h"
#include "nav2util.h"
typedef TCHAR tchar;
namespace isab{
   namespace {
   

      /***/
      class FileOutput : public Output{
      public:
         /***/
         FileOutput(const char* name);
         ~FileOutput();
      protected:
         /**/
         virtual size_t realPuts(const char* txt);
      private:
         FILE* m_file;
      };

      //==================================================
      //===== inlines for FileOutput =====================

      FileOutput::FileOutput(const char* name) :
         Output(), m_file(NULL)
      {
         if(name){
            m_file = fopen(name, "rt");
            if(m_file != NULL){
               fclose(m_file);
               m_file = NULL;
               // file already exists
               char* copyName = new char[strlen(name) + 1];
               strcpy(copyName, name);
               char* ending = strrchr(copyName, '.');
               if(ending != NULL){
                  *ending++ = '\0';
               }
               char* numName = new char[strlen(name) + 10];
               int i;
               for(i = 0; i < 50; ++i){
                  if(ending){
                     sprintf(numName, "%s%d.%s", copyName, i, ending);
                  } else {
                     sprintf(numName, "%s%d", copyName, i);
                  }
                  m_file = fopen(numName, "rt");
                  if(m_file == NULL){
                     break;
                  }
                  fclose(m_file);
               }
               if(i < 50){
                  m_file = fopen(numName, "wt");
               } else {
                  m_file = fopen(name, "wt");
               }
               delete[] copyName;
               delete[] numName;
            } else {
               m_file = fopen(name, "wt");
            }
         } 
      }

      FileOutput::~FileOutput()
      {
         if(m_file){
            fclose(m_file);
         }
      }   

      size_t FileOutput::realPuts(const char* txt)
      {
         int len = strlen(txt);
         if(m_file){
            fwrite(txt, sizeof(char), len, m_file);
         }
         return len;
      }

      //===== end of FileOutput inlines ==================
      //==================================================


      /** This output class handles output to the Windows CE TRACE facility. */
      class TraceOutput : public Output
      {
      public:
         /** Constructor
          */
         TraceOutput();
      protected:
         /** Outputs text to the TRACE facility.
          * @param txt the text to print.
          * @return the number of characters printed.
          */
         virtual size_t realPuts(const char* txt);

      private:
      };

      //==================================================
      //===== inlines for FileOutput =====================

      inline TraceOutput::TraceOutput() :
         Output()
      {
      }

      inline size_t TraceOutput::realPuts(const char* txt)
      {
#ifdef _DEBUG
         tchar *tmp;
         int len = 0;
         if(txt){
            len = dupChar2Tchar(txt, tmp);
            // TODO: Fix this
            // TRACE(tmp);
            delete[] tmp;
         }
         return len;
#else
         return 0;
#endif
      }
      //===== end of FileOutput inlines ==================
      //==================================================


      
   }
      

   Output* OutputFactory::createOutput(const char* target) 
   {
      Output* ret = NULL;
      if(target == NULL) target = "cerr";
      char* memref = new char[strlen(target) + 1];
      char* remaining = memref;
      strcpy(remaining, target);
      const char delim[] = ":";
      char* type = strsep(&remaining, delim);
      if(strequ(type, "null")){
	      ret = new NullOutput();
      }else if(strequ(type, "trace")){
         ret = new TraceOutput();
      }else if(strequ(type, "tcp")){
         uint16 thePort = 0;
         char* host = NULL;
         char* threadName = new char[strlen(remaining) + 1];
         strcpy(threadName, remaining);
         if(!isdigit(*remaining)){
            host = strsep(&remaining, delim);
         } 
         long port = strtol(remaining, NULL, 10);
         if(port > 1 && port <= MAX_UINT16){
            thePort = uint16(port & 0x0ffff);
         }
         if(host && thePort != 0){
            ret = new SocketOutput(threadName, host, thePort);
         }

         delete threadName;
         threadName = NULL;
      }else if(strequ(type, "file")){
         ret = new FileOutput(remaining);
      }
      delete[] memref;
      return ret;
   }

}
#endif
