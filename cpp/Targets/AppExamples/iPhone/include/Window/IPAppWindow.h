#ifndef IPAPPWINDOW_H
#define IPAPPWINDOW_H

#include "Window/AppWindow.h"

class IPAppWindow : public AppWindow {
public:
   /**
    *  Constructor.
    */
   IPAppWindow();   

   /**
    *  Requests that the window should be redrawn.
    */ 
   virtual void requestDraw() ;
   
   /**
    *  Get the drawing context.
    * 
    *  @return The drawing context.
    */ 
   virtual WFAPI::DrawingContext* getDrawingContext();
      
   /**
    *  Destructor.
    */
   virtual ~IPAppWindow() {}
private:
   
};


#endif /* IPAPPWINDOW_H */
