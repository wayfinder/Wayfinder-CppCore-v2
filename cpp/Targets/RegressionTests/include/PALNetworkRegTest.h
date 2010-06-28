/*
Copyright (c) 1999 - 2010, Vodafone Group Services Ltd
All rights reserved.

Redistribution and use in source and binary forms, with or without modification, are permitted provided that the following conditions are met:

    * Redistributions of source code must retain the above copyright notice, this list of conditions and the following disclaimer.
    * Redistributions in binary form must reproduce the above copyright notice, this list of conditions and the following disclaimer in the documentation and/or other materials provided with the distribution.
    * Neither the name of the Vodafone Group Services Ltd nor the names of its contributors may be used to endorse or promote products derived from this software without specific prior written permission.

THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS" AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE ARE DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT HOLDER OR CONTRIBUTORS BE LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
*/


#ifndef _PALNETWORKREGTEST_H_
#define _PALNETWORKREGTEST_H_

#include "PALNetwork.h"
#include "RegressionTest.h"
#include "WFString.h"
#include <string>

class Nav2Session;


/**
 * This class do some socket magic.
 */

class PALNetworkRegTest : public RegressionTest
{
public:

   enum state_t { UNINITIALIZED,
                  ERROR,
                  DONE };
   
   /**
    * Constructor.
    *
    * @param session The Nav2Session to run within.
    * @param name
    */
   PALNetworkRegTest(Nav2Session& session, const char* name);

   /**
    * Destructor.
    */
   virtual ~PALNetworkRegTest();

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

   /** Callback function used by PALNetwork.
    * @param networkContext The network context.
    * @param connectionState The state of the network context.
    * @param extendedErrorData Data specific to the network context state.
    * @param platformstatus Platform specific error code to send up to UI.
    */
   void networkCallback(pal::NetworkContext networkContext,
                        pal::NetConnectionState connectionState,
                        void* extendedErrorData,
                        unsigned int platformStatus);

   /** Callback function used by PALNetwork.
    * @param networkContext The network context.
    * @param connectionState The state of the network context.
    * @param extendedErrorData Data specific to the network context state.
    * @param platformstatus Platform specific error code to send up to UI.
    * @param data Contains context information.
    */
   static void CnetworkCallback(pal::NetworkContext networkContext,
                                pal::NetConnectionState connectionState,
                                void* extendedErrorData,
                                unsigned int platformStatus,
                                void* data);
   
private:

   /**
    * Simple function that writes to socket. Does select before write.
    * @param buffer The string of data to write to buffer.
    * @return True if writing to the socket succeeded.
    */
   bool writeToSocket(WFAPI::WFString buffer);

   /**
    * Simple function that reads from socket. Does select before read.
    * @return True if reading from the socket succeeded.
    */
   bool readFromSocket();
   
   /// Keeps track of the current state
   state_t m_state;

   /// The network context.
   pal::NetworkContext m_networkContext;

   /// The socket context.
   pal::SocketContext m_socketContext;

   /// The state of the network context.
   pal::NetConnectionState m_connectionState;
   
};

#endif
