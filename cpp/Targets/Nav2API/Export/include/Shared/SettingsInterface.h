/*
  Copyright (c) 1999 - 2010, Vodafone Group Services Ltd
  All rights reserved.

  Redistribution and use in source and binary forms, with or without modification, are permitted provided that the following conditions are met:

  * Redistributions of source code must retain the above copyright notice, this list of conditions and the following disclaimer.
  * Redistributions in binary form must reproduce the above copyright notice, this list of conditions and the following disclaimer in the documentation and/or other materials provided with the distribution.
  * Neither the name of the Vodafone Group Services Ltd nor the names of its contributors may be used to endorse or promote products derived from this software without specific prior written permission.

  THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS" AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE ARE DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT HOLDER OR CONTRIBUTORS BE LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
*/

#ifndef WFAPI_SETTINGS_INTERFACE_H
#define WFAPI_SETTINGS_INTERFACE_H

#include "WFAPIConfig.h"
#include "Interface.h"
#include "DistanceUnit.h"
#include "VoiceVerbosity.h"
#include "RouteCost.h"
#include "TransportationType.h"
#include "SynchronousStatus.h"
#include "AsynchronousStatus.h"
#include "POICategories.h"

namespace WFAPI {

// Forward declarations
class StackedDialogVisualSpec;
class ScreenPoint;
class Nav2APIImpl;
class SettingsInterfaceImpl;
class SettingsListener;

/**
 * Interface for settings, used to get and set settings.
 */
class SettingsInterface : public Interface
{

public:

   /**
    * Destructor.
    */
   virtual ~SettingsInterface();

   /**
    * Add a listener for setting events.
    * @see SettingsListener
    *
    * @param listener The SettingsListener to add. 
    *                 It may not be deleted until it is removed by
    *                 calling removeSettingsListener.
    */
   void addSettingsListener(SettingsListener* listener);

   /**
    * Remove a settings listener.
    *
    * @param listener The SettingsListener to remove.
    *                 It should now be deleted by caller, unless caller
    *                 keeps it around for later use.
    */
   void removeSettingsListener(SettingsListener* listener);

   /**
    * Get if it is set to automatic calculate a new route if off track.
    *
    * @param autoReroute Set to the value of autoReroute.
    * @return StatusCodes::OK if operation succeeded.
    */
   SynchronousStatus getAutoReroute(bool& autoReroute);

   /**
    * Get if it is set to avoid highways.
    *
    * @param avoidHighway Set to the value of AvoidHighway.
    * @return StatusCodes::OK if operation succeeded.
    */
   SynchronousStatus getAvoidHighway(bool& avoidHighway);

   /**
    * Get if it is set to avoid toll roads.
    *
    * @param avoidTollRoad Set to the value of AvoidTollRoad.
    * @return StatusCodes::OK if operation succeeded.
    */
   SynchronousStatus getAvoidTollRoad(bool& avoidTollRoad);

   /**
    * Get if it is set to check for traffic changes.
    *
    * @param checkRouteForTrafficChanges Set to the value of 
    *        CheckRouteForTrafficChanges.
    * @return StatusCodes::OK if operation succeeded.
    */
   SynchronousStatus getCheckRouteForTrafficChanges(
      bool& checkRouteForTrafficChanges);

   /**
    * Get units used for measuring distances.
    * 
    * @param distanceUnit Set to the value of DistanceUnit.
    * @return StatusCodes::OK if operation succeeded.
    */
   SynchronousStatus getDistanceUnit(DistanceUnit& distanceUnit);

   /**
    * Get how route costs are measured.
    *
    * @param routeCost Set to the value of RouteCost.
    * @return StatusCodes::OK if operation succeeded.
    */
   SynchronousStatus getRouteCost(RouteCost& routeCost);

   /**
    * Get how often it is set to check for traffic information updates.
    *
    * @param trafficInformationUpdateTime Set to the value of 
    *        TrafficInformationUpdateTime, in minutes.
    * @return StatusCodes::OK if operation succeeded.
    */
   SynchronousStatus getTrafficInformationUpdateTime(
      wf_uint32& trafficInformationUpdateTime);

   /**
    * Get which verbosity level is set for voices.
    *
    * @param voiceVerbosity Set to the value of VoiceVerbosity.
    * @return StatusCodes::OK if operation succeeded.
    */
   SynchronousStatus getVoiceVerbosity(VoiceVerbosity& voiceVerbosity);

   /**
    * Get the TransportationType set in the last routeTo call in 
    * RouteInterface.
    *
    * @param transportationType Set to the value of TransportationType.
    * @return StatusCodes::OK if operation succeeded.
    */
   SynchronousStatus getCurrentTransportationType(
      TransportationType& transportationType);

   /**
    * To set if to automatic calculate a new route if off track.
    *
    * @param newVal If to automatic calculate a new route if off track.
    * @return StatusCodes::OK if operation succeeded.
    */
   SynchronousStatus setAutoReroute(const bool& newVal);

