// **********************************************************************
//
// Copyright (c) 2001
// IONA Technologies, Inc.
// Waltham, MA, USA
//
// All Rights Reserved
//
// **********************************************************************

#ifndef JTC_HANDLE_H
#define JTC_HANDLE_H

#include "JTC/JTCTypes.h"

#ifndef WIN32
#    include "JTC/JTCMutex.h"
#endif

//
// Provide some handy typedefs.
//
template <class T> class JTCHandleT;

class JTCThread;
typedef JTCHandleT<JTCThread> JTCThreadHandle;

class JTCThreadGroup;
typedef JTCHandleT<JTCThreadGroup> JTCThreadGroupHandle;

class JTCRunnable;
typedef JTCHandleT<JTCRunnable> JTCRunnableHandle;

//
// JTCRefCount class. This is a base class to be used in conjunction
// with the JTCHandleT class.
//
class JTCRefCount
{
public:

    long m_refcount;

    //
    // Under WIN32 we do interlocked increments and decrements so no
    // mutex is necessary to protect the reference count.
    //
#ifndef WIN32
    JTCMutex m_ref_mutex;
#endif
    
    JTCRefCount()
        : m_refcount(0)
    {
    }

    virtual
    ~JTCRefCount()
    {
    }
};

//
// This class is a handle to a reference counted object. This ensures
// that the object is deleted when it is no longer referenced.
//
template <class T>
class JTCHandleT
{
public:

    //
    // Constructors.
    //
    JTCHandleT(
	T* tg = 0
	);

    JTCHandleT(
	const JTCHandleT<T>& rhs
	);

    ~JTCHandleT();

    //
    // Assignment operator.
    //
    JTCHandleT<T>&
    operator=(
	const JTCHandleT<T>& rhs
    );

    JTCHandleT<T>&
    operator=(
	T* tg
    );

    //
    // Equality and inequality.
    //
    bool
    operator==(
	const JTCHandleT<T>& rhs
    ) const
    {
	return m_object == rhs.m_object;
    }

    bool
    operator!=(
	const JTCHandleT<T>& rhs
    ) const
    {
	return m_object != rhs.m_object;
    }

    //
    // Logical not operator. Are we referring to a valid Thread?
    //
    bool
    operator!() const
    {
	return m_object == 0;
    }

    operator bool() const
    {
	return m_object != 0;
    }
    
    //
    // Make access to the Thread intuitive.
    //
    T*
    operator->() const
    {
	return m_object;
    }

    T*
    get() const
    {
	return m_object;
    }

    T&
    operator*()
    {
	return *m_object;
    }

private:

    T* m_object; // Referenced object
};

#if defined(HAVE_NO_EXPLICIT_TEMPLATES) && !defined(HAVE_PRAGMA_DEFINE)
#   include "JTC/JTCHandleI.h"
#endif

#endif
