#ifndef _PROPERTY_UTILS_H_
#define _PROPERTY_UTILS_H_

#include "Property.h"
#include "PropertyBag.h"

namespace gubi {

template<class T>
bool InsertProperty(
   gubi::PropertyBag* pPropertyBag,
   const gubi::Identifier& ePropertyId,
   const T& eValue )
{
   bool bReturn = false;

   if (pPropertyBag != NULL)
   {
      bReturn = pPropertyBag->Insert(ePropertyId,
                                     new gubi::Property<T>(eValue));
   }

   return bReturn;
}

template<class T>
bool UpdateProperty(
   gubi::PropertyBag* pPropertyBag,
   const gubi::Identifier& ePropertyId,
   const T& eValue )
{
   bool bReturn = false;

   if (pPropertyBag != NULL)
   {
      gubi::IProperty* pProperty = NULL;

      if (pPropertyBag->Find(ePropertyId, &pProperty) &&
         pProperty->Size() == sizeof(T))
      {
         bReturn = pProperty->WriteData(sizeof(T),
                                        (const void*)&eValue,
                                        NULL);
      }
   }

   return bReturn;
}

}

#endif // _PROPERTY_UTILS_H_
