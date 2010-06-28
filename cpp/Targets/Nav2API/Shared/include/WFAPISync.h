/*
  Copyright (c) 1999 - 2010, Vodafone Group Services Ltd
  All rights reserved.

  Redistribution and use in source and binary forms, with or without modification, are permitted provided that the following conditions are met:

  * Redistributions of source code must retain the above copyright notice, this list of conditions and the following disclaimer.
  * Redistributions in binary form must reproduce the above copyright notice, this list of conditions and the following disclaimer in the documentation and/or other materials provided with the distribution.
  * Neither the name of the Vodafone Group Services Ltd nor the names of its contributors may be used to endorse or promote products derived from this software without specific prior written permission.

  THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS" AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE ARE DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT HOLDER OR CONTRIBUTORS BE LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
*/

#ifndef WFAPI_SYNC_H
#define WFAPI_SYNC_H

#include "WFAPIConfig.h"
#include "Monitor.h"
#include "Mutex.h"

namespace WFAPI {
   
/**
 * Class wrapping the monitor and mutex lock and unlock functions.
 * Used for making sure not forgetting to lock and unlock the monitor
 * or the mutex. The constructor locks the monitor or the mutex and the
 * destructor unlocks it.
 */
class WFAPISync {
public:

   /**
    * Constructor.
    *
    * @param mon A pointer to the monitor to be locked.
    */
   WFAPISync(isab::Monitor* mon) : 
         m_monitor(mon), m_mutex(NULL) { m_monitor->lock(); }

   /**
    * Constructor.
    *
    * @param mut A pointer to the mutex to be locked.
    */ 
   WFAPISync(isab::Mutex* mut) 
      : m_monitor(NULL), m_mutex(mut) { m_mutex->lock(); }

   /**
    * Destructor.
    * Unlocks the monitor or the mutex depending on which is set.
    */
   ~WFAPISync()
      {
      if (m_monitor != NULL) {
         m_monitor->unlock();
      } else {
         m_mutex->unlock();
         
      }
   }
   
private:

   /// A pointer to the monitor to be locked and unlocked.
   isab::Monitor* m_monitor;

   /// A pointer to the mutex to be locked and unlocked.
   isab::Mutex* m_mutex;
};

} // End namespace WFAPI

#endif // End WFAPI_SYNC_H

