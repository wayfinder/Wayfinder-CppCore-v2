/*
Copyright (c) 1999 - 2010, Vodafone Group Services Ltd
All rights reserved.

Redistribution and use in source and binary forms, with or without modification, are permitted provided that the following conditions are met:

    * Redistributions of source code must retain the above copyright notice, this list of conditions and the following disclaimer.
    * Redistributions in binary form must reproduce the above copyright notice, this list of conditions and the following disclaimer in the documentation and/or other materials provided with the distribution.
    * Neither the name of the Vodafone Group Services Ltd nor the names of its contributors may be used to endorse or promote products derived from this software without specific prior written permission.

THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS" AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE ARE DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT HOLDER OR CONTRIBUTORS BE LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
*/


#include "NMEALocationInterfaceImpl.h"
#include "LocationStatusCode.h"
#include "Nav2APIChannel.h"
#include "GuiProtMess.h"
#include "LocationListener.h"
#include "NMEALocationThread.h"

using namespace WFAPI;

NMEALocationInterfaceImpl::NMEALocationInterfaceImpl(Nav2APIImpl* api)
   : LocationInterfaceImpl(api),
     m_nmeaFile("/tmp/blah.txt"),
     m_locationThread(NULL)

{
   m_locationThread = NULL;
}


NMEALocationInterfaceImpl::~NMEALocationInterfaceImpl()
{
   killThread();
}

SynchronousStatus NMEALocationInterfaceImpl::lbsStarted()
{
   wf_int statusCode = LBS_STATUS_STARTED;

   if (!m_locationThread) {
      statusCode = LBS_STATUS_NOT_STARTED;
   }
   
   return SynchronousStatus(statusCode, "", "");
}

AsynchronousStatus NMEALocationInterfaceImpl::startLbs()
{
   m_locationThread = new NMEALocationThread(getBtChannel());
   if(m_locationThread->start()) {
      isab::GenericGuiMess message(isab::GuiProtEnums::CONNECT_GPS);
      sendAsynchronousRequestToNav2(&message);
      
      return AsynchronousStatus(RequestID(RequestID::INVALID_REQUEST_ID),
                                OK, "", "");      
   } else {
      killThread();
      
      return AsynchronousStatus(RequestID(RequestID::INVALID_REQUEST_ID),
                                GENERAL_ERROR, "", "");      
   }
}


void NMEALocationInterfaceImpl::killThread()
{
   if(m_locationThread) {
      m_locationThread->terminate();
      m_locationThread->join();
      delete m_locationThread;
      m_locationThread = NULL;      
   }
}

SynchronousStatus NMEALocationInterfaceImpl::stopLbs()
{
   if (m_locationThread) {
      killThread();
   }

   isab::GenericGuiMess message(isab::GuiProtEnums::DISCONNECT_GPS);
   sendAsynchronousRequestToNav2(&message);
   
   return SynchronousStatus(OK, "", "");
}

void NMEALocationInterfaceImpl::replayNMEAData(NMEAVec& vec)
{
   if(m_locationThread) {
      m_locationThread->updateNMEAData(vec);
   }
}

