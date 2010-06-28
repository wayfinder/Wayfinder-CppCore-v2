/*
Copyright (c) 1999 - 2010, Vodafone Group Services Ltd
All rights reserved.

Redistribution and use in source and binary forms, with or without modification, are permitted provided that the following conditions are met:

    * Redistributions of source code must retain the above copyright notice, this list of conditions and the following disclaimer.
    * Redistributions in binary form must reproduce the above copyright notice, this list of conditions and the following disclaimer in the documentation and/or other materials provided with the distribution.
    * Neither the name of the Vodafone Group Services Ltd nor the names of its contributors may be used to endorse or promote products derived from this software without specific prior written permission.

THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS" AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE ARE DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT HOLDER OR CONTRIBUTORS BE LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
*/


#include "InternalConfig.h"
#include "MapProjection/MapProjection.h"
#include "TileMapHandler.h"
#include "Camera.h"


#define CORE_LOGGING_MODULE_NAME "InternalConfig"
#define CORE_LOGGING_MODULE_LEVEL CL_ALL_FILE
#include "CoreDebugPrint.h"

namespace WFAPI {

InternalConfig::InternalConfig( TileMapHandler* mapHandler )
{
   m_mapHandler = mapHandler; 
}

double InternalConfig::getArcAngle() const
{
   MapProjection& proj = m_mapHandler->getMapProjection();
   return proj.getCamera().getArcAngle();
}

void InternalConfig::setArcAngle( double arcAngle )
{
   MapProjection& proj = m_mapHandler->getMapProjection();
   proj.getCamera().setArcAngle( arcAngle );
}

void InternalConfig::setFOV( double fov )
{
   MapProjection& proj = m_mapHandler->getMapProjection();
   proj.getCamera().setFOV( fov );
}

double InternalConfig::getFOV() const
{
   MapProjection& proj = m_mapHandler->getMapProjection();
   return proj.getCamera().getFOV();
}

void InternalConfig::setFocalPointPosition( float normPos )
{
   MapProjection& proj = m_mapHandler->getMapProjection();
   proj.getCamera().setFocalPointPosition(normPos);
}

float InternalConfig::getFocalPointPosition()
{
   MapProjection& proj = m_mapHandler->getMapProjection();
   return proj.getCamera().getFocalPointPosition();
}

void InternalConfig::setForcedUpdates(bool enabled)
{
   m_mapHandler->setForcedRedraws(enabled);
}

void InternalConfig::setHorizonDistance( float distance )
{
   m_mapHandler->getMapProjection().getCamera().setHorizonDistance(distance);
}
float InternalConfig::getHorizonDistance()
{
   return
      m_mapHandler->getMapProjection().getCamera().getHorizonDistance();
}

bool InternalConfig::get3dMode()
{
   return m_mapHandler->get3dMode();
}

}

bool WFAPI::InternalConfig::isMapFullyLoaded() const
{
   return m_mapHandler->mapFullyLoaded(); 
}

void WFAPI::InternalConfig::cycleTextPlacementPosition()
{
   coreprintln(__FUNCTION__);
}

void WFAPI::InternalConfig::cycleTextPlacementAlgorithm()
{
   m_mapHandler->cycleTextPlacementAlgorithm();
}

void WFAPI::InternalConfig::animatedZoomStepIn()
{
   
}

void WFAPI::InternalConfig::animatedZoomStepOut()
{
    
}


