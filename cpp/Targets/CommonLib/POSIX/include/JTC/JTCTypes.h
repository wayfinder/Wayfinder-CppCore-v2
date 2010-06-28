// **********************************************************************
//
// Copyright (c) 2001
// IONA Technologies, Inc.
// Waltham, MA, USA
//
// All Rights Reserved
//
// **********************************************************************

#ifndef JTC_TYPES_H
#define JTC_TYPES_H

//
// Configuration stuff
//
#include "JTC/JTCConfig.h"

//
// If we're in a pthreads environment we want to define _REENTRANT
//
#if defined(HAVE_POSIX_THREADS) || defined(HAVE_DCE_THREADS)
#   ifndef _REENTRANT
#       define _REENTRANT
#   endif
#endif

// 
// Under HP/UX we want to define _CMA_NOWRAPPERS_ to stop read,
// write and other friends from being redefined to the cma_              
// versions.
//                
#if defined(__hpux)
#   include <errno.h>
#   ifndef _CMA_NOWRAPPERS_
#       define _CMA_NOWRAPPERS_
#   endif
#endif

//
// DLL support
//
#ifdef JTC_DLL
#   ifdef JTC_BUILD_DLL
#        define JTC_IMPORT /**/
#   else
#        define JTC_IMPORT __declspec(dllimport)
#   endif
#else
#   define JTC_IMPORT /**/
#endif

//
// Include common header files.
//
#ifndef HAVE_JTC_NO_IOSTREAM
#  ifdef HAVE_IOSTREAM
#     include <iostream>
#  else
#     include <iostream.h>
#  endif
#endif // !HAVE_JTC_NO_IOSTREAM

#include <stdio.h>
#include <string.h>

#ifndef HAVE_JTC_NO_IOSTREAM
#  ifdef HAVE_STD_IOSTREAM
#     define JTC_STD(x) std::x
#  else
#     define JTC_STD(x) x
#  endif
#endif // !HAVE_JTC_NO_IOSTREAM

//
// Typedef for bool type if bool is not supported.
//
#if SIZEOF_BOOL == 0
#   ifndef HAVE_BOOL_TYPEDEF
#       define HAVE_BOOL_TYPEDEF
typedef int bool;
#       if !defined(true)
const bool true = 1;
#       endif
#       if !defined(false)
const bool false = 0;
#       endif
#   endif
#endif

#if !defined(HAVE_POSIX_THREADS) && !defined(HAVE_DCE_THREADS) && \
    !defined(HAVE_WIN32_THREADS) && !defined(HAVE_VXWORKS_THREADS)
#   error Environment must support POSIX or WIN32 threads or vxWorks tasks.
#endif

#if defined(HAVE_POSIX_THREADS) || defined(HAVE_DCE_THREADS)

#   include <pthread.h>

//
// This class represents a thread id under POSIX threads, and DCE
// threads.
//
class JTCThreadId
{
public:
    //
    // Constuctor.  By default a thread id is a null thread id.
    //
    JTCThreadId()
    	: m_is_null_id(true)
    {
    }
    
    JTCThreadId(pthread_t id)
	: m_id(id), m_is_null_id(false)
    {
    }

    //
    // Use default copy constructor and assignment operator.
    //

    //
    // Equality and inequality.
    //
    bool
    operator!=(
	const JTCThreadId& rhs
    ) const
    {
	return !(*this == rhs);
    }
    
    bool
    operator==(
	const JTCThreadId& rhs
    ) const
    {
        //
        // Two thread ids that are not null should use pthread_equal.
        //
	if (!m_is_null_id && !rhs.m_is_null_id)
	{
	    return pthread_equal(m_id, rhs.m_id);
	}
        //
        // If they are both null then they are equal.
        //
	else if (m_is_null_id && rhs.m_is_null_id)
	{
	    return true;
	}
        //
        // Else one is null and the other isnt.  Therefore they are
        // not equal.
        
        //
	//
	// dont remove this else.  This works around a bug with
	// the SUN C++ 4.1 compiler.
	//
	else return false;
    }

    //
    // Return the id of the current thread.
    //
    static JTCThreadId
    self()
    {
	return JTCThreadId(pthread_self());
    }

    //
    // Conversion operator.
    //
    operator pthread_t() const
    {
	return m_id;
    }

    //
    // Copy the first len - 1 characters of the string representation of
    // the thread id to a buffer.  The buffer is returned.
    //
    char*
    toString(
	char* buf,
	size_t len
    ) const
    {
	char str[128];
#if defined(HAVE_POSIX_THREADS)
	sprintf(str, "%lu", (unsigned long) m_id);
#endif
#if defined(HAVE_DCE_THREADS)
	sprintf(str, "(%lu,%lu)", (unsigned long) m_id.field1,
		(unsigned long) m_id.field2);
#    if !defined(__alpha)
	sprintf(str, "(%lu,%lu,%lu)", (unsigned long) m_id.field1,
		(unsigned long) m_id.field2, (unsigned long) m_id.field3);
#    endif
#endif
	buf[len - 1] = '\0';
	return strncpy(buf, str, len - 1);
    }

#ifndef HAVE_JTC_NO_IOSTREAM

    friend JTC_STD(ostream)&
    operator<<(
	JTC_STD(ostream)&,
	const JTCThreadId&
    );

#endif // !HAVE_JTC_NO_IOSTREAM

private:

    pthread_t m_id; // The thread id.

