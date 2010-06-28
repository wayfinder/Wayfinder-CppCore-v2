/*
Copyright (c) 1999 - 2010, Vodafone Group Services Ltd
All rights reserved.

Redistribution and use in source and binary forms, with or without modification, are permitted provided that the following conditions are met:

    * Redistributions of source code must retain the above copyright notice, this list of conditions and the following disclaimer.
    * Redistributions in binary form must reproduce the above copyright notice, this list of conditions and the following disclaimer in the documentation and/or other materials provided with the distribution.
    * Neither the name of the Vodafone Group Services Ltd nor the names of its contributors may be used to endorse or promote products derived from this software without specific prior written permission.

THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS" AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE ARE DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT HOLDER OR CONTRIBUTORS BE LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
*/


#ifndef _REGRESSIONTEST_H_
#define _REGRESSIONTEST_H_

class Nav2Session;

#include "TestException.h"
#include "WFString.h"
#include <string>

/**
 *  Base class for all regression tests. 
 */ 
class RegressionTest {
public:
   RegressionTest(const char* name,
                  unsigned int timeOutMs = 10000);
   
   /**
    * Deletes the regression test and frees all resources. Make sure
    * that your class deregister any active listeners.
    */ 
   virtual ~RegressionTest();

   /**
    * @return True if the test has been finished.
    */   
   virtual const bool& isTestFinished();
   
   /**
    * @return The name of the test.
    */ 
   const char* getTestName() {
      return m_name;
   }
   
   /**
    * Starts the test. The test will be finished when isTestFinished is
    * true.
    *
    * Since regression tests are typically run inside the GUI thread
    * context, you will need to "sleep" and let Nav2 propagate messages
    * upwards. The regression test logic will take place inside these
    * callback functions.
    *
    * This can be done using Environment::Wait(...).
    */ 
   virtual void startTest() = 0;

   /**
    * @return True if the test was finished successfully.
    */ 
   virtual bool testPassed();

   /**
    * @return The reason the test failed. 
    */ 
   const WFAPI::WFString& getFailedReason();

   /**
    * Returns the maximum allowed time this test can execute for.
    */
   unsigned int getTimeOutMs();

   /**
    * Sets the time out in milliseconds.
    */ 
   void setTimeOut(unsigned int timeOutMs);
   
   /**
    * Some tests can receive arguments from the invocation list. If
    * so they can process the argument(s) by implementing setArgImpl
    */ 
   void setArg(const std::string& arg);

   /**
    * Returns the registered string argument for this regression test.
    */ 
   const std::string& getArg();
protected:
   /**
    * Updates the test status to be finished and passed.
    */ 
   void setTestPassed();

   /**
    * Updates the test status to be finished and failed.
    */ 
   void setTestFailed(const WFAPI::WFString& failReason);
   
private:
   virtual void setArgImpl(const std::string& arg) {}
   
   /// Whether or not the test has finished;
   bool m_testFinished;
   bool m_testPassed;

   WFAPI::WFString m_failReason;
   const char* m_name;
   unsigned int m_timeOutMs;
   std::string m_arg;
   TestException m_exception;
};

#define rtAssertEq(x, y) if((x) != (y)) {             \
      std::cout << __FILE__ << ":" << __LINE__ << ":" << x << " != " << y << std::endl; \
   setTestFailed(#x "!=" #y);                                    \
   return; };

#define rtAssert(x) if(!(x)) { printf("%s:%d:%s\n", __FILE__, __LINE__, #x); \
      setTestFailed(#x); return; };

#endif /* _REGRESSIONTEST_H_ */
