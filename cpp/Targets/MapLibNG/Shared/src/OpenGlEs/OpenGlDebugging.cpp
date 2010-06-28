/*
Copyright (c) 1999 - 2010, Vodafone Group Services Ltd
All rights reserved.

Redistribution and use in source and binary forms, with or without modification, are permitted provided that the following conditions are met:

    * Redistributions of source code must retain the above copyright notice, this list of conditions and the following disclaimer.
    * Redistributions in binary form must reproduce the above copyright notice, this list of conditions and the following disclaimer in the documentation and/or other materials provided with the distribution.
    * Neither the name of the Vodafone Group Services Ltd nor the names of its contributors may be used to endorse or promote products derived from this software without specific prior written permission.

THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS" AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE ARE DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT HOLDER OR CONTRIBUTORS BE LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
*/


#define CORE_LOGGING_MODULE_NAME "OpenGlDebugging"
//#define CORE_LOGGING_MODULE_LEVEL CL_ALL_FILE

#include "CoreDebugPrint.h"
#include "config.h"
#include "OpenGlDebugging.h"
#include "OpenGlEs.h"
#include "Vector3d.h"
#include <math.h>


bool frame_done();

typedef struct ViewPort {
   GLfloat xMin;
   GLfloat xMax;
   GLfloat yMax;
   GLfloat yMin;
   GLfloat zFar;
   GLfloat zNear;
} ViewPort;

void OpenGlDebugging::drawAxis()
{
   fixedp NUM_60 = fixedp(fixedp::RAW, 5650*4);
   
   vec3x origin( fx::FX_ZERO.fx,    fx::FX_ZERO.fx,   fx::FX_ZERO.fx );
   vec3x  axisX(     NUM_60.fx,  fx::FX_ZERO.fx,   fx::FX_ZERO.fx );
   vec3x  axisY( fx::FX_ZERO.fx,      NUM_60.fx,   fx::FX_ZERO.fx );
   vec3x  axisZ( fx::FX_ZERO.fx,    fx::FX_ZERO.fx,     NUM_60.fx );   
   
   glLineWidth(5.f);
   drawLine(origin, axisX, fx::FX_ONE.fx,  fx::FX_ZERO.fx, fx::FX_ZERO.fx, fx::FX_ONE.fx );
   drawLine(origin, axisY, fx::FX_ZERO.fx, fx::FX_ONE.fx,  fx::FX_ZERO.fx, fx::FX_ONE.fx );
   drawLine(origin, axisZ, fx::FX_ZERO.fx, fx::FX_ZERO.fx, fx::FX_ONE.fx,  fx::FX_ONE.fx );
   glLineWidth(1.f);
}

void OpenGlDebugging::drawLine( const vec3x& from,
                                const vec3x& to,
                                fixedp r, fixedp g, fixedp b, fixedp a )
{
   vbuf3x buffer;
   buffer.push_back( from );
   buffer.push_back( to );

   glColor4x( r.fx, g.fx, b.fx, a.fx );

   glVertexPointer(3, GL_FIXED,  0, &buffer[0]);

   glDrawArrays (GL_LINES, 0, static_cast<int> ( buffer.size() ));
}

void drawLineFloat( float x1, float y1, float z1,
                    float x2, float y2, float z2,
                    float r, float g, float b, float a )
{
   std::vector<GLfloat> buffer;

   buffer.push_back( x1 );
   buffer.push_back( y1 );
   buffer.push_back( z1 );
   buffer.push_back( x2 );
   buffer.push_back( y2 );
   buffer.push_back( z2 );
   
   //    glColor4f( r, g, b, a);

   glVertexPointer(3, GL_FLOAT,  0, &buffer[0]);

   glDrawArrays (GL_LINES, 0, static_cast<int> ( buffer.size() ));
}

