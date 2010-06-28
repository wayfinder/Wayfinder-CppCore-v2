/*
  Copyright (c) 1999 - 2010, Vodafone Group Services Ltd
  All rights reserved.

  Redistribution and use in source and binary forms, with or without modification, are permitted provided that the following conditions are met:

  * Redistributions of source code must retain the above copyright notice, this list of conditions and the following disclaimer.
  * Redistributions in binary form must reproduce the above copyright notice, this list of conditions and the following disclaimer in the documentation and/or other materials provided with the distribution.
  * Neither the name of the Vodafone Group Services Ltd nor the names of its contributors may be used to endorse or promote products derived from this software without specific prior written permission.

  THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS" AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE ARE DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT HOLDER OR CONTRIBUTORS BE LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
*/

/* #undef NO_LOG_OUTPUT */
#define CORE_LOGGING_MODULE_NAME "Nav2"
//#define CORE_LOGGING_MODULE_LEVEL CL_ALL_FILE

#include "CoreDebugPrint.h"
#include "Nav2.h"
#include "CtrlHubInternal.h"
#include "MsgBufferEnums.h"
#include "Module.h"
#include "Serial.h"
#include "Mutex.h"
#include "QueueSerial.h"
#include "GlobalData.h"
#include "TimerThread.h"
#include "nav2util.h"
#include "LogMacros.h"

#ifdef __SYMBIAN32__
void Panic(::isab::Thread* thread)
{
   ::isab::getGlobalData().m_nav2->Panic(thread);
}
#endif


namespace isab{

#ifndef __SYMBIAN32__
   // Global data pointer for architectures other than Symbian
   GlobalData *Nav2GlobalData;
#endif
   
   void Nav2::Halt()
   {
      coretracefunc();
      if(m_gui) m_gui->disconnect();
      if(m_bt)  m_bt->disconnect();
      StartShutdownNavigate2();
   }

   Nav2::Nav2(Nav2StartupData &sdata) :
      m_gui(NULL), m_bt(NULL),
      initialized(0),
      shutdownstarted(0),
      mainlog(NULL)
   {
      // Create the global data object
      m_globalData = new GlobalData;
#ifdef __SYMBIAN32__
      m_archGlobalData = new ArchGlobalData;
      m_tlsGlobalData  = new TlsGlobalData;
      m_tlsGlobalDataId = getThreadHandle();
      m_tlsGlobalData->m_global     = m_globalData;
      m_tlsGlobalData->m_archGlobal = m_archGlobalData;
      setTlsGlobalData(m_tlsGlobalData);
#else
      Nav2GlobalData = m_globalData;
#endif
      
      // Copy objects we do not assume ownership of.
      m_globalData->m_writable_files_path = strdup_new(sdata.writableFilesPath);
      m_globalData->m_readonly_files_path = strdup_new(sdata.readOnlyFilesPath);

      // Save things we do take ownership of.
      m_errorTable = sdata.errorTable;
      m_audioSyntax = sdata.audioSyntax;
   }

   Nav2::~Nav2(){
      coretracefunc();
      
      Halt();
      WaitNavigate2();

      coreprintln("Halt and WaitNavigate2 completed..");
      
      delete[] m_globalData->m_writable_files_path;
      delete[] m_globalData->m_readonly_files_path;
      delete m_errorTable;
      delete mainlog;
      delete m_globalData->m_logmaster;
      delete m_globalData;
#ifdef __SYMBIAN32__
      delete m_archGlobalData->m_timerThread;
      if(m_archGlobalData->m_socketServ){
         m_archGlobalData->m_socketServ->Close();
         delete m_archGlobalData->m_socketServ;
         m_archGlobalData->m_socketServ=NULL;
      }
      delete m_archGlobalData;
      deleteTlsGlobalData(m_tlsGlobalData, m_tlsGlobalDataId);
#endif
      
      //delete m_audioSyntax;   - owned by AudioCtrl...
   }


   void Nav2::joinModule(Module* module, const char* name,
                         long millis, int nanos)
   {
      coretracefunc();
      
      bool doDebug = true;
#ifdef NO_LOG_OUTPUT
      doDebug = false;
#endif
      coreprintln("Before join(%d ms): %s is %salive", millis, name, module->isAlive()?"":"not ");

      int js = module->join(millis,nanos);

      switch(js){
      case Thread::PROPER_JOIN:
         coreprintln_info("Properly joined %s", name);
         break;
      case Thread::JOIN_FAIL:
         coreprintln_error("Failed to join %s", name);
         break;
      case Thread::TIME_OUT:
         coreprintln_error("Timed-out when joining %s", name);
         break;
      default:
         coreprintln_error("Returned with strange value %d when joining %s",
                           js, name);
      }
      coreprintln("After join: %s is %salive", name, module->isAlive()?"":"not ");
   }

