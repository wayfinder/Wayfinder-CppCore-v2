/*
Copyright (c) 1999 - 2010, Vodafone Group Services Ltd
All rights reserved.

Redistribution and use in source and binary forms, with or without modification, are permitted provided that the following conditions are met:

    * Redistributions of source code must retain the above copyright notice, this list of conditions and the following disclaimer.
    * Redistributions in binary form must reproduce the above copyright notice, this list of conditions and the following disclaimer in the documentation and/or other materials provided with the distribution.
    * Neither the name of the Vodafone Group Services Ltd nor the names of its contributors may be used to endorse or promote products derived from this software without specific prior written permission.

THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS" AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE ARE DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT HOLDER OR CONTRIBUTORS BE LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
*/


#include "LinuxLocationInterfaceImpl.h"
#include "LocationInterface.h"
#include <iostream>

using namespace std;

namespace WFAPI {

LinuxLocationInterfaceImpl::
LinuxLocationInterfaceImpl(Nav2APIImpl* api) :
   LocationInterfaceImpl(api)
{
}

LinuxLocationInterfaceImpl::~LinuxLocationInterfaceImpl()
{
}

SynchronousStatus
LinuxLocationInterfaceImpl::lbsStarted()
{
   // Implement this if needed
   return SynchronousStatus(GENERAL_ERROR, "", "");
}


AsynchronousStatus
LinuxLocationInterfaceImpl::startLbs()
{
   // Test code:
   // Network UMTS signalStrength 6 mcc 240 mnc 8 lac 330 cellID 50141
   TGPPCellIDRequestData defaultCellIDRequest(
      "UMTS", "6"/*What units?*/, "0xF0"/*MCC 240 se*/, 
      "0x8"/*MNC*/, "0x14a"/*Phoney LAC*/, "0xC3DD"/*cellID*/);
   sendCellIDRequest(defaultCellIDRequest);
   
   // Implement this if needed
   return AsynchronousStatus(RequestID(RequestID::INVALID_REQUEST_ID),
                             GENERAL_ERROR, "", "");
}

SynchronousStatus
LinuxLocationInterfaceImpl::stopLbs()
{
   // Implement this if needed
   return SynchronousStatus(GENERAL_ERROR, "", "");
}

} // End namespace WFAPI
