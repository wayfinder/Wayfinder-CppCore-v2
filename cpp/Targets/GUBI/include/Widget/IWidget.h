
#ifndef _IWIDGET_H_
#define _IWIDGET_H_

#include "Renderer/RenderNode.h"
#include "Signal/ISignalCollection.h"

namespace gubi {

class IRenderFunction;
class IPropertyBag;

class IWidget : public gubi::RenderNode,
                public gubi::ISignalCollection
{
public:
   virtual void setSize(
      fmath::fixed width,
      fmath::fixed height ) = 0;

   virtual void getSize(
      fmath::fixed& width,
      fmath::fixed& height ) = 0;
};

}

#endif // _IWIDGET_H_
