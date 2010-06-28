/*
Copyright (c) 1999 - 2010, Vodafone Group Services Ltd
All rights reserved.

Redistribution and use in source and binary forms, with or without modification, are permitted provided that the following conditions are met:

    * Redistributions of source code must retain the above copyright notice, this list of conditions and the following disclaimer.
    * Redistributions in binary form must reproduce the above copyright notice, this list of conditions and the following disclaimer in the documentation and/or other materials provided with the distribution.
    * Neither the name of the Vodafone Group Services Ltd nor the names of its contributors may be used to endorse or promote products derived from this software without specific prior written permission.

THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS" AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE ARE DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT HOLDER OR CONTRIBUTORS BE LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
*/


#ifndef NAV2SESSION_H
#define NAV2SESSION_H

#include "Nav2StatusListener.h"
#include "RegressionTest.h"

namespace WFAPI {
class Nav2API;
class MapLibAPI;
}

class Nav2Session : public WFAPI::Nav2StatusListener {
public:
   Nav2Session();

   virtual ~Nav2Session();

   /**
    * @see WFAPI::Nav2StatusListener
    */
   void error(const WFAPI::AsynchronousStatus& s);

   /**
    * @see WFAPI::Nav2StatusListener
    */ 
   void startupComplete();

   /**
    * @see WFAPI::Nav2StatusListener
    */ 
   void mapLibStartupComplete();

   /**
    * @see WFAPI::Nav2StatusListener
    */ 
   void stopComplete();

   /*/
    * Returns the Nav2 instance that this session is responsible for.
    **/
   WFAPI::Nav2API* getNav2();

   /**
    * Returns the MapLib instance that this session is responsible for.
    * Note that while there is always a Nav2 instance available, an
    * external test or similar must create and set the MapLib instance
    * via setMapLib before it is valid.
    */ 
   WFAPI::MapLibAPI* getMapLib();

   /**
    * Sets the instance of mapLib that this session will own.
    */ 
   void setMapLib(WFAPI::MapLibAPI* mapLib);
   
   /**
    * @return The folder in which the param seed file is located.
    */ 
   const char* getParamSeedPath();

   /**
    * @return The folder in which the parameter files are written.
    */
   const char* getParameterStoragePath();

   /**
    * @return The folder in which the resource files are found (i.e. sounds)
    */ 
   const char* getResourceStoragePath();

   /**
    * @return The folder that the images will be downloaded to.
    */ 
   const char* getImageStoragePath();

   /**
    * @return The folder that the category images will be downloaded to.
    */ 
   const char* getCategoryImageStoragePath();

   /**
    * @return The folder that the category tree images will be downloaded to.
    */ 
   const char* getCategoryTreeImageStoragePath();
   
   /**
    * Redirects the status updates so that they go to newListener instead.
    *
    * @param newListener The listener that should receive the status updates.
    */ 
   void redirectStatusNotification(WFAPI::Nav2StatusListener* newListener);

   /**
    * Redirects the status updates so that they go to the session again.
    *
    * @param prevListener The listener that previously received the updates.
    */
   void resetStatusNotification(WFAPI::Nav2StatusListener* prevListener);

   void deleteMapLib();

   void deleteNav2();
private:
   void initPaths();
   
   void createNav2();
   
   WFAPI::Nav2API* m_nav2;
   WFAPI::MapLibAPI* m_mapLib;

   /// The directory in which the param seed file will be located
   char* m_paramSeedPath;
   /// The directory where the parameter files will be written
   char* m_paramFilesPath;
   /// The directory where the resources (sounds) will be placed
   char* m_resourcePath;
   /// The directory that the images will be downloaded to
   char* m_imagesPath;
   /// The directory that the category images will be downloaded to
   char* m_catImagesPath;
   /// The directory that the category tree images will be downloaded to
   char* m_catTreeImagesPath;
};


#endif /* NAV2SESSION_H */
