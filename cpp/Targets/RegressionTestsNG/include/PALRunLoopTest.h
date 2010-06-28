#ifndef PALRUNLOOPTEST_H
#define PALRUNLOOPTEST_H

#include "RegressionTest.h"

class PALRunLoopTest : public RegressionTest { 
public:
   
   /**
    * Add documentation here
    */
   PALRunLoopTest(Nav2Session& session, const char* name);
   
   virtual ~PALRunLoopTest();
   
   virtual void startTest();
   
private:
   void timerCallback(int id);
   
   static void timerCallbackProxy(unsigned int id, void* data);
   
   struct Impl;
   Impl* m_impl;
};

#endif //PALRUNLOOPTEST_H
