#ifndef WORKDISPATCHER_H
#define WORKDISPATCHER_H

class WorkListener;

class WorkDispatcher {
public:
   /**
    *  Virtual destructor so that subclasses are deleted
    *  properly.
    */ 
   virtual ~WorkDispatcher() {}
   
   /**
    *   Schedules a work unit to be performed under the objects
    *   thread context. When the work has been completed, the
    *   listener object will be notified (on the main thread).
    *
    *  @param listener The listener that will receive a callback when
    *                  the work has been performed.
    *  @param data The data that the work will be performed upon.                
    */ 
   virtual void scheduleWork(WorkListener* listener, void* data) = 0;

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
   virtual bool cancelWork(WorkListener* listener, void* data) = 0;

};

#endif /* WORKDISPATCHER_H */
