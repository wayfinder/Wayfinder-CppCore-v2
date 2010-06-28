/*
Copyright (c) 1999 - 2010, Vodafone Group Services Ltd
All rights reserved.

Redistribution and use in source and binary forms, with or without modification, are permitted provided that the following conditions are met:

    * Redistributions of source code must retain the above copyright notice, this list of conditions and the following disclaimer.
    * Redistributions in binary form must reproduce the above copyright notice, this list of conditions and the following disclaimer in the documentation and/or other materials provided with the distribution.
    * Neither the name of the Vodafone Group Services Ltd nor the names of its contributors may be used to endorse or promote products derived from this software without specific prior written permission.

THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS" AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE ARE DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT HOLDER OR CONTRIBUTORS BE LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
*/

#include "ProjectionInterfaceTest.h"
#include "MapModel.h"
#include "MC2Coordinate.h"
#include "MapProjection/ProjectionCoordinate.h"
#include "MapProjection/ProjectionInterface.h"
#include "BufferRequester/BufferRequesterSettings.h"
#include "Window/AppWindow.h"
#include "Window/AppWindowFactory.h"
#include "MapProjection/MapProjection.h"

struct ProjectionInterfaceTest::Impl {
   AppWindow* window;
};

ProjectionInterfaceTest::ProjectionInterfaceTest(Nav2Session& session, const char* name) : 
   RegressionTest(name)
{
   m_impl = new Impl;
   m_impl->window = AppWindowFactory::createWindow();
}

void ProjectionInterfaceTest::startTest() {
   BufferRequesterSettings modelSettings;
   MapModel model(modelSettings, NULL, m_impl->window->getDrawingContext());

   WFAPI::WGS84Coordinate paddingtonWGS(51.515713, -0.17503);

   
   MC2Coordinate paddington = paddingtonWGS;
   MercatorCoord paddingtonMerc = MercatorCoord(paddington);
   MC2Coordinate paddingtonMC2Merc(paddingtonMerc.lat, paddingtonMerc.lon);
   
   model.getMapProjection().setCenter(paddingtonMC2Merc);

   unsigned int width = m_impl->window->getWidthPixels();
   unsigned int height = m_impl->window->getHeightPixels();

   // printf("width = %u, height = %u\n", width, height);

   ProjectionCoordinate pCoord;
   ProjectionInterface& interface = model.getProjectionInterface();

   interface.getProjectionCoordinate(paddingtonWGS.latDeg,
                                     paddingtonWGS.lonDeg,
                                     &pCoord);

   float x, y, z;
   interface.projectf(pCoord, &x, &y, &z);

   // printf("x = %f, y = %f, z = %f\n", x, y, z);
   
   rtAssertEq(static_cast<unsigned int>(x), width / 2);
   rtAssertEq(static_cast<unsigned int>(y), height / 2);

   interface.getProjectionCoordinate(paddingtonWGS.latDeg,
                                     paddingtonWGS.lonDeg,
                                     &pCoord);
   
   setTestPassed();
}

ProjectionInterfaceTest::~ProjectionInterfaceTest() {
   delete m_impl;
}
