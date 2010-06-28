// **********************************************************************
//
// Copyright (c) 2001
// IONA Technologies, Inc.
// Waltham, MA, USA
//
// All Rights Reserved
//
// **********************************************************************

#include "JTC/JTCCond.h"
#include "JTC/JTCMutex.h"
#include "JTC/JTCSyncT.h"

#include <errno.h>
#include <assert.h>

#if defined(VXWORKS)
#define TICKS_PER_MS 60000
#endif

#if defined(HAVE_POSIX_THREADS)
#   include <unistd.h>
#   include <sys/time.h>
#endif

#ifndef HAVE_JTC_NO_IOSTREAM
#   ifdef HAVE_STD_IOSTREAM
using namespace std;
#   endif
#endif // !HAVE_JTC_NO_IOSTREAM


#if defined(HAVE_POSIX_THREADS) || defined(HAVE_DCE_THREADS)

//
// This helper class is used to re-acquire the the recursive mutex.
// It's in place to help with robustness in the event of
// pthread_cancel.
//
class JTCCondHelper
{
    JTCRecursiveMutex& m_mutex;
    int m_count;

public:

    JTCCondHelper(
	JTCRecursiveMutex& mutex,
	int count
    )
	: m_mutex(mutex),
	  m_count(count)
    {
    }

    ~JTCCondHelper()
    {
	//       
	// We need to acquire the mutex rationally... We unlock the
	// mutexes critical section, and lock the mutex.  Restore
	// saved information. NOTE: It's not correct to set the count
	// and the owner of the mutex, the lock API method MUST be
	// called.
	//
	pthread_mutex_unlock(&m_mutex.m_lock);
	m_mutex.lock(m_count);
    }
};
#endif

#if defined(WIN32) || defined(VXWORKS)

class JTCSemaphore
{
public:

    JTCSemaphore(long initial_count = 0);
    ~JTCSemaphore();

    //
    // Wait for the semaphore to be signaled for timeout msec. A
    // negative number means wait indefinitely. Return true if wait
    // terminated successfully (that is no timeout).
    //
    bool
    wait(
	long timeout = -1
    );

    //
    // Increment the semaphore count times.
    //
    void
    post(
	int count = 1
    );

private:

#if defined(HAVE_WIN32_THREADS)
    HANDLE m_sem; // The semaphore handle
#endif
#if defined(HAVE_VXWORKS_THREADS)
    SEM_ID m_sem; // The semaphore handle    
#endif
};

// ----------------------------------------------------------------------
// JTCSemaphore constructor and destructor
// ----------------------------------------------------------------------

JTCSemaphore::JTCSemaphore(long initial_count)
{
#if defined(HAVE_WIN32_THREADS)
    JTC_SYSCALL_4(m_sem = CreateSemaphore, 0, initial_count, 0x7fffffff, 0,
		  == INVALID_HANDLE_VALUE)
#endif
#if defined(HAVE_VXWORKS_THREADS)
    m_sem = semCCreate(SEM_Q_PRIORITY, initial_count);
    if (m_sem == NULL) // TODO
        throw JTCOutOfMemoryError();
#endif
}

JTCSemaphore::~JTCSemaphore()
{
#if defined(HAVE_WIN32_THREADS)
    CloseHandle(m_sem);
#endif
#if defined(HAVE_VXWORKS_THREADS)
    semTake(m_sem, WAIT_FOREVER);
    semDelete(m_sem);   
#endif
}

// ----------------------------------------------------------------------
// JTCSemaphore public member implementation
// ----------------------------------------------------------------------

bool
JTCSemaphore::wait(long timeout)
{
#if defined(HAVE_WIN32_THREADS)
    if (timeout < 0)
    {
	timeout = INFINITE;
    }
    int rc;
    JTC_SYSCALL_2(rc = WaitForSingleObject, m_sem, timeout, == WAIT_ABANDONED);
    return rc != WAIT_TIMEOUT;
#endif
#if defined(HAVE_VXWORKS_THREADS)
    if (timeout < 0)
    {
	timeout = WAIT_FOREVER;
    }
    int rc;
    rc = semTake(m_sem, timeout * TICKS_PER_MS); // TODO
    return rc != ERROR;
#endif
}

void
JTCSemaphore::post(int count)
{
#if defined(HAVE_WIN32_THREADS)
    JTC_SYSCALL_3(ReleaseSemaphore, m_sem, count, 0, == 0)
#endif
#if defined(HAVE_VXWORKS_THREADS)
    while (0 < count--)
        semGive(m_sem);
#endif
}

//
// The condition variable implementation for WIN32. The WIN32
// implementation of broadcast wakes one waiting thread. Each woken
// thread then signals the next waiting thread to wake. This approach
// hopefully should cause the least number of context switches
// assuming a short wait(), command, wait() cycle.
//
class CondImpl
{
public:

    CondImpl()
	: m_gate(1),
	  m_blocked(0),
	  m_unblocked(0),
	  m_to_unblock(0)
    {
    }

