/*
  Copyright (c) 1999 - 2010, Vodafone Group Services Ltd
  All rights reserved.

  Redistribution and use in source and binary forms, with or without modification, are permitted provided that the following conditions are met:

  * Redistributions of source code must retain the above copyright notice, this list of conditions and the following disclaimer.
  * Redistributions in binary form must reproduce the above copyright notice, this list of conditions and the following disclaimer in the documentation and/or other materials provided with the distribution.
  * Neither the name of the Vodafone Group Services Ltd nor the names of its contributors may be used to endorse or promote products derived from this software without specific prior written permission.

  THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS" AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE ARE DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT HOLDER OR CONTRIBUTORS BE LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
*/

#include "WinTCPConnectionHandler.h"

#include "TileMapUtil.h"
static uint32 connectBegin, connectEnd;
static uint32 readBegin, readEnd;
static uint32 writeBegin, writeEnd;

#include <string>
#include "LogFile.h"

/* printing macros */
#define HEX(x)    hex << "0x" << x << dec
#define DEC(x)    dec << x 
#define OCT(x)    oct << "O" << x << dec

/* the async message id for the socket */
#define SOCKMSG   WM_USER+5

/* the size of the read buffer */
#define READ_BUFFER_SIZE   (128*1024)

/* the name of the socket window */
static const LPCTSTR SOCKCLASSNAME = _T("MAPSOCKCLASS");

/* Client Socket Implementation */

/* private constructor */
ClientSocket::ClientSocket()
: m_socket(INVALID_SOCKET),
  m_parentWindow(NULL),
  m_host(""),
  m_port(0)
{
}

/* allocator - async messages are sent to the parent window */
ClientSocket* ClientSocket::allocate(HWND parentWin)
{
   /* create a new object */
   ClientSocket* newObj = new ClientSocket();
   if(newObj == NULL) return(NULL);

   /* do second-phase construction */
   if(!newObj->construct(parentWin)) {
      /* error during construction, delete the object */
      delete newObj;
      return(NULL);
   }

   /* success, return the allocated object */
   return(newObj);
}

/* second-phase constructor */
bool ClientSocket::construct(HWND parentWin)
{
   if(parentWin) {
      m_parentWindow = parentWin;
   }
   else {
      /* invalid window handle - NULL */
      return(false);
   }

   /* success everywhere */
   return(true);
}

/* destructor */
ClientSocket::~ClientSocket()
{
   disconnect();
}

DWORD WINAPI ClientSocket::eventSelectThread(LPVOID pParam)
{
   ClientSocket* pClientSocket = (ClientSocket*) pParam;
   pClientSocket->eventSelectThreadImpl();
   return TRUE;
}

void ClientSocket::eventSelectThreadImpl()
{
   WSANETWORKEVENTS wsaEvents;
   int nIndex = 0;
   int errCode;  
   LPARAM lParam;

   while( (nIndex != WSA_WAIT_FAILED) && (nIndex != WSA_WAIT_TIMEOUT) ) {
      /* Waiting for the network events */
      errCode = 0; 
      nIndex = ::WSAWaitForMultipleEvents(1, &m_hEventSelect, FALSE, WSA_INFINITE, FALSE); 

      if ((nIndex != WSA_WAIT_FAILED) && (nIndex != WSA_WAIT_TIMEOUT)) {
         /* Determine which network event occured */
         ::WSAEnumNetworkEvents(m_socket, m_hEventSelect, &wsaEvents);   

         if(wsaEvents.iErrorCode[FD_CONNECT_BIT] != 0 || 
            wsaEvents.iErrorCode[FD_READ_BIT]    != 0 ||
            wsaEvents.iErrorCode[FD_CLOSE_BIT]   != 0) {
               errCode = WSAGetLastError();
            }

         /* Prepare the network event information */
         lParam = MAKELPARAM(wsaEvents.lNetworkEvents, errCode); 

         /* Send message to window */ 
         PostMessage(m_parentWindow, SOCKMSG, m_socket, lParam);

         if(wsaEvents.lNetworkEvents == FD_CLOSE) 
            break;
      }
      else {
         break;
      }

      WSAResetEvent(m_hEventSelect);

      
   }

   WSACloseEvent(m_hEventSelect);
}

