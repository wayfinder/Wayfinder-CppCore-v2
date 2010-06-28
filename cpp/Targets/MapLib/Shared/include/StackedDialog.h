/*
Copyright (c) 1999 - 2010, Vodafone Group Services Ltd
All rights reserved.

Redistribution and use in source and binary forms, with or without modification, are permitted provided that the following conditions are met:

    * Redistributions of source code must retain the above copyright notice, this list of conditions and the following disclaimer.
    * Redistributions in binary form must reproduce the above copyright notice, this list of conditions and the following disclaimer in the documentation and/or other materials provided with the distribution.
    * Neither the name of the Vodafone Group Services Ltd nor the names of its contributors may be used to endorse or promote products derived from this software without specific prior written permission.

THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS" AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE ARE DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT HOLDER OR CONTRIBUTORS BE LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
*/


#ifndef STACKEDDIALOG_H
#define STACKEDDIALOG_H

#include "StackedDialogVisualSpec.h"
#include "MapPlotter.h"
#include "MapProjection.h"
#include "WGS84Coordinate.h"
#include "MC2Point.h"
#include "TileMapToolkit.h"

class OverlayItemInternal;
class SelectedMapObjectNotifier;
class MapDrawingInterfaceInternal;
class OverlayView;
class OverlayDrawer;
class MapProjectionAdapter;

namespace WFAPI {
class OverlayItem;
}

/**
 * A stacked dialog represents the dialog that is shown when
 * a user has clicked on a stacked item, i.e. a composite of
 * items. This dialog class supports scrolling of all the items
 * as well as clicking.
 *
 * This class governs the internal logic of the dialog. The triggering
 * of states and events are dealt with by the input handler of the
 * OverlayView. 
 *
 */ 
class StackedDialog {
public:
   StackedDialog(OverlayView* view,
                 TileMapToolkit* toolkit,
                 MapDrawingInterfaceInternal* mapDrawingInterface,
                 const MapProjectionAdapter& projection);
   
   ~StackedDialog();

   /**
    * Specifies the visual spec that the dialog will use for
    * geometry calculations.
    */ 
   void setVisualSpec(const WFAPI::StackedDialogVisualSpec* dialogVisualSpec,
                      unsigned int numOfItems);
   
   /**
    * Draw this stack dialog using the specified plotter.
    *
    * @param drawer The drawer to use.
    */ 
   void draw(OverlayDrawer* drawer);

   void drawPartial(OverlayDrawer* drawer);

   /**
    * Enables the dialog with the specified item as item source.
    *
    * @param The item to display in the dialog.
    * @return If the dialog could be enabled (i.e. there is a
    *         visual spec for the given number of items);
    */ 
   void enableDialog(OverlayItemInternal* item);

   /**
    * Disables the dialog, cancelling all animations and events.
    */ 
   
   void disableDialog();

   /**
    * Scroll the list of items by deltaX pixels.
    *
    * @param deltaX The amount of pixels to scroll by.
    */ 
   bool scroll(int deltaX);

   /**
    * Returns the current speed of the flicking animation in pixels
    * per seconds.
    *
    * @return The speed of the flicking animation.
    */ 
   float currentFlickingSpeedPxPs();

   /**
    * Initiates a flicking animation. The initial velocity will be
    * as specified. The velocity will decay as time goes by.
    *
    * @param initialVelocityPxPs Initial velocity in pixels per second.
    */ 
   void startFlicking(float initialVelocityPxPs);

   /**
    * @return True if we are currently doing a flicking animation.
    */ 
   bool isFlicking();

   /**
    * Terminates the active flicking animation.
    */ 
   void endFlicking();

   /**
    * @return The pixel box of the dialog.
    */ 
   PixelBox getPixelBox();

   /**
    * @return The overlay item which corresponds to a click on the supplied
    *         point.
    */ 
   const WFAPI::OverlayItem* getSelectedItem(const WFAPI::ScreenPoint& point);

