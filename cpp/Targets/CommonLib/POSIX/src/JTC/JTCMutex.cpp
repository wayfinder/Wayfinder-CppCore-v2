// **********************************************************************
//
// Copyright (c) 2001
// IONA Technologies, Inc.
// Waltham, MA, USA
//
// All Rights Reserved
//
// **********************************************************************

#include "JTC/JTCMutex.h"

#include <stdlib.h>
#include <assert.h>
#include <errno.h>

#ifndef HAVE_JTC_NO_IOSTREAM
#   ifdef HAVE_STD_IOSTREAM
using namespace std;
#   endif
#endif // !HAVE_JTC_NO_IOSTREAM

// ----------------------------------------------------------------------
// JTCRecursiveMutex constructor and destructor
// ----------------------------------------------------------------------

JTCRecursiveMutex::JTCRecursiveMutex()
    : m_count(0),
      m_owner(JTCThreadId())
{
#if defined(HAVE_POSIX_THREADS)
    JTC_SYSCALL_2(pthread_mutex_init, &m_lock, 0, != 0)
#endif

#if defined(HAVE_DCE_THREADS)
    JTC_SYSCALL_2(pthread_mutex_init, &m_lock, pthread_mutexattr_default, != 0)
#endif

#if defined(HAVE_WIN32_THREADS)
    InitializeCriticalSection(&m_lock);
#endif

#if defined(HAVE_VXWORKS_THREADS)
    m_lock = semMCreate(SEM_Q_PRIORITY |
                        SEM_DELETE_SAFE |
                        SEM_INVERSION_SAFE);
    if (m_lock == NULL) // TODO
        throw JTCOutOfMemoryError();
#endif
}

JTCRecursiveMutex::~JTCRecursiveMutex()
{
#if defined(HAVE_POSIX_THREADS) || defined(HAVE_DCE_THREADS)
    pthread_mutex_destroy(&m_lock);
#endif

#if defined(HAVE_WIN32_THREADS)
    DeleteCriticalSection(&m_lock);
#endif

#if defined(HAVE_VXWORKS_THREADS)
    semTake(m_lock, WAIT_FOREVER);
    semDelete(m_lock);
#endif
}

// ----------------------------------------------------------------------
// JTCRecursiveMutex public member implementation
// ----------------------------------------------------------------------

//
// Lock the mutex.
//
bool
JTCRecursiveMutex::lock() const
{
    //
    // Work around lack of mutable.
    //
    return ((JTCRecursiveMutex*)this) -> lock_internal(1);
}

//
// Unlock the mutex.
//
bool
JTCRecursiveMutex::unlock() const
{
    //
    // Work around lack of mutable.
    //
    return ((JTCRecursiveMutex*)this) -> unlock_internal();
}

//
// Unlock the mutex.
//
bool
JTCRecursiveMutex::trylock() const
{
    //
    // Work around lack of mutable.
    //
    return ((JTCRecursiveMutex*)this) -> trylock_internal();
}

//
// Return the ID of the owning thread. If the mutex isn't locked then
// return nullThreadId.
//
JTCThreadId
JTCRecursiveMutex::get_owner() const
{
    JTCRecursiveMutex* This = (JTCRecursiveMutex*)this;
    //
    // Aquire the internal mutex.
    //
    This -> m_internal.lock();
    JTCThreadId id;
    if (m_count > 0)
        id = m_owner;
    This -> m_internal.unlock();
    
    return id;
}

// ----------------------------------------------------------------------
// JTCRecursiveMutex private member implementation
// ----------------------------------------------------------------------

