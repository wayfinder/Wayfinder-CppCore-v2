/*
Copyright (c) 1999 - 2010, Vodafone Group Services Ltd
All rights reserved.

Redistribution and use in source and binary forms, with or without modification, are permitted provided that the following conditions are met:

    * Redistributions of source code must retain the above copyright notice, this list of conditions and the following disclaimer.
    * Redistributions in binary form must reproduce the above copyright notice, this list of conditions and the following disclaimer in the documentation and/or other materials provided with the distribution.
    * Neither the name of the Vodafone Group Services Ltd nor the names of its contributors may be used to endorse or promote products derived from this software without specific prior written permission.

THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS" AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE ARE DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT HOLDER OR CONTRIBUTORS BE LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
*/


#include "PangoTextInterfaceTests.h"
#include "TestFramework.h"
#include "PangoTextInterface.h"
#include <vector>
#include <algorithm>
#include <iterator>

namespace PangoTextInterfaceTests {
   
   void testInitialization( TestReporter& reporter ) {
      
      NEW_TEST_FUNCTION( reporter );

      PangoTextInterface* textInterface = new PangoTextInterface;
            
      CHECK( textInterface != NULL , reporter);

      delete textInterface;
   }


   void testStringLengthBug( TestReporter& reporter ) {
      
      //    NEW_TEST_FUNCTION( reporter );

      //    PangoTextInterface* textInterface = new PangoTextInterface;
            
      //    const char* testStr = "Engelbrektsg";


   }


   

   
/*
   void testEmptyString( TestReporter& reporter ) {

      NEW_TEST_FUNCTION( reporter );

      PangoTextInterface textInterface;

      STRING str = "";
      MC2Point center( 0, 0 );
      isab::Rectangle rect =
         textInterface.getStringAsRectangle( str, center, 0, 0, 0.0f );
      
      CHECK( rect.getHeight() == 0, reporter );
      CHECK( rect.getWidth() == 0, reporter );
      CHECK( rect.getX() == center.getX(), reporter );
      CHECK( rect.getY() == center.getY(), reporter );

      std::vector<isab::Rectangle> boxes;
      int res = textInterface.getStringAsRectangles( boxes, str, center, 0, 0, 0.0f );
      
      CHECK( res == 0, reporter );

      MC2Point center2( -5, -5 );
      isab::Rectangle rect2 =
         textInterface.getStringAsRectangle( str, center2, 0, 0, 0.0f);

      CHECK( rect2.getHeight() == 0, reporter );
      CHECK( rect2.getWidth() == 0, reporter );
      CHECK( rect2.getX() == center2.getX(), reporter );
      CHECK( rect2.getY() == center2.getY(), reporter );

   }
*/ 
   void testSingleChar( TestReporter& reporter ) {

      NEW_TEST_FUNCTION( reporter );

      PangoTextInterface textInterface;

      STRING str = "A";

      /**
       *   -- getStringAsRectangle  
       *   Check so that the position is smaller than the center coordinate
       *   and the position + size is larger than the center coordinate.
       *
       */
      MC2Point center( 0, 0 );
      isab::Rectangle rect =
         textInterface.getStringAsRectangle( str, center, 0, 1, 0.0f );

      CHECK( rect.getX() < center.getX(), reporter );
      CHECK( ( rect.getX() + static_cast<int>( rect.getWidth() ) )
             > center.getX(), reporter );
      CHECK( rect.getY() < center.getY(), reporter );
      CHECK( ( rect.getY() + static_cast<int>( rect.getHeight() ) )
             > center.getY(), reporter );

      /**
       *   -- getStringAsRectangle  
       *   The same check as before, but with another center coordinate.
       *
       */
      MC2Point center2( -5, -5 );
      isab::Rectangle rect2 =
         textInterface.getStringAsRectangle( str, center2, 0, 1, 0.0f);

      CHECK( rect2.getX() < center2.getX(), reporter );
      CHECK( ( rect2.getX() + static_cast<int>( rect2.getWidth() ) )
             > center2.getX(), reporter );
      CHECK( rect2.getY() < center2.getY(), reporter );
      CHECK( ( rect2.getY() + static_cast<int>( rect2.getHeight() ) )
             > center2.getY(), reporter );

      /**
       *   --getStringAsRectangles
       *   Check so that only one rectangle is returned, and that
       *   the position of this rectangle is smaller than the center
       *   coordinate supplied, and that the position + size is larger
       *   than the center coordinate supplied.
       *
       */
      std::vector<isab::Rectangle> boxes;
      int res = textInterface.getStringAsRectangles( boxes, str,
                                                      center, 0, 1,
                                                      0.0f );
      CHECK( res == 1, reporter );
      CHECK( boxes.size() == 1, reporter );

      isab::Rectangle rect3 = boxes.at( 0 );

      CHECK( rect3.getX() < center.getX(), reporter );
      CHECK( ( rect3.getX() + static_cast<int>( rect3.getWidth() ) )
             > center.getX(), reporter );
      CHECK( rect3.getY() < center.getY(), reporter );
      CHECK( ( rect3.getY() + static_cast<int>( rect3.getHeight() ) )
             > center.getY(), reporter );


      /**
       *   --getStringAsRectangles
       *   Same check as above, but with another center coordinate.
       *   
       */
      std::vector<isab::Rectangle> boxes2;
      int res2 = textInterface.getStringAsRectangles( boxes2, str,
                                                      center2, 0, 1,
                                                      0.0f );
      CHECK( res2 == static_cast<int>( strlen( str ) ), reporter );
      CHECK( boxes2.size() == 1, reporter );

      isab::Rectangle rect4 = boxes2.at( 0 );

      CHECK( rect4.getX() < center2.getX(), reporter );
      CHECK( ( rect4.getX() + static_cast<int>( rect4.getWidth() ) )
             > center2.getX(), reporter );
      CHECK( rect4.getY() < center2.getY(), reporter );
      CHECK( ( rect4.getY() + static_cast<int>( rect4.getHeight() ) )
             > center2.getY(), reporter );
      
   }