/* connects to the specified host and port */
int ClientSocket::connect(const char* hostName, int32 port)
{
   /* do safety disconnect */
   disconnect();

   int errCode = 0;
   /* try to create the socket, expects Winsock to be already initialised */
   m_socket = ::socket( AF_INET, SOCK_STREAM, 0 );
   if(m_socket == INVALID_SOCKET) {
      /* socket could not be created */
      errCode = WSAGetLastError();
      LogFile::Print("SOCKET : Error : Creation : %d\n", errCode);
      return(errCode);
   }

   /* set socket options - enable KeepAlive */
   int keepAlive = 1;
   ::setsockopt(m_socket, IPPROTO_TCP, 
                SO_KEEPALIVE, 
                (const char*)&keepAlive, sizeof(int));

   /* try to set the Asynchronous handler for the socket */
   m_hEventSelect = WSACreateEvent();
   errCode = WSAEventSelect(m_socket, m_hEventSelect, FD_READ | FD_CLOSE | FD_CONNECT);

   if(errCode == SOCKET_ERROR) {
      errCode = WSAGetLastError();
      /* error on event selection, close the socket */
      LogFile::Print("SOCKET : Error : Event selection: %d\n", errCode);
      disconnect();
      WSACloseEvent(m_hEventSelect);
      return(errCode);
   }    

   /* Create thread for selecting network events */
   DWORD dwThreadID;
   HANDLE hEventSelectThread = CreateThread(NULL, 0, ClientSocket::eventSelectThread, 
                                            this, 0L, &dwThreadID);
   CloseHandle(hEventSelectThread);

   /* make the connection */
   struct hostent* hostIP;
   /* get the IP address */
   hostIP = gethostbyname( hostName );
   if( hostIP == NULL ) {
      /* error on DNS Search */
      errCode = ::WSAGetLastError();
      return(errCode);
   }

   LogFile::Print("SOCKET : Remote Host : %s\n", hostIP->h_name);

   int A = ((unsigned int)hostIP->h_addr_list[0][0]) & 0xff;
   int B = ((unsigned int)hostIP->h_addr_list[0][1]) & 0xff;
   int C = ((unsigned int)hostIP->h_addr_list[0][2]) & 0xff;
   int D = ((unsigned int)hostIP->h_addr_list[0][3]) & 0xff;
   LogFile::Print("SOCKET� : Remote IP : %d.%d.%d.%d \n", A, B, C, D);
   LogFile::Print("SOCKET : Remote Port : %d \n", port);

   /* create the connection address */
   struct sockaddr_in sin;
   memset( &sin, 0, sizeof(sin) );
   sin.sin_family = AF_INET;
   sin.sin_addr.s_addr = ((struct in_addr *)(hostIP->h_addr))->s_addr;
   sin.sin_port = htons( port );

   /* store the remote host info */
   m_host = hostName;
   m_port = port;

   /* do the connection asynchronously */
   errCode = ::connect( m_socket, (const sockaddr*)&sin, sizeof(sin));
   if(errCode == SOCKET_ERROR) {
      /* error on connect */
      errCode = WSAGetLastError();
      if(errCode == WSAEWOULDBLOCK) {
         /* this is normal, as we have selected async connection, move on */
         errCode = 0;
      }
      else {
         /* major error, do the needful */
         LogFile::Print("SOCKET : Error : Connect : %d \n", errCode);
         return(errCode);
      }
   }

   /* success */
   return(errCode);
}

/* reconnects the socket to the previously specified host:port */
int ClientSocket::reconnect()
{
   return(connect(m_host.c_str(), m_port));
}

