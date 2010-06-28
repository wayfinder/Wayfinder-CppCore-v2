#ifndef _IEFFECT_INTERNAL_H_
#define _IEFFECT_INTERNAL_H_

#include "Common/IReference.h"
#include "Effects/IEffect.h"
#include "Effects/EffectState.h"

namespace gubi {

/**
 * Internal interface for the effects used by the effect manager.
 */
class IEffectInternal : public gubi::IEffect,
                        public gubi::IReference
{
public:
   /**
    * Updates the effect.
    */
   virtual void Update(
      unsigned int iTime ) = 0;

   virtual void SetState(
      const gubi::EffectState& eState ) = 0;

   virtual gubi::EffectState GetState(
      void ) = 0;

   virtual unsigned int GetStartTime(
      void ) = 0;
};

}

#endif // _IEFFECT_INTERNAL_H_
