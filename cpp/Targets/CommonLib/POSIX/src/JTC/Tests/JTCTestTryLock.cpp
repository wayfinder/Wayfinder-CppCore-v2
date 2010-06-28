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
UT_ADD_TEST_FILE(JTCTestTryLock);

class TrylockThread : public JTCThread
{
public:

    virtual void
    run()
    {
	assert(!m_mut.trylock());
    }

    JTCMutex m_mut;
};

class TrylockThread2 : public JTCThread
{
public:

    virtual void
    run()
    {
	assert(!m_mut.trylock());
    }

    JTCRecursiveMutex m_mut;
};

#ifndef HAVE_NO_EXPLICIT_TEMPLATES
template class JTCHandleT<TrylockThread>;
template class JTCHandleT<TrylockThread2>;
#else
#  ifdef HAVE_PRAGMA_DEFINE
#    pragma define(JTCHandleT<TrylockThread>)
#    pragma define(JTCHandleT<TrylockThread2>)
#  endif
#endif

UT_NEW_TEST(JTCTestTryLock)
{
    try
    {
	JTCInitialize boot_jtc;

	//
	// Under WIN32 mutexes are actually recursive
	//
#if !defined(WIN32)
	JTCMutex mut;
	mut.lock();
	assert(!mut.trylock());
	mut.unlock();
#endif

	JTCRecursiveMutex rm;
	rm.lock();
	assert(rm.trylock());
	rm.unlock();
	rm.unlock();

	JTCHandleT<TrylockThread> t = new TrylockThread();
	t -> m_mut.lock();
	t -> start();
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
	t -> m_mut.unlock();

	JTCHandleT<TrylockThread2> t2 = new TrylockThread2();
	t2 -> m_mut.lock();
	t2 -> start();
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
	t2 -> m_mut.unlock();
    }
    catch(const JTCException& e)
    {
       UT_FAIL(e.getMessage());
    }
}
