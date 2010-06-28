#ifndef SAMPLEREGTEST_H
#define SAMPLEREGTEST_H

#include "RegressionTest.h"

class SampleRegTest : public RegressionTest { 
public:
   
   /**
    * Add documentation here
    */
   SampleRegTest(Nav2Session& session, const char* name);
   
   virtual ~SampleRegTest();
   
   virtual void startTest();
   
private:
   struct Impl;
   Impl* m_impl;
};

#endif //SAMPLEREGTEST_H
