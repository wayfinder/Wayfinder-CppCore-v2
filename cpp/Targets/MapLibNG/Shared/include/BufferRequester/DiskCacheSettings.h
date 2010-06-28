#ifndef DISKCACHESETTINGS_H
#define DISKCACHESETTINGS_H

#include "PALMachineTypes.h"
#include "WFString.h"

/**
 *  Class describing the read write disk cache settings.
 */
class DiskCacheSettings {
public:
   
   /**
    *  Constructor.
    *
    *  @param path The path for the read write cache.
    *  @param maxSize The maximum size in bytes for the cache.
    */
   DiskCacheSettings(const WFAPI::WFString& path,
                     pal::wf_uint32 maxSize)
      : m_path(path), m_maxSize(maxSize)
      {}

   /**
    * @return the cache path that this config has set.
    */ 
   const WFAPI::WFString& getPath() const {
      return m_path;
   }

   /**
     * @return The maximum size for the disk cache. 
    */
   pal::wf_uint32 getMaxSize() const {
      return m_maxSize;
   }
   
private:
   /// Path for the cache.
   WFAPI::WFString m_path;

   /// Max size in bytes for the cache.
   pal::wf_uint32 m_maxSize;
};

#endif /* DISKCACHESETTINGS_H */

