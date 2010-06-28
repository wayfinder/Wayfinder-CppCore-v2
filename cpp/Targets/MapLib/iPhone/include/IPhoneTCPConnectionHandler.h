/*
  Copyright (c) 1999 - 2010, Vodafone Group Services Ltd
  All rights reserved.

  Redistribution and use in source and binary forms, with or without modification, are permitted provided that the following conditions are met:

  * Redistributions of source code must retain the above copyright notice, this list of conditions and the following disclaimer.
  * Redistributions in binary form must reproduce the above copyright notice, this list of conditions and the following disclaimer in the documentation and/or other materials provided with the distribution.
  * Neither the name of the Vodafone Group Services Ltd nor the names of its contributors may be used to endorse or promote products derived from this software without specific prior written permission.

  THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS" AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE ARE DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT HOLDER OR CONTRIBUTORS BE LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
*/

#ifndef IPHONETCPCONNECTIONHANDLER_H
#define IPHONETCPCONNECTIONHANDLER_H

#include <netinet/in.h>
#import <CoreFoundation/CFRunLoop.h>
#include "HttpClientConnection.h"
#import <CFNetwork/CFHost.h>
#import <Foundation/Foundation.h>
#import <UIKit/UIKit.h>

@interface ConnectionTimer : NSObject 
{
    NSTimer *connTimer;
  	TCPClientConnection* connection;
    int timerId;
}
@property (nonatomic, retain) NSTimer *connTimer;
@property (nonatomic, assign) TCPClientConnection* connection;
@property (nonatomic, assign) int timerId;

- (void)timerExpired:(NSTimer*)timer;

- (void)startTimer:(uint32)time connection:(TCPClientConnection*)con timerID:(uint32)timId;

@end


class IPhoneTCPConnectionHandler : public TCPConnectionHandler
{
public:

	IPhoneTCPConnectionHandler();
	virtual ~IPhoneTCPConnectionHandler();
	   
   /**
    *   Tells the underlying layer to connect to the
    *   supplied host and port. When connected, connected
    *   should be called.
    *   Should call ccon->connectDone when done.
    *   Max one connect per ClientConnection is allowed simultaneously.
    */
    virtual void connect(const char* host, unsigned int port,
                        TCPClientConnection* ccon);

   /**
    *   Tells the underlying layer to disconnect the
    *   current connection.
    */
    virtual void disconnect(TCPClientConnection* ccon);

   /**
    *   Tells the underlying layer to write the supplied
    *   bytes. The underlying layer must keep the order
    *   of the write requests.
    *   Should call ccon->writeDone when done.
    *   Max one write per ClientConnection is allowed.
    *   Not allowed if not connected.
    */
    virtual void write(const byte* bytes, int length,
                      TCPClientConnection* ccon);
      
   /**
    *   Tells the underlying layer to start receiving reads
    *   with up to maxLength bytes from the 
    *   socket. Max <code>length</code> bytes.
    *   Should call the ccon->readDone when done.
    *   Max one read per ClientConnection is allowed.
    *   Not allowed if not connected.
    */
    virtual void read(int maxLength,
                     TCPClientConnection* ccon);

   /**
    *   Removes the connection from the internal list of
    *   connections in the TCPConnectionHandler.
    */
    virtual void remove(TCPClientConnection* conn);

   /**
    *   Requests a timeout for the connection conn.
    *   After the timeout, the connectionhandler should call
    *   timerExpired on the connection.
    *   @param nbrMillis The number of milliseconds to wait before
    *                    sending the timeout.
    *   @param conn      The connection that should have the timeout.
    *   @return ID of the timeout.
    */
    virtual int requestTimer(uint32 nbrMillis,
                            TCPClientConnection* conn);

   /**
    *   Cancels the timer with id timerID.
    */
    virtual void cancelTimer(int timerID);

    struct ConnectionInfo 
    {
        CFHostRef host;
        unsigned int port;
        TCPClientConnection* con;
        CFSocketRef socket; 
    };
    
private: 
    void createSocket(void);

    CFSocketRef mySocket; 
   
    static void dealWithData(CFSocketRef s, CFSocketCallBackType type, 
                             CFDataRef address, const void *data, void *info);

    CFRunLoopSourceRef source;
	
    char *hostName;

    int portNumber;
	
    TCPClientConnection* tcpConnection;
    
    ConnectionTimer *connectTimer; 
    
    static uint32 connTimerId; 
    
    ConnectionInfo conInfo;
};

#endif
