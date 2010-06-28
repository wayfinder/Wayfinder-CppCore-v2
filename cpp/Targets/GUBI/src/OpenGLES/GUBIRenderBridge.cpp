/*
Copyright (c) 1999 - 2010, Vodafone Group Services Ltd
All rights reserved.

Redistribution and use in source and binary forms, with or without modification, are permitted provided that the following conditions are met:

    * Redistributions of source code must retain the above copyright notice, this list of conditions and the following disclaimer.
    * Redistributions in binary form must reproduce the above copyright notice, this list of conditions and the following disclaimer in the documentation and/or other materials provided with the distribution.
    * Neither the name of the Vodafone Group Services Ltd nor the names of its contributors may be used to endorse or promote products derived from this software without specific prior written permission.

THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS" AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE ARE DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT HOLDER OR CONTRIBUTORS BE LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
*/

#include "Renderer/OpenGlEsCompatibility.h"
#include "PALGraphics.h"
#include "Renderer/Image.h"
#include "OpenGLES/GUBIRenderBridge.h"
#include <string>

#define ITOFP(x) ((x)<<16)

GUBIRenderBridge::GUBIRenderBridge()
{
   m_textureID = 0;
   m_imageWidth = 32;
   m_imageHeight = 32;
   m_viewState = gubi::RenderBridgeViewSpace_3D;
   
   m_vertices[ 0] = fmath::tofixed(-0.5f),
   m_vertices[ 1] = fmath::tofixed(-0.5f),
   m_vertices[ 2] = fmath::tofixed(0),
   m_vertices[ 3] = fmath::tofixed(-0.5f),
   m_vertices[ 4] = fmath::tofixed( 0.5f),
   m_vertices[ 5] = fmath::tofixed(0),
   m_vertices[ 6] = fmath::tofixed( 0.5f),
   m_vertices[ 7] = fmath::tofixed( 0.5f),
   m_vertices[ 8] = fmath::tofixed(0),
   m_vertices[ 9] = fmath::tofixed( 0.5f),
   m_vertices[10] = fmath::tofixed(-0.5f),
   m_vertices[11] = fmath::tofixed(0);

   int *c = &m_colors[0];
   (*c++) = ITOFP(1), (*c++) = ITOFP(1), (*c++) = ITOFP(1), (*c++) = ITOFP(1),
   (*c++) = ITOFP(1), (*c++) = ITOFP(1), (*c++) = ITOFP(1), (*c++) = ITOFP(1),
   (*c++) = ITOFP(1), (*c++) = ITOFP(1), (*c++) = ITOFP(1), (*c++) = ITOFP(1),
   (*c++) = ITOFP(1), (*c++) = ITOFP(1), (*c++) = ITOFP(1), (*c++) = ITOFP(1);

   int *s = &m_texcoords[0];
   (*s++) = ITOFP(0), (*s++) = ITOFP(1),
   (*s++) = ITOFP(0), (*s++) = ITOFP(0),
   (*s++) = ITOFP(1), (*s++) = ITOFP(0),
   (*s++) = ITOFP(1), (*s++) = ITOFP(1);
   
   unsigned char *i = &m_indices[0];
   (*i++) = 0, (*i++) = 1, (*i++) = 2,
   (*i++) = 0, (*i++) = 2, (*i++) = 3;

   // create an projection matrix
   glMatrixMode(GL_PROJECTION);
   glLoadIdentity();

   // this equivalent with gluPerspective
   fmath::fixed fov = fmath::static_int2fix<45>::value;
   fmath::fixed aspect = fmath::division(fmath::static_int2fix<480>::value, fmath::static_int2fix<800>::value);
   fmath::fixed near = fmath::static_int2fix<1>::value;
   fmath::fixed far = fmath::static_int2fix<1000>::value;

   fmath::fixed ymin, ymax, xmin, xmax;
   fmath::fixed fov_pi = fmath::multiply(fov, fpi);
   fmath::fixed fov_pi_div_360 = fmath::division(fov_pi, fmath::static_int2fix<360>::value);
   ymax = fmath::multiply(near, fmath::tan(fov_pi_div_360));
   ymin = -ymax;
   xmin = fmath::multiply(ymin,aspect);
   xmax = fmath::multiply(ymax, aspect);
   
   glFrustumx(xmin, xmax, ymin, ymax, near, far);

   glGetFixedv(GL_PROJECTION_MATRIX, m_projMatrix);
   
   // create an ortographic matrix
   glLoadIdentity();

   glFrustumx(ITOFP(-1), ITOFP(1), ITOFP(1), ITOFP(-1), ITOFP(1),  ITOFP(100));

   glGetFixedv(GL_PROJECTION_MATRIX, m_ortoMatrix);
}

GUBIRenderBridge::~GUBIRenderBridge()
{
   
}

void GUBIRenderBridge::begin(gubi::RenderBridgeViewSpace viewSpace)
{
   // set intenal values before rendering starts
   //m_textureID = 0;

   // set some gl state
   //glClearColor(1,0,1,1);
   //glClear(GL_COLOR_BUFFER_BIT);
  
   // is this correct ?
   // glViewport(0, 0, 480, 800);

   // setup the projection matrix
   glMatrixMode(GL_PROJECTION);

   // load the apropriate matrix here
   switch (viewSpace)
   {
      case gubi::RenderBridgeViewSpace_2D:
         {
            glLoadMatrixx(m_ortoMatrix);
         } break;

      case gubi::RenderBridgeViewSpace_3D:
         {
            glLoadMatrixx(m_projMatrix);
         } break;
   }

   // diable lighting and alpha blending
   //glDisable(GL_LIGHTING);
   //glEnable(GL_BLEND);

   glMatrixMode(GL_MODELVIEW);
   glLoadIdentity();
}

