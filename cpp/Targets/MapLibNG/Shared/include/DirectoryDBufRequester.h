#ifndef DIRECTORYDBUFREQUESTER_H
#define DIRECTORYDBUFREQUESTER_H

#include "Util/NotCopyable.h"
#include "DBufRequester.h"
#include "TileMapToolkit.h"

/**
 *  This class uses the directory structure of the
 *  underlying file system as the cache mechanism.
 *  Buffers are directly stored as files.
 */ 
class DirectoryDBufRequester : public DBufRequester,
                               public NotCopyable,
                               public TileMapIdleListener {
public:
   DirectoryDBufRequester(const char* path,
                          bool writeable,
                          TileMapToolkit* toolkit);

   virtual ~DirectoryDBufRequester();

   /**
    *  @see DBufRequester::toString
    */ 
   virtual const char* toString() {
      return "DirectoryDBufRequester";
   }

   
   /**
    *  @see DBufRequester::request
    */ 
   virtual void request(const MC2SimpleString& descr,
                        DBufRequestListener* caller,
                        CachePolicy whereFrom );

   /**
    *  @see DBufRequester::cancelRequest
    */ 
   virtual void cancelRequest(const MC2SimpleString& descr,
                              DBufRequestListener* caller);

   void dealWithLoadedBuffers();
   
   /**
    *   Called when the system is a bit idle.
    */
   void runIdleTask(uint32 id);
private:

   struct Impl;
   Impl* m_impl;
};

#endif /* DIRECTORYDBUFREQUESTER_H */
