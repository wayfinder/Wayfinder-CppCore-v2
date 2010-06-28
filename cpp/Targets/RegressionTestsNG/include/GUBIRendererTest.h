#ifndef GUBIRENDERERTEST_H
#define GUBIRENDERERTEST_H

#include "RegressionTest.h"

class GUBIRendererTest : public RegressionTest { 
public:
   
   /**
    * Add documentation here
    */
   GUBIRendererTest(Nav2Session& session, const char* name);
   
   virtual ~GUBIRendererTest();
   
   virtual void startTest();
   
private:
   struct Impl;
   Impl* m_impl;
};

#endif //GUBIRENDERERTEST_H
