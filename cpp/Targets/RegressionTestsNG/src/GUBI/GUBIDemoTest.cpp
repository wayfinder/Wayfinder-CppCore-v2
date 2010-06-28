/*
Copyright (c) 1999 - 2010, Vodafone Group Services Ltd
All rights reserved.

Redistribution and use in source and binary forms, with or without modification, are permitted provided that the following conditions are met:

    * Redistributions of source code must retain the above copyright notice, this list of conditions and the following disclaimer.
    * Redistributions in binary form must reproduce the above copyright notice, this list of conditions and the following disclaimer in the documentation and/or other materials provided with the distribution.
    * Neither the name of the Vodafone Group Services Ltd nor the names of its contributors may be used to endorse or promote products derived from this software without specific prior written permission.

THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS" AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE ARE DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT HOLDER OR CONTRIBUTORS BE LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
*/

#include "GUBIDemoTest.h"
#include "Window/AppWindow.h"
#include "Window/AppWindowFactory.h"
#include "GUBIControl.h"
#include "DrawingContext.h"

#define CORE_LOGGING_MODULE_NAME "GUBIDemoTest"
#define CORE_LOGGING_MODULE_LEVEL CL_ALL_FILE
#include "CoreDebugPrint.h"

struct GUBIDemoTest::Impl {
   AppWindow* window;
   GUBIControl* control;
};

GUBIDemoTest::GUBIDemoTest(Nav2Session& session, const char* name) : 
   RegressionTest(name)
{
   m_impl = new Impl;

   m_impl->window = AppWindowFactory::createWindow();
   m_impl->control = new GUBIControl(m_impl->window, this);
   
   WFAPI::MapDrawingCallback* mapDrawCallback =
      m_impl->window->getDrawingContext()->getMapDrawingCallback();
}

void GUBIDemoTest::startTest() {
   coreprintln("start");
   
}

GUBIDemoTest::~GUBIDemoTest() {
   delete m_impl->window;
   delete m_impl->control;
   delete m_impl;
}

void GUBIDemoTest::gubiShouldBeDrawn()
{
    
}

void GUBIDemoTest::onExitRequest(int code)
{
   setTestPassed(); 
}
