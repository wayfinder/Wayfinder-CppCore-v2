/*
Copyright (c) 1999 - 2010, Vodafone Group Services Ltd
All rights reserved.

Redistribution and use in source and binary forms, with or without modification, are permitted provided that the following conditions are met:

    * Redistributions of source code must retain the above copyright notice, this list of conditions and the following disclaimer.
    * Redistributions in binary form must reproduce the above copyright notice, this list of conditions and the following disclaimer in the documentation and/or other materials provided with the distribution.
    * Neither the name of the Vodafone Group Services Ltd nor the names of its contributors may be used to endorse or promote products derived from this software without specific prior written permission.

THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS" AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE ARE DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT HOLDER OR CONTRIBUTORS BE LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
*/


#include "SearchCategory.h"

namespace WFAPI {

SearchCategory::SearchCategory(const WFString& name,
                               const WFString& id,
                               wf_int32 intId,
                               const WFString& imageName)
   : m_name(name),
     m_id(id),
     m_intId(intId),
     m_imageName(imageName)
{
    
}

SearchCategory::~SearchCategory()
{    
}

const WFString& SearchCategory::getName() const
{
   return m_name;
}

const WFString& SearchCategory::getId() const
{
   return m_id;
}

wf_int32 SearchCategory::getIntId() const
{
   return m_intId;
}

const WFString& SearchCategory::getImageName() const
{
   return m_imageName;
}

void SearchCategory::setName(const WFString& name)
{
   m_name = name;
}

void SearchCategory::setId(const WFString& id)
{
   m_id = id;
}

void SearchCategory::setIntId(wf_int32 intId)
{
   m_intId = intId;
}

void SearchCategory::setImageName(const WFString& imageName)
{
   m_imageName = imageName;
}

} // End namespace WFAPI
