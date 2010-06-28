
#ifndef _IVIEW_INTERNAL_H_
#define _IVIEW_INTERNAL_H_

#include "View/IView.h"
#include "Renderer/RenderQueue.h"

namespace gubi {

class IViewInternal : public gubi::IView
{
public:
   virtual bool FetchNodes(
      const gubi::ViewSpace& eViewSpace,
      gubi::RenderQueue& renderQueue ) = 0;

   virtual bool Visible(
      void ) = 0;
};

}

#endif // _IVIEW_INTERNAL_H_
