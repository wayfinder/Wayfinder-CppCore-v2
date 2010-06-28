
#ifndef _IPROPERTY_BAG_
#define _IPROPERTY_BAG_

#include "Common/IReference.h"
#include "Common/Identifier.h"

namespace gubi {

// Prototypes
class IProperty;

/**
 * Contains all properties for a specific object.
 */
class IPropertyBag : public gubi::IReference
{
public:
   /**
    * Searches for a specific property.
    */
   virtual bool Find(
      const gubi::Identifier& ePropertyId,
      gubi::IProperty** ppProperty ) = 0;

   /**
    * Searches for a specific property.
    */
   virtual bool Find(
      const gubi::Identifier& ePropertyId,
      const gubi::IProperty** ppProperty ) const = 0;

   /**
    * Retrieves the next property in the bag.
    */
   virtual bool GetNext(
      gubi::IProperty** ppProperty ) = 0;

   /**
    * Retrieves the next property in the bag.
    */
   virtual bool GetNext(
      const gubi::IProperty** ppProperty ) const = 0;

   /**
    * Resets the enumerator.
    */
   virtual void Reset(
      void ) const = 0;
};

}

#endif
