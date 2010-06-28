// **********************************************************************
//
// Copyright (c) 2001
// IONA Technologies, Inc.
// Waltham, MA, USA
//
// All Rights Reserved
//
// **********************************************************************

#ifndef JTC_SYNC_T_H
#define JTC_SYNC_T_H

//
// A template version of the JTCSynchronized class. It is recommended
// to use this class for the most efficient implementation. However,
// the JTCSynchronized class below is easier to use.
//
template <class T>
class JTCSyncT
{
public:

    JTCSyncT(const T& mutex)
	: m_mutex(mutex)
    {
	m_mutex.lock();
    }

    ~JTCSyncT()
    {
	m_mutex.unlock();
    }

private:
    
    const T& m_mutex;
};

#endif
