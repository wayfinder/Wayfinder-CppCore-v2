/*
Copyright (c) 1999 - 2010, Vodafone Group Services Ltd
All rights reserved.

Redistribution and use in source and binary forms, with or without modification, are permitted provided that the following conditions are met:

    * Redistributions of source code must retain the above copyright notice, this list of conditions and the following disclaimer.
    * Redistributions in binary form must reproduce the above copyright notice, this list of conditions and the following disclaimer in the documentation and/or other materials provided with the distribution.
    * Neither the name of the Vodafone Group Services Ltd nor the names of its contributors may be used to endorse or promote products derived from this software without specific prior written permission.

THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS" AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE ARE DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT HOLDER OR CONTRIBUTORS BE LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
*/


#include <aknviewappui.h>
#include <eikmenub.h>
#include <avkon.hrh>
#include <akncontext.h>
#include <akntitle.h>
#include <stringloader.h>
#include <barsread.h>
#include <eikbtgpc.h>
#include <WFAPITestClient.rsg>
#include "WFAPITestClient.hrh"
#include "WFAPITestClientContainerView.h"
#include "WFAPITestClientContainer.hrh"
#include "WFAPITestClientContainer.h"
#include "Shared/ImageInterface.h"
#include "S60ImageTester.h"
#include "LocationTester.h"
#include "Shared/Nav2API.h"
#include "Shared/MapLibAPI.h"
#include "WFAPITestClientAppUi.h"

CWFAPITestClientContainerView::CWFAPITestClientContainerView()
{
	iWFAPITestClientContainer = NULL;
   m_imageTester = NULL;
   m_locationTester = NULL;
   m_appUi = NULL;		
}

CWFAPITestClientContainerView::~CWFAPITestClientContainerView()
{
	delete iWFAPITestClientContainer;

	iWFAPITestClientContainer = NULL;
}

CWFAPITestClientContainerView*
CWFAPITestClientContainerView::NewL(const char* appPath,
                                    CWFAPITestClientAppUi* appUi)
{
	CWFAPITestClientContainerView* self =
      CWFAPITestClientContainerView::NewLC(appPath, appUi);
	CleanupStack::Pop(self);
	return self;
}

CWFAPITestClientContainerView*
CWFAPITestClientContainerView::NewLC(const char* appPath,
                                     CWFAPITestClientAppUi* appUi)
{
	CWFAPITestClientContainerView* self =
      new (ELeave) CWFAPITestClientContainerView();
	CleanupStack::PushL(self);
	self->ConstructL(appPath, appUi);
	return self;
}

void CWFAPITestClientContainerView::ConstructL(const char* appPath,
                                               CWFAPITestClientAppUi* appUi)
{
   m_nav2Api = NULL;
   m_appPath = appPath;
   m_appUi = appUi;
   
	BaseConstructL(R_WFAPITEST_CLIENT_CONTAINER_WFAPITEST_CLIENT_CONTAINER_VIEW);
}

TUid CWFAPITestClientContainerView::Id() const
{
	return TUid::Uid(EWFAPITestClientContainerViewId);
}

void CWFAPITestClientContainerView::HandleCommandL(TInt aCommand)
{
	TBool commandHandled = EFalse;
	switch (aCommand) {
		case EWFAPITestClientContainerViewBetween_coordsMenuItemCommand:
			commandHandled = HandleBetween_coordsMenuItemSelectedL(aCommand);
			break;
      case EWFAPITestClientContainerViewLoad_to_bufferMenuItemCommand:
         commandHandled = HandleLoad_to_bufferMenuItemSelectedL(aCommand);
         break;
      case EWFAPITestClientContainerViewLoad_to_fileMenuItemCommand:
         commandHandled = HandleLoad_to_fileMenuItemSelectedL(aCommand);
         break;
      case EWFAPITestClientContainerViewStart_LBSMenuItemCommand:
         if(m_locationTester) {
            commandHandled = m_locationTester->handleStartLbs();            
         }
         break;
      case EWFAPITestClientContainerViewStop_LBSMenuItemCommand:
         if(m_locationTester) {
            commandHandled = m_locationTester->handleStopLbs();            
         }
         break;
		default:
			break;
   }
		
	if (!commandHandled) {	
		if (aCommand == EAknSoftkeyExit) {
			AppUi()->HandleCommandL(EEikCmdExit);
      }
   }	
}

