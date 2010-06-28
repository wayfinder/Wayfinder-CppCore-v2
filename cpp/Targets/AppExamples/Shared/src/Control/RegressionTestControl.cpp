/*
Copyright (c) 1999 - 2010, Vodafone Group Services Ltd
All rights reserved.

Redistribution and use in source and binary forms, with or without modification, are permitted provided that the following conditions are met:

    * Redistributions of source code must retain the above copyright notice, this list of conditions and the following disclaimer.
    * Redistributions in binary form must reproduce the above copyright notice, this list of conditions and the following disclaimer in the documentation and/or other materials provided with the distribution.
    * Neither the name of the Vodafone Group Services Ltd nor the names of its contributors may be used to endorse or promote products derived from this software without specific prior written permission.

THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS" AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE ARE DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT HOLDER OR CONTRIBUTORS BE LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
*/

#include "Control/RegressionTestControl.h"
#include "RegressionTestStarter.h"
//#include "GUBIWidgetExample.h"

struct RegressionTestControl::Impl {
   
};

RegressionTestControl::RegressionTestControl(AppWindow* window,
                                             AppEventListener* appListener,
                                             const std::string suitePath)
   : NotCopyable()
{
//   GUBIWidgetExample test;

   m_impl = new Impl;

   const char* argv[3] = { "RegressionTestControl",
                           "-f",
                           suitePath.c_str()};

   // The const_cast below is justified by that the argv argument in
   // int main(int argc, char** argv) is not const as we would like
   // it to be. As a consequence, gtk_init etc is not const either
   RegressionTests::runTests(sizeof(argv) / sizeof(const char*),
                                   const_cast<char**>(argv));
}

RegressionTestControl::~RegressionTestControl()
{
   delete m_impl; 
}

void RegressionTestControl::onWindowClosed()
{
    
}

void RegressionTestControl::onPointerUp(const PointerEvent& event)
{
    
}

void RegressionTestControl::onKeyDown(const KeyEvent& event)
{
    
}