void GUBIRenderBridge::end()
{
   
}

void GUBIRenderBridge::bindImage(gubi::IImage *image)
{
   if (image != 0L) {
      // get the size of the image
      m_imageWidth = image->getWidth();
      m_imageHeight = image->getHeight();
      //printf("width %d, height %d\n", m_imageWidth, m_imageHeight);
      
      gubi::Image<GUBIImage> *_image = reinterpret_cast<gubi::Image<GUBIImage>*>(image);
      
      if (_image != NULL) {
         GUBIImage *_gubiImage = _image->getData();

         if (_gubiImage != NULL) {
            m_textureID = _gubiImage->m_tboID;;
         } else {
            printf("could not get native image\n");
         }
      } else {
         printf("could not get pointer to image\n");
      }
   }
}

void GUBIRenderBridge::unbindImage()
{
   glDisable(GL_TEXTURE_2D);
   m_textureID = 0;
}

bool GUBIRenderBridge::freeImage(gubi::IImage *image)
{
   bool result = false;

   if (image != NULL) {
      gubi::Image<GUBIImage> *_image = reinterpret_cast<gubi::Image<GUBIImage>*>(image);
      
      if (_image != NULL) {
         GUBIImage *_gubiImage = _image->getData();
         
         if (_gubiImage != NULL) {
            if (_gubiImage->m_tboID != 0) {
               glDeleteTextures(GL_TEXTURE_2D, &_gubiImage->m_tboID);
            }
            delete _gubiImage;
            _gubiImage = NULL;
            result = true;
         }
      
         delete _image;
         _image = NULL;
      }
   }
   
   return result;
}

bool GUBIRenderBridge::createImage(
    const std::string &filepath,
    const std::string &filename,
    gubi::IImage **image)
{
   bool result = false;
   // PAL
   pal::GraphicsContext palContext;
   pal::wf_uint32 widthPixels, heightPixels;
   pal::ResultBuffer resBuffer;
   pal::pstatus status = pal::PAL_OK;
   
   if (filename.size() > 0 && image != NULL) {
      // load the image...with the PAL stuff...
      status = pal::createGraphicsContext(&palContext);
      
      if (status == pal::PAL_OK) {
         std::string imgPath = filepath + filename;
         FILE *filePointer = fopen(imgPath.c_str(), "rb");
         
         if (filePointer != NULL) {
            printf("openfile %s\n",imgPath.c_str());
            status = pal::loadImageFromFile(palContext,
                                            filePointer,
                                            pal::PNG,
                                            pal::RGBA8,
                                            0, 0,
                                            &widthPixels,
                                            &heightPixels,
                                            &resBuffer);
            if (status == pal::PAL_OK) {
               // we got the image data in resBuffer
               printf("Image size: %ux%u\n", widthPixels, heightPixels);
               GLuint id = 0;
               glGenTextures(1, &id);

               if (id != 0) {
                  glBindTexture(GL_TEXTURE_2D, id);
                  glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA,
                               widthPixels, heightPixels,
                               0, GL_RGBA,
                               GL_UNSIGNED_BYTE, resBuffer.getData());
                  glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
                  glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
                  glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
                  glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
                  glBindTexture(GL_TEXTURE_2D, 0);
                  // set the id to an image/texture object

                  GUBIImage *_image = new GUBIImage;;
                  _image->m_tboID = id;
                  _image->m_width = widthPixels;
                  _image->m_height = heightPixels;
                  gubi::Image<GUBIImage> *_gubiImage = NULL;
                  _gubiImage = new gubi::Image<GUBIImage>(_image, widthPixels, heightPixels);
                  *image = _gubiImage;
                  result = true;
               } else {
                  printf("could not generate OpenGL texture id\n");
               }
            } else {
               printf("could not load file: %s from: %s\n", filename.c_str(), filepath.c_str());
            }
         } else {
            printf("could not create graphics context\n");
         }
      }
   }
   
   return result;
}

void GUBIRenderBridge::pushMatrix(const fmath::fmatrix16 &matrix)
{
   // not actually push it here, just take it for the drawImage..
   // we are pushing it there...
   m_worldMatrix = matrix;
}

void GUBIRenderBridge::popMatrix()
{
   
}

void GUBIRenderBridge::drawImage(
    const fmath::fixed &width,
    const fmath::fixed &height)
{
   glPushMatrix();
   glLoadMatrixx(m_worldMatrix);
   
   glEnableClientState(GL_VERTEX_ARRAY);
   glEnableClientState(GL_COLOR_ARRAY);
   
   if (m_textureID != 0) {
      glEnable(GL_TEXTURE_2D);
      glEnableClientState(GL_TEXTURE_COORD_ARRAY);
      glBindTexture(GL_TEXTURE_2D, m_textureID);
   }
  
   glVertexPointer(3, GL_FIXED, 0, m_vertices);
   glColorPointer(4, GL_FIXED, 0, m_colors);

   if (m_textureID != 0) {
      glTexCoordPointer(2, GL_FIXED, 0, m_texcoords);
   }

   //glTranslatex(0, 0, fmath::tofixed(-15));
   //glScalex(fmath::tofixed(1.0f), fmath::tofixed(1.0f), fmath::tofixed(1));

   glDrawElements(GL_TRIANGLES, 6, GL_UNSIGNED_BYTE, m_indices);
   
   // Do not use glDrawArrays in production, only for debug,
   // this generates indices, and is not the optimal sollution
   //glDrawArrays(GL_TRIANGLES, 0, 3);

   if (m_textureID != 0) {
      glDisableClientState(GL_TEXTURE_COORD_ARRAY);
   }

   glDisableClientState(GL_COLOR_ARRAY);
   glDisableClientState(GL_VERTEX_ARRAY);

   glPopMatrix();
}
