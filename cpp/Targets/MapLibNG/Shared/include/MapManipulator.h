/*
Copyright (c) 1999 - 2010, Vodafone Group Services Ltd
All rights reserved.

Redistribution and use in source and binary forms, with or without modification, are permitted provided that the following conditions are met:

    * Redistributions of source code must retain the above copyright notice, this list of conditions and the following disclaimer.
    * Redistributions in binary form must reproduce the above copyright notice, this list of conditions and the following disclaimer in the documentation and/or other materials provided with the distribution.
    * Neither the name of the Vodafone Group Services Ltd nor the names of its contributors may be used to endorse or promote products derived from this software without specific prior written permission.

THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS" AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE ARE DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT HOLDER OR CONTRIBUTORS BE LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
*/


#ifndef _MAPMANIPULATOR_H_
#define _MAPMANIPULATOR_H_


class SmoothMapManipulator;
class DraggingMapManipulator;
class AnimatedMapManipulator;
class TileMapToolkit;

namespace WFAPI {
class MapOperationInterface;
class MapDrawingInterface;
}

/**
 * This class handles the state of map manipulation.
 * Before manipulating the map, one should ask this class if the
 * type of manipulation is allowed at that time.
 * The need for this class arises when time-based manipulations
 * (for example an animation) is ongoing and user input based
 * manipulations should be blocked.
 * This class is also the owner of the different map manipulators.
 * 
 * Manipulation of the map can be done in several ways,
 * using key input, by dragging it around or invoking an animated event
 * like centering on a POI etc.
 */

class MapManipulator {
public:
   /**
    * Constructor.
    * Note that setMapOperationInterface should be called
    * before the MapManipulator is completely initialized.
    *
    * @param drawingInterface A handle to the drawing interface.
    *                         This is needed to call repaint during
    *                         animations etc.
    * @param platform A toolkit used to create timers for smooth
    *                 map movement and animations.
    */
   MapManipulator(WFAPI::MapDrawingInterface* drawingInterface,
                  TileMapToolkit* platform);

   /**
    * Destructor
    */
   ~MapManipulator();


   /**
    * Sets the mapOperationInterface. It is needed by the different
    * mapManipulators in order to be able to move the map.
    * This function must be called to completely initialize the
    * MapManipulator.
    *
    * @param operationInterface A handle to the operation interface
    *                           that the mapManipulator are to use.
    */
   void setMapOperationInterface(
      WFAPI::MapOperationInterface* operationInterface);

   /**
    * Enum describing the different manipul
    */
   enum ManipulatorTag { ANIMATED, SMOOTH, STATIC, DRAGGING, EXTERNAL, FLICKING };

   /**
    * @param The type of manipulation that the query applies to.
    * @return True if the manipulation is allowed to start at this time.
    */
   bool okayToStart(ManipulatorTag tag);

   /**
    * @param The type of manipulation that is being started.
    */
   void setIsStarting(ManipulatorTag tag);

   /**
    * @return True if stopAllManipulators can be called without
    *         aborting any ongoing manipulations.
    */
   bool okayToStop(); //Called by parties not wishing to manipulate

   /**
    * Stops all manipulators.
    */
   void stopAllManipulators();

   /**
    * @return The manipulator used for animations.
    */
   AnimatedMapManipulator& getAnimatedManipulator();

   /**
    * @return The manipulator used for dragging the map.
    */
   DraggingMapManipulator& getDraggingMapManipulator();

   /**
    * @return The manipulator used for smooth manipulation of the map.
    */
   SmoothMapManipulator& getSmoothMapManipulator();
   
private:

   /**
    * Protection of the copy constructor and copy assignment operator.
    */
   MapManipulator(const MapManipulator& rhs);
   MapManipulator& operator=(MapManipulator& rhs);

   /**
    * The map manipulators.
    */
   DraggingMapManipulator* m_dragging;
   SmoothMapManipulator* m_smooth;
   AnimatedMapManipulator* m_animated;
};

#endif /* _MAPMANIPULATOR_H_ */
