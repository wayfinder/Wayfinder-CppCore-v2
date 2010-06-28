/*
  Copyright (c) 1999 - 2010, Vodafone Group Services Ltd
  All rights reserved.

  Redistribution and use in source and binary forms, with or without modification, are permitted provided that the following conditions are met:

  * Redistributions of source code must retain the above copyright notice, this list of conditions and the following disclaimer.
  * Redistributions in binary form must reproduce the above copyright notice, this list of conditions and the following disclaimer in the documentation and/or other materials provided with the distribution.
  * Neither the name of the Vodafone Group Services Ltd nor the names of its contributors may be used to endorse or promote products derived from this software without specific prior written permission.

  THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS" AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE ARE DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT HOLDER OR CONTRIBUTORS BE LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
*/

#ifndef WFAPI_SETTINGS_INTERFACE_IMPL_H
#define WFAPI_SETTINGS_INTERFACE_IMPL_H

#include "WFAPIConfig.h"
#include "InterfaceImpl.h"
#include "SynchronousStatus.h"
#include "RequestID.h"
#include "DistanceUnit.h"
#include "VoiceVerbosity.h"
#include "RouteCost.h"
#include "TransportationType.h"
#include "TextLanguage.h"
#include "POICategories.h"

namespace isab {
// Forward declarations
class GuiProtMess;
class GeneralParameterMess;
class DataGuiMess;
class NewVersionMess;
}

namespace WFAPI {

// Forward declarations
class SettingsListener;

/**
 * The behind the scenes class for Settings Interface.
 */
class SettingsInterfaceImpl : public InterfaceImpl {
public:

   /**
    * Constructor.
    */ 
   SettingsInterfaceImpl(Nav2APIImpl* api);

   /**
    * Destructor.
    */
   virtual ~SettingsInterfaceImpl();

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
    * Implementation of abstract function from InterfaceImpl.
    */
   //@{
   /**
    * Function called when sending reply from Nav2. This is called when
    * receiving replies from asynchronous requests.
    *
    * @param guiProtMess The returning data of the reply.
    * @param requestID The RequestID for the guiProtMess.
    */
   void receiveAsynchronousReply(const isab::GuiProtMess& guiProtMess,
                                 RequestID requestID);
   //@}

   /**
    * Functions wrapped from SettingsInterface.
    */
   //@{
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
   //@}

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
    * Convert a nav2 distance unit to WFAPI DistanceUnit.
    *
    * @param dist The input distance unit.
    * @return The output distance unit.
    */
   static DistanceUnit nav2DistanceUnitToWFAPI(wf_int32 dist);

   /**
    * Convert a WFAPI DistanceUnit to nav2 distance unit.
    *
    * @param dist The input distance unit.
    * @return The output distance unit.
    */
   static wf_int32 WFAPIDistanceUnitToNav2(DistanceUnit dist);

   /**
    * Convert a nav2 route cost to WFAPI route cost.
    *
    * @param dist The input route cost.
    * @return The output route cost.
    */
   RouteCost nav2RouteCostToWFAPI(wf_int32 cost);

   /**
    * Convert a WFAPI route cost to nav2 route cost.
    *
    * @param dist The input distance unit.
    * @return The output distance unit.
    */
   wf_int32 WFAPIRouteCostToNav2(RouteCost cost);

   /**
    * Convert a WFAPI voice verbosity to nav2 voice verbosity.
    *
    * @param dist The input voice verbosity.
    * @return The output voice verbosity.
    */
   VoiceVerbosity nav2VoiceVerbosityToWFAPI(wf_int32 verb);

   /**
    * Convert a nav2 voice verbosity to WFAPI voice verbosity.
    *
    * @param dist The input voice verbosity.
    * @return The output voice verbosity.
    */
   wf_int32 WFAPIVoiceVerbosityTonav2(VoiceVerbosity verb);

   /**
    * Splits the Nav2 userTrafficUpdatePeriod into it's components.
    *
    * @param nav2Val The value from Nav2.
    * @param checkRouteForTrafficChanges Output value.
    * @param trafficInformationUpdateTime Output value.
    */
   static void nav2UpdateTimeToValues( wf_int32 nav2Val, 
                                       bool& checkRouteForTrafficChanges,
                                       wf_int32& trafficInformationUpdateTime);

   /**
    * Turns a paramPoiCategories parameter into a POICategories.
    *
    * @param mess The GeneralParameterMess with the paramPoiCategories 
    *             parameter data.
    * @return The POICategories.
    */
   static POICategories makePOICategoriesFromNav2(
      isab::GeneralParameterMess* mess);

   /** 
    * Parses the message from Nav2 that contains information about
    * a new version.
    * 
    * @param mess Messsage containing all information required for
    *             the application to udate the version.
    */
   void handleNewVersionMsg(const isab::NewVersionMess& mess);

   /** 
    * Get the new version if it has come in the server info already.
    * @see SettingsInterface.h
    * 
    * @param version The new version.
    * @param url The url for the new version.
    * @param versionString A string describing the new version.
    * @param force Force the client to upgrade or not.
    */
   SynchronousStatus getNewVersionInformation(WFString& version,
                                              WFString& url,
                                              WFString& versionString,
                                              bool& force);

protected:

private:

   /**
    * Data container that holds information about the new version
    * that was retreived through the handleNewVersionMsg callback.
    */
   struct VersionInfo {
      VersionInfo() : force(false) { }

      /// The version number
      WFString version;

      /// The url from which the app can download the new version
      WFString url;

      /// Platform specific information, e.g market id for iPhone
      WFString versionString;

      /// Tells if this should be a force upgrade or not.
      bool force;
   };
   
   /**
    * Get a parameter from an ID.
    *
    * @param paramID The ID of the Nav2 parameter to get.
    * @param value Set to the wf_int32 value of the parameter if status is OK.
    * @return StatusCodes::OK if operation succeeded.
    */
   SynchronousStatus getParam(wf_uint16 paramID, wf_int32& value);

   /**
    * Get a parameter from an ID.
    *
    * @param paramID The ID of the Nav2 parameter to get.
    * @param value Set to the bool value of the parameter if status is OK.
    * @return StatusCodes::OK if operation succeeded.
    */
   SynchronousStatus getParam(wf_uint16 paramID, bool& value);


   /**
    * Set a parameter from an ID.
    *
    * @param paramID The ID of the Nav2 parameter to set.
    * @param value The wf_int32 value to set parameter to.
    * @return StatusCodes::OK if operation succeeded.
    */
   SynchronousStatus setParam(wf_uint16 paramID, wf_int32 value);

   /**
    * Set the poi categories stored in Nav2.
    *
    * @param  categories The new value of POI categories.
    * @return StatusCodes::OK if operation succeeded.
    */
   SynchronousStatus setPOICategoriesinNav2(POICategories& categories);

   /**
    * Function that handles the eula reply. Saves the eula to a file if a CRC
    * check is OK.
    *
    * @param mess The message containing the eula reply.
    */
   void handleEULAReply(const isab::DataGuiMess& mess);

   /// the filename of the file containing the CRC data for the EULA.
   WFString m_crcData;

   /// The filename of the file containing the eula data.
   WFString m_eulaDataFileName;

   /// Typedef for a set of listeners.
   typedef std::set<SettingsListener*> ListenerCont;

   /// Set of listeners.
   ListenerCont m_listeners;

   /**
    * Holds the information about a new version if this information
    * was sent by the server.
    */
   VersionInfo m_versionInfo;
};

} // End namespace WFAPI

#endif // End WFAPI_SETTINGS_INTERFACE_IMPL_H

