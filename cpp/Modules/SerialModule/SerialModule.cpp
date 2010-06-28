/*
  Copyright (c) 1999 - 2010, Vodafone Group Services Ltd
  All rights reserved.

  Redistribution and use in source and binary forms, with or without modification, are permitted provided that the following conditions are met:

  * Redistributions of source code must retain the above copyright notice, this list of conditions and the following disclaimer.
  * Redistributions in binary form must reproduce the above copyright notice, this list of conditions and the following disclaimer in the documentation and/or other materials provided with the distribution.
  * Neither the name of the Vodafone Group Services Ltd nor the names of its contributors may be used to endorse or promote products derived from this software without specific prior written permission.

  THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS" AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE ARE DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT HOLDER OR CONTRIBUTORS BE LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
*/


#ifndef __SYMBIAN32__

#include "arch.h"
#include "Serial.h"
#include "SerialModule.h"
#include "Buffer.h"

#if !defined(_MSC_VER) && !defined(__SYMBIAN32__)
# include <sys/types.h>
# include <sys/stat.h>
# include <fcntl.h>
# include <unistd.h>
# include <sys/file.h>
#else
# define WINCE
#endif

#include "LogMacros.h"

const isab::SerialModule::speed_pair_t isab::SerialModule::speed_table[] = {
#ifndef _MSC_VER
   {      0,      B0 },
   {   4800,   B4800 },
   {   9600,   B9600 },
   {  19200,  B19200 },
   {  38400,  B38400 },
   {  57600,  B57600 },
   { 115200, B115200 },
#ifndef __CYGWIN__
   { 230400, B230400 },
#endif
   {      0,      B0 }
#else
#define B0 CBR_110
   {      0,   CBR_110 },
   {   4800,  CBR_4800 },
   {   9600,  CBR_9600 },
   {  19200, CBR_19200 },
   {  38400, CBR_38400 },
   {  57600, CBR_57600 },
   { 115200,CBR_115200 },
   {      0,   CBR_110 }
#endif
};

isab::SerialModule::SerialModule(int portnumber,
                                 int speed,
                                 tcflag_t andflags,
                                 tcflag_t orflags,
                                 bool blocking) : 
   Module("SerialModule"), m_portnumber(portnumber), m_fd(-1), 
   m_speed(speed), m_andflags(andflags), m_orflags(orflags), 
   m_blockingio(blocking), m_state(CLOSED)
{
#ifdef _MSC_VER
   m_hPort = INVALID_HANDLE_VALUE;
   m_fileFlags = 0;
#endif

   if (portnumber > 0){
      if (open() > 0){
         ERR("Failed to open port %d", portnumber);
      } else {
         INFO("Opened port %d", portnumber);
         m_serialthread = new SerialThread(m_serialConsumer, this);
         m_serialthread->start();
         THREADINFO(*m_serialthread);
      }
   } else {
      DBG("Waiting for connection message");
   }
   
}


