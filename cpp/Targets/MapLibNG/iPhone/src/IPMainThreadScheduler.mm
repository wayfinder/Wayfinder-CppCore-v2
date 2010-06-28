#include "IPMainThreadScheduler.h"
#import <Foundation/Foundation.h>
#import <UIKit/UIKit.h>

@interface MTSWrapper: NSObject {
   schedFunc_t func;
   void* data;
}
@end

@implementation MTSWrapper

- (void)mtsDelegate {
   func(data);
}

@end

void IPMainThreadScheduler::scheduleOnMainThread(schedFunc_t func, void* data)
{
   MTSWrapper *wrapper = [[MTSWrapper alloc] init];
   wrapper->func = func;
   wrapper->data = data;
      
   [wrapper performSelectorOnMainThread: @selector(mtsDelegate)
            withObject: nil
            waitUntilDone: NO ];

   [wrapper release];   
}

