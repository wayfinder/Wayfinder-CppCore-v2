/*
  Copyright (c) 1999 - 2010, Vodafone Group Services Ltd
  All rights reserved.

  Redistribution and use in source and binary forms, with or without modification, are permitted provided that the following conditions are met:

  * Redistributions of source code must retain the above copyright notice, this list of conditions and the following disclaimer.
  * Redistributions in binary form must reproduce the above copyright notice, this list of conditions and the following disclaimer in the documentation and/or other materials provided with the distribution.
  * Neither the name of the Vodafone Group Services Ltd nor the names of its contributors may be used to endorse or promote products derived from this software without specific prior written permission.

  THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS" AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE ARE DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT HOLDER OR CONTRIBUTORS BE LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
*/

#ifndef WFAPI_NAV2_API_H
#define WFAPI_NAV2_API_H

#include "WFAPIConfig.h"
#include "AsynchronousStatus.h"
#include "VoiceLanguage.h"

namespace WFAPI {

// Forward declarations
class NavigationInterface;
class FavouriteInterface;
class StartupData;
class Nav2StatusListener;
class RouteInterface;
class SearchInterface;
class Startup;
class MapLibAPI;
class LocationInterface;
class NetworkInterface;
class ImageInterface;
class SettingsInterface;
class BillingInterface;
class TunnelInterface;
class Nav2APIImpl;
class DBufConnection;
struct ProgramVersion;
struct Nav2APISharedImpl;

/**
 * This is the main class for the API. It holds all the different interfaces 
 * such as search and route.
 * To get a specific interface use the matching get function in this class.
 * 
 * See each interface for information about their usage.
 * 
 * To startup the API first create a Nav2API instance and call start function
 * on it. After the Nav2StatusListener's startupComplete function has been 
 * called the API is ready for use, if there is an error in startup the error
 * function in the Nav2StatusListener is called instead.
 * 
 * To connect a MapLibAPI use the connectMapLib function, this must be done
 * after the Nav2StatusListener has received the startupComplete call. 
 * The Nav2StatusListener will receive mapLibStartupComplete call when
 * MapLib is ready for usage.
 *
 * See MapLibAPI for more information about MapLibAPI usage.
 */
class Nav2API
{

public:

   /**
    * Create the Nav2API, don't forget to call start function.
    */
   Nav2API();

   /**
    * Destructor.
    */
   virtual ~Nav2API();

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
    * Get DBufConnection, i.e. the communication channel which can
    * be supplied into MapLib.
    *
    * @return The DBufConnection.
    */
   DBufConnection* getDBufConnection();

   /**
    * Call function to begin starting the Nav2API. Nav2StatusListener is
    * called when startup has completed.
    *
    * @param statusListener The Nav2StatusListener to call when startup 
    *                       is done and for later errors as well.
    * @param startupData The StartupData with the startup settings.
    * @return The status and if status is OK the startup is initiated and
    *         the statusListener will get a startupComplete, or error,
    *         call when startup is done.
    */
   virtual AsynchronousStatus start(Nav2StatusListener* statusListener, 
                                    StartupData* startupData) = 0;
   
   /**
    * @deprecated Use addStatusListner/removeStatusListener
    * 
    * This method will change the listener that receives status updates
    * from Nav2. To completely disable such notification, pass NULL to
    * this function.
    *
    * @param statusListener The new status listener.
    */ 
   void setStatusListener(Nav2StatusListener* statusListener);

   
   /**
    * @deprecated Use addStatusListner/removeStatusListener
    * 
    * This method will add a listener that receives status updates
    * from Nav2. 
    *
    * @param statusListener The new status listener.
    */ 
   void addStatusListener(Nav2StatusListener* statusListener);
   
   /**
    * @deprecated Use addStatusListner/removeStatusListener
    * 
    * This method will remove a listener that receives status updates
    * from Nav2. 
    *
    * @param statusListener The new status listener.
    */ 
   void removeStatusListener(Nav2StatusListener* statusListener);
   
   /**
    * Set this to true if the connection to the server should use
    * http. If set to false the connection should use socket.
    *
    * @param httpProto True if using http connection, false if using sockets.
    */
   virtual void setHttpProto(bool httpProto);

   /**
    * Get the client type which will use the API.
    *
    * NOTE: This only needs to be set for Symbian platform, hence Linux, WM,
    * Maemo and iPhone clientType is set in their respective Nav2API files.
    * I.E. the Linux client type is set from the beginning the LinuxNav2API.cpp
    * file.
    *
    * @return the client type for this client.
    */
   virtual const WFString& getClientType() const;
   
   /**
    * To set the client type which will use the API.
    *
    * @param clientType The type of the client. 
    */
   virtual void setClientType(const WFString& clientType);
   
   /**
    * To set the client type options. 
    *
    * @param clientTypeOptions The client type options.
    */
   virtual void setClientTypeOptions(const WFString& clientTypeOptions);
   
   /**
    * To set the http server for the requests to our server. Note that
    * this function should only be used if the client don't want to use
    * the ordinary server.
    *
    * NOTE: Implementation only for S60. Linux, WM, have no
    *       implementation yet.
    *
    * @param httpRequestString The http request string containing the server
    *                          adress, such as "http://host:/nav". 
    */
   virtual void setHttpRequestString(const WFString& httpRequestString);

   /**
    * This function sets the version of the client.
    *
    * @param programVersion The version of the application.
    */
   virtual void setProgramVersion(const ProgramVersion& programVersion);

   /**
    * Sets the folder name of the voice directory. The voices should be
    * in the resource data path / the audioDirName. So if the client
    * are using ogg files the preferrable set this path to Vorbis.
    *
    * @param audioTypeDirName The name of the audio type directory.
    */
   virtual void setAudioTypeDirName(const WFString& audioTypeDirName);
   
   /**
    * Starts shut down procedure of Nav2API. When done the
    * Nav2StatusListener will get a call to stopComplete.
    */
   void stop();

   /**
    * Get the two character string for a language.
    *
    * @param lang The language to get a string for.
    * @param uppercase If to return upper case or lower case.
    * @return The string for the language.
    */
   WFString getVoiceLanguageAsString(VoiceLanguage::VoiceLanguage lang, 
                                     bool uppercase);

   /**
    * Internal function.
    */
   Nav2APIImpl* getImpl();

protected:

   /// Internal struct used as a data container.
   Nav2APISharedImpl* m_sharedImpl;
   
private:
   struct Nav2APIImplStruct;

   Nav2APIImplStruct* m_impl;
};

} // End namespace WFAPI

#endif // End WFAPI_NAV2_API_H
