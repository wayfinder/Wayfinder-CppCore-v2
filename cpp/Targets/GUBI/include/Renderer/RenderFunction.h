
#ifndef _RENDER_FUNCTION_H_
#define _RENDER_FUNCTION_H_

#include "IRenderFunction.h"

namespace gubi {

template<class T>
class RenderFunction : public gubi::IRenderFunction
{
public:
   RenderFunction(
      T* pInstance,
      void (T::*pFunction)(const gubi::RenderNode*) )
   {
      m_pInstance = pInstance;
      m_pFunction = pFunction;
   }

   ~RenderFunction(
      void )
   {
      m_pInstance = NULL;
      m_pFunction = NULL;
   }

   void PerformRender(
      const gubi::RenderNode* pObject )
   {
      if (m_pInstance != NULL && m_pFunction != NULL)
      {
         (m_pInstance->*m_pFunction)(pObject);
      }
   }

   void Release(
      void )
   {
      delete this;
   }

private:
   T* m_pInstance;
   void (T::*m_pFunction)(const gubi::RenderNode*);
};

template<class T>
gubi::IRenderFunction* CreateRenderFunction(
   T* pInstance,
   void (T::*pFunction)(const gubi::RenderNode*))
{
   return new RenderFunction<T>(pInstance, pFunction);
}

}

#endif // _RENDER_FUNCTION_H_
