
#ifndef _IWIDGET_DESTRUCTOR_H_
#define _IWIDGET_DESTRUCTOR_H_

namespace gubi {

class IWidgetInternal;

class IWidgetDestructor
{
public:
   virtual void OnWidgetDestroy(
      const gubi::IWidgetInternal* pWidget ) = 0;
};

}

#endif // _IWIDGET_DESTRUCTOR_H_
