#ifndef VELOCITYTRACKER_H
#define VELOCITYTRACKER_H 

#include "Util/NotCopyable.h"

class PointerEvent;

/**
 *  Class that is used to determine the velocity of a sequence of pointer events.
 * 
 *  Inspiration is taken from Android's VelocityTracker.
 * 
 */ 
class VelocityTracker : public NotCopyable {
public:
   /**
    *  Constructor.
    *
    *  @param listener The GestureListener to supply detected gestures into.
    */
   VelocityTracker();

   /**
    *  Destructor.
    */
   virtual ~VelocityTracker();
   
   /**
    *  Add movement using a pointer event.
    *  
    *  @param event The pointer event to add.
    */
   void addMovement(const PointerEvent& event);

   /**
    *  Reset movement history.
    */
   void reset();

   /**
    *  Compute the current velocity.
    *
    *  @param velocityX Set to contain the velocity in the x-axis.
    *  @param velocityY Set to contain the velocity in the y-axis.
    */
   void getCurrentVelocity(float& velocityX, float& velocityY);
   
private:
  
   struct Impl;
   Impl* m_impl;

};


#endif /* VELOCITYTRACKER_H */
