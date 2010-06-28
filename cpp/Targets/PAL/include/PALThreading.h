/*
Copyright (c) 1999 - 2010, Vodafone Group Services Ltd
All rights reserved.

Redistribution and use in source and binary forms, with or without modification, are permitted provided that the following conditions are met:

    * Redistributions of source code must retain the above copyright notice, this list of conditions and the following disclaimer.
    * Redistributions in binary form must reproduce the above copyright notice, this list of conditions and the following disclaimer in the documentation and/or other materials provided with the distribution.
    * Neither the name of the Vodafone Group Services Ltd nor the names of its contributors may be used to endorse or promote products derived from this software without specific prior written permission.

THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS" AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE ARE DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT HOLDER OR CONTRIBUTORS BE LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
*/


#ifndef PALTHREADING_H
#define PALTHREADING_H

#include "PAL.h"
#include "PALTypes.h"

/**
 * This file contains declarations for functions dealing
 * with threading related operations.
 */
namespace pal {

//a helper typedef to make function pointing a bit easier to use
typedef void(*ThreadStartFunc)(void*);

typedef void* Thread;
typedef void* Mutex;
typedef void* RecursiveMutex;
typedef void* Conditional;

/**
 * Initialize the thread system. This needs to be called before doing any thread related
 * work.
 */
pstatus initThread();

/**
 * Deinitialize the thread system. This needs to be called before exiting the program, or it
 * will leak.
 */
pstatus deinitThread();
   
/**
 * Create a new thread.
 * 
 * @param function A function pointer for the thread to start in
 * @param data A parameter containing data to be passed as an argument to above
 *        function
 * @param name The name of the thread.
 * 
 * @return A Thread, or NULL if no thread could be created.
 */
Thread createThread(ThreadStartFunc function, void* data, const char* name = NULL);

/**
 * Start a thread.
 * 
 * @param thread The thread to start
 * 
 * @return The thread starting status
 */
pstatus startThread(Thread thread);

/**
 * Destroy a thread, as in remove the resources it occupies. Cannot be used to
 * stop a running thread.
 * 
 * @param thread The thread to destroy
 * 
 * @return The thread destruction status
 */
pstatus destroyThread(Thread thread);

/**
 * Get the currently executing thread.
 *
 * @return A pointer to the thread currently executing, or 0 when executed by the main thread.
 */
Thread currentThread();

/**
 * Get the name of a thread
 *
 * @param thread the thread to get the name from
 *
 * @return the name of the thread
 */
const char* getThreadName(Thread thread);

/**
 * Get the ID of the current thread
 *
 * @return the Thread ID
 */
wf_int32 getCurrentThreadID();

/**
 * Create a mutex.
 * 
 * @return The newly created mutex
 */
Mutex createMutex();

/**
 * Destroy a mutex.
 *
 * @param Mutex The mutex to destroy
 *
 * @return The Mutex destruction status
 */
pstatus destroyMutex(Mutex mutex);

/**
 * Lock a mutex.
 * 
 * @param thread The mutex to lock
 * 
 * @return The mutex lock status
 */
bool lockMutex(Mutex mutex);

/**
 * Unlock a mutex.
 * 
 * @param thread The mutex to unlock
 * 
 * @return The mutex unlock status
 */
bool unlockMutex(Mutex mutex);

/**
 * Trylock a mutex.
 * 
 * @param thread The mutex to trylock
 * 
 * @return The mutex trylock status
 */
bool tryLockMutex(Mutex mutex);

/**
 * Create a recursive mutex.
 * 
 * @return The newly created recursive mutex
 */
RecursiveMutex createRecursiveMutex();

/**
 * Destroy a recursive mutex.
 *
 * @param recursiveMutex The recursive mutex to destroy
 *
 * @return The recursiveMutex destruction status
 */
pstatus destroyRecursiveMutex(RecursiveMutex recursiveMutex);

/**
 * Lock a recursive mutex.
 * 
 * @param thread The recursive mutex to lock
 * 
 * @return The recursiveMutex lock status
 */
bool lockRecursiveMutex(RecursiveMutex recursiveMutex);

/**
 * Unlock a recursive mutex.
 * 
 * @param thread The recursive mutex to unlock
 * 
 * @return The recursiveMutex unlock status
 */
bool unlockRecursiveMutex(RecursiveMutex recursiveMutex);

/**
 * Trylock a recursive mutex.
 * 
 * @param thread The recursive mutex to trylock
 * 
 * @return The recursiveMutex trylock status
 */
bool tryLockRecursiveMutex(RecursiveMutex recursiveMutex);

/**
 * Create a new conditional variable.
 * 
 * @return A Conditional, or NULL if no conditional could be created.
 */
Conditional createConditional();

/**
 * Destroy a conditional.
 * 
 * @param conditional The conditional to destroy
 * 
 * @return The conditional destruction status
 */
pstatus destroyConditional(Conditional conditional);

/**
 * Wait on a conditional. This will unlock the mutex before going into wait
 * mode, and locks the mutex when leaving wait mode.
 * 
 * @param conditional the conditional variable to use for waiting
 * @param mutex the mutex to use for locking associated with the this
 *              conditional waiting
 * 
 * @return always returns PAL_OK
 */
pstatus conditionalWait(Conditional conditional, Mutex mutex);

/**
 * Wait on a conditional. This will unlock the mutex before going into wait
 * mode, and locks the mutex when leaving wait mode.
 * The conditional is woken up by either signal, broadcast or the timeout
 * milliseconds.
 * 
 * @param conditional the conditional variable to use for waiting
 * @param mutex the mutex to use for locking associated with the this
 *              conditional waiting
 * @param timeout the amount of milliseconds to wait before waking up if no
 *                signal or broadcast has arrived.
 * 
 * @return PAL_OK if the conditional woke up from a signal or broadcast,
 *         PAL_CONDITIONAL_WOKE_UP_FROM_TIMEOUT if waking up from timeout
 */
pstatus conditionalWait(Conditional conditional, Mutex mutex, wf_uint32 timeout);

/**
 * Wait on a conditional. This will unlock the recursive mutex before going into wait
 * mode, and locks the recursive mutex when leaving wait mode.
 * 
 * @param conditional the conditional variable to use for waiting
 * @param recursiveMutex the recursiveMutex to use for locking associated with the this
 *              conditional waiting
 * 
 * @return always returns PAL_OK
 */
pstatus conditionalWaitRecursive(Conditional conditional, RecursiveMutex recursiveMutex);

/**
 * Wait on a conditional. This will unlock the recursive mutex before going into wait
 * mode, and locks the recursive mutex when leaving wait mode.
 * The conditional is woken up by either signal, broadcast or the timeout
 * milliseconds.
 * 
 * @param conditional the conditional variable to use for waiting
 * @param recursiveMutex the recursiveMutex to use for locking associated with the this
 *              conditional waiting
 * @param timeout the amount of milliseconds to wait before waking up if no
 *                signal or broadcast has arrived.
 * 
 * @return PAL_OK if the conditional woke up from a signal or broadcast,
 *         PAL_CONDITIONAL_WOKE_UP_FROM_TIMEOUT if waking up from timeout
 */
pstatus conditionalWaitRecursive(Conditional conditional, RecursiveMutex recursiveMutex, wf_uint32 timeout);

/**
 * Send a signal to a conditional, telling it to wake up
 *
 * @param conditional the conditional variable to wake up
 *
 * @return always returns PAL_OK
 */
pstatus conditionalSignal(Conditional conditional);

/**
 * Send a broadcast to a conditional, telling all threads waiting on it it to wake up
 *
 * @param conditional the conditional variable to wake up
 *
 * @return always returns PAL_OK
 */
pstatus conditionalBroadcast(Conditional conditional);

/**
 * Sleep the current thread
 * 
 * @param usecond The amount of milliseconds to sleep
 */
pstatus threadSleep(int mseconds);

} // End of namespace pal

#endif /* PALTHREADING_H */
