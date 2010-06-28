#ifndef _SIGNAL_UTILS_H_
#define _SIGNAL_UTILS_H_

#include "Signal/ISignalCollection.h"
#include "Signal/ISignalHub.h"
#include "Signal/Signal.h"
#include "Common/Identifier.h"

namespace gubi {

// Prototypes
class IPropertyBag;

/**
 * Helper function for connecting a signal to an object.
 */
template<class T>
bool SignalConnect(
   gubi::ISignalCollection* pCollection,
   const gubi::Identifier& eSignalId,
   T* pInstance,
   void (T::*pFunction)(gubi::IPropertyBag*),
   gubi::Identifier* pConnectionId )
{
   bool bReturn = false;

   if (pCollection != NULL)
   {
      gubi::ISignalHub* pSignalHub = NULL;

      if (pCollection->GetSignal(eSignalId, &pSignalHub))
      {
         bReturn = pSignalHub->Connect(new gubi::Signal<T>(pInstance,
                                                           pFunction),
                                                           pConnectionId);

         pSignalHub->Release();
      }
   }

   return bReturn;
}

/**
 * Helper function for disconnecting a signal from an object.
 */
bool SignalDisconnect(
   gubi::ISignalCollection* pCollection,
   const gubi::Identifier& eSignalId,
   unsigned int iConnectionId );
}

#endif // _SIGNAL_UTILS_H_
