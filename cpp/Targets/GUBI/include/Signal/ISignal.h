#ifndef _ISIGNAL_H_
#define _ISIGNAL_H_

#include "Common/IReference.h"

namespace gubi {

// Prototypes
class IPropertyBag;

/**
 * Interface for dispatching a signal.
 */
class ISignal : public gubi::IReference
{
public:
   /**
    * Dispatches the signal.
    */
   virtual void Dispatch(
      gubi::IPropertyBag* pPropertyBag ) = 0;
};

}

#endif // _ISIGNAL_H_