    void
    signal(
	bool broadcast
    )
    {
	//
	// Acquire the gate semaphore and the internal mutex. Once
	// these have been acquired no further threads can enter
	// signal() or wait().
	//
	m_gate.wait();
	m_internal.lock();

	//
	// Adjust the number of blocked waiters. There must be a
	// separation of unblocked vs. blocked otherwise signal()
	// cannot determine if there are pending signals which have
	// not yet been processed.
	//
	if (m_unblocked != 0)
	{
	    m_blocked -= m_unblocked;
	    m_unblocked = 0;
	}

	//
	// If there are blocked waiters then calculate the number to
	// unblock, unlock the internal mutex and wake the first
	// blocked thread.
	//
	if (m_blocked > 0)
	{
	    m_to_unblock = (broadcast) ? m_blocked : 1;
	    m_internal.unlock();
	    m_queue.post();
	}
	//
	// Otherwise no blocked waiters so release the gate and the
	// internal mutex.
	//
	else
	{
	    m_gate.post();
	    m_internal.unlock();
	}
    }

    void
    pre_wait()
    {
	//
	// Called prior to wait(). Acquire the gate semaphore,
	// increment the number of blocked threads and release the
	// gate.
	//
	m_gate.wait();
	m_blocked++;
	m_gate.post();
    }

    bool
    wait(
	long timeout
    )
    {
	//
	// Wait for the queue semaphore to be signaled.
	//
	try
	{
	    bool rc = m_queue.wait(timeout);
	    postwait(!rc);
	    return rc;
	}
	catch(...)
	{
	    postwait(false);
	    throw;
	}
    }

private:
    
    void
    postwait(
	bool timeout
    )
    {
	//
	// Acquire the internal mutex, there is one more unblocked
	// thread.
	//
	m_internal.lock();
	m_unblocked++;

	//
	// Determine the action, release the internal mutex. If there
	// are no threads to unblock wait() on the queue returned as a
	// result of a timeout.
	//
	if (m_to_unblock != 0)
	{
	    bool last = (--m_to_unblock == 0);
	    m_internal.unlock();

	    //
	    // If the wait() terminated as a consequence of a timeout
	    // then consume one of the wakeups.
	    //
	    if (timeout)
	    {
		m_queue.wait();
	    }
	    
	    //
	    // Otherwise, if there are more threads to unblock then
	    // signal the queue, otherwise signal the gate.
	    //
	    if (last)
	    {
		m_gate.post();
	    }
	    else
	    {
		m_queue.post();
	    }
	}
	else
	{
	    m_internal.unlock();
	}
    }

    //
    // Binary semaphore which protects the blocked count. This
    // semaphore also protects wait() from being re-entered when a
    // broadcast is pending. This must be a sempahore and not a mutex
    // since the lock() and unlock() threads may not be the same.
    //
    JTCSemaphore m_gate;

    //
    // Semaphore upon which waiting threads are queued
    //
    JTCSemaphore m_queue;

    //
    // Internal mutex
    //
    JTCMutex m_internal;

    //
    // The number of blocked waiters.
    //
    long m_blocked;

    //
    // The number of waiters that have been unblocked.
    //
    long m_unblocked;

    //
    // The number of waiters to be unblocked.
    //
    long m_to_unblock;
};
#endif


// ----------------------------------------------------------------------
// JTCCond constructor and destructor
// ----------------------------------------------------------------------

JTCCond::JTCCond()
{
#if defined(HAVE_POSIX_THREADS)
    JTC_SYSCALL_2(pthread_cond_init, &m_cond, 0, != 0)
#endif

#if defined(HAVE_DCE_THREADS)
    JTC_SYSCALL_2(pthread_cond_init, &m_cond, pthread_condattr_default, != 0)
#endif

#if defined(HAVE_WIN32_THREADS) || defined(HAVE_VXWORKS_THREADS)
    m_impl = new CondImpl;
#endif
}

JTCCond::~JTCCond()
{
#if defined(HAVE_POSIX_THREADS) || defined(HAVE_DCE_THREADS)
    pthread_cond_destroy(&m_cond);
#endif

#if defined(HAVE_WIN32_THREADS) || defined(HAVE_VXWORKS_THREADS)
    delete m_impl;
#endif
}

// ----------------------------------------------------------------------
// JTCMonitor public member implementation
// ----------------------------------------------------------------------

//
// Wait to be signaled.
//
void
JTCCond::wait(
    JTCRecursiveMutex& mutex
)
{
    wait_internal(mutex, -1);
}

//
// Wait to be signaled.
//
void
JTCCond::wait(
    JTCMutex& mutex
)
{
    wait_internal(mutex, -1);
}

//
// Wait to be signaled for timeout milliseconds.
//
bool
JTCCond::wait(
    JTCRecursiveMutex& mutex,
    long timeout
)
{
    if (timeout < 0)
    {
	throw JTCIllegalArgumentException("timeout value is negative");
    }
    return wait_internal(mutex, timeout);
}


//
// Wait to be signaled for timeout milliseconds.
//
bool
JTCCond::wait(
    JTCMutex& mutex,
    long timeout
)
{
    if (timeout < 0)
    {
	throw JTCIllegalArgumentException("timeout value is negative");
    }
    return wait_internal(mutex, timeout);
}

