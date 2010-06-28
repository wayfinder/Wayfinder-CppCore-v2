/*
  Copyright (c) 1999 - 2010, Vodafone Group Services Ltd
  All rights reserved.

  Redistribution and use in source and binary forms, with or without modification, are permitted provided that the following conditions are met:

  * Redistributions of source code must retain the above copyright notice, this list of conditions and the following disclaimer.
  * Redistributions in binary form must reproduce the above copyright notice, this list of conditions and the following disclaimer in the documentation and/or other materials provided with the distribution.
  * Neither the name of the Vodafone Group Services Ltd nor the names of its contributors may be used to endorse or promote products derived from this software without specific prior written permission.

  THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS" AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE ARE DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT HOLDER OR CONTRIBUTORS BE LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
*/
 
#include "config.h"

#import "IPhoneMapLib.h"
#import "IPhoneTileMapToolkit.h"
#import "IPhoneFileHandler.h"

#import "TileMapTypes.h"
#import "TileMapHandler.h"

using namespace isab;

isab::IPhoneMapRenderer* IPhoneMapLib::m_mapPlotter = NULL;

IPhoneMapRenderer* IPhoneMapLib::createPlotter(int width, int height)
{   
	isab::IPhoneMapRenderer* mapPlotter = new IPhoneMapRenderer(width, height);
	m_mapPlotter = mapPlotter;
	return m_mapPlotter;
}



TileMapToolkit* IPhoneMapLib::createToolkit( id parent )
{
   m_toolkit = new IPhoneTileMapToolkit(parent); 
   return m_toolkit;
}


IPhoneMapLib::IPhoneMapLib(WFAPI::DrawingContext* drawingContext,
                           WFAPI::DBufConnection* bufferConnection,
                           const WFAPI::MapLibInitialConfig& initialConfig) :
   MapLib(drawingContext, createToolkit(0), bufferConnection, initialConfig)
{
   
}

void IPhoneMapLib::setRouteVisibility(bool visible)
{
    /* Not the nicest way to get the TileMapHandler pointer but it and the
       MapDrawingInterface are the same thing so this works. MapLib does not
       have a separate method for getting TileMapHandler so it's also the
       only way... */
    TileMapHandler* mapHandler = (TileMapHandler*)getMapDrawingInterface();
    bool isVisible = mapHandler->isLayerVisible(TileMapTypes::c_routeLayer);
    
    if (isVisible != visible) {
        mapHandler->toggleLayerToDisplay(TileMapTypes::c_routeLayer);
    }
}

FileHandler* IPhoneMapLib::createFileHandler( const char* filename,
                                  bool readOnly,
                                  bool createFile,
                                  bool initNow )
{
	return new IPhoneFileHandler(filename, createFile, m_toolkit);
}


const char* IPhoneMapLib::getPathSeparator() const
{
   return "/";
}

