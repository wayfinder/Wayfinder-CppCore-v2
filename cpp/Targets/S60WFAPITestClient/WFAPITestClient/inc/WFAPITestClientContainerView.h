/*
Copyright (c) 1999 - 2010, Vodafone Group Services Ltd
All rights reserved.

Redistribution and use in source and binary forms, with or without modification, are permitted provided that the following conditions are met:

    * Redistributions of source code must retain the above copyright notice, this list of conditions and the following disclaimer.
    * Redistributions in binary form must reproduce the above copyright notice, this list of conditions and the following disclaimer in the documentation and/or other materials provided with the distribution.
    * Neither the name of the Vodafone Group Services Ltd nor the names of its contributors may be used to endorse or promote products derived from this software without specific prior written permission.

THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS" AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE ARE DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT HOLDER OR CONTRIBUTORS BE LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
*/


#ifndef WFAPITESTCLIENTCONTAINERVIEW_H
#define WFAPITESTCLIENTCONTAINERVIEW_H

#include <aknview.h>
class CWFAPITestClientContainer;
class S60ImageTester;
class LocationTester;
class CWFAPITestClientAppUi;

namespace WFAPI {
class Nav2API;
class MapLibAPI;
}
			
class CWFAPITestClientContainerView : public CAknView {
public:
	CWFAPITestClientContainerView();
   
   static CWFAPITestClientContainerView* NewL(const char* appPath,
                                              CWFAPITestClientAppUi* appUi);
   static CWFAPITestClientContainerView* NewLC(const char* appPath,
                                               CWFAPITestClientAppUi* appUi);
   
   void ConstructL(const char* appPath,
                   CWFAPITestClientAppUi* appUi);
   
	virtual ~CWFAPITestClientContainerView();

   // Initialize our context and enable hooks to MapLib and Nav2
   void Init(WFAPI::Nav2API* nav2Api,
             WFAPI::MapLibAPI* mapLibApi,
             const char* appPath);

   // Called when the Nav2 and MapLib API:s have been closed
   void MapLibAndNav2Stopped();
   
	// from base class CAknView
	TUid Id() const;
   
	void HandleCommandL( TInt aCommand );	

	CWFAPITestClientContainer* CreateContainerL();
protected:
   void DoActivateL( const TVwsViewId& aPrevViewId,
                     TUid aCustomMessageId,
                     const TDesC8& aCustomMessage );
   
	void DoDeactivate();
   
	void HandleStatusPaneSizeChange();
	
	TBool HandleBetween_coordsMenuItemSelectedL( TInt aCommand );
   
   TBool HandleLoad_to_bufferMenuItemSelectedL( TInt aCommand );
   
	TBool HandleLoad_to_fileMenuItemSelectedL( TInt aCommand );
   
private:
   
	void SetupStatusPaneL();
   
	void CleanupStatusPane();
	
	CWFAPITestClientContainer* iWFAPITestClientContainer;

   /// Our Nav2 API instance
   WFAPI::Nav2API* m_nav2Api;

   /// Our MapLib instance
   WFAPI::MapLibAPI* m_mapLib;

   /// Class used to test fetching of images etc.
   S60ImageTester* m_imageTester;

   /// Class used to test location updates etc
   LocationTester* m_locationTester;

   /// The path to a writable application folder
   const char* m_appPath;

   /// Our AppUI instance
   CWFAPITestClientAppUi* m_appUi;
};

#endif // WFAPITESTCLIENTCONTAINERVIEW_H
