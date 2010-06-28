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

#ifdef HAVE_IOSTREAM
#  include <iostream>
#else
#  include <iostream.h>
#endif

#include "UnitTestFramework.h"

#ifdef HAVE_STD_IOSTREAM
using namespace std;
#endif

UT_ADD_TEST_FILE(JTCMonitorTest);

class MTTestThread : public JTCThread, public JTCMonitor
{
public:

    MTTestThread()
	: m_started(false)
    {
    }

    virtual void
    run()
    {
	//
	// Sleep before acquiring the mutex to allow the main thread
	// to enter the wait_start method.
	//
	JTCThread::sleep(1000);

	JTCSynchronized sync(*this);
	m_started = true;
	notify();
	wait();
    }

    void
    wait_start()
    {
	JTCSynchronized sync(*this);
	while (!m_started)
	{
	    wait();
	}
    }

private:

    bool m_started;
};

class MTWaitThread : public JTCThread
{
public:

    MTWaitThread(JTCMonitor& mon)
	: m_mon(mon)
    {
    }

    virtual void
    run()
    {
	JTCSynchronized sync(m_mon);
	m_mon.wait();
    }

private:

    JTCMonitor& m_mon;
};

class MTWaitThread2 : public JTCThread
{
public:

    MTWaitThread2(JTCMonitor& m, int count)
	: m_mon(m), m_count(count)
    {
    }

    virtual void
    run()
    {
	JTCSynchronized sync(m_mon);
	while (m_count > 0)
	{
	    m_mon.wait();
	}
    }

    //
    // Call with monitor locked
    //
    void
    consume()
    {
	--m_count;
    }

private:

    JTCMonitor& m_mon;
    int m_count;
};

#ifndef HAVE_NO_EXPLICIT_TEMPLATES
template class JTCHandleT<MTTestThread>;
template class JTCHandleT<MTWaitThread>;
template class JTCHandleT<MTWaitThread2>;
#else
#  ifdef HAVE_PRAGMA_DEFINE
#    pragma define(JTCHandleT<MTTestThread>)
#    pragma define(JTCHandleT<MTWaitThread>)
#    pragma define(JTCHandleT<MTWaitThread2>)
#  endif
#endif


UT_NEW_TEST(JTCMonitorTest)
{
    JTCInitialize boot_jtc;

    try
    {
	//
	// TEST: Verify notify() or notifyAll() occur when wait() is
	// called.
	//
	JTCHandleT<MTTestThread> t = new MTTestThread;
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
	    JTCSynchronized sync(*t);
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
	JTCMonitor mon;
	JTCHandleT<MTWaitThread> t1 = new MTWaitThread(mon);
	JTCHandleT<MTWaitThread> t2 = new MTWaitThread(mon);
	t1 -> start();
	t2 -> start();

	//
	// Wait until the threads enter wait().
	//
	JTCThread::sleep(1000);

	{
	    JTCSynchronized sync(mon);
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
	    JTCSynchronized sync(mon);
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
	t1 = new MTWaitThread(mon);
	t2 = new MTWaitThread(mon);
	t1 -> start();
	t2 -> start();

	//
	// Wait until the threads enter wait().
	//
	JTCThread::sleep(1000);

	{
	    JTCSynchronized sync(mon);
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
	t1 = new MTWaitThread(mon);
	t2 = new MTWaitThread(mon);
	t1 -> start();
	t2 -> start();

	//
	// Wait until the threads enter wait().
	//
	JTCThread::sleep(1000);

	{
	    JTCSynchronized sync(mon);
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

	//
	// TEST: Verify that a call to notifyAll() actually wakes both
	// threads.
	//
	int count = 1000;
	JTCHandleT<MTWaitThread2> wt1 = new MTWaitThread2(mon, count);
	JTCHandleT<MTWaitThread2> wt2 = new MTWaitThread2(mon, count);
	wt1 -> start();
	wt2 -> start();

	//
	// Wait until the threads enter wait().
	//
	JTCThread::sleep(1000);

	while (count > 0)
	{
	    JTCSynchronized sync(mon);
	    wt1 -> consume();
	    wt2 -> consume();
	    mon.notifyAll();
	    --count;
	}
	while (wt1 -> isAlive())
	{
	    try
	    {
		wt1 -> join();
	    }
	    catch(const JTCInterruptedException&)
	    {
	    }
	}
	while (wt2 -> isAlive())
	{
	    try
	    {
		wt2 -> join();
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
