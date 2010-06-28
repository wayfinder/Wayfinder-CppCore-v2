/*
Copyright (c) 1999 - 2010, Vodafone Group Services Ltd
All rights reserved.

Redistribution and use in source and binary forms, with or without modification, are permitted provided that the following conditions are met:

    * Redistributions of source code must retain the above copyright notice, this list of conditions and the following disclaimer.
    * Redistributions in binary form must reproduce the above copyright notice, this list of conditions and the following disclaimer in the documentation and/or other materials provided with the distribution.
    * Neither the name of the Vodafone Group Services Ltd nor the names of its contributors may be used to endorse or promote products derived from this software without specific prior written permission.

THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS" AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE ARE DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT HOLDER OR CONTRIBUTORS BE LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
*/


#ifndef SERVICE_WINDOW_TESTER_H
#define SERVICE_WINDOW_TESTER_H

#include "TunnelListener.h"
#include "TunnelInterface.h"
#include "Nav2API.h"
#include "RequestID.h"
#include "WFStringArray.h"
#include "GenericTester.h"

using namespace WFAPI;

/**
 * Test class that implements service window testing.
 */
class ServiceWindowTester : public TunnelListener, 
                            public GenericTester
{

public:
   
   /**
    * Constructor, adds itself as listener to TunnelInterface
    *
    * @param tunnelInterface The tunnel interface which to add
    *                           ourself as listener.
    */
   ServiceWindowTester(TunnelInterface& tunnelInterface);
                   
   /**
    * Destructor.
    */
   ~ServiceWindowTester();

   /**
    * Requests data through the Tunnel Interface.
    */
   void requestData();
   
public: // From base class

   /**
    * Called when an error has occured.
    */
   virtual void error(const AsynchronousStatus& status);

   /**
    * @See TunnelListener
    */
   virtual void dataReceived(RequestID requestID, ReplyData replyData);

   /**
    * Handle the input and return true if it was consumed.
    *
    * @param input The input to handle.
    * @param goBack Out parameter. Will be set to true if the
    *               input indicated that the tester should go
    *               out of scope.
    * @return True if the input was handled, false otherwise.
    */
   bool handleInput(const char* input, bool& goBack);

private:

   /**
    * Writes data to file.
    *
    * @param replyData Contains the data to write to file.
    *
    * @return If the writing succeeded.
    */
   bool writeToFile(const ReplyData& replyData);
   
   /**
    * Adds the command options valid for this class. The storage is a map
    * where the key is a String and the value is a CommandHolder.
    * The CommandHolder holds the actual command to perform so by finding
    * in the map after user input we can map this against a real command.
    */
   void addCommandOptions();

   /// Holds the tunnel interface
   TunnelInterface& m_tunnelInterface;

};

#endif
