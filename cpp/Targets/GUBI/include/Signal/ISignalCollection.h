#ifndef _ISIGNAL_COLLECTION_H_
#define _ISIGNAL_COLLECTION_H_

#include "Common/Identifier.h"

namespace gubi {

// Prototypes
class ISignalHub;

/**
 * Interface to query an objects signal hubs.
 */
class ISignalCollection
{
public:
   /**
    * Query a signal hub by its assigned signal type.
    */
   virtual bool GetSignal(
      const gubi::Identifier& eSignalId,
      gubi::ISignalHub** ppSignalHub ) = 0;
};

}

#endif // _ISIGNAL_COLLECTION_H_
