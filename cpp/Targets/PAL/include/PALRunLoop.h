#ifndef PALRUNLOOP_H
#define PALRUNLOOP_H

#include "PAL.h"

/**
 *   This module contains functionality that allows
 *   you to schedule and perform iterations on the
 *   run loop of the GUI thread.
 */ 
namespace pal {

struct RunLoopContext;

typedef void(*pal_run_loop_func)(unsigned int,void*);

/**
 *  Creates a new run loop context from which things
 *  can be scheduled.
 */
pstatus createRunLoopContext(RunLoopContext** context);

/**
 *  Destroys a previously created run loop context. Will
 *  cancel all pending timers.
 */ 
pstatus destroyRunLoopContext(RunLoopContext* context);

/**
 *  Performs a single iteration of the run loop.
 */  
pstatus performIteration();

/**
 *  Requests a new timer to be expired on the main thread approximately
 *  timeOutMs milliseconds from now. Returns a unique integer representing
 *  the timer request.
 *
 *  When the timer expires, func shall be called with the unique id as its
 *  first argument and data as its second argument.
 *
 *  @param context The context that will initiate the timer
 *  @param timeOutMs The number of milliseconds before the timer should
 *                   trigger.
 *  @param func The function to invoke when the timer is triggered.
 *  @param data The data to pass to the invoked function.
 *  @param id Will be set to the unique id representing the timer request.
 */ 
pstatus requestTimer(RunLoopContext* context,
                     unsigned int timeOutMs,
                     pal_run_loop_func func,
                     void* data,
                     unsigned int* id);

/**
 *  Cancels a previously requested timer.
 *
 *  @param context The context that initiated the timer.
 *  @param timerId The ID returned from the timer request.
 *
 *  @return PAL_ERR_INVALID_TIMER if the timer id is invalid
 *          PAL_ERR_EXPIRED_TIMER if timer has already expired
 */ 
pstatus cancelTimer(RunLoopContext* context,
                    unsigned int timerId);

}

#endif /* PALRUNLOOP_H */
