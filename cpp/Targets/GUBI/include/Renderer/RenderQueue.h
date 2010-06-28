
#ifndef RENDER_QUEUE_H
#define RENDER_QUEUE_H

#include <vector>

namespace gubi {
  
class RenderNode;

class RenderQueue {
public:
   void addRenderNode(RenderNode *node);

   void render();

private:
   static bool zSort(RenderNode *n1, RenderNode *n2);
   static void renderNode(RenderNode *node);

   std::vector<RenderNode*> m_queue;
};

} // namespace gubi

#endif // RENDER_QUEUE_H
