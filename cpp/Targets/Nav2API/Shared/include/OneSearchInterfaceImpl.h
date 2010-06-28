/*
  Copyright (c) 1999 - 2010, Vodafone Group Services Ltd
  All rights reserved.

  Redistribution and use in source and binary forms, with or without modification, are permitted provided that the following conditions are met:

  * Redistributions of source code must retain the above copyright notice, this list of conditions and the following disclaimer.
  * Redistributions in binary form must reproduce the above copyright notice, this list of conditions and the following disclaimer in the documentation and/or other materials provided with the distribution.
  * Neither the name of the Vodafone Group Services Ltd nor the names of its contributors may be used to endorse or promote products derived from this software without specific prior written permission.

  THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS" AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE ARE DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT HOLDER OR CONTRIBUTORS BE LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
*/

#ifndef WFAPI_ONE_SEARCH_INTERFACE_IMPL_H
#define WFAPI_ONE_SEARCH_INTERFACE_IMPL_H

#include "InterfaceImpl.h"
#include "RequestID.h"
#include "WFString.h"
#include "MessageSenderInterface.h"

#include "CSImageDimension.h"

namespace isab {
// Forward declarations
class GuiProtMess;
}

// Forward declarations
class GuiProtMessageReceiver;
// class CategoryTreeDispatcher;

namespace WFAPI {

class Nav2APIImpl;
class WFStringArray;
class CategoryTreeArray;
class SearchListener;
class WGS84Coordinate;
class OneSearchQuery;

/**
 * Interface for category tree. Uses the category tree when searching.
 * Getting category trees in different regions.
 */
class OneSearchInterfaceImpl : public InterfaceImpl,
                                  public MessageSenderInterface
                                  //public MSearchResultObserver
{
public:

   /**
    * Constructor.
    */ 
   OneSearchInterfaceImpl(Nav2APIImpl* api);

   /**
    * Destructor.
    */
   virtual ~OneSearchInterfaceImpl();

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
   void receiveAsynchronousReply(const isab::GuiProtMess& guiProtMess,
                                 RequestID requestID);
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

   /**
    * To add a listener for searches.
    * 
    * @param listener The SearchListener to add.
    *                 It may not be deleted until it is removed by
    *                 calling removeSearchListener.
    */
   void addSearchListener(SearchListener* listener);

   /**
    * To remove a listener.
    * 
    * @param listener The SearchListener to be removed.
    */
   void removeSearchListener(SearchListener* listener);

   /**
    * Sets the image path for the search result icons. Make sure this
    * path exists, if not no images will be stored.
    *
    * @param imagePath The path to where to store the images
    *                  fetched from the server.
    */
   void setImagePath(const WFString& imagePath);

   /**
    * Sets the image extension. 
    *
    * @param imageExt The image extension to use.
    */
   void setImageExt(const WFString& imageExt);

   /** 
    * Set the dimension of images that will be downloaded.
    * 
    * @param imageDimension The dimension if downloaded images.
    */
   void setImageDimension(CombinedSearch::CSImageDimension imageDimension);

   /**
    * This asynchronous function requests a category tree using a coord.
    *
    * @param coord For what region to request a category tree.
    * @return The status and if status is OK the RequestID that will be
    *         returned in the SearchListeners category tree reply function.
    */
   AsynchronousStatus sendSearchRequest(const OneSearchQuery& searchQuery);

private:
   /// Typedef for a set of listeners.
   // typedef std::set<SearchListener*> ListenerCont;
   
   // /// Set of listeners.
   // ListenerCont m_listeners;

   // /// The path to the category images folder.
   // WFString m_categoryImagePath;
   
   // /// The extension for the images to download.
   // WFString m_imageExt;

   /**
    * Category tree dispatcher that gets a category tree and parses results.
    */
   // CategoryTreeDispatcher* m_ctDispatcher;

   /**
    * Request id of the current request.
    * NOTE: This means we, theoratically, could
    * get a new request before the old one is handled. We would then
    * get in a state where the UI never can finish his first request.
    * This can be solved using a map, mapping a request against a requestID.
    */
   RequestID m_requestID;
   
   // /// The checksum for the currently stored categories.
   // wf_int32 m_categoryCrc;

   /// Message linking together a request id against a message sender.
   // typedef std::map<wf_uint32, GuiProtMessageReceiver*> MessageSenderMap;
   // MessageSenderMap m_messageReceivers;

   struct Impl;
   Impl* m_impl;
};

} // End namespace WFAPI

#endif // end WFAPI_CATEGORY_TREE_INTERFACE_IMPL_H
