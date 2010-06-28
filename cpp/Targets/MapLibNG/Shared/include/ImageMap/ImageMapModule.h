#ifndef IMAGEMAPMODULE_H
#define IMAGEMAPMODULE_H

#include "MapProjection/MapProjection.h"
#include "BufferRequester/BufferRequesterListener.h"
#include "Map/MapModule.h"
#include "Util/NotCopyable.h"
#include "WorkDispatching/WorkListener.h"

class MapParam;
class BufferRequester;
class ExternalRenderer;
class MC2SimpleString;
class ImageTile;
class ExternalRenderer;
class MapParamFactory;
class TextureBlock;

namespace WFAPI {
class MapDrawingCallback;
class DrawingContext;
}

/**
 *    Class that is responsible for initiating drawing and requesting of image maps.
 */
class ImageMapModule : public MapProjectionListener,
                       public MapModule,
                       public BufferRequesterListener,
                       public WorkListener {
public:

   /**
    *  Constructor.
    *
    *  @param projection The map projection
    *  @param factory The factory used to form request strings.
    *  @param bufferRequester The buffer requester.
    *  @param drawContext The draw context.
    */
   ImageMapModule(MapProjection& projection,
                  MapParamFactory* factory,
                  BufferRequester& bufferRequester,
                  ExternalRenderer* renderer);

   /**
    *  Explicitly force this module to redraw itself immediately.
    *  Used when external forces requires this.
    */ 
   void redraw();
      
   /**
    *  @see MapProjectionListener.
    */
   virtual void projectionUpdated(projectionEvent trigger);

   /**
    *  @see BufferRequesterListener.
    */
   void requestReceived(const MC2SimpleString& descr,
                        BitBuffer* dataBuffer,
                        OriginType origin);

   /**
    *  Updates the parameter factory to use. Will clear
    *  internal storage etc to accomodate for new tiles.
    *
    *  @param factory The new factory to use.
    */ 
   void setMapParamFactory(MapParamFactory* factory);

   /**
    *  Draws this module based on its internal state.
    *
    */ 
   void draw();

   void setMapDrawingCallback(WFAPI::MapDrawingCallback* callback);


   
   /**
    *  Called when the PNG thread has finished loading a texture.
    *  If a tile request, load it into a tile. If not, examine
    *  string 
    */ 
   void onWorkDone(void* work);
private:
   void requestParams(vector<MapParam>& reqParams);

   /**
    *  Issues a drawing request to the responsible control.
    */ 
   void requestDraw();

private:
   struct Impl;
   Impl* m;
};

#endif /* IMAGEMAPMODULE_H */
