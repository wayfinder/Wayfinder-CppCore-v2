// **********************************************************************
//
// Copyright (c) 2001
// IONA Technologies, Inc.
// Waltham, MA, USA
//
// All Rights Reserved
//
// **********************************************************************

#include "JTC/JTCMonitor.h"
#include "JTC/JTCMonitorI.h"



#ifndef HAVE_JTC_NO_IOSTREAM
#   if defined(HAVE_SSTREAM)
#      include <sstream>
#   else
#      if defined(HAVE_STRSTREA_H)
#         include <strstrea.h>
#      else
#         include <strstream.h>
#      endif
#   endif
#   ifdef HAVE_STD_IOSTREAM
using namespace std;
#   endif
#endif // !HAVE_JTC_NO_IOSTREAM

#include <stdlib.h>

//
// TODO: For development purposes having a template based monitor
// implementation is a total pain.
//
//#ifndef HAVE_NO_EXPLICIT_TEMPLATES
//template class JTCMonitorT<JTCRecursiveMutex>;
//#else
//#  ifdef HAVE_PRAGMA_DEFINE
//#    pragma define(JTCMonitorT<JTCRecursiveMutex>)
//#  endif
//#endif

// ----------------------------------------------------------------------
// JTCMonitor constructor and destructor
// ----------------------------------------------------------------------

JTCMonitorBase::JTCMonitorBase()
    : m_nnotify(0)
{
}

JTCMonitorBase::~JTCMonitorBase()
{
}

// ----------------------------------------------------------------------
// JTCMonitorBase private member implementation
// ----------------------------------------------------------------------

//
// Verify that the mutex is locked by the calling thread. If the mutex
// is not locked by the calling thread then throw
// JTCIllegalMonitorStateException.
//
void
JTCMonitorBase::validate_owner(
    const JTCThreadId& id,
    const char* caller
) const
{
    if (id != JTCThreadId::self())
    {
#ifndef HAVE_JTC_NO_IOSTREAM
#ifndef HAVE_SSTREAM
	char buf[1024];
        JTC_STD(ostrstream) os(buf, 1024);
        os << "JTCMonitor::"
           << caller
           << " Thread: "
           << JTCThreadId::self()
           << " doesn't own mutex.  Owner: "
           << id
           << ends;
	throw JTCIllegalMonitorStateException(buf);
#else // HAVE_SSTREAM
	JTC_STD(ostringstream) os;
	os << "JTCMonitor::"
	   << caller
	   << " Thread: "
           << JTCThreadId::self()
	   << " doesn't own mutex.  Owner: "
	   << id
	   << ends;
	throw JTCIllegalMonitorStateException(os.str().c_str());
#endif // HAVE_SSTREAM
#else // HAVE_JTC_NO_IOSTREAM
	char buf[1024];
	char buf2[128];
	char buf3[128];
	sprintf(buf, "JTCMonitor::%s Thread: %s doesn't own mutex.  Owner: %s",
		caller, JTCThreadId::self().toString(buf2, 128),
		id.toString(buf3, 128));
	throw JTCIllegalMonitorStateException(buf);
#endif // !HAVE_JTC_NO_IOSTREAM
    }
}

//
// TODO: JTCMonitor implementation for development purposes
//

//
// Implementation notes:
//
// This monitor implements the Mesa monitor semantics. That is any
// calls to notify() or notifyAll() are delayed until the monitor is
// unlocked. This can happen either due to a call to unlock(), or a
// call to wait(). The m_nnotify flag keeps track of the number of
// pending notification calls. -1 indicates a broadcast, a positive
// number indicates <n> calls to notify(). The m_nnotify flag is reset
// upon initial acquisition of the monitor lock (either through a call
// to lock(), or a return from wait().
//

// ----------------------------------------------------------------------
// JTCMonitorT constructor and destructor
// ----------------------------------------------------------------------

JTCMonitor::JTCMonitor()
{
}

JTCMonitor::~JTCMonitor()
{
}

// ----------------------------------------------------------------------
// JTCMonitor public member implementation
// ----------------------------------------------------------------------

//
// Wait for the monitor to be signalled.
//
void
JTCMonitor::wait()
{
    //
    // Verify that the monitor is locked by the calling thread.
    //
    validate_owner(m_mon_mutex.get_owner(), "wait()");

    //
    // Perform any pending notifies
    //
    notify_internal(m_nnotify);
 
    //
    // Wait for a notification
    //
    try
    {
	m_mon_cond.wait(m_mon_mutex);
	//
	// Reset the nnotify count once wait() returns.
	//
    }
    catch(...)
    {
	m_nnotify = 0;
	throw;
    }
    m_nnotify = 0;
}

//
// Wait to be signalled for timeout milliseconds.
//
void
JTCMonitor::wait(
    long timeout
)
{
    //
    // Verify that the monitor is locked by the calling thread.
    //
    validate_owner(m_mon_mutex.get_owner(), "wait(long)");

    //
    // Perform any pending notifies.
    //
    notify_internal(m_nnotify);

    //
    // Wait for a notification.
    //
    try
    {
	m_mon_cond.wait(m_mon_mutex, timeout);

	//
	// Reset the nnotify count once wait() returns.
	//
    }
    catch(...)
    {
	m_nnotify = 0;
	throw;
    }

    m_nnotify = 0;
}

//
// Wake one waiting thread.
//
void
JTCMonitor::notify()
{
    //
    // Ensure that the monitors mutex is locked by the calling thread.
    //
    validate_owner(m_mon_mutex.get_owner(), "notify");

    //
    // Increment the m_nnotify flag, unless a broadcast has already
    // been requested.
    //
    if (m_nnotify != -1)
    {
	++m_nnotify;
    }
}

//
// Wake all waiting threads.
//
void
JTCMonitor::notifyAll()
{
    //
    // Ensure that the monitors mutex is locked by the calling thread.
    //
    validate_owner(m_mon_mutex.get_owner(), "notifyAll");

    //
    // Set the m_nnotify flag to -1 (indicates broadcast)
    //
    m_nnotify = -1;
}

// ----------------------------------------------------------------------
// JTCMonitor private member implementation
// ----------------------------------------------------------------------

//
// Lock the monitors mutex.
//
void
JTCMonitor::lock() const
{
    if (m_mon_mutex.lock())
    {
	//
	// On the first mutex acquisition set the number of notifies
	// to 0.
	//
	JTCMonitor* This = (JTCMonitor*)this;
	This -> m_nnotify = 0;
    }
}

//
// Unlock the monitors mutex.
//
void
JTCMonitor::unlock() const
{
    int nnotify = m_nnotify;
    if (m_mon_mutex.unlock())
    {
	//
	// Perform any pending notifies
	//
	JTCMonitor* This = (JTCMonitor*)this;
	This -> notify_internal(nnotify);
    }
}

//
// Perform any pending notifications.
//
void
JTCMonitor::notify_internal(
    int nnotify
)
{
    //
    // Zero indicates no notifies.
    //
    if (nnotify != 0)
    {
	//
	// -1 means notifyAll.
	//
	if (nnotify == -1)
	{
	    m_mon_cond.broadcast();
	    return;
	}
	else
	{
	    //
	    // Otherwise notify n times.
	    //
	    while (nnotify > 0)
	    {
		m_mon_cond.signal();
		--nnotify;
	    }
	}
    }
}
