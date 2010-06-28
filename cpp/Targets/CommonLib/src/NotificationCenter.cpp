/*
Copyright (c) 1999 - 2010, Vodafone Group Services Ltd
All rights reserved.

Redistribution and use in source and binary forms, with or without modification, are permitted provided that the following conditions are met:

    * Redistributions of source code must retain the above copyright notice, this list of conditions and the following disclaimer.
    * Redistributions in binary form must reproduce the above copyright notice, this list of conditions and the following disclaimer in the documentation and/or other materials provided with the distribution.
    * Neither the name of the Vodafone Group Services Ltd nor the names of its contributors may be used to endorse or promote products derived from this software without specific prior written permission.

THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS" AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE ARE DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT HOLDER OR CONTRIBUTORS BE LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
*/


#define CORE_LOGGING_MODULE_NAME "NotificationCenter"
// #define CORE_LOGGING_MODULE_LEVEL CL_ALL_FILE

#include "NotificationCenter.h"
#include "CoreDebugPrint.h"
#include "Synchronized.h"

NotificationCenter::NotificationCenter()
{
   m_mutex = new isab::Mutex;
}

NotificationCenter::~NotificationCenter()
{
   delete m_mutex;
}


void NotificationCenter::postNotification(const std::string& source,
                                          const std::string& info)
{
   isab::Synchronized lock(*m_mutex);
   std::pair<ListenerMap::iterator,ListenerMap::iterator> iters;
   iters = m_listeners.equal_range(source);

   while (iters.first != iters.second) {
      iters.first->second->notificationReceived(source, info);
      iters.first++;
   }

   coreprintln("%s: %s", source.c_str(), info.c_str());
}



void NotificationCenter::addListener(const std::string& target,
                                     NotificationListener* listener)
{
   isab::Synchronized lock(*m_mutex);
   std::pair<ListenerMap::iterator,ListenerMap::iterator> iters;
   iters = m_listeners.equal_range(target);

   bool exists = false;

   while (iters.first != iters.second) {
      if (iters.first->second == listener){
         exists = true;
         break;
      }
      iters.first++;
   }
   
   if (!exists){
      m_listeners.insert(ListenerPair(target,listener));
   }
}

void NotificationCenter::removeListener(const std::string& target,
                                        NotificationListener* listener)
{
   isab::Synchronized lock(*m_mutex);
   std::pair<ListenerMap::iterator,ListenerMap::iterator> iters;
   iters = m_listeners.equal_range(target);

   bool exists = false;

   while (iters.first != iters.second) {
      if (iters.first->second == listener){
         exists = true;
         break;
      }
      iters.first++;
   }
   
   if (exists){
      m_listeners.erase(iters.first);
   }
    
}

