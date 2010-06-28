
#ifndef IIMAGE_H
#define IIMAGE_H

#include "Core/IObject.h"

namespace gubi {

class IImage : public IObject {
public:

   virtual unsigned int getWidth() = 0;

   virtual unsigned int getHeight() = 0;
};
      
} // namespace gubi

#endif // IIMAGE_H
