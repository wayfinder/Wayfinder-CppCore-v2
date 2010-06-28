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
UT_ADD_TEST_FILE(JTCStaticMonitorTest);

#ifdef HAVE_STD_IOSTREAM
using namespace std;
#endif

class SMTGlobal : public JTCMonitor
{
public:

    static void
    increment()
    {
	JTCSynchronized sync(m_mon);
	++m_counter;
    }

    static void
    decrement()
    {
	JTCSynchronized sync(m_mon);
	--m_counter;
    }

    static int
    value()
    {
	JTCSynchronized sync(m_mon);
	return m_counter;
    }

private:

    static int m_counter;
    static JTCMonitor m_mon;
};

int SMTGlobal::m_counter = 0;
JTCMonitor SMTGlobal::m_mon;

class SMTCountThread : public JTCThread
{
public:
    SMTCountThread(
	bool countUp
    )
        : m_count_up(countUp)
    {
    }

    virtual void
    run()
    {
        int times = 100;
        while (--times > 0)
        {
            if (m_count_up)
                SMTGlobal::increment();
            else
                SMTGlobal::decrement();
            yield();
        }
    }

private:

    bool m_count_up;
};

UT_NEW_TEST(JTCStaticMonitorTest)
{
    try
    {
	JTCInitialize boot_jtc;
        JTCThreadHandle t[4];
        
        t[0] = new SMTCountThread(true);
        t[1] = new SMTCountThread(true);
        t[2] = new SMTCountThread(false);
        t[3] = new SMTCountThread(false);

        unsigned int i;
        for (i = 0; i < sizeof(t)/sizeof(t[0]); ++i)
        {
            t[i] -> start();
        }

        for (i = 0; i < sizeof(t)/sizeof(t[0]); ++i)
        {
            do
            {
                try
                {
                    t[i] -> join();
                }
                catch(const JTCInterruptedException&)
                {
                }
            }
            while (t[i] -> isAlive());
        }

        UT_ASSERT(SMTGlobal::value() == 0);
    }
    catch(const JTCException& e)
    {
       UT_FAIL(e.getMessage());
    }
}
