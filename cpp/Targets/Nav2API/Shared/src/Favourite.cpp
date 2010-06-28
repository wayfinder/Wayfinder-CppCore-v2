/*
  Copyright (c) 1999 - 2010, Vodafone Group Services Ltd
  All rights reserved.

  Redistribution and use in source and binary forms, with or without modification, are permitted provided that the following conditions are met:

  * Redistributions of source code must retain the above copyright notice, this list of conditions and the following disclaimer.
  * Redistributions in binary form must reproduce the above copyright notice, this list of conditions and the following disclaimer in the documentation and/or other materials provided with the distribution.
  * Neither the name of the Vodafone Group Services Ltd nor the names of its contributors may be used to endorse or promote products derived from this software without specific prior written permission.

  THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS" AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE ARE DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT HOLDER OR CONTRIBUTORS BE LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
*/

#include "Favourite.h"

namespace WFAPI {

const wf_uint32 Favourite::INVALID_FAVOURITE_ID = WF_MAX_UINT32;

Favourite::Favourite(const wf_uint32 id)
   : m_id(id)
{
}
Favourite::Favourite(wf_uint32 id,
                     WGS84Coordinate position,
                     const WFString& name,
                     const WFString& description,
                     ItemInfoArray infos )
   : m_id(id), m_position(position), m_name(name),
     m_description(description), m_infos(infos)
{
}

Favourite::~Favourite()
{
}

wf_uint32
Favourite::getID() const
{
   return m_id;
}

WGS84Coordinate
Favourite::getPosition() const
{
   return m_position;
}

void
Favourite::setPosition(const WGS84Coordinate& position)
{
   m_position = position;
}

const WFString&
Favourite::getName() const
{
   return m_name;
}

void
Favourite::setName(const WFString& name)
{
   m_name = name;
}

const WFString&
Favourite::getDescription() const
{
   return m_description;
}

void
Favourite::setDescription(const WFString& description)
{
   m_description = description;
}

const ItemInfoArray&
Favourite::getInformationArray() const
{
   return m_infos;
}

void
Favourite::addItemInfoEntry(const ItemInfoEntry& info)
{
   m_infos.push_back(info);
}

} // End namespace WFAPI
