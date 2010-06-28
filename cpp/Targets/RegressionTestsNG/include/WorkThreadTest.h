#ifndef WORKTHREADTEST_H
#define WORKTHREADTEST_H

#include "RegressionTest.h"
#include "WorkDispatching/WorkListener.h"

class WorkThreadTest : public RegressionTest, public WorkListener { 
public:
   
   /**
    * Add documentation here
    */
   WorkThreadTest(Nav2Session& session, const char* name);
   
   virtual ~WorkThreadTest();
   
   virtual void startTest();

   void onWorkDone(void* work);
   
private:
   struct Impl;
   Impl* m_impl;
};

#endif //WORKTHREADTEST_H
