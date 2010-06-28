/*
Copyright (c) 1999 - 2010, Vodafone Group Services Ltd
All rights reserved.

Redistribution and use in source and binary forms, with or without modification, are permitted provided that the following conditions are met:

    * Redistributions of source code must retain the above copyright notice, this list of conditions and the following disclaimer.
    * Redistributions in binary form must reproduce the above copyright notice, this list of conditions and the following disclaimer in the documentation and/or other materials provided with the distribution.
    * Neither the name of the Vodafone Group Services Ltd nor the names of its contributors may be used to endorse or promote products derived from this software without specific prior written permission.

THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS" AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE ARE DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT HOLDER OR CONTRIBUTORS BE LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
*/


#ifndef WFAPITESTCLIENTCONTAINER_H
#define WFAPITESTCLIENTCONTAINER_H

#include <coecntrl.h>		
#include <map>
#include "Shared/MapLibKeyInterface.h"
#include "Shared/SelectedMapObjectListener.h"

class MEikCommandObserver;		

namespace WFAPI {
class MapLibAPI;
class SymbianNativeDrawer;
class SymbianOpenGLESDrawer;
class MapLibKeyInterface;
class Nav2API;
class OverlayItemZoomSpec;
class WGS84Coordinate;
}

class CWFAPITestClientContainer : public CCoeControl,
                                  public WFAPI::SelectedMapObjectListener {
public:
	// constructors and destructor
	CWFAPITestClientContainer();
	static CWFAPITestClientContainer* NewL(const TRect& aRect, 
                                          const CCoeControl* aParent, 
                                          MEikCommandObserver* aObs);

   static CWFAPITestClientContainer* NewLC(const TRect& aRect, 
                                           const CCoeControl* aParent, 
                                           MEikCommandObserver* aObs);
      
	void ConstructL(
		const TRect& aRect, 
		const CCoeControl* aParent, 
		MEikCommandObserver* aCommandObserver);
      
	virtual ~CWFAPITestClientContainer();

   static TInt DrawCallback(TAny* instance);
public:
	// from base class CCoeControl
	TInt CountComponentControls() const;
      
	CCoeControl* ComponentControl(TInt aIndex) const;
      
	TKeyResponse OfferKeyEventL(const TKeyEvent& aKeyEvent, 
                               TEventCode aType);

   void HandlePointerEventL( const TPointerEvent& aPointerEvent );
      
	void HandleResourceChange(TInt aType);

   void InitMap(WFAPI::Nav2API* nav2Api,
                WFAPI::MapLibAPI* mapLib,
                const char* appPath);

   void InitOverlay(const char* appPath);
   
   void MapLibStopped();

   /**
    * Called whenever a stacked dialog has been opened.
    *
    * @see WFAPI::SelectedMapObjectListener::handleStackedDialogOpened
    */
   virtual void handleStackedDialogOpened();

   /**
    * Called whenever a stacked dialog has been closed.
    *
    * @see WFAPI::SelectedMapObjectListener::handleStackedDialogClosed
    */
   virtual void handleStackedDialogClosed();

protected:
	// from base class CCoeControl
	void SizeChanged();

private:
	// from base class CCoeControl
	void Draw(const TRect& aRect) const;

private:
   void handleSelectedMapObject(const WFAPI::MapObjectInfo& mapObjectInfo,
                                WFAPI::OverlayItem* overlayItem,
                                const WFAPI::WGS84Coordinate& coord,
                                bool longPress);
   
   enum LayerId { FRIENDS_LAYER = 0,
                  LOCATIONS_LAYER,
                  SCREEN_LAYER,
                  ME_LAYER};
   
   void InitializeKeyMap();

   void InitSearchHitsLayer(const char* appPath);

   void InitFriendsLayer(const char* appPath);
   
   
   void EnableImmediateDrawing();
      
   /**
    *   This control can switch between OpenGL ES rendering
    *   and native Symbian rendering. This enum signifies the
    *   state we are currently in.
    */
   enum DrawerType { OPENGL_ES, NATIVE };

   /**
    *   Toggles the current drawer in use.
    */
   void toggleDrawer();
      
	void InitializeControlsL();
	void LayoutControls();
      
	CCoeControl* iFocusControl;
	MEikCommandObserver* iCommandObserver;
   
   /**
    *  The native drawer that MapLib uses to draw its maps with. 
    */
   WFAPI::SymbianNativeDrawer* iNativeDrawer;

   /**
    *  The OpenGL ES-based drawer.
    */
   WFAPI::SymbianOpenGLESDrawer* iGLESDrawer;
   
   /**
    *   Our MapLib instance.
    */
   WFAPI::MapLibAPI* m_mapLib;

   /**
    *  The zoom specification for our overlay view.
    */ 
   WFAPI::OverlayItemZoomSpec* m_zoomSpec;
   
   /**
    *   The current drawer that we are using.
    */ 
   DrawerType m_drawer;
   
   typedef std::map<int, WFAPI::MapLibKeyInterface::keyType> KeyMap;
   KeyMap m_keyMap;
public: 
	enum TControls
   {
		// add any user-defined entries here...	
		ELastControl
   };
};
				
#endif // WFAPITESTCLIENTCONTAINER_H