void OpenGlDebugging::drawGrid()
{  
   glColor4f(0.25f, 0.25f, 0.25f, 1.0f);

   int columns = 256;
   int rows = 256;
   int gridSize = 5560; //Approx 50 meters at the equator.
   
   std::vector<int> gridVector;

   int zpos = 0;

   for (int i=0; i<=rows; i++) {
      /*First point */
      //glVertex3f(0, 0, i*gridSize);
      gridVector.push_back( (0) );
      gridVector.push_back( (i*gridSize) );
      gridVector.push_back( zpos );
      
      /* Second point */
      //glVertex3f( i2x(columns, 0, i*gridSize);
      gridVector.push_back( columns * gridSize );
      gridVector.push_back( i*gridSize );
      gridVector.push_back( zpos );
   }

   for (int i=0; i<=columns; i++) {
      /* First point */
      //glVertex3f(i*gridSize, 0, 0);
      gridVector.push_back( i*gridSize );
      gridVector.push_back( 0 );
      gridVector.push_back( zpos );

      /* Second point */
      //glVertex3f(i*gridSize, 0, rows);
      gridVector.push_back( (i*gridSize) );
      gridVector.push_back( (rows * gridSize) );
      gridVector.push_back( zpos );
   }

   glVertexPointer(3, GL_FIXED,  0, &gridVector[0]);

   glDrawArrays (GL_LINES, 0, gridVector.size() / 3);
}


// void OpenGlDebugging::simpleTexture()
// {
// 	int f = 0;
// 	GLuint tex;

// 	/* Generate texture */

//    vbuf3x vertices;

//    int imgWidth = 32;
//    int imgHeight = 32;
   
//    std::vector<unsigned int> texData;
//    texData.resize( imgWidth * imgHeight, 0x00FF00FF );

   
//    vertices.push_back(
//       Vector2d( fixedp::fromDouble( 0.0f), fixedp::fromDouble (0.0f ) ) );
//    vertices.push_back(
//       Vector2d( fixedp::fromDouble( 1.0f), fixedp::fromDouble (0.0f ) ) );
//    vertices.push_back(
//       Vector2d( fixedp::fromDouble( 0.0f), fixedp::fromDouble (1.0f ) ) );
//    vertices.push_back(
//       Vector2d( fixedp::fromDouble( 1.0f), fixedp::fromDouble (1.0f ) ) );
   
// 	glGenTextures(2, &tex);
// 	glBindTexture(GL_TEXTURE_2D, tex);

//    glTexImage2D( GL_TEXTURE_2D, 0,
//                  GL_RGBA,
//                  imgWidth, imgHeight,
//                  0, GL_RGBA,
//                  GL_UNSIGNED_BYTE, &texData[0] );
   
//    //    glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
// 	glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
// 	//    glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
// 	//    glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
	
// 	/* Arrays */

// 	glVertexPointer(2, GL_FIXED, 0, &vertices[0]);
// 	glEnableClientState(GL_VERTEX_ARRAY);

// 	/* Set up multitexturing */

// 	glClientActiveTexture(GL_TEXTURE0);
// 	glTexCoordPointer(2, GL_FIXED, 0, &vertices[0]);
// 	glEnableClientState(GL_TEXTURE_COORD_ARRAY);

// 	glActiveTexture(GL_TEXTURE0);
// 	glEnable(GL_TEXTURE_2D);
// 	glBindTexture(GL_TEXTURE_2D, tex);

//    /* Something else... */

// 	assert( glGetError() == GL_NO_ERROR );

// 	glMatrixMode(GL_PROJECTION);
// 	glLoadIdentity();

//    //    glFrustumf(-1, 1, -1, 1, 1, 10);
//    glOrthof( 0.0f, 1.0f, 1.0f, 0.0f, -1, 1);

// 	//    glEnable(GL_DEPTH_TEST);

// 	//    glEnable(GL_CULL_FACE);
// 	//    glCullFace(GL_BACK);

	
// 	while (1)
// 	{
// 		glClearColor(0.2f, 0.3f, 0.4f, 0.0f);
// 		glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

// 		glMatrixMode(GL_MODELVIEW);

// 		glLoadIdentity();

// 		//    glTranslatef(0, 0, -5);
// 		//    glRotatef(f / 9.0f, 0.8f, 0, 0.2f);

// 		glColor4f(1, 1, 1, 1);

//       glDrawArrays( GL_TRIANGLE_STRIP, 0, 4 );

// 		assert(glGetError() == GL_NO_ERROR);

// 		frame_done();
// 		f++;
// 	}
// }


//    bool frame_done();