//
// Wake one waiting thread.
//
void
JTCCond::signal()
{
#if defined(HAVE_POSIX_THREADS) || defined(HAVE_DCE_THREADS)
    JTC_SYSCALL_1(pthread_cond_signal, &m_cond, != 0)
#endif

#if defined(HAVE_WIN32_THREADS) || defined(HAVE_VXWORKS_THREADS)
    m_impl -> signal(false);
#endif
}

//
// Wake all waiting threads.
//
void
JTCCond::broadcast()
{
#if defined(HAVE_POSIX_THREADS) || defined(HAVE_DCE_THREADS)
    JTC_SYSCALL_1(pthread_cond_broadcast,&m_cond, != 0)
#endif

#if defined(HAVE_WIN32_THREADS) || defined(HAVE_VXWORKS_THREADS)
    m_impl -> signal(true);
#endif
}

// ----------------------------------------------------------------------
// JTCCond private member implementation
// ----------------------------------------------------------------------

//
// Wait for the condition variable to be signaled. A timeout value
// of -1 means wait forever. Otherwise, only wait for timeout
// milliseconds.
//
bool
JTCCond::wait_internal(
    JTCRecursiveMutex& mutex,
    long timeout
)
{
#if defined(HAVE_POSIX_THREADS) || defined(HAVE_DCE_THREADS)
    try
    {
	unsigned int count = mutex.reset_for_condvar();
	JTCCondHelper unlock(mutex, count);

	if (timeout < 0)
	{
	    JTC_SYSCALL_2(pthread_cond_wait, &m_cond, &mutex.m_lock, != 0);
	}
	else
	{
	    //
	    // Calculate the timeout period.
	    //
	    struct timespec abstime = calc_timeout(timeout);
	    JTC_SYSCALL_3(pthread_cond_timedwait, &m_cond, &mutex.m_lock,
			  &abstime, != 0);
	}
    }
    catch(const JTCSystemCallException& e)
    {
	//
	// EINTR is translated to InterruptedException.
	//
	if (e.getError() == EINTR)
	    throw JTCInterruptedException();
	//
	// EAGAIN means that the wait time has passed.
	//
	else if (e.getError() == EAGAIN ||
		 (e.getError() == ETIMEDOUT && timeout >= 0))
	    return false;
	
	//
	// Rethrow caught exception.
	//
	throw;
    }

    return true;

#endif

#if defined(HAVE_WIN32_THREADS) || defined(HAVE_VXWORKS_THREADS)

    m_impl -> pre_wait();

    unsigned int count = mutex.reset_for_condvar();

    try
    {
	bool rc = m_impl -> wait(timeout);
	mutex.lock(count);
	return rc;
    }
    catch(...)
    {
	mutex.lock(count);
	throw;
    }
#endif
}

//
// Wait for the condition variable to be signaled. A timeout value
// of -1 means wait forever. Otherwise, only wait for timeout
// milliseconds.
//
bool
JTCCond::wait_internal(
    JTCMutex& mutex,
    long timeout
)
{
#if defined(HAVE_POSIX_THREADS) || defined(HAVE_DCE_THREADS)
    
    try
    {
	if (timeout < 0)
	{
	    JTC_SYSCALL_2(pthread_cond_wait, &m_cond, &mutex.m_lock, != 0);
	}
	else
	{
	    //
	    // Calculate the timeout period.
	    //
	    struct timespec abstime = calc_timeout(timeout);
	    JTC_SYSCALL_3(pthread_cond_timedwait, &m_cond, &mutex.m_lock,
			  &abstime, != 0);
	}
    }
    catch(const JTCSystemCallException& e)
    {
	//
	// EINTR is translated to InterruptedException.
	//
	if (e.getError() == EINTR)
	    throw JTCInterruptedException();
	//
	// EAGAIN means that the wait time has passed.
	//
	else if (e.getError() == EAGAIN ||
		 (e.getError() == ETIMEDOUT && timeout >= 0))
	    return false;
	
	//
	// Rethrow caught exception.
	//
	throw;
    }

    return true;
#endif

#if defined(HAVE_WIN32_THREADS) || defined(HAVE_VXWORKS_THREADS)

    m_impl -> pre_wait();

    mutex.unlock();

    try
    {
	bool rc = m_impl -> wait(timeout);
	mutex.lock();
	return rc;
    }
    catch(...)
    {
	mutex.lock();
	throw;
    }
#endif
}

#if defined(HAVE_POSIX_THREADS) || defined(HAVE_DCE_THREADS)
struct timespec
JTCCond::calc_timeout(
    long timeout
)
{
    assert(timeout >= 0);
    struct timeval tv;
    gettimeofday(&tv, 0);
    //                       123456789 - 10^9
    const long oneBillion = 1000000000;

    struct timespec abstime;

    abstime.tv_sec = tv.tv_sec + (timeout/1000);
    abstime.tv_nsec = (tv.tv_usec * 1000) + ((timeout%1000) * 1000000);
    if (abstime.tv_nsec > oneBillion)
    {
	++abstime.tv_sec;
	abstime.tv_nsec -= oneBillion;
    }
    
    return abstime;
}
#endif
