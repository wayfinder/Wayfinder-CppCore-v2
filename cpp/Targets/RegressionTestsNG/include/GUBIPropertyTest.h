#ifndef GUBIPROPERTYTEST_H
#define GUBIPROPERTYTEST_H

#include "RegressionTest.h"

class GUBIPropertyTest : public RegressionTest { 
public:
   
   /**
    * Add documentation here
    */
   GUBIPropertyTest(Nav2Session& session, const char* name);
   
   virtual ~GUBIPropertyTest();
   
   virtual void startTest();
   
private:
   struct Impl;
   Impl* m_impl;
};

#endif //GUBIPROPERTYTEST_H