//    static const signed char vertices[] =
//    {
//    	-1, -1,  1,
//    	-1,  1,  1,
//       1,  1,  1,
//       1, -1,  1,
//    	-1, -1, -1,
//    	-1,  1, -1,
//       1,  1, -1,
//       1, -1, -1
//    };

//    static const float texcoords[] =
//    {
//    	0, 0,
//    	0, 1,
//    	1, 1,
//    	1, 0,
//    	0, 0,
//    	0, 1,
//    	1, 1,
//    	1, 0,
//    #if 0
//    	0, 0,
//    	0, 127,
//    	0, 0,
//    	0, 127,
//    	127, 0,
//    	127, 127,
//    	127, 0,
//    	127, 127
//    #endif
//    };

//    static const unsigned char triangles[] =
//    {
//    	0, 2, 1,
//    	0, 7, 3,
//    	1, 4, 0,
//    	2, 5, 1,
//    	3, 2, 0,
//    	3, 6, 2,
//    	4, 6, 7,
//    	4, 7, 0,
//    	5, 4, 1,
//    	5, 6, 4,
//    	6, 5, 2,
//    	7, 6, 3
//    };

//    static const unsigned char colors[] =
//    {
//    	255, 255, 128, 255,
//    	255, 128, 255, 255,
//    	128, 255, 255, 255,
//    	255, 128,   0, 255,
//    	255, 255, 128, 255,
//    	255, 128, 255, 255,
//    	128, 255, 255, 255,
//    	255,   0, 255, 255,
//    };

//    static const signed char normals[] =
//    {
//    	-128, -128,  127,
//    	-128,  127,  127,
//       127,  127,  127,
//       127, -128,  127,
//    	-128, -128, -128,
//    	-128,  127, -128,
//       127,  127, -128,
//       127, -128, -128
//    };

//    static const unsigned char texture[] =
//    {
//    	/* Palette (4 bit, rgba8) */
//       0,  0,  0,  0,   32, 16, 16, 16,
//       32, 64, 32, 32,   48, 48, 48, 48,
//       64, 64,128, 64,   80, 96, 80, 80,
//       96,128, 96, 96,  176,112,112,112,
//    	128,128,128,128,  144,144,144,144,
//    	160,176,160,160,  176,176,192,176,
//    	192,192,192,192,  208,208,208,208,
//    	240,224,224,224,  240,255,240,240,
//    #if 0
//    	/* Palette (4 bit, rgba4) */
//    #define C(r, g, b, a) ((r) << 4) | (g), ((b) << 4) | (a)
//    	C(0, 0, 0, 15), C(15, 0, 0, 15),
//    	C(0, 15, 0, 15), C(0, 0, 15, 15),
//    	C(0, 15, 15, 15), C(0, 15, 0, 15),
//    	C(0, 0, 0, 15), C(8, 8, 8, 15),
//    	C(0, 12, 0, 15), C(0, 0, 0, 15),
//    	C(0, 0, 0, 15), C(7, 0, 0, 15),
//    	C(0, 0, 9, 15), C(0, 0, 0, 15),
//    	C(0, 0, 0, 15), C(15, 15, 15, 15),
//    #undef C
//    #endif
//    	0x01, 0x23, 0x45, 0x67,
//    	0x12, 0x34, 0x56, 0x78,
//    	0x23, 0x45, 0x67, 0x89,
//    	0x34, 0x56, 0x78, 0x9A,
//    	0x45, 0x67, 0x89, 0xAB,
//    	0x56, 0x78, 0x9A, 0xBC,
//    	0x67, 0x89, 0xAB, 0xCD,
//    	0x78, 0x9A, 0xBC, 0xDE,
//    };

//    #if 0
//    static unsigned char texture2[] =
//    {
//    	64,64,64,  0,0,0,
//    	0,0,0,  64,64,64,
//    };
//    #else
//    static unsigned char texture2a[] =
//    {
//    	64,64,64,255,  0,0,0,255,
//    	0,0,0,255,  64,64,64,255,
//    };
//    #endif

//    int OpenGlDebugging::simpleCube()
//    {
//    	static const GLfixed light_position[] = { 0, 4 * 65536, 0, 1 * 65536 };
//    	int f = 0;
//    	GLuint tex[2];

//    	/* Generate texture */

