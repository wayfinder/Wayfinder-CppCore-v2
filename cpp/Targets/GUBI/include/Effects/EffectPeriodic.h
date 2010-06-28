#ifndef _EFFECT_PERIODIC_H_
#define _EFFECT_PERIODIC_H_

#include "EffectImplementation.h"

namespace gubi {

class EffectPeriodic : public gubi::EffectImplementation
{
public:
   static gubi::IEffectInternal* Create(
      gubi::IPropertyBag* pPropertyBag,
      unsigned int iTime );

   EffectPeriodic(
      unsigned int iNextUpdate,
      unsigned int iPeriod );

   virtual ~EffectPeriodic(
      void );

   void Update(
      unsigned int iTime );

private:
   unsigned int m_iNextUpdate;
   unsigned int m_iPeriod;
};

}

#endif // _EFFECT_PERIODIC_H_