    bool m_is_null_id; // Is this a null thread id?
};

#ifndef HAVE_JTC_NO_IOSTREAM

//
// IOstream insertion operator. This method is different for various
// thread packages since the type pthread_t is not specified.
// Therefore it may not be printable.
//
inline JTC_STD(ostream)&
operator<<(JTC_STD(ostream)& os, const JTCThreadId& id)
{
    if (id == JTCThreadId())
    {
        os << "NIL";
    }
    else
    {
#if defined(HAVE_POSIX_THREADS)
        os << id.m_id;
#endif
#if defined(HAVE_DCE_THREADS)
        os << "(" << id.m_id.field1
           << "," << id.m_id.field2
#    if !defined(__alpha)
           << "," << id.m_id.field3
#endif
           << ")";
#endif
    }
    return os;
}

#endif // !HAVE_JTC_NO_IOSTREAM

#endif

#if defined(HAVE_WIN32_THREADS)

//
// This class represents a thread id under WIN32.
//
class JTCThreadId
{
public:

    // This field should be private but VC++ 6 doesn't allow this.
    DWORD m_thread_id; // The thread id.

    //
    // Constructor.
    //
    JTCThreadId(DWORD thread_id = 0)
	: m_thread_id(thread_id)
    {
    }

    //
    // Use default copy constructor and assignment operator.
    //

    //
    // Equality and inequality.
    //
    bool operator==(
	const JTCThreadId& rhs
    ) const
    {
	return m_thread_id == rhs.m_thread_id;
    }
    
    bool operator!=(
	const JTCThreadId& rhs
    ) const
    {
	return !(*this == rhs);
    }

    //
    // Conversion operator.
    //
    operator DWORD() const
    {
    	return m_thread_id;
    }

    //
    // Copy the first len - 1 characters of the string representation of
    // the thread id to a buffer.  The buffer is returned.
    //
    char*
    toString(
	char* buf,
	size_t len
    ) const
    {
	char str[128];
	sprintf(str, "%lu", (unsigned long) m_thread_id);
	buf[len - 1] = '\0';
	return strncpy(buf, str, len - 1);
    }

    //
    // Return the id of the current thread.
    //
    static JTCThreadId 
    self()
    {
	return JTCThreadId(GetCurrentThreadId());
    }

//    friend ostream& operator<<(ostream&, const JTCThreadId&);
    friend class JTCThread;
};

#ifndef HAVE_JTC_NO_IOSTREAM

//
// IOstream insertion operator.
//
inline JTC_STD(ostream)&
operator<<(
    JTC_STD(ostream)& os,
    const JTCThreadId& id
)
{
    os << id.m_thread_id;
    return os;
}

#endif // !HAVE_JTC_NO_IOSTREAM

#endif

#if defined(HAVE_VXWORKS_THREADS)

#   include <taskLib.h>

//
// This class represents a thread id under vxWorks threads.
//
class JTCThreadId
{
public:
    //
    // Constuctor.  By default a thread id is a null thread id.
    //
    JTCThreadId()
    	: m_is_null_id(true)
    {
    }
    
    JTCThreadId(int id)
	: m_id(id), m_is_null_id(false)
    {
    }

    //
    // Use default copy constructor and assignment operator.
    //

    //
    // Equality and inequality.
    //
    bool operator==(
	const JTCThreadId& rhs
    ) const
    {
	return m_id == rhs.m_id;
    }

    bool
    operator!=(
	const JTCThreadId& rhs
    ) const
    {
	return !(*this == rhs);
    }
    
    //
    // Return the id of the current thread.
    //
    static JTCThreadId
    self()
    {
	return JTCThreadId(taskIdSelf());
    }

    //
    // Conversion operator.
    //
    operator int() const
    {
	return m_id;
    }

    //
    // Copy the first len - 1 characters of the string representation of
    // the thread id to a buffer.  The buffer is returned.
    //
    char*
    toString(
	char* buf,
	size_t len
    ) const
    {
	char str[128];
	sprintf(str, "%lu", (unsigned long) m_id);
	buf[len - 1] = '\0';
	return strncpy(buf, str, len - 1);
    }

#ifndef HAVE_JTC_NO_IOSTREAM

    friend JTC_STD(ostream)&
    operator<<(
	JTC_STD(ostream)&,
	const JTCThreadId&
    );

#endif // !HAVE_JTC_NO_IOSTREAM

private:

    int m_id; // The thread id.

    bool m_is_null_id; // Is this a null thread id?
};

#ifndef HAVE_JTC_NO_IOSTREAM

//
// IOstream insertion operator. This method is different for various
// thread packages since the type pthread_t is not specified.
// Therefore it may not be printable.
//
inline JTC_STD(ostream)&
operator<<(JTC_STD(ostream)& os, const JTCThreadId& id)
{
    if (id == JTCThreadId())
    {
        os << "NIL";
    }
    else
    {
        os << id.m_id;
    }
    return os;
}

#endif // !HAVE_JTC_NO_IOSTREAM

#endif

//
// The type JTCThreadKey represents a thread specific storage key.
//
#if defined(HAVE_POSIX_THREADS) || defined(HAVE_DCE_THREADS)
typedef pthread_key_t JTCThreadKey;
#endif

#if defined(HAVE_WIN32_THREADS)
typedef DWORD JTCThreadKey;
#endif

#if defined(HAVE_VXWORKS_THREADS)
typedef void** JTCThreadKey;
#endif

#endif
