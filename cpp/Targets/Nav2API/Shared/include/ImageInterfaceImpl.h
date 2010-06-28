/*
  Copyright (c) 1999 - 2010, Vodafone Group Services Ltd
  All rights reserved.

  Redistribution and use in source and binary forms, with or without modification, are permitted provided that the following conditions are met:

  * Redistributions of source code must retain the above copyright notice, this list of conditions and the following disclaimer.
  * Redistributions in binary form must reproduce the above copyright notice, this list of conditions and the following disclaimer in the documentation and/or other materials provided with the distribution.
  * Neither the name of the Vodafone Group Services Ltd nor the names of its contributors may be used to endorse or promote products derived from this software without specific prior written permission.

  THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS" AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE ARE DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT HOLDER OR CONTRIBUTORS BE LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
*/

#ifndef WFAPI_IMAGE_INTERFACE_IMPL_H
#define WFAPI_IMAGE_INTERFACE_IMPL_H

#include "WFAPIConfig.h"
#include "Interface.h"
#include "RequestID.h"
#include "WFString.h"
#include "AsynchronousStatus.h"
#include "InterfaceImpl.h"
#include "TmapImageObserver.h"
#include "TmapImageHandler.h"
#include "MessageSenderInterface.h"
#include <set>

namespace WFAPI {

// Forward declarations
   class ImageListener;
   class Nav2APIImpl;

/**
 * The behind the scenes class for handling image fetching.
 */
class ImageInterfaceImpl : public InterfaceImpl,
                           public TmapImageObserver,
                           public MessageSenderInterface
{
   
public:

   /**
    * Constructor.
    */
   ImageInterfaceImpl(Nav2APIImpl* api);
   
   
   /**
    * Destructor.
    */
   virtual ~ImageInterfaceImpl();
   
   /**
    * Send a asynchronous request for the image data for an image name.
    * The image names from SearchItems and SearchDescs do not
    * have image type extension so you need to add an appropriate one.
    * Example: Image name from a SearchItem is "restaurant" add
    * ".png" to it to make a full image name, "restaurant.png".
    *
    * @param imageName The name of the image to get, make sure the 
    *                  name has an file extension, like ".png".
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
    * @param path The path to where to store images.
    */
   void setImagePath(const WFString& imagePath);
   
   /**
    * Get path to where images are stored.
    *
    * @param path The path to where images are stored.
    */
   WFString& getImagePath();

   /**
    * Set the extension to use for images.
    *
    * @param path The extension to use.
    */
   void setImageExtension(const WFString& imageExtension);

   /**
    * Get the extension which are use for images.
    *
    * @param path The extension used for images.
    */
   WFString& getImageExtension();

   /**
    * Implementation of abstract function from InterfaceImpl.
    */
   //@{
   /**
    * Function called when sending reply from Nav2. This is called when
    * receiving replies from asynchronous requests.
    *
    * @param guiProtMess The returning data of the reply.
    * @param requestID The RequestID for the guiProtMess.
    */
   virtual void receiveAsynchronousReply(const isab::GuiProtMess& guiProtMess,
                                         RequestID requestID);
   //@}

   /**
    * Implementation of abstract functions from TmapImageObserver.
    */
   //@{
   /**
    * Function gets called by TmapImageHandler
    * when a requested image is downloaded.
    * 
    * @param imageName The name of the downloaded image
    */
   virtual void ImageReceived(const char* imageName);

   /**
    * Function gets called by TmapImageHandler
    * when a requested image is downloaded and will be returned as a Buffer.
    *
    * @param imageName The name of the downloaded image
    * @param imageBuffer The downloaded image buffer.
    * @param imageBufferLength Length of the downloaded buffer.
    */
   virtual void ImageReceived(const char* imageName,
                              const uint8* imageBuffer,
                              uint32 imageBufferLength);

   /**
    * Function gets called by TmapImageHandler
    * when a requested image failed to be downloaded.
    * @param imageName, The name of the image that failed to be downloaded
    */
   virtual void ImageReceivedError(const char* imageName);
   //@}

   /**
    * Implementation of abstract function from MessageSenderInterface.
    */
   //@{
   
   /**
    * Function for sending messages to Nav2. Will be called
    * by the dispatcher when sending search queries.
    *
    * @param message The message to be sent to Nav2/server.
    */
   virtual void sendMessage(isab::GuiProtMess& message);
   
   /**
    * Function for sending messages to Nav2. Will be called
    * by the dispatcher when sending search queries.
    *
    * @param message The message to be sent to Nav2/server.
    * @param messageReceiver The receiver that should receive the message.
    */
   virtual void sendMessage(isab::GuiProtMess& message,
                            GuiProtMessageReceiver* messageReceiver);
   
   /**
    * Function for sending messages to Nav2. Will be called
    * by the dispatcher when sending search queries.
    *
    * @param params The paramblock to be sent to Nav2/server.
    * @param navRequestType The request type of the message to send.
    */
   virtual void sendMessage(const isab::NParamBlock& params,
                            uint16 navRequestType);
   
   //@}
   
private:

   /// The path to where images are stored.
   WFString m_imagePath;

   /// The extension used for images.
   WFString m_imageExtension;
   
   /// Typedef for a set of listeners.
   typedef std::set<ImageListener*> ListenerCont;
   
   /// Set of listeners.
   ListenerCont m_listeners;

   /// ImageHandler that handles image fetching.
   TmapImageHandler* m_imageHandler;
   
   /// The requestID for this image fetching.
   RequestID m_requestID;

};
   
} // End namespace WFAPI

#endif // End WFAPI_IMAGE_INTERFACE_IMPL_H
