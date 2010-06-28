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
UT_ADD_TEST_FILE(JTCUncaughtException);

class UESimpleThread : public JTCThread
{
public:

    UESimpleThread(
	const char* name = 0
    )
	: JTCThread(name)
	{
	}

   virtual ~UESimpleThread() {

   }
   
    UESimpleThread(
	JTCThreadGroupHandle& handle
    )
	    : JTCThread(handle)
    {
    }

    virtual void
    run()
    {
	throw JTCException("To test");
    }
};

class MyThreadGroup : public JTCThreadGroup
{
public:

    MyThreadGroup(
	const char* name
    )
	: JTCThreadGroup(name)
	{
	}

   virtual ~MyThreadGroup() {
   }
   
    virtual void
    uncaughtException(
	JTCThreadHandle,
	const JTCException& e)
    {
	// cerr << "Caught exception: " << e.getType()
	//      << ": " << e.getMessage() << endl;
    }

    virtual void
    uncaughtException(
	JTCThreadHandle
    )
    {
	// cerr << "Caught unknown exception" << endl;
    }
};

UT_NEW_TEST(JTCUncaughtException)
{   
   try
   {
      JTCInitialize boot_jtc;
      JTCThreadHandle t = new UESimpleThread;
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

      JTCThreadGroupHandle my_group = new MyThreadGroup("test");
   
      t = new UESimpleThread(my_group);
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
   }
   catch(const JTCException& e)
   {
      UT_FAIL(e.getMessage());
   }
}
