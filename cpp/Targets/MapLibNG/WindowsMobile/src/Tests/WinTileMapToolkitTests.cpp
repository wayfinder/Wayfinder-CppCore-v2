/*
Copyright (c) 1999 - 2010, Vodafone Group Services Ltd
All rights reserved.

Redistribution and use in source and binary forms, with or without modification, are permitted provided that the following conditions are met:

    * Redistributions of source code must retain the above copyright notice, this list of conditions and the following disclaimer.
    * Redistributions in binary form must reproduce the above copyright notice, this list of conditions and the following disclaimer in the documentation and/or other materials provided with the distribution.
    * Neither the name of the Vodafone Group Services Ltd nor the names of its contributors may be used to endorse or promote products derived from this software without specific prior written permission.

THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS" AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE ARE DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT HOLDER OR CONTRIBUTORS BE LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
*/


#include "WinTileMapToolkit.h"
#include "UnitTestFramework.h"
#include "TileMapUtil.h"

#define CORE_LOGGING_MODULE_NAME "WinTileMapToolkitTests"
// #define CORE_LOGGING_MODULE_LEVEL CL_ALL_FILE

#define TIMEOUT_MS 20000

static const LPCTSTR CLASSNAME = _T("NAV2READYTORUNCLASS");

#include "CoreDebugPrint.h"

UT_ADD_TEST_FILE(WinTileMapToolkitTests);

bool g_idleTestDone = false;

#define NUM_IDLE_EXPIRATIONS 129

LRESULT
IdleCallbackWndProc(HWND hwnd,
                    UINT msg,
                    WPARAM wParam,
                    LPARAM lParam)
{
   coreprintln("Main HWND received message: %u", msg);
   
   return true;
}

class IdleTestClass : public TileMapIdleListener {
public:
   IdleTestClass() {
      m_numExpirations = 0;
   }
   
   virtual void runIdleTask(uint32 id) {
      m_numExpirations++;
      
      if(m_numExpirations == NUM_IDLE_EXPIRATIONS) {
         g_idleTestDone = true;         
      }

      assert(m_numExpirations <= NUM_IDLE_EXPIRATIONS);
   }
private:
    unsigned int m_numExpirations;
};

UT_NEW_TEST(WMTestIdleTimer)
{
   coreprintln("TestIdleTimer");

   HINSTANCE appInstance = GetModuleHandle(NULL);
   HWND hwnd;
   
   /* create the window class if required */
   
   /* create the message processing window */
   WNDCLASS wc;

   /* initialize window structure for non-visible window */
   wc.style         = CS_NOCLOSE;
   wc.lpfnWndProc   = IdleCallbackWndProc;
   wc.cbClsExtra    = 0;
   wc.cbWndExtra    = 0;
   wc.hInstance     = appInstance;
   wc.hIcon         = LoadIcon(NULL, IDC_ICON);
   wc.hCursor       = LoadCursor(NULL, IDC_ARROW);
   wc.hbrBackground = (HBRUSH)(COLOR_WINDOW+1);
   wc.lpszMenuName  = NULL;
   wc.lpszClassName = CLASSNAME;

   /* register the window class */
   if(!RegisterClass(&wc)) {
      UT_FAIL("Failed to register class");
   }
   

   /* create the window */
	hwnd = CreateWindowEx(WS_EX_STATICEDGE, 
                           CLASSNAME, _T(""), 
                           WS_DISABLED, 
			                  1, 1, 
                           1, 1, 
                           NULL, NULL, 
                           appInstance, NULL);
   
   /* check for errors */
   if(hwnd == NULL) {
      UT_FAIL("Failed to create window");
	}
   
   IdleTestClass idleTest;
   
   WinTileMapToolkit* toolkit = new WinTileMapToolkit(hwnd, appInstance);

   for(int i = 0; i < NUM_IDLE_EXPIRATIONS; i++) {
      toolkit->requestIdle(&idleTest);      
   }

   MSG msg;
   uint32 startTimeMs = TileMapUtil::currentTimeMillis();
   
   while(!g_idleTestDone) {
      while (PeekMessage( &msg, NULL, 0U, 0U, PM_REMOVE ) ){
         coreprintln("Removed message of type %u from the main message queue",
                     msg.message);
                     
         TranslateMessage( &msg );
         DispatchMessage( &msg );
      }
      
      Sleep(10);
      
      if(TileMapUtil::currentTimeMillis() - startTimeMs > TIMEOUT_MS) {
         coreprintln("Timeout occured..");
         break;
      }
   }

   if(!g_idleTestDone) {
      UT_FAIL("Idle event not received after %u ms!", TIMEOUT_MS);
   }
   
   delete toolkit;
}