   /**
    * To set if to avoid highways.
    *
    * @param newVal If to avoid highways.
    * @return StatusCodes::OK if operation succeeded.
    */
   SynchronousStatus setAvoidHighway(const bool& newVal);

   /**
    * To set if to avoid toll roads.
    *
    * @param newVal If to avoid toll roads.
    * @return StatusCodes::OK if operation succeeded.
    */
   SynchronousStatus setAvoidTollRoad(const bool& newVal);

   /**
    * To set if to check route for traffic changes.
    *
    * @param newVal If to check route for traffic changes.
    * @return StatusCodes::OK if operation succeeded.
    */
   SynchronousStatus setCheckRouteForTrafficChanges(const bool& newVal);

   /**
    * To set which unit to use for distances.
    *
    * @param newVal The unit to use for distances.
    * @return StatusCodes::OK if operation succeeded.
    */
   SynchronousStatus setDistanceUnit(DistanceUnit newVal);

   /**
    * Set how to measure route costs.
    *
    * @param newVal The route cost to be used.
    * @return StatusCodes::OK if operation succeeded.
    */
   SynchronousStatus setRouteCost(RouteCost newVal);

   /**
    * To set how often to check for traffic information updates.
    *
    * @param newVal Time in minutes between traffic information updates.
    * @return StatusCodes::OK if operation succeeded.
    */
   SynchronousStatus setTrafficInformationUpdateTime(wf_uint32 newVal);

   /**
    * To set verbosity level for voices.
    *
    * @param newVal The verbosity level to use for voices.
    * @return StatusCodes::OK if operation succeeded.
    */
   SynchronousStatus setVoiceVerbosity(VoiceVerbosity newVal);

   /**
    * Set this parameter to true to get the position based search
    * categories list.
    *
    * @param newVal If set to true the postion will be used when requesting
    *               the search categories list.
    * @return StatusCodes::OK if operation succeeded.
    */
   SynchronousStatus setUsePositionForSearchCategoriesRequests(bool newVal);

   /**
    * Get the parameter that enables the position based search categories list.
    *
    * @param usePos If set to true the postion will be used when requesting
    *               the search categories list.
    * @return StatusCodes::OK if operation succeeded.
    */
   SynchronousStatus getUsePositionForSearchCategoriesRequests(bool &usePos);

   /**
    * Settings for MapLib, set in the connected MapLibAPI and
    * stored in Nav2's settings storage between application restarts.
    */
   //@{
   /**
    * Sets the max size of the disk cache if it exists.
    *
    * @param nbrBytes The maximum size of the disk cache in bytes.
    */
   void setMaxDiskCacheSize(wf_uint32 nbrBytes);

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
   
   //@}

   /**
    * Asynchronous function to check if EULA is updated. The result of the
    * request comes in a callback to the listener.
    *
    * @param filename The filename of the EULA, this is the name to the file
    *                 where the EULA is stored.
    * @return The status of the request.
    */
   AsynchronousStatus isEULAUpdated(const WFString& filename);

   /**
    * Synchronous function for retrieving the user's username
    *
    * @param username The username to be retrieved.
    * @return The status of the request.
    */
   SynchronousStatus getUsername(WFString& username);

   /**
    * Synchronous function for setting the user's web password
    *
    * @param password The password to change to.
    * @return The status of the request.
    */
   SynchronousStatus setPassword(const WFString& password);

   /**
    * Synchronous function for setting the UIN of this user.
    *
    * @param UIN Where the UIN will be stored.
    * @return The status of the request.
    */ 
   SynchronousStatus setUIN(const WFString& UIN);
   
   /** 
    * Call to get new version information if there is a new version available.
    * The strings are empty if there are no new version available.
    * 
    * @param versionNumber The new version number. Comes as a string
    * containing 3 different versions:
    * <Program version> : <Resource version> : <MLFW version>
    * where the "Resource version" and "MLFW version" are optional.
    * The program and resource versions are specified as:
    * [1-9][0-9]*'.'[1-9][0-9]*'.'[1-9][0-9]*
    * and the MLFW version is specified as a single number.
    * @param versionUrl The url where the new version is available for
    * download.
    * @param versionString A freetext string for the new version. Can be
    * an URI, a platform market link, a package name, an id, etc.
    * @param force A flag saying if the update should be forced,
    * e.g. the user should not be able to use the application until he
    * has accepted the upgrade.
    */
   SynchronousStatus getNewVersionInformation(WFString& version,
                                              WFString& url,
                                              WFString& versionString,
                                              bool& force);
   
   /**
    * Internal function.
    */
   SettingsInterfaceImpl* getImpl();

private:
   /**
    * Do not create SettingsInterface get it from the Nav2API class.
    */
   SettingsInterface(Nav2APIImpl* nav2APIimpl);

   /**
    * We do not allow copies of this interface.
    */
   SettingsInterface(const SettingsInterface&);
   const SettingsInterface& operator=(const SettingsInterface&);

   struct impl;

   impl* m_impl;

   friend class Nav2APIImpl;
};

} // End namespace WFAPI.

#endif // End WFAPI_SETTINGS_INTERFACE_H.
