/*
  Copyright (c) 1999 - 2010, Vodafone Group Services Ltd
  All rights reserved.

  Redistribution and use in source and binary forms, with or without modification, are permitted provided that the following conditions are met:

  * Redistributions of source code must retain the above copyright notice, this list of conditions and the following disclaimer.
  * Redistributions in binary form must reproduce the above copyright notice, this list of conditions and the following disclaimer in the documentation and/or other materials provided with the distribution.
  * Neither the name of the Vodafone Group Services Ltd nor the names of its contributors may be used to endorse or promote products derived from this software without specific prior written permission.

  THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS" AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE ARE DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT HOLDER OR CONTRIBUTORS BE LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
*/

#ifndef WFAPI_SCREENPOINT_H
#define WFAPI_SCREENPOINT_H

#include "PALMachineTypes.h"

#ifdef WFAPI_IOSTREAM_AVAILABLE
#include <iostream>
#endif

namespace WFAPI {
   
/**
 * Point on the screen, in pixel coordinates.
 * Origo is in the top left corner.
 * Larger x coordinate means further right on the screen.
 * Larger y coordinate means further down on the screen.
 */
class ScreenPoint {
public:
   /// Type for the coordinates.
   typedef wf_int32 coord_t;
   
   /**
    * Constructor with screen coordinates as parameters.
    *
    * @param xPos The x coordinate of the position.
    * @param yPos The y coordinate of the position.
    */
   ScreenPoint(coord_t xPos, coord_t yPos) : x(xPos), y(yPos) {}

   /**
    * Empty constructor. Initialized with origo as position.
    */
   ScreenPoint() : x(0), y(0) {}
   
   /**
    * Returns the x coordinate of the point.
    *
    * @return The x coordinate.
    */
   inline const coord_t getX() const { return x; }
   
   /**
    * Returns the y coordinate of the point.
    *
    * @return The y coordinate.
    */
   inline const coord_t getY() const { return y; }

   /**
    * Returns a reference to the x coordinate for writing.
    *
    * @return A reference to the x coordinate.
    */
   inline coord_t& getX() { return x; }

   /**
    * Returns a reference to the y value for writing.
    *
    * @return A reference to the y coordinate.
    */
   inline coord_t& getY() { return y; }
   
   /**
    *   Returns true if the two points are equal
    */
   inline bool operator==(const ScreenPoint& rhs) const {
      return getX() == rhs.getX() && getY() == rhs.getY();
   }

   /**
    *   Returns true if the two points are equal
    */
   inline bool operator!=(const ScreenPoint& rhs) const {
      return getX() != rhs.getX() || getY() != rhs.getY();
   }
#ifdef WFAPI_OSTREAM_AVAILABLE
   inline friend std::ostream& operator<< (std::ostream& strm,
                                           const ScreenPoint& other)
   {
      strm << "( " << other.getX() << ", " << other.getY() << " )";
      return strm;
   /**
    *   Returns true if the two points are not equal
    */
   inline bool operator!=(const ScreenPoint& rhs) const {
      return !(*this==rhs);
   }
#endif
   

private:
   /// X value.
   coord_t x;
   /// Y value.
   coord_t y;

};

} // End of namespace WFAPI

#endif // WFAPI_SCREENPOINT_H
