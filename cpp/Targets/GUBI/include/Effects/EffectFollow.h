#ifndef _EFFECT_FOLLOW_H_
#define _EFFECT_FOLLOW_H_

#include "EffectImplementation.h"

namespace gubi {

// Prototypes
class IPropertyBag;

class EffectFollow : public gubi::EffectImplementation
{
public:
   static IEffectInternal* Create(
      gubi::IPropertyBag* pPropertyBag,
      unsigned int iTime );

   EffectFollow(
      int iTarget,
      unsigned char iSourceSpeed,
      unsigned char iTargetSpeed );

   virtual ~EffectFollow(
      void );

   void Update(
      unsigned int iTime );

private:
   int           m_iValue;
   int           m_iTarget;
   int           m_iWaypoint;
   unsigned char m_iSourceSpeed;
   unsigned char m_iTargetSpeed;
};

}

#endif // _EFFECT_FOLLOW_H_
