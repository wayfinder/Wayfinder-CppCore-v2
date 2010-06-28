/*
Copyright (c) 1999 - 2010, Vodafone Group Services Ltd
All rights reserved.

Redistribution and use in source and binary forms, with or without modification, are permitted provided that the following conditions are met:

    * Redistributions of source code must retain the above copyright notice, this list of conditions and the following disclaimer.
    * Redistributions in binary form must reproduce the above copyright notice, this list of conditions and the following disclaimer in the documentation and/or other materials provided with the distribution.
    * Neither the name of the Vodafone Group Services Ltd nor the names of its contributors may be used to endorse or promote products derived from this software without specific prior written permission.

THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS" AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE ARE DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT HOLDER OR CONTRIBUTORS BE LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
*/

#include "BufferRequester/BufferRequesterSettings.h"
#include "BufferRequester/DiskCacheSettings.h"
#include "BufferRequester/MemCacheSettings.h"
#include "BufferRequester/ResourceDirectorySettings.h"

struct BufferRequesterSettings::Impl {
   Impl() :
      diskSettings("", 0),
      memSettings(1024*100),
      resDirSettings("", 0)
   {}
   
   DiskCacheSettings diskSettings;
   MemCacheSettings memSettings;
   ResourceDirectorySettings resDirSettings;
};

BufferRequesterSettings::BufferRequesterSettings() 
{
   m_impl = new Impl;
}

BufferRequesterSettings::~BufferRequesterSettings()
{
   delete m_impl; 
}

BufferRequesterSettings::BufferRequesterSettings(const BufferRequesterSettings& rhs)
{
   m_impl = new Impl;
   m_impl->diskSettings = rhs.m_impl->diskSettings;
   m_impl->memSettings = rhs.m_impl->memSettings;
}

BufferRequesterSettings& BufferRequesterSettings::operator=(const BufferRequesterSettings& rhs)
{
   m_impl = new Impl;
   m_impl->diskSettings = rhs.m_impl->diskSettings;
   m_impl->memSettings = rhs.m_impl->memSettings;
   
   return *this;
}

void BufferRequesterSettings::setDiskCacheSettings(const DiskCacheSettings& diskSettings)
{
   m_impl->diskSettings = diskSettings;
}

void BufferRequesterSettings::setMemCacheSettings(const MemCacheSettings& memSettings)
{
   m_impl->memSettings = memSettings;
}

void BufferRequesterSettings::setResourceDirectorySettings(
   const ResourceDirectorySettings& resDirSettings)
{
   m_impl->resDirSettings = resDirSettings; 
}


const DiskCacheSettings& BufferRequesterSettings::getDiskCacheSettings() const
{
   return m_impl->diskSettings;
}

const MemCacheSettings& BufferRequesterSettings::getMemCacheSettings() const
{
   return m_impl->memSettings;
}

const ResourceDirectorySettings&
BufferRequesterSettings::getResourceDirectorySettings() const
{
   return m_impl->resDirSettings; 
}


