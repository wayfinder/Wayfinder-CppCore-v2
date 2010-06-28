/*
Copyright (c) 1999 - 2010, Vodafone Group Services Ltd
All rights reserved.

Redistribution and use in source and binary forms, with or without modification, are permitted provided that the following conditions are met:

    * Redistributions of source code must retain the above copyright notice, this list of conditions and the following disclaimer.
    * Redistributions in binary form must reproduce the above copyright notice, this list of conditions and the following disclaimer in the documentation and/or other materials provided with the distribution.
    * Neither the name of the Vodafone Group Services Ltd nor the names of its contributors may be used to endorse or promote products derived from this software without specific prior written permission.

THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS" AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE ARE DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT HOLDER OR CONTRIBUTORS BE LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
*/


#ifndef _NAV2APISHAREDIMPL_H_
#define _NAV2APISHAREDIMPL_H_

#include "WFString.h"
#include "ProgramVersion.h"
#include "ConnectionManager.h"

namespace WFAPI {

/**
 * Shared struct containing data that could be set
 * by the client before calling start on Nav2API.
 * The data is shared between platforms and in this way
 * we get one shared storage for all the platforms.
 */
struct Nav2APISharedImpl {

   Nav2APISharedImpl() :
      m_httpProto(true),
      m_clientType("nav2-linux"),
      m_clientTypeOptions(""),
      m_httpRequestString(""),
      m_audioTypeDirName("Vorbis"),
      m_programVersion(0, 0, 0)
      { }

   /// True if using http protocol when talking to our server.
   bool m_httpProto;

   /// The client type for the client.
   WFString m_clientType;

   /// The client type options for the client.
   WFString m_clientTypeOptions;

   /// If client wants a different request string then the default one Nav.
   WFString m_httpRequestString;

   /// The name of the folder where the audio are stored.
   WFString m_audioTypeDirName;

   /// The version, can be set by the client.
   ProgramVersion m_programVersion;

};

}

#endif /* _NAV2APISHAREDIMPL_H_ */
