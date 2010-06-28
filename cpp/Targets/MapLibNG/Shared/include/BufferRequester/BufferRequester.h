#ifndef BUFFERREQUESTER_H
#define BUFFERREQUESTER_H

#include "DBufRequester.h"
#include "Util/NotCopyable.h"

namespace WFAPI {
class DBufConnection;
} // End of namespace WFAPI

// Forward declares
class ResourceDirectorySettings;
class MemCacheSettings;
class DiskCacheSettings;
class HttpReqSettings;
class TileMapToolkit;
class BufferRequesterListener;
class BufferReleasePolicy;
struct CachePolicy;

/**
 *  Class encapsulating functionality for fetching and caching buffers.
 */
class BufferRequester : private DBufRequestListener,
                        public NotCopyable {
public:
   /**
    *  Constructor.
    *
    *  @param toolkit The toolkit.
    *  @param netConnection The network connection.
    *  @param memCacheSettings The memory cache settings.
    *  @param diskCacheSettings The disk cache settings.
    */
   BufferRequester(TileMapToolkit* toolkit,
                   WFAPI::DBufConnection* netConnection,
                   const MemCacheSettings& memCacheSettings,
                   const DiskCacheSettings& diskCacheSettings,
                   const ResourceDirectorySettings& resDirSettings);
   
   /**
    *  Destructor.
    */
   virtual ~BufferRequester();

public:
   /**
    *  Requests a buffer matching the description.
    */ 
   void requestBuffer(const MC2SimpleString& descr,
                      BufferRequesterListener* callback,
                      const CachePolicy& whereFrom = CachePolicy::cacheOrInternet);

   /**
    *  Attempts to cancel a previously issued request.
    */
   void cancelRequest(const MC2SimpleString& descr,
                      BufferRequesterListener* callback);
   
   /**
    *  The listener that will be notified when there is no
    *  recipient for a received request. I.e. server issued
    *  TMFDs.
    */ 
   void setDefaultListener(BufferRequesterListener* callback);

   /**
    *  Sets the buffer release policy to use.
    *  
    *  @see BufferReleasePolicy for further details
    */ 
   void setBufferReleasePolicy(BufferReleasePolicy* policy);
   
   /**
    *  When you are done with a requested object, you should
    *  always release it. This will allow the buffer to be
    *  cached or deleted depending upon various factors.
    */ 
   void release(const MC2SimpleString& descr, BitBuffer* obj);

   /**
    *   Cancels all outstanding requests.
    */ 
   void cancelAll();

   /**
    *   Clears the read write cache of all cached buffers.
    */ 
   void clearReadWriteCache();

   /**
    *   Updates the disk cache settings. 
    */ 
   void setDiskCacheSettings(const DiskCacheSettings& diskCacheSettings);

   /**
    *   Updates the memory cache settings
    */ 
   void setMemCacheSettings(const MemCacheSettings& memCacheSettings);

   /**
    *   Updates the HTTP request settings
    */ 
   void setHttpReqSettings(const HttpReqSettings& reqSettings);
   
   /**
    *   Prints the chain of requesters.
    */ 
   void printChain();

   /**
    *   Enables/disables request amount limit.
    *   
    *   TODO: Should this ever be not enabled?
    */ 
   void setReqAmountLimitEnabled(bool enabled);
private:
   /**
    * @see DBufRequestListener::requestReceived
    */ 
   void requestReceived(const MC2SimpleString& descr,
                        BitBuffer* dataBuffer,
                        const DBufRequester& origin);

   /**
    *  Get the root requester, i.e. the first requester
    *  that should be tried when fetching buffers.
    */
   DBufRequester* getRoot();

   struct Impl;
   Impl* m_impl;
};

#endif /* BUFFERREQUESTER_H */
 
