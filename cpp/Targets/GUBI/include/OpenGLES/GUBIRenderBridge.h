
#ifndef GUBI_RENDER_BRIDGE_H
#define GUBI_RENDER_BRIDGE_H

#include "Renderer/IRenderBridge.h"

/*
 * A simple image wrapper
 */
class GUBIImage {
public:
   GUBIImage()
   {
      m_tboID = 0;
      m_width = 0;
      m_height = 0;
   }
   ~GUBIImage() {}
   
   unsigned int getWidth()
   {
      return m_width;
   }

   unsigned int getHeight()
   {
      return m_height;
   }

   unsigned int getID()
   {
      return m_tboID;
   }
private:
   unsigned int m_tboID;
   unsigned int m_width;
   unsigned int m_height;

   friend class GUBIRenderBridge;
};

class GUBIRenderBridge : public gubi::IRenderBridge {
public:
   GUBIRenderBridge();
   ~GUBIRenderBridge();
   
   void begin(gubi::RenderBridgeViewSpace viewSpace);

   void end();

   void bindImage(gubi::IImage *image);

   void unbindImage();

   bool freeImage(gubi::IImage *image);
   
   bool createImage(
       const std::string &filepath,
       const std::string &filename,
       gubi::IImage **image);

   void pushMatrix(const fmath::fmatrix16 &matrix);
   void popMatrix();

   void drawImage(
       const fmath::fixed &width,
       const fmath::fixed &height);

private:
   unsigned int m_textureID;
   unsigned int m_imageWidth;
   unsigned int m_imageHeight;

   int m_vertices[12];
   int m_colors[16];
   int m_texcoords[8];
   unsigned char m_indices[6];

   fmath::fmatrix16 m_projMatrix;
   fmath::fmatrix16 m_ortoMatrix;
   fmath::fmatrix16 m_worldMatrix;

   gubi::RenderBridgeViewSpace m_viewState;
};


#endif // GUBI_RENDER_BRIDGE_H
