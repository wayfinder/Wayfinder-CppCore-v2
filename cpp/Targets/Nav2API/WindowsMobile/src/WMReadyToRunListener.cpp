/*
Copyright (c) 1999 - 2010, Vodafone Group Services Ltd
All rights reserved.

Redistribution and use in source and binary forms, with or without modification, are permitted provided that the following conditions are met:

    * Redistributions of source code must retain the above copyright notice, this list of conditions and the following disclaimer.
    * Redistributions in binary form must reproduce the above copyright notice, this list of conditions and the following disclaimer in the documentation and/or other materials provided with the distribution.
    * Neither the name of the Vodafone Group Services Ltd nor the names of its contributors may be used to endorse or promote products derived from this software without specific prior written permission.

THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS" AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE ARE DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT HOLDER OR CONTRIBUTORS BE LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
*/


#include "WMReadyToRunListener.h"

static const LPCTSTR SOCKCLASSNAME = _T("NAV2READYTORUNCLASS");

#define WM_RTR_RUN (WM_USER + 4000) 

bool classExists(LPCTSTR className,HINSTANCE appInstance)
{
   /* use GetClassInfoEx to see if the class exists */
   WNDCLASS wnd;
   int retCode = GetClassInfo(appInstance, 
                              className,
                              &wnd);
   /* if return code is zero, the class does no exist; return false */
   if(retCode == 0) {
      return(false);
   }

   /* class found, return true */
   return(true);
}


LRESULT
WMReadyToRunListener::CallbackWndProc(HWND hwnd, UINT msg, WPARAM wParam, LPARAM lParam)
{
   if(msg == WM_RTR_RUN) {
      WMReadyToRunListener* pclass =
         reinterpret_cast<WMReadyToRunListener*>(lParam);
      pclass->doCallbacks();
   }
   
   return 0;
}

WMReadyToRunListener::WMReadyToRunListener()
{
   m_appInstance = 0;
}

bool WMReadyToRunListener::init(HINSTANCE hInst) {
   m_appInstance = hInst;
   
   /* create the window class if required */
   if(!classExists(SOCKCLASSNAME, m_appInstance)) {
      /* create the message processing window */
	   WNDCLASS wc;

	   /* initialize window structure for non-visible window */
	   wc.style         = CS_NOCLOSE;
	   wc.lpfnWndProc   = WMReadyToRunListener::CallbackWndProc;
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
		   return false;
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
   
   return true;
}

void WMReadyToRunListener::ready()
{
   PostMessage(m_hwnd, WM_RTR_RUN, 0, reinterpret_cast<LPARAM>(this));
}
