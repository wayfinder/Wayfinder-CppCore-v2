/*
  Copyright (c) 1999 - 2010, Vodafone Group Services Ltd
  All rights reserved.

  Redistribution and use in source and binary forms, with or without modification, are permitted provided that the following conditions are met:

  * Redistributions of source code must retain the above copyright notice, this list of conditions and the following disclaimer.
  * Redistributions in binary form must reproduce the above copyright notice, this list of conditions and the following disclaimer in the documentation and/or other materials provided with the distribution.
  * Neither the name of the Vodafone Group Services Ltd nor the names of its contributors may be used to endorse or promote products derived from this software without specific prior written permission.

  THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS" AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE ARE DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT HOLDER OR CONTRIBUTORS BE LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
*/

#include "IPWFSession.h"
#include <iostream>
#include <IPhoneNav2API.h>
#include <string>
#include "StartupData.h"
#include "IPhoneFactory.h"
#include "MapLibInitialConfig.h"
#include "NetworkInterface.h"
#include "IPhoneOpenGLESDrawer.h"
#include "MapLibAPI.h"
#include "IPWFLocationHandler.h"
#include "OverlayInterface.h"
#include "OverlayItemZoomSpec.h"
#include "OverlayItemVisualSpec.h"
#include "ScreenPoint.h"
#include "WGS84Coordinate.h"
#include "ImageSpec.h"

using namespace WFAPI;

IPWFSession::IPWFSession(EAGLView* view) :
   m_view(view),
   m_nav2(NULL),
   m_mapLib(NULL),
   m_glesDrawer(NULL),
   m_locationHandler(NULL)
{
   
}

IPWFSession::~IPWFSession()
{
   delete m_mapLib;
   delete m_glesDrawer;
   delete m_nav2;
}
void IPWFSession::startupComplete()
{
   std::cout << "startupComplete" << std::endl;
   
   m_nav2->getNetworkInterface().addNetworkListener(this);
   m_nav2->getNetworkInterface().testServerConnection();
}

void IPWFSession::testServerConnectionReply(WFAPI::RequestID requestId)
{
   // We use the documents path as root for the file cache, since that
   // path is writable
   MapLibInitialConfig initialConfig;
   initialConfig.setDiskCacheSettings(getDocumentsPath().c_str(),
                                      10*1024*1024);

   m_mapLib = IPhoneFactory::createMapLib(m_nav2->getDBufConnection(),
                                          NULL,
                                          initialConfig);

   m_nav2->connectMapLib(m_mapLib);    
}

void IPWFSession::mapLibStartupComplete()
{
   m_nav2->getNetworkInterface().removeNetworkListener(this);

   // We only set the drawer after we've successfully started MapLib 
   if(m_glesDrawer) {
      m_mapLib->setDrawingContext(m_glesDrawer->getDrawingContext());
   }
   
   std::cout << "mapLibStartupComplete" << std::endl;
   m_locationHandler = new IPWFLocationHandler(m_nav2->getLocationInterface(), (*m_mapLib));
   addSimpleOverlayItem();
}

void IPWFSession::stopComplete()
{
   std::cout << "stopComplete" << std::endl;
}

void IPWFSession::error(const AsynchronousStatus& s)
{
   std::cout << "error: " << s.getStatusMessage().c_str() << std::endl;
   std::cout << "code: " << s.getStatusCode() << std::endl;
}

std::string IPWFSession::getBundlePath() const
{
   NSString *nsBundlePath = [[NSBundle mainBundle] resourcePath];
   std::string bundleStr( [nsBundlePath cString] + std::string("/"));
   return bundleStr;
}

std::string IPWFSession::getDocumentsPath() const
{
   NSArray * docList = NSSearchPathForDirectoriesInDomains(NSDocumentDirectory,
                                                           NSUserDomainMask,
                                                           YES);
   
   /** Take the last path in the list, this is the most suitable */
   NSString* nsDocumentsDirectory = [docList lastObject];
   std::string docPathStr([nsDocumentsDirectory cString] + std::string("/"));

   return docPathStr;
}

