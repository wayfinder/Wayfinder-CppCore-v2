/*
  Copyright (c) 1999 - 2010, Vodafone Group Services Ltd
  All rights reserved.

  Redistribution and use in source and binary forms, with or without modification, are permitted provided that the following conditions are met:

  * Redistributions of source code must retain the above copyright notice, this list of conditions and the following disclaimer.
  * Redistributions in binary form must reproduce the above copyright notice, this list of conditions and the following disclaimer in the documentation and/or other materials provided with the distribution.
  * Neither the name of the Vodafone Group Services Ltd nor the names of its contributors may be used to endorse or promote products derived from this software without specific prior written permission.

  THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS" AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE ARE DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT HOLDER OR CONTRIBUTORS BE LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
*/

#ifndef WFAPI_IMAGE_REPLY_DATA_H
#define WFAPI_IMAGE_REPLY_DATA_H

#include "WFAPIConfig.h"

namespace WFAPI {

/**
 * Representation of an image buffer.
 */
class ImageReplyData
{
   
public:

   /**
    * Constructor.
    *
    * @param imgBuf The image buffer.
    * @param imgBufSize The size of the image buffer.
    */
   ImageReplyData(const wf_uint8* imgBuf, wf_uint32 imgBufSize);
   
   /**
    * Destructor, the image buffer is destroyed in the destructor.
    */
   virtual ~ImageReplyData();

   /**
    * Get a reference to the image buffer.
    *
    * @return The image buffer as an array.
    */
   const wf_uint8* getImageBuffer() const;

   /**
    * Get the size of the image buffer.
    *
    * @return The size of the image buffer.
    */
   wf_uint32 getImageBufferSize() const;
   
private:

   /// The image buffer 
   const wf_uint8* m_imgBuf;

   /// The size of the image buffer.
   wf_uint32 m_size;
};

} // End namespace WFAPI

#endif // End WFAPI_IMAGE_REPLY_DATA_H
