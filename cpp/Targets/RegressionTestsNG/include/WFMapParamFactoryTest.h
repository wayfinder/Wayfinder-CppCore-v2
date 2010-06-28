#ifndef WFMAPPARAMFACTORYTEST_H
#define WFMAPPARAMFACTORYTEST_H

#include "RegressionTest.h"

class WFMapParamFactoryTest : public RegressionTest { 
public:
   
   /**
    * Add documentation here
    */
   WFMapParamFactoryTest(Nav2Session& session, const char* name);
   
   virtual ~WFMapParamFactoryTest();
   
   virtual void startTest();
   
private:
   struct Impl;
   Impl* m_impl;
};

#endif //WFMAPPARAMFACTORYTEST_H
