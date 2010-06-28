/*
  Copyright (c) 1999 - 2010, Vodafone Group Services Ltd
  All rights reserved.

  Redistribution and use in source and binary forms, with or without modification, are permitted provided that the following conditions are met:

  * Redistributions of source code must retain the above copyright notice, this list of conditions and the following disclaimer.
  * Redistributions in binary form must reproduce the above copyright notice, this list of conditions and the following disclaimer in the documentation and/or other materials provided with the distribution.
  * Neither the name of the Vodafone Group Services Ltd nor the names of its contributors may be used to endorse or promote products derived from this software without specific prior written permission.

  THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS" AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE ARE DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT HOLDER OR CONTRIBUTORS BE LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
*/

#ifndef WFAPI_NAVIGATION_INTERFACE_IMPL_H
#define WFAPI_NAVIGATION_INTERFACE_IMPL_H

#include "WFAPIConfig.h"
#include "InterfaceImpl.h"
#include "SynchronousStatus.h"
#include "RequestID.h"
#include "WFStringArray.h"
#include "DistanceUnit.h"
#include <set>


namespace isab {
// Forward declarations
class GuiProtMess;
}

namespace WFAPI {

// Forward declarations
class NavigationInfoUpdateListener;


/**
 * The behind the scenes class for Navigation Interface.
 */
class NavigationInterfaceImpl : public InterfaceImpl {
public:

   /**
    * Constructor.
    */ 
   NavigationInterfaceImpl(Nav2APIImpl* api);

   /**
    * Destructor.
    */
   virtual ~NavigationInterfaceImpl();

   /**
    * To set the distance unit that is set in settings interface.
    *
    * @param The distance unit set in settings.
    */ 
   void setDistanceUnit(DistanceUnit distanceUnit);

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
    * Functions wrapped from NavigationInterface.
    */
   //@{

   /**
    * Add a listener for navigation callbacks.
    * @see NavigationInfoUpdateListener
    *
    * @param listener The NavigationInfoUpdateListener to add. 
    *                 It may not be deleted until it is removed by
    *                 calling removeNavigationInfoUpateListener.
    */
   void addNavigationInfoUpateListener(
      NavigationInfoUpdateListener* listener);

   /**
    * Remove a listener.
    *
    * @param listener The NavigationInfoUpdateListener to remove.
    *                 It should now be deleted by caller, unless caller
    *                 keeps it around for later use.
    */
   void removeNavigationInfoUpateListener(
      NavigationInfoUpdateListener* listener);

   /**
    * Called after the prepareSound in a NavigationInfoUpdateListener
    * has prepared the sounds.
    *
    * @param duration The playing time of the sounds in milliseconds.
    * @return The status of the operation, StatusCodes::OK if operation
    *         succeeded.
    */
   SynchronousStatus soundPrepared(wf_uint32 duration);

   /**
    * Called after the playSound in a NavigationInfoUpdateListener
    * has played the sounds.
    *
    * @return The status of the operation, StatusCodes::OK if operation
    *         succeeded.
    */
   SynchronousStatus soundPlayed();
   //@}

protected:

private:

   /*
    * Convert a distance in meters to the distance unit set in the settings
    * interface.
    *
    * @param distance The distance to convert.
    * @return The distance in the distance unit set in the settings interface.
    */ 
   WFString convertDistance(wf_uint distance);

   typedef std::set<NavigationInfoUpdateListener*> ListenerCont;

   /**
    * Set of listeners.
    */
   ListenerCont m_listeners;

   /// The current list of sound file names.
   WFStringArray m_soundFileNames;

   /// The current distance unit setting.
   DistanceUnit m_distanceUnit;
};

} // End namespace WFAPI

#endif // End WFAPI_NAVIGATION_INTERFACE_IMPL_H

