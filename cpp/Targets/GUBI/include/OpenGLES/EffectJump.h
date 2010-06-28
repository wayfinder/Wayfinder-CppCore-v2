
#ifndef EFFECT_JUMP_H
#define EFFECT_JUMP_H

// Forward declarations
namespace gubi {
class IDevice;
class IWidget;
class IEffect;
class IPropertyBag;
} // namespace gubi

class EffectJump {
public:
   EffectJump(gubi::IDevice *device,
              gubi::IWidget *widget,
              float startX, float startY, float startZ,
              float endX, float endY, float endZ);
  
   ~EffectJump();

   bool init(int delay = 0);
  
private:
   /* effect callbacks */
   void lerpFirst(gubi::IPropertyBag *bag);
   void lerpSecond(gubi::IPropertyBag *bag);
   void lerpThird(gubi::IPropertyBag *bag);

   // new callbacks
   void waveUpdate(gubi::IPropertyBag *bag);
   void interpolateUpdate(gubi::IPropertyBag *bag);
   void interpolateFinish(gubi::IPropertyBag *bag);
   void interpolateStart(gubi::IPropertyBag *bag);
   
   float m_start_position[3];
   float m_end_position[3];

   // Millisec
   int m_lerpTimeFirst;
   int m_lerpTimeSecond;
   int m_lerpTimeThird;

   bool m_is_lerp_third_running;
   bool m_is_lerp_third_created;

   gubi::IWidget *m_widget;
   gubi::IDevice *m_device;
   gubi::IEffect *m_effect;

   gubi::IEffect *m_effectLerpFirst;
   gubi::IEffect *m_effectLerpSecond;
   gubi::IEffect *m_effectLerpThird;
};

#endif // EFFECT_JUMP_H
