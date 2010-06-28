#ifndef PLATFORMMFDBUFREQUESTER_H
#define PLATFORMMFDBUFREQUESTER_H

#include "MultiFileDBufRequester.h"

class FileHandlerContext;
class TileMapToolkit;
class DBufRequester;
class MemTracker;

class PlatformMFDBufRequester : public MultiFileDBufRequester {
public:
   /**
    *   Constructor.
    */
   PlatformMFDBufRequester( FileHandlerContext* fileContext,
                            TileMapToolkit* toolkit,
                            DBufRequester* parent,
                            const char* basePath,
                            MemTracker* memTracker,
                            WFAPI::wf_uint32 maxSize,
                            bool read_only);


   /**
    *  @see DBufRequester::toString
    */ 
   virtual const char* toString() {
      return "PlatformMFDBufRequester";
   }
   
   /// Creates a file handler.
   FileHandler* createFileHandler( const char* fileName,
                                   bool createIfNotExists,
                                   bool initNow = true );

   /// Returns the path separator
   const char* getPathSeparator() const;
private:
   TileMapToolkit* m_toolkit;
   FileHandlerContext* m_fileContext;
};
   




#endif /* PLATFORMMFDBUFREQUESTER_H */
