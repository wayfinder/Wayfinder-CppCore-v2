#ifndef _EFFECT_WAVE_H_
#define _EFFECT_WAVE_H_

#include "EffectImplementation.h"

namespace gubi {

class EffectWave : public gubi::EffectImplementation
{
public:
   static gubi::IEffectInternal* Create(
      gubi::IPropertyBag* pPropertyBag,
      unsigned int iTime );

   EffectWave(
      unsigned int iPeriod,
      int iAmplitude,
      int iPhase );

   virtual ~EffectWave(
      void );

   void Update(
      unsigned int iTime );

private:
   bool         m_bHasStartTime;
   unsigned int m_iStartTime;
   unsigned int m_iPeriod;
   int m_iAmplitude;
   int m_iPhase;
};

}

#endif // _EFFECT_WAVE_H_
