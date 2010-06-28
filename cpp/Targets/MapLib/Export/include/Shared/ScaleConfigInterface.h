/*
  Copyright (c) 1999 - 2010, Vodafone Group Services Ltd
  All rights reserved.

  Redistribution and use in source and binary forms, with or without modification, are permitted provided that the following conditions are met:

  * Redistributions of source code must retain the above copyright notice, this list of conditions and the following disclaimer.
  * Redistributions in binary form must reproduce the above copyright notice, this list of conditions and the following disclaimer in the documentation and/or other materials provided with the distribution.
  * Neither the name of the Vodafone Group Services Ltd nor the names of its contributors may be used to endorse or promote products derived from this software without specific prior written permission.

  THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS" AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE ARE DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT HOLDER OR CONTRIBUTORS BE LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
*/

#ifndef WFAPI_SCALE_CONFIG_INTERFACE_H
#define WFAPI_SCALE_CONFIG_INTERFACE_H

#include "PALMachineTypes.h"
#include "DistanceUnit.h"

class MapLib;

namespace WFAPI {

/**
 * Interface used to configure the scale widget, i.e. the bar that
 * shows the current scale of the map in distance units
 * (ex. kilometers or miles).
 */
class ScaleConfigInterface {
public:
   /**
    * Constructor.
    *
    * @param mapLib The instance of mapLib to configure.
    */
   ScaleConfigInterface(MapLib* mapLib);

   /**
    * Destructor.
    */
   ~ScaleConfigInterface();

   /**
    * To enable the scale in the map.
    *
    * @param enable If the scale should be enabled.
    */
   void enableScale(bool enable);
   
   /**
    * Set distance unit used in the scale
    * 
    * @param unit The unit to use for showing distance in the scale.
    */
   void setDistanceUnit(DistanceUnit unit);

   /**
    * Alignment to use for the scale.
    */
   enum ScaleAlignment {
      /// Left alignment of the scale. Scale grows from left to right.
      LEFTALIGNMENT,

      /// Right alignment of the scale. Scale grows from right to left.
      RIGHTALIGNMENT
   };
   
   /**
    * Set the alignment of the scale.
    *
    * @param scaleAlignment The alignment of the scale.
    */
   void setAlignment(ScaleAlignment scaleAlignment);

   /**
    * Set the maximum width of the scale.
    *
    * @param maxScaleWidth The maximum width of the scale. 
    */
   void setMaxWidth(wf_uint16 maxScaleWidth);
   
   /**
    * Set the height of the scale.
    *
    * @param scaleHeight The height of the scale. 
    */
   void setHeight(wf_uint16 scaleHeight);
   
   /**
    * Set the position of the scale.
    *
    * @param posX The position in X-led.
    * @param posY The position in Y-led.
    */
   void setPosition(wf_uint16 posX, wf_uint16 posY);

   /**
    * Get the maximum width of the scale.
    *
    * @return The maximum scale width. 
    */
   wf_uint16 getMaxWidth() const;
   
   /**
    * Get the height of the scale.
    *
    * @return The height of the scale. 
    */
   wf_uint16 getHeight() const;
   
   /**
    * Get the position of the scale in X-led.
    *
    * @return The position of the scale in X-led. 
    */
   wf_int32 getPositionXLed() const;

   /**
    * Get the position of the scale in Y-led.
    *
    * @return The position of the scale in Y-led. 
    */
   wf_int32 getPositionYLed() const;

private:
   /**
    * We do not allow copies of this interface.
    */
   ScaleConfigInterface(const ScaleConfigInterface&);
   const ScaleConfigInterface& operator=(const ScaleConfigInterface&);

   MapLib* m_mapLib;   
};

} // End namespace WFAPI.

#endif // End WFAPI_SCALE_CONFIG_INTERFACE_H
