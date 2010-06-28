#ifndef OSMMAPFACTORYTEST_H
#define OSMMAPFACTORYTEST_H

#include "RegressionTest.h"

class OSMMapFactoryTest : public RegressionTest { 
public:
   
   /**
    * Add documentation here
    */
   OSMMapFactoryTest(Nav2Session& session, const char* name);
   
   virtual ~OSMMapFactoryTest();
   
   virtual void startTest();
   
private:
   void testWorldToGridSteps();
   void testWorldToGridWhole();

   void testGridToWorldSteps();
   void testGridToWorldWhole();
   
   struct Impl;
   Impl* m_impl;
};

#endif //OSMMAPFACTORYTEST_H
