/*
  Copyright (c) 1999 - 2010, Vodafone Group Services Ltd
  All rights reserved.

  Redistribution and use in source and binary forms, with or without modification, are permitted provided that the following conditions are met:

  * Redistributions of source code must retain the above copyright notice, this list of conditions and the following disclaimer.
  * Redistributions in binary form must reproduce the above copyright notice, this list of conditions and the following disclaimer in the documentation and/or other materials provided with the distribution.
  * Neither the name of the Vodafone Group Services Ltd nor the names of its contributors may be used to endorse or promote products derived from this software without specific prior written permission.

  THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS" AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE ARE DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT HOLDER OR CONTRIBUTORS BE LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
*/

#ifndef WFAPI_NAV2_API_IMPL_H
#define WFAPI_NAV2_API_IMPL_H

#include "WFAPIConfig.h"
#include "WFString.h"
#include "RequestID.h"
#include "StatusCode.h"
#include "HardwareIDArray.h"
#include "VoiceLanguage.h"
#include "WGS84Coordinate.h"
#include <deque>
#include <map>
#include <set>
#include "ReadyToRunListener.h"
#include "Nav2StatusNotifier.h"
#include "InternalSessionData.h"
#include "CSImageDimension.h"

namespace isab {
// Forward declarations
class Buffer;
class Nav2;
class GuiProtMess;
class Mutex;
class GeneralParameterMess;
}

class Nav2StatusNotifier;
class NotificationCenter;

namespace WFAPI {

// Forward declarations
class Nav2API;
class Interface;
class InterfaceImpl;
class QueueThread;
class FavouriteInterface;
class ImageInterface;
class LocationInterface;
class MapLibAPI;
class NavigationInterface;
class NetworkInterface;
class RouteInterface;
class SearchInterface;
class SettingsInterface;
class BillingInterface;
class TunnelInterface;
class StartupData;
class Nav2StatusListener;
class Nav2APIChannel;
class MapRequesterInterfaceImpl;
class ReadyToRunListener;
class MapLibReadyToRunListener;

struct ProgramVersion;


class Nav2APIImpl : public ReadyToRunReceiver {
public:

   /**
    * Constructor.
    */ 
   Nav2APIImpl(Nav2API* api);

   /**
    * Destructor.
    */
   virtual ~Nav2APIImpl();

   /**
    * Function for receiving requests from interfaces to be sent to Nav2
    * via the channel.
    *
    * @param guiProtMess The message to be sent to the channel.
    * @param interface A pointer to the interface that made the request.
    * @param synchronous If the request is synchronous.
    * @param requestID (opt) If to use a specific requestID.
    * @return The RequestID made for this request.
    */
   RequestID nav2Request(isab::GuiProtMess* guiProtMess,
                         InterfaceImpl* interface,
                         bool synchronous,
                         RequestID* requestID = NULL); 

   /**
    * Functions wrapped from Nav2API.
    */
   //@{

   /**
    * Call this to connect the MapLibAPI with this Nav2API server
    * communication and settings.
    * Only one MapLibAPI can be connected.
    *
    * @param mapLibAPI The MapLibAPI to connect.
    */
   void connectMapLib(MapLibAPI* mapLibAPI);

   /**
    * Get the FavouriteInterface.
    * @see FavouriteInterface to find out it's capabilities.
    *
    * @return The FavouriteInterface.
    */
   FavouriteInterface& getFavouriteInterface();

   /**
    * Get the ImageInterface.
    * @see ImageInterface to find out it's capabilities.
    *
    * @return The ImageInterface.
    */
   ImageInterface& getImageInterface();

   /**
    * Get the GPS and cell ID interface.
    * @see LocationInterface to find out it's capabilities.
    *
    * @return The LocationInterface.
    */
   LocationInterface& getLocationInterface();

   /**
    * Get the navigation interface, it relies on route interface for making
    * routes to navigate.
    * @see NavigationInterface to find out it's capabilities.
    *
    * @return The NavigationInterface.
    */
   NavigationInterface& getNavigationInterface();

