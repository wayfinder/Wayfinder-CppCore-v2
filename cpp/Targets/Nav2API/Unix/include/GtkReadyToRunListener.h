/*
Copyright (c) 1999 - 2010, Vodafone Group Services Ltd
All rights reserved.

Redistribution and use in source and binary forms, with or without modification, are permitted provided that the following conditions are met:

    * Redistributions of source code must retain the above copyright notice, this list of conditions and the following disclaimer.
    * Redistributions in binary form must reproduce the above copyright notice, this list of conditions and the following disclaimer in the documentation and/or other materials provided with the distribution.
    * Neither the name of the Vodafone Group Services Ltd nor the names of its contributors may be used to endorse or promote products derived from this software without specific prior written permission.

THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS" AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE ARE DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT HOLDER OR CONTRIBUTORS BE LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
*/


#ifndef _GTKREADYTORUNLISTENER_H_
#define _GTKREADYTORUNLISTENER_H_

#include <gtk/gtk.h>
#include "ReadyToRunListener.h"
#include "Nav2API.h"

/**
 * Class for synchronising callbacks from Nav2API using Gtk's main loop.
 * Uses a timer that is set when Nav2API calls the ready function. Then
 * the gtk main loop will call gtkTimeoutCallback that in turn calls 
 * doCallbacks that makes the callbacks that Nav2API wants to do to
 * the listeners.
 */
class GtkReadyToRunListener : public WFAPI::ReadyToRunListener {
public:
   
   GtkReadyToRunListener();

   ~GtkReadyToRunListener();

   virtual void ready();

   void internalDoCallbacks();

   static gboolean gtkTimeoutCallback(void* callback);

   static void gtkNotifyCallback(void* callback);

   static void disableRuntime();
   
private:

   /// The currently set timeout's id, 0 if none set.
   guint m_timerID;

   /// If this is set, no messages should be forwarded to the main thread
   static bool s_runtimeDisabled;
};



#endif /* _GTKREADYTORUNLISTENER_H_ */
