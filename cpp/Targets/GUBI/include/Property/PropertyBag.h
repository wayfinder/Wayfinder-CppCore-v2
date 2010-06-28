
#ifndef _PROPERTY_BAG_
#define _PROPERTY_BAG_

#include <map>
#include "Property/IPropertyBag.h"
#include "Common/Reference.h"
#include "Common/SmartPointer.h"

namespace gubi {

/**
 * Handles all properties for an object.
 */
class PropertyBag : public gubi::IPropertyBag,
                    public gubi::Reference
{
   typedef std::map<gubi::Identifier,
      gubi::SmartPointer<gubi::IProperty> > property_map;
public:
   /**
    * Default constructor.
    */
   PropertyBag(
      void );

   /**
    * Destructor.
    */
   ~PropertyBag(
      void );

   /**
    * Searches for a specific property.
    */
   bool Find(
      const gubi::Identifier& ePropertyId,
      gubi::IProperty** ppProperty );

   /**
    * Searches for a specific property.
    */
   bool Find(
      const gubi::Identifier& ePropertyId,
      const gubi::IProperty** ppProperty ) const;

   /**
    * Retrieves the next property in the bag.
    */
   bool GetNext(
      gubi::IProperty** ppProperty );

   /**
    * Retrieves the next property in the bag.
    */
   bool GetNext(
      const gubi::IProperty** ppProperty ) const;

   /**
    * Resets the enumerator.
    */
   void Reset(
      void ) const;

   /**
    * Inserts a new property.
    */
   bool Insert(
      const gubi::Identifier& ePropertyId,
      gubi::IProperty* pProperty );

   /**
    * Adds a new reference to the object.
    *
    * @return Number of reference to the object.
    */
   unsigned int AddReference(
      void ) const;

   /**
    * Releases a reference to the object.
    *
    * @return Number of reference to the object.
    */
   unsigned int Release(
      void ) const;

private:
   mutable property_map           m_mapProperty;
   mutable property_map::iterator m_mapIterator;
};

}

#endif
