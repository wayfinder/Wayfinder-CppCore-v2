#ifndef NETWORKREQTEST_H
#define NETWORKREQTEST_H

#include "RegressionTest.h"
#include "DBufRequester.h"
#include "Util/NotCopyable.h"

class NetworkReqTest : public RegressionTest,
                       public NotCopyable,
                       private DBufRequestListener { 
public:
   
   /**
    * Add documentation here
    */
   NetworkReqTest(Nav2Session& session, const char* name);
   
   virtual ~NetworkReqTest();
   
   virtual void startTest();

   virtual void requestReceived(const MC2SimpleString& descr,
                                BitBuffer* dataBuffer,
                                const DBufRequester& origin );
private:
   struct Impl;
   Impl* m_impl;
};

#endif //NETWORKREQTEST_H
