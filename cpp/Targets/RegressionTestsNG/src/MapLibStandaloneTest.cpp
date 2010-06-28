/*
Copyright (c) 1999 - 2010, Vodafone Group Services Ltd
All rights reserved.

Redistribution and use in source and binary forms, with or without modification, are permitted provided that the following conditions are met:

    * Redistributions of source code must retain the above copyright notice, this list of conditions and the following disclaimer.
    * Redistributions in binary form must reproduce the above copyright notice, this list of conditions and the following disclaimer in the documentation and/or other materials provided with the distribution.
    * Neither the name of the Vodafone Group Services Ltd nor the names of its contributors may be used to endorse or promote products derived from this software without specific prior written permission.

THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS" AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE ARE DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT HOLDER OR CONTRIBUTORS BE LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
*/


#include "MapLibStandaloneTest.h"

#define CORE_LOGGING_MODULE_NAME "MapLibStandalone"
#define CORE_LOGGING_MODULE_LEVEL CL_ALL
#include "CoreDebugPrint.h"

#include "MapLibInitTest.h"
#include "MapLibAPI.h"
#include "MapLibInitialConfig.h"
#include "Nav2API.h"
#include "WGS84Coordinate.h"
#include "TileMapUtil.h"
#include "MapLibNetworkContext.h"
#include "Environment.h"
#include "Thread.h"
#include "InternalConfig.h"
#include "MapOperationInterface.h"
#include "NullPlotter.h"

#if defined(__linux__)
#include <GtkFactory.h>
#elif defined(_WIN32_WCE)
#include <WinMobileFactory.h>
#elif defined(__SYMBIAN__)
# include <SymbianFactory.h>
#elif defined(IPHONE)
# include <IPhoneFactory.h>
#endif

using namespace WFAPI;

struct MapLibStandaloneTest::Impl {
   MapLibAPI* mapLib;
   MapLibNetworkContext* networkContext;
};

MapLibStandaloneTest::~MapLibStandaloneTest()
{
   delete m->mapLib;
   delete m->networkContext;
}

struct NamedCenter {
   NamedCenter(const char* name, WGS84Coordinate coord) :
      name(name), coord(coord) {}
   
   const char* name;
   WGS84Coordinate coord;
};

static NamedCenter cityCenters[] =
{
   NamedCenter("Malmo",          WGS84Coordinate(55.6057,12.9991)), 
   NamedCenter("Kalmar",         WGS84Coordinate(56.6694,16.3468)), 
   NamedCenter("Stockholm",      WGS84Coordinate(59.3327,18.066)), 
   NamedCenter("Copenhagen",     WGS84Coordinate(55.6828,12.5652)), 
   NamedCenter("Aarhus",         WGS84Coordinate(56.1578,10.2193)), 
   NamedCenter("Hamburg",        WGS84Coordinate(53.556,9.98905)), 
   NamedCenter("Bremen",         WGS84Coordinate(53.0534,8.79369)), 
   NamedCenter("Berlin",         WGS84Coordinate(52.5232,13.4096)), 
   NamedCenter("Brussel",        WGS84Coordinate(50.8417,4.34471)), 
   NamedCenter("Paris",          WGS84Coordinate(48.8544,2.35432)), 
   NamedCenter("London",         WGS84Coordinate(51.5062,-0.131634)), 
   NamedCenter("Birmingham",     WGS84Coordinate(52.4868,-1.88902)), 
   NamedCenter("Sheffield",      WGS84Coordinate(53.384,-1.46339)), 
   NamedCenter("Edinburgh",      WGS84Coordinate(55.9521,-3.1887)), 
   NamedCenter("Ottawa",         WGS84Coordinate(45.4378,-75.6923)), 
   NamedCenter("Washington",     WGS84Coordinate(38.8769,-77.0083)), 
   NamedCenter("Philadelphia",   WGS84Coordinate(39.9555,-75.1671)), 
   NamedCenter("New York",       WGS84Coordinate(40.7655,-73.9817)), 
   NamedCenter("Boston",         WGS84Coordinate(42.3637,-71.0622)), 
   NamedCenter("Ontario",        WGS84Coordinate(42.3259,-83.0296)), 
   NamedCenter("Chicago",        WGS84Coordinate(41.8846,-87.6445)), 
   NamedCenter("Kansas",         WGS84Coordinate(39.1131,-94.586)), 
   NamedCenter("Los Angeles",    WGS84Coordinate(34.0826,-118.234)), 
   NamedCenter("Oakland",        WGS84Coordinate(37.7825,-122.201)), 
   NamedCenter("Mexico City",    WGS84Coordinate(19.4448,-99.079)), 
   NamedCenter("Kingston",       WGS84Coordinate(18.0102,-76.790))
};


void MapLibStandaloneTest::initMaplib()
{
   DrawingContext* context = NullPlotter::createDrawingContext();
   
#if defined (NAV2_CLIENT_MAEMO) || defined (NAV2_CLIENT_I386)
   MapLibInitialConfig initialConfig;

   m->networkContext = GtkFactory::createHTTPContext();

   
   initialConfig.setDiskCacheSettings("./nav2data/tilemaps", 10*1024*1024);

   m->mapLib = GtkFactory::createMapLib(
      m->networkContext->getConnection(),
      context,
      initialConfig);
#elif defined(IPHONE)
   MapLibInitialConfig initialConfig;
   initialConfig.setDiskCacheSettings("/var/root/Documents", 10*1024*1024);

   m->mapLib = IPhoneFactory::createMapLib(
      NULL,
      NULL,
      initialConfig);
   
#elif defined(_WIN32_WCE)
   MapLibInitialConfig initialConfig;

   m->mapLib = WinMobileFactory::createMapLib(
      NULL,
      NULL, //No drawing context, probably run from console app
      initialConfig,
      0, 0);

#elif defined __SYMBIAN__
   MapLibInitialConfig initialConfig;
   
   m->mapLib = SymbianFactory::createMapLib(
      NULL,
      NULL, //No drawing context, probably run from console app
      initialConfig);
   delete context; // Maplib will not delete this for us...

#endif
    
}

void MapLibStandaloneTest::startTest()
{
   initMaplib();
   
   InternalConfig* internalConf = m->mapLib->getInternalConfig();
   MapOperationInterface* opInterface = m->mapLib->getMapOperationInterface();
   
   const int numPos = 10;

   unsigned int startTimeMs = TileMapUtil::currentTimeMillis();

   int curPos = 0;

   NamedCenter curCenter = cityCenters[curPos];

   bool done = false;
   
   while(TileMapUtil::currentTimeMillis() - startTimeMs < getTimeOutMs() &&
      curPos < numPos)
   {
      Environment::sleep(0);
      
      if(internalConf->isMapFullyLoaded()) {
         startTimeMs = TileMapUtil::currentTimeMillis();
         
         if(curPos==0) {
            coreprintln("Initial position fetched.");
         } else {
            coreprintln("%s now fetched.", curCenter.name);
            done = true;
         }
         
         if(curPos == numPos) {
            setTestPassed();
         } else {
            curCenter = cityCenters[curPos];
            curPos++;
            opInterface->setCenter(curCenter.coord);
         }
      }
   }

   if(done) {
      setTestPassed();
   } else {
      setTestFailed("Failed to download all of the maps.\n");
   }

}

MapLibStandaloneTest::MapLibStandaloneTest(Nav2Session& session, const char* name)
   : RegressionTest(name, 90000),
     m(new Impl)
{
   
}


