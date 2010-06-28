// **********************************************************************
//
// Copyright (c) 2001
// IONA Technologies, Inc.
// Waltham, MA, USA
//
// All Rights Reserved
//
// **********************************************************************

#ifndef JTC_MONITOR_I_H
#define JTC_MONITOR_I_H

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

template<class T>
JTCMonitorT<T>::JTCMonitorT()
{
}

template <class T>
JTCMonitorT<T>::~JTCMonitorT()
{
}

// ----------------------------------------------------------------------
// JTCMonitorT public member implementation
// ----------------------------------------------------------------------

//
// Wait for the monitor to be signalled.
//
template <class T> void
JTCMonitorT<T>::wait()
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
template <class T> void
JTCMonitorT<T>::wait(
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
template <class T> void
JTCMonitorT<T>::notify()
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
template <class T> void
JTCMonitorT<T>::notifyAll()
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
// JTCMonitorT<T> private member implementation
// ----------------------------------------------------------------------

//
// Lock the monitors mutex.
//
template <class T> void
JTCMonitorT<T>::lock() const
{
    if (m_mon_mutex.lock())
    {
	//
	// On the first mutex acquisition set the number of notifies
	// to 0.
	//
	JTCMonitorT<T>* This = (JTCMonitorT<T>*)this;
	This -> m_nnotify = 0;
    }
}

//
// Unlock the monitors mutex.
//
template <class T> void
JTCMonitorT<T>::unlock() const
{
    int nnotify = m_nnotify;
    if (m_mon_mutex.unlock())
    {
	//
	// Perform any pending notifies
	//
	JTCMonitorT<T>* This = (JTCMonitorT<T>*)this;
	This -> notify_internal(nnotify);
    }
}

//
// Perform any pending notifications.
//
template <class T> void
JTCMonitorT<T>::notify_internal(
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

#endif
