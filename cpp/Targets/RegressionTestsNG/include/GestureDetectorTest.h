#ifndef GESTUREDETECTORTEST_H
#define GESTUREDETECTORTEST_H

#include "RegressionTest.h"
#include "Control/Input/GestureListener.h"

class PointerEvent;

class GestureDetectorTest : public RegressionTest, public GestureListener { 
public:
   
   /**
    * Add documentation here
    */
   GestureDetectorTest(Nav2Session& session, const char* name);
   
   virtual ~GestureDetectorTest();
   
   virtual void startTest();

   /**
    *  @see GestureListener::onFling
    */
   virtual bool onFling(const PointerEvent& event, 
                        float velocityX,
                        float velocityY);

   
private:
   struct Impl;
   Impl* m_impl;
};

#endif //GESTUREDETECTORTEST_H