//    	glGenTextures(2, tex);

//    	glBindTexture(GL_TEXTURE_2D, tex[0]);
//    	glCompressedTexImage2D(GL_TEXTURE_2D, 0, GL_PALETTE4_RGBA8_OES, 8, 8, 0,
//    	                       sizeof(texture), texture);
//    	glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
//    	glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
//    	glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
//    	glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);

//    	glBindTexture(GL_TEXTURE_2D, tex[1]);
//    #if 0
//    	glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, 2, 2, 0, GL_RGB, GL_UNSIGNED_BYTE,
//    	             texture2);
//    #else
//    	glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, 2, 2, 0, GL_RGBA, GL_UNSIGNED_BYTE,
//    	             texture2a);
//    #endif

//    	glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
//    	glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
//    	glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
//    	glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);

//    	/* Arrays */

//    	glVertexPointer(3, GL_BYTE, 0, vertices);
//    	glEnableClientState(GL_VERTEX_ARRAY);

//    #if 0
//    	glColorPointer(4, GL_UNSIGNED_BYTE, 0, colors);
//    	glEnableClientState(GL_COLOR_ARRAY);
//    #endif

//    	glNormalPointer(GL_BYTE, 0, normals);
//    	glEnableClientState(GL_NORMAL_ARRAY);

//    	/* Set up multitexturing */

//    	glClientActiveTexture(GL_TEXTURE0);
//    	glTexCoordPointer(2, GL_FLOAT, 0, texcoords);
//    	glEnableClientState(GL_TEXTURE_COORD_ARRAY);

//    	glActiveTexture(GL_TEXTURE0);
//    	glEnable(GL_TEXTURE_2D);
//    	glBindTexture(GL_TEXTURE_2D, tex[0]);

//    	glClientActiveTexture(GL_TEXTURE1);
//    	glTexCoordPointer(2, GL_FLOAT, 0, texcoords);
//    	glEnableClientState(GL_TEXTURE_COORD_ARRAY);

//    	glActiveTexture(GL_TEXTURE1);
//    	glEnable(GL_TEXTURE_2D);
//    	glBindTexture(GL_TEXTURE_2D, tex[1]);
//    //    	glTexEnvi(GL_TEXTURE_ENV, GL_TEXTURE_ENV_MODE, GL_ADD);

//    	/* Something else... */

//    	assert(glGetError() == GL_NO_ERROR);

//    	glMatrixMode(GL_PROJECTION);
//    	glLoadIdentity();
//    	glFrustumf(-1, 1, -1, 1, 1, 10);

//    	glLightxv(GL_LIGHT0, GL_POSITION, light_position);
//    	glEnable(GL_LIGHT0);
//    	glEnable(GL_LIGHTING);

//    	glEnable(GL_COLOR_MATERIAL);

//    	glEnable(GL_DEPTH_TEST);

//    	glEnable(GL_CULL_FACE);
//    	glCullFace(GL_BACK);

//    #if 0
//    	{
//    		float a = -1;
//    		glPointSize(-44);
//    		glGetFloatv(GL_POINT_SIZE_MIN, &a);
//    		fprintf(stderr, "min %f\n", a);
//    		glGetFloatv(GL_POINT_SIZE_MAX, &a);
//    		fprintf(stderr, "max %f\n", a);
//    		glGetFloatv(GL_POINT_SIZE, &a);
//    		fprintf(stderr, "sisi %f\n", a);
//    		fprintf(stderr, "%d\n", glIsEnabled(GL_POINT_SMOOTH));
//    	}
//    #endif

//    #if 0
//    	glDisable(GL_TEXTURE_2D);
//    	glDisable(GL_LIGHTING);
//    	glDisable(GL_FOG);
//    	glDisable(GL_DEPTH_TEST);
//    #endif

//    	assert(glGetError() == GL_NO_ERROR);

	
//    	while (1)
//    	{
//    		glClearColor(0.2f, 0.3f, 0.4f, 0.0f);
//    		glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

//    		glMatrixMode(GL_TEXTURE);

//    		glActiveTexture(GL_TEXTURE0);
//    		glLoadIdentity();
//    		glScalef(5, 5, 1);

