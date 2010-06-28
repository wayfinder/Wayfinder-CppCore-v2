/*
  Copyright (c) 1999 - 2010, Vodafone Group Services Ltd
  All rights reserved.

  Redistribution and use in source and binary forms, with or without modification, are permitted provided that the following conditions are met:

  * Redistributions of source code must retain the above copyright notice, this list of conditions and the following disclaimer.
  * Redistributions in binary form must reproduce the above copyright notice, this list of conditions and the following disclaimer in the documentation and/or other materials provided with the distribution.
  * Neither the name of the Vodafone Group Services Ltd nor the names of its contributors may be used to endorse or promote products derived from this software without specific prior written permission.

  THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS" AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE ARE DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT HOLDER OR CONTRIBUTORS BE LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
*/

#ifndef WFAPI_WGS84COORDINATE_H
#define WFAPI_WGS84COORDINATE_H

#include "PALMachineTypes.h"

#ifdef WFAPI_IOSTREAM_AVAILABLE
#include <iostream>
#endif //WFAPI_IOSTREAM_AVAILABLE

namespace WFAPI {

/**
 * Class representing a real world WGS84 coordinate in degrees.
 */
class WGS84Coordinate {
public:

   /**
    * Default constructor. Both latitude and longitude
    * will be set to 180.0 degrees. This will be an invalid coordinate.
    */
   WGS84Coordinate();
   
   /**
    * Constructs a WGS84Coordinate in degrees. As the name implies, the
    * degrees are defined using the World Geodetic System - revision WGS84.
    *
    * @param latDeg The latitude in degrees.
    * @param lonDeg The longitude in degrees.
    */
   WGS84Coordinate(double latDeg, double lonDeg);

   /**
    * Creates a copy of the supplied coordinate.
    *
    * @param coord The coord to copy.
    */
   WGS84Coordinate(const WGS84Coordinate& coord);

   /**
    * Returns whether the coordinate is valid.
    * If the longitude and latitude is equal to 180.0,
    * the coordinate is invalid.
    * 
    * @return true if the coordinate is valid.
    */
   bool isValid() const;
   
   /**
    *   Assignment operator.
    */
   WGS84Coordinate& operator=(const WGS84Coordinate& other);

   /**
    * Compare operator
    *
    * @param rhs The position to compare with
    * @return If they are equal.
    */
   bool operator==(const WGS84Coordinate& rhs);
   
#ifdef WFAPI_IOSTREAM_AVAILABLE   
   /**
    * Print on ostream.
    *
    * @param stream The stream to print on.
    * @param coord  The coordinate to print.
    * @return The stream.
    */
   friend std::ostream& operator<< (std::ostream& stream,
                                    const WGS84Coordinate& coord);
#endif //WFAPI_IOSTREAM_AVAILABLE   
   /**
    * WGS84 latitude in degrees.
    */
   double latDeg;
      
   /**
    * WGS84 longitude in degrees.
    */
   double lonDeg;
};

} // End of namespace WFAPI.

#endif // WFAPI_WGS84COORDINATE_H
