/*
Copyright (c) 1999 - 2010, Vodafone Group Services Ltd
All rights reserved.

Redistribution and use in source and binary forms, with or without modification, are permitted provided that the following conditions are met:

    * Redistributions of source code must retain the above copyright notice, this list of conditions and the following disclaimer.
    * Redistributions in binary form must reproduce the above copyright notice, this list of conditions and the following disclaimer in the documentation and/or other materials provided with the distribution.
    * Neither the name of the Vodafone Group Services Ltd nor the names of its contributors may be used to endorse or promote products derived from this software without specific prior written permission.

THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS" AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE ARE DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT HOLDER OR CONTRIBUTORS BE LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
*/


// #define CORE_LOGGING_MODULE_LEVEL CL_ALL_FILE
#define CORE_LOGGING_MODULE_NAME "CATEGORY_TREE_ELEM"

#include "CategoryTreeElem.h"
#include "nav2util.h"
#include <algorithm>
#include "CoreDebugPrint.h"

namespace CategoryTreeImpl {

using namespace isab;

CategoryTreeElem::CategoryTreeElem() :
   m_id(-1), m_name(NULL), m_imageName(NULL)
{
}

CategoryTreeElem::CategoryTreeElem(const CategoryTreeElem& other)
{
   init(other);
}

CategoryTreeElem::CategoryTreeElem(int32 id,
                   const char* name,
                   const char* imageName,
                   const std::vector<int32>& subCats)
{
   init(id, name, imageName, subCats);
}

void
CategoryTreeElem::init(const CategoryTreeElem& other)
{
   init(other.m_id,
        other.m_name,
        other.m_imageName,
        other.m_subCats);
}

void
CategoryTreeElem::init(int32 id,
                       const char* name,
                       const char* imageName,
                       const std::vector<int32>& subCats)
{
   m_id = id;
   m_name = strdup_new(name);
   m_imageName = strdup_new(imageName);   
   m_subCats = subCats;   
}

CategoryTreeElem::~CategoryTreeElem()
{
   delete[] m_name;
   delete[] m_imageName;
}

void CategoryTreeElem::swap(CategoryTreeElem& other)
{
   std::swap(m_id, other.m_id);
   std::swap(m_name, other.m_name);
   std::swap(m_imageName, other.m_imageName);
   m_subCats.swap(other.m_subCats);
}

int32 CategoryTreeElem::getId() const
{
   return m_id; 
}

const char* CategoryTreeElem::getName() const
{
   return m_name;
}

const char* CategoryTreeElem::getImageName() const
{
   return m_imageName;
}

const std::vector<int32>& CategoryTreeElem::getSubCats() const
{
   return m_subCats;
}

CategoryTreeElem& CategoryTreeElem::operator=(const CategoryTreeElem& other)
{
   if (this == &other) {
      return *this;
   }
   CategoryTreeElem tmp(other);
   tmp.swap(*this);
   return *this;
}

//#define WFAPI_IOSTREAM_AVAILABLE
#ifdef WFAPI_IOSTREAM_AVAILABLE

std::ostream&
operator<< (std::ostream& stream,
            const CategoryTreeElem& catTreeElem)
{
   return stream << catTreeElem.m_id << "\n"
                 << catTreeElem.m_name << "\n"
                 << catTreeElem.m_imageName << "\n"
                 << catTreeElem.m_subCats.size() << "\n";
}

#endif //WFAPI_IOSTREAM_AVAILABLE

} // End namespace CategoryTree
