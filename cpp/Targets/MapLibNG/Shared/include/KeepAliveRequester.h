#ifndef KEEPALIVEREQUESTER_H
#define KEEPALIVEREQUESTER_H

#include "TileMapToolkit.h"
#include <cstdio>

class KeepAliveRequester : public TileMapTimerListener,
                           public BufferRequesterListener {
public:
   KeepAliveRequester(TileMapToolkit* toolkit,
                      BufferRequester& req) :
      m_toolkit(toolkit),
      m_bufReq(req)
   {
      m_timer =      
         toolkit->createPeriodicTimer( this,
                                       TileMapToolkit::PRIO_STANDARD);
      m_timer->start(2000);
   }


   /**
    *  @see BufferRequesterListener.
    */
   void requestReceived(const MC2SimpleString& descr,
                        BitBuffer* dataBuffer,
                        OriginType origin)
   {
      printf("[KA_REQ]: Recv %p (size %u) / %s and origin %s\n",
             dataBuffer,
             dataBuffer ? dataBuffer->getBufferSize() : 0,
             descr.c_str(),
             OriginType::toString(origin));
      
      delete dataBuffer;
   }

   virtual void timerExpired(uint32 id) {
      printf("timerExpired()\n");
      
      m_bufReq.requestBuffer("", this);
   }

private:
   TileMapToolkit* m_toolkit;
   TileMapPeriodicTimer* m_timer;
   BufferRequester& m_bufReq;
};

#endif /* KEEPALIVEREQUESTER_H */
