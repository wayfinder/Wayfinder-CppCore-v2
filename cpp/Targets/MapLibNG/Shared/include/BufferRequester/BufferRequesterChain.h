#ifndef BUFFERREQUESTERCHAIN_H
#define BUFFERREQUESTERCHAIN_H

#include "PALMachineTypes.h"
#include "Util/NotCopyable.h"

class DBufRequester;
class MemCacheSettings;
class DiskCacheSettings;
class TileMapToolkit;
class HttpReqSettings;
class ResourceDirectorySettings;

namespace WFAPI {
class DBufConnection;
}

/**
 *  This class manages the hierarchy of caches.
 */ 
class BufferRequesterChain : public NotCopyable {
public:

   /**
    *  Constructor.
    *
    *  @param toolkit The toolkit.
    *  @param netConnection The network connection.
    *  @param memCacheSettings The memory cache settings.
    *  @param diskCacheSettings The disk cache settings.
    */
   BufferRequesterChain(TileMapToolkit* toolkit,
                        WFAPI::DBufConnection* netConnection,
                        const MemCacheSettings& memCacheSettings,
                        const DiskCacheSettings& diskCacheSettings,
                        const ResourceDirectorySettings& resDirSettings);

   /**
    *  Destructor.
    */
   virtual ~BufferRequesterChain();
   
   /**
    *  Adds a cache which handles a single file containing preloaded maps.
    */ 
   bool addPreloadedMapsCache(const char* fileName);

   /**
    *  Clears the memory cache of all cached buffers.
    */ 
   void clearMemoryCache();

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
    *   Updates the memory cache settings
    */ 
   void setResourceDirectorySettings(const ResourceDirectorySettings& resDirSettings);

   /**
    *   Updates the settings used when requesting from HTTP.
    */ 
   void setHttpReqSettings(const HttpReqSettings& reqSettings);
   
   /**
    *  @return The requester to use when requesting buffers.
    */ 
   DBufRequester* getRoot();

   /**
    *  Prints out the tree of connected DBufRequester
    */ 
   void printChain();
   
private:
   /**
    *  Removes requester req from chain. If req is NULL, return false.
    */ 
   bool removeRequester( DBufRequester* req );
      
   /** 
    *  Adds a disk-based cache which will be read and written to during runtime.
    */ 
   int addReadWriteCache(const char* path,
                         const char* uin,
                         WFAPI::wf_uint32 maxSize,
                         int read_only );

   DBufRequester* getRequesterOfExactType( pal::wf_uint32 typeMask );

   struct Impl;
   Impl* m_impl;
};

#endif /* BUFFERREQUESTERCHAIN_H */
