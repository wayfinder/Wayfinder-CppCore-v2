/*
Copyright (c) 1999 - 2010, Vodafone Group Services Ltd
All rights reserved.

Redistribution and use in source and binary forms, with or without modification, are permitted provided that the following conditions are met:

    * Redistributions of source code must retain the above copyright notice, this list of conditions and the following disclaimer.
    * Redistributions in binary form must reproduce the above copyright notice, this list of conditions and the following disclaimer in the documentation and/or other materials provided with the distribution.
    * Neither the name of the Vodafone Group Services Ltd nor the names of its contributors may be used to endorse or promote products derived from this software without specific prior written permission.

THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS" AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE ARE DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT HOLDER OR CONTRIBUTORS BE LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
*/


#ifndef _ENVIRONMENT_H_
#define _ENVIRONMENT_H_

namespace Environment {

/**
 *  @return the number of milliseconds since a defined epoch.
 */
unsigned int currentTimeMillis();

/**
 * This function will wait (in a manner similar to the function above)
 * for conditionVariable to evaluate to true. In the meantime,
 * this function will allow GUI-thread events to occur (since the regression
 * tests are single-threaded, this is the only way the condition could change).
 * If the condition has not been evaluated to true after timeOutMs the
 * method will return false.
 *
 * @ret False if the conditionVariable is not true after timeOutMs milliseconds.
 */ 
bool conditionalWait(const bool& conditionVariable, unsigned int timeOutMs);

/**
 * This is not a traditional sleep. During the "sleeping", platform specific
 * event processing occurs which allows callbacks to take place.
 */
void sleep(unsigned int timeOutMs);

}

#endif /* _ENVIRONMENT_H_ */
