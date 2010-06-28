#ifndef FMATHTEST_H
#define FMATHTEST_H

#include "RegressionTest.h"

class FMathTest : public RegressionTest { 
public:
   
   /**
    * Add documentation here
    */
   FMathTest(Nav2Session& session, const char* name);
   
   virtual ~FMathTest();
   
   virtual void startTest();
   
private:
   struct Impl;
   Impl* m_impl;
};

#endif //FMATHTEST_H
