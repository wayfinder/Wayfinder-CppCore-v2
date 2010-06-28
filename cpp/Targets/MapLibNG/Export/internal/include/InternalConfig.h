/*
Copyright (c) 1999 - 2010, Vodafone Group Services Ltd
All rights reserved.

Redistribution and use in source and binary forms, with or without modification, are permitted provided that the following conditions are met:

    * Redistributions of source code must retain the above copyright notice, this list of conditions and the following disclaimer.
    * Redistributions in binary form must reproduce the above copyright notice, this list of conditions and the following disclaimer in the documentation and/or other materials provided with the distribution.
    * Neither the name of the Vodafone Group Services Ltd nor the names of its contributors may be used to endorse or promote products derived from this software without specific prior written permission.

THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS" AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE ARE DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT HOLDER OR CONTRIBUTORS BE LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
*/


#ifndef _INTERNALCONFIG_H_
#define _INTERNALCONFIG_H_

class TileMapHandler;

namespace WFAPI {

class ImageSpec;

class InternalConfig {
public:
   InternalConfig( TileMapHandler* mapHandler );

   /**
    *  Returns the arc angle of the camera.
    */ 
   double getArcAngle() const;

   /**
    *  Sets the arc angle of the camera.
    */ 
   void setArcAngle( double arcAngle );

   /**
    *   Returns the field of view of the camera.
    */ 
   double getFOV() const;

   /**
    *   Sets the field of view of the camera.
    */
   void setFOV( double fov );

   /**
    *   Sets the normalized center of the camera.
    */ 
   void setFocalPointPosition( float normPos );

   /**
    *   Returns the normalized center of the camera.
    */
   float getFocalPointPosition();

   /**
    * Sets the horizon distance.
    */
   void setHorizonDistance( float distance );

   /**
    * Returns the distance to the horizon.
    */
   float getHorizonDistance();
   
   /**
    * Sets whether to use forced screen updates or not.
    *
    * @param enabled If true, screen updates will be forced.
    */
   void setForcedUpdates(bool enabled);

   /**
    * Returns whether the map currently is in 3d mode.
    * 
    * @return true if the map is in 3d mode.
    */
   bool get3dMode();

   /**
    * @return True if the map is fully loaded.
    */ 
   bool isMapFullyLoaded() const;

   /**
    * Moves to a new location which is suitable for
    * evaluating the performance of various text placement
    * algorithms. Repeated calls to this function will move
    * to new places.
    */
   void cycleTextPlacementPosition();

   /**
    * Changes the algorithm that is used for text placement.
    */
   void cycleTextPlacementAlgorithm();


   void animatedZoomStepIn();

   void animatedZoomStepOut();
   
private:
   TileMapHandler* m_mapHandler;
};

} // End of namespace WFAPI

#endif /* _INTERNALCONFIG_H_ */