   /**
    * Called when the periodic timer has been expired, which is used for
    * animation of flicking states and so forth.
    */ 
   void  timerExpired(uint32);

   bool isValidFor(const OverlayItemInternal* item);
private:

   /**
    * Point is the topleft point of an item in the dialog. If the
    * active drawing area of items is defined by pixelBox, this
    * method determines if we should draw the item or not. Since
    * we draw partial items, point can be offset by an an entire
    * items worth of pixels before it is considered as a no-draw.
    */ 
   
   bool isVisible(const MC2Point& point,
                  const PixelBox& pixelBox);

   /**
    * Used to implement a carousel like effect where the list of items
    * reappears once it goes completely outside one of the borders.
    */ 
   struct RectWrappingState {
      int leftX;     /// The left side of the first (left) list of items.
      int rightX;    /// The left side of the second (right) list of items.
      int width;     /// The width of the list of items.
   };

   /**
    * The state of the flicking animation.
    */ 
   struct FlickingState {
      float curVelPxPs;
      unsigned int lastTime;
      float decelerationConstant;
   };

   /**
    * Returns true if the item which has its leftmost X-position at
    * itemPosX overlaps the click at clickPosition.
    */ 
   bool clickOverlapsItem(int itemPosX,
                          const MC2Point& clickPosition);

   /**
    * Sets up the geometry variables.
    */
   void setupGeometry(unsigned int numOfItems);

   /**
    * Sets up the item offset for a new stacked item.
    */ 
   void setupItemOffset();

   /**
    * Updates the set of positions for items.
    */ 
   void updatePositions();

   /**
    * Updates the delta of the positions, normally happens after a scroll
    * event occurs.
    */ 
   bool updatePositionsDelta(int deltaX);

   /**
    * Returns the index of the specification (and corresponding geometry)
    * that should be used for the supplied item.
    *
    * @param item The item that is to be used.
    * @return The index to the specification to be used. If no specifications
    *         are found, -1 will be returned.
    */
   int getItemSpecIndex(const OverlayItemInternal* item) const;

   /// Our overlay view parent
   OverlayView* m_view;
   
   /// Used to trigger redraws
   MapDrawingInterfaceInternal* m_mapDrawingInterface;

   /// Used to project screen positions from world coordinates
   const MapProjectionAdapter& m_projection;

   typedef std::map<unsigned int,
                    const WFAPI::StackedDialogVisualSpec*> SpecMap;
   
   /// Specifies the geometry of the items.
   SpecMap m_dialogSpecs;

   /// The world position of the stacked dialog, i.e. the point that
   /// it focuses on.
   WFAPI::WGS84Coordinate m_position;

   /// The item that we are currently observing in the stacked dialog
   OverlayItemInternal* m_item;

   /// The positions of the items in the stacked dialog.
   std::vector<MC2Point> m_itemPositions;

   /// Describes the internal geometry of the dialog
   struct GeometryState {
      int dialogWidth;       /// The width of the entire dialog.
      int dialogHeight;      /// The height of the entire dialog.
      int itemDialogWidth;   /// The width of the area containing items
      int itemDialogHeight;  /// The height of the area containing items
      int itemWidth;         /// The width of a single item inside the dialog.
      int itemHeight;        /// The height of a single item inside the dialog.
      int itemSpacing;       /// The spacing between center points of items
      int itemPosY;          /// The y-position of the items, alway same
      int lowerBoundX;       /// The left x-position of the dialog area
      int upperBoundX;       /// The right x-position of the dialog area
   };

   /// Our instance of the geometry state
   std::map<unsigned int, GeometryState> m_geometry;

   /// Our instance of the rectangle wrapping state
   RectWrappingState m_rectState;

   /// Our instance of the flicking state
   FlickingState m_flicking;

   /// Periodic timer for flicking motion updates
   TileMapPeriodicTimer* m_timer;

   /// Listener
   TileMapTimerListenerTemplate<StackedDialog>* m_timerListener;
};

#endif // STACKEDDIALOG_H
