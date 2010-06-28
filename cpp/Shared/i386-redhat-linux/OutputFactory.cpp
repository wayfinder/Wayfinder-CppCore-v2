/*
  Copyright (c) 1999 - 2010, Vodafone Group Services Ltd
  All rights reserved.

  Redistribution and use in source and binary forms, with or without modification, are permitted provided that the following conditions are met:

  * Redistributions of source code must retain the above copyright notice, this list of conditions and the following disclaimer.
  * Redistributions in binary form must reproduce the above copyright notice, this list of conditions and the following disclaimer in the documentation and/or other materials provided with the distribution.
  * Neither the name of the Vodafone Group Services Ltd nor the names of its contributors may be used to endorse or promote products derived from this software without specific prior written permission.

  THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS" AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE ARE DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT HOLDER OR CONTRIBUTORS BE LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
*/


#ifndef i386_REDHAT_LINUX_OUTPUT_FACTORY_H
#define i386_REDHAT_LINUX_OUTPUT_FACTORY_H
#include "arch.h"
#include <string.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <unistd.h>
#include <iostream>
#include "nav2util.h"
#include "Output.h"
#include <stdlib.h>

using std::ostream;
using std::cerr;
using std::cout;
using std::clog;
namespace isab{
   namespace {
      /** Output class for ostreams.*/
      class StreamOutput : public Output{
      public:
         /** Constructor.
          * @param stream a stream to use. Defaults to cerr.
          */
         StreamOutput(std::ostream* stream = &std::cerr);
      protected:
         /** The output function.
          * @param txt the text to output.
          * @return the number of characters written.
          */
         virtual size_t realPuts(const char* txt);
      private:
         /** A pointer ot the ostream to use for output.*/
         std::ostream* m_stream;
      };
      
      //==================================================
      //===== inlines for StreamOutput ===================
      inline StreamOutput::StreamOutput(std::ostream* stream) : 
         Output(), m_stream(stream)
      {
      }

      size_t StreamOutput::realPuts(const char* txt)
      {
         *m_stream << txt;
         return strlen(txt) + 1;
      }

      //===== end of StreamOutput inlines. ===============
      //==================================================

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
         int m_fd;
      };

      //==================================================
      //===== inlines for FileOutput =====================

      FileOutput::FileOutput(const char* name) :
         Output(), m_fd(0)
      {
         if(name){
            int flags = 0 | O_WRONLY | O_CREAT | O_TRUNC;
            mode_t mode = S_IRUSR | S_IWUSR | S_IRGRP | S_IROTH;
            m_fd = ::open(name, flags, mode);
            if(m_fd < 1){
               m_fd = 0;
            }
         }
      }

      FileOutput::~FileOutput()
      {
         if(m_fd){
            ::close(m_fd);
         }
      }   

      size_t FileOutput::realPuts(const char* txt)
      {
         size_t ret = 0;
         if(m_fd){
            ret = ::write(m_fd, txt, strlen(txt));
         }
         return ret;
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
      }else if(strequ(type, "cerr")){
         ret = new StreamOutput(&std::cerr);
      }else if(strequ(type, "cout")){
         ret = new StreamOutput(&std::cout);
      }else if(strequ(type, "clog")){
         ret = new StreamOutput(&std::clog);
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

         delete[] threadName;
         threadName = NULL;
      }else if(strequ(type, "file")){
         ret = new FileOutput(remaining);
      }
      delete[] memref;
      return ret;
   }

}
#endif
