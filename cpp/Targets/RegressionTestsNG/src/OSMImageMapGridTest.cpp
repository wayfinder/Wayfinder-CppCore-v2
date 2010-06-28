/*
Copyright (c) 1999 - 2010, Vodafone Group Services Ltd
All rights reserved.

Redistribution and use in source and binary forms, with or without modification, are permitted provided that the following conditions are met:

    * Redistributions of source code must retain the above copyright notice, this list of conditions and the following disclaimer.
    * Redistributions in binary form must reproduce the above copyright notice, this list of conditions and the following disclaimer in the documentation and/or other materials provided with the distribution.
    * Neither the name of the Vodafone Group Services Ltd nor the names of its contributors may be used to endorse or promote products derived from this software without specific prior written permission.

THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS" AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE ARE DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT HOLDER OR CONTRIBUTORS BE LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
*/

#include "config.h"
#include "OSMImageMapGridTest.h"
#include "ImageMap/ImageMapGrid.h"
#include "ImageMap/ImageMapGridConverter.h"

struct OSMImageMapGridTest::Impl {
   
};

OSMImageMapGridTest::OSMImageMapGridTest(Nav2Session& session, const char* name) : 
   RegressionTest(name)
{
   m_impl = new Impl;
}

void OSMImageMapGridTest::startTest() {
   static const float MAX_SCALE = 3231313123213.0f;
   
   ImageMapGrid grid(18, 0,
                     -128, -128, -128, -128,
                     256, 2);

   ImageMapGridConverter converter(grid);

   BoundingBox wholeMercator(MC2Point(MIN_INT32, MAX_INT32),
                             MC2Point(MAX_INT32, MIN_INT32));

   BoundingBox boxMaxPoint = converter.worldToGrid(wholeMercator,
                                                   MAX_SCALE).box;

   rtAssert(boxMaxPoint.topLeft.getX() == -128);
   rtAssert(boxMaxPoint.topLeft.getY() ==  128);
   rtAssert(boxMaxPoint.bottomRight.getX() ==  128);
   rtAssert(boxMaxPoint.bottomRight.getY() ==  -128);
   
   setTestPassed();
}

OSMImageMapGridTest::~OSMImageMapGridTest() {
   delete m_impl;
}
