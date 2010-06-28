/*
  Copyright (c) 1999 - 2010, Vodafone Group Services Ltd
  All rights reserved.

  Redistribution and use in source and binary forms, with or without modification, are permitted provided that the following conditions are met:

  * Redistributions of source code must retain the above copyright notice, this list of conditions and the following disclaimer.
  * Redistributions in binary form must reproduce the above copyright notice, this list of conditions and the following disclaimer in the documentation and/or other materials provided with the distribution.
  * Neither the name of the Vodafone Group Services Ltd nor the names of its contributors may be used to endorse or promote products derived from this software without specific prior written permission.

  THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS" AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE ARE DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT HOLDER OR CONTRIBUTORS BE LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
*/

#ifndef WFAPI_OVERLAYITEMVISUALSPEC_H
#define WFAPI_OVERLAYITEMVISUALSPEC_H

#include "PALMachineTypes.h"
#include "RefCounted.h"
#include "ScreenPoint.h"
#include "WFString.h"
#include "FontSpec.h"

namespace WFAPI {

// Forward declarations
class ImageSpec;
class ScreenPoint;

/**
 * Overlay item visual spec class.
 *
 * All ScreenPoints that are used in this class are referring to
 * the offset relative to the upper left corner of the background image.
 * I.e. ScreenPoint(10,10) refers to the position 10 pixels down and 
 * 10 pixels to the right of the upper left corner of the background image.
 */
class OverlayItemVisualSpec : public RefCounted {
public:

   /**
    * Struct that is an internal data type for a color, contains
    * red, green and blue values.
    */
   struct RGB {

      /**
       * Default constructor, defaults to black.
       */
      RGB() : m_red(0), m_green(0), m_blue(0) {}

      /**
       * Constructor that takes the r,g and b values and sets its
       * member to the given values.
       *
       * @param red The value of the red.
       * @param green The value of the green.
       * @param blue The value of the blue.
       */
      RGB(wf_uint8 red, wf_uint8 green, wf_uint8 blue)
         : m_red(red), m_green(green), m_blue(blue) {}

      /// The red value, R
      wf_uint8 m_red;

      /// The green value, G
      wf_uint8 m_green;

      /// The blue value, B
      wf_uint8 m_blue;
   };

   /**
    *  Enumeration type for text placement. Concerns horizontal
    *  orientation.
    */
   enum TextHorizontalAlignment {
      /// Horizontal text alignment left
      TEXT_HALIGN_LEFT,
      /// Horizontal text alignment center
      TEXT_HALIGN_CENTER,
      /// Horizontal text alignment right
      TEXT_HALIGN_RIGHT
   };
   /**
    *  Enumeration type for text placement. Concerns vertical
    *  orientation.
    */
   enum TextVerticalAlignment {
      /// Verticval text alignment top
      TEXT_VALIGN_TOP,
      /// Verticval text alignment center
      TEXT_VALIGN_CENTER,
      /// Verticval text alignment bottom
      TEXT_VALIGN_BOTTOM
   };

   /**
    * Struct that holds a horizontal and a vertical alignment for
    * texts. See TextHorizontalAlignment and TextVerticalAlignment
    * for the different values.
    */
   struct TextAlignment {

      /**
       * Constructor that creates a TextAlignment with the given values.
       *
       * @param horizAlign The horizontal alignment, see
       *                   TextHorizontalAlignment
       * @param vertAlign The vertical alignment, see
       *                  TextVerticalAlignment.
       */
      TextAlignment(TextHorizontalAlignment horizAlign,
                    TextVerticalAlignment vertAlign) :
         m_vertAlign(vertAlign),
         m_horizAlign(horizAlign) {}

      /**
       * Default constructor, defaulting to center center.
       */
      TextAlignment() :
         m_vertAlign(TEXT_VALIGN_CENTER),
         m_horizAlign(TEXT_HALIGN_CENTER) {}

      /// The vertical alignment.
      TextVerticalAlignment m_vertAlign;

      /// The horizontal alignment.
      TextHorizontalAlignment m_horizAlign;
   };
   
