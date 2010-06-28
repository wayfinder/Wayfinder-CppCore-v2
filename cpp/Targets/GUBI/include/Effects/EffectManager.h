#ifndef _EFFECT_MANAGER_H_
#define _EFFECT_MANAGER_H_

#include <list>
#include "Effects/IEffectInternal.h"
#include "Effects/EffectFactory.h"
#include "Effects/EffectStartOption.h"
#include "Common/SmartPointer.h"

namespace gubi {

// Prototypes
class ITime;

/**
 * Handles all effect processing.
 */
class EffectManager
{
   typedef std::list<gubi::SmartPointer<IEffectInternal> > effect_list;

public:
   /**
    * Default constructor.
    */
   EffectManager(
      gubi::ITime* pTime );

   /**
    * Destructor.
    */
   virtual ~EffectManager(
      void );

   bool CreateEffect(
      const gubi::Identifier& eEffectType,
      gubi::IPropertyBag* pPropertyBag,
      const gubi::EffectStartOption& eStartOption,
      gubi::IEffect** ppEffect );

   bool Update(
      void );

private:
   void UpdateEffect(
      gubi::IEffectInternal* pEffect,
      unsigned int iTime );

   void SyncTime(
      void );

   gubi::SmartPointer<gubi::ITime> m_pTime;
   EffectFactory                   m_EffectFactory;
   effect_list                     m_lEffects;
   int                             m_iEffectTime;
   int                             m_iTimeShift;
};

}

#endif // _EFFECT_MANAGER_H_
