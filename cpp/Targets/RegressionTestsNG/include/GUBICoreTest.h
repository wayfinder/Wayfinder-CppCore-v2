#ifndef GUBICORETEST_H
#define GUBICORETEST_H

#include "RegressionTest.h"

class GUBICoreTest : public RegressionTest { 
public:
   
   /**
    * Add documentation here
    */
   GUBICoreTest(Nav2Session& session, const char* name);
   
   virtual ~GUBICoreTest();
   
   virtual void startTest();
   
private:
   struct Impl;
   Impl* m_impl;
};

#endif //GUBICORETEST_H