   /**
    * Allocates a new visual spec. Initially the number of text rows are
    * set to 0, if you wish to support text you will need to define a
    * rectangle which will contain the text and the number of text rows.
    * @see setTextProperties.
    *
    * Initially the reference count will be set to 1. Once you are done
    * with this object, you will need to call removeReference to ensure
    * that the object is properly deallocated.
    * 
    * @param backgroundImage The background image.
    * @param imgId   The image id to use as representation. OverlayItems
    *                can have multiple images representing their states,
    *                this determines which one to use when drawing.
    * @param focusPoint The focus point of the image, i.e. this is the 
    *                   point of the image that should be focused on
    *                   a certain coordinate on the map (i.e. the tip).
    * @param imgCenterPoint The center point of where to put the overlayed
    *                       image.
    */
   
   static OverlayItemVisualSpec* allocate(const ImageSpec* backgroundImage,
                                          const ScreenPoint& focusPoint,
                                          const ScreenPoint& imgCenterPoint,
                                          wf_uint32 imgId);

   /**
    * Allocates a new visual spec that shows no icon image.
    * Initially the number of text rows are set to 0, if you wish to
    * support text you will need to define a rectangle which will contain
    * the text and the number of text rows.
    *
    * Initially the reference count will be set to 1. Once you are done
    * with this object, you will need to call removeReference to ensure
    * that the object is properly deallocated.
    * 
    * @see setTextProperties.
    *
    * @param backgroundImage The background image.
    * @param focusPoint The focus point of the image, i.e. this is the 
    *                   point of the image that should be focused on
    *                   a certain coordinate on the map (i.e. the tip).
    */
   
   static OverlayItemVisualSpec* allocate(const ImageSpec* backgroundImage,
                                          const ScreenPoint& focusPoint);

   /**
    * Set the properties for overlaying text onto the background image.
    * The area of the background image that is used for drawing text
    * is defined by the rectangle that is formed by the two diagonal
    * ScreenPoints.
    *
    * @param upperLeftPoint The upper left point of the rectangle
    *                       indicating where text will be drawn.
    * @param lowerRightPoint The lower right point of the rectangle
    *                        indicating where text will be drawn.
    * @param textAlignment The alignment for the text.
    * @param textColor The color that should be used for the text.
    * @param textFont The font that should be used for drawing the text.
    *                 NULL means default font.
    */
   void setTextProperties(const ScreenPoint& upperLeftPoint,
                          const ScreenPoint& lowerRightPoint,
                          const TextAlignment& textAlignment,
                          const RGB& textColor = RGB(0, 0, 0),
                          FontSpec* textFont = NULL);

   /**
    * Set the properties for overlaying text onto the background image.
    * The area of the background image that is used for drawing text
    * pis defined by the rectangle that is formed by the two diagonal
    * ScreenPoints.
    *
    * @param upperLeftPoint The upper left point of the rectangle
    *                       indicating where text will be drawn.
    * @param lowerRightPoint The lower right point of the rectangle
    *                        indicating where text will be drawn.
    * @param numberUpperLeftPoint The upper left point of the rectangle
    *                             indicating where the number of items
    *                             will be drawn.
    * @param numberLowerRightPoint The lower right point of the rectanble
    *                              indicating where the number of items
    *                              will be drawn.
    * @param textAlignment The alignment for the text.
    * @param numberAlignment The alignment for the number of stacked items.
    * @param textColor The color that should be used when drawing the text
    *                  e.g the name of the placemarker.
    * @param nbrColor The color that should be used when drawing number
    *                 of stacked items.
    * @param textFont The font for the text area.
    * @param nbrFont The font for the number area.
    *                 
    */
   void setTextProperties(const ScreenPoint& upperLeftPoint,
                          const ScreenPoint& lowerRightPoint,
                          const ScreenPoint& numberUpperLeftPoint,
                          const ScreenPoint& numberLowerRightPoint,
                          const TextAlignment& textAlignment,
                          const TextAlignment& numberAlignment,
                          const RGB& textColor,
                          const RGB& nbrColor,
                          FontSpec* textFont,
                          FontSpec* nbrFont);

   /**
    * Returns the focus point of image, i.e. this is the 
    * point of the image that should be focused on
    * a certain coordinate on the map (i.e. the tip).
    *
    * @return The focus point of the image.
    */ 
   const ScreenPoint& getFocusPoint() const;   

