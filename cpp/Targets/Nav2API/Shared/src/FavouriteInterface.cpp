/*
  Copyright (c) 1999 - 2010, Vodafone Group Services Ltd
  All rights reserved.

  Redistribution and use in source and binary forms, with or without modification, are permitted provided that the following conditions are met:

  * Redistributions of source code must retain the above copyright notice, this list of conditions and the following disclaimer.
  * Redistributions in binary form must reproduce the above copyright notice, this list of conditions and the following disclaimer in the documentation and/or other materials provided with the distribution.
  * Neither the name of the Vodafone Group Services Ltd nor the names of its contributors may be used to endorse or promote products derived from this software without specific prior written permission.

  THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS" AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE ARE DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT HOLDER OR CONTRIBUTORS BE LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
*/


#include "FavouriteInterface.h"
#include "FavouriteInterfaceImpl.h"
#include "CoreFunctionPrologue.h"

namespace WFAPI {

struct FavouriteInterface::impl {
   impl(Nav2APIImpl* nav2APIImpl) : m_impl(nav2APIImpl) {}

   FavouriteInterfaceImpl m_impl;
};

FavouriteInterface::FavouriteInterface(Nav2APIImpl* nav2APIImpl)
{
   m_impl = new impl(nav2APIImpl);
}

FavouriteInterface::~FavouriteInterface()
{
   delete m_impl;
}

SynchronousStatus
FavouriteInterface::addFavourite(const Favourite& favourite)
{
   CORE_FUNCTION_PROLOGUE();
   return m_impl->m_impl.addFavourite(favourite);
}

SynchronousStatus
FavouriteInterface::addCurrentPositionAsFavourite(const WFString& name,
                                                  const WFString& description,
                                                  ItemInfoArray& info)
{
   CORE_FUNCTION_PROLOGUE();
   return m_impl->m_impl.addCurrentPositionAsFavourite(name, description, info);
}

void
FavouriteInterface::addFavouriteListener(FavouriteListener* listener)
{
   CORE_FUNCTION_PROLOGUE();
   m_impl->m_impl.addFavouriteListener(listener);
}

SynchronousStatus
FavouriteInterface::deleteFavourite(wf_uint32 id)
{
   CORE_FUNCTION_PROLOGUE();
   return m_impl->m_impl.deleteFavourite(id);
}

SynchronousStatus
FavouriteInterface::getFavourites(wf_uint32 startIndex, 
                                  wf_uint32 count,
                                  wf_uint32& totalFavouriteCount,
                                  FavouriteArray& favouriteArray)
{
   CORE_FUNCTION_PROLOGUE();
   return m_impl->m_impl.getFavourites(startIndex, count,
                                totalFavouriteCount, favouriteArray);
}

void
FavouriteInterface::removeFavouriteListener(FavouriteListener* listener)
{
   CORE_FUNCTION_PROLOGUE();
   m_impl->m_impl.removeFavouriteListener(listener);
}

AsynchronousStatus
FavouriteInterface::syncFavourites()
{
   CORE_FUNCTION_PROLOGUE();
   return m_impl->m_impl.syncFavourites();
}

FavouriteInterfaceImpl* 
FavouriteInterface::getImpl()
{
   CORE_FUNCTION_PROLOGUE();
   return &m_impl->m_impl;
}

} // End namespace WFAPI
