#ifndef _TIMER_DEFAULT_H_
#define _TIMER_DEFAULT_H_

#include "ITime.h"
#include "Reference.h"

namespace gubi {

/**
 * Implementation of a default time class.
 */
class TimeDefault : public gubi::ITime,
                    public gubi::Reference
{
public:
   unsigned int AddReference(
      void ) const;

   unsigned int Release(
      void ) const;

   unsigned int GetTime(
      void );
};

}

#endif
