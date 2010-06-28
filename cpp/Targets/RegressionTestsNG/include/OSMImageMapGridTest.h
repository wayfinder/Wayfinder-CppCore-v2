#ifndef OSMIMAGEMAPGRIDTEST_H
#define OSMIMAGEMAPGRIDTEST_H

#include "RegressionTest.h"

class OSMImageMapGridTest : public RegressionTest { 
public:
   
   /**
    * Add documentation here
    */
   OSMImageMapGridTest(Nav2Session& session, const char* name);
   
   virtual ~OSMImageMapGridTest();
   
   virtual void startTest();
   
private:
   struct Impl;
   Impl* m_impl;
};

#endif //OSMIMAGEMAPGRIDTEST_H
