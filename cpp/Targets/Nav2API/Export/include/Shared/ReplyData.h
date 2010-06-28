/*
  Copyright (c) 1999 - 2010, Vodafone Group Services Ltd
  All rights reserved.

  Redistribution and use in source and binary forms, with or without modification, are permitted provided that the following conditions are met:

  * Redistributions of source code must retain the above copyright notice, this list of conditions and the following disclaimer.
  * Redistributions in binary form must reproduce the above copyright notice, this list of conditions and the following disclaimer in the documentation and/or other materials provided with the distribution.
  * Neither the name of the Vodafone Group Services Ltd nor the names of its contributors may be used to endorse or promote products derived from this software without specific prior written permission.

  THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS" AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE ARE DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT HOLDER OR CONTRIBUTORS BE LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
*/

#ifndef WFAPI_REPLY_DATA_H
#define WFAPI_REPLY_DATA_H

#include "WFAPIConfig.h"
#include "WFString.h"
namespace WFAPI {

/**
 * Contains data information that is requested through the tunnel interface.
 */
class ReplyData
{
   
public:

   /**
    * Constructor.
    *
    * @param data The actual data.
    * @param size The size of this chunk of data.
    * @param totalDataSize The total size of data for a specific request.
    * @param complete If it is the last chunk of data for a specific request.
    */
   ReplyData(const char* data,
             wf_uint32 size,
             wf_uint32 totalDataSize,
             bool complete);
   
   /**
    * Destructor, the data is destroyed in the destructor.
    */
   virtual ~ReplyData();

   /**
    * Get a reference to the data.
    *
    * @return The data.
    */
   const char* getData() const;

   /**
    * Get the size of this chunk.
    *
    * @return The size of this chunk in bytes.
    */
   wf_uint32 getSize() const;

   /**
    * Get the size of the data.
    *
    * @return The total size of the data in bytes.
    */
   wf_uint32 getTotalDataSize() const;

   /**
    * If it is the last chunk of data for this request.
    *
    * @return If it is the last chunk of data.
    */
   bool isComplete();
   
private:

   /// The image buffer 
   const char* m_data;

   /// Size of this chunk.
   wf_uint32 m_size;

   /// The size of the data.
   wf_uint32 m_totalDataSize;

   /// If the the data is complete, by that means no more chunk is requiered.
   bool m_complete;
};

} // End namespace WFAPI

#endif // End WFAPI_REPLY_DATA_H
