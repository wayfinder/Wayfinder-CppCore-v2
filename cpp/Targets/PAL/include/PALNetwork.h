/*
Copyright (c) 1999 - 2010, Vodafone Group Services Ltd
All rights reserved.

Redistribution and use in source and binary forms, with or without modification, are permitted provided that the following conditions are met:

    * Redistributions of source code must retain the above copyright notice, this list of conditions and the following disclaimer.
    * Redistributions in binary form must reproduce the above copyright notice, this list of conditions and the following disclaimer in the documentation and/or other materials provided with the distribution.
    * Neither the name of the Vodafone Group Services Ltd nor the names of its contributors may be used to endorse or promote products derived from this software without specific prior written permission.

THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS" AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE ARE DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT HOLDER OR CONTRIBUTORS BE LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
*/


#ifndef PALNETWORK_H
#define PALNETWORK_H

#include "PAL.h"
#include "PALTypes.h"
#include <stdio.h>

namespace pal {

// Typedef for the network environment.
typedef void* NetworkContext;

// Typedef for the socket environment.
typedef void* SocketContext;

/**
 * This enum contains the different states the network environment can
 * have. When the state has change it should be reported to all registered
 * callbacks.
 */
enum NetConnectionState {
   /**
    * The connection is uninitialized. It is not possible to connect to the
    * server until the network has been proper initilized.
    */
   UNINITIALIZED = 0,

