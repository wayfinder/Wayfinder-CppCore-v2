#ifndef GUBIVIEWTEST_H
#define GUBIVIEWTEST_H

#include "RegressionTest.h"

class GUBIViewTest : public RegressionTest { 
public:
   
   /**
    * Add documentation here
    */
   GUBIViewTest(Nav2Session& session, const char* name);
   
   virtual ~GUBIViewTest();
   
   virtual void startTest();
   
private:
   struct Impl;
   Impl* m_impl;
};

#endif //GUBIVIEWTEST_H
