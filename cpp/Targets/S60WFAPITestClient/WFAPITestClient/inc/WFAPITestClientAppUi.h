/*
Copyright (c) 1999 - 2010, Vodafone Group Services Ltd
All rights reserved.

Redistribution and use in source and binary forms, with or without modification, are permitted provided that the following conditions are met:

    * Redistributions of source code must retain the above copyright notice, this list of conditions and the following disclaimer.
    * Redistributions in binary form must reproduce the above copyright notice, this list of conditions and the following disclaimer in the documentation and/or other materials provided with the distribution.
    * Neither the name of the Vodafone Group Services Ltd nor the names of its contributors may be used to endorse or promote products derived from this software without specific prior written permission.

THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS" AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE ARE DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT HOLDER OR CONTRIBUTORS BE LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
*/


#ifndef WFAPITESTCLIENTAPPUI_H
#define WFAPITESTCLIENTAPPUI_H

#include <aknviewappui.h>
#include "Shared/Nav2StatusListener.h"

namespace WFAPI {
class Nav2API;
class MapLibAPI;
}

class CWFAPITestClientContainerView;

class CWFAPITestClientAppUi : public CAknViewAppUi,
                              public WFAPI::Nav2StatusListener
{
public: 
	// constructor and destructor
	CWFAPITestClientAppUi();
	virtual ~CWFAPITestClientAppUi();
	void ConstructL();
   void SecondPhaseConstruction();
public:
	// from CCoeAppUi
	TKeyResponse HandleKeyEventL(const TKeyEvent& aKeyEvent, TEventCode aType );

	// from CEikAppUi
	void HandleCommandL( TInt aCommand );
	void HandleResourceChangeL( TInt aType );

	// from CAknAppUi
	void HandleViewDeactivation( const TVwsViewId& aViewIdToBeDeactivated, 
                                const TVwsViewId& aNewlyActivatedViewId );

   // From Listener.h
   virtual void error(const WFAPI::AsynchronousStatus& status);

   // From Nav2StatusListener
   virtual void startupComplete();
   virtual void mapLibStartupComplete();
   virtual void stopComplete();

   /// Star the initialization of the API's
   void InitializeAPIs();

private:
   
   /// Initialize our map container
	void InitializeContainersL();
   
   /// Retrieves an IAP point and stores the result in iIAp and iConnOK
   void AccessPointGetterL();

   /// The view which contains our map container
	CWFAPITestClientContainerView* iWFAPITestClientContainerView;

   /**
    * Locates a path on device or emulator that can be used to
    * store data files etc.
    */ 
   const char* GetApplicationPath();

   /// Our Nav2 API instance
   WFAPI::Nav2API* m_nav2Api;

   /// Our MapLib API instance
   WFAPI::MapLibAPI* m_mapLib;

   /// The writable application path
   const char* m_appPath;

   /// The IAP used for the connection
   TUint32 iIap;

   /// Whether or not the IAP was successfully retrieved
   TBool iConnOK;
};

#endif // WFAPITESTCLIENTAPPUI_H			
