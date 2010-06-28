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

#ifdef HAVE_STD_IOSTREAM
using namespace std;
#endif

#include "UnitTestFramework.h"

UT_ADD_TEST_FILE(JTCReentrantMonitorTest);

class RMTReentrant : public JTCMonitor
{
public:

    void
    a()
    {
	JTCSynchronized sync(*this);
	b();
	// cout << "here I am, in a()" << endl;
    }

    void
    b()
    {
	JTCSynchronized sync(*this);
	// cout << "here I am, in b()" << endl;
    }
};

UT_NEW_TEST(JTCReentrantMonitorTest)
{
    try
    {
	JTCInitialize boot_jtc;

	RMTReentrant r;
	r.a();
    }
    catch(const JTCException& e)
    {
       UT_FAIL(e.getMessage());
    }
}

