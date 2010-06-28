/*
Copyright (c) 1999 - 2010, Vodafone Group Services Ltd
All rights reserved.

Redistribution and use in source and binary forms, with or without modification, are permitted provided that the following conditions are met:

    * Redistributions of source code must retain the above copyright notice, this list of conditions and the following disclaimer.
    * Redistributions in binary form must reproduce the above copyright notice, this list of conditions and the following disclaimer in the documentation and/or other materials provided with the distribution.
    * Neither the name of the Vodafone Group Services Ltd nor the names of its contributors may be used to endorse or promote products derived from this software without specific prior written permission.

THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS" AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE ARE DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT HOLDER OR CONTRIBUTORS BE LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
*/

#define CORE_LOGGING_MODULE_NAME "DirectoryDBufRequester"
#define CORE_LOGGING_MODULE_LEVEL CL_ALL_FILE

#include "CoreDebugPrint.h"
#include "DirectoryDBufRequester.h"
#include "BitBuffer.h"
#include <string>
#include "MC2SimpleString.h"
#include "FileReaderThread.h"

using namespace std;

struct DirectoryDBufRequester::Impl {   
   string path;
   pthread_t threadId;
   FileReaderThread* thread;
   TileMapToolkit* toolkit;
};

void *threadStartFunc(void *inData)
{
   FileReaderThread* thread = (FileReaderThread*)(inData);
   thread->run();
}

DirectoryDBufRequester::DirectoryDBufRequester(const char* path,
                                               bool writeable,
                                               TileMapToolkit* toolkit)
   : NotCopyable()
{
   m_impl = new Impl;
   m_impl->thread = new FileReaderThread(path, this);
   m_requesterType = OriginType::RESOURCE_DIRECTORY;
   m_impl->toolkit = toolkit;
   
   pthread_create(&m_impl->threadId, NULL, threadStartFunc, m_impl->thread);
   toolkit->requestIdle(this);
}

DirectoryDBufRequester::~DirectoryDBufRequester()
{
   m_impl->thread->kill();
   delete m_impl;
}

void DirectoryDBufRequester::request(const MC2SimpleString& descr,
                                     DBufRequestListener* caller,
                                     CachePolicy whereFrom )
{
   // printf("[DirectoryDBufRequester]: request(%s)\n",
   //        descr.c_str());
   
   if(whereFrom == CachePolicy::onlyResDir) {
      int rc;
      FileReaderThread::job_t j = {descr, caller, NULL, whereFrom, };
      m_impl->thread->addWork(j);      
   } else {
      DBufRequester::request(descr, caller, whereFrom);
   }
}

void DirectoryDBufRequester::cancelRequest(const MC2SimpleString& descr,
                                           DBufRequestListener* caller)
{
   FileReaderThread::job_t j = {descr, caller, NULL};
   
   m_impl->thread->cancelWork(j);   

   DBufRequester::cancelRequest(descr, caller);
}

void DirectoryDBufRequester::dealWithLoadedBuffers()
{
   int rc = 0;

   // printf("DirectoryDBufRequester::dealWithLoadedBuffers()\n");

   vector<FileReaderThread::job_t> tmp = m_impl->thread->acquireFinished();
   
   for(unsigned int i = 0; i < tmp.size(); i++) {
      FileReaderThread::job_t& f = tmp[i];

      if(f.res==NULL && f.whereFrom != CachePolicy::onlyResDir) {
         DBufRequester::request(f.descr, f.caller, f.whereFrom);
      } else {
         f.caller->requestReceived(f.descr, f.res, *this );         
      }
   }
}

void DirectoryDBufRequester::runIdleTask(uint32 id)
{
   
}
