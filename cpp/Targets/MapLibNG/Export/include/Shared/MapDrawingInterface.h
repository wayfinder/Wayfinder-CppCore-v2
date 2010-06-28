/*
  Copyright (c) 1999 - 2010, Vodafone Group Services Ltd
  All rights reserved.

  Redistribution and use in source and binary forms, with or without modification, are permitted provided that the following conditions are met:

  * Redistributions of source code must retain the above copyright notice, this list of conditions and the following disclaimer.
  * Redistributions in binary form must reproduce the above copyright notice, this list of conditions and the following disclaimer in the documentation and/or other materials provided with the distribution.
  * Neither the name of the Vodafone Group Services Ltd nor the names of its contributors may be used to endorse or promote products derived from this software without specific prior written permission.

  THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS" AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE ARE DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT HOLDER OR CONTRIBUTORS BE LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
*/

#ifndef WFAPI_MAPDRAWINGINTERFACE_H
#define WFAPI_MAPDRAWINGINTERFACE_H

#include "MapDrawingCallback.h"

namespace WFAPI {

class ImageSpec;
class ScreenPoint;

/**
 * Class which controls redrawing of the map.
 */
class MapDrawingInterface {
public:
   
   /**
    * Virtual destructor.
    */
   virtual ~MapDrawingInterface() {}
   
   /**
    * Requests that the display should be repainted now
    * as a blocking call.
    */
   virtual void repaintNow() = 0;

   /**
    * Requests that the display should be repainted when
    * convenient.
    */
   virtual void requestRepaint() = 0;

   /**
    * Request that the map should not be redrawn ever, even if it thinks it
    * needs to redraw itself.
    * Typically used if the maps is in a background view and one want to make
    * sure it's not redrawn, that is make sure it does not use ui thread cpu
    * cycles.
    * The default state for this is enabled, until the user changes it.
    *
    * @param mapDrawingEnabled set to true if map should be drawn, false to disable drawing.
    */
   virtual void setMapDrawingEnabled(bool mapDrawingEnabled) = 0;

   /**
    * Sets the callback that will be invoked when the map is
    * ready to be drawn to a platform specific surface.
    */
   virtual void setMapDrawingCallback(MapDrawingCallback* callback) = 0;

   /**
    * Draws an image specification at the specified screen point.
    */ 
   virtual void drawImageSpec(const WFAPI::ImageSpec* spec,
                              const WFAPI::ScreenPoint& point) = 0;
};

} // End of namespace WFAPI

#endif // WFAPI_MAPDRAWINGINTERFACE_H
