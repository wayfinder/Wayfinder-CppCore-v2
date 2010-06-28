/*
Copyright (c) 1999 - 2010, Vodafone Group Services Ltd
All rights reserved.

Redistribution and use in source and binary forms, with or without modification, are permitted provided that the following conditions are met:

    * Redistributions of source code must retain the above copyright notice, this list of conditions and the following disclaimer.
    * Redistributions in binary form must reproduce the above copyright notice, this list of conditions and the following disclaimer in the documentation and/or other materials provided with the distribution.
    * Neither the name of the Vodafone Group Services Ltd nor the names of its contributors may be used to endorse or promote products derived from this software without specific prior written permission.

THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS" AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE ARE DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT HOLDER OR CONTRIBUTORS BE LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
*/


#define CORE_LOGGING_MODULE_NAME "MAPLIB_INIT_TEST"

#include "MapLibInitTest.h"
#include "MapLibAPI.h"
#include "MapLibInitialConfig.h"
#include "Nav2Session.h"
#include "Nav2API.h"
#include "CoreDebugPrint.h"

#if defined(__linux__)
#include <GtkFactory.h>
#elif defined(_WIN32_WCE)
#include <WinMobileFactory.h>
#elif defined(__SYMBIAN__)
# include <SymbianFactory.h>
#elif defined(IPHONE)
# include <IPhoneFactory.h>
#endif

#include "RegressionTestErrorPrinting.h"

using namespace WFAPI;

void MapLibInitTest::createMapLib()
{
#if defined (NAV2_CLIENT_MAEMO) || defined (NAV2_CLIENT_I386)
   MapLibInitialConfig initialConfig;
   initialConfig.setDiskCacheSettings("./nav2data/tilemaps", 10*1024*1024);

   m_mapLib = GtkFactory::createMapLib(
      m_session.getNav2()->getDBufConnection(),
      NULL, //No drawing context, probably run from console app
      initialConfig);

   m_session.getNav2()->connectMapLib(m_mapLib);
#elif defined(IPHONE)
   MapLibInitialConfig initialConfig;
   initialConfig.setDiskCacheSettings("/var/root/Documents", 10*1024*1024);

   m_mapLib = IPhoneFactory::createMapLib(
      m_session.getNav2()->getDBufConnection(),
      NULL,
      initialConfig);

   m_session.getNav2()->connectMapLib(m_mapLib);
   
#elif defined(_WIN32_WCE)
   MapLibInitialConfig initialConfig;

   m_mapLib = WinMobileFactory::createMapLib(
      m_session.getNav2()->getDBufConnection(),
      NULL, //No drawing context, probably run from console app
      initialConfig,
      0, 0);

   m_session.getNav2()->connectMapLib(m_mapLib);

#elif defined __SYMBIAN__
   MapLibInitialConfig initialConfig;
   
   m_mapLib = SymbianFactory::createMapLib(
      m_session.getNav2()->getDBufConnection(),
      NULL, //No drawing context, probably run from console app
      initialConfig);

   m_session.getNav2()->connectMapLib(m_mapLib);
   
#endif
   
}

void MapLibInitTest::error(const WFAPI::AsynchronousStatus& status)
{
   dumpError(status);
   setTestFailed(status.getStatusMessage()); 
}
