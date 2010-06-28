/*
Copyright (c) 1999 - 2010, Vodafone Group Services Ltd
All rights reserved.

Redistribution and use in source and binary forms, with or without modification, are permitted provided that the following conditions are met:

    * Redistributions of source code must retain the above copyright notice, this list of conditions and the following disclaimer.
    * Redistributions in binary form must reproduce the above copyright notice, this list of conditions and the following disclaimer in the documentation and/or other materials provided with the distribution.
    * Neither the name of the Vodafone Group Services Ltd nor the names of its contributors may be used to endorse or promote products derived from this software without specific prior written permission.

THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS" AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE ARE DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT HOLDER OR CONTRIBUTORS BE LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
*/


#ifndef CELLIDREADER_H
#define CELLIDREADER_H

#include <windows.h>
#include "CellIDListener.h"
#include "LocationInterfaceImpl.h"

namespace WFAPI {

/**
 * This class reads Cell ID related Network information from Windows
 * Mobile devices and 
 * 
 */
class CellIDReader {
public:
   /**
    * Constructor
    * @param listener The listener that is interested in the cell
    *                 id information.
    */
   CellIDReader(CellIDListener& listener);

   /**
    * Destructor
    */
   ~CellIDReader();

   /**
    * Call this function to request cellID information.
    *
    * @return True if the request was successfully sent.
    */
   bool requestCellID();

   /**
    * This function is used by a helper callback function when handling
    * the reply from the request.
    */
   CellIDListener& getCellIDListener();
   
private:

   /**
    * Initializes the Radio Interface Layer that is used to get cell
    * ID information.
    */
   bool initializeRIL();

   HANDLE m_hRil;
   HINSTANCE m_rilDll;
   CellIDListener& m_listener;

};

}

#endif /* CELLIDREADER_H */
