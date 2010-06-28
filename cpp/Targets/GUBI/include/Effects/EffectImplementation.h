#ifndef _EFFECT_IMPLEMENTATION_H_
#define _EFFECT_IMPLEMENTATION_H_

#include "Effects/IEffectInternal.h"
#include "Effects/EffectState.h"
#include "Common/Reference.h"
#include "Common/SmartPointer.h"
#include "Signal/SignalCollection.h"

namespace gubi {

// Prototypes
class PropertyBag;

/**
 * Implementation of the basics for all effect types.
 */
class EffectImplementation : public gubi::IEffectInternal,
                             public gubi::Reference,
                             public gubi::SignalCollection
{
public:
   EffectImplementation(
      void );

   virtual ~EffectImplementation(
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
    * Query a signal hub by its assigned signal type.
    */
   bool GetSignal(
      const gubi::Identifier& eSignalId,
      gubi::ISignalHub** ppSignalHub );

   /**
    * Signals the effect to start.
    */
   bool Start(
      void );

   /**
    * Signals the effect to stop. The effect will
    * automatically deallocate in the next update cycle.
    */
   void Stop(
      void );

   /**
    * Pauses the effect.
    */
   bool Pause(
      void );

   /**
    * Resumes the effect.
    */
   bool Continue(
      void );

   void SetState(
      const gubi::EffectState& eState );

   gubi::EffectState GetState(
      void );

   virtual unsigned int GetStartTime(
      void );

protected:
   gubi::SmartPointer<gubi::PropertyBag> m_pPropertyBag;

private:
   gubi::EffectState m_eEffectState;
};

}

#endif // _EFFECT_IMPLEMENTATION_H_
