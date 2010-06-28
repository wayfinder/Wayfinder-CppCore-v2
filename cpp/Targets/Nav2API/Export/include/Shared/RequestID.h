/*
  Copyright (c) 1999 - 2010, Vodafone Group Services Ltd
  All rights reserved.

  Redistribution and use in source and binary forms, with or without modification, are permitted provided that the following conditions are met:

  * Redistributions of source code must retain the above copyright notice, this list of conditions and the following disclaimer.
  * Redistributions in binary form must reproduce the above copyright notice, this list of conditions and the following disclaimer in the documentation and/or other materials provided with the distribution.
  * Neither the name of the Vodafone Group Services Ltd nor the names of its contributors may be used to endorse or promote products derived from this software without specific prior written permission.

  THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS" AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE ARE DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT HOLDER OR CONTRIBUTORS BE LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
*/

#ifndef WFAPI_REQUEST_ID_H
#define WFAPI_REQUEST_ID_H

#include "WFAPIConfig.h"

namespace WFAPI {

/**
 * The identifier of an asynchronous request. Used to identify what request
 * a reply corresponds to.
 */
class RequestID
{

public:

   /// The invalid RequestID.
   static const wf_uint32 INVALID_REQUEST_ID;

   /**
    * Constructs a new RequestID.
    */
   RequestID( wf_uint32 id );

   /**
    * Destructor.
    */
   virtual ~RequestID();

   /**
    * Get the identification number.
    */
   wf_uint32 getID() const;

   /**
    * Returns true if o.getID() == this->getID().
    */
   bool operator==( const RequestID& o ) const;

   /**
    * Returns true if o.getID() != this->getID().
    */
   bool operator!=( const RequestID& o ) const;

   /**
    * Returns true if this.getID() < o.getID()
    */
   bool operator<(const RequestID& o) const;

private:

   /// The request id.
   wf_uint32 m_id;
};

} // End namespace WFAPI

#endif // End WFAPI_REQUEST_ID_H