bool
JTCRecursiveMutex::lock_internal(
    int count
)
{
    bool rc = false;

    //
    // This flag is set to true once the mutex has been obtained.
    //
    bool obtained = false;
    while (!obtained)
    {
        //
        // Lock the internal mutex.
        //
	m_internal.lock();

        //
        // m_count represents the number of times the mutex has been
        // aquired.  If m_count is zero then the mutex is not yet
        // aquired.  Note that two mutexes cannot be in here at once,
        // since this CS is protected by the mutex m_internal.
        //
	if (m_count == 0)
	{
            //
            // We're attempting to aquire the mutex count more times.
            //
	    m_count = count;
	    m_owner = JTCThreadId::self();
	    obtained = true;
	    rc = true;

            //
            // Acquire the mutexes critical section.
            //
            try
	    {
#if defined(HAVE_POSIX_THREADS) || defined(HAVE_DCE_THREADS)
		JTC_SYSCALL_1(pthread_mutex_lock, &m_lock, != 0)
#endif

#if defined(HAVE_WIN32_THREADS)
		EnterCriticalSection(&m_lock);
#endif

#if defined(HAVE_VXWORKS_THREADS)
                semTake(m_lock, WAIT_FOREVER);
#endif
	    }
	    catch(...)
	    {
		try
		{
		    m_internal.unlock();
		}
		catch(...)
		{
		}
		throw;
	    }
	}
        //
        // Otherwise m_count is non-zero.  This means that the mutex is
        // currently aquired.  There are two cases here:
	//
        // - The mutex is owned by the caller. Simply increment the
        //   count in this case.
	//
        // - The mutex is owned by a thread other than the caller. In
        //   this case we attempt to aquire the mutexes critical
        //   section. This CS is unlocked once mutex is released m_count
        //   times.
        //
	else if (m_owner == JTCThreadId::self())
	{
	    m_count += count;
	    obtained = true;
	}
	m_internal.unlock();

        //
        // If we haven't yet obtained the mutex lock the critical
        // section mutex.  Immediately unlock the CS, then attempt to
        // re-obtain.
        //
	if (!obtained)
	{
#if defined(HAVE_POSIX_THREADS) || defined(HAVE_DCE_THREADS)
	    JTC_SYSCALL_1(pthread_mutex_lock, &m_lock, != 0)
	    pthread_mutex_unlock(&m_lock);
#endif

#if defined(HAVE_WIN32_THREADS)
	    EnterCriticalSection(&m_lock);
	    LeaveCriticalSection(&m_lock);
#endif

#if defined(HAVE_VXWORKS_THREADS)
            semTake(m_lock, WAIT_FOREVER);            
            semGive(m_lock);
#endif

	}
    }

    return rc;
}

bool
JTCRecursiveMutex::trylock_internal()
{
    //
    // This flag is set to true once the mutex has been obtained.
    //
    bool obtained = false;

    //
    // Lock the internal mutex.
    //
    m_internal.lock();
    
    //
    // m_count represents the number of times the mutex has been
    // aquired.  If m_count is zero then the mutex is not yet
    // aquired.  Note that two mutexes cannot be in here at once,
    // since this CS is protected by the mutex m_internal.
    //
    if (m_count == 0)
    {
	//
	// We're attempting to aquire the mutex count more times.
	//
	m_count = 1;
	m_owner = JTCThreadId::self();
	obtained = true;

	//
	// Acquire the mutexes critical section.
	//
	try
	{
#if defined(HAVE_POSIX_THREADS) || defined(HAVE_DCE_THREADS)
	    JTC_SYSCALL_1(pthread_mutex_lock, &m_lock, != 0)
#endif

#if defined(HAVE_WIN32_THREADS)
	    EnterCriticalSection(&m_lock);
#endif

#if defined(HAVE_VXWORKS_THREADS)
            semTake(m_lock, WAIT_FOREVER);
#endif
	}
	catch(...)
	{
	    try
	    {
		m_internal.unlock();
	    }
	    catch(...)
	    {
	    }
	    throw;
	}
    }
    //
    // Otherwise m_count is non-zero.  This means that the mutex is
    // currently aquired.  There are two cases here:
    //
    // - The mutex is owned by the caller. Simply increment the
    //   count in this case.
    //
    // - The mutex is owned by a thread other than the caller. In
    //   this case we attempt to aquire the mutexes critical
    //   section. This CS is unlocked once mutex is released m_count
    //   times.
    //
    else if (m_owner == JTCThreadId::self())
    {
	m_count++;
	obtained = true;
    }
    m_internal.unlock();

    return obtained;
}

bool
JTCRecursiveMutex::unlock_internal()
{
    bool rc;
    //
    // Aquire the internal mutex.
    //
    m_internal.lock();

    //
    // If m_count decrements to zero then unlock the mutexes CS. The
    // mutex is no longer owned by a thread.
    //
    if (--m_count == 0)
    {
	m_owner = JTCThreadId();

#if defined(HAVE_POSIX_THREADS) || defined(HAVE_DCE_THREADS)
	pthread_mutex_unlock(&m_lock);
#endif

#if defined(HAVE_WIN32_THREADS)
	LeaveCriticalSection(&m_lock);
#endif

#if defined(HAVE_VXWORKS_THREADS)
        semGive(m_lock);
#endif

	rc = true;
    }
    else
	rc = false;

    m_internal.unlock();

    return rc;
}

//
// Lock the mutex count times.
//
void
JTCRecursiveMutex::lock(
    unsigned int count
) const
{
    //
    // Work around lack of mutable.
    //
    ((JTCRecursiveMutex*)this) -> lock_internal(count);
}

unsigned int
JTCRecursiveMutex::reset_for_condvar()
{
    m_internal.lock();
    
    unsigned int count = m_count;
    m_count = 0;
    m_owner = JTCThreadId();

    m_internal.unlock();

#if defined(WIN32)
    //
    // Under NT the actual mutex has to be unlocked. For pthreads the
    // condvar implementation takes care of this.
    //
    LeaveCriticalSection(&m_lock);
#endif

#if defined(HAVE_VXWORKS_THREADS)
    semGive(m_lock);
#endif

    return count;
}
