/*
Copyright (c) 1999 - 2010, Vodafone Group Services Ltd
All rights reserved.

Redistribution and use in source and binary forms, with or without modification, are permitted provided that the following conditions are met:

    * Redistributions of source code must retain the above copyright notice, this list of conditions and the following disclaimer.
    * Redistributions in binary form must reproduce the above copyright notice, this list of conditions and the following disclaimer in the documentation and/or other materials provided with the distribution.
    * Neither the name of the Vodafone Group Services Ltd nor the names of its contributors may be used to endorse or promote products derived from this software without specific prior written permission.

THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS" AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE ARE DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT HOLDER OR CONTRIBUTORS BE LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
*/


#ifndef IPWFSESSION_H
#define IPWFSESSION_H

#include "NetworkListener.h"
#include "Nav2StatusListener.h"
#import "EAGLView.h"
#import "IPWFLocationHandler.h"
#import <OpenGLES/EAGL.h>
#import <OpenGLES/ES1/gl.h>
#import <OpenGLES/ES1/glext.h>

namespace WFAPI {
class Nav2API;
class MapLibAPI;
class IPhoneOpenGLESDrawer;
}

class IPWFSession : public WFAPI::Nav2StatusListener,
                    public WFAPI::NetworkListener {
public:
   /**
    * Creates a new session towards the WF API:s. It will based
    * upon the supplied view.
    */ 
   IPWFSession(EAGLView* view);

   virtual ~IPWFSession();

   /**
    * @see Nav2StatusListener::startupComplete
    */
   virtual void startupComplete();

   /**
    * @see Nav2StatusListener::mapLibStartupComplete
    */
   virtual void mapLibStartupComplete();

   /**
    * @see Nav2StatusListener::stopComplete
    */
   virtual void stopComplete();

   /**
    * @see NetworkListener::testServerConnectionReply
    *
    * When we have received a valid reply from the server, we continue
    * with the initialization.
    */ 
   virtual void testServerConnectionReply(WFAPI::RequestID requestId);
   
   /**
    * @see WFAPI::Nav2StatusListener
    */
   void error(const WFAPI::AsynchronousStatus& s);

   /**
    *  Initializes our API instances and begins the startup phase.
    */ 
   void init();

   /**
    * Initializes the drawer component of the map.
    */ 
   void initMapDrawer(GLuint viewRenderbuffer,
                      GLuint viewFramebuffer,
                      EAGLContext* eaglContext);

   /**
    * Returns the OpenGL ES based drawer associated with the session.
    */ 
   WFAPI::IPhoneOpenGLESDrawer* getDrawer() const;

   /**
    * Returns the pointer to our maplib member.
    */
   WFAPI::MapLibAPI* getMapLib() const;
   
private:

   /**
    * Adds a simple OverlayItem to the overlayinterface.
    */
   void addSimpleOverlayItem();
   
   /**
    * Retrieves the bundle path for the application, this path
    * will contain the paramseed.txt file.
    */ 
   std::string getBundlePath() const;

   /**
    * Retrieves the documents path for the application, we will store
    * writable data here (such as paramfile, icons etc)
    */ 
   std::string getDocumentsPath() const;
   
   /// Our Nav2API instance
   WFAPI::Nav2API* m_nav2;

   /// Our MapLibAPI instance
   WFAPI::MapLibAPI* m_mapLib;

   /// Our OpenGL ES drawer
   WFAPI::IPhoneOpenGLESDrawer* m_glesDrawer;
   
   /// Our view 
   EAGLView* m_view;

   ///Our Location Handler
   IPWFLocationHandler* m_locationHandler;
};

#endif /* IPWFSESSION_H */
