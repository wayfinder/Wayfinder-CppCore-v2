#ifndef BUFFERREQUESTERTEST_H
#define BUFFERREQUESTERTEST_H

#include "RegressionTest.h"
#include "BufferRequester/BufferRequesterListener.h"

class BufferRequester;

namespace WFAPI {
class MapLibNetworkContext;
}

class BufferRequesterTest : public RegressionTest,
                            public BufferRequesterListener {
public:
   BufferRequesterTest(Nav2Session& session, const char* name);
   virtual ~BufferRequesterTest();
   
   virtual void startTest();

   virtual void requestReceived(const MC2SimpleString& descr,
                                BitBuffer* dataBuffer,
                                OriginType origin);

   virtual void setArgImpl(const std::string& str);
private:
   BufferRequester* m_requester;
   WFAPI::MapLibNetworkContext* m_netContext;
   std::string m_reqString;
   bool m_expectsValidResult;
   int m_count;
};

#endif /* BUFFERREQUESTERTEST_H */
