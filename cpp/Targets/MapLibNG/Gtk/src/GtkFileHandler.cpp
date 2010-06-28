/*
Copyright (c) 1999 - 2010, Vodafone Group Services Ltd
All rights reserved.

Redistribution and use in source and binary forms, with or without modification, are permitted provided that the following conditions are met:

    * Redistributions of source code must retain the above copyright notice, this list of conditions and the following disclaimer.
    * Redistributions in binary form must reproduce the above copyright notice, this list of conditions and the following disclaimer in the documentation and/or other materials provided with the distribution.
    * Neither the name of the Vodafone Group Services Ltd nor the names of its contributors may be used to endorse or promote products derived from this software without specific prior written permission.

THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS" AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE ARE DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT HOLDER OR CONTRIBUTORS BE LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
*/

#include "GtkFileHandler.h"

/**
 * Idle structure for callbacks
 */
typedef struct GtkFileHandler::IdleData_t {
   int result;
   FileHandlerListener *listener;
   GtkFileHandler *handler;
};

gboolean
GtkFileHandler::on_read_idle( gpointer data )
{
   IdleData_t *idata = static_cast<IdleData_t*>( data );

   MC2_ASSERT( idata->handler->m_active == true );

   idata->handler->m_active = false;
   idata->handler->m_readIdleData = 0;
   idata->handler->m_readIdle = 0;
   idata->listener->readDone( idata->result );

   delete idata; // we dont need it anymore
   
   return 0; // No more calls, please
}

gboolean
GtkFileHandler::on_write_idle( gpointer data ) 
{
   IdleData_t *idata = static_cast<IdleData_t*>( data );

   MC2_ASSERT( idata->handler->m_active == true );
   
   idata->handler->m_active = false;
   idata->handler->m_writeIdleData = 0;
   idata->handler->m_writeIdle = 0;
   idata->listener->writeDone( idata->result );

   delete idata; // we dont need it anymore

   return 0; // No more calls, please
}


GtkFileHandler::GtkFileHandler( const char* fileName,
                                bool createFiles ):
   UnixFileHandler( fileName, createFiles ),
   m_readIdle(0),
   m_readIdleData(0),
   m_writeIdle(0),
   m_writeIdleData(0),
   m_active(false)
{

}

GtkFileHandler::~GtkFileHandler() {
   if (m_readIdle != 0) {
      g_source_remove( m_readIdle );
      delete m_readIdleData;
   }

   if (m_writeIdle != 0){
      g_source_remove( m_writeIdle );
      delete m_writeIdleData;
   }
}

int
GtkFileHandler::read( uint8* bytes,
                      int maxLength,
                      FileHandlerListener* listener )
{
   int res = UnixFileHandler::read( bytes, maxLength, NULL );

   if ( listener == NULL ) {
      return res;
   } else {
      MC2_ASSERT( m_active == false );
      m_active = true;

      // create and fill in idle data
      // (data destroyed in idle function)

      m_readIdleData = new IdleData_t;
      m_readIdleData->result = res;
      m_readIdleData->handler = this;
      m_readIdleData->listener = listener;
      // Request the idle object
      
      m_readIdle = g_idle_add( GtkFileHandler::on_read_idle,
                               m_readIdleData );

      return 0;
   }
}

int
GtkFileHandler::write( const uint8* bytes,
                       int length,
                       FileHandlerListener* listener )
{
   int res = UnixFileHandler::write( bytes, length, NULL );
   if ( listener == NULL ) {
      return res;
   } else {
      MC2_ASSERT( m_active == false );
      m_active = true;

      // create and fill in idle data
      // (data destroyed in idle function)
      m_writeIdleData = new IdleData_t;
      m_writeIdleData->result = res;
      m_writeIdleData->handler = this;
      m_writeIdleData->listener = listener;
      // request idle object
      m_writeIdle = g_idle_add( GtkFileHandler::on_write_idle,
                                m_writeIdleData );

      return 0;
   }
}
