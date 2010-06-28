/*
Copyright (c) 1999 - 2010, Vodafone Group Services Ltd
All rights reserved.

Redistribution and use in source and binary forms, with or without modification, are permitted provided that the following conditions are met:

    * Redistributions of source code must retain the above copyright notice, this list of conditions and the following disclaimer.
    * Redistributions in binary form must reproduce the above copyright notice, this list of conditions and the following disclaimer in the documentation and/or other materials provided with the distribution.
    * Neither the name of the Vodafone Group Services Ltd nor the names of its contributors may be used to endorse or promote products derived from this software without specific prior written permission.

THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS" AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE ARE DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT HOLDER OR CONTRIBUTORS BE LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
*/


#include "arch.h"
#include <e32std.h>
#include "InnerThread.h"
#include "nav2util.h"
#include "GlobalData.h"
#include "WFTextUtil.h"

TInt SetExceptionHandler(class RThread& aThread, TExceptionHandlerPtr aHandler)
{
#if defined NAV2_CLIENT_UIQ3 || defined NAV2_CLIENT_SERIES60_V3
   return KErrNone;
   // aThread.Handle(); //shut up the compiler.
   // return User::SetExceptionHandler(*aHandler, MAX_UINT32);
#else
   return aThread.SetExceptionHandler(aHandler, MAX_UINT32);
#endif
}

isab::InnerThread::
InnerThread(class Thread* thread,
            class Runnable* runnable,
            const char* name) :
   m_runnable(runnable),
   m_thread(thread),
   m_created(false)
{
   if ( name == NULL ) {
      name = "NULL";
   }
   m_name = strdup_new(name);
   // Don't use the name yet
}

isab::InnerThread::~InnerThread()
{
   if(m_created && m_threadHandle.ExitReason() == 0){
      m_threadHandle.Kill(1);
   }
   m_threadHandle.Close();
   delete[] m_name;
   m_name = NULL;
//    GlobalData* gd = getGlobalData();
//    gd->m_innerThread = NULL;
//    delete gd;
}

class isab::ThreadID
isab::InnerThread::getID() const
{
   return m_id;
}

class isab::Thread*
isab::InnerThread::getThread() const
{
   return m_thread;
}

class isab::Runnable*
isab::InnerThread::getRunnable() const
{
	return m_runnable;
}

class RThread&
isab::InnerThread::getHandle()
{
   return m_threadHandle;
}

const char* 
isab::InnerThread::getName() const
{
   return m_name;
}

void 
isab::InnerThread::setAlive( bool alive ) {
   m_thread->setAlive( alive );
}

int isab::InnerThread::getThreadInfo(char* data, int dataLength)
{
   snprintf(data, dataLength, 
            "%s : threadId : 0x%"PRIx32" : handle: 0x%"PRIx32,
            getName(), TUint(m_threadHandle.Id()), m_threadHandle.Handle());
   data[dataLength - 1] = '\0';
   return strlen(data);
}

TInt
isab::InnerThread::start_hook(TAny *aPtr)
{
   class TlsGlobalData* newTlsGlobalData = 
      reinterpret_cast<TlsGlobalData*>(aPtr);
   class ArchGlobalData& archGlobalData = *(newTlsGlobalData->m_archGlobal);

   setTlsGlobalData(aPtr);

   class InnerThread* innerThread = newTlsGlobalData->m_innerThread;
   class CTrapCleanup* tc = CTrapCleanup::New();

   TExceptionHandlerPtr eH = archGlobalData.m_exceptionHandler;
   if(KErrNone != SetExceptionHandler(innerThread->getHandle(), eH)){
   }

   TRAPD(trappederror, innerThread->m_runnable->run());
   if (trappederror != KErrNone) {
#ifdef STORE_TRAP_INFO_IN_FILE
      char fileName[256];
      strncpy(256, fileName, getGlobalData()->m_commondata_base_path);
      strcat(fileName, "trap.txt"); //may overflow
      
      FILE* foo = fopen(fileName, "w");
      if(foo){
         fprintf(foo, "Error %i in %s\n",trappederror, innerThread->getName());
         fclose(foo);
      }
#endif
      //starts a terminateandjoin process in the main thread.
      //now exit as gracefully as possible.
      ::Panic(innerThread->m_thread);
   }

   delete tc;
   innerThread->m_thread->setAlive(false);

   deleteTlsGlobalData(newTlsGlobalData);

   if(trappederror != KErrNone){
      User::Exit(1);
   }

   // The thread has stopped.
   return 0;
}

int
isab::InnerThread::start()
{
   //if ( m_id == m_threadHandle.Id() ) {
   if(m_thread->isAlive()){
      return Thread::ALREADY_STARTED;
   }

   // Copy the global section for the new thread. Fill it in as well.
   class TlsGlobalData * newTls = new TlsGlobalData(*getTlsGlobalData(), this);
   if(!newTls){
      return KErrNoMemory;
   }

   //widen thread name. 
   HBufC* tname = WFTextUtil::Alloc(m_name);
   if(!tname){
      delete newTls;
      return KErrNoMemory;
   }
   
   // Uses creators Heap
   TInt res = m_threadHandle.Create(
      *tname,
      /*(TInt (*)(TAny *))*/start_hook,
      KDefaultStackSize,//128*1024, // Stack Size
      NULL, // Use same heap as creator, can delete things created by other
      (TAny*)newTls // Data pointer to send to start_hook
      /*TOwnerType aType=EOwnerProcess*/ ); 

   delete tname;

   if ( res != KErrNone ) {
      delete newTls;
      switch(res){
         case KErrAlreadyExists:
            return Thread::ALREADY_STARTED + 100;
         case KErrBadName:
            return res;
         default:
            return res - 100;
      }
   } else {
      m_created = true;
   }
   m_id = m_threadHandle.Id();
   if (getArchGlobalData().m_use_exception_handler) {
      getArchGlobalData().m_exceptionHandler = &exceptionHandler;
   }
   m_thread->setAlive( true );

   m_threadHandle.Resume();
/*    Thread::sleep( 100, 0 ); */
/*    Thread::sleep( 100, 0 ); */

   return Thread::GOOD_START;
}