   void testString( TestReporter& reporter ) {

      NEW_TEST_FUNCTION( reporter );

      PangoTextInterface textInterface;

      /**
       *   --getStringAsRectangle
       *   Check so that the returned rectangle's position is smaller than
       *   the center coordinate supplied, and the position + size is larger
       *   than the center coordinate supplied.
       *
       */
      STRING str = "TESTSTRING";
      MC2Point center( 0, 0 );

      isab::Rectangle rect =
         textInterface.getStringAsRectangle( str, center, 0,
                                             strlen( str ), 0.0f );

      CHECK( rect.getX() < center.getX(), reporter );
      CHECK( ( rect.getX() + static_cast<int>( rect.getWidth() ) )
             > center.getX(), reporter );
      CHECK( rect.getY() < center.getY(), reporter );
      CHECK( ( rect.getY() + static_cast<int>( rect.getHeight() ) )
             > center.getY(), reporter );


      /**
       *   --getStringAsRectangles
       *   Check so that the center positions of the boxes returned
       *   are evenly spaced, with a tolerance of 1 pixel.
       *
       */   
      std::vector<isab::Rectangle> boxes;
      int res = textInterface.getStringAsRectangles( boxes, str, center,
                                                     0, strlen( str ), 0.0f );

      CHECK( res == static_cast<int>( strlen( str ) ), reporter );
      CHECK( res == static_cast<int>( boxes.size() ), reporter );


      std::vector<int> centerXPositions;
      std::vector<int> centerYPositions;
      for( int i = 0; i < res; i++ ) {
         centerXPositions.push_back( boxes.at( i ).getX()
                                     + boxes.at( i ).getWidth() / 2 );
         centerYPositions.push_back( boxes.at( i ).getY()
                                     + boxes.at( i ).getHeight() / 2 );
         
      }
      //    std::copy(centerXPositions.begin(),
      //              centerXPositions.end(),
      //              ostream_iterator<int>(cout, "\n"));
      
      int spacingX = ( centerXPositions.at( res - 1) - centerXPositions.at( 0 ) )
         / ( res - 1 );
      int spacingY = ( centerYPositions.at( res - 1) - centerYPositions.at( 0 ) )
         / ( res - 1 );

      for( int i = 1; i < res; i++ ) {

         CHECK( abs( centerXPositions.at( i ) -
                     centerXPositions.at( i - 1 ) - spacingX ) <= 1,
                reporter );

         CHECK( abs( centerYPositions.at( i ) -
                     centerYPositions.at( i - 1 ) - spacingY ) <= 1,
                reporter );
         
      }


      /**
       *  --getStringAsRectangles
       *  Same test as above, with angle != 0.
       *
       */  
      centerXPositions.clear();
      centerYPositions.clear();
      center = MC2Point( -5, -5 );
      boxes.clear();

      res = textInterface.getStringAsRectangles( boxes, str, center,
                                                 0, strlen( str ), 60.0f );

      CHECK( res == static_cast<int>( strlen( str ) ), reporter );
      CHECK( res == static_cast<int>( boxes.size() ), reporter );

      for( int i = 0; i < res; i++ ) {
         centerXPositions.push_back( boxes.at( i ).getX()
                                     + boxes.at( i ).getWidth() / 2 );
         centerYPositions.push_back( boxes.at( i ).getY()
                                     + boxes.at( i ).getHeight() / 2 );
         
      }
      
      spacingX = ( centerXPositions.at( res - 1) - centerXPositions.at( 0 ) )
         / ( res - 1 );
      spacingY = ( centerYPositions.at( res - 1) - centerYPositions.at( 0 ) )
         / ( res - 1 );

      for( int i = 1; i < res; i++ ) {

         CHECK( abs( centerXPositions.at( i ) -
                     centerXPositions.at( i - 1 ) - spacingX ) <= 1,
                reporter );

         CHECK( abs( centerYPositions.at( i ) -
                     centerYPositions.at( i - 1 ) - spacingY ) <= 1,
                reporter );
         
      }
      
   }

}
