#ifndef CACHEPOLICY_H
#define CACHEPOLICY_H

/**
 *  Cache policy settings.
 */
struct CachePolicy {
   enum enum_t {
      /// It is allowed to request the map from internet or the cache
      cacheOrInternet = 0,
      /// It is allowed to request the map from caches only.
      onlyCache       = 1,
      /// It is allowed to request the map from the resource directory only
      onlyResDir      = 2
   };

   // This is for simple forward declares and casts of enums.
   CachePolicy():m_t(enum_t(0)) {}
   template <typename T>
   inline CachePolicy( T t ):m_t(enum_t(t)) {}
   operator enum_t() const { return m_t; }
   enum_t m_t;
};


#endif /* CACHEPOLICY_H */
