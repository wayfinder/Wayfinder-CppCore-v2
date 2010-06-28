/*
Copyright (c) 1999 - 2010, Vodafone Group Services Ltd
All rights reserved.

Redistribution and use in source and binary forms, with or without modification, are permitted provided that the following conditions are met:

    * Redistributions of source code must retain the above copyright notice, this list of conditions and the following disclaimer.
    * Redistributions in binary form must reproduce the above copyright notice, this list of conditions and the following disclaimer in the documentation and/or other materials provided with the distribution.
    * Neither the name of the Vodafone Group Services Ltd nor the names of its contributors may be used to endorse or promote products derived from this software without specific prior written permission.

THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS" AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE ARE DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT HOLDER OR CONTRIBUTORS BE LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
*/


#ifndef TILEMAPTEXTHANDLER
#define TILEMAPTEXTHANDLER

#include "config.h"

#include "PixelBox.h"
#include "OverlapDetector.h"
#include "MC2SimpleString.h"
#include "MC2Point.h"

#include "TileMapCoord.h"
#include "TileMapHandlerTypes.h"

#include "TileFeature.h" // For VectorProxy, which should be moved.
#include "TileMapContainer.h"
#include "TileMapTextSettings.h"
#include "NativeTextInterface.h"
#include "ViewPortInterface.h"
#include "TextureBlock.h"

/**
 
  THINGS LEFT TO DO:
 
In the cost function.
   test if the string is outside the screencords and set increase the cost
   for every char thats outside.
   test more possibel placments of the text. for example iterate al
   possible variations over every segment. now we only try by filling
   al but the last segment, could try by filling the first to the max.
   then to max - 1, then to max - 2, and so on.

In the place inside polygon.
   merge polygons from sorounding tilemaps so that we can put the text 
   of for example parks in the middle of the park and not in the middle
   of the tilemaps that lies in the park.

In getTextWidth.
   Change this methods implemetation to use a getwidth in maprenderer 
   and not the .width from the rectangle returned by getRectangle.
   this is ofcource slow as a snail and needs fixing.
   
*/

#include "TextPlacement/TextResult.h"

class TileMap;
class TileMapParams;
class TileFeature;
class MapProjection;
class TileMapHandler;
class TextureBlock;

/**
 *   Handles the textplacement for tilemaps.
 *   
 */
class TileMapTextHandler {   
public:
   /**
    *    Constructor.
    */
   TileMapTextHandler( NativeTextInterface* textInterface,
                       ViewPortInterface* viewPort,
                       const MapProjection& transformMatrix,
                       const MapsToDrawIt& endMapIt,
                       TileMapHandler& mapHandler );
      
   /**
    *    Destructor.
    */
   virtual ~TileMapTextHandler();

   /**
    *    Sets new text settings.
    */
   void setTextSettings( const TileMapTextSettings& settings );


   /**
    *   Changes the native text interface to use.
    */
   
   void setTextInterface( NativeTextInterface* textInterface );


   /**
    *    Changes the viewport interface to use.
    */
   
    void setViewPortInterface( ViewPortInterface* viewPortInterface );
   
    /**
    *    Move to the current feature index and possibly the map iterator
    *    so that it points to a feature that has a string.
    */
   int moveToNextString();
  
   /**
    *    A new map has arrived at the specified iterator.
    *    @param   curIt    The iterator of the arrived map.
    *    @param   beginIt  The begin iterator of the mapstodraw.
    *    @param   endIt    The end iterator of the mapstodraw.
    */
   void newMapArrived( const MapsToDrawIt& curIt,
                       const MapsToDrawIt& beginIt,
                       const MapsToDrawIt& endIt );

   /**
    *    The map and params vector have changed.
    *    If trackingModeOn then it will try to 
    *    move the texts and keep drawing them if the 
    *    scale hasn't changed too much or similar.
    */
   void mapVectorChanged( const MapsToDrawIt& beginIt,
                          const MapsToDrawIt& endIt,
                          int trackingModeOn );
      
   /**
    *    @return true If the texthandling needs to run.
    */
   int needsToRun();
      
