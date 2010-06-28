/*
Copyright (c) 1999 - 2010, Vodafone Group Services Ltd
All rights reserved.

Redistribution and use in source and binary forms, with or without modification, are permitted provided that the following conditions are met:

    * Redistributions of source code must retain the above copyright notice, this list of conditions and the following disclaimer.
    * Redistributions in binary form must reproduce the above copyright notice, this list of conditions and the following disclaimer in the documentation and/or other materials provided with the distribution.
    * Neither the name of the Vodafone Group Services Ltd nor the names of its contributors may be used to endorse or promote products derived from this software without specific prior written permission.

THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS" AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE ARE DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT HOLDER OR CONTRIBUTORS BE LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
*/


#ifndef _TUNNEL_INTERFACE_REG_TEST_H_
#define _TUNNEL_INTERFACE_REG_TEST_H_

#include "TunnelInterface.h"
#include "TunnelListener.h"
#include "RegressionTest.h"
#include "ReplyData.h"
#include "RequestID.h"
#include "WFString.h"

#include <string>

class Nav2Session;

/**
 * This class downloads a website and a picture from the server.
 */

class TunnelInterfaceRegTest : public WFAPI::TunnelListener,
                               public RegressionTest
{
public:

   // Enum for the different states.
   enum state_t { UNINITIALIZED,
                  WEB_REQ,
                  PIC_REQ,
                  ERROR,
                  DONE };
   
   /**
    * Constructor.
    *
    * @param session The Nav2Session to run within.
    * @param name
    */
   TunnelInterfaceRegTest(Nav2Session& session, const char* name);

   /**
    * Destructor.
    */
   virtual ~TunnelInterfaceRegTest();

   /**
    * Starts the test.
    */
   virtual void startTest();

   /**
    * Checks if the test is finished or not.
    *
    * @return True if done, false if not.
    */
   bool finished();

   /**
    * @See TunnelListener
    */
   virtual void dataReceived(WFAPI::RequestID requestID, WFAPI::ReplyData replyData);
   
   /**
    * From Favourite listener, called when a favourite operation has triggered
    * an error in some way.
    *
    * @param status The status containing the error message.
    */
   virtual void error(const WFAPI::AsynchronousStatus& status);

private:

   /**
    * Writes data to a specific file.
    *
    * @param replyData The data to write to file.
    * @param fileName The file name of the file to write.
    *
    * @return If the writing succeeded.
    */
   bool writeFile(WFAPI::ReplyData replyData, WFAPI::WFString fileName);
   
   /// Keeps track of the current state
   state_t m_state;

   /// The interface to handled favourites.
   WFAPI::TunnelInterface& m_tunnelInterface;
};

#endif
