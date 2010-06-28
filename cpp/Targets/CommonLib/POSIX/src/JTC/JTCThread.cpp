// **********************************************************************
//
// Copyright (c) 2001
// IONA Technologies, Inc.
// Waltham, MA, USA
//
// All Rights Reserved
//
// **********************************************************************

#include "JTC/JTCThread.h"
#include "JTC/JTCSyncT.h"
#include "JTC/JTCThreadGroup.h"
#include "JTC/JTCTSS.h"
#include "JTC/JTCRunnable.h"
#include "JTC/JTCHandleI.h"

#include "JTC/JTCTSSManager.h"

#include <string.h>
#include <stdlib.h>
#include <stdio.h>
#include <errno.h>
#include <assert.h>

#if defined(HAVE_POSIX_THREADS) || defined(HAVE_DCE_THREADS) || defined(VXWORKS)
#   ifdef HAVE_SCHED_H
#       include <sched.h>
#   else
#       ifdef HAVE_SYS_SCHED_H
#           include <sys/sched.h>
#       endif
#   endif
#   ifdef HAVE_TIME_H
#       include <time.h>
#   endif
#   ifdef HAVE_SYS_TIME_H
#           include <sys/time.h>
#   endif
#endif

#if defined(HAVE_POSIX_THREADS) || defined(HAVE_DCE_THREADS)
#   include <unistd.h>
#   include <sys/types.h>
#endif

#ifdef WIN32
#   include <sys/timeb.h>
#   include <process.h>
#endif

#ifdef HAVE_EXCEPTION
#   include <exception>
#endif

#ifndef HAVE_JTC_NO_IOSTREAM
#   ifdef HAVE_STD_IOSTREAM
using namespace std;
#   endif
#endif // !HAVE_JTC_NO_IOSTREAM

#if defined(HAVE_EXCEPTION_H)
#   include <exception.h>
#elif defined(HAVE_UNEXPECTED_H)
#   include <unexpected.h>
#endif

#ifdef HAVE_TERMINATE_H
#   include <terminate.h>
#endif

#ifndef HAVE_NO_EXPLICIT_TEMPLATES
template class JTCHandleT<JTCThread>;
#else
#   ifdef HAVE_PRAGMA_DEFINE
#       pragma define(JTCHandleT<JTCThread>)
#   endif
#endif

#ifndef HAVE_NO_EXPLICIT_TEMPLATES
template class JTCHandleT<JTCRunnable>;
#else
#   ifdef HAVE_PRAGMA_DEFINE
#       pragma define(JTCHandleT<JTCRunnable>)
#   endif
#endif

//
// Global "root" thread group
//
static JTCThreadGroup* lsd_root_thread_group = 0;

//
// First JTCInitialize object instance (used to initialize further
// JTCInitialize instances).
//
static JTCInitialize* lsd_initialize_instance = 0;

#ifdef VXWORKS
static size_t lsd_initial_stack_size = 81920;
#else
static size_t lsd_initial_stack_size = 0;
#endif

#if defined(__sgi) && !defined(__GNUC__)
static long
lsf_jtc_thread_id()
{
    return pthread_self();
}
#endif

//
// This solves the conflict that ::abort() has C binding but
// set_terminate() and set_unexpected() expect C++ binding.
//
#ifndef WIN32
void
jtc_abort()
{
    ::abort();
}
#endif

//
// This is used as a hook from the WIN32 threading library, and the
// POSIX threading library into JTC. It's better not to use a static
// member of the JTCThread class since they are not guaranteed to be C
// callable.
//
#ifndef WIN32
extern "C"
#else
static
#endif
void*
lsf_thread_adapter(
    void* arg
)
{
    //
    // Ensure that unexpected exceptions cause an abort.
    //
#ifdef HAVE_STD_SET_X
    std::set_terminate(jtc_abort);
    std::set_unexpected(jtc_abort);
#else
    set_terminate(::abort);
    set_unexpected(::abort);
#endif

#if defined(__sgi) && !defined(__GNUC__)
    //
    // SGI needs this to handle multiple exceptions in threads.
    //
    set_thread_id_function(lsf_jtc_thread_id);
#endif

    //
    // The arg is a pointer to the JTCThread class.
    //
    JTCThread* thr = (JTCThread*)arg;
    try
    {
	thr -> entrance_hook();
    }
    catch(...)
    {
	// Consume
    }

    try
    {
        //
        // Call the exit method of the thread.
        //
        thr -> exit_hook();
    }
    catch(...)
    {
	// Consume
    }

    //    
    //
    // No return value.
    //
    return 0;
}

// ----------------------------------------------------------------------
// JTCThread private member implementation
// ----------------------------------------------------------------------

//
// TSS key. This key is used to store a pointer to the JTCThread
// object.
//
JTCThreadKey JTCThread::sm_tss_key;

