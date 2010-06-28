/*
Copyright (c) 1999 - 2010, Vodafone Group Services Ltd
All rights reserved.

Redistribution and use in source and binary forms, with or without modification, are permitted provided that the following conditions are met:

    * Redistributions of source code must retain the above copyright notice, this list of conditions and the following disclaimer.
    * Redistributions in binary form must reproduce the above copyright notice, this list of conditions and the following disclaimer in the documentation and/or other materials provided with the distribution.
    * Neither the name of the Vodafone Group Services Ltd nor the names of its contributors may be used to endorse or promote products derived from this software without specific prior written permission.

THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS" AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE ARE DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT HOLDER OR CONTRIBUTORS BE LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
*/


#include "MapObjectInfo.h"


namespace WFAPI {

MapObjectInfo::MapObjectInfo(const WFString& name,
                             map_object_info_t type,
                             wf_int32 idKey,
                             void* genericData) :
   m_name(name),
   m_type(type),
   m_idType(ID_KEY),
   m_idKey(idKey),
   m_genericData(genericData)

{
    
}

MapObjectInfo::MapObjectInfo(const WFString& name,
                             map_object_info_t type,
                             const WFString& idString,
                             void* genericData) :
   m_name(name),
   m_type(type),
   m_idType(ID_STRING),
   m_idKey(INVALID_FEATURE),
   m_idString(idString),
   m_genericData(genericData)
{
    
}

const WFString&
MapObjectInfo::getName() const
{
   return m_name;
}

MapObjectInfo::map_object_info_t
MapObjectInfo::getType() const
{
   return m_type;
}

MapObjectInfo::idType
MapObjectInfo::getIDType() const
{
   return m_idType;
}

wf_int32
MapObjectInfo::getIDKey() const
{
   return m_idKey;
}

const WFString&
MapObjectInfo::getIDString() const
{
   return m_idString;
}

void
MapObjectInfo::setGenericData(void* genericData)
{
   m_genericData = genericData;
}

void*
MapObjectInfo::getGenericData() const
{
   return m_genericData;
}

} // End of namespace WFAPI
