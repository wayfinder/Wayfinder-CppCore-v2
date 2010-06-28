/*
  Copyright (c) 1999 - 2010, Vodafone Group Services Ltd
  All rights reserved.

  Redistribution and use in source and binary forms, with or without modification, are permitted provided that the following conditions are met:

  * Redistributions of source code must retain the above copyright notice, this list of conditions and the following disclaimer.
  * Redistributions in binary form must reproduce the above copyright notice, this list of conditions and the following disclaimer in the documentation and/or other materials provided with the distribution.
  * Neither the name of the Vodafone Group Services Ltd nor the names of its contributors may be used to endorse or promote products derived from this software without specific prior written permission.

  THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS" AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE ARE DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT HOLDER OR CONTRIBUTORS BE LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
*/

#ifndef LANG_CONVERTER_H
#define LANG_CONVERTER_H

#include "PALMachineTypes.h"
#include "TextLanguage.h"

namespace WFAPI {

class LangConverter {

public:

   /**
    * To convert from WFAPI TextLanguage to Nav2Language.
    *
    * @param lang The language to convert.
    * @return The language in Nav2 representation.
    */
   static wf_uint32
   textLanguageToNav2Lang(TextLanguage lang)
   {
      switch(lang) {
         case ARABIC:
            return 0x15;
            //return NavServerComEnums::ARABIC;
         case CZECH:
            return 0x0c;
            //return NavServerComEnums::CZECH;
         case DANISH:
            return 0x03;
            //return NavServerComEnums::DANISH;
         case DUTCH:
            return 0x07;
            //return NavServerComEnums::DUTCH;
         case ENGLISH:
            return 0x00;
            //return NavServerComEnums::ENGLISH;
         case ENGLISH_AMERICAN:
            return 0x0d;
            //return NavServerComEnums::AMERICAN_ENGLISH;
         case FINNISH:
            return 0x04;
            //return NavServerComEnums::FINNISH;
         case FRENCH:
            return 0x09;
            //return NavServerComEnums::FRENCH;
         case GERMAN:
            return 0x02;
            //return NavServerComEnums::GERMAN;
         case GREEK:
            return 0x0f; 
            //return NavServerComEnums::GREEK;
         case HUNGARIAN:
            return 0x0e;
            //return NavServerComEnums::HUNGARIAN;
         case ITALIAN:
            return 0x06;
            //return NavServerComEnums::ITALIAN;
         case NORWEGIAN:
            return 0x05;
            //return NavServerComEnums::NORWEGIAN;
         case POLISH:
            return 0x10;
            //return NavServerComEnums::POLISH;
         case PORTUGUESE:
            return 0x0b;
            //return NavServerComEnums::PORTUGUESE;
         case RUSSIAN:
            return 0x12;
            //return NavServerComEnums::RUSSIAN;
         case SLOVAK:
            return 0x11;
            //return NavServerComEnums::SLOVAK;
         case SLOVENIAN:
            return 0x13;
            //return NavServerComEnums::SLOVENIAN;
         case SPANISH:
            return 0x08;
            //return NavServerComEnums::SPANISH;
         case SWEDISH:
            return 0x01;
            //return NavServerComEnums::SWEDISH;
         case TURKISH:
            return 0x14;
            //return NavServerComEnums::TURKISH;
        default:
           return 0;
      }
      return 0x00;
   }
};

} /// End namespace WFAPI

#endif /// End LANG_CONVERTER_H
