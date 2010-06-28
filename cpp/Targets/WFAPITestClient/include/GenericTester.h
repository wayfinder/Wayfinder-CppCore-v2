/*
  Copyright (c) 1999 - 2010, Vodafone Group Services Ltd
  All rights reserved.

  Redistribution and use in source and binary forms, with or without modification, are permitted provided that the following conditions are met:

  * Redistributions of source code must retain the above copyright notice, this list of conditions and the following disclaimer.
  * Redistributions in binary form must reproduce the above copyright notice, this list of conditions and the following disclaimer in the documentation and/or other materials provided with the distribution.
  * Neither the name of the Vodafone Group Services Ltd nor the names of its contributors may be used to endorse or promote products derived from this software without specific prior written permission.

  THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS" AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE ARE DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT HOLDER OR CONTRIBUTORS BE LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
*/

#ifndef WFAPI_GENERIC_TESTER_H
#define WFAPI_GENERIC_TESTER_H

#include "WFAPIConfig.h"
#include "CommandOption.h"

namespace WFAPI {
   class SynchronousStatus;
}

/**
 * Abstract interface for tester classes.
 */
class GenericTester {
public:
   /**
    * Constructor.
    */
   GenericTester();

   /**
    * Destructor.
    */
   virtual ~GenericTester();

   /**
    * Handle the input and return true if it was consumed.
    *
    * @param input The input to handle.
    * @param goBack Out parameter. Will be set to true if the
    *               input indicated that the tester should go
    *               out of scope.
    * @return True if the input was handled, false otherwise.
    */
   virtual bool handleInput(const char* input, bool& goBack) = 0;

   /**
    * Print help text.
    */
   virtual void printHelp();
   
   /**
    * Print error status. Will dump the status object.
    * 
    * @param status The status object to dump.
    * @param context [Optional] The context from which the error occured.
    *
    */
   void printErrorStatus(const WFAPI::SynchronousStatus* status, 
                         const char* context = NULL);
   
protected:

   /// Map containing all the menu options.
   StringMap m_commandOptions;
    
};


#endif // End WFAPI_GENERIC_TESTER_H
