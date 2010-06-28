#ifndef PLATFORMFACTORY_H
#define PLATFORMFACTORY_H

class FileHandlerContext;
class FileHandler;
class TileMapToolkit;
class MainThreadScheduler;

/**
 *  Different platforms can derive from this class
 *  to store platform specific state variables.
 */ 
class FileHandlerContext {
public:
   virtual ~FileHandlerContext() {}
};

namespace PlatformFactory {
/**
 *  Creates a new toolkit that can be used for timers etc.
 */ 
TileMapToolkit* createToolkit();

/**
 *  Creates a platform specific context that can be used
 *  to create file handlers.
 */ 
FileHandlerContext* createFileHandlerContext();

/**
 *  Creates a new file handler that is guaranteed to work for the current platform.
 */ 
FileHandler* createFileHandler(FileHandlerContext* context,
                               TileMapToolkit* toolkit,
                               const char* filename,
                               bool readOnly,
                               bool createFile,
                               bool initNow );

/**
 * Returns the path separator that is valid for the system
 */ 
const char* getPathSeparator();

/**
 *  Returns a scheduler which can be used to perform work
 *  on the main thread, where this is possible.
 */ 
MainThreadScheduler* createMainThreadSheduler();

} // End of namespace PlatformFactory


#endif /* PLATFORMFACTORY_H */