/* disconnects the socket */
int ClientSocket::disconnect()
{
   int errCode = 0;
   /* check if the socket is connected */
   if(m_socket != INVALID_SOCKET) {
      /* shut down the socket */
      shutdown();
      /* close the socket handle */
      errCode = ::closesocket(m_socket);
      if(errCode == SOCKET_ERROR) {
         errCode = WSAGetLastError();
         LogFile::Print("SOCKET : Error : Disconnect : %d \n", errCode);         
      }
      /* invalidate the handle - IMP. since this is used to check for active
         connections. */
      m_socket = INVALID_SOCKET;
   }
   return(errCode);
}

/* shuts down all operations on the socket, but the socket remains connected */
int ClientSocket::shutdown()
{
   int errCode = 0;
   /* shut down all communications on the socket */
   errCode = ::shutdown(m_socket, 2);
   if(errCode == SOCKET_ERROR) {
      /* there has been an error */
      errCode = WSAGetLastError();
      LogFile::Print("SOCKET : Error : Shutdown : %d \n", errCode);         
   }
   return(errCode);
}

/* writes the specified bytes to the socket */
int ClientSocket::write(const byte* dataBuf, int32 len)
{
   /* check if the socket is connected */
   if(m_socket == INVALID_SOCKET) return(IO_CLOSED);

   int errCode;
   /* try to do the write - Testing MSG_DONTROUTE - Remove if it causes problems */
   errCode = ::send(m_socket, (const char*)dataBuf, len, MSG_DONTROUTE);
   if(errCode == SOCKET_ERROR) {
      /* error while sending data */
      errCode = WSAGetLastError();
      /* check if the socket is invalid or not connected */
      if(errCode == WSAENOTCONN || errCode == WSAESHUTDOWN || 
         errCode == WSAECONNABORTED || errCode == WSAECONNRESET) {
         /* yup, tell the caller that the socket is closed */
         LogFile::Print("SOCKET : Error : Write : Socket Closed : %d \n", errCode);         
         return(IO_CLOSED);
      }
      else {
         /* some other error */
         LogFile::Print("SOCKET : Error : Write : %d \n", errCode);         
         return(IO_ERROR);
      }
   }

   /* success */
   return(IO_OK);
}

/* reads bytes from the socket and returns 
   the data in the specified buffer. Returns 
   the actual number of bytes read in bytesRead */
int ClientSocket::read(byte* dataBuf, int32 maxReadLen, int32& bytesRead)
{
   /* check if the socket is connected */
   if(m_socket == INVALID_SOCKET) return(IO_CLOSED);

   int nbrBytes, errCode;
   /* read the socket for the specfied number of bytes */
   nbrBytes = ::recv(m_socket, (char*)dataBuf, maxReadLen, 0);
   if(nbrBytes == 0) {
      /* connection has been closed */
      LogFile::Print("SOCKET : Error : Read : Socket Closed Before Read");         

      return(IO_CLOSED);
   }
   if(nbrBytes == SOCKET_ERROR) {
      /* error on read */
      errCode = WSAGetLastError();
      if(errCode == WSAENOTCONN || errCode == WSAESHUTDOWN || 
         errCode == WSAECONNABORTED || errCode == WSAECONNRESET) {
         /* yup, tell the caller that the socket is closed */
         LogFile::Print("SOCKET : Error : Write : Socket Closed : %d \n", errCode);         
         return(IO_CLOSED);
      }
      else {
         /* some other error */
         LogFile::Print("SOCKET : Error : Read : %d \n", errCode);      
         return(IO_ERROR);
      }
   }

   /* set the number of bytes actually read */
   bytesRead = nbrBytes;

   /* success */
   return(IO_OK);
}



/* initialize the static variables */
WinTCPConnectionHandler* WinTCPConnectionHandler::m_curObject = NULL;

/* constructor */
WinTCPConnectionHandler::WinTCPConnectionHandler()
   : m_hwnd(NULL),
     m_conn(NULL),
     m_listener(NULL),
     m_bytesToWrite(0),
     m_writeBuffer(NULL),
     m_bytesToRead(0),
     m_readBuffer(NULL)
{
}

