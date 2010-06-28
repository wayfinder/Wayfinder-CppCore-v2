/*
  Copyright (c) 1999 - 2010, Vodafone Group Services Ltd
  All rights reserved.

  Redistribution and use in source and binary forms, with or without modification, are permitted provided that the following conditions are met:

  * Redistributions of source code must retain the above copyright notice, this list of conditions and the following disclaimer.
  * Redistributions in binary form must reproduce the above copyright notice, this list of conditions and the following disclaimer in the documentation and/or other materials provided with the distribution.
  * Neither the name of the Vodafone Group Services Ltd nor the names of its contributors may be used to endorse or promote products derived from this software without specific prior written permission.

  THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS" AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE ARE DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT HOLDER OR CONTRIBUTORS BE LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
*/

#ifndef WFAPI_STACKEDDIALOGVISUALSPEC_H
#define WFAPI_STACKEDDIALOGVISUALSPEC_H

#include "ScreenPoint.h"
#include "RefCounted.h"
#include <vector>

namespace WFAPI {

class ImageSpec;
class OverlayItemVisualSpec;

/**
 * StackedDialogVisualSpec
 * 
 * Class defining the visual specification of a stacked dialog which
 * consists of a background image, arrows for cycling among the available
 * entries and a set of positions for the overlay items that will appear
 * in the dialog.
 * 
 * \verbatim
         +-----------------------------------------------+
         |      +---------------------------------+      |
         |      |                                 |      |
         | <|   |                                 |   |> |
         |      |                                 |      |
         |      +---------------------------------+      | 
         +-----------------------------------------------+
  \endverbatim
  
 * There is an area of the stacked dialog that will contain overlay
 * items. This area is defined by the two diagonal screenpoints
 * itemUpperLeftPoint, itemLowerRightPoint. The argument numItems
 * defines how many items will be displayed inside this box. The
 * items will be placed proportionally inside it and their positions
 * will be dependant upon how many items are displayed.
 *
 * All ScreenPoints that are used in this class are referring to
 * the offset relative to the upper left corner of the background image.
 * I.e. ScreenPoint(10,10) refers to the position 10 pixels down and
 * 10 pixels to the right of the upper left corner of the background image.
 *
 * For configurations with more than one OverlayItem position, MapLib
 * will allow either the first or last defined position to be empty, to
 * denote that there are no more items in the stack in those directions. 
 */
class StackedDialogVisualSpec : public RefCounted {
public:
   
   /**
    * Creates a new stacked dialog.
    *
    * Initially the reference count will be set to 1. Once you are done
    * with this object, you will need to call removeReference to ensure
    * that the object is properly deallocated.
    *
    * @param dialogBackground The background image of the stacked dialog.
    * @param focusPoint The focus point of the backgrond, i.e. this is the 
    *                   point of the image that should be focused on
    *                   a certain coordinate on the map (i.e. the tip).
    * @param itemsUpperLeftPoint The upper left point of the rectangle
    *                           indicating where the items will be drawn.
    * @param itemsLowerRightPoint The lower right point of the rectangle
    *                            indicating where the items will be drawn.
    * @param leftArrowImg The image representing the left arrow
    * @param leftArrowCenterPos The center position of the left arrow
    * @param rightArrowImg The image representing the right arrow
    * @param rightArrowCenterPos The center position of the right arrow
    * @param visualSpec The visual specification which describes how
    *                   OverlayItems should be rendered inside the
    *                   stack view dialog.
    */
   static StackedDialogVisualSpec* allocate(
      const ImageSpec* dialogBackground,
      const ScreenPoint& focusPoint,
      const ScreenPoint& itemsUpperLeftPoint,
      const ScreenPoint& itemsLowerRightPoint,
      const ImageSpec* leftArrowImg,
      const ScreenPoint& leftArrowCenterPos,
      const ImageSpec* rightArrowImg,
      const ScreenPoint& rightArrowCenterPos,
      const OverlayItemVisualSpec* visualSpec);

