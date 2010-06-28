#ifndef _IOBJECT_H_
#define _IOBJECT_H_

#include "Common/IReference.h"

// GUBI namespace
namespace gubi {

// Prototypes
class IPropertyBag;

/**
 * Basic object class. Includes properties and references.
 */
class IObject : public gubi::IReference
{
public:
   /**
    * Queries the objects property bag.
    *
    * @return Returns true if the operation was successful.
    */
   virtual bool GetProperties(
      gubi::IPropertyBag** ppProperties ) = 0;

      /**
    * Queries the objects property bag.
    *
    * @return Returns true if the operation was successful.
    */
   virtual bool GetProperties(
      const gubi::IPropertyBag** ppProperties ) const = 0;
};

}

#endif // _IOBJECT_H_
