#ifndef _EFFECT_STATE_H_
#define _EFFECT_STATE_H_

namespace gubi {

enum EffectState
{
   EffectState_Unknown,
   EffectState_Idle,
   EffectState_Start,
   EffectState_Run,
   EffectState_Finish,
   EffectState_Paused,
   EffectState_Remove
};

}

#endif // _EFFECT_STATE_H_
