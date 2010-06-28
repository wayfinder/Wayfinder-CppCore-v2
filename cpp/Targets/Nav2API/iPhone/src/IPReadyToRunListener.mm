/*
  Copyright (c) 1999 - 2010, Vodafone Group Services Ltd
  All rights reserved.

  Redistribution and use in source and binary forms, with or without modification, are permitted provided that the following conditions are met:

  * Redistributions of source code must retain the above copyright notice, this list of conditions and the following disclaimer.
  * Redistributions in binary form must reproduce the above copyright notice, this list of conditions and the following disclaimer in the documentation and/or other materials provided with the distribution.
  * Neither the name of the Vodafone Group Services Ltd nor the names of its contributors may be used to endorse or promote products derived from this software without specific prior written permission.

  THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS" AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE ARE DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT HOLDER OR CONTRIBUTORS BE LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
*/


#define CORE_LOGGING_MODULE_NAME "IPReadyToRunListener"
// #define CORE_LOGGING_ENABLE_MODULE

#include "IPReadyToRunListener.h"
#include "CoreDebugPrint.h"
#include "Mutex.h"
#include "WFAPISync.h"
#import <Foundation/Foundation.h>
#import <UIKit/UIKit.h>

using namespace WFAPI;

@interface RTRWrapper: NSObject {
   IPReadyToRunListener* obj;
}
@end

@implementation RTRWrapper

- (void)rtrDelegate {
   obj->doCallbacks();
}

@end


IPReadyToRunListener::IPReadyToRunListener()
{
}

IPReadyToRunListener::~IPReadyToRunListener()
{
}

void IPReadyToRunListener::ready()
{
   RTRWrapper *wrapper = [[RTRWrapper alloc] init];
   wrapper->obj = this;
      
   [wrapper performSelectorOnMainThread: @selector(rtrDelegate)
            withObject: nil
            waitUntilDone: NO ];

   [wrapper release];
}

