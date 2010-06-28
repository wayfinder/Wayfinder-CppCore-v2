/*
Copyright (c) 1999 - 2010, Vodafone Group Services Ltd
All rights reserved.

Redistribution and use in source and binary forms, with or without modification, are permitted provided that the following conditions are met:

    * Redistributions of source code must retain the above copyright notice, this list of conditions and the following disclaimer.
    * Redistributions in binary form must reproduce the above copyright notice, this list of conditions and the following disclaimer in the documentation and/or other materials provided with the distribution.
    * Neither the name of the Vodafone Group Services Ltd nor the names of its contributors may be used to endorse or promote products derived from this software without specific prior written permission.

THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS" AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE ARE DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT HOLDER OR CONTRIBUTORS BE LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
*/


#include "S60OverlayHelpers.h"
#include "Symbian/SymbianFactory.h"
#include <akniconutils.h>
#include <FBS.H>
#include "SymbianTextUtil.h"
#include "BAUTILS.H"
#include <e32debug.h>
#include <eikenv.h>

namespace S60OverlayHelpers {

using namespace WFAPI;

WFAPI::ImageSpec* S60OverlayHelpers::loadMif(const std::string& path,
                                             const std::string& fileName)
{
   std::string fullPath(path + "\\" + fileName);

   HBufC* symbianPath = SymbianTextUtil::toHBufC(fullPath.c_str());

   if(!BaflUtils::FileExists(CEikonEnv::Static()->FsSession(), *symbianPath))
   {
      CEikonEnv::InfoWinL( _L("Mif image file not found"),
                           *symbianPath );

      delete symbianPath;
      
      return NULL;
   }
      
   CFbsBitmap* bitmap = NULL;
   CFbsBitmap* mask = NULL;

   TInt bitmapId = 0;
   TInt maskId = 1;
   
   AknIconUtils::ValidateLogicalAppIconId(*symbianPath, bitmapId, maskId);
   AknIconUtils::CreateIconL(bitmap, mask, *symbianPath, bitmapId, maskId);

   TSize size;
   AknIconUtils::GetContentDimensions(bitmap, size);
   AknIconUtils::SetSize(bitmap, size);
   
   ImageSpec* spec = SymbianFactory::createSymbianImageSpec(bitmap, mask);
   
   delete symbianPath;
   delete bitmap;
   delete mask;
   
   return spec; 
}

} // End of namespace S60OverlayHelpers
