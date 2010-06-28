
#ifndef RENDER_NODE_H
#define RENDER_NODE_H

#include <vector>
#include "fmath.h"
#include "Core/IObject.h"
#include "Common/Reference.h"
#include "Common/SmartPointer.h"
#include "Property/PropertyBag.h"
#include "Renderer/RenderQueue.h"

namespace gubi {

enum RenderNodeFlag {
   RenderNodeFlag_Dirty = 0x1,
   RenderNodeFlag_Renderable = 0x2,
   RenderNodeFlag_Visible = 0x4
};

// TODO
typedef int RenderNodeFlagSet;

class RenderNode : public gubi::IObject,
                   public gubi::Reference
{
public:
   RenderNode();
   virtual ~RenderNode();

   unsigned int AddReference(
      void ) const;
   
   unsigned int Release(
      void ) const;

   virtual bool update(RenderQueue &renderQueue, bool forceUpdateMatrix);

   virtual void render();

   void getTranslation(fmath::fvec3 &translation);
   void getRotation(fmath::fvec3 &rotation);
   void getScale(fmath::fvec3 &scale);
   fmath::fmatrix16& getMatrix();
   const fmath::fmatrix16& getMatrix() const;
   
   void setTranslation(const fmath::fixed &x,
                       const fmath::fixed &y,
                       const fmath::fixed &z);
   void setRotation(const fmath::fixed &x,
                    const fmath::fixed &y,
                    const fmath::fixed &z);
   void setScale(const fmath::fixed &x,
                 const fmath::fixed &y,
                 const fmath::fixed &z);

   RenderNodeFlagSet addRenderNodeFlag(const RenderNodeFlag &flag);
   RenderNodeFlagSet removeRenderNodeFlag(const RenderNodeFlag &flag);
   bool hasRenderNodeFlag(const RenderNodeFlag &flag);

   unsigned int getNumberOfChildren();
   RenderNode* getChild(const unsigned int &idx);
   unsigned int addChild(RenderNode *child);
   unsigned int removeChild(RenderNode *child, bool releaseChild = true);

      /**
    * Queries the objects property bag.
    *
    * @return Returns true if the operation was successful.
    */
   bool GetProperties(
      gubi::IPropertyBag** ppProperties );

         /**
    * Queries the objects property bag.
    *
    * @return Returns true if the operation was successful.
    */
   bool GetProperties(
      const gubi::IPropertyBag** ppProperties ) const;
protected:
   fmath::fmatrix16 m_matrix;
   fmath::fvec3 m_translation;
   fmath::fvec3 m_rotation;
   fmath::fvec3 m_scale;
   SmartPointer<PropertyBag> m_propertyBag;
   
private:
   void updateMatrix();

   RenderNodeFlagSet m_flags;
   std::vector<RenderNode*> m_children;
};
  
}

#endif // RENDER_NODE_H
