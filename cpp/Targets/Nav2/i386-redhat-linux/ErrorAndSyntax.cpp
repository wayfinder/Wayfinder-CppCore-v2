/*
  Copyright (c) 1999 - 2010, Vodafone Group Services Ltd
  All rights reserved.

  Redistribution and use in source and binary forms, with or without modification, are permitted provided that the following conditions are met:

  * Redistributions of source code must retain the above copyright notice, this list of conditions and the following disclaimer.
  * Redistributions in binary form must reproduce the above copyright notice, this list of conditions and the following disclaimer in the documentation and/or other materials provided with the distribution.
  * Neither the name of the Vodafone Group Services Ltd nor the names of its contributors may be used to endorse or promote products derived from this software without specific prior written permission.

  THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS" AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE ARE DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT HOLDER OR CONTRIBUTORS BE LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
*/


#include "ErrorAndSyntax.h"
#include "Nav2ErrorXX.h"
#include "AudioCtrlScript.h"

#include "NavServerComEnums.h"

bool ErrorAndSyntax(enum isab::NavServerComEnums::languageCode language,
                    class isab::Nav2Error::Nav2ErrorTable*& errorTable,
                    class isab::AudioCtrlLanguage*& audioSyntax)
{
   using namespace isab;
   using namespace NavServerComEnums;
   audioSyntax = new AudioCtrlLanguageScript();
   switch(language){
   case ENGLISH:    errorTable = new Nav2Error::Nav2ErrorTableEn;
                    break; 
   case SWEDISH:    errorTable = new Nav2Error::Nav2ErrorTableSv;
                    break; 
   case GERMAN:     errorTable = new Nav2Error::Nav2ErrorTableDe;
                    break; 
   case DANISH:     errorTable = new Nav2Error::Nav2ErrorTableDa;
                    break; 
   case FINNISH:    errorTable = new Nav2Error::Nav2ErrorTableFi;
                    break; 
   case NORWEGIAN:  errorTable = new Nav2Error::Nav2ErrorTableNo;
                    break; 
   case ITALIAN:    errorTable = new Nav2Error::Nav2ErrorTableIt;
                    break; 
   case DUTCH:      errorTable = new Nav2Error::Nav2ErrorTableNl;
                    break; 
   case SPANISH:    errorTable = new Nav2Error::Nav2ErrorTableEs;
                    break; 
   case FRENCH:     errorTable = new Nav2Error::Nav2ErrorTableFr;
                    break; 
   case WELCH:      //errorTable = new Nav2Error::Nav2ErrorTableCy;
                    break; 
   case PORTUGUESE: errorTable = new Nav2Error::Nav2ErrorTablePt;
                    break; 
   case CZECH:      errorTable = new Nav2Error::Nav2ErrorTableCs;
                    break; 
   case AMERICAN_ENGLISH:
                    errorTable = new Nav2Error::Nav2ErrorTableUs;
                    break; 
   case HUNGARIAN:  errorTable = new Nav2Error::Nav2ErrorTableHu;
                    break; 
   case GREEK:      errorTable = new Nav2Error::Nav2ErrorTableEl;
                    break; 
   case POLISH:     errorTable = new Nav2Error::Nav2ErrorTablePl;
                    break; 
   case SLOVAK:     //errorTable = new Nav2Error::Nav2ErrorTableSk;
                    //break; 
   case RUSSIAN:    //errorTable = new Nav2Error::Nav2ErrorTableRu;
                    //break;
   case SLOVENIAN:  errorTable = new Nav2Error::Nav2ErrorTableSl;
                    break;
   case TURKISH:    //errorTable = new Nav2Error::Nav2ErrorTableTr;
                    //break;
   case ARABIC:     //errorTable = new Nav2Error::Nav2ErrorTableAr;
                    //break;
      break;
   }
   return errorTable && audioSyntax;
}
