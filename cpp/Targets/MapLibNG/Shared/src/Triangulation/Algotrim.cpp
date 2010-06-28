/*
Copyright (c) 1999 - 2010, Vodafone Group Services Ltd
All rights reserved.

Redistribution and use in source and binary forms, with or without modification, are permitted provided that the following conditions are met:

    * Redistributions of source code must retain the above copyright notice, this list of conditions and the following disclaimer.
    * Redistributions in binary form must reproduce the above copyright notice, this list of conditions and the following disclaimer in the documentation and/or other materials provided with the distribution.
    * Neither the name of the Vodafone Group Services Ltd nor the names of its contributors may be used to endorse or promote products derived from this software without specific prior written permission.

THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS" AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE ARE DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT HOLDER OR CONTRIBUTORS BE LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
*/


#include "Triangulation/Algotrim.h"
#include "Triangulation/ag_triangulate.h"
#include <stdlib.h>

#define CORE_LOGGING_MODULE_NAME "Algotrim"
// #define CORE_LOGGING_MODULE_LEVEL CL_ALL

#include "CoreDebugPrint.h"

#define INITIAL_MEM_SIZE 1000

namespace Triangulation {
namespace Algotrim {

// Our heap allocation caches
static std::vector<char> wmem(1000);
static std::vector<int> imem(1000);
static std::vector<ag_point> pres(1000);

int triangulatePolygon(CoordsArg::const_iterator begin,
                       CoordsArg::const_iterator end,
                       const MC2Coordinate& refCoord,
                       int color,
                       vec4x* output)
{
   // Iterate using regular pointer
   
   const MC2Coordinate* beginPtr = &(*begin);

   /*
    *  Calculate the dimensions that will be required of our
    *  temporary buffers.
    */ 
   unsigned int numPoints = std::distance(begin, end);

   unsigned int wmemNeeded = triangulate_poly_mem_usage(numPoints);

   unsigned int indicesNeeded = 3 * (numPoints - 2);

   /*
    *  If necessary, grow our heap allocation buffers to accomodate
    *  for the required data.
    */ 
   if(wmemNeeded > wmem.size()) {
      wmem.resize(wmemNeeded, 0);
   }

   if(indicesNeeded > imem.size()) {
      imem.resize(indicesNeeded);
   }

   const ag_point* mc2CastPoints = reinterpret_cast<const ag_point*>(beginPtr);
   
   /*
    *  Execute the actual triangulation
    */ 
   int ret = triangulate_poly(numPoints, mc2CastPoints, &wmem[0], &imem[0]);
      
   if(ret == 0) {
      return 0; 
   }

   /* 
    *  Loop over the triangulation result and output the data to the
    *  final storage type (vec4x).
    */ 
   for(unsigned int i = 0; i < indicesNeeded; i++) {

      const MC2Coordinate& p = beginPtr[imem[i]];
      
      vec4x tp = { vec3x( fixedp(fixedp::RAW,  p.lon - refCoord.lon ),
                          fixedp(fixedp::RAW,  p.lat - refCoord.lat ) )      
#ifdef VEC_3D_USE_COLOR_BUFFER
                    , color
#endif
      };

      *output++ = tp;
   }
   
   return ret;
}

int triangulateLine(CoordsArg::const_iterator begin,
                    CoordsArg::const_iterator end,
                    const MC2Coordinate& refCoord,
                    int multCosLat,
                    int multCosLatInv,
                    unsigned int widthMC2,
                    int color,
                    vec4x* output)
{   
   unsigned int numPoints = std::distance(begin, end);

   unsigned int resPoints = numPoints * 4;
   
   const ag_point* points = reinterpret_cast<const ag_point*>(&(*begin));

   if(resPoints > pres.size()) {
      pres.resize(resPoints);
   }

   if(triangulate_line(numPoints, points, multCosLat, multCosLatInv, widthMC2,
                       &pres[0]) == 0)
   {
      return 0;
   }
   
   const ag_point* res = &pres[0];
   
   /* 
    *  Loop over the triangulation result and output the data to the
    *  final storage type (vec4x).
    */ 
   for(unsigned int i = 0; i < resPoints; i++) {

                   
      vec4x tp = { vec3x( fixedp(fixedp::RAW,  res->x - refCoord.lon ),
                          fixedp(fixedp::RAW,  res->y - refCoord.lat) )      
#ifdef VEC_3D_USE_COLOR_BUFFER
                   , color
#endif
      };

      *output++ = tp;
      res++;
   }
   return 1;
}


int cosLatToInt(float cosLat)
{
   return coslat2int(cosLat);
}

int cosLatToCosLatInv(float cosLatMult)
{
   return coslat2coslatinv(cosLatMult);
}

} // End of namespace Algotrim 


} // End of namespace Triangulation


