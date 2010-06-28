#include "EAGLView.h"
#include "WindowBridge.h"

void WindowBridge::requestDraw()
{
   if(m_view) {
      // No need, will be notified when displaylink triggers
      // [m_view drawView:nil];         
   } 
}

WFAPI::DrawingContext* WindowBridge::getDrawingContext()
{
   if(m_view) {
      WFAPI::DrawingContext* ret = [m_view getDrawingContext];
      return ret;
   } else {
      return NULL;
   }
    
}

void WindowBridge::setView(EAGLView* view)
{
   m_view = view; 
}

unsigned int WindowBridge::getWidthPixels()
{
   return [m_view getWidth];
}

unsigned int WindowBridge::getHeightPixels()
{
   return [m_view getHeight];
}
