/*
Copyright (c) 1999 - 2010, Vodafone Group Services Ltd
All rights reserved.

Redistribution and use in source and binary forms, with or without modification, are permitted provided that the following conditions are met:

    * Redistributions of source code must retain the above copyright notice, this list of conditions and the following disclaimer.
    * Redistributions in binary form must reproduce the above copyright notice, this list of conditions and the following disclaimer in the documentation and/or other materials provided with the distribution.
    * Neither the name of the Vodafone Group Services Ltd nor the names of its contributors may be used to endorse or promote products derived from this software without specific prior written permission.

THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS" AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE ARE DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT HOLDER OR CONTRIBUTORS BE LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
*/


#include "CoreFunctionPrologue.h"
#include "StackedDialogVisualSpec.h"
#include "ImageSpec.h"
#include "OverlayItemVisualSpec.h"
#include "ScreenPoint.h"


namespace WFAPI {

StackedDialogVisualSpec::StackedDialogVisualSpec(
   const ImageSpec* background,
   const ScreenPoint& focusPoint,
   const ScreenPoint& itemsUpperLeftPoint,
   const ScreenPoint& itemsLowerRightPoint,
   const ImageSpec* leftArrowImg,
   const ScreenPoint& leftArrowCenterPos,
   const ImageSpec* rightArrowImg,
   const ScreenPoint& rightArrowCenterPos,
   const OverlayItemVisualSpec* visualSpec,
   const OverlayItemVisualSpec* tappedVisualSpec)
   : m_background(background),
     m_itemsUpperLeftPoint(itemsUpperLeftPoint),
     m_itemsLowerRightPoint(itemsLowerRightPoint),
     m_leftArrowImg(leftArrowImg),
     m_leftArrowCenterPos(leftArrowCenterPos),
     m_rightArrowImg(rightArrowImg),
     m_rightArrowCenterPos(rightArrowCenterPos),
     m_focusPoint(focusPoint),
     m_visualSpec(visualSpec),
     m_tappedVisualSpec(tappedVisualSpec)
{
   CORE_FUNCTION_PROLOGUE();
   
   m_background->addReference();   

   if(m_leftArrowImg){
      m_leftArrowImg->addReference();
   }
   if(m_rightArrowImg){
      m_rightArrowImg->addReference();
   }
   
   m_visualSpec->addReference();
   m_tappedVisualSpec->addReference();
}


StackedDialogVisualSpec*
StackedDialogVisualSpec::allocate(const ImageSpec* background,
                                  const ScreenPoint& focusPoint,
                                  const ScreenPoint& itemsUpperLeftPoint,
                                  const ScreenPoint& itemsLowerRightPoint,
                                  const ImageSpec* leftArrowImg,
                                  const ScreenPoint& leftArrowCenterPos,
                                  const ImageSpec* rightArrowImg,
                                  const ScreenPoint& rightArrowCenterPos,
                                  const OverlayItemVisualSpec* visualSpec)
{
   CORE_FUNCTION_PROLOGUE();
   
   return new StackedDialogVisualSpec(background,
                                      focusPoint,
                                      itemsUpperLeftPoint,
                                      itemsLowerRightPoint,
                                      leftArrowImg,
                                      leftArrowCenterPos,
                                      rightArrowImg,
                                      rightArrowCenterPos,
                                      visualSpec,
                                      visualSpec);
}

StackedDialogVisualSpec* WFAPI::StackedDialogVisualSpec::allocate(
   const ImageSpec* dialogBackground,
   const ScreenPoint& focusPoint,
   const ScreenPoint& itemsUpperLeftPoint,
   const ScreenPoint& itemsLowerRightPoint,
   const ImageSpec* leftArrowImg,
   const ScreenPoint& leftArrowCenterPos,
   const ImageSpec* rightArrowImg,
   const ScreenPoint& rightArrowCenterPos,
   const OverlayItemVisualSpec* visualSpec,
   const OverlayItemVisualSpec* tappedVisualSpec)
{
   CORE_FUNCTION_PROLOGUE();
   
   return new StackedDialogVisualSpec(dialogBackground,
                                      focusPoint,
                                      itemsUpperLeftPoint,
                                      itemsLowerRightPoint,
                                      leftArrowImg,
                                      leftArrowCenterPos,
                                      rightArrowImg,
                                      rightArrowCenterPos,
                                      visualSpec,
                                      tappedVisualSpec);

}



StackedDialogVisualSpec::~StackedDialogVisualSpec()
{
   CORE_FUNCTION_PROLOGUE();
   
   m_background->removeReference();
   if(m_leftArrowImg){
      m_leftArrowImg->removeReference();
   }
   if(m_rightArrowImg){ 
      m_rightArrowImg->removeReference();     
   }
   m_visualSpec->removeReference();
   m_tappedVisualSpec->removeReference();
}

ScreenPoint
StackedDialogVisualSpec::getLeftArrowCenterPos() const
{
   CORE_FUNCTION_PROLOGUE();
   
   return m_leftArrowCenterPos;
}

ScreenPoint
StackedDialogVisualSpec::getRightArrowCenterPos() const
{
   CORE_FUNCTION_PROLOGUE();
   
   return m_rightArrowCenterPos;
}

const OverlayItemVisualSpec*
StackedDialogVisualSpec::getItemVisualSpec() const
{
   CORE_FUNCTION_PROLOGUE();
   
   return m_visualSpec;
}

const OverlayItemVisualSpec*
StackedDialogVisualSpec::getTappedItemVisualSpec() const
{
   CORE_FUNCTION_PROLOGUE();
   
   return m_tappedVisualSpec;
}


const ImageSpec*
StackedDialogVisualSpec::getLeftArrowImage() const
{
   CORE_FUNCTION_PROLOGUE();
   
   return m_leftArrowImg; 
}

const ImageSpec*
StackedDialogVisualSpec::getRightArrowImage() const
{
   CORE_FUNCTION_PROLOGUE();
   
   return m_rightArrowImg; 
}

const ImageSpec*
StackedDialogVisualSpec::getDialogBackgroundImage() const
{
   CORE_FUNCTION_PROLOGUE();
   
   return m_background;
}

ScreenPoint WFAPI::StackedDialogVisualSpec::getItemsUpperLeftPoint() const
{
   CORE_FUNCTION_PROLOGUE();
   
   return m_itemsUpperLeftPoint; 
}

ScreenPoint WFAPI::StackedDialogVisualSpec::getItemsLowerRightPoint() const
{
   CORE_FUNCTION_PROLOGUE();
   
   return m_itemsLowerRightPoint; 
}

ScreenPoint WFAPI::StackedDialogVisualSpec::getFocusPoint() const
{
   CORE_FUNCTION_PROLOGUE();
   
   return m_focusPoint;
}

} // End of namespace WFAPI


