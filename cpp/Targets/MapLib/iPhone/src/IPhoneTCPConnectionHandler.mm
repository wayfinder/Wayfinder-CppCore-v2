/*
  Copyright (c) 1999 - 2010, Vodafone Group Services Ltd
  All rights reserved.

  Redistribution and use in source and binary forms, with or without modification, are permitted provided that the following conditions are met:

  * Redistributions of source code must retain the above copyright notice, this list of conditions and the following disclaimer.
  * Redistributions in binary form must reproduce the above copyright notice, this list of conditions and the following disclaimer in the documentation and/or other materials provided with the distribution.
  * Neither the name of the Vodafone Group Services Ltd nor the names of its contributors may be used to endorse or promote products derived from this software without specific prior written permission.

  THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS" AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE ARE DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT HOLDER OR CONTRIBUTORS BE LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
*/

#import <CoreFoundation/CFSocket.h>
#import <CFNetwork/CFNetwork.h>
#include <sys/types.h>
#include <sys/time.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <netdb.h>
#include <arpa/inet.h>

#import "IPhoneTCPConnectionHandler.h"

static BOOL showDebug = NO;

uint32 IPhoneTCPConnectionHandler::connTimerId = 0;


@implementation ConnectionTimer
@synthesize connTimer, connection, timerId;

- (void)timerExpired:(NSTimer*)timer
{
    if (timer == connTimer) {
        connection->timerExpired(timerId);
    }
    [connTimer invalidate];
}

- (void)startTimer:(uint32)time connection:(TCPClientConnection*)con timerID:(uint32)timId;
{
    timerId = timId;
        
    if ([connTimer isValid]) {
        [connTimer invalidate];
    }

    [connTimer release];

    connTimer = [[NSTimer scheduledTimerWithTimeInterval:time*0.001
                target:self
                selector:@selector(timerExpired:)
                userInfo:nil
                repeats:NO] retain];
    connection = con;
}

@end


IPhoneTCPConnectionHandler::IPhoneTCPConnectionHandler()
	: mySocket(NULL), 
	  source(NULL), 
	  hostName(NULL),
	  portNumber(0),
	  tcpConnection(NULL),
      connectTimer(NULL)
{
    connectTimer = [[ConnectionTimer alloc] retain]; 
}

	
IPhoneTCPConnectionHandler::~IPhoneTCPConnectionHandler()
{
    if (hostName) {
        delete hostName;
    }

    [connectTimer release];
}


void IPhoneTCPConnectionHandler::createSocket()
{
    if (mySocket) {
        CFSocketInvalidate(mySocket);
        CFRelease(mySocket);
    }

    CFSocketContext connContext = { 0, this, NULL, NULL, NULL };
	
	mySocket = CFSocketCreate( kCFAllocatorDefault, 
								PF_INET, 
								SOCK_STREAM, 
								IPPROTO_TCP, 
								kCFSocketDataCallBack, 
								(CFSocketCallBack)&dealWithData, &connContext);

	source = CFSocketCreateRunLoopSource(kCFAllocatorDefault, mySocket, 0);
	CFRunLoopAddSource(CFRunLoopGetCurrent(), source, kCFRunLoopDefaultMode);
	CFRelease(source);
}


