#ifndef MAPPARAMCANCELTEST_H
#define MAPPARAMCANCELTEST_H

#include "RegressionTest.h"

class MapParamCancelTest : public RegressionTest { 
public:
   
   /**
    * Add documentation here
    */
   MapParamCancelTest(Nav2Session& session, const char* name);
   
   virtual ~MapParamCancelTest();
   
   virtual void startTest();
   
private:
   struct Impl;
   Impl* m_impl;
};

#endif //MAPPARAMCANCELTEST_H
