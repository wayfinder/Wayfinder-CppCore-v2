/*
  Copyright (c) 1999 - 2010, Vodafone Group Services Ltd
  All rights reserved.

  Redistribution and use in source and binary forms, with or without modification, are permitted provided that the following conditions are met:

  * Redistributions of source code must retain the above copyright notice, this list of conditions and the following disclaimer.
  * Redistributions in binary form must reproduce the above copyright notice, this list of conditions and the following disclaimer in the documentation and/or other materials provided with the distribution.
  * Neither the name of the Vodafone Group Services Ltd nor the names of its contributors may be used to endorse or promote products derived from this software without specific prior written permission.

  THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS" AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE ARE DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT HOLDER OR CONTRIBUTORS BE LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
*/

#include "ImageInterfaceImpl.h"
#include "ImageListener.h"
#include "InterfaceImpl.h"
#include "AsynchronousStatus.h"
#include "SynchronousStatus.h"
#include "RequestID.h"
#include "ImageStatusCode.h"

// Include GuiProt
#include "GuiProtMess.h"
#include "GuiProtFavoriteMess.h"
#include "GuiProtEnums.h"

#include "Nav2Logging.h"

using namespace std;

namespace WFAPI {

ImageInterfaceImpl::ImageInterfaceImpl(Nav2APIImpl* api)
   : InterfaceImpl(api), m_requestID(RequestID::INVALID_REQUEST_ID)
{
   m_imageHandler = new TmapImageHandler(*this, *this);
}

ImageInterfaceImpl::~ImageInterfaceImpl()
{
   delete m_imageHandler;
}

void
ImageInterfaceImpl::addImageListener(ImageListener* listener)
{
   m_listeners.insert(listener);
}

void
ImageInterfaceImpl::removeImageListener(ImageListener* listener)
{
   m_listeners.erase(listener);
}

void
ImageInterfaceImpl::setImagePath(const WFString& imagePath)
{
   m_imagePath = imagePath;
}

WFString&
ImageInterfaceImpl::getImagePath()
{
   return m_imagePath;
}

void
ImageInterfaceImpl::setImageExtension(const WFString& imageExtension)
{
   m_imageExtension = imageExtension;
}

WFString&
ImageInterfaceImpl::getImageExtension()
{
   return m_imageExtension;
}

AsynchronousStatus
ImageInterfaceImpl::getImage(const WFString& imageName, bool getAsBuffer)
{
   char* completeFileName = new char[m_imagePath.length() +
                                     imageName.length() + 
                                     m_imageExtension.length() + 1];
   strcpy(completeFileName, m_imagePath.c_str());
   strcat(completeFileName, imageName.c_str());
   strcat(completeFileName, m_imageExtension.c_str());

   m_imageHandler->FetchImageUsingOldTMapInterface(completeFileName,
                                                   getAsBuffer);
   m_requestID = RequestID(RequestID::INVALID_REQUEST_ID);
   delete[] completeFileName;
   return AsynchronousStatus(m_requestID, OK, "", "");
}

void
ImageInterfaceImpl::receiveAsynchronousReply(const isab::GuiProtMess&
                                             guiProtMess,
                                             RequestID requestID)
{
   AsynchronousStatus status = createAsynchStatus(&guiProtMess, requestID);
   // Check status if OK.
   if (status.getStatusCode() != OK) {
      // Bad status. Report error function for all listeners.
      reportError(status, m_listeners.begin(), m_listeners.end());
      return;
   }
   m_imageHandler->GuiProtReceiveMessage(
      const_cast<isab::GuiProtMess*>(&guiProtMess));
}

void
ImageInterfaceImpl::ImageReceived(const char* imageName)
{
   
   ListenerCont::iterator itEnd = m_listeners.end();
   for(ListenerCont::iterator it = m_listeners.begin();
       it != itEnd; it++) {
      (*it)->imageReply(m_requestID, imageName);
   }
}

void
ImageInterfaceImpl::ImageReceived(const char* imageName,
                                  const uint8* imageBuffer,
                                  uint32 imageBufferLength)
{
   
   ListenerCont::iterator itEnd = m_listeners.end();
   for(ListenerCont::iterator it = m_listeners.begin();
       it != itEnd; it++) {
      (*it)->imageReply(m_requestID, imageName,
                        ImageReplyData(static_cast<const wf_uint8*>
                                       (imageBuffer),
                                       static_cast<wf_uint32>
                                       (imageBufferLength)));
   }
}

void
ImageInterfaceImpl::ImageReceivedError(const char* imageName)
{
   // No error handling. Just call imageReceived.
   ImageReceived(imageName);
}

void
ImageInterfaceImpl::sendMessage(isab::GuiProtMess& message)
{
   AsynchronousStatus status = sendAsynchronousRequestToNav2(&message);
   if(status.getStatusCode() != OK) {
      //Bad status, something went wrong.
      
      nav2log << "ImageInterface::sendMessage, bad status" << endl;
      reportError(status, m_listeners.begin(), m_listeners.end());
      return;
   }
   m_requestID = status.getRequestID();
}

void
ImageInterfaceImpl::sendMessage(isab::GuiProtMess& message,
                                GuiProtMessageReceiver* messageReceiver)
{
   sendMessage(message);
}

void ImageInterfaceImpl::sendMessage(const isab::NParamBlock& params,
                                            uint16 navRequestType)
{
   // Implement when needed.
}

} // End namespace WFAPI
