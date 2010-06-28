/*
Copyright (c) 1999 - 2010, Vodafone Group Services Ltd
All rights reserved.

Redistribution and use in source and binary forms, with or without modification, are permitted provided that the following conditions are met:

    * Redistributions of source code must retain the above copyright notice, this list of conditions and the following disclaimer.
    * Redistributions in binary form must reproduce the above copyright notice, this list of conditions and the following disclaimer in the documentation and/or other materials provided with the distribution.
    * Neither the name of the Vodafone Group Services Ltd nor the names of its contributors may be used to endorse or promote products derived from this software without specific prior written permission.

THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS" AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE ARE DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT HOLDER OR CONTRIBUTORS BE LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
*/

#include "FileReaderThread.h"
#include "MainThreadScheduler.h"
#include <sched.h>
#include "DirectoryDBufRequester.h"
#include "BitBuffer.h"
#include <deque>
#include <vector>
#include "PlatformFactory.h"
#include "TileMapUtil.h"
#include <pthread.h>

using namespace std;

void mainThreadCallback(void* data) {
   DirectoryDBufRequester* req = (DirectoryDBufRequester*)(data);
   req->dealWithLoadedBuffers();
}

struct FileReaderThread::Impl {
   bool terminated;
   string path;
   pthread_mutex_t     mutex;
   pthread_cond_t      cond;
   bool workToDo;
   bool callbackDealtWith;
   deque<job_t> work;
   vector<job_t> finished;
   DirectoryDBufRequester* parent;
   MainThreadScheduler* scheduler;
};

FileReaderThread::FileReaderThread(const char* path,
                                   DirectoryDBufRequester* parent) {
   m_impl = new Impl;
   m_impl->path = path;
   m_impl->parent = parent;
   m_impl->terminated = false;
   pthread_mutex_t tmpMutex = PTHREAD_MUTEX_INITIALIZER;
   pthread_cond_t tmpCond = PTHREAD_COND_INITIALIZER;
   m_impl->mutex = tmpMutex;
   m_impl->cond = tmpCond;
   m_impl->callbackDealtWith = true;
   m_impl->workToDo = false;
   m_impl->scheduler = PlatformFactory::createMainThreadSheduler();
}

FileReaderThread::~FileReaderThread() {
   delete m_impl->scheduler;
   delete m_impl;
}

void FileReaderThread::addWork(job_t j) {
   int rc;
   rc = pthread_mutex_lock(&m_impl->mutex);
   m_impl->work.push_back(j);
   m_impl->workToDo = true;
   rc = pthread_cond_broadcast(&m_impl->cond);
   rc = pthread_mutex_unlock(&m_impl->mutex);
}

void FileReaderThread::cancelWork(job_t j) {
   int rc;
   rc = pthread_mutex_lock(&m_impl->mutex);
   
   for(unsigned int i = 0; i < m_impl->work.size(); i++) {
      job_t& cur = m_impl->work[i];
      if(cur.descr == j.descr && cur.caller == j.caller) {
         m_impl->work.erase(
            m_impl->work.begin() + i);
         break;
      }
   }
   
   rc = pthread_mutex_unlock(&m_impl->mutex);
}

void FileReaderThread::kill() {
   m_impl->terminated = true;
}
   
void FileReaderThread::run() {      
   while(!m_impl->terminated) {
      int rc = pthread_mutex_lock(&m_impl->mutex);
                
      // Wait for work to arrive
      while(!m_impl->workToDo && !m_impl->callbackDealtWith) {
         rc = pthread_cond_wait(&m_impl->cond, &m_impl->mutex);
      }

      // Get some work to do
      static const size_t MAX_WORK_UNITS = 1;
         
      size_t numWorkToDo = std::min(m_impl->work.size(), MAX_WORK_UNITS);

      vector<job_t> jobsToDo;
         
      for(unsigned int i = 0; i < numWorkToDo; i++) {
         jobsToDo.push_back(m_impl->work[0]);
         m_impl->work.pop_front();
      }
         
      if(m_impl->work.empty()) {
         m_impl->workToDo = false;            
      }

      vector<job_t> finishedTmp;
         
      // RELEASE MUTEX, DO WORK
      unsigned int start = TileMapUtil::currentTimeMillis();
         
      rc = pthread_mutex_unlock(&m_impl->mutex);
         
      for(unsigned int i = 0; i < jobsToDo.size(); i++) {
         jobsToDo[i].res = readFromFile(jobsToDo[i].descr);
         finishedTmp.push_back(jobsToDo[i]);
      }

      // // Sleep some time..
      usleep(100 * 1000);
         
      // REACQUIRE MUTEX, NOTIFY CALLBACK
      rc = pthread_mutex_lock(&m_impl->mutex);
         
      m_impl->finished.insert(m_impl->finished.end(), finishedTmp.begin(), finishedTmp.end());
         
      if(m_impl->callbackDealtWith) {
         // Loaded all buffers, time to notify..
         m_impl->callbackDealtWith = false;
         m_impl->scheduler->scheduleOnMainThread(mainThreadCallback, m_impl->parent);
      }
            
      rc = pthread_mutex_unlock(&m_impl->mutex);
   }

   delete this;
   pthread_exit(NULL);      
}

vector<FileReaderThread::job_t> FileReaderThread::acquireFinished() {
   int rc;
   rc = pthread_mutex_lock(&m_impl->mutex);
   vector<job_t> tmp = m_impl->finished;
   m_impl->finished.clear();
   m_impl->callbackDealtWith = true;
   rc = pthread_mutex_unlock(&m_impl->mutex);
   return tmp;
}

BitBuffer* FileReaderThread::readFromFile(const MC2SimpleString& descr) {
   string completePath =
      m_impl->path +
      PlatformFactory::getPathSeparator() +
      string(descr.c_str());
   
   FILE* file = fopen(completePath.c_str(), "r");

   BitBuffer* retBuf = NULL;
   
   if ( file ) {
      printf("[DirectoryDBufRequester]: Successfully opened: %s\n", completePath.c_str());
      // Seek to the end of file
      int seekRes = fseek(file, 0, SEEK_END);
      if ( seekRes < 0 ) {
         fclose(file);
      }
      long fileSize = ftell(file);
      // Set back the position.
      seekRes = fseek(file, 0, SEEK_SET);
      retBuf = new BitBuffer(fileSize);
      // FIXME: How about some error checks?
      fread(retBuf->getBufferAddress(), fileSize, 1, file);
      fclose(file);
   } else {
      printf("[DirectoryDBufRequester]: Failed to open: %s\n", completePath.c_str());
      // No file yet.
   }
   
   return retBuf; 
}
   

