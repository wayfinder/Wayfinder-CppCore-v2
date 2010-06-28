#ifndef BUFFERRELEASEPOLICY_H
#define BUFFERRELEASEPOLICY_H

/**
 *   Class that is asked if it is ok to release the buffer to the
 *   next requester. Used for the traffic information maps that are
 *   not allowed to be stored anywhere else than in the
 *   MemoryDBufRequester.
 */
class BufferReleasePolicy {
public:
   /**
    *   Returns true if the buffer is allowed to be released
    *   from origin req to another cache. Otherwise, it should
    *   be destroyed.
    */   
   virtual bool releaseToNextAllowed( const MC2SimpleString& descr,
                                      const OriginType& req ) = 0;

   /**
    *  Destructor.
    */
   virtual ~BufferReleasePolicy() {}
};

#endif /* BUFFERRELEASEPOLICY_H */
