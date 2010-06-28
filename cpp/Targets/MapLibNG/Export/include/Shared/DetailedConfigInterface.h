/*
  Copyright (c) 1999 - 2010, Vodafone Group Services Ltd
  All rights reserved.

  Redistribution and use in source and binary forms, with or without modification, are permitted provided that the following conditions are met:

  * Redistributions of source code must retain the above copyright notice, this list of conditions and the following disclaimer.
  * Redistributions in binary form must reproduce the above copyright notice, this list of conditions and the following disclaimer in the documentation and/or other materials provided with the distribution.
  * Neither the name of the Vodafone Group Services Ltd nor the names of its contributors may be used to endorse or promote products derived from this software without specific prior written permission.

  THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS" AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE ARE DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT HOLDER OR CONTRIBUTORS BE LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
*/

#ifndef WFAPI_DETAILEDCONFIGINTERFACE_H
#define WFAPI_DETAILEDCONFIGINTERFACE_H

#include "PALMachineTypes.h"
#include "POICategories.h"
#include "TextLanguage.h"
#include "DistanceUnit.h"

class MapLib;

namespace WFAPI {

class ScreenPoint;
class StackedDialogVisualSpec;

/**
 * Class used to configure runtime MapLib properties that
 * are less commonly used.
 */
class DetailedConfigInterface {
public:

   /**
    * Constructor.
    *
    * @param mapLib The instance of mapLib to configure.
    */
   DetailedConfigInterface(MapLib* mapLib);
   
   /**
    * Sets the stacked dialog visual specifications to be used
    * by MapLib. This will determine the layout of the dialogs
    * containing stacked OverlayItems.
    * 
    * @param twoItemsDialogSpec The specification to use when the dialog
    *                           contains only two items.
    * @param threeItemsDialogSpec The specification to use when the dialog
    *                             contains three or more items.
    */ 
   void setStackedDialogVisualSpecs(
      const StackedDialogVisualSpec* twoItemsDialogSpec,
      const StackedDialogVisualSpec* threeItemsDialogSpec);
   
   /**
    * Set the language to be used for the maps.
    * 
    * @param language The language to be used.
    */
   void setLanguage(TextLanguage language);
   
   /**
    * Sets the max size of the disk cache if it exists.
    *
    * @param nbrBytes The maximum size of the disk cache in bytes.
    */
   void setMaxDiskCacheSize(wf_uint32 nbrBytes);

   /**
    * Clears the disk cache.
    */
   void clearDiskCache();

   /**
    * Set if to show copyright string.
    * Some versions of the library binary file allows you to change this setting,
    * others don't. 
    * 
    * @return Returns true if you are allowed to change the default setting, otherwise false
    * 
    */
   bool showCopyright(bool show);
   
   /**
    * Set the position of the copyright string of the map.
    *
    * @param pos The left baseline (lower left) position of the 
    *            copyright string.
    */
   void setCopyrightPos(const ScreenPoint& pos);

   /**
    * Set if automatic highlight should be enabled or not.
    * Automatic highlight means that features in the center of the
    * map area will automatically be highlighted.
    *
    * @param enable True if automatic highlight should be enabled,
    *               false otherwise if disabled.
    */
   void enableAutomaticHighlight(bool enable);

   /**
    * Get a copy of the MapLib POI categories.
    *
    * @return A copy of the current POI categories.
    */
   POICategories getPOICategoriesSnapshot();

   /**
    * Set the visibility of a POI category with the specified id.
    * 
    * @param id The id of the category to modify visibility of.
    * @param visible If true, the category will be visible.
    * @return True if the category id was found.
    */
   bool setCategoryVisible(wf_uint32 id, bool visible);
 
   /**
    * Synchronize a local set of POI categories with visibility information
    * with the currently available categories in MapLib. This may lead to
    * that some categories are removed and some are added.
    * If a category is not removed, its visibility state remains unchanged.
    * The visibility state of added categories will be set by MapLib.
    *
    * @param localCategories The locally stored POI categories to synchronize
    *                        with MapLib's current category set. This is also
    *                        an out parameter which will be updated
    *                        accordingly.
    * @return True if any POI categories were added or removed to 
    *         the supplied localCategories.
    */
   bool synchronizePOICategories(POICategories& localCategories);

   /**
    * Hides the server supplied POIs from the map. 
    *
    * @param hidden If true, the POIs will be hidden.
    */
   void hidePOIs(bool hidden);
   
   /**
    * Set if traffic information should be shown on the map.
    *
    * @param enabled True if traffic info should be shown, false otherwise.
    */
   void showTrafficInfo(bool enabled);

   /**
    * Set the update interval for the traffic information in minutes.
    *
    * @param updateIntervalMinutes The update interval in minutes.
    */
   void setTrafficInfoUpdateInterval(wf_uint32 updateIntervalMinutes);

private:

   /**
    * We do not allow copies of this interface.
    */
   DetailedConfigInterface(const DetailedConfigInterface&);
   const DetailedConfigInterface& operator=(const DetailedConfigInterface&);

   MapLib* m_mapLib;   
};

} // End of namespace WFAPI

#endif // WFAPI_DETAILEDCONFIGINTERFACE_H
