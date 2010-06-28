#ifndef MERCATORMATHTESTS_H
#define MERCATORMATHTESTS_H

#include "RegressionTest.h"

class MercatorMathTests : public RegressionTest {
public:
   MercatorMathTests(Nav2Session& session, const char* name) :
      RegressionTest(name)
      {}

   virtual void startTest();
private:
};

#endif /* MERCATORMATHTESTS_H */
