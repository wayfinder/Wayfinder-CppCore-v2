#ifndef BUFFERREQUESTERLISTENER_H
#define BUFFERREQUESTERLISTENER_H

class MC2SimpleString;
class BitBuffer;
class DBufRequester;

#include "BufferRequester/OriginType.h"

class BufferRequesterListener {
public:
   /**
    *  Used to notify when an incoming request has been
    *  received.
    *
    *  @param descr The parameter descr for the buffer.
    *  @param buffer The buffer
    *  @param origin The origin source of the buffer.
    */ 
   virtual void requestReceived(const MC2SimpleString& descr,
                                BitBuffer* dataBuffer,
                                OriginType origin) = 0;
   
   /**
    *  Destructor.
    */
   virtual ~BufferRequesterListener() {}
};

#endif /* BUFFERREQUESTERLISTENER_H */
