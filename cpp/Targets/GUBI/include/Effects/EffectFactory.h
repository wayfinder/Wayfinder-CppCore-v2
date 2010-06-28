#ifndef _EFFECT_FACTORY_H_
#define _EFFECT_FACTORY_H_

#include <map>
#include "Common/Identifier.h"

namespace gubi {

// Prototypes
class IEffectInternal;
class IPropertyBag;

typedef IEffectInternal*(*EffectCreator)(gubi::IPropertyBag*, unsigned int);

/**
 * Handles all effect creations.
 */
class EffectFactory
{
   typedef std::map<gubi::Identifier,EffectCreator> creator_map;

public:
   /**
    * Default constructor.
    */
   EffectFactory(
      void );

   /**
    * Destructor.
    */
   ~EffectFactory(
      void );

   /**
    * Register a effect creator.
    */
   bool RegisterEffect(
      const gubi::Identifier& eEffectId,
      EffectCreator pCreator );

   /**
    * Unregister a effect creator.
    */
   bool UnregisterEffect(
      const gubi::Identifier& eEffectId );

   /**
    * Creates a effect object.
    */
   bool CreateEffect(
      unsigned int iTime,
      const gubi::Identifier& eEffectId,
      gubi::IPropertyBag* pPropertyBag,
      gubi::IEffectInternal** ppEffect );

private:
   creator_map m_mapCreators;
};

}

#endif // _EFFECT_FACTORY_H_
