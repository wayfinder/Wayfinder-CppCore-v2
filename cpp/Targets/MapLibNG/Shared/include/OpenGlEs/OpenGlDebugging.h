/*
Copyright (c) 1999 - 2010, Vodafone Group Services Ltd
All rights reserved.

Redistribution and use in source and binary forms, with or without modification, are permitted provided that the following conditions are met:

    * Redistributions of source code must retain the above copyright notice, this list of conditions and the following disclaimer.
    * Redistributions in binary form must reproduce the above copyright notice, this list of conditions and the following disclaimer in the documentation and/or other materials provided with the distribution.
    * Neither the name of the Vodafone Group Services Ltd nor the names of its contributors may be used to endorse or promote products derived from this software without specific prior written permission.

THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS" AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE ARE DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT HOLDER OR CONTRIBUTORS BE LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
*/


#ifndef OPENGLDEBUGGING_H
#define OPENGLDEBUGGING_H

#include "Vector3d.h"
#include "Vector2d.h"
#include "FixedPointType.h"


namespace OpenGlDebugging {
   void drawLine( const vec3x& from,
                  const vec3x& to,
                  fixedp r, fixedp g, fixedp b, fixedp a );

   void initPolyLineDebugging( int resX, int resY );
   void polyLineDebugging( int resX, int resY );
   

   void drawTriangleStripsAsLines( const vbuf3x& buffer );

   void drawDottedLine( const vbuf3x& buffer,
                        int pointSize = 8,
                        int lineWidth = 2);

   void drawDot( const vec3x& point );

   void orthographicProjection();

   void simpleTexture();
   void drawWfPolygon();
   void drawAxis();
   void drawGrid();
   void simpleAxis();
   int  simpleCube();

   void printMatrix( float m[16] );
   void dumpGlState();
}




#endif /* OPENGLDEBUGGING_H */
