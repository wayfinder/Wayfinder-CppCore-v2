#define CORE_LOGGING_MODULE_NAME "PalRunLoop-iphone"
#define CORE_LOGGING_MODULE_LEVEL CL_ALL_FILE
#include "CoreDebugPrint.h"

#include "PALRunLoop.h"
#include <CoreFoundation/CFRunLoop.h>
#include <map>
#import <Foundation/Foundation.h>
#import <UIKit/UIKit.h>

using namespace pal;

namespace pal {
   struct CallbackData;
}

/******************************************************************************
 *
 * One-shot timer Objective-C class - Interface part.
 *
 *****************************************************************************/
@interface IPTimer : NSObject
{
@private
   NSTimer* timer;
   CallbackData* callbackData;
   unsigned int timerID;
}

@property (nonatomic, readonly) NSTimer* timer;
@property (nonatomic, readonly) unsigned int timerID;
@property (nonatomic, readonly) CallbackData* callbackData;


- (id)initWithListener:(CallbackData *)aCallbackData
               timeout:(unsigned int)timeout
               timerID:(unsigned int)timerID;
- (void)timerCallback:(NSTimer*)timer;
- (void)stop;
@end

namespace pal {

   /**
    *  Holds the necessary data so that the callback
    *  function can be properly invoked when the timer
    *  is triggered.
    */ 
   struct CallbackData {
   
      // The function to call
      pal_run_loop_func func;
   
      // The associated id of the timer event
      int id;
   
      // The user-supplied data
      void* data;
   
      // The context which initiated the callback
      RunLoopContext* context;
   };


typedef std::map<unsigned int, IPTimer*> TimerMap;

/**
 *  Represents a run loop context which has information about
 *  outstanding timers and other state data.
 */
struct RunLoopContext {
   // The variable used to increment unique ID:s for each request
   unsigned int timer_count;
   
   // The collection of outstanding timers that are not yet fired
   TimerMap timers;
};

pstatus cancelTimer(TimerMap& timers, TimerMap::iterator itr) {
   if (itr != timers.end()) {
      delete itr->second->callbackData;
      [itr->second dealloc];

      return PAL_OK;
   } else {
      return PAL_ERR_EXPIRED_TIMER;      
   }   
}

}
      
/******************************************************************************
 *
 * One-shot timer Objective-C class - Implementation part.
 *
 *****************************************************************************/
@implementation IPTimer
@synthesize timer, timerID, callbackData;
   
- (id)initWithListener:(CallbackData*)aCallbackData
               timeout:(unsigned int)timeout
               timerID:(unsigned int)aTimerID
{
   if (self = [super init])
   {
      callbackData = aCallbackData;
      timerID = aTimerID;
      
      // Note that the timer is not allocated. The run loop will retain it
      // and release it once it is either expired or when we call invalidate.
      timer = [NSTimer
                 scheduledTimerWithTimeInterval:(double)(timeout / 1000.0)
                 target:self
                 selector:@selector(timerCallback:)
                 userInfo:nil
                 repeats:NO];
   }
   return self;
}


- (void)dealloc 
{
   // coreprintln("IPTimer: dealloc for timer %d", timerID);
	[self stop];
	[super dealloc];
}

 
- (void)timerCallback:(NSTimer*)aTimer
{
	// coreprintln("timerCallback for timer: %d", timerID);

   // Intentionally not calling stop, since an expired timer that does not repeat
   // will be invalidated automatically.
	timer = nil;

   callbackData->func(callbackData->id, callbackData->data);   
   delete callbackData;
   
   TimerMap& timers(callbackData->context->timers);

   TimerMap::iterator itr = timers.find(callbackData->id);

   timers.erase(itr);
   // Note that the system should delete the actual timer..(?)
}

- (void)stop
{
   coreprintln("IPTimer: stop for timer %d", timerID);
	if([timer isValid]){
		// Only invalidate if the timer has not already been invalidated.
		coreprintln("IPTimer::stop - invalidating and setting to nil");
		[timer invalidate];
		timer = nil;
	}
}
@end

namespace pal {
   
pstatus performIteration()
{
   SInt32 res = CFRunLoopRunInMode(kCFRunLoopDefaultMode, 0.1, false);

   switch(res) {
   case kCFRunLoopRunFinished:
      // coreprintln("The run loop mode mode has no sources or timers.");
      break;
   case kCFRunLoopRunStopped:
      // coreprintln("The run loop was stopped with CFRunLoopStop.");
      break;
   case kCFRunLoopRunTimedOut:
      // coreprintln("The time interval seconds passed.");
      break;
   case kCFRunLoopRunHandledSource:
      break;
   }

   return PAL_OK;
}

pstatus requestTimer(RunLoopContext* context,
                     unsigned int timeOutMs,
                     pal_run_loop_func func,
                     void* data,
                     unsigned int* id)
{
   // printf("Requesting timer\n");
   unsigned int timerId = ++context->timer_count;

   CallbackData *info = new CallbackData;
   info->context = context;
   info->id = timerId;
   info->data = data;
   info->func = func;

   IPTimer* newTimer = [[IPTimer alloc] initWithListener:info
                                        timeout:timeOutMs
                                        timerID:timerId];

   context->timers[timerId] = newTimer;
   *id = timerId;
   
   return PAL_OK;
}

pstatus createRunLoopContext(RunLoopContext** context)
{
   *context = new RunLoopContext;
   (*context)->timer_count = 0;
   
   return PAL_OK;
}



pstatus cancelTimer( RunLoopContext* context,
                     unsigned int timerId )
{
   if(timerId > context->timer_count) {
      return PAL_ERR_INVALID_TIMER;
   }
   
   TimerMap& timers(context->timers);

   TimerMap::iterator itr = timers.find(timerId);

   pstatus ret = cancelTimer(timers, itr);
   
   if(ret == PAL_OK) {
      timers.erase(itr);
   }

   return ret;
}

pstatus destroyRunLoopContext(RunLoopContext* context)
{
   for(TimerMap::iterator itr = context->timers.begin();
       itr != context->timers.end();
       itr++)
   {
      cancelTimer(context->timers, itr);
   }
   
   delete context;
   return PAL_OK;
}















} // End of namespace pal