   void Nav2::deleteModule(Module* module)
   {
     char* name = NULL; 
     name = strdup_new(module->getName());
     coreprintln("deleting %s", name);

     delete module;

     coreprintln("deleted %s", name);
     
     delete[] name;
   }


   void Nav2::CleanupNavigate2()
   {
      coretracefunc();
      ModuleIterator i;
      for (i = allModules.begin() ; i != allModules.end() ; ++i)
      {
         joinModule(*i, (*i)->getName(), 10000, 0);
      }
#ifdef __SYMBIAN32__
      int js = m_archGlobalData->m_timerThread->terminateAndJoin();
      if(mainlog){
         mainlog->debug("Join returned a fishy value: %d.", js);
      }  
#endif

      for (i = allModules.begin() ; i != allModules.end() ; ++i)
      {
         deleteModule(*i);
      }
      allModules.clear();

      deleteModule(hub);
      hub = NULL;
      delete mainlog;
      mainlog = NULL;
      initialized=0;
      shutdownstarted = 0;
   }

void Nav2::WaitNavigate2()
{
   coretracefunc();

   if (initialized) {
      bool doDebug = true;
#ifdef NO_LOG_OUTPUT
      doDebug = false;
#endif

      coreprintln("joining CtrlHub module..");
      
      joinModule(hub, "CtrlHub", -1, -1);

      coreprintln("CtrlHub module has joined");
      
      CleanupNavigate2();
      if (mainlog && doDebug) {
          mainlog->debug("Cleanup is now complete");
      }
#ifdef __SYMBIAN32__
      if (m_archGlobalData->m_socketServ) {
         m_archGlobalData->m_socketServ->Close();
         delete(m_archGlobalData->m_socketServ);
         m_archGlobalData->m_socketServ = NULL;
      }
#endif
   }
}

void Nav2::StartShutdownNavigate2()
{
   if(initialized && !shutdownstarted){
      shutdownstarted = 1;
      hub->initiateShutdown();
   }
}


#ifdef __SYMBIAN32__
   void Nav2::Panic(Thread* thread)
   {
      if (mainlog) {
         mainlog->debug("Thread panic: %s", thread->getName());
      }
      if(initialized && !shutdownstarted) {
         if (mainlog) {
            mainlog->debug("killing all threads");
         }
         shutdownstarted = 1;
         m_archGlobalData->m_timerThread->terminateAndJoin();
         for (ModuleIterator i = allModules.begin() ; i != allModules.end() ; ++i)
         {
            (*i)->Kill(1, thread);
         }
         hub->Kill(1, thread);
      } else if (initialized && shutdownstarted) {
         if (mainlog) {
            mainlog->debug("Shutdown already started");
         }
      }
   }
#endif


   class Nav2::Channel* Nav2::getBluetoothChannel()
   {
      if(m_bt){
         return new Channel(m_bt);
      }
      return NULL;
   }
   class Nav2::Channel* Nav2::getGuiChannel()
   {
      if(m_gui){
         return new Channel(m_gui);
      }
      return NULL;
   }
   
   Nav2::Channel::Channel(QueueSerial* qs) : m_qs(qs)
   {
   }

   bool Nav2::Channel::writeData(const uint8* data, int length)
   {
      return m_qs->write(data, length);
   }
   
   bool Nav2::Channel::readData(Buffer* buf)
   {
      return m_qs->read(buf);
   }

   bool Nav2::Channel::readData(Buffer* buf, Monitor* monitor)
   {
      return m_qs->read(buf, monitor);
   }


#ifdef __SYMBIAN32__
   void Nav2::Channel::armReader(TRequestStatus *reqStat)
   {
      m_qs->armReader(reqStat);
   } 

   void Nav2::Channel::cancelArm()
   {
      m_qs->cancelArm();
   }
#endif

   void Nav2::Channel::connect()
   {
      m_qs->connect();
   }

   void Nav2::Channel::disconnect()
   {
      m_qs->disconnect();
   }

   int Nav2::Channel::getPollInterval()
   {
      return m_qs->getPollInterval();
   }

   bool Nav2::Channel::empty()
   {
      return m_qs->empty();
   }
}
