#ifndef WINDOWBRIDGE_H
#define WINDOWBRIDGE_H

#include "Window/AppWindow.h"

@class EAGLView;
#ifdef DISABLE_WF

class WindowBridge {
   
};

#else //DISABLE_WF

/**
 *  This class acts as a bridge between the Objective C
 *  window (EAGLView most often) and our C++ AppWindow
 *  class. It simply forwards methods to the ObjC class.
 *
 */ 
class WindowBridge : public AppWindow {
public:
   WindowBridge() : m_view(NULL)
      {}

   void setView(EAGLView* view);

   virtual void requestDraw();

   virtual WFAPI::DrawingContext* getDrawingContext();

   /**
    *  Returns the width of the widget in pixels.
    *
    *  @return width of widget in pixels.
    */ 
   virtual unsigned int getWidthPixels();

   /**
    *  Returns the height of the widget in pixels.
    *
    *  @return width of widget in pixels.
    */    
   virtual unsigned int getHeightPixels();

private:
   EAGLView* m_view;
};
#endif // DISABLE_WF

#endif /* WINDOWBRIDGE_H */
