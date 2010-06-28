/*
Copyright (c) 1999 - 2010, Vodafone Group Services Ltd
All rights reserved.

Redistribution and use in source and binary forms, with or without modification, are permitted provided that the following conditions are met:

    * Redistributions of source code must retain the above copyright notice, this list of conditions and the following disclaimer.
    * Redistributions in binary form must reproduce the above copyright notice, this list of conditions and the following disclaimer in the documentation and/or other materials provided with the distribution.
    * Neither the name of the Vodafone Group Services Ltd nor the names of its contributors may be used to endorse or promote products derived from this software without specific prior written permission.

THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS" AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE ARE DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT HOLDER OR CONTRIBUTORS BE LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
*/

#include "ImageMapModuleTest.h"
#include "Window/AppWindowFactory.h"
#include "Window/AppWindow.h"
#include "Control/ImageMapControl.h"
#include "BufferRequester/BufferRequesterSettings.h"
#include "BufferRequester/DiskCacheSettings.h"
#include "BufferRequester/ResourceDirectorySettings.h"
#include "MapLibNetworkContext.h"

#ifdef __linux__
#include <GtkTileMapToolkit.h>
#include <GtkHttpContext.h>
#endif

#define CORE_LOGGING_MODULE_NAME "ImageMapModuleTest"
#define CORE_LOGGING_MODULE_LEVEL CL_ALL_FILE
#include "CoreDebugPrint.h"

ImageMapModuleTest::ImageMapModuleTest(Nav2Session& session, const char* name)
   : RegressionTest(name, 16000*1000)
{

}

void ImageMapModuleTest::startTest()
{
   m_window = AppWindowFactory::createWindow();   

   WFAPI::DBufConnection* conn = NULL;

   BufferRequesterSettings modelSettings;
   
#ifdef __linux__
   m_netContext = new GtkHttpContext;
   conn = m_netContext->getConnection();
   modelSettings.setDiskCacheSettings(DiskCacheSettings("/tmp/tilemaps",
                                                        1024*1024*10));
   modelSettings.setResourceDirectorySettings(
      ResourceDirectorySettings("./resources", false));
#endif

   m_control = new ImageMapControl(m_window,
                                   "resources",
                                   modelSettings,
                                   conn,
                                   this);

   m_window->setListener(m_control);
}

ImageMapModuleTest::~ImageMapModuleTest()
{
   delete m_window;
   delete m_control;
   delete m_netContext;
}

void ImageMapModuleTest::onExitRequest(int code)
{
   setTestPassed(); 
}