   /**
    * Get the network link settings and connection interface.
    * @see NetworkInterface to find out it's capabilities.
    *
    * @return The NetworkInterface.
    */
   NetworkInterface& getNetworkInterface();

   /**
    * Get the RouteInterface.
    * @see RouteInterface to find out it's capabilities.
    *
    * @return The RouteInterface.
    */
   RouteInterface& getRouteInterface();

   /**
    * Get the SearchInterface.
    * @see SearchInterface to find out it's capabilities.
    *
    * @return The SearchInterface.
    */
   SearchInterface& getSearchInterface();

   /**
    * Get the SettingsInterface.
    * @see SettingsInterface to find out it's capabilities.
    *
    * @return The SettingsInterface.
    */
   SettingsInterface& getSettingsInterface();
    
   /**
    * Get the BillingInterface.
    * @see BillingInterface to find out it's capabilities.
    *
    * @return The BillingInterface.
    */
   BillingInterface& getBillingInterface();

   /**
    * Get the TunnelInterface.
    * @see TunnelInterface to find out it's capabilities.
    *
    * @return The TunnelInterface.
    */
   TunnelInterface& getTunnelInterface();
   
   /**
    * Get the MapRequesterInterfacImpl.
    * @see MapRequesterInterfacImpl to find out it's capabilities.
    *
    * @return The MapRequesterInterfacImpl.
    */
   MapRequesterInterfaceImpl& getMapRequesterInterfaceImpl();

   /**
    * Call function to begin starting the Nav2API. Nav2StatusListener is
    * called when startup has completed.
    *
    * @param statusListener The Nav2StatusListener to call when startup 
    *                       is done.
    * @param startupData The StartupData with the startup settings.
    */
   void start(StartupData* startupData,
              const WFString& audioTypeDirName,
              const ProgramVersion& programVersion, 
              const WFString& clientType, 
              const WFString& clientTypeOptions,
              bool httpProto,
              const WFString& httpUserAgent,
              const WFString& httpRequestString,
              const WFString& httpFixedHost,
              bool useTracking,
              bool wayfinderIDStartUp,
              const HardwareIDArray& hwids,
              ReadyToRunListener* readyListener,
              Nav2StatusNotifier* statusNotifier);

   void addStatusListener(Nav2StatusListener* statusListener);

   void removeStatusListener(Nav2StatusListener* statusListener);
   
   /**
    * Starts shut down procedure of Nav2API. When done the
    * Nav2StatusListener will get a call to stopComplete.
    */
   void stop();

   //@}

   /**
    * Turns a GuiProtMess error code into a StatusCode or a request
    * specific error code..
    */
   wf_uint32 nav2ErrorNumberToStatusCode(wf_uint32 errorNumber) const;

   /**
    * Get the Nav2API.
    *
    * @return The Nav2API.
    */
   Nav2API* getNav2API();
   
   /**
    * Get the connected MapLibAPI, might be NULL.
    *
    * @return The connected MapLibAPI, NULL if not connected.
    */
   MapLibAPI* getConnectedMapLib();

   /**
    * Called when synchronising callbacks should be made, using
    * the context of the caller.
    */
   virtual void doCallbacks();

   /**
    * Get the two character string for a language.
    *
    * @param lang The language to gget a string for.
    * @param uppercase If to return upper case or lower case.
    * @return The string for the language.
    */
   WFString getVoiceLanguageAsString(VoiceLanguage::VoiceLanguage lang, 
                                     bool uppercase);

   /**
    * Get the current SessionData. Can be used to get the current language
    * etc.
    *
    * @return A const pointer to the current SessionData.
    */
   const InternalSessionData* getSessionData() const;
   
   /*
    * Get the current position stored in the location interface.
    *
    * @return The position as a WGS84Coordinate.
    */ 
   WGS84Coordinate getCurrentPosition();

