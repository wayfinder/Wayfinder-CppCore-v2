/*
Copyright (c) 1999 - 2010, Vodafone Group Services Ltd
All rights reserved.

Redistribution and use in source and binary forms, with or without modification, are permitted provided that the following conditions are met:

    * Redistributions of source code must retain the above copyright notice, this list of conditions and the following disclaimer.
    * Redistributions in binary form must reproduce the above copyright notice, this list of conditions and the following disclaimer in the documentation and/or other materials provided with the distribution.
    * Neither the name of the Vodafone Group Services Ltd nor the names of its contributors may be used to endorse or promote products derived from this software without specific prior written permission.

THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS" AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE ARE DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT HOLDER OR CONTRIBUTORS BE LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
*/


#include "NetworkLinkID.h"

namespace WFAPI {

const wf_int32 NetworkLinkID::TRY_DEFAULT = wf_int32(-1);

const wf_int32 NetworkLinkID::ALWAYS_ASK = wf_int32(-2);

const wf_uint32 NetworkLinkID::INVALID_PROXY_PORT = WF_MAX_UINT32;

NetworkLinkID::NetworkLinkID()
   : m_id(TRY_DEFAULT),
     m_proxyPort(INVALID_PROXY_PORT),
     m_proxyHost()
{
}

NetworkLinkID::NetworkLinkID(wf_int32 id)
   : m_id(id),
     m_proxyPort(INVALID_PROXY_PORT),
     m_proxyHost()
{
}

NetworkLinkID::NetworkLinkID(wf_int32 id,
                             wf_uint32 proxyPort,
                             WFString proxyHost)
   : m_id(id),
     m_proxyPort(proxyPort),
     m_proxyHost(proxyHost)
{
}

NetworkLinkID::~NetworkLinkID()
{
}

wf_int32
NetworkLinkID::getID() const
{
   return m_id;
}

wf_uint32
NetworkLinkID::getProxyPort() const
{
   return m_proxyPort;
}

const WFString&
NetworkLinkID::getProxyHost() const
{
   return m_proxyHost;
}

void
NetworkLinkID::setID(wf_int32 id)
{
   m_id = id;
}

void
NetworkLinkID::setProxyPort(wf_uint32 proxyPort)
{
   m_proxyPort = proxyPort;
}

void
NetworkLinkID::setProxyHost(const WFString& proxyHost)
{
   m_proxyHost = proxyHost;
}

} // End namespace WFAPI


