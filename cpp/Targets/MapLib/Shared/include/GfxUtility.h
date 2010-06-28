/*
Copyright (c) 1999 - 2010, Vodafone Group Services Ltd
All rights reserved.

Redistribution and use in source and binary forms, with or without modification, are permitted provided that the following conditions are met:

    * Redistributions of source code must retain the above copyright notice, this list of conditions and the following disclaimer.
    * Redistributions in binary form must reproduce the above copyright notice, this list of conditions and the following disclaimer in the documentation and/or other materials provided with the distribution.
    * Neither the name of the Vodafone Group Services Ltd nor the names of its contributors may be used to endorse or promote products derived from this software without specific prior written permission.

THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS" AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE ARE DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT HOLDER OR CONTRIBUTORS BE LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
*/


#ifndef GFXUTILITY_H
#define GFXUTILITY_H

#include "config.h"

class MC2Coordinate;
class MC2Point;

/**
 *    This static class contains a lot of utility-methods about 
 *    geographical data.
 */
class GfxUtility 
{
public:
   /**
    *    Get the distance between two points.
    *    @return  The squared linear distance between two points.
    *             In meters squared.
    */
   static float64 squareP2Pdistance_linear(int32 lat1, int32 lon1, 
                                           int32 lat2, int32 lon2,
                                           float64 cosLat = 2.0);
   
   /**
    *    Get the distance between two points.
    *    @return  The squared linear distance between two points.
    *             In meters squared.
    */
   static float64 squareP2Pdistance_linear( const MC2Coordinate& p1,
                                            const MC2Coordinate& p2,
                                            float64 cosLat = 2.0 );

   /**
    *    Get the distance between two points.
    *    @return  The linear distance between two points.
    *             In meters (sqrt of squareP2Pdistance).
    */
   static float64 rtP2Pdistance_linear( int32 lat1, int32 lon1, 
                                        int32 lat2, int32 lon2,
                                        float64 cos_lat = 2.0 );

   /**
    *    Get the distance between two points.
    *    @return  The linear distance between two points.
    *             In meters (sqrt of squareP2Pdistance).
    */
   static float64 rtP2Pdistance_linear( const MC2Coordinate& p1,
                                        const MC2Coordinate& p2,
                                        float64 cos_lat = 2.0 );


   /**
    *    Get the angle from the north-direction. Used to calculate 
    *    the angle described by the figure:
    *    @verbatim
    (lat2, lon2)
    |     /
    |    /
    |   /
    |A /
    |\/
    |/ (lat1, lon1)
    |
    |
    |
    @endverbatim
    *
    *    @param   lat1  The latitude of the startpoint for the line.   
    *    @param   lon1  The longitude of the startpoint for the line.   
    *    @param   lat2  The latitude of the endpoint for the line.   
    *    @param   lon2  The longitude of the endpoint for the line.   
    *    @return  Angle in radians from the north-direction in 
    *             clockwise direction. The return value is in the 
    *             interval $[0,2\pi[$.
    */
   static float64 getAngleFromNorthRadians( int32 lat1, int32 lon1, 
                                            int32 lat2, int32 lon2,
                                            float64 cosLat );
   
   /**
    * Same as above but the result is in degrees rather than in
    * radians.
    *    @param   lat1  The latitude of the startpoint for the line.   
    *    @param   lon1  The longitude of the startpoint for the line.   
    *    @param   lat2  The latitude of the endpoint for the line.   
    *    @param   lon2  The longitude of the endpoint for the line.   
    *    @return  Angle in radians from the north-direction in 
    *             clockwise direction. The return value is in the 
    *             interval $[0,2\pi[$.
    */
   static float64 getAngleFromNorthDegrees( int32 lat1, int32 lon1, 
                                            int32 lat2, int32 lon2,
                                            float64 cosLat );

   /**
    * The intersection result.
    */
   enum IntersectionResult { LINE_PARALLEL, LINE_NON_INTERSECTING, LINE_INTERSECTING };

   /**
    *    Calculates the intersection between the lines formed by
    *    p1p2 and p3p4, if such an intersection exists.
    *
    *
    *    @param p1               First point of first line
    *    @param p2               Second point of first line
    *    @param p3               First point of second line
    *    @param p4               Second point of second line
    *    @param intersection     Intersection point
    *
    *    @return                 If the lines are parallel, LINE_PARALLEL will
    *                            be returned. IF they are not intersecting,
    *                            LINE_NON_INTERSECTING will be returned. In
    *                            both cases, intersection will not be set. If
    *                            on the other hand LINE_INTERSECTING is
    *                            returned, an intersection was found and the
    *                            intersection point will be set accordingly.
    *
    */
   static IntersectionResult getIntersection(const MC2Point& p1, const MC2Point& p2,
                                             const MC2Point& p3, const MC2Point& p4,
                                             MC2Point& intersection);


                                            

};

#endif
