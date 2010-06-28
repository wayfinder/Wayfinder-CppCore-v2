/*
Copyright (c) 1999 - 2010, Vodafone Group Services Ltd
All rights reserved.

Redistribution and use in source and binary forms, with or without modification, are permitted provided that the following conditions are met:

    * Redistributions of source code must retain the above copyright notice, this list of conditions and the following disclaimer.
    * Redistributions in binary form must reproduce the above copyright notice, this list of conditions and the following disclaimer in the documentation and/or other materials provided with the distribution.
    * Neither the name of the Vodafone Group Services Ltd nor the names of its contributors may be used to endorse or promote products derived from this software without specific prior written permission.

THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS" AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE ARE DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT HOLDER OR CONTRIBUTORS BE LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
*/


#ifndef _S60WORKAROUNDS_H_
#define _S60WORKAROUNDS_H_

#include <stdlib.h>
#include <stdio.h>
#include <f32file.h>

namespace WFAPI {
namespace S60WorkArounds {

/**
 *  The linking process in Carbide sometimes resolves certain standard
 *  library functions incorrectly (i.e. not to ESTLIB.dll) inside
 *  static libraries. By calling this function, you will create
 *  references to these functions inside the main application, and
 *  all subsequent resolves will then be correct.
 */ 
void fixExports() {
   void* dummies[] = {
      (void*)fread,
      (void*)fseek,
      (void*)ftell,
      (void*)fopen,
      (void*)fgetc,
      (void*)getc,
      (void*)malloc,
      (void*)calloc,
      (void*)realloc
   };

   // To avoid compiler warnings about unused variables
   void* tmp = dummies[0];
   dummies[0] = dummies[1];
   dummies[1] = tmp;
}

} // End of namespace S60WorkArounds

} // End of namespace WFAPI

#endif /* _S60WORKAROUNDS_H_ */