unsigned long JTCThread::sm_thread_number = 0;
JTCMutex JTCThread::sm_thread_number_mutex;

#if defined(HAVE_POSIX_THREADS) || defined(HAVE_DCE_THREADS)
JTCAttrHook JTCThread::sm_attr_hook = 0;
#endif

JTCRunHook JTCThread::sm_run_hook = 0;
JTCStartHook JTCThread::sm_start_hook = 0;

#ifndef HAVE_JTC_NO_IOSTREAM

//
// IOstream operator for pointer to void function taking a pointer
// to void.
//
ostream&
operator<<(
    ostream& os,
    void * (fn)(void *)
)
{
    os << (void*)fn;
    return os;
}

#endif // !HAVE_JTC_NO_IOSTREAM

#ifdef HAVE_POSIX_THREADS
#define PTHREAD_ATTR_INIT(a) pthread_attr_init(a)
#define PTHREAD_ATTR_DESTROY(a) pthread_attr_destroy(a)
#define PTHREAD_CREATE(a,b,c,d)  \
	JTC_SYSCALL_4(pthread_create, &a, &b, c, d,!= 0)
#endif
#ifdef HAVE_DCE_THREADS
#define PTHREAD_ATTR_INIT(a) pthread_attr_create(a)
#define PTHREAD_ATTR_DESTROY(a) pthread_attr_delete(a)
#define PTHREAD_CREATE(a,b,c,d)  \
	JTC_SYSCALL_4(pthread_create, &a, b, c, d,!= 0)
#endif

//
// Helper for all constructors but the default.
//
void
JTCThread::init(
    const JTCThreadGroupHandle& group,
    JTCRunnableHandle target,
    const char* name
)
{
    //
    // Set default values for the thread state flags.
    //
    m_name = 0;
    m_target = target;
    m_state = new_thread;
    m_thread_id = JTCThreadId();
#ifndef WIN32
    m_detached = false;
#endif
    m_thread_number = get_next_thread_number();
    ++m_refcount; // We're referencing outself.  Boost the reference count.
    m_is_adopted = false; // The thread was not adopted
#ifdef WIN32
    m_handle = 0; // Initialize the thread handle
#endif
    
    //
    // Group of 0 means that we inherit the thread group from the
    // creating thread.
    //
    m_group = group;
    if (m_group.get() == 0)
    {
	m_group = currentThread() -> getThreadGroup();
    }

    //
    // Set the threads name.
    //
    setName(name);

    //
    // Add this thread to the group.
    //
    m_group -> add(this);
}

unsigned long
JTCThread::get_next_thread_number()
{
    JTCSyncT<JTCMutex> sync(sm_thread_number_mutex);
    return sm_thread_number++;
}

// ----------------------------------------------------------------------
// JTCThread private constructor
// ----------------------------------------------------------------------

//
// This is called to initialize the JTC thread system, or to adopt a
// thread.
//
JTCThread::JTCThread(
    JTCThreadId id,
    bool main
)
    : m_name(0),
      m_is_adopted(!main)
{
    //
    // Initialize to default state. Remember that this thread doesn't
    // need to be spawned since it represents the main thread of the
    // application.
    //

    //
    // Set default values for the thread state variables.
    //
    m_thread_id = id;
#ifndef WIN32
    m_detached = true;
#endif
    m_thread_number = get_next_thread_number();
    m_state = JTCThread::runnable;
#ifdef WIN32
    m_handle = GetCurrentThread();
#endif

    //
    // Set the TSS data for this thread.
    //
    JTCTSS::set(sm_tss_key, this);
    setName(0);
    ++m_refcount; // We're referencing outself.  Boost the reference count.

    //
    // Create the default `system' thread group (for an adopted thread
    // a new root thread is created)
    //
    const char* group_name = (main) ? "system" : "adopted";
    m_group = new JTCThreadGroup(lsd_root_thread_group, group_name);
    m_group -> add(this);

    if (!m_is_adopted)
    {
	//
	// The main thread runs at normal priority. This may or may
	// not be the default for the OS. This must be done after the
	// system thread group has been created.
	//
	setPriority(JTC_NORM_PRIORITY);
    }
}

// ----------------------------------------------------------------------
// JTCThread constructor and destructor
// ----------------------------------------------------------------------

JTCThread::JTCThread(
    const char* name
)
{
    init(JTCThreadGroupHandle(0), JTCRunnableHandle(0), name);
}

JTCThread::JTCThread(
    JTCRunnableHandle target,
    const char* name
)
{
    init(JTCThreadGroupHandle(0), target, name);
}

JTCThread::JTCThread(
    JTCThreadGroupHandle& group,
    const char* name
)
{
    init(group, JTCRunnableHandle(0), name);
}

JTCThread::JTCThread(
    JTCThreadGroupHandle& group,
    JTCRunnableHandle target,
    const char* name
)
{
    init(group, target, name);
}

