#ifndef GUBICOMMONTEST_H
#define GUBICOMMONTEST_H

#include "RegressionTest.h"

class GUBICommonTest : public RegressionTest { 
public:
   
   /**
    * Add documentation here
    */
   GUBICommonTest(Nav2Session& session, const char* name);
   
   virtual ~GUBICommonTest();
   
   virtual void startTest();
   
private:
   struct Impl;
   Impl* m_impl;
};

#endif //GUBICOMMONTEST_H
