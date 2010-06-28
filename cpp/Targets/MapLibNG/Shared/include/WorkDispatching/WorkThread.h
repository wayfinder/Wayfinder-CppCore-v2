#ifndef WORKTHREAD_H
#define WORKTHREAD_H

#include "Util/NotCopyable.h"
#include "WorkDispatching/WorkDispatcher.h"

class WorkPerformer;
class WorkListener;

/**
 *  Thread implementation of a work dispatcher.
 *
 *  This class will use an associated WorkPerformer to
 *  perform work under a different thread context than
 *  the main thread. When work has been completed, it
 *  will issue a callback on the main thread with the
 *  completed work. This is done via the WorkListener
 *  registered in the original request.
 */ 
class WorkThread : public NotCopyable,
                   public WorkDispatcher {
public:
   /**
    *   Creates a new work thread.
    *
    *   @param performer The performer that this class will use to
    *                    perform the scheduled work requests.
    *             
    *   @param sleepTimeMs Some threads will do a lot of heavy processing,
    *                      and in such cases this parameter can be used to
    *                      introduce a delay between jobs.
    *
    *   @param workPerIteration The number of work operations performed
    *                           per thread iteration. Like sleepTimeMs, this
    *                           can be tweaked to make the thread behave
    *                           more nicely.
    */ 
   WorkThread(WorkPerformer* performer,
              unsigned int sleepTimeMs,
              unsigned int workPerIteration);

   /**
    *   Destructor. Will halt the threads execution before
    *   deallocating its resources.
    */ 
   virtual ~WorkThread();

   /**
    *   Schedules a work unit to be performed under the objects
    *   thread context. When the work has been completed, the
    *   listener object will be notified (on the main thread).
    *
    *  @param listener The listener that will receive a callback when
    *                  the work has been performed.
    *  @param data The data that the work will be performed upon.                
    */ 
   void scheduleWork(WorkListener* listener, void* data);

   /**
    *   Attempts to cancel a previously scheduled work. It will
    *   lock the internal work mutex. This mutex might already be
    *   acquired and as such there is no guarantee that the requested
    *   cancellation will take effect (since the thread might be
    *   working on it already).
    *   
    *  @param listener The listener associated with the original request.
    *  @param data The data associated with the original request.
    *  @return True if the work was successfully cancelled.
    */ 
   bool cancelWork(WorkListener* listener, void* data);

   /**   
    *   Kills the currently running thread context. Will make
    *   the run loop to exit on its next run.
    */ 
   void kill();

   /**
    *   Returns true if this object is still alive.
    */ 
   bool isAlive() const;
private:
   /**
    *  Internal class used to track issued work requests.
    */ 
   class WorkLoad {
   public:
      WorkLoad() : workListener(),
                   data() {}
                
      WorkListener* workListener;
      void* data;
   };

   /**
    *   The run loop of the underlying thread. Will wait for
    *   scheduled requests via scheduleWork using a condition
    *   variable and is thus dormant while there is no available
    *   work.
    */ 
   void run();

   /**
    *   Method called by the main thread scheduler in order to
    *   notify registered listeners about work being completed.
    */  
   void notifyCallbacks();
   
   /**
    *   Callback function used by the main thread scheduler.
    *   When this is called, the work thread will notify the
    *   listeners that the work has been completed.
    */ 
   static void mainThreadCallback(void* data);

   /**
    *   Entry point for the underlying thread.  
    */ 
   static void* threadStartFunc(void *inData);
   
   struct Impl;
   Impl* m_impl;
};

#endif /* WORKTHREAD_H */
