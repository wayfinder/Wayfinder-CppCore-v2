/*
  Copyright (c) 1999 - 2010, Vodafone Group Services Ltd
  All rights reserved.

  Redistribution and use in source and binary forms, with or without modification, are permitted provided that the following conditions are met:

  * Redistributions of source code must retain the above copyright notice, this list of conditions and the following disclaimer.
  * Redistributions in binary form must reproduce the above copyright notice, this list of conditions and the following disclaimer in the documentation and/or other materials provided with the distribution.
  * Neither the name of the Vodafone Group Services Ltd nor the names of its contributors may be used to endorse or promote products derived from this software without specific prior written permission.

  THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS" AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE ARE DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT HOLDER OR CONTRIBUTORS BE LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
*/

#include "NokiaMaemoNav2API.h"
#include "WFString.h"
#include <string>
#include "JTC/JTCInit.h"

bool NetPMGConfGetString(WFAPI::WFString& imeiStr)
{
      // Fake imei for now. Should be read from system if available.
      imeiStr = "LinuxNav2APITestIMEI001";
      return true;
}

bool getIMSI(WFAPI::WFString& imsiStr)
{
      // Fake imsi for now. Should be read from system if available.
      imsiStr = "fake-imsi";
      return true;
}

namespace WFAPI {

NokiaMaemoNav2API::NokiaMaemoNav2API()
{
   JTCInit::initJTC();
}

NokiaMaemoNav2API::~NokiaMaemoNav2API()
{
}

SynchronousStatus
NokiaMaemoNav2API::makeHardwareIDs(HardwareIDArray& hwids) 
{
   bool ok = true;
   // IMEI
   WFString imeiStr;
   ok = NetPMGConfGetString(imeiStr);

   // IMSI
   WFString imsiStr;
   ok = ok && getIMSI(imsiStr);

   if (ok) {
      hwids.push_back(HardwareID(imeiStr, "imei"));
      hwids.push_back(HardwareID(imsiStr, "imsi"));

      return SynchronousStatus(OK, "", "");
   } else {
      return SynchronousStatus(GENERAL_ERROR, "", "");
   }
}


} // End namespace WFAPI