static void hostClientCallBack(CFHostRef host, CFHostInfoType typeInfo, const CFStreamError *error, void *info)
{
    if (showDebug) NSLog(@"hostClientCallBack called ");
        
    IPhoneTCPConnectionHandler::ConnectionInfo *inf = 
        (IPhoneTCPConnectionHandler::ConnectionInfo*)info;    
   
    CFHostUnscheduleFromRunLoop(inf->host, 
                                CFRunLoopGetCurrent(), 
                                kCFRunLoopDefaultMode);
    
    if (error->error == noErr) {
        if (typeInfo == kCFHostAddresses) {
            //get the host address
            CFStreamError streamError;
            Boolean result = CFHostStartInfoResolution(inf->host, 
                                                       kCFHostAddresses, 
                                                       &streamError);
            if (result) {
                struct sockaddr_in sin;
                sin.sin_port = htons(inf->port);
                
                //CFHostGetAddressing retrieves the known addresses from the given host.
                CFArrayRef addresses = CFHostGetAddressing(inf->host, NULL);
                if (addresses) {
                    struct sockaddr_in *addr = 
                        (struct sockaddr_in *)CFDataGetBytePtr((CFDataRef)CFArrayGetValueAtIndex(addresses, 0));
                        
                    if(addr->sin_family == AF_INET) {
                        sin.sin_family = addr->sin_family;
                        memmove(&sin.sin_addr, &addr->sin_addr, addr->sin_len);
                                
                        if (showDebug) NSLog(@"Resolved %d.%d.%d.%d", 
                            (addr->sin_addr.s_addr >> 0) & 0xFF, 
                            (addr->sin_addr.s_addr >> 8) & 0xFF, 
                            (addr->sin_addr.s_addr >> 16) & 0xFF, 
                            (addr->sin_addr.s_addr >> 24) & 0xFF);
                    }
                }
                else {
                    if (showDebug) NSLog(@"No host address found.");
                }

                CFDataRef address = CFDataCreate(NULL, (const UInt8*)(char*)&sin, sizeof(sin));
                CFSocketError res = CFSocketConnectToAddress(inf->socket, address, 5);
                CFRelease(address);

                if (res == kCFSocketSuccess) {
                    if (showDebug) NSLog(@"connect: Success");
                    inf->con->connectDone(TCPClientConnection::OK); 
                }
                else if (res == kCFSocketTimeout) {
                    if (showDebug) NSLog(@"connect: Timeout");
                    inf->con->connectDone(TCPClientConnection::TIMEOUT); 		
                }
                else { //if (res == kCFSocketError) 
                    if (showDebug) NSLog(@"connect: Error");		
                    inf->con->connectDone(TCPClientConnection::ERROR); 	
                }
            }
            else {
                if (showDebug) NSLog(@"Host lookup not started.");
            }
        }
        else {
            if (showDebug) NSLog(@"Info: Unhandled CFHostInfoType (%d)", typeInfo);
        }
    } 
    else { //on error
        if (showDebug) NSLog(@"Info: hostClientCallBack returned error domain = %d, id = %ld", 
            error->domain, error->error);
        
        if (error->domain == kCFStreamErrorDomainNetDB && error->error == EAI_NONAME)
            NSLog(@"Hostname nor servname provided, or not known");
        
        if (showDebug) NSLog(@"connection Error");		
        
        inf->con->connectDone(TCPClientConnection::ERROR); 	
    }
}


/**
*   Tells the underlying layer to connect to the
*   supplied host and port. When connected, connected
*   should be called.
*   Should call ccon->connectDone when done.
*   Max one connect per ClientConnection is allowed simultaneously.
*/
void IPhoneTCPConnectionHandler::connect(const char* host, unsigned int port,
                        TCPClientConnection* ccon)
{
    if (showDebug) NSLog(@"********** connect called **********");

    createSocket();

    if (!hostName) {
        hostName = new char[strlen(host)];
        if (!hostName) {
            NSLog(@"Error: hostname allocation failed.");
            return;
        }
        strcpy(hostName, &host[strlen("http://")]);
    }

    CFStringRef hostname = CFStringCreateWithCString(NULL, hostName, kCFStringEncodingASCII);
    if (!hostname) {
        if (showDebug) NSLog(@"Error: host name");
    }
    
    if (showDebug) NSLog(@"hostname: %@", hostname);

    //create a host
    CFHostRef theHost = CFHostCreateWithName(kCFAllocatorDefault, hostname);
    if (!theHost) {
        if (showDebug) NSLog(@"Error: create host");
    }
    CFRelease(hostname);

    conInfo.host = theHost;
    conInfo.port = port;
    conInfo.con = ccon;
    conInfo.socket = mySocket;

    //CFHostSetClient associates a client context and callback function with a CFHostRef.
    //This is required for asynchronous usage.  If not set, resolve will take place synchronously.
    CFHostClientContext context = {0, (void*)&conInfo, NULL, NULL, NULL};
    BOOL success = CFHostSetClient(theHost, hostClientCallBack, &context);
    if (success) {
        //CFHostScheduleWithRunLoop schedules the given host on a run loop and mode so
        //the client will receive its callbacks on that loop and mode.
        CFHostScheduleWithRunLoop(theHost, CFRunLoopGetCurrent(), kCFRunLoopDefaultMode);

        //CFHostStartInfoResolution performs a lookup for the given host.
        //It will search for the requested information if there is no other active request.
        CFStreamError error;
        success = CFHostStartInfoResolution(theHost, kCFHostAddresses, &error);
        if (!success) {
             if (showDebug) NSLog(@"CFHostStartInfoResolution returned error (%d, %ld)", error.domain, error.error);
        }
    } 
    else {
        if (showDebug) NSLog(@"CFHostSetClient failed");
        CFRelease(host);
    }
    
    tcpConnection = ccon;
    connectTimer.connection = ccon;
}