/* second-phase constructor */
bool WinTCPConnectionHandler::construct(HINSTANCE hInst)
{
   /* try to allocate read buffer */
   m_readBuffer = new byte[READ_BUFFER_SIZE];
   if(m_readBuffer == NULL) {
      /* insufficient memory */
      return(false);
   }

   /* set the instance */
   m_appInstance = hInst;

   /* create the window class if required */
   if(!classExists(SOCKCLASSNAME)) {
      /* create the message processing window */
	   WNDCLASS wc;

	   /* initialize window structure for non-visible window */
	   wc.style         = CS_NOCLOSE;
	   wc.lpfnWndProc   = WinTCPConnectionHandler::SocketWndProc;
	   wc.cbClsExtra    = 0;
	   wc.cbWndExtra    = 0;
	   wc.hInstance     = m_appInstance;
	   wc.hIcon         = LoadIcon(NULL, IDC_ICON);
	   wc.hCursor       = LoadCursor(NULL, IDC_ARROW);
	   wc.hbrBackground = (HBRUSH)(COLOR_WINDOW+1);
	   wc.lpszMenuName  = NULL;
	   wc.lpszClassName = SOCKCLASSNAME;

	   /* register the window class */
	   if(!RegisterClass(&wc)) {
		   return(false);
	   }
   }

	/* create the window */
	m_hwnd = CreateWindowEx(WS_EX_STATICEDGE, 
                           SOCKCLASSNAME, _T(""), 
                           WS_DISABLED, 
			                  1, 1, 
                           1, 1, 
                           NULL, NULL, 
                           m_appInstance, NULL);
   /* check for errors */
   if(m_hwnd == NULL) {
		return(false);
	}

   /* try to initialize WinSock */
   WSADATA winsockInfo;
   WORD winsockReqVersion = MAKEWORD(1, 1);
   if(WSAStartup(winsockReqVersion, &winsockInfo) != 0) {
      /* error during WinSock startup */
		return(false);
	}

   /* create the socket connection */
   m_conn = ClientSocket::allocate(m_hwnd);
   if(m_conn == NULL) {
      /* cannot create connection */
      return(false);
   }

   /* success */
   return(true);
}

/* Allocator.
 * hInst : the parent application's instance.
 */
WinTCPConnectionHandler* WinTCPConnectionHandler::allocate(HINSTANCE hInst)
{
   LogFile::Print("WINTCP : Allocating...\n");      

   /* create the object */
   WinTCPConnectionHandler* newObj = new WinTCPConnectionHandler();
   if(newObj == NULL) return(NULL);

   LogFile::Print("WINTCP : Initializing...\n");      

   /* do second-phase construction */
   if(!newObj->construct(hInst)) {
      /* something went wrong */
      delete newObj;
      return(NULL);
   }

   LogFile::Print("WINTCP : Done : %d \n", newObj);

   /* success, return the newly created object */
   return(newObj);
}

/* destructor */
WinTCPConnectionHandler::~WinTCPConnectionHandler()
{
   LogFile::Print("WINTCP : Deleting...\n");

   /* close and release the socket */
   delete m_conn;
   /* shutdown WinSock */
   WSACleanup();
   /* destroy the window */
   DestroyWindow(m_hwnd);
   /* delete the read buffer */
   delete m_readBuffer;

   LogFile::Print("Done!\n");
}

/**
 *   Tells the underlying layer to connect to the
 *   supplied host and port. When connected, connected
 *   should be called.
 *   Should call ccon->connectDone when done.
 *   Max one connect per ClientConnection is allowed simultaneously.
 */
void WinTCPConnectionHandler::connect(const char* host, unsigned int port,
                                      TCPClientConnection* ccon)
{
   LogFile::Print("WINTCP : Connecting...\n");

   /* prepare for async */
   prepareForAsync(ccon);

   /* log the starting time */
   connectBegin = TileMapUtil::currentTimeMillis();

   /* try the connect */
   int errCode = m_conn->connect(host, port);
   if(errCode != 0) {
      /* the connect failed for some reason, inform the listener */
      ccon->connectDone((TCPClientConnection::status_t)TCPERROR);
      return;
   }

   return;
}


/**
 *   Tells the underlying layer to disconnect the
 *   current connection.
 */
