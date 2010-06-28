// **********************************************************************
//
// Copyright (c) 2001
// IONA Technologies, Inc.
// Waltham, MA, USA
//
// All Rights Reserved
//
// **********************************************************************

#ifndef JTC_COND_H
#define JTC_COND_H

#include "JTC/JTCTypes.h"

#if defined(WIN32) || defined(VXWORKS)
#include "JTC/JTCMutex.h"
#endif

//
// Forward declarations
//
class JTCRecursiveMutex;
class JTCMutex;

#if defined(WIN32) || defined(VXWORKS)
class CondImpl;
#endif

//
// This class acts like a pthreads condition variable. The difference
// is that this implementation works with the recursive mutex
// implementation in this library.
//
class JTCCond
{
public:

    JTCCond();
    ~JTCCond();

    //
    // Wait for the condition variable to be signaled.
    //
    void
    wait(
	JTCRecursiveMutex& mutex
    );

    //
    // Wait for the condition variable to be signaled.
    //
    void
    wait(
	JTCMutex& mutex
    );

    //
    // Wait for the condition variable to be signaled for timeout
    // msec. Returns false if wait() terminated due to a timeout.
    //
    bool
    wait(
	JTCRecursiveMutex& mutex,
	long timeout
    );

    //
    // Wait for the condition variable to be signaled for timeout
    // msec. Returns false if wait() terminated due to a timeout.
    //
    bool
    wait(
	JTCMutex& mutex,
	long timeout
    );

    //
    // Signal one waiting thread.
    //
    void
    signal();

    //
    // Signal all waiting threads.
    //
    void
    broadcast();

private:

    //
    // Actual implementation of wait for a recursive mutex.
    //
    bool
    wait_internal(
	JTCRecursiveMutex&,
	long timeout
    );

    //
    // Actual implementation of wait for a non-recursive mutex.
    //
    bool
    wait_internal(
	JTCMutex&,
	long timeout
    );

#if defined(HAVE_POSIX_THREADS) || defined(HAVE_DCE_THREADS) \
 || defined(HAVE_VXWORKS_THREADS)
    struct timespec
    calc_timeout(
	long timeout
    );
#endif

    //
    // Hide copy constructor and assignment operator.
    //
    JTCCond(
	const JTCCond&
    );
    void
    operator=(
	const JTCCond&
    );

#if defined(HAVE_POSIX_THREADS) || defined(HAVE_DCE_THREADS)
    pthread_cond_t m_cond; // Pthread condition variable.
#endif

#if defined(HAVE_WIN32_THREADS) || defined(HAVE_VXWORKS_THREADS)
    CondImpl* m_impl;
#endif
};

#endif
