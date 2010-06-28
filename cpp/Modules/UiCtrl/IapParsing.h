/*
Copyright (c) 1999 - 2010, Vodafone Group Services Ltd
All rights reserved.

Redistribution and use in source and binary forms, with or without modification, are permitted provided that the following conditions are met:

    * Redistributions of source code must retain the above copyright notice, this list of conditions and the following disclaimer.
    * Redistributions in binary form must reproduce the above copyright notice, this list of conditions and the following disclaimer in the documentation and/or other materials provided with the distribution.
    * Neither the name of the Vodafone Group Services Ltd nor the names of its contributors may be used to endorse or promote products derived from this software without specific prior written permission.

THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS" AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE ARE DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT HOLDER OR CONTRIBUTORS BE LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
*/


#ifndef _IAPPARSING_H_
#define _IAPPARSING_H_

#include "IapData.h"

namespace IapParsing {

/**
 * Converts the data from an IapData into a formated string ready to be
 * printed to the parameter file and also in a format that can be used
 * by NavServerCom module.
 *
 * Colon between host and port.
 * ---------------------------- 
 * Comma betwwen id and proxy |
 * -----------------          |
 *                 |          |
 * The format is id,proxy host:proxy port
 *
 * @param iapData The IapData object containing all the data to be parsed.
 * @return An allocated string containing the iap data in the format above.
 */
char* toString(isab::IapData* iapData);

/**
 * Creates and allocates an IapData object with the data from a string.
 * The string should be in the same format as when created by the toString
 * function above.
 * @param str The string containing the iap data to be parsed to an IapData
 *            object.
 * @return An allocated IapData object containing id, host and port.
 */
isab::IapData* fromString(const char* str);

} // End of namespace IapParsing

#endif /* _IAPPARSING_H_ */