//    		glActiveTexture(GL_TEXTURE1);
//    		glLoadIdentity();
//    		glScalef(3, 3, 1);
//    		glRotatef(f / 15.0f, 0, 0, 1);
//    		glTranslatef(-0.5f, -0.5f, 0);

//    		glMatrixMode(GL_MODELVIEW);

//    		glLoadIdentity();

//    		glTranslatef(0, 0, -5);
//    		glRotatef(f / 8.0f, 0.7f, 1, 0);
//    		glRotatef(f / 9.0f, 0.8f, 0, 0.2f);

//    		glColor4f(1, 1, 1, 1);
//    		glDrawElements(GL_TRIANGLES, sizeof(triangles) / sizeof(triangles[0]),
//                         GL_UNSIGNED_BYTE, triangles);

//    		assert(glGetError() == GL_NO_ERROR);

//    		frame_done();
//    		f++;
//    	}

//    	return 0;
//    }



//    static void glPrintMatrix(GLfloat m[16]) {

//    //       printf(" [ %f %f %f %f ] \n"
//    //              " [ %f %f %f %f ] \n"
//    //              " [ %f %f %f %f ] \n"
//    //              " [ %f %f %f %f ] \n",
//    //              m[0+4*0], m[0+4*1], m[0+4*2], m[0+4*3],
//    //              m[1+4*0], m[1+4*1], m[1+4*2], m[1+4*3],
//    //              m[2+4*0], m[2+4*1], m[2+4*2], m[2+4*3],
//    //              m[3+4*0], m[3+4*1], m[3+4*2], m[3+4*3]);
//    }

struct SpaceVector {
   float x, y, z;
};

struct SpaceVectorFixed {
   int x, y;
};

void doDrawLine(SpaceVector& from, SpaceVector& to,
                                      float r, float g, float b, float a)
{
   std::vector<SpaceVector> buffer;
   buffer.push_back(from);
   buffer.push_back(to);

   glColor4f( r, g, b, a );

   glVertexPointer(3, GL_FLOAT,  0, &buffer[0]);

   glDrawArrays (GL_LINES, 0, static_cast<int>( buffer.size() ));
}

// void OpenGlDebugging::simpleAxis()
// {
//    glViewport (0, 0, 800, 600);

//    float yMax, yMin, xMax, xMin;
   
//    float aspect = static_cast<GLfloat>( 800 ) / static_cast<GLfloat>( 600 );
//    float fovY = 50.0f;
//    float zNear = 2.0f;
//    float zFar = 120.0f;
   
//    yMax = zNear * (GLfloat) (tan (fovY * 3.1415962f / 360.0f));
//    yMin = -yMax;
//    xMax = yMax * aspect;
//    xMin = yMin * aspect;
//    zNear = zNear;
//    zFar = zFar;

//    glMatrixMode( GL_PROJECTION );
//    glLoadIdentity();
//    glFrustumf(  xMin, xMax, yMin, yMax, zNear, zFar);
   
//    glMatrixMode (GL_MODELVIEW);

//    float rot = 0.0f;

//    glEnableClientState(GL_VERTEX_ARRAY);
   
//    //    SpaceVector origin = {0.0f, 0.01f, 0.0f};
// //       SpaceVector  axisX = {3.0f, 0.01f, 0.0f};
// //       SpaceVector  axisY = {0.0f, 3.0f,  0.0f};
// //       SpaceVector  axisZ = {0.0f, 0.01f, 3.0f};


// //       SpaceVectorFixed p1 = { 0, 0, 0 };
// //       SpaceVectorFixed p2 = { 8 << 16, 0, 0 };
// //       SpaceVectorFixed p3 = { 8 << 16, 0, 8 << 16 };
// //       SpaceVectorFixed p4 = { 0, 0, 8 << 16 };   


//    int offsetVal = 8 << 16;
   
// //       float offsetVal = 8.0f;
   
//    SpaceVectorFixed p1 = { 0, 0 };
//    SpaceVectorFixed p2 = { offsetVal, 0 };
//    SpaceVectorFixed p3 = { offsetVal, offsetVal };
//    SpaceVectorFixed p4 = { 0, offsetVal };
   
//    std::vector<SpaceVectorFixed> triBuffer;

