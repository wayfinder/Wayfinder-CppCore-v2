/*
  Copyright (c) 1999 - 2010, Vodafone Group Services Ltd
  All rights reserved.

  Redistribution and use in source and binary forms, with or without modification, are permitted provided that the following conditions are met:

  * Redistributions of source code must retain the above copyright notice, this list of conditions and the following disclaimer.
  * Redistributions in binary form must reproduce the above copyright notice, this list of conditions and the following disclaimer in the documentation and/or other materials provided with the distribution.
  * Neither the name of the Vodafone Group Services Ltd nor the names of its contributors may be used to endorse or promote products derived from this software without specific prior written permission.

  THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS" AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE ARE DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT HOLDER OR CONTRIBUTORS BE LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
*/

#ifndef WFAPI_IMAGE_LISTENER_H
#define WFAPI_IMAGE_LISTENER_H

#include "WFAPIConfig.h"
#include "Listener.h"
#include "ImageStatusCode.h"
#include "ImageReplyData.h"

namespace WFAPI {

/**
 * Listener for Image replies.  The abstract functions are the
 * callback functions that is called for the respective events.
 *
 * Create a subclass and implement the callbacks and add a 
 * subclass object to the ImageInterface to start getting
 * imageReply callbacks.
 *
 * The error function, declared in the super class Listener, will be called
 * for all requests that returns with an error. The status code can be
 * either from the StatusCode enumeration or from ImageStatusCode enumeration.
 */
class ImageListener : public Listener
{

public:
   /**
    * Constructor.
    */
   ImageListener();

   /**
    * Destructor.
    */
   virtual ~ImageListener();

   /**
    * Called when an image request, which are sent by calling getImage in 
    * ImageInterface, returns with the image written to file.
    *
    * @param requestID The identification of the getImage request
    *                  sent from the ImageInterface.
    * @param imageName The name of the image.
    */
   virtual void imageReply(RequestID requestID,
                           const WFString& imageName) = 0;
   
   /**
    * Called when an image request, which are sent by calling getImage in 
    * ImageInterface, returns with the image.
    *
    * @param requestID The identification of the getImage request
    *                  sent from the ImageInterface.
    * @param imageName The name of the image.
    * @param imageReplyData The image buffer.
    */
   virtual void imageReply(RequestID requestID,
                           const WFString& imageName,
                           const ImageReplyData& imageReplyData) = 0;

};

#endif // End WFAPI_IMAGE_LISTENER_H

} // End namespace WFAPI
