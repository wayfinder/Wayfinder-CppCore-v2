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
#include <eikappui.h>
#include <WFAPITestClient.rsg>
#include "S60OverlayHelpers.h"
#include "SymbianTextUtil.h"
#include "WFAPITestClientContainer.h"
#include "WFAPITestClientContainerView.h"
#include "WFAPITestClient.hrh"
#include "WFAPITestClientContainer.hrh"
#include "Shared/ConfigInterface.h"
#include "Shared/ScreenPoint.h"
#include "Shared/MapOperationInterface.h"
#include "Shared/MapLibKeyInterface.h"
#include "Shared/MapLibInitialConfig.h"
#include "Shared/MapObjectInterface.h"
#include "Shared/MapDrawingInterface.h"
#include "Shared/MapLibKeyInterface.h"
#include "Shared/WGS84Coordinate.h"
#include "Shared/OverlayInterface.h"
#include "Symbian/SymbianFactory.h"
#include "Symbian/SymbianOpenGLESDrawer.h"
#include "Shared/MapLibNetworkContext.h"
#include "Shared/MapLibAPI.h"
#include "Symbian/SymbianNativeDrawer.h"
#include "Symbian/SymbianOpenGLESDrawer.h"
#include "Shared/Nav2API.h"
#include "S60OverlaySetup.h"
#include "Shared/ConfigInterface.h"
#include "Shared/DetailedConfigInterface.h"
#include "Shared/StackedDialogVisualSpec.h"
#include "Shared/OverlayItemZoomSpec.h"
#include "Shared/ScaleConfigInterface.h"

using namespace WFAPI;

/* Symbian specific keyCodes */
#define KEY_PEN_SHIFT_LEFT  EStdKeyLeftShift
#define KEY_ABC_SHIFT_RIGHT EStdKeyRightShift
#define KEY0                0x30
#define KEY1                0x31
#define KEY2                0x32
#define KEY3                0x33
#define KEY4                0x34
#define KEY5                0x35
#define KEY6                0x36
#define KEY7                0x37
#define KEY8                0x38
#define KEY9                0x39
#define KEY_HASH            EStdKeyHash
#ifdef __WINS__
#define KEY_STAR            EStdKeyNkpAsterisk
#else
#define KEY_STAR            0x2a
#endif
#define KEY_SOFTKEY_LEFT    EStdKeyDevice0
#define KEY_SOFTKEY_RIGHT   EStdKeyDevice1
#define KEY_POWER           EStdKeyDevice2
#define KEY_OK              EStdKeyDevice3
#define KEY_MENU            EStdKeyApplication0
#define KEY_GREEN_PHONE     EStdKeyYes
#define KEY_RED_PHONE       EStdKeyNo


CWFAPITestClientContainer::CWFAPITestClientContainer()
{
   m_zoomSpec = NULL;
   m_mapLib = NULL;
}

CWFAPITestClientContainer::~CWFAPITestClientContainer()
{
   
}
				
CWFAPITestClientContainer*
CWFAPITestClientContainer::NewL(const TRect& aRect, 
                                const CCoeControl* aParent, 
                                MEikCommandObserver* aCommandObserver)
{
	CWFAPITestClientContainer* self =
      CWFAPITestClientContainer::NewLC(aRect, 
                                       aParent, 
                                       aCommandObserver);
	CleanupStack::Pop(self);
	return self;
}

CWFAPITestClientContainer*
CWFAPITestClientContainer::NewLC(const TRect& aRect, 
                                 const CCoeControl* aParent, 
                                 MEikCommandObserver* aCommandObserver)
{
	CWFAPITestClientContainer* self = new (ELeave) CWFAPITestClientContainer();
	CleanupStack::PushL(self);
	self->ConstructL(aRect, aParent, aCommandObserver);
	return self;
}
			
void
CWFAPITestClientContainer::ConstructL(const TRect& aRect, 
                                      const CCoeControl* aParent, 
                                      MEikCommandObserver* aCommandObserver)
{
   m_mapLib = NULL;
   iNativeDrawer = NULL;
   iGLESDrawer = NULL;
	iCommandObserver = aCommandObserver;
	iFocusControl = NULL;
   
   CreateWindowL();
   
   SetExtentToWholeScreen(); // Take the whole screen into use

   // Example: Add disk cache
   // Choose appropriate data directory and put cache there.
   //     m_mapLibHolder->getConfigurator()->addDiskCache(pathToDiskCache,
   //                                                      maxSizeOnDisk);

   InitializeKeyMap();
   
   MakeVisible(true);
   
   SetFocus(true);
   
   SetRect(aRect);
   
   EnableDragEvents();
   
   ActivateL();
}