//    triBuffer.push_back( p1 );
//    triBuffer.push_back( p2 );
//    triBuffer.push_back( p3 );
//    triBuffer.push_back( p4 );

//    GLuint vbo;

//    glBindBuffer( GL_ARRAY_BUFFER, vbo );
//    glBufferData( GL_ARRAY_BUFFER, triBuffer.size() * sizeof(SpaceVectorFixed),
//                  &triBuffer[0], GL_STATIC_DRAW );
//    glBindBuffer( GL_ARRAY_BUFFER, 0 );

   
//    while (1)
// 	{
// //          float modelMatrix[16];
// //          float projectionMatrix[16];
      
//       glClearColor(0.2f, 0.3f, 0.4f, 0.0f);
//       glClear (GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
      
//       glLoadIdentity();

//       rot+=1.0f;

//       if( rot > 360.0f ) {
//          rot = 0.0f;
//       }

//       glRotatef(70.8f, 1.0f, 0.0f, 0.0f );
//       glRotatef(-51.0f, 0.0f, 1.0f, 0.0f );

//       glTranslatef( -5.754f, -19.01f, -6.024 );
      
// //          glGetFloatv (GL_PROJECTION_MATRIX, projectionMatrix);
// //          glGetFloatv (GL_MODELVIEW_MATRIX, modelMatrix);

// //          std::cout << "Projection matrix" << std::endl;
// //          glPrintMatrix( projectionMatrix );
// //          std::cout << "Model matrix" << std::endl;
// //          glPrintMatrix( modelMatrix );
      
      
// //          glLineWidth(5.f);
// //          doDrawLine(origin, axisX, 1.0, 0.0, 0.0, 1.0);
// //          doDrawLine(origin, axisY, 0.0, 1.0, 0.0, 1.0);
// //          doDrawLine(origin, axisZ, 0.0, 0.0, 1.0, 1.0);
// //          glLineWidth(1.f);

//       glBindBuffer( GL_ARRAY_BUFFER, vbo );
//       glVertexPointer( 2, GL_FIXED, 0, 0);
//       glDrawArrays (GL_TRIANGLE_STRIP, 0, triBuffer.size() );
//       glBindBuffer( GL_ARRAY_BUFFER, 0 );

// //          glVertexPointer( 2, GL_FIXED, 0, &triBuffer[0]);
// //          glDrawArrays (GL_TRIANGLE_STRIP, 0, triBuffer.size() );
      
//       glFlush();
//       glFinish();
      
// 		frame_done();
// 	}
// }

void OpenGlDebugging::drawWfPolygon()
{
//       vbuf3x vb;

//       vb.push_back( vec3x(2.060455322265625, 0, 0) );
//       vb.push_back( vec3x(0, 0, 0) );
//       vb.push_back( vec3x(0, 0, 2.060455322265625) );
//       vb.push_back( vec3x(0, 0, 2.060455322265625) );
//       vb.push_back( vec3x(2.060455322265625, 0, 2.060455322265625) );
//       vb.push_back( vec3x(2.060455322265625, 0, 0) );

//       glVertexPointer(3, /*3 components per vertex (x,y,z)*/
//                       GL_FIXED, 0, &vb[0]);
   
//       glColor4f( 0.0, 0.0, 1.0f, 1.0f );

//       glDrawArrays (GL_TRIANGLES, 0, vb.size() );
}

void OpenGlDebugging::drawTriangleStripsAsLines( const vbuf3x& buffer )
{   
   vbuf3x lineSet;

   vbuf3x::const_iterator vBI = buffer.begin();
   vec3x p0 = *vBI++;
   vec3x p1 = *vBI++;
   vec3x p2;

   // P0->P1 
   lineSet.push_back( p0 );
   lineSet.push_back( p1 );
   
   for( ; vBI != buffer.end(); vBI++ ) {
      p2 =  *vBI;
      
      // P1->P2 line
      lineSet.push_back( p1 );
      lineSet.push_back( p2 );

      // P0->P2 line
      lineSet.push_back( p0 );
      lineSet.push_back( p2 );

      p0 = p1;
      p1 = p2;
   }

   glColor4f( 1.0f, 1.0f, 1.0f, 1.0f );
   
   glVertexPointer(2, /*3 components per vertex (x,y,z)*/
                   GL_FIXED, 0, &lineSet[0]);

   glDrawArrays( GL_LINES, 0, lineSet.size() );
}

