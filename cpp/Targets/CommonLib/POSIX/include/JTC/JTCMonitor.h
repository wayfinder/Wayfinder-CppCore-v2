// **********************************************************************
//
// Copyright (c) 2001
// IONA Technologies, Inc.
// Waltham, MA, USA
//
// All Rights Reserved
//
// **********************************************************************

#ifndef JTC_MONITOR_H
#define JTC_MONITOR_H

//
// Forward declarations.
//
template <class T> class JTCSyncT;

#include "JTC/JTCTypes.h"
#include "JTC/JTCCond.h"

//
// A monitor is used to ensure that only one thread can have access to
// the internals of an object at one time. Use the class
// JTCSynchronized to automatically lock and unlock a monitor.
//
// The JTCMonitor class also provides an efficient way of notification
// and suspension of threads. The wait method waits for a notification
// by another thread. A thread may notify a waiting object with the
// notify call. A thread may notify all waiting objects with the
// notifyAll call.
//

class JTCMonitorBase
{
protected:

    JTCMonitorBase();

    virtual
    ~JTCMonitorBase();

protected:

    //
    // Validate that the monitor is locked by the calling thread.
    //
    void validate_owner(
	const JTCThreadId&,
    	const char* caller
    ) const;

    JTCCond m_mon_cond; // Monitors condition variable.
    int m_nnotify; // Number of pending notifications (-1 broadcast).

private:

    //
    // Hide copy constructor and assignment operator.
    //
    JTCMonitorBase(
	const JTCMonitorBase&
    );
    void
    operator=(
	const JTCMonitorBase&
    );
};

template <class T>
class JTCMonitorT : public JTCMonitorBase
{
public:

    JTCMonitorT();
    virtual
    ~JTCMonitorT();

    //
    // Wait for notification.
    //
    void
    wait();

    //
    // Wait for notification, or timeout milliseconds.
    //
    void
    wait(
	long msec
    );
	
    //
    // Wake one waiting object.
    //
    void
    notify();

    //
    // Wake all waiting objects.
    //
    void
    notifyAll();

private:

    friend class JTCSynchronized;
    friend class JTCSyncT< JTCMonitorT >;

    //
    // Lock the monitors mutex
    //
    void
    lock() const;

    //
    // Unlock the monitors mutex.
    //
    void
    unlock() const;

    //
    // Perform any pending notifications.
    //
    void
    notify_internal(
	int times
    );

    T m_mon_mutex; // The monitors mutex
};

#include "JTC/JTCMonitorI.h"

//
// Provide default typedef for a re-entrant (Java style) Monitor.
//
#include "JTC/JTCMutex.h"

//
// TODO: For development purposes having a template based monitor
// implementation is a total pain.
//
//typedef JTCMonitorT< JTCRecursiveMutex> JTCMonitor;

class JTCMonitor : public JTCMonitorBase
{
public:

    JTCMonitor();
    virtual
    ~JTCMonitor();

    //
    // Wait for notification.
    //
    void
    wait();

    //
    // Wait for notification, or timeout milliseconds.
    //
    void
    wait(
	long msec
    );
	
    //
    // Wake one waiting object.
    //
    void
    notify();

    //
    // Wake all waiting objects.
    //
    void
    notifyAll();

private:

    friend class JTCSynchronized;
    friend class JTCSyncT< JTCMonitor >;

    //
    // Lock the monitors mutex
    //
    void
    lock() const;

    //
    // Unlock the monitors mutex.
    //
    void
    unlock() const;

    //
    // Perform any pending notifications.
    //
    void
    notify_internal(
	int times
    );

    JTCRecursiveMutex m_mon_mutex;
};

#if defined(HAVE_NO_EXPLICIT_TEMPLATES) && !defined(HAVE_PRAGMA_DEFINE)
#   include "JTC/JTCMonitorI.h"
#endif

#endif
