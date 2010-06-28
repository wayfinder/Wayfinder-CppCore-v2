/*
Copyright (c) 1999 - 2010, Vodafone Group Services Ltd
All rights reserved.

Redistribution and use in source and binary forms, with or without modification, are permitted provided that the following conditions are met:

    * Redistributions of source code must retain the above copyright notice, this list of conditions and the following disclaimer.
    * Redistributions in binary form must reproduce the above copyright notice, this list of conditions and the following disclaimer in the documentation and/or other materials provided with the distribution.
    * Neither the name of the Vodafone Group Services Ltd nor the names of its contributors may be used to endorse or promote products derived from this software without specific prior written permission.

THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS" AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE ARE DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT HOLDER OR CONTRIBUTORS BE LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
*/


#include "CoreFunctionPrologue.h"
#include "config.h"
#include "OverlayItemZoomSpec.h"
#include "OverlayItemVisualSpec.h"
#include "RefCountHelpers.h"
#include <algorithm>


namespace WFAPI {

OverlayItemZoomSpec* OverlayItemZoomSpec::allocate()
{
   CORE_FUNCTION_PROLOGUE();
   
   return new OverlayItemZoomSpec;
}

WFAPI::OverlayItemZoomSpec::~OverlayItemZoomSpec()
{
   CORE_FUNCTION_PROLOGUE();
   
   ZoomRangeVector::const_iterator end = m_zoomRanges.end();
   
   for(ZoomRangeVector::const_iterator itr = m_zoomRanges.begin();
       itr != end; itr++)
   {
      const ZoomRange& cur = *itr;

      if(cur.normalSpec) {
         REMOVE_REF(cur.normalSpec);         
      }

      if(cur.stackedSpec) {
         REMOVE_REF(cur.stackedSpec);         
      }

      if(cur.normalTappedSpec) {
         REMOVE_REF(cur.normalTappedSpec);         
      }

      if(cur.stackedSpec) {
         REMOVE_REF(cur.stackedTappedSpec);         
      }
   }
   
   if (m_normalHighlighted){
      REMOVE_REF(m_normalHighlighted);      
   }
   
   if (m_stackedHighlighted){
      REMOVE_REF(m_stackedHighlighted);      
   }

      
   if (m_normalTappedHighlighted){
      REMOVE_REF(m_normalTappedHighlighted);      
   }
   
   if (m_stackedTappedHighlighted){
      REMOVE_REF(m_stackedTappedHighlighted);      
   }
}

bool zoomRangeCmp(const OverlayItemZoomSpec::ZoomRange& lhs,
                  const OverlayItemZoomSpec::ZoomRange& rhs)
{   
   return lhs.startZoomLevel < rhs.startZoomLevel;
}
void
OverlayItemZoomSpec::addZoomLevelRange(wf_uint32 minZoom, wf_uint32 maxZoom,
                                       const OverlayItemVisualSpec* normalSpec,
                                       const OverlayItemVisualSpec* stackedSpec)
{
   CORE_FUNCTION_PROLOGUE();
   
   // We set the "untapped" specs twice. Then there will be no visual difference when
   // tapping the items.
   addZoomLevelRange(minZoom, maxZoom,
                     normalSpec, stackedSpec,
                     normalSpec, stackedSpec);
}


void
OverlayItemZoomSpec::addZoomLevelRange(wf_uint32 minZoom, wf_uint32 maxZoom,
                                       const OverlayItemVisualSpec* normalSpec,
                                       const OverlayItemVisualSpec* stackedSpec,
                                       const OverlayItemVisualSpec* normalTappedSpec,
                                       const OverlayItemVisualSpec* stackedTappedSpec)
{
   CORE_FUNCTION_PROLOGUE();
   
   if(normalSpec) {
      normalSpec->addReference();      
   }

   if(stackedSpec) {
      stackedSpec->addReference();      
   }

   if(normalTappedSpec) {
      normalTappedSpec->addReference();      
   }

   if(stackedTappedSpec) {
      stackedTappedSpec->addReference();      
   }

   ZoomRange range = { minZoom, maxZoom,
                       normalSpec,
                       stackedSpec,
                       normalTappedSpec,
                       stackedTappedSpec};

   m_zoomRanges.push_back(range);
   
   std::sort(m_zoomRanges.begin(), m_zoomRanges.end(), zoomRangeCmp);
}
void
OverlayItemZoomSpec::setHighlightedSpecs(const OverlayItemVisualSpec* normalSpec,
                                         const OverlayItemVisualSpec* stackedSpec)
{
   CORE_FUNCTION_PROLOGUE();
   
   setHighlightedSpecs(normalSpec, stackedSpec,
                       normalSpec, stackedSpec);
}

void
OverlayItemZoomSpec::setHighlightedSpecs(
   const OverlayItemVisualSpec* normalSpec,
   const OverlayItemVisualSpec* stackedSpec,
   const OverlayItemVisualSpec* normalTappedSpec,
   const OverlayItemVisualSpec* stackedTappedSpec)
{
   CORE_FUNCTION_PROLOGUE();
   
   m_normalHighlighted = normalSpec;

   if (m_normalHighlighted){
      m_normalHighlighted->addReference();
   }

   m_stackedHighlighted = stackedSpec;
   
   if (m_stackedHighlighted){
      m_stackedHighlighted->addReference();
   }

   m_normalTappedHighlighted = normalTappedSpec;

   if (m_normalTappedHighlighted){
      m_normalTappedHighlighted->addReference();
   }

   m_stackedTappedHighlighted = stackedTappedSpec;
   
   if (m_stackedTappedHighlighted){
      m_stackedTappedHighlighted->addReference();
   }
}

const OverlayItemVisualSpec*
OverlayItemZoomSpec::getNormalSpec(wf_uint32 zoomLevel) const
{
   CORE_FUNCTION_PROLOGUE();
   
   const ZoomRange* range = getZoomRange(zoomLevel);
   
   if(!range) {
      return NULL;
   } else {
      return range->normalSpec;
   }
}

const OverlayItemVisualSpec*
OverlayItemZoomSpec::getStackedSpec(wf_uint32 zoomLevel) const
{
   CORE_FUNCTION_PROLOGUE();
   
   const ZoomRange* range = getZoomRange(zoomLevel);
   
   if(!range) {
      return NULL;
   } else {
      return range->stackedSpec;
   }
}

const OverlayItemVisualSpec*
OverlayItemZoomSpec::getNormalTappedSpec(wf_uint32 zoomLevel) const
{
   CORE_FUNCTION_PROLOGUE();
   
   const ZoomRange* range = getZoomRange(zoomLevel);
   
   if(!range) {
      return NULL;
   } else {
      return range->normalTappedSpec;
   }
}

const OverlayItemVisualSpec*
OverlayItemZoomSpec::getStackedTappedSpec(wf_uint32 zoomLevel) const
{
   CORE_FUNCTION_PROLOGUE();
   
   const ZoomRange* range = getZoomRange(zoomLevel);
   
   if(!range) {
      return NULL;
   } else {
      return range->stackedTappedSpec;
   }
}

const OverlayItemZoomSpec::ZoomRange*
OverlayItemZoomSpec::getZoomRange(wf_uint32 zoomLevel) const
{
   CORE_FUNCTION_PROLOGUE();
   
   ZoomRangeVector::const_iterator end = m_zoomRanges.end();
   
   for(ZoomRangeVector::const_iterator itr = m_zoomRanges.begin();
       itr != end; itr++)
   {
      const ZoomRange& cur = *itr;

      if( zoomLevel >= cur.startZoomLevel &&
          zoomLevel <= cur.endZoomLevel )
      {
         return &cur;
      }
   }
   
   return NULL;
}

const OverlayItemVisualSpec*
OverlayItemZoomSpec::getNormalHighlightedSpec() const
{
   CORE_FUNCTION_PROLOGUE();
   
   return m_normalHighlighted;
}

const OverlayItemVisualSpec*
OverlayItemZoomSpec::getStackedHighlightedSpec() const
{
   CORE_FUNCTION_PROLOGUE();
   
   return m_stackedHighlighted;
}


OverlayItemZoomSpec::OverlayItemZoomSpec()
   : RefCounted(),
     m_normalHighlighted(NULL),
     m_stackedHighlighted(NULL),
     m_normalTappedHighlighted(NULL),
     m_stackedTappedHighlighted(NULL)
{
   CORE_FUNCTION_PROLOGUE(); 
}

const OverlayItemVisualSpec*
OverlayItemZoomSpec::getNormalTappedHighlightedSpec() const
{
   CORE_FUNCTION_PROLOGUE();
   
   return m_normalTappedHighlighted;
}

const OverlayItemVisualSpec*
OverlayItemZoomSpec::getStackedTappedHighlightedSpec() const
{
   CORE_FUNCTION_PROLOGUE();
   
   return m_stackedTappedHighlighted;
}

} // End of namespace WFAPI


