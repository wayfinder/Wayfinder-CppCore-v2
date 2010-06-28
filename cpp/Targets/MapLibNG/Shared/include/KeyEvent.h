#ifndef KEYEVENT_H
#define KEYEVENT_H

/*
*  The intention is that the view (represented by for instance a GTK window
*  class) supplies input events through a WindowEventListener to the
*  controller. Some of these input events use this struct.
*/
struct KeyEvent {
   /**
    *  This enumeration type represents a key code received from an input
    *  source (for instance a window). It is not platform dependant, and its
    *  purpose is to have the ability to create cross-platform controllers.
    *
    */
   enum Code {
      ML_F1,
      ML_F2,
      ML_F3,
      ML_F4,
      ML_F5,
      ML_F6,
      ML_F7,
      ML_F8,
      ML_F9,
      ML_Plus,
      ML_Minus,
      ML_Up,
      ML_Down,
      ML_Left,
      ML_Right,
      ML_Space,
      ML_PageUp,
      ML_PageDown,
      ML_a,
      ML_b,
      ML_c,
      ML_d,
      ML_e,
      ML_f,
      ML_g,
      ML_h,
      ML_i,
      ML_j,
      ML_k,
      ML_l,
      ML_m,
      ML_n,
      ML_o,
      ML_p,
      ML_q,
      ML_r,
      ML_s,
      ML_t,
      ML_u,
      ML_v,
      ML_w,
      ML_x,
      ML_y,
      ML_z
   }; // End of enum enum_t

   enum Type {
      KEY_DOWN,
      KEY_UP
   };

   Type type;
   Code code;
};

#endif /* KEYEVENT_H */
