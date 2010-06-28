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
#include "OverlayDrawing.h"
#include "PALMachineTypes.h"
#include "ImageSpec.h"
#include "OverlayItemZoomSpec.h"
#include "OverlayItemVisualSpec.h"
#include "OverlayItemInternal.h"
#include "MapPlotter.h"
#include "WFString.h"
#include "OverlayDrawer.h"

#include "DebugDrawing.h"

namespace OverlayDrawing {

void drawOverlayItem(const OverlayItemInternal* item,
                     const WFAPI::OverlayItemVisualSpec* visualSpec,
                     const WFAPI::WFString& itemName,
                     /*WFAPI::TextAlignment nameAlignment,*/
                     const WFAPI::WFString& nbrItems,
                     /*WFAPI::TextAlignment nbrItemsAlignment,*/
                     const MC2Point& topLeftPos,
                     OverlayDrawer* drawer)
{
   if (item == NULL || visualSpec == NULL || drawer == NULL){
      // We are unable to draw anything!       
      return;
   }
   
   // Get the background image of the spec
   const WFAPI::ImageSpec* background = visualSpec->getBackgroundImage();

   // Get the overlay icon that will be drawn atop the background
   const WFAPI::ImageSpec* overlayIcon =
      item->getImageSpec(visualSpec->getImageId());

   if(!background) {
      return;
   }
   
   // Start by drawing the background. Its position is set to topLeftPos
   drawer->drawImageSpec(background, topLeftPos);

   isab::Rectangle bgRect(topLeftPos.getX(), topLeftPos.getY(),
                          background->getWidth(), background->getHeight());
   
   // Now, draw the overlay icon
   if(overlayIcon) {
      WFAPI::wf_uint32 overlayImgWidth = overlayIcon->getWidth();
      WFAPI::wf_uint32 overlayImgHeight = overlayIcon->getHeight();
      
      MC2Point overlayImgOffset(visualSpec->getOverlayImageCenterPos());

      overlayImgOffset.getX() -= overlayImgWidth / 2;
      overlayImgOffset.getY() -= overlayImgHeight / 2;
                                 
      drawer->drawImageSpec(overlayIcon,
                            topLeftPos + overlayImgOffset);         
   }

   MC2Point textPoint;

   if(visualSpec->getTextUpperLeftPoint() !=
      visualSpec->getTextLowerRightPoint())
   {
      textPoint = (topLeftPos + visualSpec->getTextUpperLeftPoint() );
         
      WFAPI::wf_uint32 textRectWidth =
         visualSpec->getTextLowerRightPoint().getX() - 
         visualSpec->getTextUpperLeftPoint().getX();
         
      WFAPI::wf_uint32 textRectHeight =
         visualSpec->getTextLowerRightPoint().getY() - 
         visualSpec->getTextUpperLeftPoint().getY();

      isab::Rectangle stringRect(textPoint.getX(),
                                 textPoint.getY(),
                                 textRectWidth, textRectHeight);
      
      drawer->drawOverlayText(itemName,
                              item->m_numberBoxCache,
                              visualSpec->getTextAlignment(),
                              visualSpec->getTextFont(),
                              stringRect,
                              visualSpec->getTextColor());

      // DebugDrawing::rectangle(stringRect, (isab::MapPlotter*)drawer);
   }

   if (visualSpec->getNumberUpperLeftPoint() !=
       visualSpec->getNumberLowerRightPoint()) {
      textPoint = (topLeftPos + visualSpec->getNumberUpperLeftPoint());

      WFAPI::wf_uint32 textRectWidth =
         visualSpec->getNumberLowerRightPoint().getX() - 
         visualSpec->getNumberUpperLeftPoint().getX();

      WFAPI::wf_uint32 textRectHeight =
         visualSpec->getNumberLowerRightPoint().getY() - 
         visualSpec->getNumberUpperLeftPoint().getY();

      isab::Rectangle stringRect(textPoint.getX(),
                                 textPoint.getY(),
                                 textRectWidth, textRectHeight);
      
      drawer->drawOverlayText(nbrItems,
                              item->m_textBoxCache,
                              visualSpec->getNumberAlignment(),
                              visualSpec->getNumberFont(),
                              stringRect,
                              visualSpec->getNbrColor());
   }
   
   // DebugDrawing::rectangle(bgRect, (isab::MapPlotter*)drawer);
}

} // End of namespace WFAPI