void WinTCPConnectionHandler::disconnect(TCPClientConnection* ccon)
{
   /* reset our state variables */
   m_bytesToWrite = 0;
   m_bytesToRead = 0;
   /* do the disconnection */
   m_conn->disconnect();
   return;
}

/**
 *   Tells the underlying layer to write the supplied
 *   bytes. The underlying layer must keep the order
 *   of the write requests.
 *   Should call ccon->writeDone when done.
 *   Max one write per ClientConnection is allowed.
 *   Not allowed if not connected.
 */
void WinTCPConnectionHandler::write(const byte* bytes, int length,
                                    TCPClientConnection* ccon)
{
   writeBegin = TileMapUtil::currentTimeMillis();

   /* try to write */
   int errCode = m_conn->write(bytes, length);
   /* check for errors */
   if(errCode == ClientSocket::IO_CLOSED) {
      ccon->writeDone((TCPClientConnection::status_t)TCPCLOSED);
   }
   else if(errCode == ClientSocket::IO_ERROR) {
      ccon->writeDone((TCPClientConnection::status_t)TCPERROR);
   }
   else {
      writeEnd = TileMapUtil::currentTimeMillis();
      LogFile::Print("WINTCP : Wrote %d bytes\n", length);
      LogFile::Print("WINTCP : Write : %d MS\n", (writeEnd - writeBegin));

      /* success */
      ccon->writeDone((TCPClientConnection::status_t)TCPOK);
   }

   return;
}

/**
 *   Tells the underlying layer to start receiving reads
 *   with up to maxLength bytes from the 
 *   socket. Max <code>length</code> bytes.
 *   Should call the ccon->readDone when done.
 *   Max one read per ClientConnection is allowed.
 *   Not allowed if not connected.
 */
void WinTCPConnectionHandler::read(int maxLength,
                                   TCPClientConnection* ccon)
{
   /* prepare async */
   prepareForAsync(ccon);

   /* set the max bytes to read .. add it to the current total */
   m_bytesToRead += maxLength;
   /* check if number of bytes to read is greater than the buffer size,
      clamp if it is */
   if(m_bytesToRead > READ_BUFFER_SIZE) m_bytesToRead = READ_BUFFER_SIZE;

   LogFile::Print("WINTCP : Read : Waiting for a maximum of %d bytes\n", m_bytesToRead);

   readBegin = TileMapUtil::currentTimeMillis();
  
   return;
}

/**
 *   Removes the connection from the internal list of
 *   connections in the TCPConnectionHandler.
 */