JTCThread::~JTCThread()
{
    try
    {
	delete[] m_name;
    }
    catch(...)
    {
    }
}

// ----------------------------------------------------------------------
// JTCThread public member implementation
// ----------------------------------------------------------------------

//
// Get a handle to this threads ThreadGroup.
//
JTCThreadGroupHandle
JTCThread::getThreadGroup()
{
    return m_group;
}

//
// Set the name of the current thread.
//
// Note: The ability to call this with a null pointer doesn't match the
// Java implementation.
//
void
JTCThread::setName(
    const char* name
)
{
    JTCSyncT<JTCMutex> guard(m_mutex);

    char* newName;
    //
    // 0 means use the default name of the thread. That is
    // Thread- get_next_thread_number()
    //
    if (name == 0)
    {
#ifndef HAVE_JTC_NO_IOSTREAM
#ifndef HAVE_SSTREAM
	char buf[1024];
        JTC_STD(ostrstream) os(buf, sizeof(buf));
        os << "Thread-" << m_thread_number << ends;
        newName = new char[strlen(buf) + 1];
        strcpy(newName, buf);
#else // HAVE_SSTREAM
	JTC_STD(ostringstream) os;
	os << "Thread-" << m_thread_number << ends;
        newName = new char[os.str().length() + 1];
        strcpy(newName, os.str().c_str());
#endif // HAVE_SSTREAM
#else // HAVE_JTC_NO_IOSTREAM
	char buf[1024];
	sprintf(buf, "Thread-%ul", m_thread_number);
        newName = new char[strlen(buf) + 1];
        strcpy(newName, buf);
#endif // !HAVE_JTC_NO_IOSTREAM
    }
    else
    {
        newName = new char[strlen(name) + 1];
        strcpy(newName, name);
    } 
    delete[] m_name;
    m_name = newName;
}

//
// Return the name of the thread.
//
const char*
JTCThread::getName(
) const
{
    JTCSyncT<JTCMutex> guard(m_mutex);
    return m_name;
}

static int
lsd_priority_map[] =
{
#if defined(HAVE_DCE_THREADS)

    PRI_OTHER_MIN,
    ( PRI_OTHER_MIN + PRI_OTHER_MAX + 1 ) / 2,
    PRI_OTHER_MAX

#elif defined(HAVE_WIN32_THREADS)

    THREAD_PRIORITY_LOWEST,
    THREAD_PRIORITY_NORMAL,
    THREAD_PRIORITY_HIGHEST

#else // defined(HAVE_POSIX_THREADS)
#   if defined(__sun) && defined(__SVR4)
    //
    // Solaris.
    //
    5,
    3,
    1
#   elif defined(__linux__) || defined(IPHONE)
    //
    // Linux has no priorities under LinuxThreads.
    //
    0,
    0,
    0
#   elif defined(__sgi)
   -20,
    0,
    20
#   elif defined(_AIX)
    //
    // AIX doesn't appear to have priorities either.
    //
    //PTHREAD_PRIO_MIN,
    //( PTHREAD_PRIO_MIN + PTHREAD_PRIO_MAX ) / 2,
    //PTHREAD_PRIO_MAX
    1,
    1,
    1
#   elif defined(__osf__)
    PRI_OTHER_MIN,
    ( PRI_OTHER_MIN + PRI_OTHER_MAX + 1 ) / 2,
    PRI_OTHER_MAX
#   elif defined(__hpux) && defined(HAVE_POSIX_THREADS)
    //
    // No thread priorities under HPUX 11.x.
    //
    0,
    0,
    0
#   elif defined(__QNX__)
    //
    // QNX/Neutrino
    //
    15,
    10,
    5
#   elif defined(VXWORKS)
    255,
    127,
    0
#   else
#       error "No known priority levels!"
#   endif
#endif
};

//
// Set the threads priority.
//
void
JTCThread::setPriority(
    int new_priority
)
{
    JTCSyncT<JTCMutex> guard(m_mutex);

    if (new_priority < JTC_MIN_PRIORITY || new_priority > JTC_MAX_PRIORITY)
    {
	throw JTCIllegalArgumentException("priority out of range");
    }
    if (new_priority > m_group -> getMaxPriority())
    {
	new_priority = m_group -> getMaxPriority();
    }
#ifdef HAVE_POSIX_THREADS
#   ifndef __hpux
    sched_param param;
    int policy = 0;
    JTC_SYSCALL_3(pthread_getschedparam, m_thread_id, &policy, &param, != 0)
    param.sched_priority = lsd_priority_map[new_priority];
    JTC_SYSCALL_3(pthread_setschedparam, m_thread_id, policy, &param, != 0)
#   endif
#endif
#ifdef HAVE_DCE_THREADS
    JTC_SYSCALL_2(pthread_setprio, m_thread_id, lsd_priority_map[new_priority],
		  == -1)
#endif
#ifdef HAVE_WIN32_THREADS
    JTC_SYSCALL_2(SetThreadPriority, m_handle, lsd_priority_map[new_priority],
		  == 0)
#endif
#ifdef HAVE_VXWORKS_THREADS
    JTC_SYSCALL_2(taskPrioritySet, m_thread_id, lsd_priority_map[new_priority],
		  != OK)
#endif
}

