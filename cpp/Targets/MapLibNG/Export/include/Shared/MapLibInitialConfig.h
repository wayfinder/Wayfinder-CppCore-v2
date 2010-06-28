/*
  Copyright (c) 1999 - 2010, Vodafone Group Services Ltd
  All rights reserved.

  Redistribution and use in source and binary forms, with or without modification, are permitted provided that the following conditions are met:

  * Redistributions of source code must retain the above copyright notice, this list of conditions and the following disclaimer.
  * Redistributions in binary form must reproduce the above copyright notice, this list of conditions and the following disclaimer in the documentation and/or other materials provided with the distribution.
  * Neither the name of the Vodafone Group Services Ltd nor the names of its contributors may be used to endorse or promote products derived from this software without specific prior written permission.

  THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS" AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE ARE DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT HOLDER OR CONTRIBUTORS BE LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
*/

#ifndef WFAPI_MAPLIBINITIALCONFIG_H
#define WFAPI_MAPLIBINITIALCONFIG_H

#include "PALMachineTypes.h"
#include "WFString.h"

// Forward definitions.
class MapLib;

namespace WFAPI {

// Forward definitions.
class WFString;

/**
 * Class used to configure the initial MapLib properties.
 * These settings cannot be changed at runtime.
 */
class MapLibInitialConfig {
public:
  
   /**
    * Constructor.
    */
   MapLibInitialConfig();

   /**
    * Set the path to the disk cache for maps.
    *
    * @param path The path to the disk cache.
    * @param maxSize The maximum size in bytes that the disk cache
    *                may use. Recommended values between
    *                1000000 and 1000000 (1-10 MB).
    * @return True if the path could succesfully be used to store
    *         the cache.
    */
   bool setDiskCacheSettings(const WFString& path, wf_uint32 maxSize);

   /**
    * Returns the cache path that this config has set.
    *
    * 
    * @return The path to the disk cache.
    */ 
   const WFString& getCachePath() const;

   /**
    * Returns the maximum size for the disk cache.
    *
    * @return The maximum size for the disk cache. 
    */ 
   wf_uint32 getCacheMaxSize() const;
private:
   /**
    * Path to cache.
    */
   WFString m_cachePath;

   /**
    * Max cache size.
    */
   wf_uint32 m_cacheMaxSize;
};


} // End of namespace WFAPI

#endif // WFAPI_MAPLIBINITIALCONFIG_H
