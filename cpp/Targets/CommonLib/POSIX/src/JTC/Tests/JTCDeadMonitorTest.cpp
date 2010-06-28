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

#ifdef HAVE_IOSTREAM
#  include <iostream>
#else
#  include <iostream.h>
#endif

#include "UnitTestFramework.h"

#ifdef HAVE_STD_IOSTREAM
using namespace std;
#endif

UT_ADD_TEST_FILE(JTCDeadMonitorTest);

class DMTBaseClass : public JTCMonitor
{
public:

    virtual
    ~DMTBaseClass()
    {

    }
};

class DMTTestThread : public DMTBaseClass, public JTCThread
{
public:

    DMTTestThread()
    {
    }

    virtual
    ~DMTTestThread()
    {
	// cout << "DMTTestThread" << endl;
    }

    virtual void
    run()
    {
	// cout << "Done." << endl;
    }
};

UT_NEW_TEST(JTCDeadMonitorTest)
{
    try
    {
	JTCInitialize boot_jtc;

	JTCThreadHandle t = new DMTTestThread();
	t -> start();
    }
    catch(const JTCException& e)
    {
       UT_FAIL(e.getMessage());
    }
}