TInt
CWFAPITestClientContainer::CountComponentControls() const {
	return (int) ELastControl;
}
				
CCoeControl*
CWFAPITestClientContainer::ComponentControl(TInt aIndex) const
{
	switch (aIndex)
   {
      
   }
	
	// handle any user controls here...
	
	return NULL;
}

void CWFAPITestClientContainer::SizeChanged() {
   if ( iNativeDrawer ) {
      // Resize the plotter buffer
      iNativeDrawer->resizeBuffer( Size().iWidth,
                                   Size().iHeight );
   }

   if( iGLESDrawer ) {
      // Resize the plotter buffer
      iGLESDrawer->resizeBuffer( Size().iWidth,
                                 Size().iHeight );
   }

	// CCoeControl::SizeChanged();
}				

void CWFAPITestClientContainer::LayoutControls()
{
   
}

static inline MapLibKeyInterface::kindOfPressType
TranslateKeyEventType( TEventCode aType )
{
   switch ( aType ) {
      case EEventKeyDown:
         return MapLibKeyInterface::KEY_DOWN_EVENT;
      case EEventKeyUp:
         return MapLibKeyInterface::KEY_UP_EVENT;
      case EEventKey:
         return MapLibKeyInterface::KEY_DOWN_EVENT;
      default:
         return MapLibKeyInterface::KEY_UNKNOWN_EVENT;
   }
   return MapLibKeyInterface::KEY_UNKNOWN_EVENT;
}

TKeyResponse
CWFAPITestClientContainer::OfferKeyEventL(const TKeyEvent& aKeyEvent, 
                                          TEventCode aType)
{
   if(!m_mapLib) {
      return EKeyWasNotConsumed;
   }
   
   TKeyResponse response = EKeyWasNotConsumed;

   if( aType == EEventKeyUp ) {
      bool handled = true;
      
      switch( aKeyEvent.iScanCode ) {
         case '7':
            toggleDrawer();
            break;
         case KEY4:
            EnableImmediateDrawing();
            break;
         case '9':
            m_mapLib->getConfigInterface()->set3dMode(
               !m_mapLib->getConfigInterface()->get3dMode() );
            break;
         default:
            handled = false;
            break;
      }

      if( handled) {
         return EKeyWasConsumed;         
      }
   }
   
   KeyMap::const_iterator it = m_keyMap.find( aKeyEvent.iScanCode );
   
   if ( it != m_keyMap.end() ) {
      MapLibKeyInterface* keyInterface = m_mapLib->getKeyInterface();
      
      bool handled =
         keyInterface->handleKeyEvent( it->second,
                                       TranslateKeyEventType( aType ) );
      if ( handled ) {
         return EKeyWasConsumed;
      }         
   }

   if (iFocusControl != NULL
       && iFocusControl->OfferKeyEventL(aKeyEvent, aType) == EKeyWasConsumed)
   {
		return EKeyWasConsumed;
   }
	return CCoeControl::OfferKeyEventL(aKeyEvent, aType);
}
				
void
CWFAPITestClientContainer::InitializeControlsL()
{
	
}

void CWFAPITestClientContainer::HandleResourceChange(TInt aType)
{
   SetExtentToWholeScreen();
}
				
void CWFAPITestClientContainer::Draw(const TRect& aRect) const
{
   if ( ! m_mapLib ) {
      return;
   }

   if( m_drawer == NATIVE ) {	  
      CWindowGc& gc = SystemGc();
   
      // Move temporary rectangle to top-left corner.
      TRect tmpRect( aRect );

      TPoint top(0, 0);
   
      tmpRect.Move( top - iPosition );

      iNativeDrawer->renderTo( gc, TPoint(aRect.iTl), tmpRect );      
   } else if( m_drawer == OPENGL_ES ) {
      iGLESDrawer->render();
   }
}

void CWFAPITestClientContainer::InitializeKeyMap()
{
   m_keyMap[KEY0] = MapLibKeyInterface::ZOOM_OUT_KEY;
   m_keyMap[KEY5] = MapLibKeyInterface::ZOOM_IN_KEY;
   m_keyMap[KEY7] = MapLibKeyInterface::RESET_ROTATION_KEY;
   m_keyMap[KEY1] = MapLibKeyInterface::ROTATE_LEFT_KEY;
   m_keyMap[KEY3] = MapLibKeyInterface::ROTATE_RIGHT_KEY;
   m_keyMap[KEY6] = MapLibKeyInterface::INTERNAL_TRIGGER_KEY;
   m_keyMap[KEY8] = MapLibKeyInterface::INTERNAL_TRIGGER_KEY_2;
   // m_keyMap[KEY_STAR] = MapLibKeyInterface::AUTO_ZOOM_KEY;
   m_keyMap[EStdKeyDownArrow] = MapLibKeyInterface::MOVE_DOWN_KEY;
   m_keyMap[EStdKeyRightArrow] = MapLibKeyInterface::MOVE_RIGHT_KEY;
   m_keyMap[EStdKeyLeftArrow] = MapLibKeyInterface::MOVE_LEFT_KEY;
   m_keyMap[EStdKeyUpArrow] = MapLibKeyInterface::MOVE_UP_KEY;
}

