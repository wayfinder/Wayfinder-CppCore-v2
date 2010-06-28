
#ifndef _IRENDER_FUNCTION_H_
#define _IRENDER_FUNCTION_H_

namespace gubi {

// Prototypes
class RenderNode;

class IRenderFunction
{
public:
   virtual void PerformRender(
      const gubi::RenderNode* pObject ) = 0;

   virtual void Release(
      void ) = 0;
};

}

#endif // _IRENDER_FUNCTION_H_
