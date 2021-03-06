/*
Copyright (c) 1999 - 2010, Vodafone Group Services Ltd
All rights reserved.

Redistribution and use in source and binary forms, with or without modification, are permitted provided that the following conditions are met:

    * Redistributions of source code must retain the above copyright notice, this list of conditions and the following disclaimer.
    * Redistributions in binary form must reproduce the above copyright notice, this list of conditions and the following disclaimer in the documentation and/or other materials provided with the distribution.
    * Neither the name of the Vodafone Group Services Ltd nor the names of its contributors may be used to endorse or promote products derived from this software without specific prior written permission.

THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS" AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE ARE DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT HOLDER OR CONTRIBUTORS BE LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
*/


#ifndef OPENGLESUTIL_H
#define OPENGLESUTIL_H

#include "Vector3d.h"

namespace OpenGlEsUtil {

/**
 *   Sets up an OpenGL perspective according to screen size etc.
 *
 *   @param viewportWidth   The width of the viewport  (resolution width)
 *   @param viewportHeight  The height of the viewport (resolution height)
 *   @param fov             The field of view (angular extent of viewport)
 *   @param zNear           The z position of the near plane
 *   @param zFar            The z position of the far plane
 *   
 */ 
   
void gluPerspective( double fov, double ratio,
                     double zNear, double zFar );

/**
 *   Sets up an OpenGL perspective according to screen size etc.
 *
 *   @see OpenGlEsUtil::gluPerspective
 *
 *   @param   m   Destination matrix.
 *   
 */ 
   
void gluPerspective( double fov, double ratio,
                     double zNear, double zFar,
                     float m[16] );

/**
 *   Initializes a matrix to the identity matrix.
 *
 *   @see glLoadIdentity in the OpenGL specification.
 *
 */ 

void glLoadIdentityf( float m[16] );

/**
 *   Sets up the rotation matrix m around the X-axis by deg degrees.
 *   Semantically equivalent to calling glRotate( deg, 1, 0, 0 ).
 *   
 *   @see glRotate in the OpenGL specification.
 */
   
void glRotateXf( float deg, float m[16] );

/**
 *   Sets up the rotation matrix m around the Y-axis by deg degrees.
 *   Semantically equivalent to calling glRotate( deg, 0, 1, 0 ).
 *   
 *   @see glRotate in the OpenGL specification.
 */
   
void glRotateYf( float deg, float m[16] );

/**
 *   Sets up the rotation matrix m around the Z-axis by deg degrees.
 *   Semantically equivalent to calling glRotate( deg, 0, 0, 1 ).
 *   
 *   @see glRotate in the OpenGL specification.
 */
   
void glRotateZf( float deg, float m[16] );
   
/**
 *   Sets up the translation matrix m.
 *   Semantically equivalent to calling glTranslate.
 *   
 *   @see glTranslate in the OpenGL specification.
 */

void glTranslatef( float x, float y, float z, float m[16] );

/**
 *   Sets up the scaling matrix m. Semantically equivalent
 *   to calling glScale.
 *   
 *   @see glScale in the OpenGL specification.
 */
   
void glScalef( float x, float y, float z, float m[16] );


/**
 *   Sets up a perspective projection for matrix
 *   m by specifying a viewing frustum.
 *
 *   @see glFrustum in the OpenGL specification
 *   
 */

void glFrustumf( float left, float right,
                 float bottom, float top,
                 float zNear, float zFar,
                 float m[16] );

/**
 *   Multiplies the matrix a by b and stores the result in r.
 *
 *   Note that a and b cannot be equal to r! This is verified by
 *   asserts.
 *   
 */
   
void glMultMatrixf( const float a[16], const float b[16], float r[16] );

/**
 *   Makes sure that the current OpenGL context has no error flag
 *   set, and will forcefully assert that such is the case.
 *
 */
 
void assertNoErrors();

/**
 *   Prints an OpenGL matrix to standard out.
 *
 *   @param   m   The matrix to be printed.
 *
 */ 
   
void glPrintMatrix( const float m[16] );

/**
 *   Prints two matrices side by side to standard out.
 *
 *   @param   l   The matrix to the left.
 *   @param   r   The matrix to the right.
 *   
 */

void glPrintMatrices( const float l[16], const float r[16] );

/**
 * Inverts the matrix in source. Assumes that the src matrix
 * is non-singular, otherwise this method will return false.
 */ 

bool glInvertMatrix(const float src[16], float inverse[16]);

/**
 * Multiplies a matrix with a vector.
 */ 
void glMultMatrixVecf(const float matrix[16],
                      const float in[4],
                      float out[4]);

/**
 * Projects the screen position at (winx, winy, winz) to a
 * position in the world. Expensive operation.
 */ 
bool
gluUnProject(const float in[4],
             const float modelMatrix[16],
             const float projMatrix[16],
             const int viewport[4],
             float out[4]);
}

#endif /* OPENGLESUTIL_H */

