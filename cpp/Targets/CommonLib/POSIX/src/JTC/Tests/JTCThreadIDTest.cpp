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
UT_ADD_TEST_FILE(JTCThreadIDTest);

class TIDTestThread : public JTCThread
{
public:

    TIDTestThread()
    {
    }
    
    virtual void
    run()
    {
    }
};

#ifndef HAVE_NO_EXPLICIT_TEMPLATES
template class JTCHandleT<TIDTestThread>;
#else
#  ifdef HAVE_PRAGMA_DEFINE
#    pragma define(JTCHandleT<TIDTestThread>)
#  endif
#endif

UT_NEW_TEST(JTCThreadIDTest)
{
    try
    {
	JTCInitialize boot_jtc;
	JTCThreadId null_id1;
	JTCThreadId null_id2;

	//
	// Test that null id's are equal.
	//
	assert(JTCThreadId() == JTCThreadId());
	assert(null_id1 == null_id2);
	assert(null_id1 == JTCThreadId());

	//
	// Test assignment, equality.
	//
	JTCThreadId this_thread_id = JTCThread::currentThread() -> getId();
	assert(this_thread_id != JTCThreadId());
	assert(this_thread_id == JTCThread::currentThread() -> getId());

	JTCHandleT<TIDTestThread> tt = new TIDTestThread;
	tt -> start();
	JTCThreadId tt_id = tt -> getId();

	assert(tt_id == tt -> getId());
	assert(tt_id != JTCThreadId());
	assert(tt_id != this_thread_id);
	tt -> join();
    }
    catch(const JTCException& e)
    {
       UT_FAIL(e.getMessage());
    }
}
