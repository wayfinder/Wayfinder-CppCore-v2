/*
Copyright (c) 1999 - 2010, Vodafone Group Services Ltd
All rights reserved.

Redistribution and use in source and binary forms, with or without modification, are permitted provided that the following conditions are met:

    * Redistributions of source code must retain the above copyright notice, this list of conditions and the following disclaimer.
    * Redistributions in binary form must reproduce the above copyright notice, this list of conditions and the following disclaimer in the documentation and/or other materials provided with the distribution.
    * Neither the name of the Vodafone Group Services Ltd nor the names of its contributors may be used to endorse or promote products derived from this software without specific prior written permission.

THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS" AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE ARE DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT HOLDER OR CONTRIBUTORS BE LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
*/


#ifndef _OVERLAYSETUPHELPERS_H_
#define _OVERLAYSETUPHELPERS_H_

#include "Shared/ScreenPoint.h"
#include "Shared/OverlayItemVisualSpec.h"

namespace WFAPI {

class OverlayItemZoomSpec;
class OverlayItemVisualSpec;
class FontSpec;
   
/**
 * This namespace has functions for setting up the
 * visual specifications for an application-specific
 * overlay.
 */
namespace OverlaySetupHelpers {
      
/**
 * Helper struct for storing data about background
 * image properties. Matches the parameters for
 * creating a OverlayVisualSpec.
 */ 
struct BackgroundProperty {
         
   /// The path to the background image file.
   const char* path;
   
   /// The focus point of the image. (@see OverlayItemVisualSpec)
   WFAPI::ScreenPoint focusPoint;
   
   /// The image point of the image. (@see OverlayItemVisualSpec)
   WFAPI::ScreenPoint imagePoint;
};

/**
 * Contains the background properties for the three different states.
 */ 
struct BackgroundProperties {
   
   /// The background property for normal mode.
   BackgroundProperty normal;
   
   /// The background property for stacked mode. 
   BackgroundProperty stacked;

   /// The background property for normal mode when tapped.
   BackgroundProperty normalTapped;
   
   /// The background property for stacked mode when tapped. 
   BackgroundProperty stackedTapped;
   
   /// The background property for highlighted mode.
   BackgroundProperty highlight;
};

/**
 * Defines a zoom range for which a certain style is valid.
 */ 
struct ZoomRange {
   /// The start of the zoom range
   WFAPI::wf_uint32 minZoom;
   /// The end of the zoom range.
   WFAPI::wf_uint32 maxZoom;
};

/**
 * Contains the text properties that define the text field inside
 * the overlay visual. Matches the textProperties parameters of a
 * OverlayItem visual spec.
 */ 
struct TextProperty {
   
   /// The upper left point of the name text rectangle.
   WFAPI::ScreenPoint nameUpperLeft;
   
   /// The lower right point of the name text rectangle.
   WFAPI::ScreenPoint nameLowerRight;
   
   /// The upper left point of the number text rectangle. 
   WFAPI::ScreenPoint nbrUpperLeft;
   
   /// The lower right point of the number text rectangle.
   WFAPI::ScreenPoint nbrLowerRight;
   
   /// The text alignment of the name text rectangle.
   WFAPI::OverlayItemVisualSpec::TextAlignment textAlignment;
   
   /// The text alignment of the nubmer text rectangle.
   WFAPI::OverlayItemVisualSpec::TextAlignment numberAlignment;   
   
   /// The color of the name text rectangle.
   WFAPI::OverlayItemVisualSpec::RGB nameColor;
   
   /// The color of the number text rectangle.
   WFAPI::OverlayItemVisualSpec::RGB nbrColor;
};
      
/**
 * Contains the text peroperties for the three different states.
 */
struct TextProperties {
   
   /// The text property for normal mode.
   TextProperty normal;
   
   /// The text property for stacked mode.
   TextProperty stacked;
   
   /// The text property for highlighted mode.
   TextProperty highlight;   
};

} // End of namespace OverlaySetupHelpers

} // End of namespace WFAPI


#endif /* _OVERLAYSETUPHELPERS_H_ */


