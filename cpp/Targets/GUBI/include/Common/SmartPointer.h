#ifndef _SMART_POINTER_H_
#define _SMART_POINTER_H_

namespace gubi {

/**
 * Handles reference counting automatically for all types IReference instances.
 */
template<class T>
class SmartPointer
{
public:
   /**
    * Default constructor.
    */
   SmartPointer(
      void )
   {
      m_pPointer = NULL;
   }

   /**
    * Copy constructor from another smart pointer instance.
    */
   SmartPointer(
      const SmartPointer& refPointer )
   {
      m_pPointer = refPointer.Get();

      InitializePointer();
   }

   /**
    * Copy constructor from a pointer.
    */
   SmartPointer(
      T* pPointer )
   {
      m_pPointer = pPointer;

      InitializePointer();
   }

   /**
    * Destructor.
    */
   ~SmartPointer(
      void )
   {
      if (m_pPointer != NULL)
      {
         m_pPointer->Release();
         m_pPointer = NULL;
      }
   }

   /**
    * Queries the pointer.
    */
   T* Get(
      void ) const
   {
      return m_pPointer;
   }

   /**
    * Checks if the smart pointer is empty.
    */
   bool IsEmpty(
      void ) const
   {
      return (m_pPointer == NULL);
   }

   /**
    * Casts the smart pointer to a pointer.
    */
   operator T*(
      void ) const
   {
      return m_pPointer;
   }

   /**
    * Access the pointers methods and variables.
    */
   T* operator->(
      void ) const
   {
      return m_pPointer;
   }

   /**
    * Sets a new pointer.
    */
   SmartPointer& operator=(
      const SmartPointer& refPointer )
   {
      T* pOld    = m_pPointer;
      m_pPointer = refPointer.Get();

      if (pOld != NULL)
      {
         pOld->Release();
      }

      InitializePointer();

      return *this;
   }

private:
   /**
    * Increases the pointer reference.
    */
   void InitializePointer(
      void ) const
   {
      if (m_pPointer != NULL)
      {
         m_pPointer->AddReference();
      }
   }

   T* m_pPointer;
};

}

#endif // _SMART_POINTER_H_
