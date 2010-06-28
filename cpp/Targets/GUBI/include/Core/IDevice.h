
#ifndef _IDEVICE_H_
#define _IDEVICE_H_

#include "Common/IReference.h"
#include "Common/Identifier.h"
#include "Core/DeviceFlags.h"
#include "Effects/EffectStartOption.h"

namespace gubi {

// Prototypes
class ITime;
class IPropertyBag;
class IView;
class IWidget;
class IWidgetArray;
class IEffect;
class IDrawCallback;
class IRenderBridge;

/**
 * Core interface for the GUBI library.
 */
class IDevice : public gubi::IReference
{
public:
   virtual void Update(
      void ) = 0;

   virtual void Render(
      void ) = 0;

   virtual bool CreateView(
      const gubi::Identifier& eViewName,
      gubi::IView** ppView ) = 0;

   virtual bool GetView(
      const gubi::Identifier& eViewName,
      gubi::IView** ppView ) = 0;

   virtual bool CreateWidget(
      const gubi::Identifier& eWidgetType,
      const gubi::IPropertyBag* pProperties,
      gubi::IWidget** ppWidget ) = 0;

   virtual bool CreateWidgetArray(
      const gubi::Identifier& eWidgetType,
      const gubi::IPropertyBag* pProperties,
      unsigned int iCount,
      gubi::IWidgetArray** ppWidgetArray ) = 0;

   virtual bool CreateEffect(
      const gubi::Identifier& eEffectType,
      gubi::IPropertyBag* pPropertyBag,
      const gubi::EffectStartOption& eStartOption,
      gubi::IEffect** ppEffect ) = 0;

   virtual void OnPointerDown(
      unsigned int iIndex,
      int x,
      int y ) = 0;

   virtual void OnPointerMove(
      unsigned int iIndex,
      int x,
      int y ) = 0;

   virtual void OnPointerUp(
      unsigned int iIndex,
      int x,
      int y ) = 0;
};

/**
 * Initializes and creates a new device instance.
 */
bool CreateDevice(
   const gubi::DeviceFlags& eDeviceFlags,
   gubi::ITime* pTime,
   gubi::IRenderBridge *pBridge,
   gubi::IDrawCallback* pDrawCallback,
   gubi::IDevice** ppDevice );

}

#endif // _IDEVICE_H_
