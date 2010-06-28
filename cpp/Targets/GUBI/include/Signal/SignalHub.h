#ifndef _SIGNAL_HUB_H_
#define _SIGNAL_HUB_H_

#include <map>
#include "Signal/ISignalHub.h"
#include "Common/Reference.h"

namespace gubi {

/**
 * Handles connectivity and dispatching of a signal.
 */
class SignalHub : public gubi::Reference,
                  public gubi::ISignalHub
{
   typedef std::map<gubi::Identifier,
      gubi::ISignal*> signal_map;
public:
   /**
    * Default constructor.
    */
   SignalHub(
      void );

   /**
    * Destructor.
    */
   virtual ~SignalHub(
      void );

   /**
    * Adds a new reference to the object.
    *
    * @return Number of reference to the object.
    */
   unsigned int AddReference(
      void ) const;

   /**
    * Releases a reference to the object.
    *
    * @return Number of reference to the object.
    */
   unsigned int Release(
      void ) const;

   /**
    * Connects a signal to the hub.
    */
   bool Connect(
      gubi::ISignal* pSignal,
      gubi::Identifier* pIdentifier );

   /**
    * Disconnects a signal to the hub.
    */
   bool Disconnect(
      gubi::Identifier eIdentifier );

   /**
    * Dispatches all connected signals.
    */
   void Dispatch(
      gubi::IPropertyBag* pPropertyBag );

private:
   signal_map m_mapSignal;
};

}

#endif // _SIGNAL_HUB_H_
