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
#include "UnitTestFramework.h"

UT_ADD_TEST_FILE(JTCMonitorTest2);

using namespace std;

class MT2TestThread : public JTCThread,
                    public JTCMonitorT<JTCMutex>
{
public:
   MT2TestThread()
      : m_started(false)
      {
       
      }

   virtual void run() {
      //
      // Sleep before acquiring the mutex to allow the main thread
      // to enter the wait_start method.
      //
      JTCThread::sleep(1000);

      JTCSyncT<JTCMonitorT<JTCMutex> > sync(*this);
      m_started = true;
      notify();
      wait();
   }

   void
   wait_start() {
      JTCSyncT<JTCMonitorT<JTCMutex> > sync(*this);
      while(!m_started)
      {
         wait();
      }
   }

private:
   bool m_started;
};

class MT2WaitThread : public JTCThread
{
public:

    MT2WaitThread(JTCMonitorT<JTCMutex>& mon)
	: m_mon(mon)
    {
    }

    virtual void
    run()
    {
	JTCSyncT<JTCMonitorT<JTCMutex> > sync(m_mon);
	m_mon.wait();
    }

private:

    JTCMonitorT<JTCMutex>& m_mon;
};

#ifndef HAVE_NO_EXPLICIT_TEMPLATES
template class JTCHandleT<MT2TestThread>;
template class JTCMonitorT<JTCMutex>;
template class JTCHandleT<MT2WaitThread>;
#else
#  ifdef HAVE_PRAGMA_DEFINE
#    pragma define(JTCHandleT<MT2TestThread>)
#    pragma define(JTCHandleT<MT2WaitThread>)
#    pragma define(JTCMonitorT<JTCMutex>)
#  endif
#endif

UT_NEW_TEST(JTCMonitorTest2)
{
    JTCInitialize boot_jtc;

    try
    {
	//
	// TEST: Verify notify() or notifyAll() occur when wait() is
	// called.
	//

       JTCHandleT<MT2TestThread> t = new MT2TestThread;
	t -> start();

	//
	// Wait for the thread to start
	//
	t -> wait_start();

	//
	// Notify the thread. Note that the monitor cannot be notified
	// until the lock is obtained which should mean that the
	// monitor is already waiting.
	//
	{
	    JTCSyncT<JTCMonitorT<JTCMutex> > sync(*t);
	    t -> notify();
	}

	while (t -> isAlive())
	{
	    try
	    {
		t -> join();
	    }
	    catch(const JTCInterruptedException&)
	    {
	    }
	}

	//
	// TEST: Verify that one call to notify() will take one
	// thread (not two!)
	//
	JTCMonitorT<JTCMutex> mon;
	JTCHandleT<MT2WaitThread> t1 = new MT2WaitThread(mon);
	JTCHandleT<MT2WaitThread> t2 = new MT2WaitThread(mon);
	t1 -> start();
	t2 -> start();

	//
	// Wait until the threads enter wait().
	//
	JTCThread::sleep(1000);

	{
	    JTCSyncT<JTCMonitorT<JTCMutex> > sync(mon);
	    mon.notify();
	}

	//
	// Sleep a second.  Either t1 or t2 might can stop -- but not
	// both.
	//
	JTCThread::sleep(1000);
	UT_ASSERT((t1 -> isAlive() && !t2 -> isAlive()) ||
             (t2 -> isAlive() && !t1 -> isAlive()));

	{
	    JTCSyncT<JTCMonitorT<JTCMutex> > sync(mon);
	    mon.notify();
	}

	while (t1 -> isAlive() || t2 -> isAlive())
	{
	    try
	    {
		t1 -> join();
	    }
	    catch(const JTCInterruptedException&)
	    {
	    }
	}
	
	while (t2 -> isAlive())
	{
	    try
	    {
		t2 -> join();
	    }
	    catch(const JTCInterruptedException&)
	    {
	    }
	}

	//
	// TEST: Verify that two calls to notify() will take two
	// threads.
	//
	t1 = new MT2WaitThread(mon);
	t2 = new MT2WaitThread(mon);
	t1 -> start();
	t2 -> start();

	//
	// Wait until the threads enter wait().
	//
	JTCThread::sleep(1000);

	{
	    JTCSyncT<JTCMonitorT<JTCMutex> > sync(mon);
	    mon.notify();
	    mon.notify();
	}

	while (t1 -> isAlive())
	{
	    try
	    {
		t1 -> join();
	    }
	    catch(const JTCInterruptedException&)
	    {
	    }
	}
	while (t2 -> isAlive())
	{
	    try
	    {
		t2 -> join();
	    }
	    catch(const JTCInterruptedException&)
	    {
	    }
	}

	//
	// TEST: Verify that a call to notifyAll() will take two
	// threads.
	//
	t1 = new MT2WaitThread(mon);
	t2 = new MT2WaitThread(mon);
	t1 -> start();
	t2 -> start();

	//
	// Wait until the threads enter wait().
	//
	JTCThread::sleep(1000);

	{
	    JTCSyncT<JTCMonitorT<JTCMutex> > sync(mon);
	    mon.notifyAll();
	}

	while (t1 -> isAlive())
	{
	    try
	    {
		t1 -> join();
	    }
	    catch(const JTCInterruptedException&)
	    {
	    }
	}
	while (t2 -> isAlive())
	{
	    try
	    {
		t2 -> join();
	    }
	    catch(const JTCInterruptedException&)
	    {
	    }
	}
    }
    catch(const JTCException& e)
    {
       UT_FAIL(e.getMessage());
    }
}






