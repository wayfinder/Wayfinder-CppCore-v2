#ifndef _PROPERTY_H_
#define _PROPERTY_H_

#include "Property/IProperty.h"
#include "Common/Reference.h"

namespace gubi {

/**
 * Contains one property of an object.
 */
template<class T>
class Property : public gubi::Reference,
                 public gubi::IProperty
{
public:
    /**
     * Copies the refData into the property container.
     */
   Property(
      const T& refData ) : 
   m_Data(refData)
   {

   }

   /**
    * Destructor.
    */
   ~Property(
      void )
   {

   }

   /**
    * Retrieves the size of the property.
    */
   unsigned int Size(
      void ) const
   {
      return sizeof(T);
   }

   /**
    * Reads the property data.
    */
   bool ReadData(
      unsigned int iSize,
      void* pData,
      unsigned int* pActualSize ) const
   {
      bool bReturn = false;

      if (pData != NULL && iSize >= sizeof(T))
      {
         *((T*)pData) = m_Data;
         bReturn      = true;

         if (pActualSize != NULL)
         {
            *pActualSize = sizeof(T);
         }
      }

      return bReturn;
   }

   /**
    * Writes the property data.
    */
   bool WriteData(
      unsigned int iSize,
      const void* pData,
      unsigned int* pActualSize )
   {
      bool bReturn = false;

      if (pData != NULL && iSize >= sizeof(T))
      {
         m_Data  = *((T*)pData);
         bReturn = true;

         if (pActualSize != NULL)
         {
            *pActualSize = sizeof(T);
         }
      }

      return bReturn;
   }

   /**
    * Adds a new reference to the object.
    *
    * @return Number of reference to the object.
    */
   unsigned int AddReference(
      void ) const
   {
      return InternalAddReference();
   }

   /**
    * Releases a reference to the object.
    *
    * @return Number of reference to the object.
    */
   unsigned int Release(
      void ) const
   {
      return InternalRelease();
   }

private:
   T m_Data;
};

}

#endif
