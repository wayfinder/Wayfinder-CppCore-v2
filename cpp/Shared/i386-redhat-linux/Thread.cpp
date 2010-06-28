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
#include "JTC/JTCInit.h"
#include "Threading.h"
#ifdef ANDROID
# include <unistd.h>
#endif

#define CORE_LOGGING_MODULE_NAME "Thread"
//#define CORE_LOGGING_MODULE_LEVEL CL_ALL_FILE
#include "CoreDebugPrint.h"

namespace isab{

void waitTermination(){
   JTCInit::g_jtcInitialize->waitTermination();
}

/** Finds the currently executing Thread. 
 * @return a pointer to the currently executing Thread. 
 */
Thread* Thread::currentThread(){
  JTCThread* jtc = JTCThread::currentThread();
  InnerThread* inner = static_cast<InnerThread*>(jtc);
  return inner->thread();
} 
/** Documentation in the .h file.*/
int Thread::join(long millis, int nanos)
{
   int status = PROPER_JOIN;
   bool blocking = true;
   while(blocking && isAlive()){
      blocking = blocking && (millis < 0 && nanos < 0);
      try{
         if(nanos < 0 && millis < 0){
            m_innerThread->join();
         } else if(nanos < 0){
            m_innerThread->join(millis);
         } else {
            m_innerThread->join(millis, nanos);
         }
      }
      catch(JTCSystemCallException){
         status = JOIN_FAIL;
      }
      if(status != JOIN_FAIL && isAlive()){
         status = TIME_OUT;
      }
   }
   return status;
}

/** Does basic setup for all constructors. 
 * @param name the Thread's name. If name is NULL a name will be
 *             constructed.
 */
void Thread::init(const char* name){
  try{
    if(name != NULL){
      m_innerThread = new InnerThread(this, name);
    }else{
      m_innerThread = new InnerThread(this);
    }
  }
  catch(JTCSystemCallException& e){
    m_innerThread = NULL;
  }
  m_runnable = NULL;
}
/** Virtual destructor.*/
Thread::~Thread(){
   const char* name = getName();
   
  if(m_innerThread->isAlive()){
    terminate();
    try{
      m_innerThread->join();
    } 
    catch(JTCSystemCallException){
      /* Never mind. */
    }
  }

  coreprintln("Thread %s deleted.", name);
}

int Thread::start(){
  int status = INIT_FAILED;
  if(m_innerThread){
    status = GOOD_START;
    try{
      m_innerThread->start();
    }
    catch(JTCSystemCallException){
      status = UNKNOWN_FAILURE;
    }
    catch(JTCIllegalThreadStateException){
      status = ALREADY_STARTED;
    }
    if(status == GOOD_START){
      while(!m_innerThread->isAlive() && !m_innerThread->hasDied());
	 }
  }
  return status;
}

int Thread::sleep(long millis, int nanos){
  int status = AWOKE;
  try{
    JTCThread::sleep(millis, nanos);
  }
  catch(JTCSystemCallException){
    status = FAIL;
  }
  catch(JTCIllegalArgumentException){
    status = ILLEGAL_ARGUMENTS;
  }
  catch(JTCInterruptedException){
    status = INTERRUPTED;
  }
  return status;
}

int Thread::getThreadInfo(char* data, int dataLength)
{
   snprintf(data, dataLength, 
            "%s: JTC id: %d",
            getName(), 
            pthread_t(m_innerThread->getId()));
   data[dataLength - 1] = '\0';
   return strlen(data);
}


void Thread::InnerThread::run(){
#ifdef ANDROID
   int tid = gettid();
#else
   int tid = syscall(SYS_gettid);
#endif

   coreprintln_info("Thread %s started, LWP id = %d", getName(), tid);
      
   if(m_thread->m_runnable != NULL){
      m_thread->m_runnable->run();
   } else {
      m_thread->run();
   }
   
   coreprintln_info("Thread %s exited, LWP id = %d", getName(), tid);
}

} // End of namespace isab
      
  


