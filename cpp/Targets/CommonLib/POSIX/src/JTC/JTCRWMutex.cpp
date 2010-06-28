// **********************************************************************
//
// Copyright (c) 2001
// IONA Technologies, Inc.
// Waltham, MA, USA
//
// All Rights Reserved
//
// **********************************************************************

#include "JTC/JTCRWMutex.h"
#include "JTC/JTCSyncT.h"

#include <assert.h>

// ----------------------------------------------------------------------
// JTCRWLock constructor and destructor
// ----------------------------------------------------------------------

JTCRWMutex::JTCRWMutex()
    : m_count(0),
      m_waiting_writers(0)
{
}

JTCRWMutex::~JTCRWMutex()
{
}

// ----------------------------------------------------------------------
// JTCRWMutex public member implementation
// ----------------------------------------------------------------------

void
JTCRWMutex::read_lock() const
{
    //
    // Work around lack of mutable
    //
    JTCRWMutex* This = (JTCRWMutex*)this;
    This -> read_lock_internal();
}

void
JTCRWMutex::write_lock() const
{
    //
    // Work around lack of mutable
    //
    JTCRWMutex* This = (JTCRWMutex*)this;
    This -> write_lock_internal();
}

void
JTCRWMutex::unlock() const
{
    //
    // Work around lack of mutable
    //
    JTCRWMutex* This = (JTCRWMutex*)this;
    This -> unlock_internal();
}

// ----------------------------------------------------------------------
// JTCRWMutex public member implementation
// ----------------------------------------------------------------------

void
JTCRWMutex::read_lock_internal()
{
    JTCSyncT<JTCMutex> sync(m_mutex);

    //
    // Wait while a writer holds the lock or while writers are waiting
    // to get the lock.
    //
    while (m_count < 0 || m_waiting_writers != 0)
	m_ok_to_read.wait(m_mutex);
    m_count++;
}

void
JTCRWMutex::write_lock_internal()
{
    JTCSyncT<JTCMutex> sync(m_mutex);

    //
    // Wait for the lock to become available and increment the number
    // of waiting writers.
    //
    while (m_count != 0)
    {
	m_waiting_writers++;
	try
	{
	    //TODO: Exception
	    m_ok_to_write.wait(m_mutex);
	}
	catch(...)
	{
	    --m_waiting_writers;
	    throw;
	}
	m_waiting_writers--;
    }

    //
    // Got the lock, indicate it's held by a writer.
    //
    m_count = -1;
}

void
JTCRWMutex::unlock_internal()
{
    bool ww;
    bool wr;
    {
	JTCSyncT<JTCMutex> sync(m_mutex);

	assert(m_count != 0);

	//
	// If m_count < 0, the calling thread is a writer that holds the
	// lock, so release the lock.  Otherwise, m_count is guaranteed to
	// be > 0, so the calling thread is a reader releasing the lock.
	//
	if (m_count < 0)
	{
	    //
	    // Writer called unlock
	    //
	    m_count = 0;
	}
	else
	{
	    //
	    // Reader called unlock
	    //
	    --m_count;
	}

	//
	// Writers are waiting (ww) if m_waiting_writers > 0.  In that
	// case, it's OK to let another writer into the region once there
	// are no more readers (m_count == 0).  Otherwise, no writers are
	// waiting but readers may be waiting (wr).
	//
	ww = (m_waiting_writers != 0 && m_count == 0);
	wr = (m_waiting_writers == 0);
    }

    //
    // Wake up a waiting writer if there is one. If not, wake up all
    // readers (just in case -- there may be none).
    //
    if (ww)
    {
	//
	// Wake writer
	//
	m_ok_to_write.signal();
    }
    else if (wr)
    {
	//
	// Wake readers
	//
	m_ok_to_read.broadcast();
    }
}

