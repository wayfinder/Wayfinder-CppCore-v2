/*
  Copyright (c) 1999 - 2010, Vodafone Group Services Ltd
  All rights reserved.

  Redistribution and use in source and binary forms, with or without modification, are permitted provided that the following conditions are met:

  * Redistributions of source code must retain the above copyright notice, this list of conditions and the following disclaimer.
  * Redistributions in binary form must reproduce the above copyright notice, this list of conditions and the following disclaimer in the documentation and/or other materials provided with the distribution.
  * Neither the name of the Vodafone Group Services Ltd nor the names of its contributors may be used to endorse or promote products derived from this software without specific prior written permission.

  THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS" AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE ARE DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT HOLDER OR CONTRIBUTORS BE LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
*/

#include "SearchInterface.h"
#include "SearchInterfaceImpl.h"
#include "CategoryTreeInterfaceImpl.h"
#include "OneSearchInterfaceImpl.h"
#include "CoreFunctionPrologue.h"

namespace WFAPI {

struct SearchInterface::impl {
   impl(Nav2APIImpl* nav2APIImpl) :
      m_impl(nav2APIImpl),
      m_ctImpl(nav2APIImpl),
      m_osImpl(nav2APIImpl) {}
      
   SearchInterfaceImpl m_impl;
   CategoryTreeInterfaceImpl m_ctImpl;
   OneSearchInterfaceImpl m_osImpl;
};


SearchInterface::SearchInterface(Nav2APIImpl* nav2APIImpl)
{
   m_impl = new impl(nav2APIImpl);
}

SearchInterface::~SearchInterface()
{
   delete m_impl;
}

void 
SearchInterface::addSearchListener(SearchListener* listener)
{
   CORE_FUNCTION_PROLOGUE();

   m_impl->m_impl.addSearchListener(listener);
   m_impl->m_ctImpl.addSearchListener(listener);
}

void 
SearchInterface::removeSearchListener(SearchListener* listener)
{
   CORE_FUNCTION_PROLOGUE();

   m_impl->m_impl.removeSearchListener(listener);
   m_impl->m_ctImpl.removeSearchListener(listener);
}

SynchronousStatus
SearchInterface::getSearchHistory(SearchHistoryArray& searchHistoryList)
{
   CORE_FUNCTION_PROLOGUE();

   return m_impl->m_impl.getSearchHistory(searchHistoryList);
}

SynchronousStatus
SearchInterface::getSearchDesc(SearchDescArray& searchDescList)
{
   CORE_FUNCTION_PROLOGUE();

   return m_impl->m_impl.getSearchDesc(searchDescList);
}

SynchronousStatus
SearchInterface::getTopRegions(TopRegionArray& topRegionList)
{
   CORE_FUNCTION_PROLOGUE();

   return m_impl->m_impl.getTopRegions(topRegionList);   
}

SynchronousStatus
SearchInterface::getSearchCategories(SearchCategoryArray& searchCategoryArray)
{
   CORE_FUNCTION_PROLOGUE();

   return m_impl->m_impl.getSearchCategories(searchCategoryArray);
}

AsynchronousStatus
SearchInterface::getSearchCategoriesByPosition(const WGS84Coordinate& position)
{
   CORE_FUNCTION_PROLOGUE();

   return m_impl->m_impl.getSearchCategoriesByPosition(position);
}

AsynchronousStatus
SearchInterface::search(const SearchQuery& searchQuery)
{
   CORE_FUNCTION_PROLOGUE();

   return m_impl->m_impl.search(searchQuery);   
}

AsynchronousStatus
SearchInterface::requestNextSearchResults(wf_uint32 index,
                                          wf_uint32 maxHits,
                                          wf_uint32 heading)
{
   CORE_FUNCTION_PROLOGUE();

   return m_impl->m_impl.requestNextSearchResults(index,
                                                  maxHits,
                                                  heading);
}


AsynchronousStatus
SearchInterface::extendedSearch(const WFString& areaID,
                                const WFString& areaName,
                                wf_uint32 heading)
{
   CORE_FUNCTION_PROLOGUE();

   return m_impl->m_impl.extendedSearch(areaID, areaName, heading);   
}

AsynchronousStatus
SearchInterface::searchDetails(const SearchIDArray& searchIDArray)
{
   CORE_FUNCTION_PROLOGUE();

   return m_impl->m_impl.searchDetails(searchIDArray);   
}

AsynchronousStatus
SearchInterface::oneSearch(const OneSearchQuery& searchQuery)
{
   CORE_FUNCTION_PROLOGUE();

   return m_impl->m_osImpl.sendSearchRequest(searchQuery);
}

AsynchronousStatus
SearchInterface::requestCategoryTree(const WGS84Coordinate& coord)
{
   CORE_FUNCTION_PROLOGUE();

   return m_impl->m_ctImpl.requestCategoryTree(coord);
}

SynchronousStatus
SearchInterface::getRootCategories(CategoryTreeArray& rootCategoryList)
{
   return m_impl->m_ctImpl.getRootCategories(rootCategoryList);
}

SynchronousStatus
SearchInterface::getSubCategoriesOf(wf_int32 categoryId,
                                    CategoryTreeArray& subCategoryList)
{
   return m_impl->m_ctImpl.getSubCategoriesOf(categoryId,
                                              subCategoryList);
}

SearchInterfaceImpl*
SearchInterface::getImpl()
{
   return &m_impl->m_impl;   
}

CategoryTreeInterfaceImpl*
SearchInterface::getCategoryTreeImpl()
{
   return &m_impl->m_ctImpl;   
}

OneSearchInterfaceImpl*
SearchInterface::getOneSearchImpl()
{
   return &m_impl->m_osImpl;
}

} // End namespace WFAPI








