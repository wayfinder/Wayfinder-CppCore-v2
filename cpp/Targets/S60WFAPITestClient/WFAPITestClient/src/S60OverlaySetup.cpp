/*
Copyright (c) 1999 - 2010, Vodafone Group Services Ltd
All rights reserved.

Redistribution and use in source and binary forms, with or without modification, are permitted provided that the following conditions are met:

    * Redistributions of source code must retain the above copyright notice, this list of conditions and the following disclaimer.
    * Redistributions in binary form must reproduce the above copyright notice, this list of conditions and the following disclaimer in the documentation and/or other materials provided with the distribution.
    * Neither the name of the Vodafone Group Services Ltd nor the names of its contributors may be used to endorse or promote products derived from this software without specific prior written permission.

THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS" AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE ARE DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT HOLDER OR CONTRIBUTORS BE LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
*/


#include "S60OverlaySetup.h"
#include "S60OverlayHelpers.h"
#include "OverlayItemZoomSpec.h"
#include <string>
#include "SymbianTextUtil.h"
#include "Symbian/SymbianFactory.h"
#include "Shared/StackedDialogVisualSpec.h"

// void testCropping();

namespace S60OverlaySetup {

using namespace WFAPI;
using namespace OverlaySetupHelpers;

// For readabilitys sake
typedef ScreenPoint SP;

static OverlayItemVisualSpec::TextAlignment topLeftAlignment (
   OverlayItemVisualSpec::TEXT_HALIGN_LEFT,
   OverlayItemVisualSpec::TEXT_VALIGN_TOP);

static OverlayItemVisualSpec::TextAlignment centerAlignment (
   OverlayItemVisualSpec::TEXT_HALIGN_CENTER,
   OverlayItemVisualSpec::TEXT_VALIGN_CENTER);

BackgroundProperties customImages[] = {
   // Images and offsets for level 0, large
   { {"overlay-item-bg.mif", SP(19, 32), SP(16, 18) },             //Normal
     {"overlay-item-bg-stacked.mif", SP(20, 47), SP(17, 25) },     //Stacked
   },
   // Images and offsets for level 1, medium
   {  {"overlay-item-bg.mif", SP(19, 42), SP(17, 18) },             //Normal
      {"overlay-item-bg-stacked.mif", SP(20, 47), SP(17, 25) },     //Stacked
   },
   // Images and offsets for level 2, small
   { {"overlay-item-bg.mif", SP(19, 42), SP(17, 18) },              //Normal
     {"overlay-item-bg-stacked.mif", SP(20, 47), SP(17, 25) },      //Stacked
   },
};

/**
 * This table contains the text specifications for all the individual
 * levels.
 */
TextProperties customTextProperties[] =
{
   // Text properties for level 0, large
   { { SP(1,36), SP(34,41),
       SP(32, 2), SP(40, 11),
       topLeftAlignment,
       centerAlignment,
       OverlayItemVisualSpec::RGB(0, 0, 0),
       OverlayItemVisualSpec::RGB(0, 0, 0) }, //Normal
     { SP(1,41), SP(34,46),
       SP(32, 2), SP(40, 11),
       topLeftAlignment,
       centerAlignment,
       OverlayItemVisualSpec::RGB(0, 0, 0),
       OverlayItemVisualSpec::RGB(0, 0, 0) }, }, //Stacked

   // Text properties for level 1, medium
   { { SP(1,36), SP(34,41),
       SP(32, 2), SP(40, 11),
       topLeftAlignment,
       centerAlignment,
       OverlayItemVisualSpec::RGB(0, 0, 0),
       OverlayItemVisualSpec::RGB(0, 0, 0) }, //Normal
     { SP(1,41), SP(34,46),
       SP(32, 2), SP(40, 11),
       topLeftAlignment,
       centerAlignment,
       OverlayItemVisualSpec::RGB(0, 0, 0),
       OverlayItemVisualSpec::RGB(0, 0, 0) }, }, //Stacked


   // Text properties for level 2, small
   { { SP(1,36), SP(34,41),
       SP(32, 2), SP(40, 11),
       topLeftAlignment,
       centerAlignment,
       OverlayItemVisualSpec::RGB(0, 0, 0),
       OverlayItemVisualSpec::RGB(0, 0, 0) }, //Normal
     { SP(1,41), SP(34,46),
       SP(32, 2), SP(40, 11),
       topLeftAlignment,
       centerAlignment,
       OverlayItemVisualSpec::RGB(0, 0, 0),
       OverlayItemVisualSpec::RGB(0, 0, 0) }, }, //Stacked
};

/* This table contains the zoom range settings for our four different
* levels.
*/ 
ZoomRange customZoomLevels[] = {
      {0,  2},                   //Level 0
      {3, 15},                   //Level 1
      {16, WFAPI::WF_MAX_INT32}, //Level 2
};

WFAPI::OverlayItemZoomSpec*
S60OverlaySetup::createZoomSpec(const char* pathToResources)
{
   // testCropping();
   
   std::string extPath(pathToResources);
   
   extPath += "overlay";

   // Start by allocating a zoom specification, which we will fill with
   // visual specs for the various zoom ranges
   OverlayItemZoomSpec* zoomSpec = OverlayItemZoomSpec::allocate();

   // Then, allocate the fonts which we'll use
   FontSpec* nameFont = FontSpec::allocate("LatinPlain10", 10);
   FontSpec* nbrFont = FontSpec::allocate("LatinPlain10", 10);

   // The number of items is a compile time constant.
   wf_uint32 numElements = sizeof(customZoomLevels) / sizeof(ZoomRange);

   for(wf_uint32 i = 0; i < numElements; i++) {
      
      BackgroundProperties& bgProps = customImages[i];
      
      // Create image specifications for all states
      ImageSpec* normalImg =
         S60OverlayHelpers::loadMif(extPath, bgProps.normal.path);
      ImageSpec* stackedImg =
         S60OverlayHelpers::loadMif(extPath, bgProps.stacked.path);

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

      // Lookup the relevant zoom range
      ZoomRange z = customZoomLevels[i];
      
      zoomSpec->addZoomLevelRange(z.minZoom, z.maxZoom,
                                  normalSpec, stackedSpec);

      // Remove references to used images and specifications
      normalImg->removeReference();
      stackedImg->removeReference();

      normalSpec->removeReference();
      stackedSpec->removeReference();
   }

   // The image with this id will be used for the highlighted
   // visual specs.
   const unsigned int HIGHLIGHTED_ICON_ID = numElements;
   
   ImageSpec* normalHighlightImg =
      S60OverlayHelpers::loadMif(extPath, "overlay-item-bg-highlighted.mif");

   OverlayItemVisualSpec* normalHighlightSpec =
      OverlayItemVisualSpec::allocate(normalHighlightImg,
                                      SP(19, 32),
                                      SP(16, 18),
                                      HIGHLIGHTED_ICON_ID);

   normalHighlightSpec->setTextProperties(
      SP(1,36), SP(34,41),
      topLeftAlignment,
      OverlayItemVisualSpec::RGB(0, 0, 0),
      nameFont);

   ImageSpec* stackedHighlightImg =
      S60OverlayHelpers::loadMif(extPath,
                                 "overlay-item-bg-highlighted-stacked.mif");

   OverlayItemVisualSpec* stackedHighlightSpec =
      OverlayItemVisualSpec::allocate(stackedHighlightImg,
                                      SP(20, 47),
                                      SP(17, 25),
                                      HIGHLIGHTED_ICON_ID);
   
   stackedHighlightSpec->setTextProperties(
      SP(1,41), SP(34,46),
      SP(32, 2), SP(40, 11),
      topLeftAlignment,
      centerAlignment,
      OverlayItemVisualSpec::RGB(0, 0, 0),
      OverlayItemVisualSpec::RGB(0, 0, 0),
      nameFont,
      nbrFont);
   
   zoomSpec->setHighlightedSpecs(normalHighlightSpec,
                                 stackedHighlightSpec);
   
   normalHighlightImg->removeReference();
   normalHighlightSpec->removeReference();

   stackedHighlightImg->removeReference();
   stackedHighlightSpec->removeReference();
   
   nameFont->removeReference();
   nbrFont->removeReference();
   
   return zoomSpec;
}


std::pair<WFAPI::StackedDialogVisualSpec*, WFAPI::StackedDialogVisualSpec*>
S60OverlaySetup::createDialogSpec(const char* pathToResources)
{ 
   std::string extPath(pathToResources);
   
   extPath += "overlay";
   
   // Create the visual spec and images for 2 stacked items
   WFAPI::ImageSpec* frame =
      S60OverlayHelpers::loadMif(extPath, "dialog-frame.mif");

   WFAPI::OverlayItemVisualSpec* visualSpec =
      WFAPI::OverlayItemVisualSpec::allocate(frame,
                                             WFAPI::ScreenPoint(20, 33),
                                             WFAPI::ScreenPoint(20, 13),
                                             0);

   FontSpec* nameFont = FontSpec::allocate("LatinPlain10", 10);
   
   visualSpec->setTextProperties(
      WFAPI::ScreenPoint(6, 23),
      WFAPI::ScreenPoint(28, 35),
      WFAPI::OverlayItemVisualSpec::TextAlignment(),
      WFAPI::OverlayItemVisualSpec::RGB(0, 0, 0),
      nameFont);

   nameFont->removeReference();
   
   WFAPI::ImageSpec* bg2 =
      S60OverlayHelpers::loadMif(extPath, "dialog-background-2.mif");
      
   // Specific to the dialog image
   WFAPI::ScreenPoint leftArrowPos2(0, 0);
   WFAPI::ScreenPoint rightArrowPos2(0, 0);
   
   WFAPI::ScreenPoint upperLeft2(10, 2);
   WFAPI::ScreenPoint lowerRight2(90, 41);
   
   WFAPI::ScreenPoint dialogFocusPoint2(48, 56);

   WFAPI::StackedDialogVisualSpec* stackedDialogSpec2 =
      WFAPI::StackedDialogVisualSpec::allocate(bg2,
                                               dialogFocusPoint2,
                                               upperLeft2,
                                               lowerRight2,
                                               NULL,
                                               leftArrowPos2,
                                               NULL,
                                               rightArrowPos2,
                                               visualSpec);
   
   WFAPI::ImageSpec* bg3 =
      S60OverlayHelpers::loadMif(extPath, "dialog-background-3.mif");
   WFAPI::ImageSpec* left3 =
      S60OverlayHelpers::loadMif(extPath, "dialog-arrow-left.mif");
   WFAPI::ImageSpec* right3 =
      S60OverlayHelpers::loadMif(extPath, "dialog-arrow-right.mif");
      
   // Specific to the dialog image
   WFAPI::ScreenPoint leftArrowPos3(-20, 21);
   WFAPI::ScreenPoint rightArrowPos3(167, 21);
   
   WFAPI::ScreenPoint upperLeft3(16, 2);
   WFAPI::ScreenPoint lowerRight3(136, 41);   

   WFAPI::ScreenPoint dialogFocusPoint3(71, 56);

   WFAPI::StackedDialogVisualSpec* stackedDialogSpec3 =
      WFAPI::StackedDialogVisualSpec::allocate(bg3,
                                               dialogFocusPoint3,
                                               upperLeft3,
                                               lowerRight3,
                                               left3,
                                               leftArrowPos3,
                                               right3,
                                               rightArrowPos3,
                                               visualSpec);
   
   bg2->removeReference();
   bg3->removeReference();
   left3->removeReference();
   right3->removeReference();
   frame->removeReference();
   visualSpec->removeReference();

   return std::make_pair(stackedDialogSpec2, stackedDialogSpec3);
}

} // End of namespace S60OverlaySetup
