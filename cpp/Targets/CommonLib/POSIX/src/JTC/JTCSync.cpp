// **********************************************************************
//
// Copyright (c) 2001
// IONA Technologies, Inc.
// Waltham, MA, USA
//
// All Rights Reserved
//
// **********************************************************************

#include "JTC/JTCSync.h"

//
// Inlining the methods causes problems with CC 7.1 under SGI
//
#if defined(__sgi)
JTCSynchronized::JTCSynchronized(
    const JTCMonitor& mon
)
    : m_monitor(&mon),
      m_lock_type(monitor)
{
    m_monitor -> lock();
}

JTCSynchronized::JTCSynchronized(
    const JTCMutex& m
)
    : m_mutex(&m),
      m_lock_type(mutex)
{
    m_mutex -> lock();
}

JTCSynchronized::JTCSynchronized(
    const JTCRecursiveMutex& m
)
    : m_rmutex(&m),
      m_lock_type(recursive_mutex)
{
    m_rmutex -> lock();
}

JTCSynchronized::JTCSynchronized(
    const JTCRWMutex& m,
    ReadWriteLockType type
)
    : m_rwmutex(&m),
      m_lock_type(read_write_mutex)
{
    if(type == read_lock)
	m_rwmutex -> read_lock();
    else
	m_rwmutex -> write_lock();
}

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
