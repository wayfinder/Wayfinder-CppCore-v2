/*
  Copyright (c) 1999 - 2010, Vodafone Group Services Ltd
  All rights reserved.

  Redistribution and use in source and binary forms, with or without modification, are permitted provided that the following conditions are met:

  * Redistributions of source code must retain the above copyright notice, this list of conditions and the following disclaimer.
  * Redistributions in binary form must reproduce the above copyright notice, this list of conditions and the following disclaimer in the documentation and/or other materials provided with the distribution.
  * Neither the name of the Vodafone Group Services Ltd nor the names of its contributors may be used to endorse or promote products derived from this software without specific prior written permission.

  THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS" AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE ARE DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT HOLDER OR CONTRIBUTORS BE LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
*/

#ifndef WFAPI_CONFIGINTERFACE_H
#define WFAPI_CONFIGINTERFACE_H

#include "PALMachineTypes.h"

class MapLib;

namespace WFAPI {

// Forward definitions
class DetailedConfigInterface;
class MapLibSettingsListener;
class ScaleConfigInterface;

/**
 * Class used to configure commonly used MapLib properties run time.
 */
class ConfigInterface {
public:
   /**
    * Constructor.
    * 
    * @param mapLib the instance of mapLib to configure.
    */ 
   ConfigInterface(MapLib* mapLib);

   /**
    * Destructor.
    */ 
   virtual ~ConfigInterface();
   
   /**
    * @deprecated, @see MapOperationInterface::set3dMode
    */
   void set3dMode(bool enabled);

   /**
    * @deprecated, @see MapOperationInterface::get3dMode
    */
   bool get3dMode() const;

   /**
    * @deprecated, @see MapOperationInterface::setVariable3dMode
    */
   void setVariable3dMode(float step);

   /**
    * Set night mode on or off.
    * 
    * @param on True if enable night mode, false otherwise (day mode).
    */ 
   void setNightMode(bool on);

   /**
    * Get if night mode is enabled.
    *
    * @return True if night mode is enabled, false otherwise (day mode).
    */
   bool getNightMode() const;

   /**
    * Set scale adjustment level for the map.
    * Guideline:
    * High resolution screen should use the value of two or larger.
    * This is to to prevent high resolution screens from showing to
    * much information over a bigger area.
    * Low resolution screens should use the value of one.
    *
    * NOTE: The parameter is a float value but is currently converted to
    * an integer, in the future this will possibly be changed to allow
    * more precise adjustments.
    * 
    * @param level The level of the map.
    */
   void setMapScaleAdjustment(float level);

   /**
    * Get the detailed config interface to MapLib, which exposes more
    * specific and detailed settings.
    *
    * @return The detailed config interface.
    */
   DetailedConfigInterface* getDetailedConfigInterface() const;

   /**
    * Get the scale config interface, which contains settings for the scale.
    *
    * @return The scale config interface.
    */
   ScaleConfigInterface* getScaleConfigInterface() const;

   /**
    * Add a settings listener which will get notifications when updated
    * settings are available.
    *
    * @param listener The listener to add.
    */
   void addSettingsListener(MapLibSettingsListener* listener);

   /**
    * Remove the settings listener so that it will no longer receive
    * notifications when updated settings are available.
    *
    * @param listener The listener to remove.
    */
   void removeSettingsListener(MapLibSettingsListener* listener);
   
private:
   /**
    * We do not allow copies of this interface.
    */
   ConfigInterface(const ConfigInterface&);
   const ConfigInterface& operator=(const ConfigInterface&);
   
   /// The MapLib instance to configure.
   MapLib* m_mapLib;

   /// The detailed configuration interface.
   DetailedConfigInterface* m_detailedConfig;

   /// The scale config interface.
   ScaleConfigInterface* m_scaleConfig;
};

} // End of namespace WFAPI

#endif // WFAPI_CONFIGINTERFACE_H
