/*
  Copyright (c) 1999 - 2010, Vodafone Group Services Ltd
  All rights reserved.

  Redistribution and use in source and binary forms, with or without modification, are permitted provided that the following conditions are met:

  * Redistributions of source code must retain the above copyright notice, this list of conditions and the following disclaimer.
  * Redistributions in binary form must reproduce the above copyright notice, this list of conditions and the following disclaimer in the documentation and/or other materials provided with the distribution.
  * Neither the name of the Vodafone Group Services Ltd nor the names of its contributors may be used to endorse or promote products derived from this software without specific prior written permission.

  THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS" AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE ARE DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT HOLDER OR CONTRIBUTORS BE LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
*/

#ifndef WFAPI_NAVIGATION_INTERFACE_H
#define WFAPI_NAVIGATION_INTERFACE_H

#include "WFAPIConfig.h"
#include "Interface.h"
#include "SynchronousStatus.h"

namespace WFAPI {

// Forward declarations
class NavigationInfoUpdateListener;
class Nav2APIImpl;
class NavigationInterfaceImpl;

/**
 * Interface class for navigating. The NavigationInterface uses
 * the current route made in the RouteInterface.
 */
class NavigationInterface : public Interface
{

public:

   /**
    * Destructor.
    */
   virtual ~NavigationInterface();

   /**
    * Add a listener for navigation updates.
    * @see NavigationInfoUpdateListener
    *
    * @param listener The NavigationInfoUpdateListener to add. 
    *                 It may not be deleted until it is removed by
    *                 calling removeNavigationInfoUpateListener.
    */
   void addNavigationInfoUpdateListener(
      NavigationInfoUpdateListener* listener);

   /**
    * Remove a listener.
    *
    * @param listener The NavigationInfoUpdateListener to remove.
    *                 It should now be deleted by caller, unless caller
    *                 keeps it around for later use.
    */
   void removeNavigationInfoUpdateListener(
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

   /**
    * Internal function.
    */
   NavigationInterfaceImpl* getImpl();

private:
   /**
    * Do not create NavigationInterface get it from the Nav2API class.
    */
   NavigationInterface(Nav2APIImpl* nav2APIimpl);

   /**
    * We do not allow copies of this interface.
    */
   NavigationInterface(const NavigationInterface&);
   const NavigationInterface& operator=(const NavigationInterface&);

   friend class Nav2APIImpl;

   struct impl;

   impl* m_impl;
};

} // End namespace WFAPI

#endif // End WFAPI_NAVIGATION_INTERFACE_H
