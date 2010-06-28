/*
  Copyright (c) 1999 - 2010, Vodafone Group Services Ltd
  All rights reserved.

  Redistribution and use in source and binary forms, with or without modification, are permitted provided that the following conditions are met:

  * Redistributions of source code must retain the above copyright notice, this list of conditions and the following disclaimer.
  * Redistributions in binary form must reproduce the above copyright notice, this list of conditions and the following disclaimer in the documentation and/or other materials provided with the distribution.
  * Neither the name of the Vodafone Group Services Ltd nor the names of its contributors may be used to endorse or promote products derived from this software without specific prior written permission.

  THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS" AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE ARE DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT HOLDER OR CONTRIBUTORS BE LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
*/

#include "SettingsInterface.h"
#include "SettingsInterfaceImpl.h"
#include "CoreFunctionPrologue.h"

namespace WFAPI {

struct SettingsInterface::impl {
   impl(Nav2APIImpl* nav2APIImpl) : m_impl(nav2APIImpl) {}

   SettingsInterfaceImpl m_impl;
};


SettingsInterface::SettingsInterface(Nav2APIImpl* nav2APIImpl)
{
   m_impl = new impl(nav2APIImpl);
}

SettingsInterface::~SettingsInterface()
{
   delete m_impl;
}

SynchronousStatus
SettingsInterface::getAutoReroute(bool& autoReroute)
{
   CORE_FUNCTION_PROLOGUE();

   // paramAutoReroute
   return m_impl->m_impl.getAutoReroute(autoReroute);
}

SynchronousStatus
SettingsInterface::getAvoidHighway(bool& avoidHighway)
{
   CORE_FUNCTION_PROLOGUE();

   // paramHighways
   return m_impl->m_impl.getAvoidHighway(avoidHighway);
}

SynchronousStatus
SettingsInterface::getAvoidTollRoad(bool& avoidTollRoad)
{
   CORE_FUNCTION_PROLOGUE();

   // paramTollRoads
   return m_impl->m_impl.getAvoidTollRoad(avoidTollRoad);
}

SynchronousStatus
SettingsInterface::getCheckRouteForTrafficChanges(
   bool& checkRouteForTrafficChanges)
{
   CORE_FUNCTION_PROLOGUE();

   return m_impl->m_impl.getCheckRouteForTrafficChanges(
      checkRouteForTrafficChanges);
}

SynchronousStatus
SettingsInterface::getDistanceUnit(DistanceUnit& distanceUnit)
{
   CORE_FUNCTION_PROLOGUE();

   return m_impl->m_impl.getDistanceUnit(distanceUnit);
}

SynchronousStatus
SettingsInterface::getRouteCost(RouteCost& routeCost)
{
   CORE_FUNCTION_PROLOGUE();

   return m_impl->m_impl.getRouteCost(routeCost);
}

SynchronousStatus
SettingsInterface::getTrafficInformationUpdateTime(
   wf_uint32& trafficInformationUpdateTime)
{
   CORE_FUNCTION_PROLOGUE();

   return m_impl->m_impl.getTrafficInformationUpdateTime(
      trafficInformationUpdateTime);
}

SynchronousStatus
SettingsInterface::getVoiceVerbosity(VoiceVerbosity& voiceVerbosity)
{
   CORE_FUNCTION_PROLOGUE();

   return m_impl->m_impl.getVoiceVerbosity(voiceVerbosity);
}

SynchronousStatus
SettingsInterface::getCurrentTransportationType(
   TransportationType& transportationType)
{
   CORE_FUNCTION_PROLOGUE();

   return m_impl->m_impl.getCurrentTransportationType(transportationType);
}

SynchronousStatus
SettingsInterface::setAutoReroute(const bool& newVal)
{
   CORE_FUNCTION_PROLOGUE();

   return m_impl->m_impl.setAutoReroute(newVal);
}

SynchronousStatus
SettingsInterface::setAvoidHighway(const bool& newVal)
{
   CORE_FUNCTION_PROLOGUE();

   return m_impl->m_impl.setAvoidHighway(newVal);
}

SynchronousStatus
SettingsInterface::setAvoidTollRoad(const bool& newVal)
{
   CORE_FUNCTION_PROLOGUE();

   return m_impl->m_impl.setAvoidTollRoad(newVal);
}

SynchronousStatus
SettingsInterface::setCheckRouteForTrafficChanges(const bool& newVal)
{
   CORE_FUNCTION_PROLOGUE();

   return m_impl->m_impl.setCheckRouteForTrafficChanges(newVal);
}

SynchronousStatus
SettingsInterface::setDistanceUnit(DistanceUnit newVal)
{
   CORE_FUNCTION_PROLOGUE();

   return m_impl->m_impl.setDistanceUnit(newVal);
}

SynchronousStatus
SettingsInterface::setRouteCost(RouteCost newVal)
{
   CORE_FUNCTION_PROLOGUE();

   return m_impl->m_impl.setRouteCost(newVal);
}

SynchronousStatus
SettingsInterface::setTrafficInformationUpdateTime(wf_uint32 newVal)
{
   CORE_FUNCTION_PROLOGUE();

   // userTrafficUpdatePeriod 
   return m_impl->m_impl.setTrafficInformationUpdateTime(newVal);
}

SynchronousStatus
SettingsInterface::setVoiceVerbosity(VoiceVerbosity newVal)
{
   CORE_FUNCTION_PROLOGUE();

   return m_impl->m_impl.setVoiceVerbosity(newVal);
}

SynchronousStatus
SettingsInterface::setUsePositionForSearchCategoriesRequests(bool newVal)
{
   CORE_FUNCTION_PROLOGUE();

   return m_impl->m_impl.setUsePositionForSearchCategoriesRequests(newVal);
}

SynchronousStatus
SettingsInterface::getUsePositionForSearchCategoriesRequests(bool &usePos)
{
   CORE_FUNCTION_PROLOGUE();

   return m_impl->m_impl.getUsePositionForSearchCategoriesRequests(usePos);    
}

void
SettingsInterface::setMaxDiskCacheSize(wf_uint32 nbrBytes)
{
   CORE_FUNCTION_PROLOGUE();

   m_impl->m_impl.setMaxDiskCacheSize(nbrBytes);
}

bool
SettingsInterface::setCategoryVisible(wf_uint32 id, bool visible)
{
   CORE_FUNCTION_PROLOGUE();

   return m_impl->m_impl.setCategoryVisible(id, visible);
}

bool
SettingsInterface::synchronizePOICategories(POICategories& localCategories)
{
   CORE_FUNCTION_PROLOGUE();

   return m_impl->m_impl.synchronizePOICategories(localCategories);
   // paramPoiCategories
}

AsynchronousStatus
SettingsInterface::isEULAUpdated(const WFString& filename)
{
   CORE_FUNCTION_PROLOGUE();

   return m_impl->m_impl.isEULAUpdated(filename);
}

SettingsInterfaceImpl*
SettingsInterface::getImpl()
{
   CORE_FUNCTION_PROLOGUE();

   return &m_impl->m_impl;
}

void
SettingsInterface::addSettingsListener(SettingsListener* listener)
{
   CORE_FUNCTION_PROLOGUE();

   m_impl->m_impl.addSettingsListener(listener);
}

void
SettingsInterface::removeSettingsListener(SettingsListener* listener)
{
   CORE_FUNCTION_PROLOGUE();

   m_impl->m_impl.removeSettingsListener(listener);
}

SynchronousStatus
SettingsInterface::getUsername(WFString& username)
{
   CORE_FUNCTION_PROLOGUE();

   return m_impl->m_impl.getUsername(username);
}

SynchronousStatus
SettingsInterface::setPassword(const WFString& password)
{
   CORE_FUNCTION_PROLOGUE();

   return m_impl->m_impl.setPassword(password);
}

SynchronousStatus
SettingsInterface::setUIN(const WFString& UIN)
{
   CORE_FUNCTION_PROLOGUE();

   return m_impl->m_impl.setUIN(UIN);
}

SynchronousStatus
SettingsInterface::getNewVersionInformation(WFString& version,
                                            WFString& url,
                                            WFString& versionString,
                                            bool& force)
{
   CORE_FUNCTION_PROLOGUE();

   return m_impl->m_impl.getNewVersionInformation(version, url,
                                                  versionString, force);
}

} // End namespace WFAPI