   /**
    * Creates a new stacked dialog. With a separate OverlayItemVisualSpec for
    * items being tapped.
    *
    * Initially the reference count will be set to 1. Once you are done
    * with this object, you will need to call removeReference to ensure
    * that the object is properly deallocated.
    *
    * @param dialogBackground The background image of the stacked dialog.
    * @param focusPoint The focus point of the backgrond, i.e. this is the 
    *                   point of the image that should be focused on
    *                   a certain coordinate on the map (i.e. the tip).
    * @param itemsUpperLeftPoint The upper left point of the rectangle
    *                           indicating where the items will be drawn.
    * @param itemsLowerRightPoint The lower right point of the rectangle
    *                            indicating where the items will be drawn.
    * @param leftArrowImg The image representing the left arrow
    * @param leftArrowCenterPos The center position of the left arrow
    * @param rightArrowImg The image representing the right arrow
    * @param rightArrowCenterPos The center position of the right arrow
    * @param visualSpec The visual specification which describes how
    *                   OverlayItems should be rendered inside the
    *                   stack view dialog.
    * @param tappedVisualSpec The visual specification which describes how
    *                         OverlayItems should be rendered inside the
    *                         stack view dialog when being tapped.
    */
   static StackedDialogVisualSpec* allocate(
      const ImageSpec* dialogBackground,
      const ScreenPoint& focusPoint,
      const ScreenPoint& itemsUpperLeftPoint,
      const ScreenPoint& itemsLowerRightPoint,
      const ImageSpec* leftArrowImg,
      const ScreenPoint& leftArrowCenterPos,
      const ImageSpec* rightArrowImg,
      const ScreenPoint& rightArrowCenterPos,
      const OverlayItemVisualSpec* visualSpec,
      const OverlayItemVisualSpec* tappedVisualSpec);


   /**
    * @return The background image of the stacked dialog.
    */
   const ImageSpec* getDialogBackgroundImage() const;

   /**
    * @return The visual specification which describes how
    *         OverlayItems should be rendered inside the
    *         stack view dialog.
    */
   const OverlayItemVisualSpec* getItemVisualSpec() const;
   
   /**
    * @return The visual specification which describes how
    *         OverlayItems should be rendered inside the
    *         stack view dialog when being tapped.
    */
   const OverlayItemVisualSpec* getTappedItemVisualSpec() const;

   /**
    * @return The focus point of the backgrond, i.e. this is the 
    *         point of the image that should be focused on
    *         a certain coordinate on the map (i.e. the tip).
    */ 
   ScreenPoint getFocusPoint() const;

   /**
    * @return The upper left point of the rectangle
    *         indicating where the items will be drawn.
    */
   ScreenPoint getItemsUpperLeftPoint() const;

   /**
    * @return The lower right point of the
    *         rectangle indicating where the
    *         items will be drawn.
    */                            
   ScreenPoint getItemsLowerRightPoint() const;

   /**
    * @return The center position of the left arrow.
    */ 
   ScreenPoint getLeftArrowCenterPos() const;

   /**
    * @return The image representing the left arrow.
    */ 
   const ImageSpec* getLeftArrowImage() const;

   /**
    * @return The center position of the right arrow.
    */ 
   ScreenPoint getRightArrowCenterPos() const;

   /**
    * @return The image representing the right arrow.
    */ 
   const ImageSpec* getRightArrowImage() const;

private:
   /**
    * Constructor.
    */
   StackedDialogVisualSpec(const ImageSpec* dialogBackground,
                           const ScreenPoint& focusPoint,
                           const ScreenPoint& itemsUpperLeftPoint,
                           const ScreenPoint& itemsLowerRightPoint,
                           const ImageSpec* leftArrowImg,
                           const ScreenPoint& leftArrowCenterPos,
                           const ImageSpec* rightArrowImg,
                           const ScreenPoint& rightArrowCenterPos,
                           const OverlayItemVisualSpec* visualSpec,
                           const OverlayItemVisualSpec* tappedVisualSpec);

   /**
    * Destructor.
    */
   ~StackedDialogVisualSpec();

   
   const ImageSpec* m_background;
   ScreenPoint m_itemsUpperLeftPoint;
   ScreenPoint m_itemsLowerRightPoint; 
   const ImageSpec* m_leftArrowImg;
   ScreenPoint m_leftArrowCenterPos;
   const ImageSpec* m_rightArrowImg;
   ScreenPoint m_rightArrowCenterPos;
   ScreenPoint m_focusPoint;
   const OverlayItemVisualSpec* m_visualSpec;
   const OverlayItemVisualSpec* m_tappedVisualSpec;
};

} // End of namespace WFAPI

#endif // STACKEDDIALOGVISUALSPEC_H