//
// Get the threads priority.
//
int
JTCThread::getPriority() const
{
    JTCSyncT<JTCMutex> guard(m_mutex);

#if defined(HAVE_POSIX_THREADS) || defined(HAVE_DCE_THREADS)
    int prio;
#   ifdef HAVE_POSIX_THREADS
    sched_param param;
    int policy;
    JTC_SYSCALL_3(pthread_getschedparam, m_thread_id, &policy, &param, != 0)
    prio = param.sched_priority;
#   else
    JTC_SYSCALL_1(prio = pthread_getprio, m_thread_id, != 0)
#   endif
    if (prio <= 0)
    {
	return JTC_MIN_PRIORITY;
    }
    else if (prio == 1)
    {
	return JTC_NORM_PRIORITY;
    }
    return JTC_MAX_PRIORITY;
#endif
#ifdef HAVE_WIN32_THREADS
    DWORD rc;
    JTC_SYSCALL_1(rc = GetThreadPriority, m_handle,
		  == THREAD_PRIORITY_ERROR_RETURN);
    switch(rc)
    {
	case THREAD_PRIORITY_HIGHEST:
	case THREAD_PRIORITY_ABOVE_NORMAL:
	    return JTC_MAX_PRIORITY;

	case THREAD_PRIORITY_NORMAL:
	    return JTC_NORM_PRIORITY;

	case THREAD_PRIORITY_BELOW_NORMAL:
	case THREAD_PRIORITY_LOWEST:
	    return JTC_MIN_PRIORITY;
    }
    return JTC_NORM_PRIORITY;
#endif
#ifdef HAVE_VXWORKS_THREADS
    int prio;
    JTC_SYSCALL_2(taskPriorityGet, m_thread_id, &prio, != OK)
    if (prio <= lsd_priority_map[JTC_NORM_PRIORITY] / 2)
        return JTC_MAX_PRIORITY;
    else if (prio <= 3 * lsd_priority_map[JTC_NORM_PRIORITY] / 2)
        return JTC_NORM_PRIORITY;
    return JTC_MIN_PRIORITY;
#endif
}

//
// Start execution of the thread.
//
void
JTCThread::start()
{
    JTCSyncT<JTCMutex> guard(m_mutex);

    if (m_state != JTCThread::new_thread)
    {
	throw JTCIllegalThreadStateException("state is not new_thread");
    }

    //
    // Start the thread. The entry point into the JTC library is the C
    // call-style function lsf_thread_adapter. This in turn calls
    // entrance_hook()
    //
#if defined(HAVE_POSIX_THREADS) || defined(HAVE_DCE_THREADS)
    pthread_attr_t attr;
    try
    {
	JTC_SYSCALL_1(PTHREAD_ATTR_INIT, &attr, != 0)
    }
    catch(const JTCSystemCallException& e)
    {
	if (e.getError() == ENOMEM)
	{
	    throw JTCOutOfMemoryError(e.getMessage());
	}
	throw;
    }

#   if defined(HAVE_POSIX_THREADS) && !defined(__hpux)
   //
   // All threads set their priority explicitely. Under HPUX 11.x this
   // requires root privilege so it's disabled.
   //
   pthread_attr_setinheritsched(&attr, PTHREAD_EXPLICIT_SCHED);
#   endif

#   ifdef HAVE_PTHREAD_ATTR_SETSTACKSIZE
    if (lsd_initial_stack_size != 0)
    {
        pthread_attr_setstacksize(&attr, lsd_initial_stack_size);
    }
#   endif

    //
    // Call the custom pthread attribute hook, if defined.
    //
    if (sm_attr_hook != 0)
    {
	(*sm_attr_hook)(&attr);
    }
    
    pthread_t id;
    try
    {
	PTHREAD_CREATE(id, attr, lsf_thread_adapter, this)
    }
    catch(const JTCSystemCallException& e)
    {
	PTHREAD_ATTR_DESTROY(&attr);
	if (e.getError() == EAGAIN || e.getError() == ENOMEM)
	{
	    throw JTCOutOfMemoryError(e.getMessage());
	}
	throw;
    }
    m_thread_id = JTCThreadId(id);

    PTHREAD_ATTR_DESTROY(&attr);
#endif
    
#ifdef HAVE_WIN32_THREADS
    DWORD id;
    try
    {
	JTC_SYSCALL_6(
	    m_handle = (HANDLE)::_beginthreadex,
	    NULL, lsd_initial_stack_size,
            (unsigned (__stdcall*)(void*))lsf_thread_adapter, (LPVOID)this,
	    0, (unsigned int*)&id, == NULL)
    }
    catch(const JTCSystemCallException& e)
    {
	if (e.getError() == ERROR_NOT_ENOUGH_MEMORY ||
	    e.getError() == ERROR_OUTOFMEMORY)
	{
	    throw JTCOutOfMemoryError(e.getMessage());
	}
	throw;
    }
    m_thread_id = JTCThreadId(id);
#endif

#ifdef HAVE_VXWORKS_THREADS
    int id;
    try
    {
	JTC_SYSCALL_15(id = taskSpawn, NULL,
                       lsd_priority_map[JTC_NORM_PRIORITY], NULL,
                       lsd_initial_stack_size,
                       (int (*)(...))lsf_thread_adapter, (int)this,
                       0, 0, 0, 0, 0, 0, 0, 0, 0, == ERROR)
    }
    catch(const JTCSystemCallException& e)
    {
	if (e.getError() == S_memLib_NOT_ENOUGH_MEMORY)
	{
	    throw JTCOutOfMemoryError(e.getMessage());
	}
	throw;
    }
    m_thread_id = JTCThreadId(id);
#endif

    //
    // State is runnable.
    //
    m_state = JTCThread::runnable;
}

