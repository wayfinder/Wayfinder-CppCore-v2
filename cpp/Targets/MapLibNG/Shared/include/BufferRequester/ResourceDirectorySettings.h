#ifndef RESOURCEDIRECTORYSETTINGS_H
#define RESOURCEDIRECTORYSETTINGS_H

#include "WFString.h"

/**
 *  Class describing the resource directory settings.
 */ 
class ResourceDirectorySettings {
public:
   /**
    *  Constructor.
    *
    *  @param path The path for the read write cache.
    *  @param writeable If the resource directory should be writeable.
    */
   ResourceDirectorySettings(const WFAPI::WFString& path,
                             bool writeable)
      : m_path(path), m_writeable(writeable)
      {}

   /**
    * @return the directory path that this config has set.
    */ 
   const WFAPI::WFString& getPath() const {
      return m_path;
   }

   /**
    * @return Whether or not the resource directory is writeable.
    */
   bool getWriteable() const {
      return m_writeable;
   }
   
private:
   /// Path for the cache.
   WFAPI::WFString m_path;

   /// Writeable state
   bool m_writeable;
};

#endif /* RESOURCEDIRECTORYSETTINGS_H */