   /**
    * Get the notification center used for Module notifications.
    *
    * @return The internal notification center.
    */
   NotificationCenter* getNotificationCenter();

   /// The storage queue of messages.
   typedef std::deque<isab::GuiProtMess*> MsgQueue;

protected:
   /**
    * Function handling replies from the channel.
    *
    * @param buf The buffer with the GuiProt reply.
    */
   void nav2Reply(isab::Buffer& buf);

   /**
    * Function that sends a GuiProtMess on the Nav2 Channel.
    *
    * @param guiProtMess The message to send.
    * @return The gui prot id assigned to the message.
    */
   wf_uint16 sendGuiMessage(isab::GuiProtMess* guiProtMess);

   /**
    * Called when the QueueThread has stoped Nav2.
    */
   void deleteNav2();
   
   /**
    * Called when the QueueThread has started Nav2.
    *
    * @param status The status of the Nav2 after startup, OK if ok.
    */
   void startupComplete(StatusCode status);

   /**
    * Called when MapLib startup is complete. 
    * 
    * @param status The status of the MapLib after startup, OK if ok.
    */
   void mapLibStartupComplete(StatusCode status);

   /**
    * Handles a unsolicited reply.
    */
   void unsolicitedReply(const isab::GuiProtMess& guiProtMess);

   /**
    * Returns the wanted bitmap extension, without dot.
    *
    * @return The image extension for the current platform, without preceeding
    *         dot.
    */
   virtual const char* getImageExtension(ImageExtension imageExtension) const;

   /** 
    * Returnes the wanted extension for bitmaps.
    *
    * @param imageDimension The selected dimension for this application. Based
    *                       on this this function will return the correct
    *                       width and height as a struct.
    * @return The image dimension for the current platform.
    */
   virtual CombinedSearch::CSImageDimension getImageDimension(
      ImageDimension imageDimension) const;
   
private:

   /**
    * Inserts message types in m_noReplyMessages, these are message types 
    * that we shouldnt expect to get a reply for. 
    */
   void initNoReplyMessageSet();

   /**
    * Common function for doCallbacks and doMapLibCallbacks.
    */
   void internalDoCallbacks(MsgQueue& msgs);

   /**
    * Handle a general parameter message, is called during startup as
    * well as later.
    *
    * @param mess The Set general parameter message.
    */
   void handleSetParameter(const isab::GeneralParameterMess& mess);

   /**
    *  Resets the internal bool matrix of category tags so that
    *  a new request can be handled correctly.
    */ 
   void resetCategoryTags();

   bool allCategoriesUpdated() const;
   
   /// Internal variable used for generating requestID's.
   wf_uint32 m_nextRequestID;

   /// Internal variable used for generating gui prot messageids.
   wf_uint16 m_nextMessageID;

   /// Pointer to Nav2.
   isab::Nav2* m_nav2;

   /**
    * The MapLibAPI, might be NULL, set by connectMapLib function.
    */
   MapLibAPI* m_mapLibAPI;

   /// A pointer to the guiChannel.
   Nav2APIChannel* m_guiChannel;

   /// A pointer to the bluetoothChannel.
   Nav2APIChannel* m_bluetoothChannel;

   /// Queue for storing received asynchronous replies.
   MsgQueue m_msgQueue;

   /**
    * Class for storing information about a request.
    * Stores which interface that made the request and if it is synchronous.
    */
   class RequestInformation
   {
   public:
      /**
       * Constructor.
       *
       * @param interface A pointer to the interface that made the request.
       * @param requestID The RequestID for this request.
       * @param synchronous If the request is synchronous.
       */
      RequestInformation(InterfaceImpl* interface,
                         RequestID requestID,
                         bool synchronous);

      /**
       * Destructor.
       */
      ~RequestInformation();

      /**
       * Get the interface which made the request.
       *
       * @return A pointer to the interface which made the request.
       */
      InterfaceImpl* getInterface();

      /**
       * The RequestID for this request.
       *
       * @return The RequestID
       */
      RequestID getRequestID() const;