void OpenGlDebugging::drawDottedLine( const vbuf3x& buffer,
                                      int pointSize,
                                      int lineWidth )
{
   glMatrixMode( GL_MODELVIEW );
   glLoadIdentity();

   glPointSizex( fixedp(pointSize).fx );

   glLineWidthx( fixedp(lineWidth).fx );
   
   glVertexPointer(3, /*3 components per vertex (x,y)*/
                   GL_FIXED, 0, &buffer[0]);
   
   glDrawArrays( GL_LINE_STRIP, 0, buffer.size() );
   glDrawArrays( GL_POINTS, 0, buffer.size() );
}

void OpenGlDebugging::drawDot( const vec3x& point )
{   
   glColor4f( 1.0f, 1.0f, 1.0f, 1.0f );
   glPointSize( 8.0f );
   
   glVertexPointer(3, /*3 components per vertex (x,y,z)*/
                   GL_FIXED, 0, &point);
   
   glDrawArrays( GL_POINTS, 0, 1 );   
}



void OpenGlDebugging::printMatrix( float m[16] ) {
   printf(" [ %f %f %f %f ] \n"
          " [ %f %f %f %f ] \n"
          " [ %f %f %f %f ] \n"
          " [ %f %f %f %f ] \n",
          m[0+4*0], m[0+4*1], m[0+4*2], m[0+4*3],
          m[1+4*0], m[1+4*1], m[1+4*2], m[1+4*3],
          m[2+4*0], m[2+4*1], m[2+4*2], m[2+4*3],
          m[3+4*0], m[3+4*1], m[3+4*2], m[3+4*3]);
}

void OpenGlDebugging::dumpGlState()
{
//       float modelMatrix[16];
//       float projectionMatrix[16];
   
//       glGetFloatv (GL_PROJECTION_MATRIX, projectionMatrix);
//       glGetFloatv (GL_MODELVIEW_MATRIX, modelMatrix);
   
//       std::cout << "Projection matrix" << std::endl;
//       printMatrix( projectionMatrix );
//       std::cout << "Model matrix" << std::endl;
//       printMatrix( modelMatrix );
}

//    void OpenGlDebugging::orthographicProjection()
//    {

//       glViewport (0, 0, 800, 600);

//       glMatrixMode( GL_PROJECTION );
//       glLoadIdentity();

//       //    glOrthof(0, 800, 600, 0, -1, 1);
   
//       GLfloat square[] = {
//          -0.5, -0.5, 0.0,
//          0.5, -0.5, 0.0,
//          -0.5, 0.5, 0.0,
//          0.5, 0.5, 0.0
//       };

//       float width = 1.0f;
//       float height = 1.0f;
   
//       glOrthof(-width/2.0f, width/2.0f, -height/2.0f, height/2.0f, -1.0f, 1.0f);

//       glMatrixMode (GL_MODELVIEW);

//       glEnableClientState(GL_VERTEX_ARRAY);

//       float scale = 1.0f;

//       //    float halfw = width / 2.0f;
//    //       float halfh = height / 2.0f;
   
//       while (1)
//    	{
//          scale -= 0.001f;
      
//          glClearColor(0.0f, 0.0f, 0.0f, 0.0f);
//          glClear (GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

//    //          glMatrixMode (GL_PROJECTION);
//    //          glLoadIdentity();
//    //          float xMax = width / scale;
//    //          float yMax = height / scale;
      
//    //          glOrthof(0.0f, xMax, 0.0f, yMax, -1.0f, 1.0f);
      
      
//          glMatrixMode (GL_MODELVIEW);
      
//          glLoadIdentity();

//          glScalef( scale, scale, 1.0f );
//    //          glTranslatef( - xMax / 2.0f, - yMax / 2.0f, 0.0f );
      
      

      
//          glEnableClientState(GL_VERTEX_ARRAY);
//          glVertexPointer(3, GL_FLOAT, 0, square);

      
      
      
//          glDrawArrays(GL_TRIANGLE_STRIP, 0, 4);

//          glFlush();
//          glFinish();
      
//    		frame_done();
//    	}
   
//    }

