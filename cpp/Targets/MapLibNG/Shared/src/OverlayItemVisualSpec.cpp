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
#include "OverlayItemVisualSpec.h"
#include "ImageSpec.h"


namespace WFAPI {

OverlayItemVisualSpec*
OverlayItemVisualSpec::allocate(const ImageSpec* backgroundImage,
                                const ScreenPoint& focusPoint,
                                const ScreenPoint& imgCenterPoint,
                                wf_uint32 imgId)
{
   CORE_FUNCTION_PROLOGUE();
   
   return new OverlayItemVisualSpec(backgroundImage,
                                    focusPoint,
                                    imgCenterPoint,
                                    imgId);
}

OverlayItemVisualSpec*
OverlayItemVisualSpec::allocate(const ImageSpec* backgroundImage,
                                const ScreenPoint& focusPoint)
{
   CORE_FUNCTION_PROLOGUE();
   
   return new OverlayItemVisualSpec(backgroundImage,
                                    focusPoint,
                                    ScreenPoint(0,0),
                                    WF_MAX_UINT32);
}

OverlayItemVisualSpec::OverlayItemVisualSpec(const ImageSpec* backgroundImage,
                                             const ScreenPoint& focusPoint,
                                             const ScreenPoint& imgCenterPoint,
                                             wf_uint32 imgId) :
   m_focusPoint(focusPoint),
   m_overlayImgCenterPos(imgCenterPoint),
   m_textUpperLeftPoint(0, 0),
   m_textLowerRightPoint(0, 0),
   m_numberUpperLeftPoint(0, 0),
   m_numberLowerRightPoint(0, 0),
   m_backgroundImg(backgroundImage),
   m_imgId(imgId),
   m_textColor(),
   m_nbrColor(),
   m_textFont(NULL),
   m_nbrFont(NULL)

{
   CORE_FUNCTION_PROLOGUE();
   
   if(m_backgroundImg) {
      m_backgroundImg->addReference();      
   }
}

WFAPI::OverlayItemVisualSpec::~OverlayItemVisualSpec()
{
   CORE_FUNCTION_PROLOGUE();
   
   if(m_backgroundImg) {
      m_backgroundImg->removeReference();      
   }
   
   if(m_textFont) {
      m_textFont->removeReference();
   }

   if(m_nbrFont) {
      m_nbrFont->removeReference();
   }
}

void OverlayItemVisualSpec::setTextProperties(
   const ScreenPoint& upperLeftPoint,
   const ScreenPoint& lowerRightPoint,
   const TextAlignment& textAlignment,
   const RGB& textColor,
   FontSpec* textFont)
{
   CORE_FUNCTION_PROLOGUE();
   
   if(m_textFont) {
      m_textFont->removeReference();
      m_textFont = NULL;
   }

   if(m_nbrFont) {
      m_nbrFont->removeReference();
      m_nbrFont = NULL;
   }
   
   m_textUpperLeftPoint = upperLeftPoint;
   m_textLowerRightPoint = lowerRightPoint;
   m_textAlignment = textAlignment;
   m_textColor = textColor;
   m_textFont = textFont;

   if(m_textFont) {
      m_textFont->addReference();
   }
}

void OverlayItemVisualSpec::setTextProperties(
   const ScreenPoint& upperLeftPoint,
   const ScreenPoint& lowerRightPoint,
   const ScreenPoint& numberUpperLeftPoint,
   const ScreenPoint& numberLowerRightPoint,
   const TextAlignment& textAlignment,
   const TextAlignment& numberAlignment,
   const RGB& textColor,
   const RGB& nbrColor,
   FontSpec* textFont,
   FontSpec* nbrFont)
{
   CORE_FUNCTION_PROLOGUE();
   
   if(m_textFont) {
      m_textFont->removeReference();
      m_textFont = NULL;
   }

   if(m_nbrFont) {
      m_nbrFont->removeReference();
      m_nbrFont = NULL;
   }
   
   m_textUpperLeftPoint = upperLeftPoint;
   m_textLowerRightPoint = lowerRightPoint;
   m_numberUpperLeftPoint = numberUpperLeftPoint;
   m_numberLowerRightPoint = numberLowerRightPoint;
   m_textAlignment = textAlignment;
   m_numberAlignment = numberAlignment;
   m_textColor = textColor;
   m_nbrColor = nbrColor;
   m_textFont = textFont;
   m_nbrFont = nbrFont;

   if(m_textFont) {
      m_textFont->addReference();
   }

   if(m_nbrFont) {
      m_nbrFont->addReference();
   }
}

const ScreenPoint& OverlayItemVisualSpec::getFocusPoint() const
{
   CORE_FUNCTION_PROLOGUE();
   
   return m_focusPoint;
}

const ImageSpec* OverlayItemVisualSpec::getBackgroundImage() const
{
   CORE_FUNCTION_PROLOGUE();
   
   return m_backgroundImg;
}

const ScreenPoint& OverlayItemVisualSpec::getOverlayImageCenterPos() const
{
   CORE_FUNCTION_PROLOGUE();
   
   return m_overlayImgCenterPos;
}

const ScreenPoint& OverlayItemVisualSpec::getTextUpperLeftPoint() const
{
   CORE_FUNCTION_PROLOGUE();
   
   return m_textUpperLeftPoint;
}

const ScreenPoint& OverlayItemVisualSpec::getTextLowerRightPoint() const
{
   CORE_FUNCTION_PROLOGUE();
   
   return m_textLowerRightPoint;
}

const ScreenPoint& OverlayItemVisualSpec::getNumberUpperLeftPoint() const
{
   CORE_FUNCTION_PROLOGUE();
   
   return m_numberUpperLeftPoint;
}

const ScreenPoint& OverlayItemVisualSpec::getNumberLowerRightPoint() const
{
   CORE_FUNCTION_PROLOGUE();
   
   return m_numberLowerRightPoint;
}

wf_uint32 OverlayItemVisualSpec::getImageId() const
{
   CORE_FUNCTION_PROLOGUE();
   
   return m_imgId; 
}

const OverlayItemVisualSpec::RGB&
OverlayItemVisualSpec::getTextColor() const
{
   CORE_FUNCTION_PROLOGUE();
   
   return m_textColor;
}

const OverlayItemVisualSpec::RGB&
OverlayItemVisualSpec::getNbrColor() const
{
   CORE_FUNCTION_PROLOGUE();
   
   return m_nbrColor;
}

const OverlayItemVisualSpec::TextAlignment&
OverlayItemVisualSpec::getTextAlignment() const
{
   CORE_FUNCTION_PROLOGUE();
   
   return m_textAlignment;
}

const OverlayItemVisualSpec::TextAlignment&
OverlayItemVisualSpec::getNumberAlignment() const
{
   CORE_FUNCTION_PROLOGUE();
   
   return m_numberAlignment;
}

FontSpec* OverlayItemVisualSpec::getTextFont() const
{
   CORE_FUNCTION_PROLOGUE();
   
   return m_textFont;
}

FontSpec* OverlayItemVisualSpec::getNumberFont() const
{
   CORE_FUNCTION_PROLOGUE();
   
   return m_nbrFont; 
}

} // End of namespace WFAPI





