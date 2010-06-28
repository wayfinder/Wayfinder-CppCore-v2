/*
  Copyright (c) 1999 - 2010, Vodafone Group Services Ltd
  All rights reserved.

  Redistribution and use in source and binary forms, with or without modification, are permitted provided that the following conditions are met:

  * Redistributions of source code must retain the above copyright notice, this list of conditions and the following disclaimer.
  * Redistributions in binary form must reproduce the above copyright notice, this list of conditions and the following disclaimer in the documentation and/or other materials provided with the distribution.
  * Neither the name of the Vodafone Group Services Ltd nor the names of its contributors may be used to endorse or promote products derived from this software without specific prior written permission.

  THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS" AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE ARE DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT HOLDER OR CONTRIBUTORS BE LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
*/

#ifndef WFAPI_IMAGE_INTERFACE_H
#define WFAPI_IMAGE_INTERFACE_H

#include "WFAPIConfig.h"
#include "Interface.h"
#include "RequestID.h"
#include "WFString.h"
#include "AsynchronousStatus.h"

namespace WFAPI {

// Forward declarations
class ImageListener;
class Nav2APIImpl;
class ImageInterfaceImpl;

/**
 * Interface class for image fetching. The image names from 
 * SearchItems and SearchDescs can be used in this interface
 * to download the actual images.
 */
class ImageInterface : public Interface
{

public:

   /**
    * Destructor.
    */
   virtual ~ImageInterface();

   /**
    * Send a asynchronous request for the image data for an image name.
    * The image names from SearchItems and SearchDescs do not
    * have image type extension so you need to add an appropriate one.
    * Example: Image name from a SearchItem is "restaurant" add
    * ".png" to it to make a full image name, "restaurant.png".
    *
    * @param imageName The name of the image to get, make sure the 
    *                  name has an file extension, like ".png".
    * @param getAsBuffer True if the image should be stored in the memory as
    *                    a buffer (uint8*). False if it should be written
    *                    to file.
    * @return The status and if status is OK the RequestID that will be
    *         returned in the imageListeners imageReply function.
    */
   AsynchronousStatus getImage(const WFString& imageName, bool getAsBuffer);

   /**
    * Add a listener for image replies.
    * @see ImageListener
    *
    * @param listener The ImageListener to add. 
    *                 It may not be deleted until it is removed by
    *                 calling removeImageListener.
    */
   void addImageListener(ImageListener* listener);

   /**
    * Remove a listener.
    *
    * @param listener The ImageListener to add. remove
    *                 It should now be deleted by caller, unless caller
    *                 keeps it around for later use.
    */
   void removeImageListener(ImageListener* listener);

   /**
    * Set path for where to store images.
    *
    * @param imagePath The path to where to store images.
    */
   void setImagePath(const WFString& imagePath);
   
   /**
    * Get path to where images are stored.
    *
    * @return The path to where images are stored.
    */
   WFString& getImagePath();

   /**
    * Set the extension to use for images.
    *
    * @return The extension to use.
    */
   void setImageExtension(const WFString& imageExtension);

   /**
    * Get the extension which are use for images.
    *
    * @return The extension used for images.
    */
   WFString& getImageExtension();

   /**
    * Internal function.
    */
   ImageInterfaceImpl* getImpl();

private:
   /**
    * Do not create Imageinterface get it from the Nav2API class.
    */
   ImageInterface(Nav2APIImpl* nav2APIimpl);

   /**
    * We do not allow copies of this interface.
    */
   ImageInterface(const ImageInterface&);
   const ImageInterface& operator=(const ImageInterface&);
   
   friend class Nav2APIImpl;

   struct impl;

   impl* m_impl;
};

} // End namespace WFAPI

#endif // End WFAPI_IMAGE_INTERFACE_H

