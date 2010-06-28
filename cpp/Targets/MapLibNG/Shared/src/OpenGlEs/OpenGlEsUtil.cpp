#define CORE_LOGGING_MODULE_NAME "OpenGlEsUtil"
// #define CORE_LOGGING_MODULE_LEVEL CL_ALL_FILE

#include "CoreDebugPrint.h"
#include "config.h"
#include "OpenGlEsUtil.h"
#include "Vector3d.h"
#include "OpenGlEs.h"
#include <math.h>
#include "MC2Logging.h"


static const char* DBG = "[GLES_UTIL]: ";
static const float degToRad = 1.0f/57.2957795f;

struct FrustumParams {
   double yMax, yMin, xMax, xMin, zNear, zFar;
};

FrustumParams getFrustumParams( double fov, double ratio,
                                double zNear, double zFar ) {

   FrustumParams ret;
   
   ret.yMax = zNear * (double) (tan (fov * 3.1415962f / 360.0f));
   ret.yMin = -ret.yMax;
   ret.xMax = ret.yMax * ratio;
   ret.xMin = ret.yMin * ratio;
   ret.zNear = zNear;
   ret.zFar = zFar;
   
   return ret;
}

void OpenGlEsUtil::gluPerspective( double fov,
                                   double ratio,
                                   double zNear,
                                   double zFar )
{
   FrustumParams params = getFrustumParams( fov, ratio, zNear, zFar );
   
   glFrustumx( fixedp( params.xMin ).fx,
               fixedp( params.xMax ).fx,
               fixedp( params.yMin ).fx,
               fixedp( params.yMax ).fx,
               fixedp( params.zNear ).fx,
               fixedp( params.zFar).fx);
}

void OpenGlEsUtil::gluPerspective( double fov,
                                   double ratio,
                                   double zNear,
                                   double zFar,
                                   float m[16] )
{
   FrustumParams params = getFrustumParams( fov, ratio, zNear, zFar );

   OpenGlEsUtil::glFrustumf( params.xMin, params.xMax,
                             params.yMin, params.yMax,
                             params.zNear, params.zFar,
                             m );
}

void OpenGlEsUtil::glFrustumf( float left, float right,
                               float bottom, float top,
                               float zNear, float zFar,
                               float m[16] )
{
   double e = (2 * zNear) / (right - left);
   double f = (2 * zNear) / (top - bottom);

   double a = (right + left) / (right - left);
   double b = (top + bottom) / (top - bottom);
   double c = -( (zFar + zNear) / (zFar - zNear ) );
   double d = -( (2 * zFar * zNear) / (zFar - zNear) );
   
   m[0+4*0] = e; m[0+4*1] = 0; m[0+4*2] = a;  m[0+4*3] = 0;
   m[1+4*0] = 0; m[1+4*1] = f; m[1+4*2] = b;  m[1+4*3] = 0;
   m[2+4*0] = 0; m[2+4*1] = 0; m[2+4*2] = c;  m[2+4*3] = d;
   m[3+4*0] = 0; m[3+4*1] = 0; m[3+4*2] = -1; m[3+4*3] = 0;    
}

void OpenGlEsUtil::assertNoErrors()
{
   GLenum err = glGetError();
   
   if( err != GL_NO_ERROR ) {
      mc2dbg << DBG << "Unexpected GL error: ";
      switch( err ) {
         case GL_INVALID_ENUM:
            mc2dbg << "INVALID_ENUM";
            break;
         case GL_INVALID_VALUE:
            mc2dbg << "INVALID_VALUE";
            break;
         case GL_INVALID_OPERATION:
            mc2dbg << "INVALID_OPERATION";
            break;
         case GL_STACK_OVERFLOW:
            mc2dbg << "STACK_OVERFLOW";
            break;
         case GL_STACK_UNDERFLOW:
            mc2dbg << "STACK_UNDERFLOW";
            break;
         case GL_OUT_OF_MEMORY:
            mc2dbg << "OUT_OF_MEMORY";
            break;
         default:
            break;
      }
      mc2dbg << endl;
      MC2_ASSERT( !"GL_PANIC" );
   }
}

void OpenGlEsUtil::glLoadIdentityf( float m[16] )
{
   m[0+4*0] = 1; m[0+4*1] = 0; m[0+4*2] = 0; m[0+4*3] = 0;
   m[1+4*0] = 0; m[1+4*1] = 1; m[1+4*2] = 0; m[1+4*3] = 0;
   m[2+4*0] = 0; m[2+4*1] = 0; m[2+4*2] = 1; m[2+4*3] = 0;
   m[3+4*0] = 0; m[3+4*1] = 0; m[3+4*2] = 0; m[3+4*3] = 1;
}