int isab::SerialModule::open()
{
   DBG("open()");

   // Find the speed in the table
   speed_t speed_code = B0;
   if (!lookupSpeed(m_speed, speed_code)) {
      ERR("Error in open(): invalid speed (%u) for serialDevice", m_speed);
      return -1;
   }

   if (m_state != CLOSED) {
      ERR("SerialModule::open: Serial port not closed before open!");
      return -1;
   }
   
#ifndef _MSC_VER
   // Speed was found. Now set up the port.
   char deviceName[30];
#ifndef __CYGWIN__
   sprintf(deviceName, "/dev/ttyS%d", m_portnumber);
#else
   sprintf(deviceName, "/dev/com%d", m_portnumber+1);
#endif
   DBG("open():trying to open %s", deviceName);
   m_fd = ::open(deviceName, O_RDWR | O_NOCTTY | O_NONBLOCK);
   if ( m_fd < 0 ) {
      ERR("open last system error: %s", strerror(errno));
      ERR("Couldn't open device %s", deviceName);
      m_state = ERRORE;
      return m_fd;
   }
   if (isatty( m_fd ) == 0) {
      ERR("%s is not a tty", deviceName);
      ERR("open isatty: %s", strerror(errno));
      m_state = ERRORE;
      return m_fd;
   }
#ifndef __CYGWIN__
   if ( flock(m_fd, LOCK_EX|LOCK_NB) < 0 ) {
      ERR("open - couldn't get exclusive lock: %s", 
          strerror(errno));
      m_state = ERRORE;
      return -1;
   }
#endif   

   int oldfl;
   /* Set terminal to non-blocking */
   if ((oldfl=fcntl(m_fd, F_GETFL)) < 0) {
      /* Error */
      ERR("getfl: %s", strerror(errno));
      m_state = ERRORE;
      return -1;
   }
   
   if (fcntl(m_fd, F_SETFL, oldfl | O_NONBLOCK) < 0) {
      /* Error */
      ERR("setfl %s", strerror(errno));
      m_state = ERRORE;
      return -1;
   }

   struct termios term_ent;      
   /* Set terminal to correct values. */
   if (tcgetattr(m_fd , &term_ent)<0) {
      /* Failed. */
      ERR("Could not get terminal attributes %s", strerror(errno));
      return -1;
   }
   /* Worked. */

   //   tcflag_t c_cflag;
   //   c_cflag=term_ent.c_cflag;
   term_ent.c_cflag |= m_orflags; //Recomends CLOCAL;
   term_ent.c_cflag &= m_andflags; //Recomends ~CRTSCTS;
#ifndef __CYGWIN__
   cfmakeraw(&term_ent);
#else
   term_ent.c_iflag &= ~(IGNBRK|BRKINT|PARMRK|ISTRIP
                           |INLCR|IGNCR|ICRNL|IXON);
   term_ent.c_oflag &= ~OPOST;
   term_ent.c_lflag &= ~(ECHO|ECHONL|ICANON|ISIG|IEXTEN);
   term_ent.c_cflag &= ~(CSIZE|PARENB);
   term_ent.c_cflag |= CS8;
#endif

   term_ent.c_cc[VMIN] = 1;
   term_ent.c_cc[VTIME] = 0;
   
   if (cfsetospeed(&term_ent, speed_code)) {
      ERR("Couldn't set output baud rate %s", strerror(errno));
      m_state = ERRORE;
      return -1;
   }
   if (cfsetispeed(&term_ent, speed_code)) {
      ERR("Couldn't set input baud rate %s", strerror(errno));
      m_state = ERRORE;
      return -1;
   }
   
   if (tcsetattr(m_fd, TCSANOW, &term_ent) < 0) {
      /* Failed. */
      ERR("Could not set terminal attributes %s", strerror(errno));
      m_state = ERRORE;
      return -1;
   }

   if (m_blockingio) {
      int oldfl = fcntl(m_fd, F_GETFL);
      if (oldfl < 0) {
         /* Error */
         ERR("getfl #3 %s", strerror(errno));
         m_state = ERRORE;
         return -1;
      }
      
      if (fcntl(m_fd, F_SETFL, oldfl & ~O_NONBLOCK) < 0) {
         ERR("setfl #3 %s", strerror(errno));
         m_state = ERRORE;
         return -1;
      }
      DBG("open - using blocking io");
   }
   
   /* Worked. */
   /* The port should now be open */
   /* Create lock file */
   char lockfilename[40];
   sprintf(lockfilename, "/var/lock/LCK..ttyS%d", m_portnumber);
   int lock_fd = ::open(lockfilename, O_RDWR|O_CREAT|O_TRUNC, 
                        S_IRUSR | S_IWUSR | S_IRGRP | S_IROTH);
   if ( lock_fd > 0 ) {
      pid_t pid = getpid();
      char pidstring[20];
      sprintf(pidstring, "%u", pid);
      ::write(lock_fd, pidstring, strlen(pidstring));
      //unlink(lockfilename);
   } else {
      ERR("Couldn't create lock file %s : %s", lockfilename, strerror(errno));
   }
   m_state = OPEN_READ_WRITE;
#else
   /* Windows specific code. */
   /* Check if the serial port is already used. */
   if (m_hPort == INVALID_HANDLE_VALUE) {
      /* Set serial port filename. */
      wchar_t szPortName[128];
      wsprintf(&(szPortName[0]), TEXT("COM%u:"), m_portnumber);
      /* Open the serial port. */
      m_hPort = CreateFile (&(szPortName[0]),
            GENERIC_READ | GENERIC_WRITE, // Access (read/write) mode
            0,            // Share mode
            NULL,         // Pointer to the security attribute
            OPEN_EXISTING,// How to open the serial port
            m_fileFlags,  // Port attributes
            NULL);        // Handle to port which to copy attr from.
      if ( m_hPort == INVALID_HANDLE_VALUE )
      {
         uint32 err_val = GetLastError();
         ERR("Could not open serial port: %li", err_val);
         m_state = ERRORE;
         return -1;
      }

      /* Set serial port information */
      DCB PortDCB;
      PortDCB.DCBlength = sizeof (DCB);
      /* Get the default port setting information. */
      if (GetCommState (m_hPort, &PortDCB) == 0) {
         uint32 err_val = GetLastError();
         ERR("WinSerDev::open: GetCommState failed. Reason= %li", err_val);
         m_state = ERRORE;
         return -1;
      }

      m_defaultPortDCB.DCBlength = sizeof(DCB);
      GetCommState(m_hPort, &m_defaultPortDCB);
      /* Change the DCB structure settings. */
      /* Speed in BPS. */
      PortDCB.BaudRate = speed_code;
      /* Binary mode; no EOF check. */
      PortDCB.fBinary = TRUE;
      /* Enable parity checking, but with no parity! See below, Parity. */
      PortDCB.fParity = TRUE;
      /* No CTS output flow control */
      PortDCB.fOutxCtsFlow = FALSE;
      /* No DSR output flow control */
      PortDCB.fOutxDsrFlow = FALSE;
      /* DTR flow control type */
      PortDCB.fDtrControl = DTR_CONTROL_DISABLE;
      /* DSR sensitivity */
      PortDCB.fDsrSensitivity = FALSE;
      /* XOFF continues Tx */
      PortDCB.fTXContinueOnXoff = TRUE;
      /* No XON/XOFF out flow control */
      PortDCB.fOutX = FALSE;
      /* No XON/XOFF in flow control */
      PortDCB.fInX = FALSE;
      /* Disable error replacement (dont replace broken bytes with marker) */
      PortDCB.fErrorChar = FALSE;
      /* Disable null stripping */
      PortDCB.fNull = FALSE;
      /* RTS flow control */
      PortDCB.fRtsControl = RTS_CONTROL_DISABLE;
      /* Do not abort reads/writes on errors. */
      PortDCB.fAbortOnError = FALSE;
      /* Number of bits per byte */
      PortDCB.ByteSize = 8;
      /* Parity scheme (odd, even, no, space, mark) */
      PortDCB.Parity = NOPARITY;
      /* Number of stopbits. */
      PortDCB.StopBits = ONESTOPBIT;

      // Configure the serial port.
      if (!SetCommState (m_hPort, &PortDCB))
      {
         uint32 err_val = GetLastError();
         ERR("WinSerDev::open: SetCommState failed. Reason=%li", err_val);
         m_state = ERRORE;
         return -1;
      }

      COMMTIMEOUTS CommTimeouts;
      /* Retrieve the time-out parameters for all read and */
      /* write operations on the serial port. */
      if (GetCommTimeouts (m_hPort, &CommTimeouts) == 0) {
         uint32 err_val = GetLastError();
         ERR("WinSerDev::open: GetCommTimeouts failed. Reason=%li", err_val);
         m_state = ERRORE;
         return -1;
      }
      /* Change the COMMTIMEOUTS structure settings. */
      CommTimeouts.ReadIntervalTimeout = MAXDWORD;
      CommTimeouts.ReadTotalTimeoutMultiplier = MAXDWORD;
      CommTimeouts.ReadTotalTimeoutConstant = 200;
      CommTimeouts.WriteTotalTimeoutMultiplier = 0;
      CommTimeouts.WriteTotalTimeoutConstant = 0;

      /* Set the time-out parameters for all read and write */
      /* operations on the serial port. */
      if (!SetCommTimeouts (m_hPort, &CommTimeouts)) {
         uint32 err_val = GetLastError();
         ERR("WinSerDev::open: SetCommTimeouts failed. Reason=%li", err_val);
         m_state = ERRORE;
         return -1;
      }  
      EscapeCommFunction(m_hPort, CLRIR);
      EscapeCommFunction(m_hPort, CLRBREAK);
      m_state = OPEN_READ_WRITE;
   } else {
      ERR("WinSerialDevice::open: Invalid serial handle!\n");
      m_state = ERRORE;
      return -1;
   }
#endif
   return 0;
}

