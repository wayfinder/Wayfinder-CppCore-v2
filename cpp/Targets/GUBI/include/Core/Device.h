
#ifndef _DEVICE_H_
#define _DEVICE_H_

#include "Core/IDevice.h"
#include "Common/Reference.h"
#include "Common/SmartPointer.h"
#include "Input/InputManager.h"
#include "Renderer/RenderSystem.h"
#include "Effects/EffectManager.h"
#include "Widget/WidgetManager.h"

namespace gubi {

// Prototypes
class IDrawCallback;
class IViewInternal;

/**
 * Handles the internal resources and process mechanism.
 */
class Device : public gubi::IDevice,
               public gubi::Reference
{
   typedef std::map<gubi::Identifier,
      gubi::SmartPointer<gubi::IViewInternal> > view_map;

public:
   /**
    *
    */
   Device(
      gubi::ITime* pTime,
      gubi::IRenderBridge* pBridge,
      gubi::IDrawCallback* pDrawCallback );

   /**
    * Destructor.
    */
   virtual ~Device(
      void );

   /**
    * Adds a new reference to the object.
    *
    * @return Number of reference to the object.
    */
   unsigned int AddReference(
      void ) const;

   /**
    * Releases a reference to the object.
    *
    * @return Number of reference to the object.
    */
   unsigned int Release(
      void ) const;

   /**
    * @see IDevice::Update
    */
   void Update(
      void );

   /**
    * @see IDevice::Render
    */
   void Render(
      void );

   /**
    * @see IDevice::CreateView
    */
   bool CreateView(
      const gubi::Identifier& eViewName,
      gubi::IView** ppView );

   /**
    * @see IDevice::GetView
    */
   bool GetView(
      const gubi::Identifier& eViewName,
      gubi::IView** ppView );

   /**
    * @see IDevice::CreateWidget
    */
   bool CreateWidget(
      const gubi::Identifier& eWidgetType,
      const gubi::IPropertyBag* pProperties,
      gubi::IWidget** ppWidget );

   /**
    * @see IDevice::CreateWidgetArray
    */
   bool CreateWidgetArray(
      const gubi::Identifier& eWidgetType,
      const gubi::IPropertyBag* pProperties,
      unsigned int iCount,
      gubi::IWidgetArray** ppWidgetArray );

   /**
    * @see IDevice::CreateEffect
    */
   bool CreateEffect(
      const gubi::Identifier& eEffectType,
      gubi::IPropertyBag* pPropertyBag,
      const gubi::EffectStartOption& eStartOption,
      gubi::IEffect** ppEffect );

   void OnPointerDown(
      unsigned int iIndex,
      int x,
      int y );

   void OnPointerMove(
      unsigned int iIndex,
      int x,
      int y );

    void OnPointerUp(
      unsigned int iIndex,
      int x,
      int y );

private:
   SmartPointer<gubi::ITime> m_pTime;
   gubi::IDrawCallback*      m_pDrawCallback;
   gubi::RenderSystem        m_renderSystem;
   gubi::EffectManager       m_effectManager;
   gubi::WidgetManager       m_widgetManager;
   gubi::InputManager        m_inputManager;
   view_map                  m_mapView;
};

}

#endif // _DEVICE_H_
