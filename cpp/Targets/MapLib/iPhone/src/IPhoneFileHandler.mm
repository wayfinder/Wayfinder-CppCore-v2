/*
  Copyright (c) 1999 - 2010, Vodafone Group Services Ltd
  All rights reserved.

  Redistribution and use in source and binary forms, with or without modification, are permitted provided that the following conditions are met:

  * Redistributions of source code must retain the above copyright notice, this list of conditions and the following disclaimer.
  * Redistributions in binary form must reproduce the above copyright notice, this list of conditions and the following disclaimer in the documentation and/or other materials provided with the distribution.
  * Neither the name of the Vodafone Group Services Ltd nor the names of its contributors may be used to endorse or promote products derived from this software without specific prior written permission.

  THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS" AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE ARE DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT HOLDER OR CONTRIBUTORS BE LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
*/
 
#import "IPhoneFileHandler.h"

IPhoneFileHandler::IPhoneFileHandler(const char* fileName, bool createFiles, TileMapToolkit* toolkit)
	: UnixFileHandler(fileName, createFiles),
      m_writeIdle(0),
      m_active(false), 
	  idleData(NULL),
      m_toolkit(toolkit)
{
	assert(m_toolkit != NULL);
}


IPhoneFileHandler::~IPhoneFileHandler() 
{
	if(m_writeIdle) {
		m_toolkit->cancelIdle(this, m_writeIdle);
		m_writeIdle = 0;
	}
}


//read directly
int
IPhoneFileHandler::read(uint8* bytes, int maxLength,
                        FileHandlerListener* listener)
{
	int res = UnixFileHandler::read(bytes, maxLength, NULL);
	if (listener == NULL) {
		return res;
	} 
	else {

		listener->readDone(res);	
	
		return 0;
	}
}


//write on idle
int
IPhoneFileHandler::write(const uint8* bytes, int length,
                       FileHandlerListener* listener)
{
	int res = UnixFileHandler::write(bytes, length, NULL);
	if (listener == NULL) {
		return res;
	} 
	else {
		assert(m_active == false);
		m_active = true;
		
		// create and fill in idle data
		// (data destroyed in idle function)
		idleData = new IdleData_t;
		if (idleData) {
			idleData->result = res;
			idleData->listener = listener;
			
			// Request the idle object
			m_writeIdle = m_toolkit->requestIdle(this);
		}
		else {
			NSLog(@"Error: idleData allocation failed in write()");
		}
		
		return 0;
	}
}


void
IPhoneFileHandler::runIdleTask(uint32 idleId)
{
	//stop idle notifications 
	if (idleId == m_writeIdle) {
		m_toolkit->cancelIdle(this, m_writeIdle);
	}

	assert(m_active == true);
	m_active = false;
		

	if (idleData) {
		FileHandlerListener *fhl = idleData->listener;
		int result = idleData->result;
			
		//release idlData
		delete idleData;
		idleData = NULL;
			
		//announce listener
		if (idleId == m_writeIdle) {
			m_writeIdle = 0;
			fhl->writeDone(result);
		}
	}
	else {
		NSLog(@"Error: idleData is NULL");
	}
}

