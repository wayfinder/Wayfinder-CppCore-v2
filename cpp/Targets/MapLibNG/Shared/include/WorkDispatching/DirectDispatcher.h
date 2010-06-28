#ifndef DIRECTDISPATCHER_H
#define DIRECTDISPATCHER_H

#include "WorkDispatching/WorkDispatcher.h"

class WorkPerformer;
class WorkListener;

class DirectDispatcher : public WorkDispatcher {
public:
   DirectDispatcher(WorkPerformer* performer);

   /**
    *  Will simply perform the job immediately.
    *  
    *  @see WorkDispatcher::scheduleWork
    */ 
   void scheduleWork(WorkListener* listener, void* data);

   /**
    *  Null-op which returns false, since the work was
    *  performed directly.
    */ 
   bool cancelWork(WorkListener* listener, void* data);
private:
   WorkPerformer* m_performer;
};


#endif /* DIRECTDISPATCHER_H */
