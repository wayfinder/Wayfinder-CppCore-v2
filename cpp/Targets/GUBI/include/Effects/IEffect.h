#ifndef _IEFFECT_H_
#define _IEFFECT_H_

#include "Signal/ISignalCollection.h"

namespace gubi {

/**
 * Interface for controlling the effect implementation.
 */
class IEffect : public gubi::ISignalCollection
{
public:
   /**
    * Signals the effect to start.
    */
   virtual bool Start(
      void ) = 0;

   /**
    * Signals the effect to stop. The effect will
    * automatically deallocate in the next update cycle.
    */
   virtual void Stop(
      void ) = 0;

   /**
    * Pauses the effect.
    */
   virtual bool Pause(
      void ) = 0;

   /**
    * Resumes the effect.
    */
   virtual bool Continue(
      void ) = 0;
};

}

#endif // _IEFFECT_H_
