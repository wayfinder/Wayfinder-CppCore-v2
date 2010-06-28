#ifndef GUBISIGNALTEST_H
#define GUBISIGNALTEST_H

#include "RegressionTest.h"

class GUBISignalTest : public RegressionTest { 
public:
   
   /**
    * Add documentation here
    */
   GUBISignalTest(Nav2Session& session, const char* name);
   
   virtual ~GUBISignalTest();
   
   virtual void startTest();
   
private:
   struct Impl;
   Impl* m_impl;
};

#endif //GUBISIGNALTEST_H