      /**
       * If the request was synchronous.
       *
       * @return If the request made was synchronous.
       */
      bool isSynchronous() const;
      
   private:
      /// A pointer to the interface.
      InterfaceImpl* m_interface;

      /// The RequestID for this request.
      RequestID m_requestID;

      /// Stores if the request was synchronous or not.
      bool m_synchronous;
   };

   typedef std::map<wf_uint16, RequestInformation> RequestInformationMap;
   /** 
    *  Map for storing which interface who sent which request and if the
    *  request is synchronous.
    */
   RequestInformationMap m_requestInformation;

   /**
    * The number of synchonous requests right now.
    */
   wf_uint32 m_nbrSynchonous;
   
   /**
    * Function for creating RequestID's.'
    *
    * @return The created RequestID.
    */
   RequestID makeRequestID();

   /**
    * Function used to cancel an earlier made request.
    *
    * @param requestID The requestID for the request to cancel.
    */
   void cancelRequest(RequestID requestID);

   /**
    * The favourites interface.
    */
   FavouriteInterface* m_favouriteInterface;

   /**
    * The network link settings and connection interface.
    */
   ImageInterface* m_imageInterface;

   /**
    * The GPS and cell ID interface.
    */
   LocationInterface* m_locationInterface;

   /**
    * The navigation interface, it relies on route interface for making
    * routes to navigate.
    */
   NavigationInterface* m_navigationInterface;

   /**
    * The network link settings and connection interface.
    */
   NetworkInterface* m_networkInterface;

   /**
    * The route interface.
    */
   RouteInterface* m_routeInterface;

   /**
    * The search interface.
    */
   SearchInterface* m_searchInterface;

   /**
    * The settings interface.
    */
   SettingsInterface* m_settingsInterface;

   /**
    * The billing interface.
    */
   BillingInterface* m_billingInterface;

   /**
    * The tunnel interface.
    */
   TunnelInterface* m_tunnelInterface;

   /**
    * The map requester interface implementation (internal).
    */
   MapRequesterInterfaceImpl* m_mapRequesterInterfaceImpl;

   /**
    * The SessionData created using the data from the startup function
    * call by API user.
    */
   InternalSessionData* m_sessionData;

   /**
    * The listener for the startup and for later errors. 
    * Set by the API user by calling startup function.
    */
   Nav2StatusListener* m_statusListener;

   /**
    * The listener for synchronising callbacks.
    */
   ReadyToRunListener* m_readyToRunListener;

   /**
    * The thread that reads from the m_guiChannel, is created and
    * started in start.
    */
   QueueThread* m_queueThread;

   /// Set of wf_int32 
   typedef std::set<wf_int32> MessageTypeSet;

   /**
    * Messages that are inserted into this set will not get any reply
    * from Nav2. In nav2Request, check if the message type is in this list.
    * then fire and forget.
    */
   MessageTypeSet m_noReplyMessages;

   /// True if shut down in progess, no messages can be sent.
   bool m_shutDown;

   /// The mutex protecting m_requestInformation.
   isab::Mutex* m_mutex;

   /// The queue of messages waiting for synchronised callbacks.
   MsgQueue m_msgs;

   /// The queue of MapLib messages waiting for synchronised callbacks.
   MsgQueue m_mapLibMsgs;

   /// The object containing this.
   Nav2API* m_api;

   /// QueueThread putasds messages in Nav2APIImpl.
   friend class QueueThread;

   /// The status notifier which synchronizes status messages to the GUI
   Nav2StatusNotifier* m_statusNotifier;

   enum CategoryTag { CHECKSUM_TAG = 0, ID_TAG, NAMES_TAG, ICONS_TAG, INTS_TAG,
                      NUM_TAGS};

   bool m_categoryState[NUM_TAGS];

   bool m_queueThreadTerminated;

   NotificationCenter* m_notificationCenter;
};

} // End namespace WFAPI

#endif // End WFAPI_NAV2_API_IMPL_H

