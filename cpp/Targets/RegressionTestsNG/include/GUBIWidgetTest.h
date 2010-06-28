#ifndef GUBIWIDGETTEST_H
#define GUBIWIDGETTEST_H

#include "RegressionTest.h"

class GUBIWidgetTest : public RegressionTest { 
public:
   
   /**
    * Add documentation here
    */
   GUBIWidgetTest(Nav2Session& session, const char* name);
   
   virtual ~GUBIWidgetTest();
   
   virtual void startTest();
   
private:
   struct Impl;
   Impl* m_impl;
};

#endif //GUBIWIDGETTEST_H
