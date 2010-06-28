#ifndef _ISIGNAL_HUB_H_
#define _ISIGNAL_HUB_H_

#include "Common/IReference.h"
#include "Common/Identifier.h"

namespace gubi {

// Prototypes
class ISignal;
class IPropertyBag;

/**
 * Interface to handling connectivity and dispatching for the signal hub.
 */
class ISignalHub : public gubi::IReference
{
public:
   /**
    * Connects a signal to the hub.
    */
   virtual bool Connect(
      gubi::ISignal* pSignal,
      gubi::Identifier* pIdentifier ) = 0;

   /**
    * Disconnects a signal to the hub.
    */
   virtual bool Disconnect(
      gubi::Identifier eIdentifier ) = 0;

   /**
    * Dispatches all connected signals.
    */
   virtual void Dispatch(
      gubi::IPropertyBag* pPropertyBag ) = 0;
};

}
#endif // _ISIGNAL_HUB_H_
