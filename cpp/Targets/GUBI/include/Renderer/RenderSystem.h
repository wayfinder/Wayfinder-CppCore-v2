
#ifndef RENDER_SYSTEM_H
#define RENDER_SYSTEM_H

#include <list>
#include "Renderer/IRenderSystem.h"
#include "Renderer/RenderQueue.h"

namespace gubi {

class IViewInternal;
class IRenderBridge;
class RenderNode;

class RenderSystem : public IRenderSystem {
public:
   RenderSystem(IRenderBridge *bridge);
   ~RenderSystem();

   bool init(
      void );

   bool update(
      void );

   void render(
      void );

   void shutdown(
      void );

   int addView(
      gubi::IViewInternal *view );

   int removeView(
      gubi::IViewInternal *view );

   virtual bool GetRenderFunction(
      const gubi::Identifier& eRenderId,
      IRenderFunction** ppRenderFunction );

private:
   void renderImage(
      const RenderNode *node );
   
   std::list<IViewInternal*> m_viewList;
   IRenderBridge*            m_bridge;
   RenderQueue               m_renderQueue[2];
};

} // namespace gubi

#endif // RENDER_SYSTEM_H
