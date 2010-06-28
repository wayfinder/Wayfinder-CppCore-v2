/*
Copyright (c) 1999 - 2010, Vodafone Group Services Ltd
All rights reserved.

Redistribution and use in source and binary forms, with or without modification, are permitted provided that the following conditions are met:

    * Redistributions of source code must retain the above copyright notice, this list of conditions and the following disclaimer.
    * Redistributions in binary form must reproduce the above copyright notice, this list of conditions and the following disclaimer in the documentation and/or other materials provided with the distribution.
    * Neither the name of the Vodafone Group Services Ltd nor the names of its contributors may be used to endorse or promote products derived from this software without specific prior written permission.

THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS" AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE ARE DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT HOLDER OR CONTRIBUTORS BE LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
*/


#ifndef CAMERAPARAMETERS_H
#define CAMERAPARAMETERS_H

/**
 * DEFINITIONS
 *
 * The variables are defined under the following conditions:
 *
 * 1. Y points to north and X to east and we are using a right-handed
 *    coordinate system. If you are using a left-handed coordinate system,
 *    you will need to negate the translationZ parameter.
 *    
 *  To utilize the data in this struct with a 3D renderer, follow these steps:
 *
 *  PERSPECTIVE MATRIX
 *
 *  Setup a perspective matrix with the following properties:
 *
 *  FOV ->         ProjectionParameters.FOVDegrees
 *  Aspect Ratio:  ProjectionParameters.aspectRatio
 *  Z-Near:        ProjectionParameters.zNear
 *  Z-Far:         Projectionparameters.zFar
 *
 *  MODELVIEW MATRIX
 *
 *  Set up the modelview matrix by these sequential matrix multiplications:
 *
 *  1. Translate the Z-axis by ModelViewParameters.translationZ
 *  2. Rotate the X-axis by    ModelViewParameters.rotationX
 *  3. Translate the Y-axis by ModelViewParameters.translationY
 *  4. Rotate the Z-axis by    ModelViewParameters.rotationZ
 *  5. Perform a scaling with  ModelViewParameters.scalingX
 *
 *  Note that if tile-based offsets are used to improve accuracy
 *  the initial setup is done here. As each tile is rendered the model
 *  view matrix is then pushed unto the matrix stack and the tile
 *  specific translation vector is multiplied into the matrix. After the
 *  rendering is finished, the stack is popped to restore the state for
 *  the next tile.
 *
 *  However, if absolute coordinates are used, we need to perform one
 *  more step.
 *
 *  6. Translate the X, Y and Z axis by ModelViewParameters.worldTranslation.
 *
 */

struct CameraParameters {

   struct ProjectionParameters {
      float FOVDegrees;
      float aspectRatio;
      float zNear;
      float zFar;
   } proj;

   struct ModelViewParameters {
      float translationZ;
      float rotationXDeg;
      float translationY;
      float rotationZDeg;
      float scalingX;

      fixedp worldTranslationX;
      fixedp worldTranslationY;
   } mv;
};

#endif // CAMERAPARAMETERS_H
