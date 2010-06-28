/*
Copyright (c) 1999 - 2010, Vodafone Group Services Ltd
All rights reserved.

Redistribution and use in source and binary forms, with or without modification, are permitted provided that the following conditions are met:

    * Redistributions of source code must retain the above copyright notice, this list of conditions and the following disclaimer.
    * Redistributions in binary form must reproduce the above copyright notice, this list of conditions and the following disclaimer in the documentation and/or other materials provided with the distribution.
    * Neither the name of the Vodafone Group Services Ltd nor the names of its contributors may be used to endorse or promote products derived from this software without specific prior written permission.

THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS" AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE ARE DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT HOLDER OR CONTRIBUTORS BE LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
*/


#include "config.h"
#include "OverlayItemUtil.h"
#include "OverlayItemVisualSpec.h"
#include "OverlayItemInternal.h"
#include "OverlayItemZoomSpec.h"


const WFAPI::OverlayItemVisualSpec*
OverlayItemUtil::getCurrentVisualSpec(const OverlayItemInternal* item,
                                      WFAPI::wf_uint32 curPixelScale)
{
   
   const WFAPI::OverlayItemVisualSpec* ret = NULL;
   const WFAPI::OverlayItemZoomSpec* zoomSpec = item->getZoomSpec();
   
   if (item->m_isHighlighted){
      if (item->m_stack.getSize() > 1){
         if (item->m_isBeingTapped){
            ret = zoomSpec->getStackedTappedHighlightedSpec();
         } else {
            ret = zoomSpec->getStackedHighlightedSpec();
         }
      } else {
         if (item->m_isBeingTapped){
            ret = zoomSpec->getNormalTappedHighlightedSpec();
         } else {
            ret = zoomSpec->getNormalHighlightedSpec();
         }
      }
   } else {
      if (item->m_stack.getSize() > 1){
         if (item->m_isBeingTapped){
            ret = zoomSpec->getStackedTappedSpec(curPixelScale);            
         } else {
            ret = zoomSpec->getStackedSpec(curPixelScale);            
         }

      } else {
         if (item->m_isBeingTapped){
            ret = zoomSpec->getNormalTappedSpec(curPixelScale);            
         } else {
            ret = zoomSpec->getNormalSpec(curPixelScale);            
         }
      }
   }
   return ret;
}

void OverlayItemUtil::verifyStack(const OverlayItemInternal* item)
{
   const OverlayItemInternal* curItem = item;
   
   unsigned int numItems = 0;
   
   while(curItem) {
      curItem = curItem->m_stack.getNext();
      numItems++;
   }
   
   MC2_ASSERT(item->m_stack.getSize() == numItems);
}
