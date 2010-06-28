/*
Copyright (c) 1999 - 2010, Vodafone Group Services Ltd
All rights reserved.

Redistribution and use in source and binary forms, with or without modification, are permitted provided that the following conditions are met:

    * Redistributions of source code must retain the above copyright notice, this list of conditions and the following disclaimer.
    * Redistributions in binary form must reproduce the above copyright notice, this list of conditions and the following disclaimer in the documentation and/or other materials provided with the distribution.
    * Neither the name of the Vodafone Group Services Ltd nor the names of its contributors may be used to endorse or promote products derived from this software without specific prior written permission.

THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS" AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE ARE DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT HOLDER OR CONTRIBUTORS BE LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
*/


#include "config.h"
#include "DetailedConfigInterface.h"
#include "StackedDialogVisualSpec.h"
#include "LangConverter.h"
#include "MapLib.h"
#include "OverlayView.h"
#include "POICategories.h"
#include "UserDefinedScaleFeature.h"
#include "UserDefinedFeature.h"
#include <vector>
#include "TileMapTypes.h"

#include "CoreFunctionPrologue.h"

namespace WFAPI {

DetailedConfigInterface::DetailedConfigInterface(MapLib* mapLib)
   : m_mapLib(mapLib)
{
}

void
DetailedConfigInterface::setLanguage(TextLanguage language)
{
   CORE_FUNCTION_PROLOGUE();
   
   int nav2Lang = LangConverter::textLanguageToNav2Lang(language);
   m_mapLib->setLanguage(LangTypes::getNavLangAsLanguage(nav2Lang));
}


void
DetailedConfigInterface::setStackedDialogVisualSpecs(
   const StackedDialogVisualSpec* twoItemsDialogSpec,
   const StackedDialogVisualSpec* threeItemsDialogSpec)
{
   CORE_FUNCTION_PROLOGUE();
   
   m_mapLib->getOverlayView()->setStackedDialogVisualSpecs(
      twoItemsDialogSpec, threeItemsDialogSpec);
}

POICategories
DetailedConfigInterface::getPOICategoriesSnapshot()
{
   CORE_FUNCTION_PROLOGUE();
   
   const std::vector<const POICategory*>* categories =
      m_mapLib->getPOICategories();

   POICategories ret;
   if(categories == NULL){
      return ret;
   }
   
   std::vector<const POICategory*>::const_iterator it = categories->begin();
   std::vector<const POICategory*>::const_iterator end = categories->end();
   while(it != end) {
      
      // Copy each category and put them in the return vector.
      ret.push_back(POICategory((*it)->getName(),
                                ((*it)->getID()),
                                (*it)->isVisible())); 
      it++;
   }

   return ret;
   
}

bool
DetailedConfigInterface::setCategoryVisible(wf_uint32 id, bool visible)
{
   CORE_FUNCTION_PROLOGUE();
   
   return m_mapLib->setPOICategoryVisible(id, visible);
}

bool
DetailedConfigInterface::synchronizePOICategories(
   POICategories& localCategories)
{
   CORE_FUNCTION_PROLOGUE();
   
   POICategories tmp = getPOICategoriesSnapshot();
   bool categoriesAddedOrDeleted = false;
   
   for (unsigned int i = 0; i < localCategories.size(); i++){
      CORE_FUNCTION_PROLOGUE();
      WFAPI::POICategory& cat = localCategories[i];
      unsigned int tmpIndex = 0;
      bool found = false;
      while(tmpIndex < tmp.size() && !found){
         if(cat.getID() == tmp[tmpIndex].getID()){
            // The category still exists in MapLib
            found = true;
            
            // We set the visibility of the return set
            // to the same as the input set.
            tmp[tmpIndex].setVisible(cat.isVisible());

            // We tell mapLib to use the same visibility setting
            m_mapLib->setPOICategoryVisible(cat.getID(), cat.isVisible());
         }
         tmpIndex++;
      }
         
      // If the category was not found, there was a difference between
      // the category sets and the returned set will have categories
      // removed or added. 
      if(!found){
         categoriesAddedOrDeleted = true;
      }
   }
   // Swap the input set with the temporary return set.
   std::swap(tmp, localCategories);

   // We check the size of the sets to determine if
   // any categories have been added. If something has been removed
   // and something else added, the sizes will be the same, but
   // the algorithm above would have detected the removed category.
   if(tmp.size() != localCategories.size()){
      categoriesAddedOrDeleted = true;
   }
   
   return categoriesAddedOrDeleted;
}

void
DetailedConfigInterface::showTrafficInfo(bool enabled)
{
   CORE_FUNCTION_PROLOGUE();
   
   // std::cerr << "DCI::showTrafficInfo " << enabled << std::endl;
   m_mapLib->setLayerVisible(TileMapTypes::c_trafficLayer, enabled);
}

void
DetailedConfigInterface::setTrafficInfoUpdateInterval(
   wf_uint32 updateIntervalMinutes)
{
   CORE_FUNCTION_PROLOGUE();
   
   // std::cerr << "DCI::setTrafficInfoUpdateInterval " << updateIntervalMinutes
   //           << std::endl;
   m_mapLib->setUpdatePeriodMinutes(TileMapTypes::c_trafficLayer, 
                                    updateIntervalMinutes);
}

void DetailedConfigInterface::setMaxDiskCacheSize(wf_uint32 nbrBytes)
{
   CORE_FUNCTION_PROLOGUE();
   
   m_mapLib->setDiskCacheSize(nbrBytes);
}

void DetailedConfigInterface::clearDiskCache()
{
   CORE_FUNCTION_PROLOGUE();
   
   m_mapLib->clearDiskCache();
}

bool WFAPI::DetailedConfigInterface::showCopyright(bool show)
{
   CORE_FUNCTION_PROLOGUE();

   return m_mapLib->showCopyright(show);
}

void WFAPI::DetailedConfigInterface::setCopyrightPos(const ScreenPoint& pos)
{
   CORE_FUNCTION_PROLOGUE();
   
   m_mapLib->setCopyrightPos(pos);
}

void WFAPI::DetailedConfigInterface::enableAutomaticHighlight(bool enable)
{
   CORE_FUNCTION_PROLOGUE();
   
   m_mapLib->enableAutomaticHighlight(enable);
}

void DetailedConfigInterface::hidePOIs(bool hidden)
{
   m_mapLib->hidePOIs(hidden);
}



} // End of namespace WFAPI






