/*
  Copyright (c) 1999 - 2010, Vodafone Group Services Ltd
  All rights reserved.

  Redistribution and use in source and binary forms, with or without modification, are permitted provided that the following conditions are met:

  * Redistributions of source code must retain the above copyright notice, this list of conditions and the following disclaimer.
  * Redistributions in binary form must reproduce the above copyright notice, this list of conditions and the following disclaimer in the documentation and/or other materials provided with the distribution.
  * Neither the name of the Vodafone Group Services Ltd nor the names of its contributors may be used to endorse or promote products derived from this software without specific prior written permission.

  THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS" AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE ARE DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT HOLDER OR CONTRIBUTORS BE LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
*/

#define CORE_LOGGING_MODULE_NAME "Nav2API"
//#define CORE_LOGGING_MODULE_LEVEL CL_ALL_FILE
//#define CORE_FUNCTION_PROLOGUE_ENABLE_TRACING

#include "CoreDebugPrint.h"
#include "Nav2API.h"
#include "Nav2APIImpl.h"
#include "Nav2APISharedImpl.h"
#include "MapRequesterInterfaceImpl.h"
#include "CoreFunctionPrologue.h"

namespace WFAPI {

struct Nav2API::Nav2APIImplStruct
{
   Nav2APIImplStruct(Nav2API* api)
         : m_impl(api)
   {}

   Nav2APIImpl m_impl;
};


Nav2API::Nav2API() {
   m_impl = new Nav2APIImplStruct(this);
   m_sharedImpl = new Nav2APISharedImpl;
}

Nav2API::~Nav2API() {
   delete m_impl;
   delete m_sharedImpl;
}

void
Nav2API::connectMapLib(MapLibAPI* mapLibAPI) {
   CORE_FUNCTION_PROLOGUE();

   m_impl->m_impl.connectMapLib(mapLibAPI);
}

FavouriteInterface&
Nav2API::getFavouriteInterface() {
   CORE_FUNCTION_PROLOGUE();

   return m_impl->m_impl.getFavouriteInterface();
}

ImageInterface&
Nav2API::getImageInterface() {
   CORE_FUNCTION_PROLOGUE();

   return m_impl->m_impl.getImageInterface();
}

LocationInterface&
Nav2API::getLocationInterface() {
   CORE_FUNCTION_PROLOGUE();

   return m_impl->m_impl.getLocationInterface();
}

NavigationInterface&
Nav2API::getNavigationInterface() {
   CORE_FUNCTION_PROLOGUE();

   return m_impl->m_impl.getNavigationInterface();
}

NetworkInterface&
Nav2API::getNetworkInterface() {
   CORE_FUNCTION_PROLOGUE();

   return m_impl->m_impl.getNetworkInterface();
}

RouteInterface&
Nav2API::getRouteInterface() {
   CORE_FUNCTION_PROLOGUE();

   return m_impl->m_impl.getRouteInterface();
}

SearchInterface&
Nav2API::getSearchInterface() {
   CORE_FUNCTION_PROLOGUE();

   return m_impl->m_impl.getSearchInterface();
}

SettingsInterface&
Nav2API::getSettingsInterface() {
   CORE_FUNCTION_PROLOGUE();

   return m_impl->m_impl.getSettingsInterface();
}

BillingInterface&
Nav2API::getBillingInterface() {
   CORE_FUNCTION_PROLOGUE();

   return m_impl->m_impl.getBillingInterface();
}

TunnelInterface&
Nav2API::getTunnelInterface() {
   CORE_FUNCTION_PROLOGUE();

   return m_impl->m_impl.getTunnelInterface();
}

DBufConnection* 
Nav2API::getDBufConnection()
{
   CORE_FUNCTION_PROLOGUE();

   return &(m_impl->m_impl.getMapRequesterInterfaceImpl());
}

void Nav2API::setStatusListener(Nav2StatusListener* statusListener)
{
   CORE_FUNCTION_PROLOGUE();

   if(m_impl) {
      m_impl->m_impl.addStatusListener(statusListener);      
   }
}



void
Nav2API::stop()
{
   CORE_FUNCTION_PROLOGUE();

   m_impl->m_impl.stop();
}

WFString
Nav2API::getVoiceLanguageAsString(VoiceLanguage::VoiceLanguage lang, 
                                  bool uppercase)
{
   CORE_FUNCTION_PROLOGUE();

   return m_impl->m_impl.getVoiceLanguageAsString(lang, uppercase);
}

void
Nav2API::setHttpProto(bool httpProto)
{
   CORE_FUNCTION_PROLOGUE();

   m_sharedImpl->m_httpProto = httpProto;   
}

Nav2APIImpl*
Nav2API::getImpl()
{
   return &m_impl->m_impl;
}

const WFString&
Nav2API::getClientType() const
{
   return m_sharedImpl->m_clientType;
}

void
Nav2API::setClientType(const WFString& clientType)
{
   CORE_FUNCTION_PROLOGUE();

   m_sharedImpl->m_clientType = clientType;
}

void
Nav2API::setClientTypeOptions(const WFString& clientTypeOptions)
{
   CORE_FUNCTION_PROLOGUE();

   m_sharedImpl->m_clientTypeOptions = clientTypeOptions;
}

void
Nav2API::setHttpRequestString(const WFString& httpRequestString)
{
   CORE_FUNCTION_PROLOGUE();

   m_sharedImpl->m_httpRequestString = httpRequestString;
}

void
Nav2API::setProgramVersion(const ProgramVersion& programVersion)
{
   CORE_FUNCTION_PROLOGUE();

   m_sharedImpl->m_programVersion = programVersion;
}

void
Nav2API::setAudioTypeDirName(const WFString& audioTypeDirName)
{
   CORE_FUNCTION_PROLOGUE();

   m_sharedImpl->m_audioTypeDirName = audioTypeDirName;
}

void Nav2API::addStatusListener(Nav2StatusListener* statusListener)
{
   CORE_FUNCTION_PROLOGUE();

   m_impl->m_impl.addStatusListener(statusListener);   
}

void Nav2API::removeStatusListener(Nav2StatusListener* statusListener)
{
   CORE_FUNCTION_PROLOGUE();

   m_impl->m_impl.removeStatusListener(statusListener);
}


} // End namespace WFAPI





