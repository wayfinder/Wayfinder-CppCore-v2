#ifndef GTKTESTWINDOW_H
#define GTKTESTWINDOW_H

#include <gtk/gtk.h>
#include "Window/AppWindow.h"
#include "MapDrawingCallback.h"
#include <map>
#include "MapLibKeyInterface.h"
#include "KeyEvent.h"

class WindowEventListener;

namespace WFAPI {
class GtkGLESDrawer;
class ScreenPoint;
}

/**
 *  Simple Gtk window for map display.
 *  This class is aimed as an example for how to 
 *  easily instantiate a map.
 */
class GtkAppWindow : public AppWindow {
public:
   /**
    *  Constructor.
    */
   GtkAppWindow();

   /**
    *  Destructor.
    */
   virtual ~GtkAppWindow();

   /**
    *  @see AppWindow::getDrawingContext
    */
   WFAPI::DrawingContext* getDrawingContext();

   /**
    *  Handle key events from gtk.
    *
    *  @param widget The gtk widget.
    *  @param event The key event.
    *  @return If the key event was handled by this class.
    */
   bool handleKey(GtkWidget* widget, GdkEventKey* event);

   /**
    *  This method is called when gtk determines that the window 
    *  should be redrawn.
    *
    *  @param event The expose event.
    */
   void expose( GdkEventExpose* event );

   /**
    *   @see AppWindow::requestDraw
    */ 
   void requestDraw();

   /**
    *   @see AppWindow::getWidthPixels
    */
   unsigned int getWidthPixels();

   /**
    *   @see AppWindow::getHeightPixels
    */
   unsigned int getHeightPixels();
private:

   /**
    *  Initialize the window.
    */
   void initWindow();
   
   /**
    *  Initialize the map drawer.
    */
   void initDrawer();
   
   /**
    *   Initializes the key map which is used to translate native
    *   GDK key events to platform agnostic key codes.
    */
   void initKeyMap();

   /**
    *   The gtk window.
    */
   GtkWidget* m_window; 
   
   /**
    *   The gtk map area.
    */
   GtkWidget* m_mapArea;
   
   /**
    *   The map area container.
    */
   GtkWidget* m_mapAreaContainer;
   
   /**
    *  The map drawer.
    */
   WFAPI::GtkGLESDrawer* m_glesDrawer;

public:
   /**
    * Start drag map mode..
    * @param button of the drag device.
    * @param pos position of the drag device.
    */
   void startDrag( int button, const WFAPI::ScreenPoint& pos );

   /**
    * End drag map mode.
    * @param button of the drag device.
    * @param pos position of the drag device.
    */
   void endDrag( int button, const WFAPI::ScreenPoint& pos );

   /**
    * Continue drag map to position
    * @param button button of the drag device 
    * @param pos position of the drag device
    */
   void drag( int button, const WFAPI::ScreenPoint& pos );

private:   
   /**
    * Do the actual drag action.
    * @param button the button of the drag event.
    * @param upordown up, down or repeat mode.
    * @param pos position of the drag event.
    */
   void dragAction( int button, int upordown, const WFAPI::ScreenPoint& pos );

   /// The map that stores the lookup information for the key
   /// translation.
   typedef std::map<int, KeyEvent::Code> KeyMap;
   KeyMap m_keyMap;

   /// Previous screen width, used to identify when screen is resized.
   int m_prevScreenWidth;
   
   /// Previous screen height, used to identify when screen is resized.
   int m_prevScreenHeight;
};

#endif /* GTKTESTWINDOW_H */
