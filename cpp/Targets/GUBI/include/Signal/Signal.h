#ifndef _SIGNAL_H_
#define _SIGNAL_H_

#include "Signal/ISignal.h"
#include "Common/Reference.h"

namespace gubi {

// Prototypes
class IPropertyBag;

/**
 * Handles execution of a function when the signal is dispatched.
 */
template<class T>
class Signal : public gubi::ISignal,
               public gubi::Reference
{
public:
   /**
    * Constructor which sets the instance and function
    * to execute when the signal is dispatched.
    */
   Signal(
      T* pInstance,
      void (T::*pFunction)(gubi::IPropertyBag*) )
   {
      m_pInstance = pInstance;
      m_pFunction = pFunction;
   }

   /**
    * Destructor.
    */
   ~Signal(
      void )
   {
      m_pInstance = NULL;
      m_pFunction = NULL;
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

   /**
    * Dispatches the signal.
    */
   void Dispatch(
      gubi::IPropertyBag* pPropertyBag )
   {
      if (m_pInstance != NULL && m_pFunction != NULL)
      {
         (m_pInstance->*m_pFunction)(pPropertyBag);
      }
   }

private:
   T*   m_pInstance;
   void (T::*m_pFunction)(gubi::IPropertyBag*);
};

}

#endif