void OpenGlEsUtil::glScalef( float x, float y, float z, float m[16] )
{
   m[0+4*0] = x; m[0+4*1] = 0; m[0+4*2] = 0; m[0+4*3] = 0;
   m[1+4*0] = 0; m[1+4*1] = y; m[1+4*2] = 0; m[1+4*3] = 0;
   m[2+4*0] = 0; m[2+4*1] = 0; m[2+4*2] = z; m[2+4*3] = 0;
   m[3+4*0] = 0; m[3+4*1] = 0; m[3+4*2] = 0; m[3+4*3] = 1;
}

void OpenGlEsUtil::glTranslatef( float x, float y, float z, float m[16] )
{
   m[0+4*0] = 1; m[0+4*1] = 0; m[0+4*2] = 0; m[0+4*3] = x;
   m[1+4*0] = 0; m[1+4*1] = 1; m[1+4*2] = 0; m[1+4*3] = y;
   m[2+4*0] = 0; m[2+4*1] = 0; m[2+4*2] = 1; m[2+4*3] = z;
   m[3+4*0] = 0; m[3+4*1] = 0; m[3+4*2] = 0; m[3+4*3] = 1;
}

void OpenGlEsUtil::glRotateXf( float deg, float m[16] )
{
   float c = cos( deg * degToRad );
   float s = sin( deg * degToRad );
   
   m[0+4*0] = 1; m[0+4*1] = 0;  m[0+4*2] = 0;  m[0+4*3] = 0;
   m[1+4*0] = 0; m[1+4*1] = c;  m[1+4*2] = -s; m[1+4*3] = 0;
   m[2+4*0] = 0; m[2+4*1] = s;  m[2+4*2] = c;  m[2+4*3] = 0;
   m[3+4*0] = 0; m[3+4*1] = 0;  m[3+4*2] = 0;  m[3+4*3] = 1;    
}

void OpenGlEsUtil::glRotateYf( float deg, float m[16] )
{
   float c = cos( deg * degToRad );
   float s = sin( deg * degToRad );
   
   m[0+4*0] = c;  m[0+4*1] = 0;  m[0+4*2] = s;  m[0+4*3] = 0;
   m[1+4*0] = 0;  m[1+4*1] = 1;  m[1+4*2] = 0;  m[1+4*3] = 0;
   m[2+4*0] = -s; m[2+4*1] = 0;  m[2+4*2] = c;  m[2+4*3] = 0;
   m[3+4*0] = 0;  m[3+4*1] = 0;  m[3+4*2] = 0;  m[3+4*3] = 1;    
}

void OpenGlEsUtil::glMultMatrixf( const float a[16],
                                  const float b[16],
                                  float r[16] )
{
   MC2_ASSERT( a != r && b != r );
   
   int i, j;

   for (i = 0; i < 4; i++) {
      for (j = 0; j < 4; j++) {
         r[i*4+j] =
            a[i*4+0]*b[0*4+j] +
            a[i*4+1]*b[1*4+j] +
            a[i*4+2]*b[2*4+j] +
            a[i*4+3]*b[3*4+j];
      }
   } 
}

void OpenGlEsUtil::glPrintMatrix( const float m[16] )
{
   printf(" [ %f %f %f %f ] \n"
          " [ %f %f %f %f ] \n"
          " [ %f %f %f %f ] \n"
          " [ %f %f %f %f ] \n",
          m[0+4*0], m[0+4*1], m[0+4*2], m[0+4*3],
          m[1+4*0], m[1+4*1], m[1+4*2], m[1+4*3],
          m[2+4*0], m[2+4*1], m[2+4*2], m[2+4*3],
          m[3+4*0], m[3+4*1], m[3+4*2], m[3+4*3]); 
}

void OpenGlEsUtil::glRotateZf( float deg, float m[16] )
{
   float c = cos(deg * degToRad);
   float s = sin(deg * degToRad);
   
   m[0+4*0] = c;  m[0+4*1] = -s;  m[0+4*2] = 0;  m[0+4*3] = 0;
   m[1+4*0] = s;  m[1+4*1] = c;  m[1+4*2] = 0;  m[1+4*3] = 0;
   m[2+4*0] = 0; m[2+4*1] = 0;  m[2+4*2] = 1;  m[2+4*3] = 0;
   m[3+4*0] = 0;  m[3+4*1] = 0;  m[3+4*2] = 0;  m[3+4*3] = 1;    
}

