/*
  Copyright (c) 1999 - 2010, Vodafone Group Services Ltd
  All rights reserved.

  Redistribution and use in source and binary forms, with or without modification, are permitted provided that the following conditions are met:

  * Redistributions of source code must retain the above copyright notice, this list of conditions and the following disclaimer.
  * Redistributions in binary form must reproduce the above copyright notice, this list of conditions and the following disclaimer in the documentation and/or other materials provided with the distribution.
  * Neither the name of the Vodafone Group Services Ltd nor the names of its contributors may be used to endorse or promote products derived from this software without specific prior written permission.

  THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS" AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE ARE DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT HOLDER OR CONTRIBUTORS BE LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
*/

#include "GuiFileOperation.h"

#ifndef MODULE_GuiCommandHandler_H
#define MODULE_GuiCommandHandler_H

namespace isab {

   class GuiCommandHandler
      {
      public:
         GuiCommandHandler(class NavTask* nt);
         ~GuiCommandHandler();
         
         void HandleOperationSelect(class GuiFileOperationSelect* op);
         void HandleOperationSelectReply(class GuiFileOperationSelectReply* op,
                                         uint32 src = 0);
         void HandleOperationConfirm(class GuiFileOperationConfirm* op);
         void HandleOperationConfirmReply(class GuiFileOperationConfirmReply* op);
         void HandleOperationCommand(class GuiFileOperationCommand* op,
                                     uint32 src = 0);
         
      private:
         class NavTask* m_navTask;
         //getDriveList is implemented in architecture specific files.
         void getDriveList(LocationVector* locations);
         void findSavedRoutes(LocationVector* locations, LocationVector* driveList);
         char* createFullPathName_new(const char* sDir, const char* sName);
         char* addFileExtension_new(const char* sFileName);
      };
   
} /* namespace isab */

#endif /* MODULE_GuiCommandHandler_H */
