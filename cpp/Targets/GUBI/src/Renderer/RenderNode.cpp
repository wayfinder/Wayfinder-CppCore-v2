/*
Copyright (c) 1999 - 2010, Vodafone Group Services Ltd
All rights reserved.

Redistribution and use in source and binary forms, with or without modification, are permitted provided that the following conditions are met:

    * Redistributions of source code must retain the above copyright notice, this list of conditions and the following disclaimer.
    * Redistributions in binary form must reproduce the above copyright notice, this list of conditions and the following disclaimer in the documentation and/or other materials provided with the distribution.
    * Neither the name of the Vodafone Group Services Ltd nor the names of its contributors may be used to endorse or promote products derived from this software without specific prior written permission.

THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS" AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE ARE DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT HOLDER OR CONTRIBUTORS BE LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
*/


#include "Core/Platform.h"
#include "Property/PropertyUtils.h"
#include "Renderer/OpenGlEsCompatibility.h"
#include "Renderer/RenderNode.h"

namespace gubi {

RenderNode::RenderNode()
      :m_propertyBag(new gubi::PropertyBag)
{
   m_flags = RenderNodeFlag_Dirty | RenderNodeFlag_Visible;
   
   m_scale[0] = fmath::static_int2fix<1>::value;
   m_scale[1] = fmath::static_int2fix<1>::value;
   m_scale[2] = fmath::static_int2fix<1>::value;

   m_translation[0] = fmath::static_int2fix<0>::value;
   m_translation[1] = fmath::static_int2fix<0>::value;
   m_translation[2] = fmath::static_int2fix<0>::value;

   m_rotation[0] = fmath::static_int2fix<0>::value;
   m_rotation[1] = fmath::static_int2fix<0>::value;
   m_rotation[2] = fmath::static_int2fix<0>::value;

   fmath::fmatrix16identity(m_matrix);

   gubi::InsertProperty(m_propertyBag, L"width", 0);
   gubi::InsertProperty(m_propertyBag, L"height", 0);
}

RenderNode::~RenderNode()
{
  
}

unsigned int RenderNode::AddReference(
   void ) const
{
   return InternalAddReference();
}

unsigned int RenderNode::Release(
   void ) const
{
   return InternalRelease();
}

bool RenderNode::update(RenderQueue &renderQueue, bool forceUpdateMatrix)
{
   bool result = false;

   if (forceUpdateMatrix || hasRenderNodeFlag(RenderNodeFlag_Dirty)) {
      updateMatrix();
      forceUpdateMatrix = true;
      result = true;
   }

   std::vector<RenderNode*>::iterator it = m_children.begin();
   for (; it != m_children.end(); ++it) {
      RenderNode *child =(*it);
      if (child != 0L) {
         result = child->update(renderQueue,
            forceUpdateMatrix) ? true : result;
      }
   }

   if (hasRenderNodeFlag(RenderNodeFlag_Renderable)) {
      renderQueue.addRenderNode(this);
   }

   return result;
}

void RenderNode::render()
{
   // Top RenderNode does nothing
}

void RenderNode::getTranslation(fmath::fvec3 &translation)
{
   translation[0] = m_translation[0];
   translation[1] = m_translation[1];
   translation[2] = m_translation[2];
}

void RenderNode::getRotation(fmath::fvec3 &rotation)
{
   rotation[0] = m_rotation[0];
   rotation[1] = m_rotation[1];
   rotation[2] = m_rotation[2];
}

void RenderNode::getScale(fmath::fvec3 &scale)
{
   scale[0] = m_scale[0];
   scale[1] = m_scale[1];
   scale[2] = m_scale[2];
}

fmath::fmatrix16& RenderNode::getMatrix()
{
   return m_matrix;
}

const fmath::fmatrix16& RenderNode::getMatrix() const
{
   return m_matrix;
}

void RenderNode::setTranslation(const fmath::fixed &x,
                                const fmath::fixed &y,
                                const fmath::fixed &z)
{
   m_translation[0] = x;
   m_translation[1] = y;
   m_translation[2] = z;

   addRenderNodeFlag(RenderNodeFlag_Dirty);
}

void RenderNode::setRotation(const fmath::fixed &x,
                             const fmath::fixed &y,
                             const fmath::fixed &z)
{
   m_rotation[0] = x;
   m_rotation[1] = y;
   m_rotation[2] = z;

   addRenderNodeFlag(RenderNodeFlag_Dirty);
}

void RenderNode::setScale(const fmath::fixed &x,
                          const fmath::fixed &y,
                          const fmath::fixed &z)
{
   m_scale[0] = x;
   m_scale[1] = y;
   m_scale[2] = z;

   addRenderNodeFlag(RenderNodeFlag_Dirty);
}

RenderNodeFlagSet RenderNode::addRenderNodeFlag(const RenderNodeFlag &flag)
{
   return (m_flags |= flag);
}

RenderNodeFlagSet RenderNode::removeRenderNodeFlag(const RenderNodeFlag &flag)
{
   return (m_flags &= ~flag);
}

bool RenderNode::hasRenderNodeFlag(const RenderNodeFlag &flag)
{
   return ((m_flags & flag) == flag);
}

unsigned int RenderNode::getNumberOfChildren()
{
   return m_children.size();
}

RenderNode* RenderNode::getChild(const unsigned int &idx)
{
   RenderNode *child = 0L;
   if (idx < getNumberOfChildren()) {
      child = m_children[idx];
   }

   return child;
}

unsigned int RenderNode::addChild(RenderNode *child)
{
   if (child != 0L) {
      m_children.push_back(child);
   }

   return m_children.size();
}

unsigned int RenderNode::removeChild(RenderNode *child, bool releaseChild)
{
   if (child != 0L) {
      std::vector<RenderNode*>::iterator it = m_children.begin();

      while (it != m_children.end()) {
         
         if ((*it) == child) {
            m_children.erase(it);

            if (releaseChild) {
               child->Release();
            }

            it = m_children.end();
         } else {
            ++it;
         }
      }
   }

   return m_children.size();
}

void RenderNode::updateMatrix()
{
   glMatrixMode(GL_MODELVIEW);
   glPushMatrix();

   glTranslatex(m_translation[0], m_translation[1], m_translation[2]);

   glScalex(m_scale[0], m_scale[1], m_scale[2]);
      
   glRotatex(m_rotation[0], 1 << 16, 0, 0);
   glRotatex(m_rotation[1], 0, 1 << 16, 0);
   glRotatex(m_rotation[2], 0, 0, 1 << 16);

   glGetFixedv(GL_MODELVIEW_MATRIX, m_matrix);

   glPopMatrix();
   
   removeRenderNodeFlag(RenderNodeFlag_Dirty);
}

bool RenderNode::GetProperties(
      gubi::IPropertyBag** ppProperties )
{
   if (ppProperties != NULL) {
      m_propertyBag->AddReference();
      *ppProperties = m_propertyBag;
      return true;
   }
   return false;
}

bool RenderNode::GetProperties(
      const gubi::IPropertyBag** ppProperties ) const
{
   if (ppProperties != NULL) {
      m_propertyBag->AddReference();
      *ppProperties = m_propertyBag;
      return true;
   }
   return false;
}

} // namespace gubi

