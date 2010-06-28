/*
Copyright (c) 1999 - 2010, Vodafone Group Services Ltd
All rights reserved.

Redistribution and use in source and binary forms, with or without modification, are permitted provided that the following conditions are met:

    * Redistributions of source code must retain the above copyright notice, this list of conditions and the following disclaimer.
    * Redistributions in binary form must reproduce the above copyright notice, this list of conditions and the following disclaimer in the documentation and/or other materials provided with the distribution.
    * Neither the name of the Vodafone Group Services Ltd nor the names of its contributors may be used to endorse or promote products derived from this software without specific prior written permission.

THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS" AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE ARE DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT HOLDER OR CONTRIBUTORS BE LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
*/


#ifndef TMAP_IMAGE_OBSERVER_H
#define TMAP_IMAGE_OBSERVER_H

#include "machine.h"

/**
 * Observer that will receive callbacks when
 * a request is done to FetchImage. 
 * Interface that has to be implemented
 * by the receiver of ImageReceived
 * class TmapImageObserver
 */
class TmapImageObserver
{
public:

   /**
    * Function gets called by TmapImageHandler
    * when a requested image is downloaded.
    * @param imageName, The name of the downloaded image
    */
   virtual void ImageReceived(const char* imageName) = 0;

   /**
    * Function gets called by TmapImageHandler
    * when a requested image is downloaded and will be returned as a Buffer.
    * @param imageName, The name of the downloaded image
    */
   virtual void ImageReceived(const char* imageName,
                              const uint8* imageBuffer,
                              uint32 imageBufferLength) = 0;

   /**
    * Function gets called by TmapImageHandler
    * when a requested image failed to be downloaded.
    * @param imageName, The name of the image that failed to be downloaded
    */
   virtual void ImageReceivedError(const char* imageName) = 0;

};

#endif
