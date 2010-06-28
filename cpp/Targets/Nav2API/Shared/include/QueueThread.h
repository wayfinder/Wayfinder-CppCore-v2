/*
Copyright (c) 1999 - 2010, Vodafone Group Services Ltd
All rights reserved.

Redistribution and use in source and binary forms, with or without modification, are permitted provided that the following conditions are met:

    * Redistributions of source code must retain the above copyright notice, this list of conditions and the following disclaimer.
    * Redistributions in binary form must reproduce the above copyright notice, this list of conditions and the following disclaimer in the documentation and/or other materials provided with the distribution.
    * Neither the name of the Vodafone Group Services Ltd nor the names of its contributors may be used to endorse or promote products derived from this software without specific prior written permission.

THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS" AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE ARE DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT HOLDER OR CONTRIBUTORS BE LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
*/


#ifndef _QUEUETHREAD_H_
#define _QUEUETHREAD_H_

#include "WFAPIConfig.h"
#include "Thread.h"
#include "Monitor.h"
#include "Mutex.h"
#include "GuiProtMess.h"
#include "GuiProtEnums.h"
#include "GuiParameterEnums.h"
#include "GuiProtRouteMess.h"
#include "Nav2Logging.h"
#include "MapLibStartupListener.h"
#include "Nav2APIChannel.h"
#include "TextLanguage.h"
#include <vector>

namespace WFAPI {

class Nav2APIImpl;

/**
 * The thread that does the reading of messages from Nav2.
 * Also waits for the startup of Nav2.
 */
class QueueThread : public isab::Thread
                  , public MapLibStartupListener
{
public:
   
   /**
    * Constructor.
    */
   QueueThread(isab::Nav2::Channel* guiChannel, Nav2APIImpl* nav2API,
               std::vector<isab::GuiProtMess*>& initialMsgs,
               TextLanguage textLanguage);

   virtual ~QueueThread();
   
   /**
    * Called at startup end to set the stored nav2 parameters in MapLib.
    */
   void setMapLibParameters(
      std::vector<isab::GeneralParameterMess*>& mapLibParams);

   /**
    * Do the startup.
    *
    * @return True if to continue running or not.
    */
   bool doStartup();

   virtual void run();
      
   virtual void terminate();

   virtual void handleMapLibStartupComplete(
      MapLibStartupListener::statusCode code);

   /**
    * The startup text language.
    *
    * @return The language set when creating this QueueThread.
    */
   TextLanguage getTextLanguage() const;
private:
   /// The communication channel to and from Nav2.
   isab::Nav2::Channel* m_guiChannel;

   /// The API that this QueueThread belong to.
   Nav2APIImpl* m_nav2API;

   /// If in startup of Nav2.
   bool m_inStartup;

   /// The monitor to wait for on for Nav2 reads.
   isab::Monitor m_guiMonitor;

   /// The initial set sound msg to send.
   std::vector<isab::GuiProtMess*> m_initialMsgs;

   /// If shutting down.
   bool m_shutDown;

   /// The status of the MapLib startup
   MapLibStartupListener::statusCode m_mapLibStartupCode;

   /// The messages awaiting to be set to maplib, once started.
   std::vector<isab::GeneralParameterMess*> m_mapLibParams;

   /// The set text language.
   TextLanguage m_textLanguage;
};

}

#endif /* _QUEUETHREAD_H_ */
