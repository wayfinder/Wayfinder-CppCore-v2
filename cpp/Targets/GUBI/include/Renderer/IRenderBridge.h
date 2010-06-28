
#ifndef IRENDER_BRIGDE_H
#define IRENDER_BRIGDE_H

#include "fmath.h"
#include "IRenderBridgeEnums.h"

namespace gubi {

class IImage;
   
class IRenderBridge {
public:
   IRenderBridge() {}
   virtual ~IRenderBridge() {}
   
   virtual void begin(gubi::RenderBridgeViewSpace viewSpace) = 0;

   virtual void end() = 0;

   virtual void bindImage(gubi::IImage *image) = 0;

   virtual void unbindImage() = 0;

   virtual bool freeImage(gubi::IImage *image) = 0;
   
   virtual bool createImage(
       const std::string &filepath,
       const std::string &filename,
       gubi::IImage **image) = 0;

   virtual void pushMatrix(const fmath::fmatrix16 &matrix) = 0;
   virtual void popMatrix() = 0;

   virtual void drawImage(
       const fmath::fixed &width,
       const fmath::fixed &height) = 0;
};

} // namespace gubi

#endif // IRENDER_BRIDGE_H
