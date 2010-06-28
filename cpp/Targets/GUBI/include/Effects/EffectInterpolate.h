#ifndef _EFFECT_INTERPOLATE_H_
#define _EFFECT_INTERPOLATE_H_

#include "Effects/EffectImplementation.h"
#include "Effects/EffectInterpolatePace.h"

namespace gubi {

class EffectInterpolate : public gubi::EffectImplementation
{
public:
   static IEffectInternal* Create(
      gubi::IPropertyBag* pPropertyBag,
      unsigned int iTime );

   EffectInterpolate(
      int iStartValue,
      int iEndValue,
      unsigned int iDuration,
      unsigned int iStartTime,
      unsigned int iEndTime,
      gubi::EffectInterpolatePace eEffectPace );

   virtual ~EffectInterpolate(
      void );

   void Update(
      unsigned int iTime );

   unsigned int GetStartTime(
      void );

private:
   int Interpolate(
      unsigned int iDeltaTime );

   int                         m_iStartValue;
   int                         m_iEndValue;
   unsigned int                m_iDuration;
   unsigned int                m_iStartTime;
   unsigned int                m_iEndTime;
   gubi::EffectInterpolatePace m_eEffectPace;
};

}

#endif // _EFFECT_INTERPOLATE_H_
