#ifndef BUFFERREQUESTERSETTINGS_H
#define BUFFERREQUESTERSETTINGS_H

class DiskCacheSettings;
class MemCacheSettings;
class ResourceDirectorySettings;

/**
 *   Contains a collection of settings used to configure
 *   the underlying map model.
 */
class BufferRequesterSettings {
public:
   /**
    *  Creates default settings for the map model.
    *  
    *  By default, the disk cache and resource directory
    *  is set to disabled and the memory cache will be
    *  initialized with a conservative size.
    */ 
   BufferRequesterSettings();

   /**
    *  Deallocates this settings object and all of its
    *  related resources.
    */
   ~BufferRequesterSettings();
   
   /**
    *  Copy constructor.
    */ 
   BufferRequesterSettings(const BufferRequesterSettings& rhs);

   /**
    *  Assignment operator.
    */ 
   BufferRequesterSettings& operator=(const BufferRequesterSettings& rhs);
   
   /** 
    *  @param diskSettings the new disk cache settings to use.
    *  @see DiskCacheSettings
    */ 
   void setDiskCacheSettings(const DiskCacheSettings& diskSettings);

   /**
    *  @param memSettings the new memory cache settings to use.
    *  @see MemCacheSettings
    */ 
   void setMemCacheSettings(const MemCacheSettings& memSettings);

   /**
    *  @param resDirSettings the new memory cache settings to use.
    *  @see ResourceDirectorySettings
    */ 
   void setResourceDirectorySettings(
      const ResourceDirectorySettings& resDirSettings);
   
   /**
    *  @return the disk cache settings associated with this class.
    */ 
   const DiskCacheSettings& getDiskCacheSettings() const;

   /**
    *  @return the memory cache settings associated with this class.
    */ 
   const MemCacheSettings& getMemCacheSettings() const;

   /**
    *  @return the resource directory settings associated with this class.
    */
   const ResourceDirectorySettings& getResourceDirectorySettings() const;
   
private:
   struct Impl;
   Impl* m_impl;
};

#endif /* BUFFERREQUESTERSETTINGS_H */
