/*
Copyright (c) 1999 - 2010, Vodafone Group Services Ltd
All rights reserved.

Redistribution and use in source and binary forms, with or without modification, are permitted provided that the following conditions are met:

    * Redistributions of source code must retain the above copyright notice, this list of conditions and the following disclaimer.
    * Redistributions in binary form must reproduce the above copyright notice, this list of conditions and the following disclaimer in the documentation and/or other materials provided with the distribution.
    * Neither the name of the Vodafone Group Services Ltd nor the names of its contributors may be used to endorse or promote products derived from this software without specific prior written permission.

THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS" AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE ARE DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT HOLDER OR CONTRIBUTORS BE LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
*/


#include "RegressionTestErrorPrinting.h"

void dumpError(const WFAPI::SynchronousStatus& status)  {
#ifdef __linux__
   time_t tt;
   struct tm* tm;

   ::time(&tt);
   tm = localtime(&tt);

   coreprintln_error("ERROR %10s %04d-%02d-%02d %02d:%02d:%02d",
                     "Time: ",
                     tm->tm_year + 1900, tm->tm_mon + 1, tm->tm_mday,
                     tm->tm_hour, tm->tm_min, tm->tm_sec);
#else
   
#endif
   coreprintln_error("ERROR %10s %s", "Reason: ", status.getStatusMessage().c_str());
   coreprintln_error("ERROR %10s %s", "URL: ", status.getStatusURL().c_str());
}

void dumpError(const WFAPI::AsynchronousStatus& status)
{
   const WFAPI::SynchronousStatus& syncStatus(status);
   dumpError(syncStatus);
   coreprintln_error("ERROR %10s reqId: %d | statusCode: %d",
                     "Async: ",
                     status.getRequestID().getID(), status.getStatusCode());
}

