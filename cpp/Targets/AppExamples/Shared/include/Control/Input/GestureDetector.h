#ifndef GESTUREDETECTOR_H
#define GESTUREDETECTOR_H

#include "Util/NotCopyable.h"

class PointerEvent;
class GestureListener;

/**
 *  Class that is used to detect gestures out of pointer events.
 * 
 */ 
class GestureDetector : public NotCopyable {
public:
   /**
    *  Constructor.
    *
    *  @param listener The GestureListener to supply detected gestures into.
    */
   GestureDetector(GestureListener* listener);

   /**
    *  Destructor.
    */
   virtual ~GestureDetector();

   /**
    *  @see WindowEventListener::onPointerDown
    */
   bool onPointerDown(const PointerEvent& event);

   /**
    *  @see WindowEventListener::onPointerMove
    */
   bool onPointerMove(const PointerEvent& event);
   
   /**
    *  @see WindowEventListener::onPointerUp
    */
   bool onPointerUp(const PointerEvent& event);

private:
  
   struct Impl;
   Impl* m_impl;

};


#endif /* GESTUREDETECTOR_H */