int isab::SerialModule::close()
{
   DBG("close()");
   int result = 0;

#ifndef _MSC_VER
   int res = ::close(m_fd);
   m_fd = -1;
   if (res < 0) {
      ERR("close() indicated that an error has occured: %s",
          strerror(errno));
      result = res;
   } else {
      result = 0;
   }
#else
   if (m_state != CLOSED) {
      if (m_hPort != INVALID_HANDLE_VALUE) {
         // try if resetting params will work better.
         SetCommState(m_hPort, &m_defaultPortDCB);
         // Close the communication port.
         if (!CloseHandle (m_hPort)) {
            uint32 err_val = GetLastError ();
            ERR("WinSerialDevice::close: CloseHandle failed. Unable"
                "to close serial port Error=%li", err_val);
            result = -int(err_val);
         } else {
            m_hPort = INVALID_HANDLE_VALUE;
            result = 0;
         }
      }
   } else {
      ERR("WinSerialDevice::close: m_state was already CLOSED");
   }
#endif

   /* Terminate reading thread. */
   delete m_serialthread;
   return result;
}

bool isab::SerialModule::lookupSpeed(int bitrate, speed_t &speed_code)
{
   DBG("lookupSpeed()");
   int i=1; // skip the first (0, B0)
   bool found = false;
   while ( speed_table[i].speed != 0 && found == false ) {
      if ( bitrate == speed_table[i].speed ) {
         speed_code = speed_table[i].speed_code;
         found = true;
      }
      i++;
   }
   if ( found == false ) {
      ERR("Error : invalid speed (%d) for serialDevice", bitrate);
      return false;
   }
   return true;
}

