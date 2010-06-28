// **********************************************************************
//
// Copyright (c) 2001
// IONA Technologies, Inc.
// Waltham, MA, USA
//
// All Rights Reserved
//
// **********************************************************************

#ifndef JTC_TSS_MANAGER_H
#define JTC_TSS_MANAGER_H

#include "JTC/JTCMutex.h"

//
// This class is needed under WIN32 since there are no native support
// for thread specific data cleanup. Although POSIX threads have
// support for associating destructor functions with a thread key they
// cannot be used. This is because the destructor functions have to
// run before the thread terminates since any thread joins will occur
// before the thread terminates.
//
class JTCTSSManager
{
public:

    JTCTSSManager();
    ~JTCTSSManager();

    //
    // Get the singleton
    //
    static JTCTSSManager*
    instance();

    //
    // Associate a key and a cleanup function
    //
    void
    allocate(
	JTCThreadKey key,
	void(*)(void*)
    );
    
    //
    // Remove the key from our internal table
    //
    void
    release(
	JTCThreadKey key
    );

    //
    // Run the cleanup functions
    //
    void
    cleanup();

private:

    struct KeyCleanup
    {
        JTCThreadKey key;
        void (*release)(void*);

        KeyCleanup* next;
    };

    KeyCleanup* m_head; // Head of the cleanup queue
    JTCMutex m_mutex; // Mutex for the cleanup queue

    static JTCTSSManager* m_instance;
};

#endif
