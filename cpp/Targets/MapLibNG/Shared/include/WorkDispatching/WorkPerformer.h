#ifndef WORKPERFORMER_H
#define WORKPERFORMER_H

/**
 *  This class is used by the WorkThread in order to
 *  abstract away the threading implementation from
 *  the algorithm implementation.
 *
 *  As such, a WorkThread can use any kind of performer
 *  to perform any kind of operation upon work items.
 *  Derive from this class to implement such behavior.
 */
class WorkPerformer {
public:
   /**
    *  Subclasses should implement this to perform
    *  the work. What type job represents is tied
    *  to the performer.
    *
    *  @param job The object to perform the operation upon.
    */ 
   virtual void performWork(void* job) = 0;
};

#endif /* WORKPERFORMER_H */
