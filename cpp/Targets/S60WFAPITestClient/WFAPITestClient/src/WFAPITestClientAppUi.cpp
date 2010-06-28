/*
Copyright (c) 1999 - 2010, Vodafone Group Services Ltd
All rights reserved.

Redistribution and use in source and binary forms, with or without modification, are permitted provided that the following conditions are met:

    * Redistributions of source code must retain the above copyright notice, this list of conditions and the following disclaimer.
    * Redistributions in binary form must reproduce the above copyright notice, this list of conditions and the following disclaimer in the documentation and/or other materials provided with the distribution.
    * Neither the name of the Vodafone Group Services Ltd nor the names of its contributors may be used to endorse or promote products derived from this software without specific prior written permission.

THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS" AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE ARE DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT HOLDER OR CONTRIBUTORS BE LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
*/


#include "S60/S60WorkArounds.h"
#include <eikapp.h>
#include <vector>
#include <eikmenub.h>
#include <akncontext.h>
#include <akntitle.h>
#include <WFAPITestClient.rsg>
#include <commdb.h>
#include <ApSettingsHandlerUI.h>
#include <e32debug.h>

#include "WFAPITestClientAppUi.h"
#include "WFAPITestClient.hrh"
#include "WFAPITestClientContainer.hrh"
#include "WFAPITestClientContainerView.h"

#include "Shared/LocationInterface.h"
#include "Shared/FavouriteInterface.h"
#include "Shared/MapLibInitialConfig.h"
#include "Shared/TextLanguage.h"
#include "Shared/VoiceLanguage.h"
#include "Shared/StartupData.h"
#include "Shared/MapLibStartupListener.h"
#include "Shared/MapLibAPI.h"
#include "Shared/NetworkLinkID.h"
#include "Shared/NetworkInterface.h"
#include "S60/S60Nav2API.h"
#include "SymbianTextUtil.h"
#include "Symbian/SymbianFactory.h"

using namespace WFAPI;

void CWFAPITestClientAppUi::AccessPointGetterL()
{
	iConnOK=EFalse;
 
	CApSettingsHandler* settingsHandler = CApSettingsHandler::NewLC(
      ETrue, 
      EApSettingsSelListIsPopUp   , 
      EApSettingsSelMenuSelectNormal,
      KEApIspTypeAll, 
      EApBearerTypeAll, 
      KEApSortNameAscending);
    
   TUint32 originallyFocused(0);
   // 0 is not valid IAP id. Failed if 0 after RunSettingsL().
   TUint32 selectedIap(0); 
 
   // Show the dialog
   settingsHandler->RunSettingsL(originallyFocused, selectedIap);
 
	// The CApUtils API is deprecated/removed, the following code can be  used
   // to convert the WapAP Id to IAP Id    
   if (selectedIap) {
      CCommsDatabase* db = CCommsDatabase::NewL();
      CleanupStack::PushL(db);
 
      CCommsDbTableView* wapTable;
      wapTable = db->OpenViewMatchingUintLC(TPtrC(WAP_ACCESS_POINT),
                                            TPtrC(COMMDB_ID), selectedIap);
      User::LeaveIfError(wapTable->GotoFirstRecord());
 
      TBuf<100> wapBearer;
      wapTable->ReadTextL(TPtrC(WAP_CURRENT_BEARER), wapBearer);
 
      if (wapBearer == TPtrC(WAP_IP_BEARER)) {
         CCommsDbTableView* bearerTable;
         bearerTable = db->OpenViewMatchingUintLC(TPtrC(wapBearer),
                                                  TPtrC(WAP_ACCESS_POINT_ID), 
                                                  selectedIap);
         User::LeaveIfError(bearerTable->GotoFirstRecord());
         bearerTable->ReadUintL(TPtrC(WAP_IAP), iIap);
         CleanupStack::PopAndDestroy(bearerTable); // bearerTable
      } else {
         User::Leave(KErrInvalidBearerType);
      }
         
      CleanupStack::PopAndDestroy(2); // db, wapTable,         	
   } else {
      iIap = 0;
   }
 
   CleanupStack::PopAndDestroy(settingsHandler);
    
   if(!iIap) {
    	iConnOK=EFalse;
   } else {
      iConnOK=ETrue;
   }
}

CWFAPITestClientAppUi::CWFAPITestClientAppUi()
{
   m_appPath = NULL;
}

CWFAPITestClientAppUi::~CWFAPITestClientAppUi()
{
   delete[] m_appPath;
   delete m_nav2Api;
   delete m_mapLib;
}

void CWFAPITestClientAppUi::InitializeContainersL()
{
	iWFAPITestClientContainerView =
      CWFAPITestClientContainerView::NewL(m_appPath, this);
   
	AddViewL(iWFAPITestClientContainerView);
	SetDefaultViewL(*iWFAPITestClientContainerView);
}

