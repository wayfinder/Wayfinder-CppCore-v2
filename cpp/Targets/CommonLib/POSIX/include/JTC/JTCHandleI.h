// **********************************************************************
//
// Copyright (c) 2001
// IONA Technologies, Inc.
// Waltham, MA, USA
//
// All Rights Reserved
//
// **********************************************************************

#ifndef JTC_HANDLE_I_H
#define JTC_HANDLE_I_H

// ----------------------------------------------------------------------
// JTCHandleT constructors and destructor
// ----------------------------------------------------------------------

template <class T>
JTCHandleT<T>::JTCHandleT(
    T* tg
)
    : m_object(tg)
{
    if (m_object != 0)
    {
	//
	// Boost reference count.
	//
#ifdef WIN32
	InterlockedIncrement(&m_object -> m_refcount);
#else
	m_object -> m_ref_mutex.lock();
	++m_object -> m_refcount;
	m_object -> m_ref_mutex.unlock();
#endif
    }
}

template <class T>
JTCHandleT<T>::~JTCHandleT()
{
#ifdef WIN32
    if (m_object != 0 && InterlockedDecrement(&m_object -> m_refcount) == 0)
	delete m_object;
#else
    if (m_object != 0)
    {
	bool delete_it = false;

	m_object -> m_ref_mutex.lock();
	if (--m_object -> m_refcount == 0)
	    delete_it = true;
	m_object -> m_ref_mutex.unlock();

	if (delete_it)
	    delete m_object;
    }
#endif
}

// ----------------------------------------------------------------------
// JTCHandle public member implementation
// ----------------------------------------------------------------------

template <class T>
JTCHandleT<T>::JTCHandleT(
    const JTCHandleT<T>& rhs
)
    : m_object(rhs.m_object)
{
    if (m_object != 0)
    {
	//
	// Boost reference count.
	//
#ifdef WIN32
	InterlockedIncrement(&m_object -> m_refcount);
#else
	m_object -> m_ref_mutex.lock();
	++m_object -> m_refcount;
	m_object -> m_ref_mutex.unlock();
#endif
    }
}

template <class T>
JTCHandleT<T>&
JTCHandleT<T>::operator=(
    const JTCHandleT<T>& rhs
)
{
    if (m_object == rhs.m_object)
	return *this;

#ifdef WIN32
    if (m_object != 0 && InterlockedDecrement(&m_object -> m_refcount) == 0)
	delete m_object;

    //
    // Copy RHS contents
    //
    m_object = rhs.m_object;

    //
    // Boost reference count.
    //
    if (m_object != 0)
	InterlockedIncrement(&m_object -> m_refcount);
#else

    if (m_object != 0)
    {
	//
	// Time to clean out object?
	//
	bool delete_it = false;

	m_object -> m_ref_mutex.lock();
	if (--m_object -> m_refcount == 0)
	    delete_it = true;
	m_object -> m_ref_mutex.unlock();

	if (delete_it)
	    delete m_object;
    }

    m_object = rhs.m_object; // Copy in RHS contents.
    if (m_object != 0)
    {
	m_object -> m_ref_mutex.lock();
	++m_object -> m_refcount; // Boost reference count.
	m_object -> m_ref_mutex.unlock();
    }
#endif

    return *this;
}

template <class T>
JTCHandleT<T>&
JTCHandleT<T>::operator=(
    T* tg
)
{
    if (m_object == tg)
	return *this;

#ifdef WIN32
    if (m_object != 0 && InterlockedDecrement(&m_object -> m_refcount) == 0)
	delete m_object;

    //
    // Copy object
    //
    m_object = tg;

    //
    // Boost reference count.
    //
    if (m_object != 0)
	InterlockedIncrement(&m_object -> m_refcount);
#else

    if (m_object != 0)
    {
	//
	// Time to clean out object?
	//
	bool delete_it = false;

	m_object -> m_ref_mutex.lock();
	if (--m_object -> m_refcount == 0)
	    delete_it = true;
	m_object -> m_ref_mutex.unlock();

	if (delete_it)
	    delete m_object;
    }

    m_object = tg; // Copy object
    if (m_object != 0)
    {
	m_object -> m_ref_mutex.lock();
	++m_object -> m_refcount; // Boost reference count.
	m_object -> m_ref_mutex.unlock();
    }
#endif

    return *this;
}

#endif