   /**
    * This is the state to have when the network has been initilized and it is
    * possible to create a socket or corresponding primitive to be used for a
    * connection.
    */
   INITIALIZED,
 };

/**
 * This struct contains the server parameters needed for setting up a connection.
 */ 
struct ServerParameters {
   /// Contains the server address.
   char* serverAddress;
   /// Contains the server port. Invalid server port is specified to MAX_UINT32.
   unsigned int serverPort;
};

/**
 * Typedef for the PALNetwork callback function.
 *
 * @param networkContext The environment for the network.
 * @param networkContextState The networkContextState for the network context.
 * @param extendedErrorData Data specific to the networkContextState.
 * @param platformStatus This can be used to send up a platform specific error
 *                       code to the UI.
 * @param data Contains scope information.
 */
typedef void (*NetworkCallbackFunc)(NetworkContext networkContext,
                                    NetConnectionState connectionState,
                                    void* extendedErrorData,
                                    unsigned int platformStatus,
                                    void* data);

/**
 * The functions below is controlling the network context that is needed to be
 * able to connect the application to a server.
 */ 

/**
 * To be able to set up a connection to the server a network context needs to
 * be proper initilized. This is done in three steps. First this function is
 * called to create the network context. For some platforms specific parameters
 * needs to be set to be able to initilize the network context. These will be
 * retrieved in the setNetworkContextParameters function, which represents the
 * second step of network initialization. The third step is to call the
 * initNetworkContext function which will complete the initialization of the
 * network context.
 *
 * @param networkContext The context for the network environment.
 *
 * @return PAL_OK if the network context was created successfully.
 */
pstatus createNetworkContext(NetworkContext* networkContext);

/**
 * The second parameter is a string that contains the
 * neccesary parameters. This string needs to be parsed to obtain the values.
 * The string will have the format KEY:VALUE;KEY:VALUE;...
 * An example to this is PROFILEID:999;SOMEOTHERKEY:somevalue;
 * The parameters should be stored in the socket context to make it possible to
 * connect to different server addresses.
 *
 * @param networkContext The network context to use for environment.
 * @param connectionParameters A string with all neccesary parameters.
 *
 * @return PAL_OK if the operation succeeded.
 */
pstatus setNetworkContextParameters(NetworkContext networkContext,
                                    char* connectionParameters);

/**
 * The function the completes the initialization phase of the network context.
 * When this function has been executed it should be possible to set upp a
 * proper socketContext and connect to the server.
 *
 * @param networkContext The network context to use for environment.
 * @param connectionParameters A string with all neccesary parameters.
 *
 * @return PAL_OK if the operation succeeded.
 */
pstatus initNetworkContext(NetworkContext networkContext);

/**
 * This function is called taking down the context for the connection. In this
 * function all platform specific code to take down the network context should
 * be done. After this function has been called, networkContext should be
 * deallocated corretly and set to NULL. Observe that networkCallbackFunctions
 * that has not been removed should be removed properly.
 *
 * @param networkContext The context for the network environment.
 *
 * @return PAL_OK if the network context was destroyed correctly.
 */
pstatus destroyNetworkContext(NetworkContext networkContext);

/**
 * This function adds a callback that PALNetwork uses to send network status
 * updates to CV3. Observe that when destroyNetworkContext is called all
 * callbacks that is still registered will be removed.
 *
 * @param networkContext The network context that holds the network environment.
 * @param netCallbackFunc The callback function to call when a status update should be sent.
 * @param data The context to call the callback function on.
 *
 * @return PAL_OK if the callback was added correctly.
 */
pstatus addNetworkCallbackFunc(NetworkContext networkContext,
                               NetworkCallbackFunc netCallbackFunc,
                               void* data);

/**
 * This function removes a callback that PALNetwork uses to send network
 * status updates to.
 *
 * @param networkContext The network context to use for environment.
 * @param listener The listener to remove
 * @param data The context to call listener on.
 *
 * @return PAL_OK if the callback was removed correctly.
 */
pstatus removeNetworkCallbackFunc(NetworkContext networkContext,
                                  NetworkCallbackFunc netCallbackFunc,
                                  void* data);

/**
 * Below are the basic primitives used for connect to the server. They need to
 * be accurate implemented for the network connection to work.
 */

/**
 * Create the socket to use for connections.
 * The socket should have a IPv4 domain, i.e. PF_INET on Linux and the
 * connection type should be sequenced, reliable, two-way, connection-based
 * byte streams. This function also creates the socket context used for this
 * socket.
 * 
 * @param networkContext The environment for the network connection.
 * @param socketContext Contains information about the created socket.
 *
 * @return PAL_OK if the socket was created correctly.
 * @return PAL_PROTOCOL_NOT_SUPPORTED if the desired protocol is not supported.
 * @return PAL_OUT_OF_MEMORY if there is not enough memory to create a socket.
 * @return PAL_SOCKET_ERROR if there is a general socket error.
 */
pstatus createSocket(SocketContext* socketContext);

/**
 * This function is called to set the server parameters that is needed by the
 * socket context to set up the connection to the server.
 *
 * @param socketContext The socket context to store the parameters for.
 * @param serverParameters Contains server address and port number.
 *
 * @return PAL_OK if the operation succeeded.
 */
pstatus setSocketContextParameters(SocketContext socketContext,
                                   ServerParameters serverParameters);

/**
 * This function will initiate the connection on a socket.
 * It should use the connection parameters that has been set to set up a
 * connection to the specified address. The socket context holds which 
 * socket that should be connected.
 *
 * @param socketContext The socket context to connect with.
 * 
 * @return PAL_OK if the connection attempt succeeded.
 * @return PAL_CONNECTION_REFUSED if no one is listening for the connection in
 *         the remote address.
 * @return PAL_SOCKET_ALREADY_CONNECTED The socket is already connected.
 * @return PAL_CONNECTION_ATTEMPT_TIMEDOUT The connection attempt timed out.
 * @return PAL_CONNECT_ERROR is there is a general connect error.
 */
pstatus connect(SocketContext socketContext);

/**
 * This function will shutdown the connection and the appropriate steps to do
 * this needs to be handled here. After this function has been called, a new
 * socket needs to be created to be able to set up a new connection.
 *
 * @param socketContext Contains information about the socket to disconnect.
 *
 * @return PAL_OK if the disconnection succeeded.
 * @return PAL_SOCKET_NOT_CONNECTED if the socket was not connected.
 * @return PAL_DISCONNECT_ERROR if there was a general disconnnect error.
 */
pstatus disconnect(SocketContext socketContext);

/**
 * This function will check the socket if they are ready for writing or
 * reading. The two last parameters will be used as both in and out
 * parameters. The corresponding platform specific select function should be
 * called when this function is called. For example this function can be called
 * with parameter readReady is set to true and writeReady set to NULL. This
 * means that a select shall be performed on the socketContext where socket
 * should be checked for if it is possible to read from it. If it is parameter
 * shall still be set to true and if it is not it should be set to false.
 * 
 * If timeout is set to 0 (zero) then select should return
 * immediately, if it is set to NULL, select can block indefinitely.
 * 
 * @param socketContext The socket to read from.
 * @param timeout The timeout value in seconds for this call, can be NULL.
 * @param readReady Out parameter if socket is ready to be read.
 * @param writeReady Out parameter if socket is ready to be written to.
 *
 * @return PAL_OK if the operation succeeded.
 * @return PAL_SELECT_ERROR if the operation failed.
 */
pstatus select(SocketContext socketContext,
               int* timeout, 
               bool* readReady,
               bool* writeReady);

/**
 * This function will read the data that was received from the server.
 *
 * @param socketContext The socket to r2ead from.
 * @param buffer Where the data should be written to.
 * @param sizeToWrite The number of bytes to read.
 * @param sizeWritten The number of byte that was read.
 * 
 * @return PAL_OK if the operation succeeded.
 * @return PAL_READ_ERROR if the operation failed.
 */
pstatus read(SocketContext socketContext,
             char* buffer,
             unsigned int bytesToRead,
             unsigned int* bytesRead);

/**
 * This function will write data to the socket so it will be sent to the 
 * server.
 * 
 * @param socketContext The socket to write to.
 * @param buffer Contains the data to write to the socket.
 * @param sizeToWrite The number of bytes to write to the socket.
 * @param sizeWritten The number of byte that was written.
 * 
 * @return PAL_OK if the operation succeeded.
 * @return PAL_WRITE_FAILED if the operation failed.
 */
pstatus write(SocketContext socketContext,
              const char* buffer,
              unsigned int bytesToWrite,
              unsigned int* bytesWritten);


}

#endif // end PALNETWORK_H