   /**
    *    Return the index of the placed text. -1 if no text was placed.
    */
   int doTheNeedful();


   /**
    *    Clears the entire text of strings etc.
    *
    */
   
   void clear();
   
   /**
    *
    * @return int/bool that tells us if the box was ok and added or 
    * not ok and overlaping something.
    */
   inline int testAndAddOverlap( const PixelBox& inBox );

   /**
    *
    * @return int/bool that tells us if the boxes was ok and added or 
    * not ok and overlaping something.
    */
   inline int testAndAddOverlaps( const std::vector<PixelBox>& inBoxes );

   /**
    *    Puts a refference to the overlapboxlist in inboxes.
    */
   void getOverlapVector( 
      std::vector<PixelBox>& in_boxes 
      );

   /**
    *    Return the placed texts.
    *    @param tr holds the textresult so that we can delete it
    *    if we dont want to add it.
    *    @param tr holds the textresult so that we can delete it
    *    if we dont wants to add it.
    */
   void addTextResult( TextResult* tr );
   
   /**
    *    Return the placed texts.
    */
   const std::vector<TextResult*>& getTexts() const;

   /**
    *    Returns pointer to the textResults.
    */
   const std::vector<TextResult*>& getTextResults();

   /**
    *    Returns the index of the start of the longest segment 
    *    in feature.
    *    @param feature A list of points describing the feature.
    *    @param widthOfTextInPixels An int telling us how wide
    *    the string that we are trying to place is.
    */
   const int getLongestSegmentInFeature( 
      const std::vector<MC2Point>& feature );
      
   /**
    *    Contains the information needed to render the text that 
    *    have been possitioned by us.
    */
   std::vector<TextResult*> m_textResults;

   /**
    *    Contains the information needed to render the text that 
    *    have been possitioned by us.
    */
   std::vector<TextResult*> m_textResultsBeforeNewMapArrived;

   /**
    *   Returns a MapPlotter-string corresponding to the
    *   supplied utf-8 string.
    *   @param strFont utf-8 font.
    *   @return MapPlotter font.
    */
   const STRING* getPlotterStr( const MC2SimpleString& strFont );

   /**
    *   @return The text settings.
    */
   inline TileMapTextSettings& getTextSettings();

   /**
    *   Set the text color for non round rects.
    */
   void setTextColor( uint32 color );
   
private:

   enum {
      c_heightDisplacmentOftextOverCityCenter = 12,
   };


   /**
    *   Initializes all resources for a TextPlacementNotice
    *   and creates it.
    *
    */
   
   TextPlacementNotice
   createPlacementNotice(const MC2Point& point,
                         const MC2Coordinate& worldCoordinate,
                         int startIndex, 
                         int endIndex, 
                         uint16 angle,
                         const MC2Coordinate& lineBeginCoord = MC2Coordinate(),
                         const MC2Coordinate& lineEndCoord = MC2Coordinate());
   
   /**
    *   Deletes all contents of the vector and clears it.
    */
   void deleteAndClear( std::vector<TextResult*>& vect );
   
   /**
    *    Reset the textplacement. Starts the textplacement al over.
    *    @param clear_m_textResultsBeforeNewMapArrived tells us
    *    if we wants to clear the textresults list or if we wants
    *    to keep it.
    */
   void reset( int clear_m_textResultsBeforeNewMapArrived = 1 );      

   /**
    *   
    */
   void updateCoordsForTextResults( std::vector<TextResult*>& vect,
                                    int angleDiff,
                                    bool trackingMode );
   
   /**
    *    Place some text in the map.
    */
   void placeSomeText();

   /**
    *    Place some text inside a polygon.
    *    @param nameOfFeature The text that we wants to draw.
    *    @param font The font that we wants to draw the text with.
    *    @param pointsInFeature A vector containing points that
    *    describe the feature that we wants to put a name on.
    *    @param type An int telling us how to render this particular
    *    text string.
    */
   int placeInsidePolygon(
      STRING* nameOfFeature,
      const VectorProxy<MC2Point>& pointsInFeature,
      uint16 type );
           
