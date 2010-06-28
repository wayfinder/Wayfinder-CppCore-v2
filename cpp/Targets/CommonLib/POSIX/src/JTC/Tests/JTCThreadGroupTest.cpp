// **********************************************************************
//
// Copyright (c) 2001
// IONA Technologies, Inc.
// Waltham, MA, USA
//
// All Rights Reserved
//
// **********************************************************************

#include "JTC/JTC.h"

#include <stdlib.h>
#include <assert.h>
#include <string.h>

#ifdef HAVE_IOSTREAM
#  include <iostream>
#else
#  include <iostream.h>
#endif

#ifdef HAVE_STD_IOSTREAM
using namespace std;
#endif

#include "UnitTestFramework.h"
//UT_ADD_TEST_FILE(JTCThreadGroupTest);

class TGTInfiniteThread : public JTCThread
{
public:

   TGTInfiniteThread(
      JTCThreadGroupHandle& group,
      const char* name
      )
      : JTCThread(group, name),
        m_stopped(false)
      {
      }

   void
   stop()
      {
         m_stopped = true;
      }

   virtual void
   run()
      {
         while (!m_stopped)
         {
            try
            {
               JTCThread::sleep(100);
            }
            catch(const JTCInterruptedException&)
            {
            }
         }
      }

private:

   bool m_stopped;
};

#ifndef HAVE_NO_EXPLICIT_TEMPLATES
template class JTCHandleT<TGTInfiniteThread>;
#else
#  ifdef HAVE_PRAGMA_DEFINE
#    pragma define(JTCHandleT<TGTInfiniteThread>)
#  endif
#endif

typedef JTCHandleT<TGTInfiniteThread> TGTInfiniteThreadHandle;

// UT_NEW_TEST(JTCThreadGroupTest)
// {
//    try
//    {
//       JTCInitialize boot_jtc;

//       const char* groupName = "island";
//       JTCThreadGroupHandle island = new JTCThreadGroup(groupName);
//       island -> setDaemon(true);

//       assert(strcmp(island -> getName(), groupName) == 0);
//       assert(island -> getParent() ==
//              JTCThread::currentThread() -> getThreadGroup());
//       assert(island -> getMaxPriority() ==
//              JTCThread::currentThread() -> getThreadGroup() ->
//              getMaxPriority());
//       assert(!island -> isDestroyed());

//       island -> setMaxPriority(JTCThread::JTC_NORM_PRIORITY);
//       assert(island -> getMaxPriority() == JTCThread::JTC_NORM_PRIORITY);
//       assert(JTCThread::currentThread() -> getThreadGroup() -> 
//              parentOf(island));
//       assert(!island -> parentOf(JTCThread::currentThread() -> 
//                                  getThreadGroup()));

//       assert(island -> activeGroupCount() == 0);

//       // printf("JTCThread::currentThread() -> getThreadGroup() -> "
//       //        "activeGroupCount() = %d",
//       //        JTCThread::currentThread() -> getThreadGroup() -> 
//       //        activeGroupCount());
      
//       // assert(JTCThread::currentThread() -> getThreadGroup() -> 
//       //        activeGroupCount() == 1);

//       TGTInfiniteThreadHandle t1 = new TGTInfiniteThread(island, "Jamaica");

//       TGTInfiniteThreadHandle t2 = new TGTInfiniteThread(island, "Fiji");

//       t1 -> start();
//       t2 -> start();
//       JTCThreadHandle list[10];
//       int len = island -> enumerate(list, sizeof(list));
//       assert(len == 2);
//       assert(list[0] != list[1]);
//       assert((list[0] == t1 || list[1] == t1)&&
//              (list[0] == t2 || list[1] == t2));

//       len = JTCThread::currentThread() -> getThreadGroup() -> 
// 			enumerate(list, sizeof(list));

//       assert(len == 3);
//       assert(list[0] != list[1] && list[1] != list[2] && list[0] != list[2]);
//       JTCThreadHandle mt = JTCThread::currentThread();
//       assert((list[0] == t1 || list[1] == t1 || list[2] == t1) &&
//              (list[0] == t2 || list[1] == t2 || list[2] == t2) &&
//              (list[0] == mt || list[1] == mt || list[2] == mt));

//       assert(island -> activeCount() == 2);
//       assert(JTCThread::currentThread() -> getThreadGroup() -> activeCount()
//              == 3);
// #ifndef HAVE_JTC_NO_IOSTREAM
//       // JTCThread::currentThread() -> getThreadGroup() -> list();
// #endif // !HAVE_JTC_NO_IOSTREAM

//       t1 -> stop();
//       do
//       {
//          try
//          {
//             t1 -> join();
//          }
//          catch(const JTCInterruptedException&)
//          {
//          }
//       }
//       while (t1 -> isAlive());

//       assert(island -> activeCount() == 1);
//       len = island -> enumerate(list, sizeof(list));
//       assert(len == 1);
//       assert(list[0] == t2);

//       t2 -> stop();
//       do
//       {
//          try
//          {
//             t2 -> join();
//          }
//          catch(const JTCInterruptedException&)
//          {
//          }
//       }
//       while (t2 -> isAlive());

//       assert(island -> activeCount() == 0);
//       len = island -> enumerate(list, sizeof(list));
//       assert(len == 0);

//       assert(island -> isDestroyed());

// #ifndef HAVE_JTC_NO_IOSTREAM
//       // JTCThread::currentThread() -> getThreadGroup() -> list();
// #endif // !HAVE_JTC_NO_IOSTREAM

//       JTCThreadGroupHandle dontDestroy = new JTCThreadGroup("dontDestroy");

//       t1 = new TGTInfiniteThread(dontDestroy, "test1");
//       t1 -> start();
        
//       t1 -> stop();
//       do
//       {
//          try
//          {
//             t1 -> join();
//          }
//          catch(const JTCInterruptedException&)
//          {
//          }
//       }
//       while (t1 -> isAlive());

//       assert(!dontDestroy -> isDestroyed());
//    }
//    catch(const JTCException& e)
//    {
//       UT_FAIL(e.getMessage());
//    }
// }
