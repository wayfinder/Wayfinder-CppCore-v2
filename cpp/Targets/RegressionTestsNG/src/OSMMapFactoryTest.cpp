/*
Copyright (c) 1999 - 2010, Vodafone Group Services Ltd
All rights reserved.

Redistribution and use in source and binary forms, with or without modification, are permitted provided that the following conditions are met:

    * Redistributions of source code must retain the above copyright notice, this list of conditions and the following disclaimer.
    * Redistributions in binary form must reproduce the above copyright notice, this list of conditions and the following disclaimer in the documentation and/or other materials provided with the distribution.
    * Neither the name of the Vodafone Group Services Ltd nor the names of its contributors may be used to endorse or promote products derived from this software without specific prior written permission.

THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS" AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE ARE DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT HOLDER OR CONTRIBUTORS BE LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
*/

#define CORE_LOGGING_MODULE_NAME "OSMMapFactoryTest"
#define CORE_LOGGING_MODULE_LEVEL CL_ALL_FILE

#include "CoreDebugPrint.h"
#include "OSMMapFactoryTest.h"
#include "CoreDebugPrint.h"
#include <math.h>
#include "ImageMap/OSM/OSMMapParamFactory.h"

#define PR(val) std::cout << #val << " = " << val << std::endl
#define CMP(lhs, rhs) std::cout << #lhs << " == " << #rhs << " -> " << lhs << " == " << rhs << std::endl

/**
 *  The whole world extent.
 */ 
static const BoundingBox mercatorWhole(MC2Point(MIN_INT32, MAX_INT32),
                                       MC2Point(MAX_INT32, MIN_INT32));

/**
 *  The upper right quadrant of the mercator space.
 */ 
static const BoundingBox mercatorUpperRight(MC2Point(0, MAX_INT32),
                                            MC2Point(MAX_INT32, 0));

/**
 *   Completely zoomed out scale.
 */ 
static const float MAX_SCALE = 3231313123213.0f;
static const float MAX_SCALE_MINUS_ONE = 110692.0;

struct OSMMapFactoryTest::Impl {
   OSMMapParamFactory factory;
};

OSMMapFactoryTest::OSMMapFactoryTest(Nav2Session& session, const char* name) : 
   RegressionTest(name)
{
   m_impl = new Impl;
}


void OSMMapFactoryTest::testWorldToGridWhole()
{
   vector<MapParam> res;

   /**
    * Start by finding the request strings for bounding boxes on the
    * highest zoom level. Should always result in a single result.
    */
   m_impl->factory.getParamsFromWorldBox(res, mercatorWhole, MAX_SCALE);
   rtAssert(res.size() == 1);
   rtAssert(find(res.begin(), res.end(), "0/0/0.png") != res.end());
   res.clear();
   m_impl->factory.getParamsFromWorldBox(res, mercatorUpperRight, MAX_SCALE);

   // for(unsigned int i = 0; i < res.size(); i++) {
   //    cout << "MapParam[" << i << "] = " << res[i].getString() << endl;
   // }

   rtAssert(res.size() == 1);
   rtAssert(find(res.begin(), res.end(), "0/0/0.png") != res.end());

   res.clear();
   m_impl->factory.getParamsFromWorldBox(res, mercatorWhole, MAX_SCALE_MINUS_ONE);

   rtAssert(res.size() == 4);
   
   rtAssert(find(res.begin(), res.end(), "1/0/0.png") != res.end());
   rtAssert(find(res.begin(), res.end(), "1/0/1.png") != res.end());
   rtAssert(find(res.begin(), res.end(), "1/1/0.png") != res.end());
   rtAssert(find(res.begin(), res.end(), "1/1/1.png") != res.end());

   res.clear();
   m_impl->factory.getParamsFromWorldBox(res,
                                         mercatorUpperRight,
                                         MAX_SCALE_MINUS_ONE);

   // for(unsigned int i = 0; i < res.size(); i++) {
   //    cout << "MapParam[" << i << "] = " << res[i].getString() << endl;
   // }

   rtAssert(res.size() == 1);

   rtAssert(find(res.begin(), res.end(), "1/1/0.png") != res.end());
   
   /**
    *  Test offsetting by one pixel
    */
   GridBox gbMO = {BoundingBox(UPoint(127, 256), UPoint(256, 128)), 0};

   res.clear();
   m_impl->factory.getParamsFromGridBox(res, gbMO);

   // A box offset by one pixels should result in two tiles being requested.
   rtAssert(res.size() == 2);
}

