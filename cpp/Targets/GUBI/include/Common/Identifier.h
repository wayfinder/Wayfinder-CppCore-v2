#ifndef _IDENTIFIER_H_
#define _IDENTIFIER_H_

#include <string>

namespace gubi {

/**
 * Represents a unique identifier id.
 */
class Identifier
{
public:
   /**
    * Constructs the identifier from a string.
    */
   Identifier(
      const std::wstring& strIdentity );

   /**
    * Constructs the identifier from a string.
    */
   Identifier(
      const wchar_t* pIdentity );

   /**
    * Constructs the identifier from an integer.
    */
   Identifier(
      unsigned int iIdentity );

   /**
    * Less-than operator for usage within a std::map.
    */
   bool operator<(
      const Identifier& refIdentifier ) const;

   /**
    * Casting operator for usage within a switch statement.
    */
   operator unsigned int(
      void ) const;

private:
   /**
    * Creates a unique hash from the string.
    */
   static unsigned int HashString(
      const wchar_t* pIdentity,
      unsigned int iLength );

   unsigned int m_iIdentity;
};

}

#endif // _IDENTIFIER_H_

