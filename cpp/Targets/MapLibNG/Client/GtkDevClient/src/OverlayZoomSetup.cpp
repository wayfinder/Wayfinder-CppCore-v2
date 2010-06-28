/*
Copyright (c) 1999 - 2010, Vodafone Group Services Ltd
All rights reserved.

Redistribution and use in source and binary forms, with or without modification, are permitted provided that the following conditions are met:

    * Redistributions of source code must retain the above copyright notice, this list of conditions and the following disclaimer.
    * Redistributions in binary form must reproduce the above copyright notice, this list of conditions and the following disclaimer in the documentation and/or other materials provided with the distribution.
    * Neither the name of the Vodafone Group Services Ltd nor the names of its contributors may be used to endorse or promote products derived from this software without specific prior written permission.

THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS" AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE ARE DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT HOLDER OR CONTRIBUTORS BE LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
*/


#include "Shared/OverlayItemZoomSpec.h"
#include "Shared/OverlayItemVisualSpec.h"
#include "Shared/ImageSpec.h"
#include "GtkUtil.h"
#include "OverlayZoomSetup.h"
#include <iostream>

namespace OverlayZoomSetup {

using namespace WFAPI;
using namespace OverlaySetupHelpers;

// For readabilitys sake
typedef ScreenPoint SP;

/**
 * This table contains the image specifications for
 * all the individual levels.
 */ 
BackgroundProperties customImages[] = {
   // Images and offsets for level 0, large
   { {"10_pm1_l_now_2945.png", SP(41, 114), SP(43, 44) },     //Normal
     {"10_pm5_l_now_2945.png", SP(41, 150), SP(43, 82) },     //Stacked
     {"10_pm1_l_now_2945_tapped.png", SP(41, 114), SP(43, 44) },     //NormalTapped
     {"10_pm5_l_now_2945_tapped.png", SP(41, 150), SP(43, 82) },     //StackedTapped
   },
   // Images and offsets for level 1, medium
   {  {"10_pm1_m_now_2945.png", SP(26, 64), SP(27, 31) },     //Normal
      {"10_pm2_m_now_2945.png", SP(25, 94), SP(31, 58) },     //Stacked
      {"10_pm1_m_now_2945_tapped.png", SP(26, 64), SP(27, 31) },     //NormalTapped
      {"10_pm2_m_now_2945_tapped.png", SP(25, 94), SP(31, 58) },     //StackedTapped
   },
   // Images and offsets for level 2, small
   { {"10_pm1_s_now_2945.png", SP(18, 45), SP(17, 20) },      //Normal
     {"10_pm2_s.png", SP(18, 45), SP(17, 20) },      //Stacked
     {"10_pm1_s_now_2945_tapped.png", SP(18, 45), SP(17, 20) },      //NormalTapped
     {"10_pm2_s_tapped.png", SP(18, 45), SP(17, 20) },      //StackedTapped
   },
};

OverlayItemVisualSpec::TextAlignment topLeftAlignment (
   OverlayItemVisualSpec::TEXT_HALIGN_LEFT,
   OverlayItemVisualSpec::TEXT_VALIGN_TOP);

OverlayItemVisualSpec::TextAlignment centerAlignment (
   OverlayItemVisualSpec::TEXT_HALIGN_CENTER,
   OverlayItemVisualSpec::TEXT_VALIGN_CENTER);

/**
 * This table contains the text specifications for all the individual
 * levels.
 */
TextProperties customTextProperties[] =
{
   // Text properties for level 0, large
   { { ScreenPoint(5,78), ScreenPoint(80,88),
       ScreenPoint(0, 0), ScreenPoint(0, 0),
       topLeftAlignment,
       centerAlignment,
       OverlayItemVisualSpec::RGB(0, 0, 0),
       OverlayItemVisualSpec::RGB(255, 255, 255) }, //Normal
     { ScreenPoint(6,117), ScreenPoint(79,127),
       ScreenPoint(62, 4), ScreenPoint(89, 27),
       topLeftAlignment,
       centerAlignment,
       OverlayItemVisualSpec::RGB(0, 0, 0),
       OverlayItemVisualSpec::RGB(255, 255, 255) }, }, //Stacked

   // Text properties for level 1, medium
   { { ScreenPoint(0, 0), ScreenPoint(0, 0),
       ScreenPoint(0, 0), ScreenPoint(0, 0),
       topLeftAlignment,
       centerAlignment,
       OverlayItemVisualSpec::RGB(0, 0, 0),
       OverlayItemVisualSpec::RGB(255, 255, 255) }, //Normal
     { ScreenPoint(0, 0), ScreenPoint(0, 0),
       ScreenPoint(37, 0), ScreenPoint(61, 23),
       topLeftAlignment,
       centerAlignment,
       OverlayItemVisualSpec::RGB(0, 0, 0),
       OverlayItemVisualSpec::RGB(255, 255, 255) } }, //Stacked

   // Text properties for level 2, small
   { { ScreenPoint(0, 0), ScreenPoint(0, 0),
       ScreenPoint(0, 0), ScreenPoint(0, 0),
       topLeftAlignment,
       centerAlignment,
       OverlayItemVisualSpec::RGB(0, 0, 0),
       OverlayItemVisualSpec::RGB(255, 255, 255) }, //Normal
     { ScreenPoint(0, 0), ScreenPoint(0, 0),
       ScreenPoint(7, 10), ScreenPoint(30, 32),
       topLeftAlignment,
       centerAlignment,
       OverlayItemVisualSpec::RGB(0, 0, 0),
       OverlayItemVisualSpec::RGB(0, 0, 0) } }, //Stacked
};

/*
 * This table contains the zoom range settings for our four different
 * levels.
 */ 
ZoomRange customZoomLevels[] = {
   {0,  2},                   //Level 0
   {3, 15},                   //Level 1
   {16, WFAPI::WF_MAX_INT32}, //Level 2
   // {36, WFAPI::WF_MAX_INT32},  //Level 3
};

OverlayItemZoomSpec* createZoomSpec(const char* pathToResources)
{
   // The number of items is a compile time constant.
   wf_uint32 numElements = sizeof(customZoomLevels) / sizeof(ZoomRange);

   // Start by allocating a zoom specification, which we will fill with
   // visual specs for the various zoom ranges
   OverlayItemZoomSpec* zoomSpec = OverlayItemZoomSpec::allocate();

   FontSpec* nameFont = FontSpec::allocate("Vodafone RgBd 18px", 18);
   FontSpec* nbrFont = FontSpec::allocate("Vodafone Rg 24px", 24);
   
   for(wf_uint32 i = 0; i < numElements; i++) {
      
      BackgroundProperties& bgProps = customImages[i];
      
      // Create image specifications for all states
      ImageSpec* normalImg =
         GtkUtil::createImageSpec(pathToResources, bgProps.normal.path);
      ImageSpec* stackedImg =
         GtkUtil::createImageSpec(pathToResources, bgProps.stacked.path);
      ImageSpec* normalTappedImg =
         GtkUtil::createImageSpec(pathToResources, bgProps.normalTapped.path);
      ImageSpec* stackedTappedImg =
         GtkUtil::createImageSpec(pathToResources, bgProps.stackedTapped.path);

      // Create visual specifications for all states using image
      // information
      
      
      OverlayItemVisualSpec* normalSpec =
         OverlayItemVisualSpec::allocate(normalImg,
                                         bgProps.normal.focusPoint,
                                         bgProps.normal.imagePoint,
                                         i);

      OverlayItemVisualSpec* stackedSpec =
         OverlayItemVisualSpec::allocate(stackedImg,
                                         bgProps.stacked.focusPoint,
                                         bgProps.stacked.imagePoint,
                                         i);

      OverlayItemVisualSpec* normalTappedSpec =
         OverlayItemVisualSpec::allocate(normalTappedImg,
                                         bgProps.normal.focusPoint,
                                         bgProps.normal.imagePoint,
                                         i);

      OverlayItemVisualSpec* stackedTappedSpec =
         OverlayItemVisualSpec::allocate(stackedTappedImg,
                                         bgProps.stacked.focusPoint,
                                         bgProps.stacked.imagePoint,
                                         i);
   
      // Setup our custom text properties
      TextProperties textProperties = customTextProperties[i];
         
      // Set the text properties for the normal visual spec
      normalSpec->setTextProperties(textProperties.normal.nameUpperLeft,
                                    textProperties.normal.nameLowerRight,
                                    textProperties.normal.nbrUpperLeft,
                                    textProperties.normal.nbrLowerRight,
                                    textProperties.normal.textAlignment,
                                    textProperties.normal.numberAlignment,
                                    textProperties.normal.nameColor,
                                    textProperties.normal.nbrColor,
                                    nameFont,
                                    nbrFont);

      // Set the text properties for the stacked visual spec
      stackedSpec->setTextProperties(textProperties.stacked.nameUpperLeft,
                                     textProperties.stacked.nameLowerRight,
                                     textProperties.stacked.nbrUpperLeft,
                                     textProperties.stacked.nbrLowerRight,
                                     textProperties.normal.textAlignment,
                                     textProperties.normal.numberAlignment,
                                     textProperties.normal.nameColor,
                                     textProperties.normal.nbrColor,
                                     nameFont,
                                     nbrFont);

      // Set the text properties for the normal visual spec
      normalTappedSpec->setTextProperties(textProperties.normal.nameUpperLeft,
                                    textProperties.normal.nameLowerRight,
                                    textProperties.normal.nbrUpperLeft,
                                    textProperties.normal.nbrLowerRight,
                                    textProperties.normal.textAlignment,
                                    textProperties.normal.numberAlignment,
                                    textProperties.normal.nameColor,
                                    textProperties.normal.nbrColor,
                                    nameFont,
                                    nbrFont);

      // Set the text properties for the stacked visual spec
      stackedTappedSpec->setTextProperties(textProperties.stacked.nameUpperLeft,
                                     textProperties.stacked.nameLowerRight,
                                     textProperties.stacked.nbrUpperLeft,
                                     textProperties.stacked.nbrLowerRight,
                                     textProperties.normal.textAlignment,
                                     textProperties.normal.numberAlignment,
                                     textProperties.normal.nameColor,
                                     textProperties.normal.nbrColor,
                                     nameFont,
                                     nbrFont);

      
      // Lookup the relevant zoom range
      ZoomRange z = customZoomLevels[i];
      
      zoomSpec->addZoomLevelRange(z.minZoom, z.maxZoom,
                                  normalSpec, stackedSpec,
                                  normalTappedSpec, stackedTappedSpec);

      // Remove references to used images and specifications
      normalImg->removeReference();
      stackedImg->removeReference();
      normalTappedImg->removeReference();
      stackedTappedImg->removeReference();

      normalSpec->removeReference();
      stackedSpec->removeReference();
      normalTappedSpec->removeReference();
      stackedTappedSpec->removeReference();
   }
   
   // The image with this id will be used for the highlighted
   // visual specs.
   const unsigned int HIGHLIGHTED_ICON_ID = numElements;

   ImageSpec* normalHighlightImg =
      GtkUtil::createImageSpec(pathToResources, 
                               "10_pm1_xl_now_2945.png");

   OverlayItemVisualSpec* normalHighlightSpec =
      OverlayItemVisualSpec::allocate(normalHighlightImg,
                                      SP(50, 128),
                                      SP(53, 45),
                                      HIGHLIGHTED_ICON_ID);

   normalHighlightSpec->setTextProperties(
      SP(4, 91), SP(98, 106),
      topLeftAlignment,
      OverlayItemVisualSpec::RGB(255, 255, 255),
      nameFont);

   ImageSpec* normalTappedHighlightImg =
      GtkUtil::createImageSpec(pathToResources, 
                               "10_pm1_xl_now_2945_tapped.png");

   OverlayItemVisualSpec* normalTappedHighlightSpec =
      OverlayItemVisualSpec::allocate(normalTappedHighlightImg,
                                      SP(50, 128),
                                      SP(53, 45),
                                      HIGHLIGHTED_ICON_ID);

   normalTappedHighlightSpec->setTextProperties(
      SP(4, 91), SP(98, 106),
      topLeftAlignment,
      OverlayItemVisualSpec::RGB(255, 255, 255),
      nameFont);

   ImageSpec* stackedHighlightImg =
      GtkUtil::createImageSpec(pathToResources, 
            "10_pm5_xl_now_2945.png");

   OverlayItemVisualSpec* stackedHighlightSpec =
      OverlayItemVisualSpec::allocate(stackedHighlightImg,
                                      SP(50, 165),
                                      SP(49, 90),
                                      HIGHLIGHTED_ICON_ID);

   
   stackedHighlightSpec->setTextProperties(
      SP(5, 130), SP(97, 141),
      SP(81, 2), SP(110, 26),
      topLeftAlignment,
      centerAlignment,
      OverlayItemVisualSpec::RGB(255, 255, 255),
      OverlayItemVisualSpec::RGB(255, 255, 255),
      nameFont,
      nbrFont);

   ImageSpec* stackedTappedHighlightImg =
      GtkUtil::createImageSpec(pathToResources, 
                               "10_pm5_xl_now_2945_tapped.png");

   OverlayItemVisualSpec* stackedTappedHighlightSpec =
      OverlayItemVisualSpec::allocate(stackedTappedHighlightImg,
                                      SP(50, 165),
                                      SP(49, 90),
                                      HIGHLIGHTED_ICON_ID);

   
   stackedTappedHighlightSpec->setTextProperties(
      SP(5, 130), SP(97, 141),
      SP(81, 2), SP(110, 26),
      topLeftAlignment,
      centerAlignment,
      OverlayItemVisualSpec::RGB(255, 255, 255),
      OverlayItemVisualSpec::RGB(255, 255, 255),
      nameFont,
      nbrFont);

   zoomSpec->setHighlightedSpecs(normalHighlightSpec,
                                 stackedHighlightSpec,
                                 normalTappedHighlightSpec,
                                 stackedTappedHighlightSpec);
   
   normalHighlightImg->removeReference();
   normalHighlightSpec->removeReference();
   normalTappedHighlightImg->removeReference();
   normalTappedHighlightSpec->removeReference();

   stackedHighlightImg->removeReference();
   stackedHighlightSpec->removeReference();
   stackedTappedHighlightImg->removeReference();
   stackedTappedHighlightSpec->removeReference();
   
   nameFont->removeReference();
   nbrFont->removeReference();
   
   return zoomSpec;
}

} // End of namespace WFAPI