void IPWFSession::init()
{
   m_nav2 = new IPhoneNav2API;

   /* Retrieve the location of the bundle package, this is where our
    * paramseed will be located. */
   std::string bundleStr(getBundlePath());

   /**
    * Retrieve the location of our documents folder, this is where we
    * will put our writable data.
    */
   std::string docPathStr(getDocumentsPath());
   
   WFAPI::StartupData startupData(bundleStr.c_str(),
                                  docPathStr.c_str(),
                                  WFAPI::ENGLISH,
                                  WFAPI::VoiceLanguage::ENGLISH,
                                  docPathStr.c_str(),
                                  docPathStr.c_str(),
                                  docPathStr.c_str());
	std::string clientTypeOptions(std::string("OpCo: GBR"));
	m_nav2->setClientTypeOptions(clientTypeOptions.c_str());
	m_nav2->start(this, &startupData);
}

void IPWFSession::addSimpleOverlayItem()
{
   // Load the png image
   UIImage *ximage = [UIImage imageNamed:@"test.png"];
   
   // Get the CGImage reference
   CGImageRef cimage = [ximage CGImage];
   
   // Create the spec from the CGImage
   ImageSpec* bgImg = IPhoneFactory::createIPhoneImageSpec(cimage);

   // Release the CGImage 
   CGImageRelease(cimage);
   
   // The visual specification with the background image supplied and the
   // focus point for it. 
   WFAPI::OverlayItemVisualSpec* visualSpec =
      OverlayItemVisualSpec::allocate(bgImg, WFAPI::ScreenPoint(43,115));

   WFAPI::OverlayItemZoomSpec* zoomSpec = OverlayItemZoomSpec::allocate();

   // We use only one zoom level range for the item 
   zoomSpec->addZoomLevelRange(0,100000, visualSpec, visualSpec);

   // The mapobjectinfo that should be attached to the item
   WFAPI::MapObjectInfo inf("TestItem", 1, 47);

   // Allocate the item itself, put it somewhere in Paris.
   WFAPI::OverlayItem* item =
      WFAPI::OverlayItem::allocate(zoomSpec,
                                   inf,
                                   WGS84Coordinate(48.89, 2.350 ));
   // Set the stackability of the item to false. This means that it can overlap
   // and be overlapped by other items.
   item->setStackable(false);
   
   // Add it to layer 1.
   m_mapLib->getOverlayInterface()->addOverlayItem(item, 1);
      
   // Remove our references to the WFAPI::RefCounted objects.
   bgImg->removeReference();
   item->removeReference();
   zoomSpec->removeReference();
   visualSpec->removeReference();

}

void IPWFSession::initMapDrawer(GLuint viewRenderbuffer,
                                GLuint viewFramebuffer,
                                EAGLContext* eaglContext)
{
   if(m_glesDrawer) {
      // If we've already created an OpenGL ES based renderer, we only
      // need to update its context.
      
      m_glesDrawer->updateContext(viewRenderbuffer,
                                  viewFramebuffer,
                                  eaglContext); 
   } else {
      // If we have not created an OpenGL ES based drawer at this time,
      // we do so using the supplied render and frame buffers
      m_glesDrawer = new IPhoneOpenGLESDrawer(m_view,
                                              viewRenderbuffer,
                                              viewFramebuffer,
                                              eaglContext);
      
      if(m_mapLib) {
         m_mapLib->setDrawingContext(m_glesDrawer->getDrawingContext());
      }
   } 
}

WFAPI::IPhoneOpenGLESDrawer*
IPWFSession::getDrawer() const
{
   return m_glesDrawer; 
}

WFAPI::MapLibAPI*
IPWFSession::getMapLib() const
{
   return m_mapLib;    
}




