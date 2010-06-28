/*
  Copyright (c) 1999 - 2010, Vodafone Group Services Ltd
  All rights reserved.

  Redistribution and use in source and binary forms, with or without modification, are permitted provided that the following conditions are met:

  * Redistributions of source code must retain the above copyright notice, this list of conditions and the following disclaimer.
  * Redistributions in binary form must reproduce the above copyright notice, this list of conditions and the following disclaimer in the documentation and/or other materials provided with the distribution.
  * Neither the name of the Vodafone Group Services Ltd nor the names of its contributors may be used to endorse or promote products derived from this software without specific prior written permission.

  THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS" AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE ARE DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT HOLDER OR CONTRIBUTORS BE LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
*/

#include "LanguageHelper.h"
#include "LangConverter.h"


namespace WFAPI {

namespace LanguageHelper {

bool 
createErrorTableAndSyntax(
   enum isab::NavServerComEnums::languageCode language,
   class isab::Nav2Error::Nav2ErrorTable*& errorTable,
   class isab::AudioCtrlLanguage*& audioSyntax)
{
   using namespace isab;
   audioSyntax = new AudioCtrlLanguageScript();
   switch(language){
      case NavServerComEnums::ENGLISH:    
         errorTable = new Nav2Error::Nav2ErrorTableEn;
         break; 
      case NavServerComEnums::SWEDISH:
         errorTable = new Nav2Error::Nav2ErrorTableSv;
         break; 
      case NavServerComEnums::GERMAN:
         errorTable = new Nav2Error::Nav2ErrorTableDe;
         break; 
      case NavServerComEnums::DANISH:
         errorTable = new Nav2Error::Nav2ErrorTableDa;
         break; 
      case NavServerComEnums::FINNISH:
         errorTable = new Nav2Error::Nav2ErrorTableFi;
         break; 
      case NavServerComEnums::NORWEGIAN:
         errorTable = new Nav2Error::Nav2ErrorTableNo;
         break; 
      case NavServerComEnums::ITALIAN:
         errorTable = new Nav2Error::Nav2ErrorTableIt;
         break; 
      case NavServerComEnums::DUTCH:
         errorTable = new Nav2Error::Nav2ErrorTableNl;
         break; 
      case NavServerComEnums::SPANISH:
         errorTable = new Nav2Error::Nav2ErrorTableEs;
         break; 
      case NavServerComEnums::FRENCH:
         errorTable = new Nav2Error::Nav2ErrorTableFr;
         break; 
      case NavServerComEnums::PORTUGUESE:
         errorTable = new Nav2Error::Nav2ErrorTablePt;
         break; 
      case NavServerComEnums::CZECH:
         errorTable = new Nav2Error::Nav2ErrorTableCs;
         break; 
      case NavServerComEnums::AMERICAN_ENGLISH:
         errorTable = new Nav2Error::Nav2ErrorTableUs;
         break; 
      case NavServerComEnums::HUNGARIAN:
         errorTable = new Nav2Error::Nav2ErrorTableHu;
         break; 
      case NavServerComEnums::GREEK:
         errorTable = new Nav2Error::Nav2ErrorTableEl;
         break; 
      case NavServerComEnums::POLISH:
         errorTable = new Nav2Error::Nav2ErrorTablePl;
         break;
      case NavServerComEnums::SLOVENIAN:
         errorTable = new Nav2Error::Nav2ErrorTableSl;
         break;
      case NavServerComEnums::ARABIC:
      case NavServerComEnums::RUSSIAN:
      case NavServerComEnums::SLOVAK:
      case NavServerComEnums::TURKISH:
      case NavServerComEnums::WELCH:
      default:
         errorTable = NULL;
         break;
   }
   return errorTable && audioSyntax;
}



enum isab::NavServerComEnums::languageCode 
textLanguageToNav2(TextLanguage lang)
{
   return static_cast<isab::NavServerComEnums::languageCode>
      (LangConverter::textLanguageToNav2Lang(lang));
}

WFString
getLanguageCodeForVoice(VoiceLanguage::VoiceLanguage lang)
{
   switch (lang) {
      case VoiceLanguage::ARABIC:
         return "AR";
      case VoiceLanguage::CZECH:
         return "CS";
      case VoiceLanguage::DANISH:
         return "DA";
      case VoiceLanguage::DUTCH:
         return "NL";
      case VoiceLanguage::ENGLISH:
         return "EN";
      case VoiceLanguage::ENGLISH_AMERICAN:
         return "US";
      case VoiceLanguage::FINNISH:
         return "FI";
      case VoiceLanguage::FRENCH:
         return "FR";
      case VoiceLanguage::GERMAN:
         return "DE";
      case VoiceLanguage::GREEK:
         return "EL";
      case VoiceLanguage::HUNGARIAN:
         return "HU";
      case VoiceLanguage::ITALIAN:
         return "IT";
      case VoiceLanguage::NORWEGIAN:
         return "NO";
      case VoiceLanguage::POLISH:
         return "PL";
      case VoiceLanguage::PORTUGUESE:
         return "PT";
      case VoiceLanguage::RUSSIAN:
         return "RU";
      case VoiceLanguage::SLOVAK:
         return "SK";
      case VoiceLanguage::SLOVENIAN:
         return "SL";
      case VoiceLanguage::SPANISH:
         return "ES";
      case VoiceLanguage::SWEDISH:
         return "SV";
      case VoiceLanguage::TURKISH:
         return "TR";
   }

   return "EN";
}

WFString
getLanguageCodeForText(TextLanguage lang)
{
   switch (lang) {
   case ARABIC:
      return "AR";
   case CZECH:
      return "CS";
   case DANISH:
      return "DA";
   case DUTCH:
      return "NL";
   case ENGLISH:
      return "EN";
   case ENGLISH_AMERICAN:
      return "US";
   case FINNISH:
      return "FI";
   case FRENCH:
      return "FR";
   case GERMAN:
      return "DE";
   case GREEK:
      return "EL";
   case HUNGARIAN:
      return "HU";
   case ITALIAN:
      return "IT";
   case NORWEGIAN:
      return "NO";
   case POLISH:
      return "PL";
   case PORTUGUESE:
      return "PT";
   case RUSSIAN:
      return "RU";
   case SLOVAK:
      return "SK";
   case SLOVENIAN:
      return "SL";
   case SPANISH:
      return "ES";
   case SWEDISH:
      return "SV";
   case TURKISH:
      return "TR";
   }

   return "EN";
}



} // End namespace LanguageHelper

} // End namespace WFAPI

