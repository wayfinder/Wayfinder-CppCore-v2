
#ifndef _VIEW_H_
#define _VIEW_H_

#include <vector>
#include "View/IViewInternal.h"
#include "Widget/IWidget.h"
#include "Common/Reference.h"
#include "Common/SmartPointer.h"
#include "Property/PropertyBag.h"
#include "Signal/SignalCollection.h"

namespace gubi {

class View : public gubi::IViewInternal,
             public gubi::Reference,
             public gubi::SignalCollection
{
public:
   View(
      void );

   ~View(
      void );

   unsigned int AddReference(
      void ) const;
   
   unsigned int Release(
      void ) const;

   bool GetProperties(
      gubi::IPropertyBag** ppProperties );
   
   bool GetProperties(
      const gubi::IPropertyBag** ppProperties ) const;

   bool GetSignal(
      const gubi::Identifier& eSignalId,
      gubi::ISignalHub** ppSignalHub );

   bool Show(
      const gubi::ViewShowFlag& eFlag );

   virtual void Hide(
      void );

   bool AddWidget(
      gubi::IWidget* pWidget,
      const gubi::ViewSpace& eViewSpace );

   bool AddWidgetArray(
      gubi::IWidgetArray* pWidgetArray,
      const gubi::ViewSpace& eViewSpace );

   bool FetchNodes(
      const gubi::ViewSpace& eViewSpace,
      gubi::RenderQueue& renderQueue );

   bool Visible(
      void );

private:
   bool                                      m_bVisible;
   std::vector<SmartPointer<gubi::IWidget> > m_vRoots[2];
};

}

#endif // _VIEW_H_
