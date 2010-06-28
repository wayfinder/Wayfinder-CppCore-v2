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

#ifdef HAVE_STD_IOSTREAM
using namespace std;
#endif

#include "UnitTestFramework.h"
UT_ADD_TEST_FILE(JTCThreadStateTest);

class TSTStarter
{
public:

    TSTStarter()
	: m_started(false)
    {
    }

    void
    wait_for_start()
    {
	JTCSynchronized sync(m_mon);
	if (!m_started)
	{
	    m_mon.wait();
	}
	m_mon.notify();
    }

    void
    start()
    {
	JTCSynchronized sync(m_mon);
	m_started = true;
	m_mon.notify();

	m_mon.wait();
    }

private:

    JTCMonitor m_mon;
    bool m_started;
};

class TSTStopper
{
public:

    TSTStopper()
	: m_stopped(false)
    {
    }

    void waitForTerminate()
    {
	JTCSynchronized sync(m_mon);
	if (!m_stopped)
	{
	    m_mon.wait();
	}
    }

    void terminate()
    {
	JTCSynchronized sync(m_mon);
	m_stopped = true;
	m_mon.notify();
    }

private:

    JTCMonitor m_mon;
    bool m_stopped;
};

class TSTThread : public JTCThread
{
public:

    TSTThread(
	TSTStarter& start,
	TSTStopper& stop
    )
	: m_stop(stop), m_start(start)
	{
    }

    virtual void
    run()
    {
	m_start.wait_for_start();
	m_stop.waitForTerminate();
    }

private:

    TSTStopper& m_stop;
    TSTStarter& m_start;
};

UT_NEW_TEST(JTCThreadStateTest)
{
    try
    {
	JTCInitialize boot_jtc;
	UT_ASSERT(JTCThread::currentThread() -> isAlive());

	TSTStarter starter;
	TSTStopper stop;
	JTCThreadHandle tt = new TSTThread(starter, stop);;

	UT_ASSERT(!tt -> isAlive());

	tt -> start();

	UT_ASSERT(tt -> isAlive());

	starter.start();

	UT_ASSERT(tt -> isAlive());

	stop.terminate();
	bool complete = false;
	do
	{
	    try
	    {
		tt -> join();
		complete = true;
	    }
	    catch(const JTCInterruptedException&)
	    {
	    }
	}
	while (!complete);

	UT_ASSERT(!tt -> isAlive());
    }
    catch(const JTCException& e)
    {
       UT_FAIL(e.getMessage());
    }
}