void CWFAPITestClientContainer::toggleDrawer()
{
   if(!m_mapLib) {
      return;
   }
   
   if( m_drawer == OPENGL_ES ) {
      m_drawer = NATIVE;
      m_mapLib->setDrawingContext( iNativeDrawer->getDrawingContext() );
   } else {
      m_drawer = OPENGL_ES;
      m_mapLib->setDrawingContext( iGLESDrawer->getDrawingContext() );
   } 
}

void CWFAPITestClientContainer::EnableImmediateDrawing()
{

}

TInt CWFAPITestClientContainer::DrawCallback(TAny* instance)
{
   User::ResetInactivityTime();
   return 0;
}

void CWFAPITestClientContainer::MapLibStopped()
{
   m_mapLib = NULL;

   m_zoomSpec->removeReference();
   m_zoomSpec = NULL;
   
   /**
    *  It is importan to delete these drawers here and not
    *  in the destructor, as the application may very well
    *  be in a post Exit()-state by then. In those cases
    *  some of the method calls in these destructors will fail.
    */
   
   delete iNativeDrawer;
   delete iGLESDrawer;
   iNativeDrawer = NULL;
   iGLESDrawer = NULL;
}

void
CWFAPITestClientContainer::handleSelectedMapObject(
   const MapObjectInfo& mapObjectInfo,
   OverlayItem* overlayItem,
   const WGS84Coordinate& coord,
   bool longPress)
{
   HBufC* symbianText = SymbianTextUtil::toHBufC(mapObjectInfo.getName().c_str());

   if(longPress) {
      CEikonEnv::InfoWinL( _L("Object sel. (longPress)"),
                           *symbianText );
   } else {
      CEikonEnv::InfoWinL( _L("Object sel."),
                           *symbianText );
   }

   delete symbianText;
}

void CWFAPITestClientContainer::InitMap(WFAPI::Nav2API* nav2Api,
                                        WFAPI::MapLibAPI* mapLib,
                                        const char* appPath)
{
   m_mapLib = mapLib;

   // Add selection notification
   WFAPI::MapObjectInterface* objInterface =
      m_mapLib->getMapObjectInterface();
   
   // Set listener to get notified when map objects have been
   // selected in the map.
   objInterface->addSelectedMapObjectListener(this);
   
   /**
    *  Then, we create a native symbian drawer that MapLib can use to
    *  draw its maps with.
    */ 
   
   iNativeDrawer = SymbianFactory::createNativeDrawer(*this);

   /**
    *  After that we create an OpenGL ES based drawer.
    */
   
   iGLESDrawer = SymbianFactory::createOpenGLESDrawer(*this,
                                                      &this->Window());
   
   //m_drawer = NATIVE;
   m_drawer = OPENGL_ES;
   
   if (m_drawer == NATIVE) {
	   m_mapLib->setDrawingContext(iNativeDrawer->getDrawingContext());
   } else {
	   m_mapLib->setDrawingContext(iGLESDrawer->getDrawingContext());
   }
   
   MapOperationInterface* i = m_mapLib->getMapOperationInterface();

   // i->setCenter(WGS84Coordinate(55.5949, 13.0117));
   i->setCenter(WGS84Coordinate(48.858,2.35487));
   i->setZoomLevel(16.781);

   m_mapLib->getConfigInterface()->
      getDetailedConfigInterface()->enableAutomaticHighlight(true);

   m_mapLib->getConfigInterface()->
      getScaleConfigInterface()->setPosition(10,10);

   nav2Api->connectMapLib(m_mapLib);

   SetExtentToWholeScreen();
   
   if (m_drawer == NATIVE) {
	   iNativeDrawer->resizeBuffer( Size().iWidth,
								    Size().iHeight );
   } else {
	   iGLESDrawer->resizeBuffer( Size().iWidth,
								  Size().iHeight );
   }

   InitOverlay(appPath);
}

