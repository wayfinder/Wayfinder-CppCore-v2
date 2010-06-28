/*
Copyright (c) 1999 - 2010, Vodafone Group Services Ltd
All rights reserved.

Redistribution and use in source and binary forms, with or without modification, are permitted provided that the following conditions are met:

    * Redistributions of source code must retain the above copyright notice, this list of conditions and the following disclaimer.
    * Redistributions in binary form must reproduce the above copyright notice, this list of conditions and the following disclaimer in the documentation and/or other materials provided with the distribution.
    * Neither the name of the Vodafone Group Services Ltd nor the names of its contributors may be used to endorse or promote products derived from this software without specific prior written permission.

THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS" AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE ARE DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT HOLDER OR CONTRIBUTORS BE LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
*/



#include "View/IViewInternal.h"
#include "Property/IProperty.h"
#include "Property/IPropertyBag.h"
#include "Renderer/IImage.h"
#include "Renderer/RenderNode.h"
#include "Renderer/IRenderBridge.h"
#include "Renderer/RenderQueue.h"
#include "Renderer/RenderFunction.h"
#include "Renderer/RenderSystem.h"

namespace gubi {

RenderSystem::RenderSystem(IRenderBridge *bridge)
      :m_bridge(bridge)
{
}

RenderSystem::~RenderSystem()
{
   shutdown();
}

bool RenderSystem::init()
{
   bool result = false;
   
   return result;
}

void RenderSystem::shutdown()
{
   m_bridge = NULL;
}

int RenderSystem::addView(IViewInternal *view)
{
   int result = 0;
   
   if (view != NULL) {
      view->AddReference();
      m_viewList.push_back(view);
      result = 1;
   }
   
   return result;
}

int RenderSystem::removeView(IViewInternal *view)
{
   int result = 0;
   
   if (view != NULL) {
      std::list<IViewInternal*>::iterator it;
      it = m_viewList.begin();

      while (it != m_viewList.end()) {
         
         if ((*it) == view) {
            m_viewList.erase(it);
            view->Release();
            it = m_viewList.end();
            result = 1;
         } else {
            ++it;
         }
      }
   }
   
   return result;
}

bool RenderSystem::update()
{
   bool nodeupdate = false;
   
   for (std::list<IViewInternal*>::iterator it = m_viewList.begin();
        it != m_viewList.end();
        ++it) {
      if ((*it)->Visible()) {
         if ((*it)->FetchNodes(ViewSpace_3d, m_renderQueue[0]) &&
             (*it)->FetchNodes(ViewSpace_Overlay, m_renderQueue[1]) &&
             !nodeupdate) {
            nodeupdate = true;
         }
      }      
   }

   return nodeupdate;
}

void RenderSystem::render(
   void )
{
   if (m_bridge != NULL)
   {   
      m_bridge->begin(RenderBridgeViewSpace_3D);
      
      m_renderQueue[0].render();

      m_bridge->end();
      
      m_bridge->begin(RenderBridgeViewSpace_2D);
      
      m_renderQueue[1].render();

      m_bridge->end();
   }
}

bool RenderSystem::GetRenderFunction(
   const gubi::Identifier& eRenderId,
   IRenderFunction** ppRenderFunction)
{
   bool bReturn = false;

   if (ppRenderFunction != NULL) {   
      switch (eRenderId)
      {
         case 3396036235: // gubi.render.image
         {
            *ppRenderFunction = CreateRenderFunction(this, &RenderSystem::renderImage);
            bReturn           = true;
         } break;
      }      
   }

   return bReturn;
}

void RenderSystem::renderImage(const RenderNode *node)
{
   /*
     TODO:
     use RenderBride to map agains WFAPI::MapDrawingInterface
     that has drawImageSpec()
     this takes : image, matrix, width, height, color

     image : WFAPI::ImageSpec
     matrix : int[4][4]
     width : unsigned int
     height : unsigned int
     color : int[4]
    */

   if (m_bridge != NULL && node != NULL) {
      // pull properties from node
      const IProperty *prop = NULL;
      const IPropertyBag *bag = NULL;
      fmath::fixed width = 0;
      fmath::fixed height = 0;
      
      if (node->GetProperties(&bag)) {
         IImage *image = NULL;

         if (bag->Find(L"width", &prop)) {
            GetDataByType(prop, &width);
            prop->Release();
         }
         if (bag->Find(L"height", &prop)) {
            GetDataByType(prop, &height);
            prop->Release();
         }
         
         if (bag->Find(L"image", &prop)) {
            if (GetDataByType(prop, &image)) {
               if (width == 0 && height == 0) {
                  width = fmath::tofixed(static_cast<int>(image->getWidth()));
                  height = fmath::tofixed(static_cast<int>(image->getHeight()));
               }
               
               // push the matrix
               m_bridge->pushMatrix(node->getMatrix());

               // the image from the widget
               m_bridge->bindImage(image);

               // draw the image
               m_bridge->drawImage(width, height);
            }
            prop->Release();
         }
         bag->Release();
      }
   }
}

} // namespace gubi
