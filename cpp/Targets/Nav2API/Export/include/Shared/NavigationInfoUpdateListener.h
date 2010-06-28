/*
  Copyright (c) 1999 - 2010, Vodafone Group Services Ltd
  All rights reserved.

  Redistribution and use in source and binary forms, with or without modification, are permitted provided that the following conditions are met:

  * Redistributions of source code must retain the above copyright notice, this list of conditions and the following disclaimer.
  * Redistributions in binary form must reproduce the above copyright notice, this list of conditions and the following disclaimer in the documentation and/or other materials provided with the distribution.
  * Neither the name of the Vodafone Group Services Ltd nor the names of its contributors may be used to endorse or promote products derived from this software without specific prior written permission.

  THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS" AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE ARE DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT HOLDER OR CONTRIBUTORS BE LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
*/

#ifndef WFAPI_NAVIGATION_INFO_UPDATE_LISTENER_H
#define WFAPI_NAVIGATION_INFO_UPDATE_LISTENER_H

#include "WFAPIConfig.h"
#include "Listener.h"
#include "UpdateNavigationDistanceInfo.h"
#include "UpdateNavigationInfo.h"
#include "WFStringArray.h"

namespace WFAPI {

/**
 * Listener for navigation call backs. The abstract functions are the
 * callback functions that is called for the respective asynchronous
 * function call in NavigationInterface.
 *
 * This listener makes it possible to make a navigation user interface
 * that is constantly updated as the position is updated.
 */
class NavigationInfoUpdateListener : public Listener
{

public:

   /**
    * Constructor with no arguments.
    */
   NavigationInfoUpdateListener();

   /**
    * Destructor.
    */
   virtual ~NavigationInfoUpdateListener();

   /**
    * Called any number of times between infoUpdate calls. 
    * A distanceUpdate call is an information update of the last
    * infoUpdate call.
    *
    * @param info The information update.
    */
   virtual void distanceUpdate(const UpdateNavigationDistanceInfo& info) = 0;

   /**
    * Called when changing from one turn to the next.
    *
    * @param info The information of the upcoming turn.
    */
   virtual void infoUpdate(const UpdateNavigationInfo& info) = 0;

   /**
    * Play the sounds from the last prepareSound call.
    * When the sounds has been played call soundPlayed in the
    * NavigationInterface.
    */
   virtual void playSound() = 0;

   /**
    * List of names of sound files to be played at the next playSound
    * call. Load the sound files from the sound clips directory.
    * When the sounds has been loaded call soundPrepared in the
    * NavigationInterface.
    *
    * @param soundNames The sound files names.
    */
   virtual void prepareSound(const WFStringArray& soundNames) = 0;
};

} // End namespace WFAPI

#endif // End WFAPI_NAVIGATION_INFO_UPDATE_LISTENER_H