//
// The run method of the thread. If the thread has an associated
// target (that is the thread has a Runnable object), then invoke it's
// run method.
//
void
JTCThread::run()
{
    if (m_target)
    {
	m_target -> run();
    }
}

//
// Destroy a thread. May only be called on a new or dead thread.
//
void
JTCThread::destroy()
{
    {
        JTCSyncT<JTCMutex> guard(m_mutex);

        if (m_state == JTCThread::dead)
            return;

        if (m_state != JTCThread::new_thread)
            throw JTCIllegalThreadStateException("state is not new_thread");
    }

    try
    {
        exit_hook();
    }
    catch(...)
    {
        // Ignore
    }
}

//
// The thread is alive if runnable or not runnable.
//
bool
JTCThread::isAlive() const
{
    JTCSyncT<JTCMutex> guard(m_mutex);
    return  m_state == JTCThread::runnable ||
            m_state == JTCThread::not_runnable;
}

//
// The thread has dead if the state is dead.
//
bool
JTCThread::hasDied() const
{
   JTCSyncT<JTCMutex> guard(m_mutex);
   return m_state == JTCThread::dead;
}

//
// Wait for termination of this thread.
//
void
JTCThread::join()
{
    join(0,0);
}

static unsigned long
lsf_current_time_millis()
{
    unsigned long t;
#ifdef WIN32
    struct _timeb timebuffer;
    _ftime(&timebuffer);
    t = timebuffer.time * 1000;
    t += timebuffer.millitm;
#elif VXWORKS
    struct timespec tp;
    clock_gettime(CLOCK_REALTIME, &tp);
    t = tp.tv_sec * 1000;
    t += tp.tv_nsec / 1000000;
#else
    struct timeval tv;
    gettimeofday(&tv, 0);
    t = tv.tv_sec * 1000;
    t += tv.tv_usec/1000;
#endif
    return t;
}

//
// Wait at most millis milliseconds for termination of this thread.
//
void
JTCThread::join(
    long millis
)
{
    if (millis < 0)
    {
	throw JTCIllegalArgumentException
		("timeout value is negative");
    }

    unsigned long base = lsf_current_time_millis();
    unsigned long now = 0;

    JTCSyncT<JTCMutex> guard(m_mutex);

    
    if (millis == 0)
    {
        while (m_state == JTCThread::runnable ||
              m_state == JTCThread::not_runnable)
        {
            m_join_cond.wait(m_mutex);
        }
    }
    else
    {
        while (m_state == JTCThread::runnable ||
              m_state == JTCThread::not_runnable)
        {
            long delay = millis - now;
            if (delay <= 0)
            {
                break;
            }
            m_join_cond.wait(m_mutex, delay);
            now = lsf_current_time_millis() - base;
        }
    }
}

//
// Wait at most millis milliseconds plus nanos nanoseconds for termination
// of this thread.
//
void
JTCThread::join(
    long millis,
    int nanos
)
{
    if (millis < 0)
    {
	throw JTCIllegalArgumentException
		("timeout value is negative");
    }
    if (nanos > 999999)
    {
	throw JTCIllegalArgumentException
		("nanosecond timeout out of range");
    }
    if (nanos >= 500000 || (nanos != 0 && millis == 0))
    {
        ++millis;
    }
    join(millis);
}

//
// Enumerate the set of threads in this group.
//
int
JTCThread::enumerate(
    JTCThreadHandle* list,
    int len
)
{
    return currentThread() -> getThreadGroup() -> enumerate(list, len);
}