void CWFAPITestClientContainerView::DoActivateL(const TVwsViewId&,
                                                TUid,
                                                const TDesC8&)
{
	SetupStatusPaneL();	
				
	if (iWFAPITestClientContainer == NULL)
   {
		iWFAPITestClientContainer = CreateContainerL();
		iWFAPITestClientContainer->SetMopParent(this);
		AppUi()->AddToStackL(*this, iWFAPITestClientContainer);
         
      m_appUi->InitializeAPIs();
   }	
}

void CWFAPITestClientContainerView::DoDeactivate()
{
	CleanupStatusPane();
	
	if (iWFAPITestClientContainer != NULL)
   {
		AppUi()->RemoveFromViewStack(*this, iWFAPITestClientContainer);
		delete iWFAPITestClientContainer;
		iWFAPITestClientContainer = NULL;
   }
}


void CWFAPITestClientContainerView::HandleStatusPaneSizeChange()
{
	CAknView::HandleStatusPaneSizeChange();
	
	// this may fail, but we're not able to propagate exceptions here
	TVwsViewId view;
	AppUi()->GetActiveViewId(view);
	if (view.iViewUid == Id())
   {
		TInt result;
		TRAP(result, SetupStatusPaneL());
   }	
}

void CWFAPITestClientContainerView::SetupStatusPaneL()
{
	// reset the context pane
	TUid contextPaneUid = TUid::Uid(EEikStatusPaneUidContext);
	CEikStatusPaneBase::TPaneCapabilities subPaneContext = 
		StatusPane()->PaneCapabilities(contextPaneUid);
   
	if (subPaneContext.IsPresent() && subPaneContext.IsAppOwned())
   {
		CAknContextPane* context = static_cast< CAknContextPane* > (
			StatusPane()->ControlL(contextPaneUid));
		context->SetPictureToDefaultL();
   }
	
	// setup the title pane
	TUid titlePaneUid = TUid::Uid(EEikStatusPaneUidTitle);
	CEikStatusPaneBase::TPaneCapabilities subPaneTitle = 
		StatusPane()->PaneCapabilities(titlePaneUid);
	if (subPaneTitle.IsPresent() && subPaneTitle.IsAppOwned())
   {
		CAknTitlePane* title = static_cast< CAknTitlePane* >(
			StatusPane()->ControlL(titlePaneUid));
		TResourceReader reader;
		iEikonEnv->CreateResourceReaderLC(reader,
                                         R_WFAPITEST_CLIENT_CONTAINER_TITLE_RESOURCE);
		title->SetFromResourceL(reader);
		CleanupStack::PopAndDestroy(); // reader internal state
   }				
}

void CWFAPITestClientContainerView::CleanupStatusPane()
{
   
}

CWFAPITestClientContainer* CWFAPITestClientContainerView::CreateContainerL()
{
   return CWFAPITestClientContainer::NewL(ClientRect(), NULL, this);
}


TBool
CWFAPITestClientContainerView::HandleBetween_coordsMenuItemSelectedL(TInt aCommand)
{
	// TODO: implement selected event handler
	return ETrue;
}
				

TBool
CWFAPITestClientContainerView::HandleLoad_to_bufferMenuItemSelectedL(TInt aCommand)
{
   if(m_imageTester) {
      m_imageTester->downloadTestImageToBuffer();      
   }
      
	// TODO: implement selected event handler
	return ETrue;
}
				

TBool
CWFAPITestClientContainerView::HandleLoad_to_fileMenuItemSelectedL(TInt aCommand)
{
   if(m_imageTester) {
      m_imageTester->downloadTestImageToFile();      
   }
   
	// TODO: implement selected event handler
	return ETrue;
}

void CWFAPITestClientContainerView::Init(WFAPI::Nav2API* nav2Api,
                                         WFAPI::MapLibAPI* mapLibApi,
                                         const char* appPath)
{
   /**
    *   Finally, we create an instance of the MapLib API.
    */

   m_nav2Api = nav2Api;
   
   m_mapLib = mapLibApi;
   
   m_imageTester = new S60ImageTester(nav2Api->getImageInterface(),
                                      m_appPath);

   m_locationTester = new LocationTester(m_nav2Api->getLocationInterface(),
                                         *m_mapLib->getMapOperationInterface());
   
   iWFAPITestClientContainer->InitMap(nav2Api, m_mapLib, appPath);
}

void CWFAPITestClientContainerView::MapLibAndNav2Stopped()
{
   if(iWFAPITestClientContainer) {
      iWFAPITestClientContainer->MapLibStopped();      
   }
   
   delete m_locationTester;
   delete m_imageTester;
   
   m_locationTester = NULL;
   m_imageTester = NULL;
   m_mapLib = NULL;
   m_nav2Api = NULL;    
}