   /**    
    *    Place some text horizontal and puts the results in
    *    m_textResults;
    *    @param nameOfFeature The text that we wants to draw.
    *    @param font The font that we wants to draw the text with.
    *    @param pointsInFeature A vector containing points that
    *    describe the feature that we wants to put a name on.
    *    @param type An int telling us how to render this particular
    *    text string.
    *    @return tells the caling method if things went well or bad.
    */
   int placeHorizontal(
      STRING* nameOfFeature, 
      const VectorProxy<MC2Point>& pointsInFeature,
      uint16 type );

   /**
    *    Placec a substring of a text on a given segment of 
    *    that texts feature.
    *    @param nameOfFeature The text that we wants to draw.
    *    @param font The font that we wants to draw the text with.
    *    @param pointsInFeature A vector containing points that
    *    describe the feature that we wants to put a name on.
    *    @param type An int telling us how to render this particular
    *    text string.
    *    @param segmentNbr Tellls us what segment we will be 
    *    working on. 
    *    @param LeftCenterRight Tells us if we want to possition the
    *    substring close to the left edge or the right or the center
    *    of the segment, ei the begining or end or center.
    *    @return tells the caling method if things went well or bad.
    */
   int placeOn_Segments(
      STRING* nameOfFeature,
      const STRING* font,
      int fontSize,
      const std::vector<MC2Point>& pointsInFeature,
      TileMapCoords::const_iterator coordsBegin,
      uint16 type,
      std::vector<int>& charsOnSegment,
      int reverseText );

   /**
    *   Places the entire text on a single segment.
    *   
    *    @param nameOfFeature The text that we wants to draw.
    *    @param font The font that we wants to draw the text with.
    *    @param pointsInFeature A vector containing points that
    *    describe the feature that we wants to put a name on.
    *    @param type An int telling us how to render this particular
    *    text string.
    *    @param segmentNbr Tellls us what segment we will be 
    *    working on. 
    *    @param lengthOfString the length of the string, in characters.
    *    @param widthOfString the width of the string, in pixels.
    *    @param textResult the text result to place the text in.
    *    @return tells the caling method if things went well or bad.
    */ 
   int placeOn_SingleSegment(
      STRING* nameOfFeature,
      const STRING* font,
      int fontSize,
      const std::vector<MC2Point>& pointsInFeature,
      TileMapCoords::const_iterator coordsBegin,
      uint16 /*type*/,
      std::vector<int>& charsOnSegment,
      int segmentNbr,
      int lengthOfString,
      int widthOfString,
      TextResult* textResult);
         
   /**
    *    Returns the width in pixels of a given string.
    *    @param nameOfFeature The text that we wants to draw.
    *    @param startIndex the index that we will start at in the
    *    string. That is if we are getting tha width for a 
    *    substring of the string.
    *    @param nbrOfChars number of character that we will be
    *    conserned with in the string, starting at startIndex.
    *    @return the widht of the String/Substring in pixels.
    */
   int getTextWidth( 
      STRING* nameOfFeature,
      int startIndex,   
      int nbrOfChars );

   /**
    *    Reverses the order of al the points in a polgon.
    *    ei the first will be last and the other way around.
    *    @param pointsInFeature the polygon.
    */
   void reversePoints(
      std::vector<MC2Point>& pointsInFeature );
   
      
   /**
    *    Tests if the text that we wants to place on the polygon
    *    should be reversed in order to be drawn correcty or not.
    *    @ param pointsInFeature the polygon.
    *    @return 0 if normal and 1 if we need to possition the text
    *    in revers order.
    */
   int IsTextReversed(
      const std::vector<MC2Point>& pointsInFeature, int firstSegment );

   /**
    *    Returns the invalue squared.
    *    @param a value to square;
    *    @return a*a;
    */
   int Pow2( int a );

