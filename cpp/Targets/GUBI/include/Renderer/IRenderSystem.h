
#ifndef _IRENDER_SYSTEM_H_
#define _IRENDER_SYSTEM_H_

#include "Common/Identifier.h"

namespace gubi {

// Prototypes
class IRenderFunction;

class IRenderSystem
{
public:
   virtual bool GetRenderFunction(
      const gubi::Identifier& eRenderId,
      IRenderFunction** ppRenderFunction ) = 0;
};

}

#endif // _IRENDER_SYSTEM_H_
