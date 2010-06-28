#include "PlatformFactory.h"
#include "IPhoneFileHandler.h"
#include "IPhoneTileMapToolkit.h"
#include "IPMainThreadScheduler.h"

FileHandlerContext* PlatformFactory::createFileHandlerContext()
{
   return NULL;
}

FileHandler* PlatformFactory::createFileHandler(FileHandlerContext* context,
                                                TileMapToolkit* toolkit,
                                                const char* filename,
                                                bool readOnly,
                                                bool createFile,
                                                bool initNow)
{
   return new IPhoneFileHandler(filename, createFile, toolkit);
}

const char* PlatformFactory::getPathSeparator()
{
   return "/"; 
}

TileMapToolkit* PlatformFactory::createToolkit() {
   return new IPhoneTileMapToolkit(0);
}

MainThreadScheduler* PlatformFactory::createMainThreadSheduler() {
   return new IPMainThreadScheduler;
}