void WinTCPConnectionHandler::remove(TCPClientConnection* conn)
{
   disconnect(conn);
   return;
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
int WinTCPConnectionHandler::requestTimer(uint32 nbrMillis,
                                          TCPClientConnection* conn)
{
   return(0);
}

/**
 *   Cancels the timer with id timerID.
 */
void WinTCPConnectionHandler::cancelTimer(int timerID)
{
   return;
}

/*
 * The Window Message Processing method.
 */
LRESULT CALLBACK WinTCPConnectionHandler::SocketWndProc(HWND hwnd, 
                                                        UINT msg, 
                                                        WPARAM wParam, 
                                                        LPARAM lParam)
{
	switch(msg)
	{
      case(WM_CREATE):
      {
         break;
      }
      /* our main socket message handling */
      case(SOCKMSG):
      {
         /* get the current object */
         WinTCPConnectionHandler* tcpConn = WinTCPConnectionHandler::m_curObject;
         TCPClientConnection* listener = tcpConn->m_listener;
         ClientSocket* sockConn = tcpConn->m_conn;

         /* get our information */
         SOCKET sock = (SOCKET)wParam;
      	WORD event = LOWORD(lParam);
	      WORD error = HIWORD(lParam);
        
         switch(event)
         {
            /* connection completed */
            case(FD_CONNECT):
            {
               /* log the ending time */
               connectEnd = TileMapUtil::currentTimeMillis();
               LogFile::Print("WINTCP : Connect : %d MS\n", (connectEnd - connectBegin));

               if(error != 0) {
                  listener->connectDone((TCPClientConnection::status_t)TCPERROR);
               }
               else {
                  LogFile::Print("WINTCP : Connected!\n");
                  listener->connectDone((TCPClientConnection::status_t)TCPOK);
               }

               break;
            }
            /* connection closed */
            case(FD_CLOSE):
            {
               LogFile::Print("WINTCP : Socket Closed!\n");
               /* reset our state variables */
               tcpConn->m_bytesToWrite = 0;
               tcpConn->m_bytesToRead = 0;
               /* ensure a disconnection */
               sockConn->disconnect();
               listener->connectionClosed((TCPClientConnection::status_t)TCPOK);
               break;
            }
            /* we can write now */
            case(FD_WRITE):
            {
               /* check if any write is needed */
               if(tcpConn->m_bytesToWrite == 0) break;

               writeEnd = TileMapUtil::currentTimeMillis();
               LogFile::Print("WINTCP : Write : %d MS\n", (writeEnd - writeBegin));               

               /* try to write */
               int errCode = sockConn->write(tcpConn->m_writeBuffer, 
                                             tcpConn->m_bytesToWrite);

               /* reset variables */
               int32 bytesWritten = tcpConn->m_bytesToWrite;
               tcpConn->m_writeBuffer = NULL;
               tcpConn->m_bytesToWrite = 0;

               /* check for errors */
               if(errCode == ClientSocket::IO_CLOSED) {
                  tcpConn->disconnect(listener);
                  //listener->writeDone((TCPClientConnection::status_t)TCPCLOSED);
               }
               else if(errCode == ClientSocket::IO_ERROR) {
                  listener->writeDone((TCPClientConnection::status_t)TCPERROR);
               }
               else {
                  LogFile::Print("WINTCP : Wrote %d bytes\n", bytesWritten);               
                  /* success */
                  listener->writeDone((TCPClientConnection::status_t)TCPOK);
               }

               break;
            }
            /* we can read now */
            case(FD_READ):
            {
               /* check if any read is pending */
               if(tcpConn->m_bytesToRead == 0) break;

               readEnd = TileMapUtil::currentTimeMillis();
               LogFile::Print("WINTCP : Read : %d MS\n", (readEnd - readBegin));     

               /* try to read */
               int errCode, bytesRead = 0;
               errCode = sockConn->read(tcpConn->m_readBuffer,
                                        tcpConn->m_bytesToRead,
                                        bytesRead);

               /* set number of bytes to read to the leftover amount */
               tcpConn->m_bytesToRead = tcpConn->m_bytesToRead - bytesRead;
               if(tcpConn->m_bytesToRead < 0) tcpConn->m_bytesToRead = 0;

               /* check for errors */
               if(errCode == ClientSocket::IO_CLOSED) {
                  tcpConn->disconnect(listener);
                  //listener->readDone((TCPClientConnection::status_t)TCPCLOSED,
                  //                   NULL, 0);
               }
               else if(errCode == ClientSocket::IO_ERROR) {
                  listener->readDone((TCPClientConnection::status_t)TCPERROR,
                                     NULL, 0);
               }
               else {
                  LogFile::Print("WINTCP : Read : %d bytes\n", bytesRead);     
                  /* success */
                  listener->readDone((TCPClientConnection::status_t)TCPOK,
                                     tcpConn->m_readBuffer, bytesRead);
               }

               break;
            }
            default: break;
         }

         break;
      }
		default:
		{
			return DefWindowProc(hwnd, msg, wParam, lParam);
		}
	}

   /* all done */
   return(0);
}

/* check if the class name already exists */
bool WinTCPConnectionHandler::classExists(LPCTSTR className)
{
   /* use GetClassInfoEx to see if the class exists */
   WNDCLASS wnd;
   int retCode = GetClassInfo(m_appInstance, 
                                className,
                                &wnd);
   /* if return code is zero, the class does no exist; return false */
   if(retCode == 0) {
      return(false);
   }

   /* class found, return true */
   return(true);
}
