#ifndef GUBIDEMOTEST_H
#define GUBIDEMOTEST_H

#include "RegressionTest.h"
#include "AppEventListener.h"

class GUBIDemoTest : public RegressionTest,
                     public AppEventListener { 
public:
   
   /**
    * Add documentation here
    */
   GUBIDemoTest(Nav2Session& session, const char* name);
   
   virtual ~GUBIDemoTest();
   
   virtual void startTest();

   virtual void gubiShouldBeDrawn();

   /**
    *  @see AppEventListener::onExitRequest
    */ 
   virtual void onExitRequest(int code);
private:
   struct Impl;
   Impl* m_impl;
};

#endif //GUBIDEMOTEST_H
