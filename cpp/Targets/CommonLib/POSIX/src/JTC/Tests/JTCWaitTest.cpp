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
#include "UnitTestFramework.h"

#ifdef HAVE_IOSTREAM
#  include <iostream>
#else
#  include <iostream.h>
#endif

#ifdef HAVE_STD_IOSTREAM
using namespace std;
#endif

UT_ADD_TEST_FILE(JTCWaitTest);

class WTThread : public JTCThread, public JTCMonitor
{
public:

    WTThread()
	: m_done(0) { }

    virtual void
    run()
    {
	JTCSynchronized sync(*this);
	// cout << "waiting" << endl;
	while (!m_done)
	{
	    try
	    {
		wait();
	    }
	    catch(JTCInterruptedException&)
	    {
		continue;
	    }
	    m_done = 1;
	}
	// cout << "run complete" << endl;
    }

private:

    int m_done;
};

#ifndef HAVE_NO_EXPLICIT_TEMPLATES
template class JTCHandleT<WTThread>;
#else
#  ifdef HAVE_PRAGMA_DEFINE
#    pragma define(JTCHandleT<WTThread>)
#  endif
#endif

UT_NEW_TEST(JTCWaitTest)
{
    try
    {
	JTCInitialize boot_jtc;

	JTCHandleT<WTThread> t = new WTThread;
	t -> start();
	JTCThread::sleep(500);
	// cout << "notify." << endl;
	{
	    JTCSynchronized guard(*t);
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

	t = new WTThread;
	t -> start();
	JTCThread::sleep(500);
	// cout << "notifyAll." << endl;
	{
	    JTCSynchronized guard(*t);
	    t -> notifyAll();
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
    }
    catch(const JTCException& e)
    {
       UT_FAIL(e.getMessage());
    }
}
