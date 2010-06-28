#ifndef ORIGINTYPE_H
#define ORIGINTYPE_H

/**
 *  Defines the origin of a requested buffer.
 */
struct OriginType {
   /// Type of requester to allow different behaviour
   enum enum_t {
      /// Memory requester
      MEMORY        = 1,
      /// Resource directory
      RESOURCE_DIRECTORY = 2,
      /// File requester
      READ_WRITE_CACHE = 4,
      /// Read only (to be or:ed together with file )
      PREINSTALLED = 8,
      /// External (internet)
      EXTERNAL  = 16,
      /// Unkown
      UNKNOWN   = 32,
   };

   // This is for simple forward declares and casts of enums.
   OriginType():m_t(enum_t(0)) {}
   template <typename T>
   inline OriginType( T t ):m_t(enum_t(t)) {}
   operator enum_t() const { return m_t; }
   enum_t m_t;

   static const char* toString(OriginType type);
};



#endif /* ORIGINTYPE_H */