void OSMMapFactoryTest::testWorldToGridSteps()
{
   /**
    *  Insert ASCII art here
    */ 
   GridBox gbWhole = m_impl->factory.getGridBox(mercatorWhole, MAX_SCALE);
   m_impl->factory.offsetGridBox(gbWhole.box, gbWhole.zoomIndex);
   
   rtAssert(gbWhole.box.topLeft.getX() == 0);
   rtAssert(gbWhole.box.topLeft.getY() == 256);
   rtAssert(gbWhole.box.bottomRight.getX() == 256);
   rtAssert(gbWhole.box.bottomRight.getY() == 0);

   m_impl->factory.invertGridBoxYAxis(gbWhole);
   
   rtAssert(gbWhole.box.topLeft.getX() == 0);
   rtAssert(gbWhole.box.topLeft.getY() == 256);
   rtAssert(gbWhole.box.bottomRight.getX() == 256);
   rtAssert(gbWhole.box.bottomRight.getY() == 0);

   /**
    *  Insert ASCII art here
    */ 
   GridBox gbMini = m_impl->factory.getGridBox(mercatorUpperRight, MAX_SCALE);
   m_impl->factory.offsetGridBox(gbMini.box, gbMini.zoomIndex);

   // cout << "gbMini: " << gbMini << endl;
   
   rtAssert(gbMini.box.topLeft.getX() == 128);
   rtAssert(gbMini.box.topLeft.getY() == 256);
   rtAssert(gbMini.box.bottomRight.getX() == 256);
   rtAssert(gbMini.box.bottomRight.getY() == 128);   

   m_impl->factory.invertGridBoxYAxis(gbMini);

   rtAssert(gbMini.box.topLeft.getX() == 128);
   rtAssert(gbMini.box.topLeft.getY() == 128);
   rtAssert(gbMini.box.bottomRight.getX() == 256);
   rtAssert(gbMini.box.bottomRight.getY() == 0);
}

void OSMMapFactoryTest::startTest() {
   vector<MapParam> res;

   testWorldToGridSteps();
   testWorldToGridWhole();
   testGridToWorldSteps();
   testGridToWorldWhole();

   setTestPassed();
}

OSMMapFactoryTest::~OSMMapFactoryTest() {
   delete m_impl;
}

void OSMMapFactoryTest::testGridToWorldSteps()
{
   BoundingBox w = m_impl->factory.getWorldBoxForParam(0, 0, 0);  
   
   rtAssert(w.topLeft.getX() == MIN_INT32 + 1);
   rtAssert(w.topLeft.getY() == MAX_INT32);
   rtAssert(w.bottomRight.getX() == MAX_INT32);
   rtAssert(w.bottomRight.getY() == MIN_INT32 + 1);

   BoundingBox w1 = m_impl->factory.getWorldBoxForParam(0, 0, 1);
   BoundingBox w2 = m_impl->factory.getWorldBoxForParam(1, 0, 1);
   BoundingBox w3 = m_impl->factory.getWorldBoxForParam(1, 1, 1);  
   BoundingBox w4 = m_impl->factory.getWorldBoxForParam(0, 1, 1);
   
   rtAssert(w1.bottomRight == UPoint(0, 0));
   rtAssert(w2.topLeft.getX() == 0);
   rtAssert(w2.bottomRight.getY() == 0);
   rtAssert(w3.topLeft == UPoint(0, 0));
   rtAssert(w4.topLeft.getY() == 0);
   rtAssert(w4.bottomRight.getX() == 0);

   rtAssert(w1.topLeft.getY() == w2.topLeft.getY());
   rtAssert(w3.bottomRight.getY() == w4.bottomRight.getY());
}

void OSMMapFactoryTest::testGridToWorldWhole()
{
   MapParam p = m_impl->factory.getParamFromString("2/1/0.png");

   rtAssert(p.getZoomIndex() == 2);
   
   rtAssert(p.getString() == "2/1/0.png");

   p = m_impl->factory.getParamFromString("0/0/0.png");

   rtAssert(p.getWorldBox().topLeft.getX() == MIN_INT32 + 1);
   rtAssert(p.getWorldBox().topLeft.getY() == MAX_INT32);
   rtAssert(p.getWorldBox().bottomRight.getX() == MAX_INT32);
   rtAssert(p.getWorldBox().bottomRight.getY() == MIN_INT32 + 1);
   
   MapParam p1 = m_impl->factory.getParamFromString("1/0/0.png");
   MapParam p2 = m_impl->factory.getParamFromString("1/1/0.png");
   MapParam p3 = m_impl->factory.getParamFromString("1/1/1.png");
   MapParam p4 = m_impl->factory.getParamFromString("1/0/1.png");

   BoundingBox w1 = p1.getWorldBox();
   BoundingBox w2 = p2.getWorldBox();
   BoundingBox w3 = p3.getWorldBox();
   BoundingBox w4 = p4.getWorldBox();
   
   rtAssert(w1.bottomRight == UPoint(0, 0));
   rtAssert(w2.topLeft.getX() == 0);
   rtAssert(w2.bottomRight.getY() == 0);
   rtAssert(w3.topLeft == UPoint(0, 0));
   rtAssert(w4.topLeft.getY() == 0);
   rtAssert(w4.bottomRight.getX() == 0);

   rtAssert(p1.getZoomIndex() == 1);
   rtAssert(p2.getZoomIndex() == 1);
   rtAssert(p3.getZoomIndex() == 1);
   rtAssert(p4.getZoomIndex() == 1);
   
   rtAssert(w1.topLeft.getY() == w2.topLeft.getY());
   rtAssert(w3.bottomRight.getY() == w4.bottomRight.getY());
}