//
// Get a pointer to the current thread.
//
JTCThread*
JTCThread::currentThread()
{
    JTCThread* current = (JTCThread*)JTCTSS::get(JTCThread::sm_tss_key);
    if (current == 0)
    {
        throw JTCUnknownThreadException();
    }

    return current;
}

//
// Put the current thread to sleep for millis millseconds and nano
// nanoseconds.
//
void
JTCThread::sleep(
    long millis,
    int nano
)
{
    if (millis < 0)
    {
	throw JTCIllegalArgumentException
		("timeout value is negative");
    }
    if (nano > 999999)
    {
	throw JTCIllegalArgumentException
		("nanosecond timeout out of range");
    }

    //m_state = JTCThread::not_runnable;

#if defined(HAVE_PTHREAD_DELAY_NP)
    struct timespec tv;
    tv.tv_sec = millis/1000;
    tv.tv_nsec = (millis% 1000)*1000000 + nano;
    JTC_SYSCALL_1(pthread_delay_np, &tv, != 0);

#elif defined(HAVE_POSIX_THREADS)

    struct timespec tv;
    tv.tv_sec = millis/1000;
    tv.tv_nsec = (millis% 1000)*1000000 + nano;
    if (nanosleep(&tv, 0) < 0 && errno == EINTR)
    {
	throw JTCInterruptedException();
    }
#endif

#ifdef HAVE_WIN32_THREADS
    Sleep(millis);
#endif

#ifdef HAVE_VXWORKS_THREADS
    struct timespec tv;
    tv.tv_sec = millis/1000;
    tv.tv_nsec = (millis% 1000)*1000000 + nano;
    if (nanosleep(&tv, 0) < 0 && errno == EINTR)
    {
	throw JTCInterruptedException();
    }
#endif

    //m_state = JTCThread::runnable;
}

//
// Provide definition for sched_yield under SGI.
//
#ifdef __sgi
extern "C" { int sched_yield(void); };
#endif

//
// Yield the current threads timeslice.
//
void
JTCThread::yield()
{
#if defined(HAVE_POSIX_THREADS) || defined(HAVE_DCE_THREADS)
#   if defined(HAVE_SCHED_YIELD)
    sched_yield();
#   elif defined(HAVE_PTHREAD_YIELD)
    pthread_yield();
#   endif
#endif

#ifdef HAVE_WIN32_THREADS
    Yield();
#endif

#ifdef HAVE_VXWORKS_THREADS
    sched_yield();
#endif
}

//
// Return the number of active threads in the current thread group.
//
int
JTCThread::activeCount()
{
    return currentThread() -> getThreadGroup() -> activeCount();
}

//
// Get the id of this thread
//
JTCThreadId
JTCThread::getId() const
{
    JTCSyncT<JTCMutex> guard(m_mutex);
    return m_thread_id;
}

#if defined(HAVE_POSIX_THREADS) || defined(HAVE_DCE_THREADS)
void
JTCThread::setAttrHook(
    JTCAttrHook attrHook,
    JTCAttrHook* oldHook
)
{
    if (oldHook != 0)
    {
	*oldHook = sm_attr_hook;
    }
    sm_attr_hook = attrHook;
}
#endif

void
JTCThread::setRunHook(
    JTCRunHook runHook,
    JTCRunHook* oldHook
)
{
    if (oldHook != 0)
    {
	*oldHook = sm_run_hook;
    }
    sm_run_hook = runHook;
}

void
JTCThread::setStartHook(
    JTCStartHook startHook,
    JTCStartHook* oldHook
)
{
    if (oldHook != 0)
    {
	*oldHook = sm_start_hook;
    }
    sm_start_hook = startHook;
}

//
// This method is called by the external "C" calling function used by
// the native thread package. This is the entrance point from the
// native thread library into the sane world of JTC.
//
void
JTCThread::entrance_hook()
{
    try
    {
        //
        // Set the TSS data for this thread. This should a pointer to
        // this thread data.
        //
	JTCTSS::set(sm_tss_key, this);

	//
	// It's not possible to set the priority until after the
	// start request has been sent, since under WIN32 the handle
	// may not have been set yet.
	//
	setPriority(JTC_NORM_PRIORITY);

	//
	// Call the start hook
	//
	if (sm_start_hook != 0)
	    (*sm_start_hook)();

	//
	// Call the run method or the run hook for the thread.
	//
	if (sm_run_hook != 0)
	{
	    (*sm_run_hook)(this);
	}
	else
	{
	    run();
	}
    }
    catch(const JTCThreadDeath&)
    {
        //
        // This exception is thrown when stop() is called.
        //
    }
    catch(const JTCException& e)
    {
	m_group -> uncaughtException(this, e);
    }
    catch(...)
    {
	m_group -> uncaughtException(this);
    }

#ifdef HAVE_WIN32_THREADS
    JTCSyncT<JTCMutex> guard(m_mutex);

    //
    // Under WIN32 we must now close the thread handle, or suffer
    // the consequences.
    //
    CloseHandle(m_handle);
    m_handle = 0;
#endif
}