isab::SerialProviderPublic * isab::SerialModule::newPublicSerial()
{
   SerialProviderPublic* spp = new SerialProviderPublic(m_queue);
   return spp;
}

void isab::SerialModule::decodedSendData(int length, const uint8 *data, 
                                         uint32 src)
{
   int pos = 0;

   if (m_state == OPEN_READ_WRITE){
      while(pos < length){
#ifndef _MSC_VER
         pos += write(m_fd, data + pos, length - pos);
#else
         DWORD numWritten;
         if (!WriteFile (m_hPort, data+pos, length-pos, &numWritten, NULL)) {
            uint32 err_val = GetLastError();
            ERR("Writefile failed %li", err_val);
            return;
         }
         pos += uint32(numWritten);
#endif
      }
   } else {
      ERR("decodedSendData: Serial is not open for writing!");
   }
}

void isab::SerialModule::decodedConnectionCtrl(enum ConnectionCtrl ctrl, 
                                               const char *method, 
                                               uint32 src)
{
   switch(ctrl){
   case Module::CONNECT:
      DBG("CONNECT %s", method);
      if(method != NULL){
         char* next;
         uint32 port = strtoul(method, &next, 0);
         uint32 speed = strtoul(next, NULL, 0);
         speed_t test;
         if(lookupSpeed(speed, test)){
            m_speed = speed;
         }
         if(port >=0){
            m_portnumber = port;
         }
         if(port >= 0 || test != B0){
            if(rawRootPublic()){
               rawRootPublic()->connectionNotify(Module::DISCONNECTING);
            }
            close();
            if(rawRootPublic()){
               rawRootPublic()->connectionNotify(Module::CONNECTING);
            }
            m_portnumber = port;
            if (open() > 0){
               m_serialthread = new SerialThread(m_serialConsumer, this);
               m_serialthread->start();
               THREADINFO(*m_serialthread);
               if(rawRootPublic()){
                  rawRootPublic()->connectionNotify(Module::CONNECTED);
               }
            }
         }
      }
   case Module::DISCONNECT:
      if (m_fd >= 0){
         close();
         if(rawRootPublic()){
            rawRootPublic()->connectionNotify(Module::DISCONNECTING);
         }
      }
   case Module::QUERY:
      DBG("QUERY %s", method);
      if(m_fd >= 0){
         if(rawRootPublic()){
            rawRootPublic()->connectionNotify(Module::CONNECTED);
         }
      } else {
         if(rawRootPublic()){
            rawRootPublic()->connectionNotify(Module::CLEAR);
         }
      }
      break;
   default:
      ERR("ConnectionCtrl: Unknown ctrl value: %d", ctrl);
   }

}

isab::SerialConsumerPublic * isab::SerialModule::rootPublic()
{
   return m_serialConsumer;
}


isab::MsgBuffer * isab::SerialModule::dispatch(MsgBuffer *buf)
{
   if(buf) buf = m_providerDecoder.dispatch(buf, this);
   if(buf) buf = Module::dispatch(buf);
   return buf;
}

isab::SerialConsumerPublic * isab::SerialModule::rawRootPublic()
{
   return reinterpret_cast<SerialConsumerPublic*>(m_rawRootPublic);
}
         
int isab::SerialModule::read()
{
   DBG("read()");
   int maxlen = 1971;
   uint8 buf[1971+2];

#ifndef _MSC_VER
   // Need to implement better error handling here.
   int res = ::read(m_fd, buf, maxlen);
   DBG("Read done");
   int er = errno;

   if ( res < 0 && er != EAGAIN )
      return IO_ERROR;
   else {
      if ( er == EAGAIN ) {
         WARN("EAGAIN - %s - "
              "Maybe another program has stolen the serial port. fd = %d",
              strerror(er), m_fd);  
         return 0;
      } else {
         if(rawRootPublic()){
            rawRootPublic()->receiveData(res, buf);
         }
         return res;
      }
   }
#else
   DWORD numBytes = 0;
   /* Read the data from the serial port. */
   if (!ReadFile(m_hPort, buf, maxlen, &numBytes, NULL)) {
      ERR("Readfile failed: GetLastError: %li", GetLastError());
   }

   if (numBytes > 0) {
      /* Read something. */
      if (rawRootPublic()){
         rawRootPublic()->receiveData(numBytes, buf);
      }
   } else {
      /* Error or timeout. */
   }
   return numBytes;
#endif
}
#endif
