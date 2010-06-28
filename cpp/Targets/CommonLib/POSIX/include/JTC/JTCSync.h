// **********************************************************************
//
// Copyright (c) 2001
// IONA Technologies, Inc.
// Waltham, MA, USA
//
// All Rights Reserved
//
// **********************************************************************

#ifndef JTC_SYNC_H
#define JTC_SYNC_H

#include "JTC/JTCMutex.h"
#include "JTC/JTCMonitor.h"
#include "JTC/JTCRWMutex.h"

//
// ReadLock class
//
class JTCReadLock
{
public:

    JTCReadLock(const JTCRWMutex& m)
	: m_mutex(&m)
    {
	m_mutex -> read_lock();
    }

    ~JTCReadLock()
    {
	m_mutex -> unlock();
    }

private:

    const JTCRWMutex* m_mutex;
};

//
// WriteLock class
//
class JTCWriteLock
{
public:

    JTCWriteLock(const JTCRWMutex& m)
	: m_mutex(&m)
    {
	m_mutex -> write_lock();
    }

    ~JTCWriteLock()
    {
	m_mutex -> unlock();
    }

private:

    const JTCRWMutex* m_mutex;
};

//
// This class is used to lock and unlock a monitor. To avoid virtual
// function call and vtbl overhead there is no mutex base class --
// hence the need for this monster.
//
class JTCSynchronized
{
public:

    JTCSynchronized(
	const JTCMonitor& mon
    );
    JTCSynchronized(
	const JTCMutex& m
    );
    JTCSynchronized(
	const JTCRecursiveMutex& m
    );
    enum ReadWriteLockType
    {
	read_lock,
	write_lock
    };
    JTCSynchronized(
	const JTCRWMutex& m,
	ReadWriteLockType type
    );
    ~JTCSynchronized();

private:

    //
    // Hide copy constructor and assignment operator.
    //
    JTCSynchronized(
	const JTCSynchronized&
    );
    void
    operator=(
	const JTCSynchronized&
    );

    const JTCMonitor* m_monitor; // Associated monitor.
    const JTCMutex* m_mutex; // Associated mutex.
    const JTCRecursiveMutex* m_rmutex; // Associated recursive mutex.
    const JTCRWMutex* m_rwmutex; // Associated read/write mutex.

    enum LockType
    {
	monitor,
	mutex,
	recursive_mutex,
	read_write_mutex
    };

    LockType m_lock_type; // The lock type
};

//
// Inlining the methods causes problems with CC 7.1 under SGI
//
#if !defined(__sgi)
inline
JTCSynchronized::JTCSynchronized(
    const JTCMonitor& mon
)
    : m_monitor(&mon),
      m_lock_type(monitor)
{
    m_monitor -> lock();
}

inline
JTCSynchronized::JTCSynchronized(
    const JTCMutex& m
)
    : m_mutex(&m),
      m_lock_type(mutex)
{
    m_mutex -> lock();
}

inline
JTCSynchronized::JTCSynchronized(
    const JTCRecursiveMutex& m
)
    : m_rmutex(&m),
      m_lock_type(recursive_mutex)
{
    m_rmutex -> lock();
}

inline
JTCSynchronized::JTCSynchronized(
    const JTCRWMutex& m,
    ReadWriteLockType type
)
    : m_rwmutex(&m),
      m_lock_type(read_write_mutex)
{
    if (type == read_lock)
	m_rwmutex -> read_lock();
    else
	m_rwmutex -> write_lock();
}

inline
JTCSynchronized::~JTCSynchronized()
{
    switch(m_lock_type)
    {
    case monitor:
	m_monitor -> unlock();
	break;

    case mutex:
	m_mutex -> unlock();
	break;

    case recursive_mutex:
	m_rmutex -> unlock();
	break;

    case read_write_mutex:
	m_rwmutex -> unlock();
	break;
    }
}
#endif

#endif
