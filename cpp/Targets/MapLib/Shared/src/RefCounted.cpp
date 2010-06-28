/*
Copyright (c) 1999 - 2010, Vodafone Group Services Ltd
All rights reserved.

Redistribution and use in source and binary forms, with or without modification, are permitted provided that the following conditions are met:

    * Redistributions of source code must retain the above copyright notice, this list of conditions and the following disclaimer.
    * Redistributions in binary form must reproduce the above copyright notice, this list of conditions and the following disclaimer in the documentation and/or other materials provided with the distribution.
    * Neither the name of the Vodafone Group Services Ltd nor the names of its contributors may be used to endorse or promote products derived from this software without specific prior written permission.

THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS" AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE ARE DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT HOLDER OR CONTRIBUTORS BE LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
*/


#define CORE_LOGGING_MODULE_NAME "REF_COUNTED"
#include "CoreFunctionPrologue.h"
#include "CoreDebugPrint.h"
#include "RefCounted.h"
#include "OverlayItemZoomSpec.h"
#include "OverlayItem.h"
#include "StackedDialogVisualSpec.h"
#include "OverlayItemVisualSpec.h"
#include "ImageSpec.h"

#include "CoreDebugPrint.h"

#ifdef REFCOUNTED_DEBUGGING_ENABLED
#  include <set>
#endif

namespace WFAPI {

#ifdef REFCOUNTED_DEBUGGING_ENABLED
typedef std::set<const RefCounted*>  RefSet;
RefSet g_allocated;

void debugAllocatedRefs() {

   coreprintln("");
   
   if(!g_allocated.empty()) {
      coreprintln("Dangling refcounted objects:");
      coreprintln("----------------------------");
   } else {
      coreprintln("Congratulations! No dangling refcounted objects.");
      coreprintln("------------------------------------------------");
   }

   typedef const OverlayItem OI;
   typedef const OverlayItemZoomSpec ZSPEC;
   typedef const OverlayItemVisualSpec VSPEC;
   typedef const StackedDialogVisualSpec DSPEC;
   typedef const ImageSpec ISPEC;

   const char* formatStr    = "%s at 0x%x with num-refs: %d. ";
   const char* extFormatStr = "%s at 0x%x with num-refs: %d. Info: ";
   
   for(RefSet::iterator itr = g_allocated.begin();
       itr != g_allocated.end(); itr++)
   {
      unsigned int numRefs =  (*itr)->numRefs();
      
      if(ZSPEC* i = dynamic_cast<ZSPEC*>(*itr)) {
         coreprintln(formatStr, "OverlayItemZoomSpec", i, numRefs, "");
      } else if(OI* i = dynamic_cast<OI*>(*itr)) {         
         coreprintln(extFormatStr, "OverlayItemZoomSpec", i,
                    numRefs, i->getMapObjectInfo().getName().c_str());
      } else if(VSPEC* i = dynamic_cast<VSPEC*>(*itr)) {
         coreprintln(formatStr, "OverlayItemVisualSpec", i, numRefs, "");         
      } else if(DSPEC* i = dynamic_cast<DSPEC*>(*itr)) {
         coreprintln(formatStr, "StackedDialogVisualSpec", i, numRefs, "");
      } else if(ISPEC* i = dynamic_cast<ISPEC*>(*itr)) {
         coreprintln(formatStr, "ImageSpec", i, numRefs, "");
      }
   }
}
#endif // REFCOUNTED_DEBUGGING_ENABLED

void RefCounted::addReference() const
{
   CORE_FUNCTION_PROLOGUE();
   m_numRefs++;
}

void RefCounted::removeReference() const
{
   CORE_FUNCTION_PROLOGUE();
   
   m_numRefs--;


   if(m_numRefs == 0) {
      delete this;
#ifdef REFCOUNTED_DEBUGGING_ENABLED
      g_allocated.erase(this);
#endif // REFCOUNTED_DEBUGGING_ENABLED
   }

}

RefCounted::RefCounted()
{
   m_numRefs = 1;
   
#ifdef REFCOUNTED_DEBUGGING_ENABLED
   g_allocated.insert(this);
#endif //REFCOUNTED_DEBUGGING_ENABLED
}

unsigned int RefCounted::numRefs() const
{
   CORE_FUNCTION_PROLOGUE();
   
   return m_numRefs;
}


RefCounted::~RefCounted() 
{
    
}

} // End of namespace WFAPI

