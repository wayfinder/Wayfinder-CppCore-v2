#ifndef PROJECTIONINTERFACETEST_H
#define PROJECTIONINTERFACETEST_H

#include "RegressionTest.h"

class ProjectionInterfaceTest : public RegressionTest { 
public:
   
   /**
    * Add documentation here
    */
   ProjectionInterfaceTest(Nav2Session& session, const char* name);
   
   virtual ~ProjectionInterfaceTest();
   
   virtual void startTest();
   
private:
   struct Impl;
   Impl* m_impl;
};

#endif //PROJECTIONINTERFACETEST_H