void OpenGlEsUtil::glPrintMatrices( const float l[16], const float r[16] )
{
   // return;
   
   // std::cout << "Printing matrices."<< std::endl;
   
   printf( " [ % .3f % .3f % .3f % 10.3f ]   [ % .3f % .3f % .3f % 10.3f ]\n",
           l[0+4*0], l[0+4*1], l[0+4*2], l[0+4*3],
           r[0+4*0], r[0+4*1], r[0+4*2], r[0+4*3] );

   printf( " [ % .3f % .3f % .3f % 10.3f ]   [ % .3f % .3f % .3f % 10.3f ]\n",
           l[1+4*0], l[1+4*1], l[1+4*2], l[1+4*3],
           r[1+4*0], r[1+4*1], r[1+4*2], r[1+4*3] );
 
   printf( " [ % .3f % .3f % .3f % 10.3f ]   [ % .3f % .3f % .3f % 10.3f ]\n",
           l[2+4*0], l[2+4*1], l[2+4*2], l[2+4*3],
           r[2+45*0], r[2+4*1], r[2+4*2], r[2+4*3] );

   printf( " [ % .3f % .3f % .3f % 10.3f ]   [ % .3f % .3f % .3f % 10.3f ]\n",
           l[3+4*0], l[3+4*1], l[3+4*2], l[3+4*3],
           r[3+4*0], r[3+4*1], r[3+4*2], r[3+4*3] );
}

bool OpenGlEsUtil::glInvertMatrix(const float src[16], float inverse[16])
{
   int i, j, k, swap;
   double t;
   float temp[4][4];

   for (i=0; i<4; i++) {
      for (j=0; j<4; j++) {
         temp[i][j] = src[i*4+j];
      }
   }

   glLoadIdentityf(inverse);

   for (i = 0; i < 4; i++) {
      /*
      ** Look for largest element in column
      */
      swap = i;
      for (j = i + 1; j < 4; j++) {
         if (fabs(temp[j][i]) > fabs(temp[i][i])) {
            swap = j;
         }
      }

      if (swap != i) {
         /*
         ** Swap rows.
         */
         for (k = 0; k < 4; k++) {
            t = temp[i][k];
            temp[i][k] = temp[swap][k];
            temp[swap][k] = t;

            t = inverse[i*4+k];
            inverse[i*4+k] = inverse[swap*4+k];
            inverse[swap*4+k] = t;
         }
      }

      if (temp[i][i] == 0) {
         /*
         ** No non-zero pivot.  The matrix is singular, which shouldn't
         ** happen.  This means the user gave us a bad matrix.
         */
         return GL_FALSE;
      }

      t = temp[i][i];
      for (k = 0; k < 4; k++) {
         temp[i][k] /= t;
         inverse[i*4+k] /= t;
      }
      for (j = 0; j < 4; j++) {
         if (j != i) {
            t = temp[j][i];
            for (k = 0; k < 4; k++) {
               temp[j][k] -= temp[i][k]*t;
               inverse[j*4+k] -= inverse[i*4+k]*t;
            }
         }
      }
   }
   return GL_TRUE;
}
void OpenGlEsUtil::glMultMatrixVecf(const float matrix[16], const float in[4],
                                    float out[4])
{
   int i;
   
   for (i=0; i<4; i++) {
      out[i] =
         in[0] * matrix[0*4+i] +
         in[1] * matrix[1*4+i] +
         in[2] * matrix[2*4+i] +
         in[3] * matrix[3*4+i];
   }
}

bool
OpenGlEsUtil::gluUnProject(const float in[4],
                           const float modelMatrix[16],
                           const float projMatrix[16],
                           const int viewport[4],
                           float out[4])
{
   float tmpIn[4] = { in[0], in[1], in[2], in[3] };
   float finalMatrix[16];
   
   glMultMatrixf(modelMatrix, projMatrix, finalMatrix);
   if (!glInvertMatrix(finalMatrix, finalMatrix)) {
      return false;  
   }
   
   /* Map x and y from window coordinates */
   tmpIn[0] = (tmpIn[0] - viewport[0]) / viewport[2];
   tmpIn[1] = (tmpIn[1] - viewport[1]) / viewport[3];

   /* Map to range -1 to 1 */
   tmpIn[0] = tmpIn[0] * 2 - 1;
   tmpIn[1] = tmpIn[1] * 2 - 1;
   tmpIn[2] = tmpIn[2] * 2 - 1;

   glMultMatrixVecf(finalMatrix, tmpIn, out);

   if (out[3] == 0.0) {
      return false;  
   }

   out[0] /= out[3];
   out[1] /= out[3];
   out[2] /= out[3];
   out[3] = 1.0f;
   
   return true;
}
