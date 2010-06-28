#ifndef _SIGNAL_COLLECTION_H_
#define _SIGNAL_COLLECTION_H_

#include <map>
#include "Common/Identifier.h"
#include "Common/SmartPointer.h"

namespace gubi {

// Prototypes
class ISignalHub;
class IPropertyBag;

/**
 * Handles a collection of signal types.
 */
class SignalCollection
{
   typedef std::map<gubi::Identifier,
      gubi::SmartPointer<gubi::ISignalHub> > hub_map;
public:
   /**
    * Default constructor.
    */
   SignalCollection(
      void );

   /**
    * Constructs the signal collection with predetermined signals. 
    */
   SignalCollection(
      const gubi::Identifier* pSignalElements,
      unsigned int iSignalElements );

   /**
    * Destructor.
    */
   virtual ~SignalCollection(
      void );

protected:
   /**
    * Adds a new signal for a specific signal id.
    */
   bool InternalAddSignal(
      const gubi::Identifier& eSignalId );

   /**
    * Removes a signal with a specific signal id.
    */
   bool InternalRemoveSignal(
      const gubi::Identifier& eSignalId );

   /**
    * Dispatches a specific signal.
    */
   void InternalDispatchSignal(
      const gubi::Identifier& eSignalId,
      gubi::IPropertyBag* pPropertyBag );

   /**
    * Retrieves a specific signal instance.
    */
   bool InternalGetSignal(
      const gubi::Identifier& eSignalId,
      gubi::ISignalHub** ppSignalHub );

private:
   hub_map m_mapSignalHub;
};

}

#endif // _SIGNAL_COLLECTION_H_