void
JTCThread::exit_hook()
{
    //
    // Run the TSS cleanup hooks. The TSSManager should always be
    // present at this point. Note that the sm_tss_key is cleared out
    // below and not at this point. However, this shouldn't make any
    // difference since the sm_tss_key doesn't have an associated data
    // destructor.
    //
    JTCTSSManager* manager = JTCTSSManager::instance();
    assert(manager != 0);
    manager -> cleanup();

    //
    // Copy the thread group handle so that it remainds valid if this
    // thread object is deleted.
    //
    JTCThreadGroupHandle my_group = m_group;

    //
    // ThreadGroup is locked first to ensure that the JTCInitialize
    // destructor doesn't wake until after the actual thread object
    // has been fully destroyed.
    //
    JTCSyncT<JTCMonitor> group_sync(*my_group.get());

    {
        //
        // Wake any waiting threads. If the thread is not running then
	// no thread can be attempting to join with it.
        //
        JTCSyncT<JTCMutex> guard(m_mutex);
	if (m_state == JTCThread::runnable ||
	    m_state == JTCThread::not_runnable)
	{
	    m_join_cond.broadcast();
	    
	    //
	    // Detach the thread, assuming it was not adopted and it
	    // was actually started.. That is ensures that the thread
	    // resources are freed once the thread terminates.
	    //
#ifndef WIN32
	    if (!m_is_adopted && !m_detached)
	    {
		try
		{
#if defined(HAVE_DCE_THREADS)
		    pthread_t id = m_thread_id;
		    JTC_SYSCALL_1(pthread_detach, &id, != 0)
#elif defined(HAVE_POSIX_THREADS)
		    JTC_SYSCALL_1(pthread_detach, m_thread_id, != 0);
#endif
		}
		catch(const JTCException&)
		{
		}

		m_detached = true;
	    }
#endif
	    m_state = JTCThread::dead;
	}
    }

    //
    // Remove the thread from its thread group
    //
    my_group -> remove(this);

    //
    // Lower the reference count.
    //
#ifdef WIN32
    if (InterlockedDecrement(&m_refcount) == 0)
	delete this;
#else
    {
	bool delete_it = false;

	m_ref_mutex.lock();
	if (--m_refcount == 0)
	    delete_it = true;
	m_ref_mutex.unlock();

	if (delete_it)
	    delete this;
    }
#endif

    //
    // Clear the current value after the reference count has been
    // lowered.  This ensures that the currentThread is available in
    // the thread objects destructor.
    //
    JTCTSS::set(sm_tss_key, 0);
}

#ifndef HAVE_JTC_NO_IOSTREAM

//
// IOstream operator for the thread class.
//
ostream&
operator<<(
    ostream& os,
    const JTCThread& thr
)
{
    os << "Thread: " << thr.getName();
    return os;
}

#endif // !HAVE_JTC_NO_IOSTREAM

//
// This represents the number of times that the thread has been
// initialized.
//
int JTCInitialize::sm_init_count = 0;

// ----------------------------------------------------------------------
// JTCInitialize private member implementation
// ----------------------------------------------------------------------

void
JTCInitialize::initialize()
{
    //
    // The first JTCInitialize instance. This instance is used by
    // other JTCInitialize objects to initialize their attributes.
    //
    lsd_initialize_instance = this;

    //
    // Create the thread specific storage manager
    //
    JTCThread::sm_tss_key = JTCTSS::allocate();

    m_tss_manager = new JTCTSSManager;

    //
    // Create "root" thread group. Note that this thread group will
    // only contain child thread groups, not threads. The
    // lsd_root_thread_group static pointer is used by adopted
    // threads.
    //
    m_root_thread_group = new JTCThreadGroup();

    lsd_root_thread_group = m_root_thread_group.get();

    //
    // Create entry for main thread.
    //
    m_main_thread = new JTCThread(JTCThreadId::self(), true);
}

// ----------------------------------------------------------------------
// JTCInitialize constructor and destructor
// ----------------------------------------------------------------------

JTCInitialize::JTCInitialize()
{
    //
    // Only initialize on first call. Subsequent attempts do nothing
    // but initialize its members with the already existing first
    // JTCInitialize instance.
    //
    if (sm_init_count++ != 0)
    {

	assert(lsd_initialize_instance != 0);

	m_root_thread_group = lsd_initialize_instance -> m_root_thread_group;
	m_main_thread = lsd_initialize_instance -> m_main_thread;
	m_tss_manager = lsd_initialize_instance -> m_tss_manager;

	return;
    }

    initialize();
}

