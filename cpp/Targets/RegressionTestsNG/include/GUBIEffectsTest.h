#ifndef GUBIEFFECTSTEST_H
#define GUBIEFFECTSTEST_H

#include "RegressionTest.h"

class GUBIEffectsTest : public RegressionTest { 
public:
   
   /**
    * Add documentation here
    */
   GUBIEffectsTest(Nav2Session& session, const char* name);
   
   virtual ~GUBIEffectsTest();
   
   virtual void startTest();
   
private:
   struct Impl;
   Impl* m_impl;
};

#endif //GUBIEFFECTSTEST_H
