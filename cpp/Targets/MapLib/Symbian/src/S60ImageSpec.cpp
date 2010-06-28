/*
Copyright (c) 1999 - 2010, Vodafone Group Services Ltd
All rights reserved.

Redistribution and use in source and binary forms, with or without modification, are permitted provided that the following conditions are met:

    * Redistributions of source code must retain the above copyright notice, this list of conditions and the following disclaimer.
    * Redistributions in binary form must reproduce the above copyright notice, this list of conditions and the following disclaimer in the documentation and/or other materials provided with the distribution.
    * Neither the name of the Vodafone Group Services Ltd nor the names of its contributors may be used to endorse or promote products derived from this software without specific prior written permission.

THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS" AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE ARE DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT HOLDER OR CONTRIBUTORS BE LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
*/


#include "S60ImageSpec.h"
#include "S60ImageUtil.h"

S60ImageSpec::~S60ImageSpec() {
   m_img->Release();
   delete m_img;
}

S60ImageSpec::S60ImageSpec(CFbsBitmap* bitmap, CFbsBitmap* mask)
   : ImageSpecInternal(),
     m_img(Image::allocate())
{
   m_img->iBitmap->Duplicate(bitmap->Handle());

   m_img->iWidth = m_img->iBitmap->SizeInPixels().iWidth;
   m_img->iHeight = m_img->iBitmap->SizeInPixels().iHeight;
   
   if(mask) {
      m_img->iMask->Duplicate(mask->Handle());
      m_img->iHasMask = ETrue;
      m_img->iMaskLoaded = ETrue;
      // S60ImageUtil::calculateVisibleRect(m_img);
   } else {
      m_img->iHasMask = EFalse;
      m_img->iMaskLoaded = EFalse;
   }

   m_img->iBitmapLoaded = ETrue;
}

WFAPI::wf_uint32 S60ImageSpec::getWidth() const
{
   return m_img->getWidth();
}

WFAPI::wf_uint32 S60ImageSpec::getHeight() const
{
   return m_img->getHeight();
}

int S60ImageSpec::load( RGBA32BitMap& bitMap ) const
{
   return 0;
}

const Image* S60ImageSpec::getImage() const
{
   return m_img; 
}




