/*
Copyright (c) 1999 - 2010, Vodafone Group Services Ltd
All rights reserved.

Redistribution and use in source and binary forms, with or without modification, are permitted provided that the following conditions are met:

    * Redistributions of source code must retain the above copyright notice, this list of conditions and the following disclaimer.
    * Redistributions in binary form must reproduce the above copyright notice, this list of conditions and the following disclaimer in the documentation and/or other materials provided with the distribution.
    * Neither the name of the Vodafone Group Services Ltd nor the names of its contributors may be used to endorse or promote products derived from this software without specific prior written permission.

THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS" AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE ARE DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT HOLDER OR CONTRIBUTORS BE LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
*/


// Nav2ErrorTableWM.cpp: implementation of the Nav2ErrorTableWM class.
//
//////////////////////////////////////////////////////////////////////

#include "Nav2ErrorTableWM.h"
#include <vector>
#include "nav2util.h"

struct Nav2ErrorItem 
{
   Nav2ErrorItem(int id, char* sError) : 
      m_nErrorID(id), m_sErrorString(sError)
   {}
   int   m_nErrorID;
   std::string m_sErrorString;
};   

std::vector<Nav2ErrorItem> g_ErrorItems;

void g_fillVector()
{
#define PUSH_BACK(x) g_ErrorItems.push_back(x);
#define NAV2ERROR_LINE(symbol, id, txt)  PUSH_BACK(Nav2ErrorItem(id, #txt))
#define NAV2ERROR_LINE_LAST(symbol, id, txt) NAV2ERROR_LINE(symbol, id, txt)
#include "Nav2Error.master"
#undef NAV2ERROR_LINE
#undef NAV2ERROR_LINE_LAST
#undef PUSH_BACK
}

void g_clearVector()
{
   g_ErrorItems.swap(std::vector<Nav2ErrorItem>());
}


namespace isab {
   namespace Nav2Error {

      Nav2ErrorTableWM::Nav2ErrorTableWM(fnGetString pfnGetString)
      {
         g_fillVector();
         int nSize = g_ErrorItems.size();
         m_tableSize = nSize;
         Nav2ErrorElement* pElements = new Nav2ErrorElement[nSize];
         for (int i = 0;i < nSize; ++i) {
            std::string& sTemp = g_ErrorItems.at(i).m_sErrorString;
            // due to lack of using preprocessor in NAV2ERROR_LINE macros
            // (see parameter #txt) we obtain """" instead of "".Following 
            //  code will remove extra quotes.
            if (!sTemp.compare("\"\"")){
               sTemp = "";
            }
            std::string sError = pfnGetString(sTemp);              
            char *pError = NULL; 
            if (sError.size() > 0) {
               pError = strdup_new(sError.c_str());
            } else {
               pError = strdup_new("");
            }
            pElements[i].errorNbr = ErrorNbr(g_ErrorItems.at(i).m_nErrorID);
            pElements[i].errorString = pError;
         }
         m_table = pElements;
         g_clearVector();
      }    

      Nav2ErrorTableWM::~Nav2ErrorTableWM()
      {
         for(uint32 i = 0; i < m_tableSize; ++i){
            delete[] const_cast<char*>(m_table[i].errorString);
         }
         delete[] const_cast<Nav2ErrorElement*>(m_table);
      }
    
   }
}