void CWFAPITestClientContainer::InitOverlay(const char* appPath)
{
   // Set up the zoom specification that we will be using
   m_zoomSpec = S60OverlaySetup::createZoomSpec(appPath);

   // Set up the stacked dialog spec
   std::pair<StackedDialogVisualSpec*, StackedDialogVisualSpec*> dialogSpecs =
      S60OverlaySetup::createDialogSpec(appPath);

   WFAPI::DetailedConfigInterface* detailedConfig =
      m_mapLib->getConfigInterface()->getDetailedConfigInterface();
   
   detailedConfig->setStackedDialogVisualSpecs(dialogSpecs.first,
                                               dialogSpecs.second);

   dialogSpecs.first->removeReference();
   dialogSpecs.second->removeReference();

   // Initialize some sample layers
   InitFriendsLayer(appPath);
   InitSearchHitsLayer(appPath);
}

void CWFAPITestClientContainer::InitSearchHitsLayer(const char* appPath)
{
   
}

void CWFAPITestClientContainer::InitFriendsLayer(const char* appPath)
{

   std::string extPath(appPath);
   
   extPath += "overlay";

   
   // Holder for the name and the additional description 
   struct NamePair {
      const char* name;
   };

   // Set of names that we will use for our friends layer
   static NamePair names[] = {
      { "John D." },
      { "Foo B." },
      { "John S." },
      { "Joe R." },
      { "Jane D." },
      { "Martin S." },
      { "Henrik F." },
   };
   
   // Create the images for the item
   ImageSpec* overlayImgLevel1 =
      S60OverlayHelpers::loadMif(extPath, "overlay-item.mif");
   ImageSpec* overlayImgLevel2 =
      S60OverlayHelpers::loadMif(extPath, "overlay-item.mif");
   ImageSpec* overlayImgLevel3 =
      S60OverlayHelpers::loadMif(extPath, "overlay-item.mif");
   ImageSpec* highlightIcon =
      S60OverlayHelpers::loadMif(extPath, "overlay-item.mif");


   // Get the overlay interface that we'll be using
   OverlayInterface* overlayInterface = m_mapLib->getOverlayInterface();


   static const int numNames = sizeof(names) / sizeof(NamePair);

   // The range that our random positions will occur in
   static const float maxDiffLon = 0.003;
   static const float maxDiffLat = 0.004;
   
   // Add all of our friends
   for(int i = 0; i < numNames; i++) {
      MapObjectInfo objInfo(names[i].name, 
                            MapObjectInfo::OTHER_TYPE, i);

      // Create normalized random numbers (0..1) to use as
      // offset for our coordinates
      float rLat = rand() / float(RAND_MAX);
      float rLon = rand() / float(RAND_MAX);
      
      // Create the item itself
      OverlayItem* item =
         OverlayItem::allocate(m_zoomSpec, objInfo,
                               WGS84Coordinate(55.59 + rLat * maxDiffLat,
                                               13.0138 + rLon * maxDiffLon));
      // Add the images.
      item->addImageSpec(overlayImgLevel1);
      item->addImageSpec(overlayImgLevel2);
      item->addImageSpec(overlayImgLevel3);
      item->addImageSpec(highlightIcon);

      overlayInterface->addOverlayItem(item, FRIENDS_LAYER);

      item->removeReference();
   }   

   overlayImgLevel1->removeReference();
   overlayImgLevel2->removeReference();
   overlayImgLevel3->removeReference();
   highlightIcon->removeReference();
}

MapLibKeyInterface::kindOfPointerType getPointerType(const TPointerEvent& ev)
{
   switch ( ev.iType ) {
      case TPointerEvent::EButton1Down:
      case TPointerEvent::EButton2Down:
      case TPointerEvent::EButton3Down:
         return MapLibKeyInterface::POINTER_DOWN_EVENT;
         break;
      case TPointerEvent::EButton1Up:
      case TPointerEvent::EButton2Up:
      case TPointerEvent::EButton3Up:
         return MapLibKeyInterface::POINTER_UP_EVENT;
         break;
      case TPointerEvent::EDrag:
         return MapLibKeyInterface::POINTER_UPDATE_EVENT;
         break;
      default:
         return MapLibKeyInterface::POINTER_UNKNOWN_EVENT;
         break;
   }
}

void
CWFAPITestClientContainer::HandlePointerEventL( const TPointerEvent& aPointerEvent )
{   
   WFAPI::ScreenPoint position(aPointerEvent.iPosition.iX,
                               aPointerEvent.iPosition.iY);
   
   MapLibKeyInterface* keyHandler = m_mapLib->getKeyInterface();
   
   bool consumed =
      keyHandler->handlePointerEvent(MapLibKeyInterface::DRAG_TO, 
                                     getPointerType(aPointerEvent),
                                     position);  
}


void CWFAPITestClientContainer::handleStackedDialogOpened() {
//   coreprintln("A stacked dialog was opened.");
}

void CWFAPITestClientContainer::handleStackedDialogClosed() {
//   coreprintln("A stacked dialog was closed.");
}
