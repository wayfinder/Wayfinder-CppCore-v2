/*
Copyright (c) 1999 - 2010, Vodafone Group Services Ltd
All rights reserved.

Redistribution and use in source and binary forms, with or without modification, are permitted provided that the following conditions are met:

    * Redistributions of source code must retain the above copyright notice, this list of conditions and the following disclaimer.
    * Redistributions in binary form must reproduce the above copyright notice, this list of conditions and the following disclaimer in the documentation and/or other materials provided with the distribution.
    * Neither the name of the Vodafone Group Services Ltd nor the names of its contributors may be used to endorse or promote products derived from this software without specific prior written permission.

THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS" AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE ARE DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT HOLDER OR CONTRIBUTORS BE LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
*/

#define CORE_LOGGING_MODULE_NAME "WFMapParamFactoryTest"
#define CORE_LOGGING_MODULE_LEVEL CL_ALL_FILE
#include "CoreDebugPrint.h"
#include <math.h>
#include "WFMapParamFactoryTest.h"
#include "ImageMap/WF/WFMapParamFactory.h"

#define PR(val) std::cout << #val << " = " << val << std::endl
#define CMP(lhs, rhs) std::cout << #lhs << " == " << #rhs << " -> " << lhs << " == " << rhs << std::endl


struct WFMapParamFactoryTest::Impl {
   WFMapParamFactory factory;
};

WFMapParamFactoryTest::WFMapParamFactoryTest(Nav2Session& session, const char* name) : 
   RegressionTest(name)
{
   m_impl = new Impl;
}

static const float MAX_SCALE_MINUS_ONE = 15653;

void WFMapParamFactoryTest::startTest() {   
   vector<MapParam> res;

   /**
    *  Start by examining the whole world extent to
    *  check corner cases.
    */ 
   BoundingBox mercatorWhole(MC2Point(MIN_INT32, MAX_INT32),
                             MC2Point(MAX_INT32, MIN_INT32));
   
   m_impl->factory.getParamsFromWorldBox(res,
                                         mercatorWhole,
                                         24000.0f);

   // cout << "res.size() = " << res.size() << endl;
   
   rtAssert(res.size() == 100);

   rtAssert(find(res.begin(), res.end(),
                 "LMMap?x=1024&y=1024&zoom=1") != res.end());

   rtAssert(find(res.begin(), res.end(),
                 "LMMap?x=-1280&y=-1280&zoom=1") != res.end());

   rtAssert(find(res.begin(), res.end(),
                 "LMMap?x=1024&y=1280&zoom=1") == res.end());
   
   rtAssert(find(res.begin(), res.end(),
                 "LMMap?x=1280&y=1280&zoom=1") == res.end());   

   /**
    *  Then move to the upper right quadrant to do single tile
    *  checking.
    */ 
   BoundingBox mercatorMini(MC2Point(0, MAX_INT32 / 8),
                            MC2Point(MAX_INT32 / 8, 0));

   res.clear();
   m_impl->factory.getParamsFromWorldBox(res, mercatorMini, 24000.0f);

   rtAssert(res.size() == 1);
   rtAssert(res[0] == "LMMap?x=0&y=0&zoom=1");

   /**
    *  Test the inverse.
    */ 
   BoundingBox b =
      m_impl->factory.getParamFromString("LMMap?x=0&y=0&zoom=1").getWorldBox();
   
   // cout << "BoundingBox: " << b << std::endl;
   
   BoundingBox b2 =
      m_impl->factory.getParamFromString("LMMap?x=512&y=1024&zoom=1").getWorldBox();

   // This one shold be in image map coord space: 
   // Upper left: (512,1280)
   // Lower right: (768,1024)
   int expectedLRY = (int) rint((MAX_INT32 / (double)1280) * 1024);
   
   rtAssert(b2.bottomRight.getY() == expectedLRY);
   rtAssert(b2.topLeft.getY() == MAX_INT32);
   
   /**
    *  Offset slightly to make sure that the same tiles are
    *  covered.
    */ 
   BoundingBox mercatorMiniOffset(MC2Point(1, MAX_INT32 / 8),
                                  MC2Point(MAX_INT32 / 8, 1));

   res.clear();
   m_impl->factory.getParamsFromWorldBox(res, mercatorMiniOffset, 24000.0f);

   rtAssert(res.size() == 1);
   rtAssert(res[0] == "LMMap?x=0&y=0&zoom=1");

   /**
    *   Zoom in to the next level
    */
   res.clear();
   m_impl->factory.getParamsFromWorldBox(res, mercatorMini, 11000.0f);
   rtAssert(res.size() == 4);

   res.clear();
   m_impl->factory.getParamsFromWorldBox(res, mercatorMiniOffset, 11000.0f);
   rtAssert(res.size() == 4);

   // for(unsigned int i = 0; i < res.size(); i++) {
   //    coreprintln("[%d]: Param: %s", i, res[i].getString().c_str());
   // }
   
   setTestPassed();
}

WFMapParamFactoryTest::~WFMapParamFactoryTest() {
   delete m_impl;
}
