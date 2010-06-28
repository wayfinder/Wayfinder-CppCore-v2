#ifndef _ITIME_H_
#define _ITIME_H_

#include "IReference.h"

namespace gubi {

/**
 *
 */
class ITime : public gubi::IReference
{
public:
   /**
    * Returns the current time in milliseconds.
    */
   virtual unsigned int GetTime(
      void ) = 0;
};

}

#endif // _ITIME_H_
