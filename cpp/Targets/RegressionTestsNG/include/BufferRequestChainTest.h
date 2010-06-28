#ifndef BUFFERREQUESTCHAINTEST_H
#define BUFFERREQUESTCHAINTEST_H

#include "RegressionTest.h"
#include "BufferRequester/BufferRequesterListener.h"

class BufferRequester;

namespace WFAPI {
class MapLibNetworkContext;
}

class BufferRequestChainTest : public RegressionTest,
                               public BufferRequesterListener {
public:
   BufferRequestChainTest(Nav2Session& session, const char* name);
   virtual ~BufferRequestChainTest();

   void startTest();

   void requestReceived(const MC2SimpleString& descr,
                        BitBuffer* dataBuffer,
                        OriginType origin);
   
   void setArgImpl(const std::string& str);
private:
   enum StateType {
      UNINITIALIZED,
      MEM_CHECK_FROM_NET,
      MEM_CHECK_FROM_MEM,
      DISK_CHECK_FROM_NET,
      DISK_CHECK_FROM_DISK
   } m_state;
   
   BufferRequester* m_requester;
   WFAPI::MapLibNetworkContext* m_netContext;
};


#endif /* BUFFERREQUESTCHAINTEST_H */