void CWFAPITestClientAppUi::HandleCommandL(TInt aCommand)
{
	TBool commandHandled = EFalse;
	switch (aCommand) {
		default:
			break;
   }	
		
	if (!commandHandled) {
		if (aCommand == EAknSoftkeyExit || aCommand == EEikCmdExit) {
         
         if(m_nav2Api) {
            // Perform the initial shutdown here. Notify the container
            // that the two API's are from now on inaccessible.
            m_nav2Api->stop();
            m_mapLib->stop();
            iWFAPITestClientContainerView->MapLibAndNav2Stopped();
         }
         
      }
   }	
}

void CWFAPITestClientAppUi::HandleResourceChangeL(TInt aType) {
	CAknViewAppUi::HandleResourceChangeL(aType);
}

TKeyResponse CWFAPITestClientAppUi::HandleKeyEventL(const TKeyEvent& aKeyEvent,
                                                    TEventCode aType)
{	
	return EKeyWasNotConsumed;
}

void
CWFAPITestClientAppUi::HandleViewDeactivation(const TVwsViewId& aDeactivateView, 
                                              const TVwsViewId& aNewView)
{
	CAknViewAppUi::HandleViewDeactivation(aDeactivateView, aNewView);
}

const char* CWFAPITestClientAppUi::GetApplicationPath()
{
   _LIT(KSysDataBasePath, "\\shared\\");
   _LIT(KPathTrailer, "\\");
      
   HBufC* applicationDllName = Application()->DllName().AllocL();
   class TParsePtrC pathParser(*applicationDllName);
   HBufC* applicationName = pathParser.Name().AllocL();
   HBufC* applicationNameBase = NULL;
      
   TInt sepPos = 0;
   sepPos = applicationName->LocateReverse('_');
   if (sepPos != KErrNotFound) {
      // Create a application name without the architecture type in it.
      applicationNameBase = HBufC::NewL(applicationName->Length());
      applicationNameBase->Des().Copy(applicationName->Left(sepPos));
   } else {
      applicationNameBase = applicationName->AllocL();
   }

   HBufC* dataPath = HBufC::NewL(pathParser.Drive().Length() +
                                  KSysDataBasePath().Length() +
                                  applicationName->Length() +
                                  KPathTrailer().Length());
      
   dataPath->Des().Copy(pathParser.Drive());
   dataPath->Des().Append(KSysDataBasePath);
   dataPath->Des().Append(*applicationNameBase);
   dataPath->Des().Append(KPathTrailer);

   /* Setting the drive component to C on windows... */
   //This is since when using the SDK the app drive is Z.
#if defined __WINS__ || defined __WINSCW__
   dataPath->Des()[0] = 'c';
#endif

   delete applicationDllName;
   delete applicationName;
   delete applicationNameBase;

   const char* ret = SymbianTextUtil::toUtf8(*dataPath);

   delete dataPath;
   
   return ret;
}

/**
 * @brief Completes the second phase of Symbian object construction. 
 * Put initialization code that could leave here. 
 */ 
void CWFAPITestClientAppUi::ConstructL()
{
	BaseConstructL(EAknEnableSkin);
   
   m_appPath = GetApplicationPath();

   WFAPI::S60WorkArounds::fixExports();

#ifndef __WINS__
   AccessPointGetterL();
#endif
   
   InitializeContainersL();
}

void CWFAPITestClientAppUi::InitializeAPIs()
{
   if(m_nav2Api) {
      return;
   }
   
    // If to use MapLib and set language for MapLib
   TextLanguage textLanguage = ENGLISH;  //Default is English
   VoiceLanguage::VoiceLanguage voiceLanguage = VoiceLanguage::ENGLISH;
   
   StartupData startupData(m_appPath,
                           m_appPath,
                           m_appPath,
                           textLanguage,
                           voiceLanguage,
                           m_appPath,
                           m_appPath);   
   
   m_nav2Api = new WFAPI::S60Nav2API;
   m_nav2Api->setHttpProto(ETrue);
   m_nav2Api->setClientType("wf-linux-demo");
   m_nav2Api->start(this, &startupData);
}

void CWFAPITestClientAppUi::startupComplete()
{
   /// Initialize our network properties
   
   NetworkInterface& network = m_nav2Api->getNetworkInterface();

   network.setKeepLinkUp(true);

#ifndef __WINS__
   NetworkLinkID link(iIap);
#else
   NetworkLinkID link(WFAPI::NetworkLinkID::TRY_DEFAULT);
#endif
   
   network.setNetworkLinkID(link);

   /// Finally, initialize our maplib instance
   
   MapLibInitialConfig initialConfig;
   initialConfig.setDiskCacheSettings(m_appPath,1000000);   
   m_mapLib = SymbianFactory::createMapLib(m_nav2Api->getDBufConnection(),
                                           NULL,
                                           initialConfig);
   
   iWFAPITestClientContainerView->Init(m_nav2Api, m_mapLib, m_appPath);
}

void CWFAPITestClientAppUi::mapLibStartupComplete()
{

}

void CWFAPITestClientAppUi::stopComplete()
{
   Exit();
}

void CWFAPITestClientAppUi::error(const AsynchronousStatus& status)
{

}

