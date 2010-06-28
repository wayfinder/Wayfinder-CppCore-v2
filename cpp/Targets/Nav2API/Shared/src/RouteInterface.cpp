/*
  Copyright (c) 1999 - 2010, Vodafone Group Services Ltd
  All rights reserved.

  Redistribution and use in source and binary forms, with or without modification, are permitted provided that the following conditions are met:

  * Redistributions of source code must retain the above copyright notice, this list of conditions and the following disclaimer.
  * Redistributions in binary form must reproduce the above copyright notice, this list of conditions and the following disclaimer in the documentation and/or other materials provided with the distribution.
  * Neither the name of the Vodafone Group Services Ltd nor the names of its contributors may be used to endorse or promote products derived from this software without specific prior written permission.

  THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS" AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE ARE DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT HOLDER OR CONTRIBUTORS BE LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
*/

#include "RouteInterface.h"
#include "RouteInterfaceImpl.h"
#include "CoreFunctionPrologue.h"

namespace WFAPI {

struct RouteInterface::impl {
   impl(Nav2APIImpl* nav2APIImpl) : m_impl(nav2APIImpl) {}
      
   RouteInterfaceImpl m_impl;
};

RouteInterface::RouteInterface(Nav2APIImpl* nav2APIImpl)
{
   m_impl = new impl(nav2APIImpl);
}

RouteInterface::~RouteInterface()
{
   delete m_impl;
}

void 
RouteInterface::addRouteListener(RouteListener* listener)
{
   CORE_FUNCTION_PROLOGUE();

   m_impl->m_impl.addRouteListener(listener);
}

void 
RouteInterface::removeRouteListener(RouteListener* listener)
{
   CORE_FUNCTION_PROLOGUE();

   m_impl->m_impl.removeRouteListener(listener);
}

SynchronousStatus 
RouteInterface::getRouteList(RouteInfoItemArray& routeInfoList)
{
   CORE_FUNCTION_PROLOGUE();

   return m_impl->m_impl.getRouteList(routeInfoList);
}

SynchronousStatus 
RouteInterface::removeRoute()
{
   CORE_FUNCTION_PROLOGUE();

   return m_impl->m_impl.removeRoute();
}

AsynchronousStatus
RouteInterface::reRoute()
{
   CORE_FUNCTION_PROLOGUE();

   return m_impl->m_impl.reRoute();
}

AsynchronousStatus
RouteInterface::routeBetweenCoordinates(const WGS84Coordinate& startCoord,
                                        const WGS84Coordinate& destCoord,
                                        TransportationType transportationType)
{
   CORE_FUNCTION_PROLOGUE();

   return m_impl->m_impl.routeBetweenCoordinates(startCoord,
                                                 destCoord,
                                                 transportationType);
}

AsynchronousStatus
RouteInterface::routeToCoordinate(const WGS84Coordinate& destCoord, 
                                  TransportationType transportationType)
{
   CORE_FUNCTION_PROLOGUE();

   return m_impl->m_impl.routeToCoordinate(destCoord,
                                           transportationType);
}

AsynchronousStatus
RouteInterface::routeToFavourite(const Favourite& favourite, 
                                 TransportationType transportationType)
{
   CORE_FUNCTION_PROLOGUE();

   return m_impl->m_impl.routeToFavourite(favourite,
                                          transportationType);
}

AsynchronousStatus
RouteInterface::routeToSearchItem(const SearchItem& searchItem, 
                                  TransportationType transportationType)
{
   CORE_FUNCTION_PROLOGUE();

   return m_impl->m_impl.routeToSearchItem(searchItem,
                                           transportationType);
}

SynchronousStatus 
RouteInterface::routeSimulationStart()
{
   CORE_FUNCTION_PROLOGUE();

   return m_impl->m_impl.routeSimulationStart();
}

SynchronousStatus 
RouteInterface::routeSimulationStop()
{
   CORE_FUNCTION_PROLOGUE();

   return m_impl->m_impl.routeSimulationStop();
}

SynchronousStatus
RouteInterface::routeSimulationRepeatOn()
{
   CORE_FUNCTION_PROLOGUE();

   return m_impl->m_impl.routeSimulationRepeatOn();
}

SynchronousStatus
RouteInterface::routeSimulationRepeatOff()
{
   CORE_FUNCTION_PROLOGUE();

   return m_impl->m_impl.routeSimulationRepeatOff();
}

SynchronousStatus 
RouteInterface::routeSimulationIncreaseSpeed()
{
   CORE_FUNCTION_PROLOGUE();

   return m_impl->m_impl.routeSimulationIncreaseSpeed();
}

SynchronousStatus 
RouteInterface::routeSimulationDecreaseSpeed()
{
   CORE_FUNCTION_PROLOGUE();

   return m_impl->m_impl.routeSimulationDecreaseSpeed();
}

SynchronousStatus 
RouteInterface::routeSimulationPause()
{
   CORE_FUNCTION_PROLOGUE();

   return m_impl->m_impl.routeSimulationPause();
}

SynchronousStatus 
RouteInterface::routeSimulationResume()
{
   CORE_FUNCTION_PROLOGUE();

   return m_impl->m_impl.routeSimulationResume();
}


RouteInterfaceImpl* 
RouteInterface::getImpl()
{
   return &m_impl->m_impl;
}

} // End namespace WFAPI


