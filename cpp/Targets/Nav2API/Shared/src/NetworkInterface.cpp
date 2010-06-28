/*
  Copyright (c) 1999 - 2010, Vodafone Group Services Ltd
  All rights reserved.

  Redistribution and use in source and binary forms, with or without modification, are permitted provided that the following conditions are met:

  * Redistributions of source code must retain the above copyright notice, this list of conditions and the following disclaimer.
  * Redistributions in binary form must reproduce the above copyright notice, this list of conditions and the following disclaimer in the documentation and/or other materials provided with the distribution.
  * Neither the name of the Vodafone Group Services Ltd nor the names of its contributors may be used to endorse or promote products derived from this software without specific prior written permission.

  THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS" AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE ARE DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT HOLDER OR CONTRIBUTORS BE LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
*/

#define CORE_LOGGING_MODULE_NAME "NetworkInterface"
//#define CORE_FUNCTION_PROLOGUE_ENABLE_TRACING

#include "NetworkInterface.h"
#include "NetworkInterfaceImpl.h"
#include "CoreFunctionPrologue.h"

namespace WFAPI {

struct NetworkInterface::impl {
   impl(Nav2APIImpl* nav2APIImpl) : m_impl(nav2APIImpl) {}
      
   NetworkInterfaceImpl m_impl;
};


NetworkInterface::NetworkInterface(Nav2APIImpl* nav2APIImpl)
{
   m_impl = new impl(nav2APIImpl);
}

NetworkInterface::~NetworkInterface()
{
   delete m_impl;
}

void
NetworkInterface::addNetworkListener(NetworkListener* listener)
{
   CORE_FUNCTION_PROLOGUE();

   m_impl->m_impl.addNetworkListener(listener);
}

SynchronousStatus
NetworkInterface::clearNetworkLinkID()
{
   CORE_FUNCTION_PROLOGUE();

   return m_impl->m_impl.clearNetworkLinkID();
}

SynchronousStatus
NetworkInterface::getKeepLinkUp(bool& val)
{
   CORE_FUNCTION_PROLOGUE();

   return m_impl->m_impl.getKeepLinkUp(val);
}

SynchronousStatus
NetworkInterface::getNetworkLinkID(NetworkLinkID& networkLinkID)
{
   CORE_FUNCTION_PROLOGUE();

   return m_impl->m_impl.getNetworkLinkID(networkLinkID);
}

void
NetworkInterface::removeNetworkListener(NetworkListener* listener)
{
   CORE_FUNCTION_PROLOGUE();

   m_impl->m_impl.removeNetworkListener(listener);
}

SynchronousStatus
NetworkInterface::setKeepLinkUp(bool newVal) 
{
   CORE_FUNCTION_PROLOGUE();

   return m_impl->m_impl.setKeepLinkUp(newVal);
}

SynchronousStatus
NetworkInterface::setNetworkLinkID(const NetworkLinkID& networkLinkID)
{
   CORE_FUNCTION_PROLOGUE();

   return m_impl->m_impl.setNetworkLinkID(networkLinkID);
}

AsynchronousStatus
NetworkInterface::testServerConnection()
{
   CORE_FUNCTION_PROLOGUE();

   return m_impl->m_impl.testServerConnection();
}

NetworkInterfaceImpl*
NetworkInterface::getImpl()
{
   return &m_impl->m_impl;
}


} // End namespace WFAPI