   /**
    *    Given some info this function tells us how many characters 
    *    we can fitt on a line. 
    *    @param nameOfFeature the string that we wants to place on
    *    the line.
    *    @param stringLength the length of the string in chars.
    *    @param segmentWidth the width of the segment we are trying
    *    to place the string on.
    *    @param startChar tells us what char in the string that 
    *    should be the first on the line.
    *    @return the number of chars that could be fitted on the
    *    line.
    */
   int NbrOfCharsThatFitsInSeg(
      STRING* nameOfFeature,
      int stringLength,
      int segmentWidth,
      int startChar );


   /**
    *    An estimation of how good a given placment of some text on
    *    a polygon is.
    *    @param nameOfFeature the string.
    *    @param charsOnSegment number of chars on each linesegment.
    *    @param the polygon.
    *    @return the total cost fo this indata.
    */
   int Cost(
      STRING* nameOfFeature,
      const std::vector<int>& segmentWidths, 
      const std::vector<int>& charsOnSegment,
      const std::vector<MC2Point>& pointsInFeature );


   /**
    *    Putts some text on a polygon using NbrOfCharsThatFitsInSeg.
    *    @param nameOfFeature the string.
    *    @param segmentWidth the width of the segment we are trying
    *    to place the string on.
    *    @param tempCharsOnSegment number of chars on each linesegment.
    *    @param stringLength the length of the string in chars.
    *    @param startSegment the index in the polygon telling us
    *    at what segment we should start to place text.
    *    @param startChar the indes in the string that we wants to 
    *    start to place text from.
    *    @return number of chars that we could possition.
    *
    */
   int FillRestOfCharsOnSegment(
      STRING* nameOfFeature,
      const std::vector<int>& segmentWidths,
      std::vector<int>& tempCharsOnSegment,
      int stringLength,
      int startSegment,
      int startChar );

  
   /**    
    *    Place some text on_line and puts the results in
    *    m_textResults;
    *    @param nameOfFeature the string (name of somthing in the map)
    *    that we wants to possition.
    *    @font the font that we wants to write the nameOfFeature with.
    *    @param pointsInFeature the polygon that we are going to use
    *    to get som hint of where to draw the nameOfFeature text.
    *    @type tells us what king of text this is and how to draw 
    *    and possition it.
    *    @param lineWidth  The total width of the line.
    *    @return will be 1 if the text was placed correctly and 0
    *    if it wasent.
    */
   int placeOn_Line(
      STRING* nameOfFeature, 
      const VectorProxy<MC2Point>& pointsInFeature,
      TileMapCoords::const_iterator coordsBegin,
      uint16 type,
      uint32 lineWidth );

   /**    
    *    Place some text on_roundRect and puts the results in
    *    m_textResults.
    *    @param nameOfFeature the string (name of somthing in the map)
    *    that we wants to possition.
    *    @font the font that we wants to write the nameOfFeature with.
    *    @param pointsInFeature the polygon that we are going to use
    *    to get som hint of where to draw the nameOfFeature text.
    *    @type tells us what king of text this is and how to draw 
    *    and possition it.
    *    @return will be 1 if the text was placed correctly and 0
    *    if it wasent.
    */
   int placeOn_RoundRect(
      STRING* nameOfFeature, 
      const VectorProxy<MC2Point>& pointsInFeature, 
      uint16 type );


   /**
    *    Tests if a point is inside a polygon or not.
    *    @param posOfString the point we wants to test against
    *    the polygon.
    *    @param pointsInFeature the polygon, or point list, that
    *    we wants to test the point against.
    *    @return 1 if posOfString is inside the pointsInFeature
    *    polygon, 0 if not.
    *    
    */
   const int pointIsInsidePoly( 
      const MC2Point& posOfString, 
      const VectorProxy<MC2Point>& pointsInFeature ); 
 
   /**
    *    Calculates the angle used to draw text. Caluclations
    *    are based on the line made up by p1 and p2. The angle
    *    should be between -180 and 180.
    *    @param p1x The x value for the first point.
    *    @param p1y The y value for the first point.
    *    @param p2x The x value for the second point.
    *    @param p2y The y value for the second point.
    *    @return the angle.
    */
   int16 calcAngleForTextOnLine( 
      int p1x, 
      int p1y, 
      int p2x, 
      int p2y );

