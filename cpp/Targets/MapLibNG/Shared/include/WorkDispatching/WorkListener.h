#ifndef WORKLISTENER_H
#define WORKLISTENER_H

/**
 *  Class used by WorkThread that ties an issued work
 *  request together with the original requester. 
 */ 
class WorkListener {
public:
   /**
    *  Will be called when the work thread has completed
    *  work on a particular work item. This call will always
    *  come from the main thread context and not that of
    *  the work thread.
    */ 
   virtual void onWorkDone(void* work) = 0;
};

#endif /* WORKLISTENER_H */
