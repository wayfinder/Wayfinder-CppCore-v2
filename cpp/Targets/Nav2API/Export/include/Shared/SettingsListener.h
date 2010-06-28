/*
  Copyright (c) 1999 - 2010, Vodafone Group Services Ltd
  All rights reserved.

  Redistribution and use in source and binary forms, with or without modification, are permitted provided that the following conditions are met:

  * Redistributions of source code must retain the above copyright notice, this list of conditions and the following disclaimer.
  * Redistributions in binary form must reproduce the above copyright notice, this list of conditions and the following disclaimer in the documentation and/or other materials provided with the distribution.
  * Neither the name of the Vodafone Group Services Ltd nor the names of its contributors may be used to endorse or promote products derived from this software without specific prior written permission.

  THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS" AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE ARE DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT HOLDER OR CONTRIBUTORS BE LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
*/

#ifndef WFAPI_SETTINGS_LISTENER_H
#define WFAPI_SETTINGS_LISTENER_H

#include "WFAPIConfig.h"
#include "Listener.h"

namespace WFAPI {

/**
 * Listener for setting events. The abstract functions are the
 * callback functions that is called for the respective asynchronous
 * function call in SettingsInterface.
 *
 * Create a subclass and implement the callbacks and add a 
 * subclass object to the SettingsFavouriteInterface to start getting
 * settingfavourite callbacks.
 *
 * The error function, declared in the super class Listener, will be called
 * for all requests that returns with an error.
 */
class SettingsListener : public Listener
{
   
public:

   /**
    * Constructor with no arguments.
    */
   SettingsListener();

   /**
    * Destructor.
    */
   virtual ~SettingsListener();

   /**
    * Called when a check against the server for a new EULA has been done.
    *
    * @param updated True when a new unseen EULA is available, false if EULA
    * available is the same as before the check against the server.
    * @param filename The filename of the EULA.
    */
   virtual void eulaAvailable(bool updated, WFString filename) = 0;

   /** 
    * Called when a new version is available to download.
    * There is no request for this callback it will just get called when
    * the there is a new version available of this client.
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
   virtual void newVersionAvailable(const WFString& versionNumber,
                                    const WFString& versionUrl,
                                    const WFString& versionString,
                                    bool force) = 0;
   
};

} // End namespace WFAPI

#endif // End WFAPI_SETTINGS_LISTENER_H
