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

typedef unsigned int uint32;

static inline uint32 currentTimeMillis() {
   struct timeval tv;
   gettimeofday(&tv, NULL);
   return (tv.tv_usec/1000 + tv.tv_sec * 1000);
}

static BOOL showDebug = NO;

uint32 IPhoneTCPConnectionHandler::s_connTimerId = 0;

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
	: m_connectTimer(NULL)
{
   m_connectTimer = [[ConnectionTimer alloc] retain];
   m_conInfo.socket = NULL;
   m_conInfo.host = NULL;
   m_conInfo.con = NULL;
   m_conInfo.port = 0;
}

IPhoneTCPConnectionHandler::~IPhoneTCPConnectionHandler()
{
    [m_connectTimer release];
}


void IPhoneTCPConnectionHandler::createSocket()
{
    CFSocketContext connContext = { 0, this, NULL, NULL, NULL };
	
	m_conInfo.socket = CFSocketCreate( kCFAllocatorDefault, 
                                      PF_INET, 
                                      SOCK_STREAM, 
                                      IPPROTO_TCP, 
                                      kCFSocketDataCallBack | kCFSocketConnectCallBack, 
                                      (CFSocketCallBack)&dealWithData, &connContext);

	CFRunLoopSourceRef source =
      CFSocketCreateRunLoopSource(kCFAllocatorDefault, m_conInfo.socket, 0);
	CFRunLoopAddSource(CFRunLoopGetCurrent(), source, kCFRunLoopDefaultMode);
	CFRelease(source);
}


static void hostClientCallBack(CFHostRef host,
                               CFHostInfoType typeInfo,
                               const CFStreamError *error,
                               void *info)
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
                
                // CFHostGetAddressing retrieves the known addresses from
                // the given host.
                CFArrayRef addresses = CFHostGetAddressing(inf->host, NULL);

                if (showDebug) NSLog(@"Attempting to resolve %s", 
                                     inf->host);
                
                if (addresses) {
                    struct sockaddr_in *addr = 
                        (struct sockaddr_in *)CFDataGetBytePtr(
                           (CFDataRef)CFArrayGetValueAtIndex(addresses, 0));
                        
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

                CFDataRef address = CFDataCreate(NULL,
                                                 (const UInt8*)(char*)&sin,
                                                 sizeof(sin));

                if (showDebug) NSLog(@"Initiating asynchronous socket connect ms");

                // Issue asynchronous connect                
                CFSocketError res = CFSocketConnectToAddress(inf->socket,
                                                             address,
                                                             -1);

                CFRelease(address);
                
                if (res != kCFSocketSuccess) {
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
void IPhoneTCPConnectionHandler::connect(const char* host,
                                         unsigned int port,
                                         TCPClientConnection* ccon)
{
    if (showDebug) NSLog(@"********** connect called **********");

    uint32 startTimeMs = currentTimeMillis();
    
    createSocket();

    std::string hostReduced(host);
    std::string httpStr("http://");
    // If prefixed with HTTP, remove
    if(hostReduced.substr(0, httpStr.size()) == httpStr) {
       hostReduced = hostReduced.substr(httpStr.size());
    }
    
    CFStringRef hostname = CFStringCreateWithCString(NULL, hostReduced.c_str(), kCFStringEncodingASCII);
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

    m_conInfo.host = theHost;
    m_conInfo.port = port;
    m_conInfo.con = ccon;

    //CFHostSetClient associates a client context and callback function with a CFHostRef.
    //This is required for asynchronous usage.  If not set, resolve will take place synchronously.
    CFHostClientContext context = {0, (void*)&m_conInfo, NULL, NULL, NULL};
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
    
    m_connectTimer.connection = ccon;
    
    printf("IPhoneTCPConnectionHandler::connect() took %u ms\n",
           currentTimeMillis() - startTimeMs);
}


/**
*   Tells the underlying layer to disconnect the
*   current connection.
*/
void IPhoneTCPConnectionHandler::disconnect(TCPClientConnection* ccon)
{
	if (showDebug) NSLog(@"disconnect called");
	

   if (m_conInfo.socket) {
      CFSocketInvalidate(m_conInfo.socket);
      CFRelease(m_conInfo.socket);
      m_conInfo.socket = NULL;
   }
	//inform callback of success in closing   
	ccon->connectionClosed(TCPClientConnection::CLOSED);
}


/**
*   Tells underlying layer to write the supplied
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
    CFSocketError res = CFSocketSendData(m_conInfo.socket, NULL, data, 0); 
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
	
	if (++s_connTimerId == 0)
        s_connTimerId = 1;
    [m_connectTimer startTimer:nbrMillis connection:conn timerID:s_connTimerId];
        
	return s_connTimerId;
}

/**
*   Cancels the timer with id timerID.
*/
void IPhoneTCPConnectionHandler::cancelTimer(int timerID)
{
	if (showDebug) NSLog(@"cancelTimer called");				
    
    if (timerID == m_connectTimer.timerId) {
        [m_connectTimer.connTimer invalidate];    
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
       if (showDebug) NSLog(@"received %d bytes from socket %d (%s)",
                            CFDataGetLength((CFDataRef)data),
                            CFSocketGetNative(s),
                            (char*)data);
       
        if (CFDataGetLength((CFDataRef)data) > 0) {
            parent->m_conInfo.con->readDone(TCPClientConnection::OK,
                                            (const byte*)CFDataGetBytePtr((CFDataRef)data),
                                            CFDataGetLength((CFDataRef)data));
        }
        else {
            if (showDebug) NSLog(@"Connection interrupted.");
            parent->disconnect(parent->m_conInfo.con);
        }
    } else if(type == kCFSocketConnectCallBack) {
       if(data == NULL) {
          if (showDebug) NSLog(@"Successfully initiated async connection");
 
          parent->m_conInfo.con->connectDone(TCPClientConnection::OK);
       } else {
          SInt32 err = *((SInt32*)data);
          
          if (err == kCFSocketTimeout) {
             if (showDebug) NSLog(@"Asynch connection failed, timeout.");
             parent->m_conInfo.con->connectDone(TCPClientConnection::TIMEOUT); 		
          }
          else { //if (res == kCFSocketError) 
             if (showDebug) NSLog(@"Asynch connection failed, general error.");
             parent->m_conInfo.con->connectDone(TCPClientConnection::ERROR); 	
          }
       }
    }
}
