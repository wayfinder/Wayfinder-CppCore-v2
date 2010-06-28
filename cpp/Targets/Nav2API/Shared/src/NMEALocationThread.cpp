/*
Copyright (c) 1999 - 2010, Vodafone Group Services Ltd
All rights reserved.

Redistribution and use in source and binary forms, with or without modification, are permitted provided that the following conditions are met:

    * Redistributions of source code must retain the above copyright notice, this list of conditions and the following disclaimer.
    * Redistributions in binary form must reproduce the above copyright notice, this list of conditions and the following disclaimer in the documentation and/or other materials provided with the distribution.
    * Neither the name of the Vodafone Group Services Ltd nor the names of its contributors may be used to endorse or promote products derived from this software without specific prior written permission.

THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS" AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE ARE DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT HOLDER OR CONTRIBUTORS BE LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
*/


#include "NMEALocationThread.h"
#include "Nav2APIChannel.h"

#define CORE_LOGGING_MODULE_NAME "NMEALocationThread"
//#define CORE_LOGGING_MODULE_LEVEL CL_ALL_FILE

#include "CoreDebugPrint.h"

using namespace WFAPI;

void NMEALocationThread::run()
{
   m_terminated = false;

   while(!m_terminated) {
      m_mutex->lock();

      if(!m_msgs.empty()) {
         std::string curMsg = m_msgs.back();
         m_msgs.pop_back();

         m_channel->getChannel()->writeData(
            reinterpret_cast<const uint8*>(curMsg.c_str()),
            curMsg.size());
         
         coreprintln("Supplying message, %d left", m_msgs.size());         
      }
      
      m_mutex->unlock();
      isab::Thread::sleep(1000, 0);
   } 
}

void WFAPI::NMEALocationThread::terminate()
{
   m_terminated = true; 
}

WFAPI::NMEALocationThread::NMEALocationThread(Nav2APIChannel* channel)    :
   isab::Thread("NMEALocationThread"),
   m_terminated(true),
   m_mutex(new isab::Mutex),
   m_channel(channel)
{
   
}

void WFAPI::NMEALocationThread::updateNMEAData(const NMEAVec& newMsgs)
{
   coreprintln("New NMEA data available, %d messages", newMsgs.size());
   
   m_mutex->lock();
   m_msgs = newMsgs;
   std::reverse(m_msgs.begin(), m_msgs.end());
   m_mutex->unlock();
}