   /**
    *    Returns the squared dist between p1 and p2.
    *    @param p1 First point.
    *    @param p2 Second point.     
    *    @return Squared distance between first and second point. 
    */
   int Pow2DistBetweenPoints( 
      MC2Point p1, 
      MC2Point p2 );

   /**
    *    Converts the dest coordinate to a world coordinate
    *    using the transformMatrix and returns a reference to dest.
    */
   inline MC2Coordinate& toWorld(MC2Coordinate& dest, const MC2Point& src);
      
   /// The overlap detector.
   OverlapDetector<PixelBox> m_overlapDetector;

   /**
    *    A pixel box that tells us how big the screen is.
    *    Is used to determine if a text will be drawn or not.
    */
   PixelBox m_screenCoords;      
      
   /**
    *    The states.
    *    (Not used but kept for future adddons)
    */
   enum state_t {
      preparing = 0,
      placingText = 1
   };
      
   /**
    *    The current state.
    */
   state_t m_state;

   /**
    *    Determines how much the angle between two segments that
    *    we are placing text on will affect the distance between 
    *    the substrings. A higher value will give a shorter dist.
    *    Resonable values are 1-90. Good values are around 20.
    */
   int m_magicConst;
      
   /**
    *    The maximum sumed angles that the cost function will 
    *    alow for a text placed on_line.
    */
   int m_maxTotalAngle;

   /**
    *    The maximum angle that two lines might have before we say 
    *    that we cant place any text here.
    */
   int m_maxAngle;

   /**
    *    The highest cost that we will accept from the cost function
    *    before we say that this will probably not look nice.
    */
   int m_maxTotalCost;

   /**
    *    Index to where we are in the currentMap. That is on what
    *    string we should work when we are asked to do so.
    */
   int m_currentTextIndexInMap;
      
   /**
    *    Iterator to the map that we are processing. If finished, 
    *    it should be the same as m_endMapIt.
    */
   MapsToDrawIt m_currentMapIt;
      
   /**
    *    End iterator of the maps to process.
    */
   MapsToDrawIt m_endMapIt;

   /// Reference to the MapProjection used.
   const MapProjection& m_matrix;
      
   /**
    *    A set used to determine if we have draw a string already
    *    and if the current string is a copy of something that has
    *    been draw for another tilemap already.
    */
   std::set<MC2SimpleString> m_stringsThatHaveBeenDrawnAlready;

      
   /**
    *    This value determines how much a on_line text is allowed
    *    to go outside the streetsegment it is beeing draw on.
    */
   double m_maxWidthOutside;

   /**
    *   Last used angle.
    */
   double m_angle;

   /**
    *   Last used scale.
    */
   float m_oldScale;
   

   /**
    *    Pointer to native text interface. Is used for retrieving
    *
    */ 

   NativeTextInterface* m_textInterface;


   /**
    *   Pointer to the viewport, used to query screen dimensions etc.
    */
   
   ViewPortInterface* m_viewPort;
   
   /**
    *    Reference to the TileMapHandler.
    */
   TileMapHandler& m_mapHandler;

   /** 
    *    Cache of already created font strings.
    */
   std::map<MC2SimpleString, STRING*> m_fonts;

   /**
    *    Settings containing which fonts to use.
    */
   TileMapTextSettings m_textSettings;

   /**
    *    Text color for non round rects.
    */
   uint32      m_textColorNonRoundRects;

   struct {
      std::vector<MC2Point> pointsInFeature;
      std::vector<int> charsOnSegment;
      std::vector<int> segmentWidths;
      std::vector<int> sufficientlyLongSegments;
      std::vector<int> tempCharsOnSegment;
      std::vector<PixelBox> stringPixelBoxes;
      std::vector<isab::Rectangle> stringRectangles;
   } m_vecCache;
};

// Inlined methods.

inline TileMapTextSettings& 
TileMapTextHandler::getTextSettings()
{
   return m_textSettings;
}

#endif