/**
*   Tells the underlying layer to disconnect the
*   current connection.
*/
void IPhoneTCPConnectionHandler::disconnect(TCPClientConnection* ccon)
{
	if (showDebug) NSLog(@"disconnect called");
	
	//inform callback of success in closing
	ccon->connectionClosed(TCPClientConnection::CLOSED);	
	tcpConnection = NULL;	
}


/**
*   Tells the underlying layer to write the supplied
*   bytes. The underlying layer must keep the order
*   of the write requests.
*   Should call ccon->writeDone when done.
*   Max one write per ClientConnection is allowed.
*   Not allowed if not connected.
*/
void IPhoneTCPConnectionHandler::write(const byte* bytes, int length,
                      TCPClientConnection* ccon)
{
    CFDataRef data = CFDataCreate(NULL, (const UInt8*)bytes, length);
    CFSocketError res = CFSocketSendData(mySocket, NULL, data, 5); 
    CFRelease(data);

    if (res == kCFSocketSuccess) {
        if (showDebug) NSLog(@"write/send: Success");
        ccon->writeDone(TCPClientConnection::OK); //supposed to be connected already
    }
    else if (res == kCFSocketTimeout) {
        if (showDebug) NSLog(@"write/send: Timeout");
        ccon->writeDone(TCPClientConnection::TIMEOUT); 
    }
    else { //if (res == kCFSocketError) 
        if (showDebug) NSLog(@"write/send: Error");		
        ccon->writeDone(TCPClientConnection::ERROR); 
    }
}


/**
*   Tells the underlying layer to start receiving reads
*   with up to maxLength bytes from the 
*   socket. Max <code>length</code> bytes.
*   Should call the ccon->readDone when done.
*   Max one read per ClientConnection is allowed.
*   Not allowed if not connected.
*/
void IPhoneTCPConnectionHandler::read(int maxLength,
                     TCPClientConnection* ccon)
{
	if (showDebug) NSLog(@"read called");
}


/**
*   Removes the connection from the internal list of
*   connections in the TCPConnectionHandler.
*/
void IPhoneTCPConnectionHandler::remove(TCPClientConnection* conn)
{
	if (showDebug) NSLog(@"remove called");

	disconnect(conn);						
}


/**
*   Requests a timeout for the connection conn.
*   After the timeout, the connectionhandler should call
*   timerExpired on the connection.
*   @param nbrMillis The number of milliseconds to wait before
*                    sending the timeout.
*   @param conn      The connection that should have the timeout.
*   @return ID of the timeout.
*/
int IPhoneTCPConnectionHandler::requestTimer(uint32 nbrMillis,
                            TCPClientConnection* conn)
{
	if (showDebug) NSLog(@"requestTimer called");
	
	if (++connTimerId == 0)
        connTimerId = 1;
    [connectTimer startTimer:nbrMillis connection:conn timerID:connTimerId];
        
	return connTimerId;
}


/**
*   Cancels the timer with id timerID.
*/
void IPhoneTCPConnectionHandler::cancelTimer(int timerID)
{
	if (showDebug) NSLog(@"cancelTimer called");				
    
    if (timerID == connectTimer.timerId) {
        [connectTimer.connTimer invalidate];    
    }
}


void IPhoneTCPConnectionHandler::dealWithData(CFSocketRef s, 
                                              CFSocketCallBackType type, 
                                              CFDataRef address, 
                                              const void *data, 
                                              void *info)
{
    IPhoneTCPConnectionHandler *parent = (IPhoneTCPConnectionHandler*)info;
		
    if (type==kCFSocketDataCallBack)
    {
        if (showDebug) NSLog(@"received %d bytes from socket %d", CFDataGetLength((CFDataRef)data), CFSocketGetNative(s));
		
        if (CFDataGetLength((CFDataRef)data) > 0) {
            parent->tcpConnection->readDone(TCPClientConnection::OK,
                                            (const byte*)CFDataGetBytePtr((CFDataRef)data),
                                            CFDataGetLength((CFDataRef)data));
        }
        else {
            if (showDebug) NSLog(@"Connection interrupted.");
        }
    }
}
