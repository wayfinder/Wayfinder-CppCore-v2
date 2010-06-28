// **********************************************************************
//
// Copyright (c) 2001
// IONA Technologies, Inc.
// Waltham, MA, USA
//
// All Rights Reserved
//
// **********************************************************************

#ifndef JTC_TSS_H
#define JTC_TSS_H

#include "JTC/JTCTypes.h"

class JTCTSS
{
public:

    //
    // Allocate a new Thread Specific Key
    //
    static JTCThreadKey
    allocate();

    //
    // Allocate a new Thread Specific Key and a cleanup function.
    // When a thread is destroyed this cleanup function will be
    // called.
    //
    static JTCThreadKey
    allocate(
	void (*cleanup)(void*)
    );

    //
    // Release a Thread Specific Key
    //
    static void
    release(
	JTCThreadKey key
    );

    //
    // Get the per-thread data associated with the Thread Specific Key
    //
    static void*
    get(
	JTCThreadKey key
    );

    //
    // Set the per-thread data associated with the Thread Specific Key
    //
    static void
    set(
	JTCThreadKey key,
	void* value
    );
};

#endif
