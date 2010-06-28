#ifndef IMAGEMAPMODULETEST_H
#define IMAGEMAPMODULETEST_H
#include "WindowEventListener.h"
#include "RegressionTest.h"
#include "Map/MapEventListener.h"
#include "AppEventListener.h"
#include <vector>

class ImageMapModule;
class MapProjection;
class BufferRequester;
class AppWindow;
class MapOperationHandler;
class NullDrawingInterface;
class MapManipulator;
class MapLibKeyHandler;
class MapParamFactory;
class ImageMapControl;
namespace WFAPI {
class MapLibNetworkContext;
}

class ImageMapModuleTest : public RegressionTest,
                           public AppEventListener {
public:
   ImageMapModuleTest(Nav2Session& session, const char* name);
   
   virtual ~ImageMapModuleTest();

   virtual void startTest();

   virtual void onExitRequest(int code);
private:
   AppWindow* m_window;

   WFAPI::MapLibNetworkContext* m_netContext;

   ImageMapControl* m_control;
};

#endif /* IMAGEMAPMODULETEST_H */ 