JTCInitialize::JTCInitialize(
    int& argc,
    char** argv
)
{
    //
    // Only initialize on first call. Subsequent attempts do nothing
    // but initialize its members with the already existing first
    // JTCInitialize instance.
    //
    if (sm_init_count++ != 0)
    {
	assert(lsd_initialize_instance != 0);

	m_root_thread_group = lsd_initialize_instance -> m_root_thread_group;
	m_main_thread = lsd_initialize_instance -> m_main_thread;
	m_tss_manager = lsd_initialize_instance -> m_tss_manager;

	return;
    }

    int i = 0;
    while (i < argc)
    {
        //
        // Initial stack size in kilobytes.
        //
	if (strcmp(argv[i], "-JTCss") == 0)
	{
            int ssTmp;
	    if (i + 1 < argc)
		ssTmp = atoi(argv[i + 1]);
	    else
	    {
		throw JTCInitializeError("argument expected for -JTCss");
	    }
            if (ssTmp <= 0)
            {
		throw JTCInitializeError("stack size must be positive");
            }
            lsd_initial_stack_size = ssTmp * 1024;
	    for (int j = i ; j + 2 <  argc ; j++)
		argv[j] = argv[j + 2];

	    argc -= 2;
	}
	else if (strcmp(argv[i], "-JTCversion") == 0)
	{
#ifndef HAVE_JTC_NO_IOSTREAM
            extern const char* JTCVersion;
            cout << JTCVersion << endl;
#endif // !HAVE_JTC_NO_IOSTREAM

	    for (int j = i ; j + 1 < argc ; j++)
		argv[j] = argv[j + 1];

	    argc -= 1;
	}
	else if (strncmp(argv[i], "-JTC", strlen("-JTC")) == 0)
	{
	    char buf[1024];
	    sprintf(buf, "unknown option: %s", argv[i]);

	    throw JTCInitializeError(buf);
	}
	else
	{
	    i++;
	}
    }
    initialize();
}

JTCInitialize::~JTCInitialize()
{
    //
    // Only terminate on last call.
    //
    if (--sm_init_count > 0)
	return;

    //
    // Wait for all running threads to terminate.
    //
    waitTermination();

    try
    {
	m_root_thread_group -> destroy();
    }
    catch(...)
    {
	// Ignore
    }

    m_root_thread_group = JTCThreadGroupHandle();

    delete m_tss_manager;

    JTCTSS::release(JTCThread::sm_tss_key);
}

// ----------------------------------------------------------------------
// JTCInitialize public member implementation
// ----------------------------------------------------------------------

//
// Wait for all running threads to terminate.
//
void
JTCInitialize::waitTermination()
{
    //
    // If waitTermination has already been called, return
    //
    if(!m_main_thread)
        return;

    //
    // Clean up main thread.
    //
    m_main_thread -> exit_hook();
    m_main_thread = JTCThreadHandle();

    //
    // Wait for all threads to terminate, and destroy any unstarted
    // threads. Note that destroying any unstarted threads can cause
    // problems if the user is actually starting new threads when in
    // this loop since it's possible if the user is very unlucky that
    // the destroy() call at the bottom of this loop will be hit and a
    // newly created, but not yet started thread will be destroyed.
    //
    int count;
    while ((count = m_root_thread_group -> activeCount()) > 0)
    {
	JTCThreadHandle* list = new JTCThreadHandle[count];

	count = m_root_thread_group -> enumerate(list, count);
	for (int i = 0 ; i < count ; i++)
	{
	    try
	    {
		list[i] -> join();
	    }
	    catch (const JTCInterruptedException&)
	    {
		// Ignore
	    }

	    //
	    // If the thread has been created, but not started then
	    // destroy it.
	    //
	    if (list[i] -> m_state == JTCThread::new_thread)
	    {
		try
		{
		    list[i] -> destroy();
		}
		catch(const JTCIllegalThreadStateException&)
		{
		    // Ignore
		}
	    }
	}
	        
	delete[] list;
    }
}

//
// Determine if the JTC library has been initialized.
//
bool
JTCInitialize::initialized()
{
    return sm_init_count > 0;
}

// ----------------------------------------------------------------------
// JTCAdoptCurrentThread constructor and destructor
// ----------------------------------------------------------------------

JTCAdoptCurrentThread::JTCAdoptCurrentThread()
{
    JTCThread* current = (JTCThread*)JTCTSS::get(JTCThread::sm_tss_key);
    if (current == 0)
    {
	//
	// Create entry for adopted thread.
	//
	m_adopted_thread = new JTCThread(JTCThreadId::self(), false);
    }
}

JTCAdoptCurrentThread::~JTCAdoptCurrentThread()
{
    if (m_adopted_thread)
    {
	//
	// Clean up main thread, and the ThreadCounter.
	//
	m_adopted_thread -> exit_hook();
    }
}
