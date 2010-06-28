/*
  Copyright (c) 1999 - 2010, Vodafone Group Services Ltd
  All rights reserved.

  Redistribution and use in source and binary forms, with or without modification, are permitted provided that the following conditions are met:

  * Redistributions of source code must retain the above copyright notice, this list of conditions and the following disclaimer.
  * Redistributions in binary form must reproduce the above copyright notice, this list of conditions and the following disclaimer in the documentation and/or other materials provided with the distribution.
  * Neither the name of the Vodafone Group Services Ltd nor the names of its contributors may be used to endorse or promote products derived from this software without specific prior written permission.

  THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS" AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE ARE DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT HOLDER OR CONTRIBUTORS BE LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
*/

#ifndef WFAPI_INTERFACE_H
#define WFAPI_INTERFACE_H

#include "WFAPIConfig.h"
#include "RequestID.h"

namespace WFAPI {

/**
 * The super class for all Nav2API interfaces.
 * The role of an Interface is to provide the functionality
 * of a specific area, for example searching.
 */
class Interface
{

public:

   /**
    * Destructor.
    */
   virtual ~Interface();

   /**
    * Cancel a sent asynchronous request using a RequestID.
    * Call this when you no longer want the reply for a request.
    * Once cancelled the request will not cause any callbacks even
    * if the API was about to make callbacks. But the request might be
    * made even if it is canceled.
    *
    * @param requestID The RequestID of the request to cancel.
    */
   virtual void cancelRequest(RequestID requestID);

protected:

   /**
    * Do not create Interface instances get one of the subclasses from the
    * Nav2API class.
    */
   Interface();
   
private:

   friend class Nav2API;
};

} // End namespace WFAPI

#endif // End WFAPI_INTERFACE_H
