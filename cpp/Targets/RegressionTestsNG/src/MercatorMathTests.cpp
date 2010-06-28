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

#define CORE_LOGGING_MODULE_NAME "MercatorMathTests"
#define CORE_LOGGING_MODULE_LEVEL CL_ALL_FILE
#include "CoreDebugPrint.h"
#include "ImageMap/ImageMapGrid.h"
#include "ImageMap/ImageMapGridConverter.h"
#include "MercatorMathTests.h"
#include <iostream>

#define PRINT_VAR(x) std::cout << #x << " = " << x << std::endl

void MercatorMathTests::startTest()
{
   ImageMapGrid grid(15, 1,
                     -1280, -1280, 1024, 1024,
                     256, 2);

   ImageMapGrid grid_cmp(15, 1,
                         -1280, -1280, 1024, 1024,
                         256, 2);

   rtAssert(grid == grid_cmp);

   ImageMapGridConverter converter(grid);

   static const float MIN_SCALE = 0.0f;
   static const float MID_SCALE = 489.196991;
   static const float MAX_SCALE = 3231313123213.0f;
   
   ImageMapGridConverter::ConversionDetails minPoint =
      converter.getConversionDetails(converter.worldScaleToZoomIndex(MIN_SCALE));
   ImageMapGridConverter::ConversionDetails midPoint =
      converter.getConversionDetails(converter.worldScaleToZoomIndex(MID_SCALE));
   ImageMapGridConverter::ConversionDetails maxPoint =
      converter.getConversionDetails(converter.worldScaleToZoomIndex(MAX_SCALE));

   // coreprintln("minPoint: %d - %d", minPoint.zoomIndex, minPoint.translationFactor);
   // coreprintln("midPoint: %d - %d", midPoint.zoomIndex, midPoint.translationFactor);
   // coreprintln("maxPoint: %d - %d", maxPoint.zoomIndex, maxPoint.translationFactor);

   BoundingBox wholeMercator(MC2Point(MIN_INT32, MAX_INT32),
                          MC2Point(MAX_INT32, MIN_INT32));

   BoundingBox boxMinPoint = converter.worldToGrid(wholeMercator, MIN_SCALE).box;
   BoundingBox boxMidPoint = converter.worldToGrid(wholeMercator, MID_SCALE).box;
   BoundingBox boxMaxPoint = converter.worldToGrid(wholeMercator, MAX_SCALE).box;

   
   rtAssert(minPoint.zoomIndex == grid.getNumLevels() - 1);
   rtAssert(maxPoint.zoomIndex == 0);
   rtAssert(midPoint.zoomIndex == 5);
   
   rtAssert(boxMinPoint.topLeft.getX() == -20971520 &&
            boxMinPoint.topLeft.getY() ==  20971520 &&
            boxMinPoint.bottomRight.getX() == 20971520 &&
            boxMinPoint.bottomRight.getY() == -20971520);

   rtAssert(boxMidPoint.topLeft.getX() == -40960 &&
            boxMidPoint.topLeft.getY() ==  40960 &&
            boxMidPoint.bottomRight.getX() == 40960 &&
            boxMidPoint.bottomRight.getY() == -40960);

   rtAssert(boxMaxPoint.topLeft.getX() == -1280 &&
            boxMaxPoint.topLeft.getY() ==  1280 &&
            boxMaxPoint.bottomRight.getX() == 1280 &&
            boxMaxPoint.bottomRight.getY() == -1280);


   setTestPassed();
}
