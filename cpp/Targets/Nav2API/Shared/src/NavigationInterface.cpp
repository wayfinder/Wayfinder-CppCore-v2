/*
  Copyright (c) 1999 - 2010, Vodafone Group Services Ltd
  All rights reserved.

  Redistribution and use in source and binary forms, with or without modification, are permitted provided that the following conditions are met:

  * Redistributions of source code must retain the above copyright notice, this list of conditions and the following disclaimer.
  * Redistributions in binary form must reproduce the above copyright notice, this list of conditions and the following disclaimer in the documentation and/or other materials provided with the distribution.
  * Neither the name of the Vodafone Group Services Ltd nor the names of its contributors may be used to endorse or promote products derived from this software without specific prior written permission.

  THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS" AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE ARE DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT HOLDER OR CONTRIBUTORS BE LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
*/

#include "NavigationInterface.h"
#include "NavigationInterfaceImpl.h"
#include "CoreFunctionPrologue.h"

namespace WFAPI {

struct NavigationInterface::impl {
   impl(Nav2APIImpl* nav2APIImpl) : m_impl(nav2APIImpl) {}
      
   NavigationInterfaceImpl m_impl;
};


NavigationInterface::NavigationInterface(Nav2APIImpl* nav2APIImpl)
{
   m_impl = new impl(nav2APIImpl);
}

NavigationInterface::~NavigationInterface()
{
   delete m_impl;
}

void
NavigationInterface::addNavigationInfoUpdateListener(
   NavigationInfoUpdateListener* listener)
{
   CORE_FUNCTION_PROLOGUE();

   m_impl->m_impl.addNavigationInfoUpateListener(listener);
}

void
NavigationInterface::removeNavigationInfoUpdateListener(
   NavigationInfoUpdateListener* listener)
{
   CORE_FUNCTION_PROLOGUE();

   m_impl->m_impl.removeNavigationInfoUpateListener(listener);
}

SynchronousStatus
NavigationInterface::soundPrepared(wf_uint32 duration)
{
   CORE_FUNCTION_PROLOGUE();

   return m_impl->m_impl.soundPrepared(duration);
}

SynchronousStatus
NavigationInterface::soundPlayed()
{
   CORE_FUNCTION_PROLOGUE();

   return m_impl->m_impl.soundPlayed();
}

NavigationInterfaceImpl*
NavigationInterface::getImpl()
{
   return &m_impl->m_impl;
}

} // End namespace WFAPI
