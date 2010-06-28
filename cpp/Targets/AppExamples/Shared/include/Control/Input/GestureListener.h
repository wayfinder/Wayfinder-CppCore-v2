#ifndef GESTURELISTENER_H
#define GESTURELISTENER_H

#include "Util/NotCopyable.h"

class PointerEvent;

/**
 *  Abstract listener class that approriate method is called
 *  by Gesturelistener when a gesture has been detected.
 */
class GestureListener {
public:
   /**
    *  Destructor
    */
   virtual ~GestureListener() {}

   /**
    *  Method that should be called when a fling (flicking motion)
    *  has been detected.
    *
    *  @param event The current pointer event.
    *  @param velocityX The velocity in pixels per second in the x-direction.
    *  @param velocityY The velocity in pixels per second in the y-direction.
    *  @return If the event was handled.
    */
   virtual bool onFling(const PointerEvent& event, 
                        float velocityX,
                        float velocityY) = 0;

};

#endif /* GESTURELISTENER_H */
