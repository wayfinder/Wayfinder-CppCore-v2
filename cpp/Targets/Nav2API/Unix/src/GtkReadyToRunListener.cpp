/*
Copyright (c) 1999 - 2010, Vodafone Group Services Ltd
All rights reserved.

Redistribution and use in source and binary forms, with or without modification, are permitted provided that the following conditions are met:

    * Redistributions of source code must retain the above copyright notice, this list of conditions and the following disclaimer.
    * Redistributions in binary form must reproduce the above copyright notice, this list of conditions and the following disclaimer in the documentation and/or other materials provided with the distribution.
    * Neither the name of the Vodafone Group Services Ltd nor the names of its contributors may be used to endorse or promote products derived from this software without specific prior written permission.

THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS" AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE ARE DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT HOLDER OR CONTRIBUTORS BE LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
*/


#include "GtkReadyToRunListener.h"

#define CORE_LOGGING_MODULE_NAME "GtkReadyToRunListener"
//#define CORE_LOGGING_MODULE_LEVEL CL_ALL_FILE

#include "CoreDebugPrint.h"

bool GtkReadyToRunListener::s_runtimeDisabled = false;

void GtkReadyToRunListener::ready()
{
   // With this we don't need the BFL, and that is good as it
   // is locked while gtk runs and then we might look here.
   g_idle_add_full(G_PRIORITY_DEFAULT, gtkTimeoutCallback, this,
                   gtkNotifyCallback); 
}

void GtkReadyToRunListener::internalDoCallbacks()
{
   // Unset timeout id by setting it to 0
   m_timerID = 0;
   // Let Nav2API to it's callbacks using the gtk main loop that called
   // this function so that there is only one execution point.
   doCallbacks(); 
}

gboolean GtkReadyToRunListener::gtkTimeoutCallback(void* callback)
{
   if(!s_runtimeDisabled) {
      // Call the GtkReadyToRunListener object's internalDoCallbacks
      // function from this static function.
      static_cast<GtkReadyToRunListener*>(callback)->internalDoCallbacks();      
   }

   return false; 
}

void GtkReadyToRunListener::gtkNotifyCallback(void* callback)
{
   // This method is called when the idle source has been destroyed,
   // no need to call the internalDoCallbacks again.

   // // Call the GtkReadyToRunListener object's internalDoCallbacks
   // // function from this static function.
   // static_cast<GtkReadyToRunListener*>(callback)->internalDoCallbacks(); 
}

GtkReadyToRunListener::GtkReadyToRunListener()
   : ReadyToRunListener(), m_timerID(0) 
{
    
}

GtkReadyToRunListener::~GtkReadyToRunListener()
{
    
}

void GtkReadyToRunListener::disableRuntime()
{
   coreprintln_info("Disabling GUI-thread callbacks for GTK");
   s_runtimeDisabled = true; 
}

