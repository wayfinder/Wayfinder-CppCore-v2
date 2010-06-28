/*
Copyright (c) 1999 - 2010, Vodafone Group Services Ltd
All rights reserved.

Redistribution and use in source and binary forms, with or without modification, are permitted provided that the following conditions are met:

    * Redistributions of source code must retain the above copyright notice, this list of conditions and the following disclaimer.
    * Redistributions in binary form must reproduce the above copyright notice, this list of conditions and the following disclaimer in the documentation and/or other materials provided with the distribution.
    * Neither the name of the Vodafone Group Services Ltd nor the names of its contributors may be used to endorse or promote products derived from this software without specific prior written permission.

THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS" AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE ARE DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT HOLDER OR CONTRIBUTORS BE LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
*/


#ifndef _DRAWINGCONTEXTPROXY_H_
#define _DRAWINGCONTEXTPROXY_H_

#include "DrawingContext.h"

/**
 *   Since we do not want the outside users of MapLib
 *   to be concerned with how the internals of different
 *   drawing paradigms work, we provide a proxy class
 *   which can be used as a base class for both paradigms.
 *
 *   This class can then be safely used inside of MapLib
 *   to access paradigm-specific rendering classes via casting.
 *
 */

class DrawingContextProxy : public WFAPI::DrawingContext {
public:
   enum Type { NONE, EXTERNAL, PLOTTER };

   /**
    *   The specific type of this context proxy.
    */
   
   Type type;
   
protected:
   
   /**
    *   Can only be called by derived classes. Each
    *   deriving class will be forced to specify its
    *   paradigm type.
    *   
    */
   
   DrawingContextProxy( Type type );
};

#endif /* _DRAWINGCONTEXTPROXY_H_ */
