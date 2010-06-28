#ifndef MEMCACHESETTINGS_H
#define MEMCACHESETTINGS_H

/**
 *  Class describing the memory cache settings for buffers.
 */
class MemCacheSettings {
public:

   /**
    *  Constructor.
    *
    *  @param maxSize The maximum size of the cache in bytes.
    */
   MemCacheSettings(unsigned int maxSize) :
      m_maxSize(maxSize)
   {}
   
   /**
    *  @return The maximum size of the cache in bytes.
    */
   unsigned int getMaxSize() const {
      return m_maxSize;
   }
   
private:
   
   /**
    *  The maximum size of the cache in bytes.
    */
   unsigned int m_maxSize;
};

#endif /* MEMCACHESETTINGS_H */
