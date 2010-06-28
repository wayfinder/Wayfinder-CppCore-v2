/*
Copyright (c) 1999 - 2010, Vodafone Group Services Ltd
All rights reserved.

Redistribution and use in source and binary forms, with or without modification, are permitted provided that the following conditions are met:

    * Redistributions of source code must retain the above copyright notice, this list of conditions and the following disclaimer.
    * Redistributions in binary form must reproduce the above copyright notice, this list of conditions and the following disclaimer in the documentation and/or other materials provided with the distribution.
    * Neither the name of the Vodafone Group Services Ltd nor the names of its contributors may be used to endorse or promote products derived from this software without specific prior written permission.

THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS" AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE ARE DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT HOLDER OR CONTRIBUTORS BE LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
*/


#ifndef UI_CONTROL_H
#define UI_CONTROL_H


#include "NavigationInfoUpdateListener.h"
#include "LocationListener.h"
#if ((defined MAEMO4) || (defined MAEMO5))
#include <libosso.h>
#endif
using namespace WFAPI;

// Forward
class GtkDevControl;

/**
 * UI control class that updates actual UI with data.
 */
class UIControl : public NavigationInfoUpdateListener, 
                  public LocationListener
{

public:

   /**
    * Constructor.
    *
    * @param devControl The GtkDevControl used to display UI.
    */
   UIControl(GtkDevControl* devControl);

   /**
   * Destructor.
   */
   virtual ~UIControl();

public: // From base classes

   /**
    * Called when an error has occured.
    */
   virtual void error(const AsynchronousStatus& status);

   /**
    * From NavigationListener, update function called during a route.
    *
    * @param info Info containing information about the route.
    */
   virtual void distanceUpdate(const UpdateNavigationDistanceInfo& info);

   /**
    * From NavigationListener, update function called during a route.
    *
    * @param info Info containing information about the route.
    */
   virtual void infoUpdate(const UpdateNavigationInfo& info);

   /**
    * From NavigationListener, called when a sound should be played.
    *
    * @param info Info containing information about the route.
    */
   virtual void playSound();

   /**
    * From NavigationListener, called before playSound so that we can
    * load sounds etc.
    *
    * @param info Info containing information about the route.
    */
   virtual void prepareSound(const WFStringArray& soundNames);


   /**
    * From LocationListener.
    */
   virtual void areaUpdate(const AreaUpdateInformation& areaUpdateInformation);

   /**
    * From LocationListener.
    */
   virtual void locationUpdate(const LocationUpdateInformation& locationUpdate);

   /**
    * Called when the LBS has started.
    */
   virtual void startedLbs();

   /**
    * Starts snapping center to the positions supplied from the
    * location updates.
    */
   void snapToPosition();

private:

   /**
    * The GtkDevControl, responsible for GUI.
    */
   GtkDevControl* m_devControl;

   /**
    * A osso context member, used in maemo to control hardware, such as backlight
    */
#if ((defined MAEMO4) || (defined MAEMO5))
   osso_context_t* m_ossocontext;
#endif
};

#endif
