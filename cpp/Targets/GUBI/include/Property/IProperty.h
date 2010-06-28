
#ifndef _IPROPERTY_H_
#define _IPROPERTY_H_

#include "Common/IReference.h"

namespace gubi {

/**
 * Represents a property bound to an object.
 */
class IProperty : public gubi::IReference
{
public:
   /**
    * Retrieves the size of the property.
    */
   virtual unsigned int Size(
      void ) const = 0;

   /**
    * Reads the property data.
    */
   virtual bool ReadData(
      unsigned int iSize,
      void* pData,
      unsigned int* pActualSize ) const = 0;


   /**
    * Writes the property data.
    */
   virtual bool WriteData(
      unsigned int iSize,
      const void* pData,
      unsigned int* pActualSize ) = 0;
};

/**
 * Queries the property data by type.
 */
template<class T>
inline bool GetDataByType(
   const gubi::IProperty* pProperty,
   T* pData )
{
   bool bReturn = false;

   if (pProperty != NULL)
   {
      unsigned int iActual = 0;
      bReturn              = pProperty->ReadData(sizeof(T), pData, &iActual);

      if (bReturn)
      {
         bReturn = (sizeof(T) == iActual);
      }
   }

   return bReturn;
}

}

#endif // _IPROPERTY_H_
