#ifndef VELOCITYTRACKERTEST_H
#define VELOCITYTRACKERTEST_H

#include "RegressionTest.h"

class VelocityTrackerTest : public RegressionTest { 
public:
   
   /**
    * Add documentation here
    */
   VelocityTrackerTest(Nav2Session& session, const char* name);
   
   virtual ~VelocityTrackerTest();
   
   virtual void startTest();
   
private:
   struct Impl;
   Impl* m_impl;
};

#endif //VELOCITYTRACKERTEST_H