   /**
    * Returns the background image of the visual spec.
    *
    * @return The background image.
    */
   const ImageSpec* getBackgroundImage() const;

   /**
    * Returns the position where the overlay items own
    * image should be placed.
    *
    * @return The overlay item image position.
    */
   const ScreenPoint& getOverlayImageCenterPos() const;
   
   /**
    * Returns the upper left point of the rectangle that
    * will contain the text.
    *
    * @return The upper left point of the text rectangle.
    */ 
   const ScreenPoint& getTextUpperLeftPoint() const;

   /**
    * Returns the lower right point of the rectangle that
    * will contain the text.
    *
    * @return The lower right point of the text rectangle.
    */    
   const ScreenPoint& getTextLowerRightPoint() const;

   /**
    * Returns the upper left point of the rectangle that
    * will contain the number of stacked items.
    *
    * @return The upper left point of the number of items rectangle.
    */ 
   const ScreenPoint& getNumberUpperLeftPoint() const;

   /**
    * Returns the lower right point of the rectangle that
    * will contain the number of stacked items.
    *
    * @return The lower right point of the number of items rectangle.
    */ 
   const ScreenPoint& getNumberLowerRightPoint() const;
   
   /**
    * Returns the number of text rows that will be displayed
    * inside the text rectangle.
    *
    * @return The number of text rows.
    */   
   wf_uint getNbrTextRows() const;

   /**
    * @return The image id of the OverlayItem to use. If no image is specified,
    *         WF_MAX_UINT32 will be returned.
    */ 
   wf_uint32 getImageId() const;

   /**
    * Returns the alignment for the text, e.g the name of a placemarker.
    *
    * @return The alignment for the text.
    */
   const TextAlignment& getTextAlignment() const;

   /**
    * Returns the alignment for the number of stacked items.
    *
    * @return The alignment for the number of stacked items.
    */
   const TextAlignment& getNumberAlignment() const;
   
   /**
    * Returns the color that should be used for the text e.g name of the
    * place marker.
    *
    * @return The text color.
    */
   const RGB& getTextColor() const;
   
   /**
    * Returns the color that should be used for the number of stacked items
    *
    * @return The number of items color.
    */
   const RGB& getNbrColor() const;


   /**
    * @return The font that should be used for drawing the text area.
    */ 
   FontSpec* getTextFont() const;

   /**
    * @return The font that should be used for drawing the text area.
    */ 
   FontSpec* getNumberFont() const;
protected:
   /**
    * Destructor.
    */ 
   virtual ~OverlayItemVisualSpec();
private:

   /**
    *  Constructor. See @allocate
    */ 
   OverlayItemVisualSpec(const ImageSpec* backgroundImage,
                         const ScreenPoint& focusPoint,
                         const ScreenPoint& imgCenterPoint,
                         wf_uint32 imgId);
   
   /// The focus point of the visual spec
   ScreenPoint m_focusPoint;

   /// The center position of the overlay image
   ScreenPoint m_overlayImgCenterPos;
   
   /// The upper left point of the text block rectangle.
   ScreenPoint m_textUpperLeftPoint;

   /// The lower right point of the text block rectangle.
   ScreenPoint m_textLowerRightPoint;

   /// The upper left point of the number of items text block rectangle.
   ScreenPoint m_numberUpperLeftPoint;
   
   /// The lower right point of the number of items text block rectangle.
   ScreenPoint m_numberLowerRightPoint;

   /// The image specification for the background image
   const ImageSpec* m_backgroundImg;
   
   /// The image id of the overlay item to use
   wf_uint32 m_imgId;

   /// Text alignment for the text, e.g name of the placemarker.
   TextAlignment m_textAlignment;

   /// Text alignment for the number of stacked items.
   TextAlignment m_numberAlignment;
   
   /// The color that should be used for the text, e.g name of a placemarker.
   RGB m_textColor;

   /// The color that should be used for the number of items.
   RGB m_nbrColor;

   /// The font that should be used for the text.
   FontSpec* m_textFont;

   /// The font that should be used for the number of items.
   FontSpec* m_nbrFont;
};

} // End of namespace WFAPI

#endif // WFAPI_OVERLAYITEMVISUALSPEC_H
