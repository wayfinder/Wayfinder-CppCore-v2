/*
  Copyright (c) 1999 - 2010, Vodafone Group Services Ltd
  All rights reserved.

  Redistribution and use in source and binary forms, with or without modification, are permitted provided that the following conditions are met:

  * Redistributions of source code must retain the above copyright notice, this list of conditions and the following disclaimer.
  * Redistributions in binary form must reproduce the above copyright notice, this list of conditions and the following disclaimer in the documentation and/or other materials provided with the distribution.
  * Neither the name of the Vodafone Group Services Ltd nor the names of its contributors may be used to endorse or promote products derived from this software without specific prior written permission.

  THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS" AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE ARE DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT HOLDER OR CONTRIBUTORS BE LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
*/

#include "IPhoneConnectionManager.h"
#define CORE_LOGGING_MODULE_NAME "IPConnMgr"
// #define CORE_LOGGING_MODULE_LEVEL CL_ALL_FILE
#include "CoreDebugPrint.h"

#import <Foundation/Foundation.h>
#import <CoreFoundation/CoreFoundation.h>
#import <SystemConfiguration/SystemConfiguration.h>

#define kReachabilityChangedNotification @"kNetworkReachabilityChangedNotification"

// The following class is a helper class that will subscribe to the system
// events indicating the network status.
@interface NetworkReachability: NSObject
{
   // A reference to the system configuration Reachability reference.
	SCNetworkReachabilityRef reachabilityRef;
   
   // The IPhoneConnectionManager that "owns" this class.
   IPhoneConnectionManager* parent;

   // This bool indicates if the last status was a non-transient connection.
   // (basically means that the last connection was over wlan).
   BOOL hadNonTransientConnection;
}

// Our constructor.
+ (NetworkReachability*) reachabilityWithHostName: (NSString*) hostName withParent:(IPhoneConnectionManager*) parent;

//Start listening for reachability notifications on the current run loop.
- (BOOL) startNotifer;

// Stop listening for notifications.
- (void) stopNotifer;

// Helper function that establishes a connection using a NSURLConnection.
- (void) establishConnection;

// Needed to determine if connection was successfully established.
- (void) connectionDidFinishLoading:(NSURLConnection *)connection;

// Will be called if the connection from establishConnection failed.
- (void) connection:(NSURLConnection *)connection didFailWithError:(NSError*)error;

// Internal selector that will be notified when the reachability has changed.
- (void) reachabilityChanged: (NSNotification* )note;

@end

@implementation NetworkReachability

// Helper that prints the current reachability flags.
static void printReachabilityFlags(SCNetworkReachabilityFlags flags,
                                   const char* comment)
{
   coreprintln("Reachability Flag Status: %c%c %c%c%c%c%c%c%c %s",
			(flags & kSCNetworkReachabilityFlagsIsWWAN)				  ? 'W' : '-',
			(flags & kSCNetworkReachabilityFlagsReachable)            ? 'R' : '-',
			
			(flags & kSCNetworkReachabilityFlagsTransientConnection)  ? 't' : '-',
			(flags & kSCNetworkReachabilityFlagsConnectionRequired)   ? 'c' : '-',
			(flags & kSCNetworkReachabilityFlagsConnectionOnTraffic)  ? 'C' : '-',
			(flags & kSCNetworkReachabilityFlagsInterventionRequired) ? 'i' : '-',
			(flags & kSCNetworkReachabilityFlagsConnectionOnDemand)   ? 'D' : '-',
			(flags & kSCNetworkReachabilityFlagsIsLocalAddress)       ? 'l' : '-',
			(flags & kSCNetworkReachabilityFlagsIsDirect)             ? 'd' : '-',
			comment
      );
}

// Static callback function.
static void ReachabilityCallback(SCNetworkReachabilityRef target,
                                 SCNetworkReachabilityFlags flags, void* info)
{
   printReachabilityFlags(flags, " in ReachabilityCallback");
   
#pragma unused (target)
   
	NSCAssert(info != NULL, @"info was NULL in ReachabilityCallback");
	NSCAssert([(NSObject*) info isKindOfClass: [NetworkReachability class]],
             @"info was wrong class in ReachabilityCallback");

	// in case someone uses the Reachablity object in a different thread.
	NSAutoreleasePool* myPool = [[NSAutoreleasePool alloc] init];
	
	NetworkReachability* noteObject = (NetworkReachability*) info;
   
	// Post a notification to notify the client that the network reachability changed.
	[[NSNotificationCenter defaultCenter] postNotificationName: kReachabilityChangedNotification object: noteObject];
	
	[myPool release];
}

- (void) establishConnection
{
   coreprintln("Trying to establish connection.");
   [[NSURLConnection alloc]
      initWithRequest:
         [NSURLRequest
            requestWithURL: [NSURL URLWithString: @"http://www.apple.com/"] 
            cachePolicy:NSURLRequestReloadIgnoringLocalCacheData
            timeoutInterval:20.0] 
      delegate:self];   
}

- (void)connectionDidFinishLoading:(NSURLConnection *)connection

{
   coreprintln("Connection successfully established.");

   // We have now established a connection, and a reconnection is apropriate.
   parent->reachabilityChanged(IPhoneConnectionManager::ReconnectionNeeded);
   
   // Release the connection.
   [connection release];
}

- (void)connection:(NSURLConnection *)connection
  didFailWithError:(NSError *)error

{
   coreprintln("Error when establing connection");
   // TODO: Notify no connection available?
   
   [connection release];
}


- (void) reachabilityChanged: (NSNotification* )note
{
   SCNetworkReachabilityFlags flags;
   
   if (!SCNetworkReachabilityGetFlags(reachabilityRef, &flags)){
      // If we failed to get the flags, something is wrong!
      coreprintln("Failed to get reachability flags.");
      return;
   }
   
   IPhoneConnectionManager::ReachabilityStatus status;

   // Helper bool to indicate if we have switched from wlan to 3g
   BOOL switchedToTransient =
      (hadNonTransientConnection &&
       (flags & kSCNetworkReachabilityFlagsTransientConnection));

   // Set the member to reflect if we currently are connected to wifi.
   // I.e. non-transient connection.
   hadNonTransientConnection =
      !(flags & kSCNetworkReachabilityFlagsTransientConnection);
   
   if (flags & kSCNetworkReachabilityFlagsConnectionRequired) {
      // A connection needs to be established.
      status = IPhoneConnectionManager::ConnectionNeeded;
      
   } else if (switchedToTransient) {
      // Switched to transient network, a connection needs to be established.
      status = IPhoneConnectionManager::ConnectionNeeded;
      
   } else if (flags & kSCNetworkReachabilityFlagsReachable) {
      // The current status is that the network is reachable and
      // we have not switched to transient. So we assume that we have
      // switched to WLAN. Issuing reconnect to get the faster connection.
      status = IPhoneConnectionManager::ReconnectionNeeded;
   } else {
      // No network available.
      status = IPhoneConnectionManager::NetworkNotAvailable;
   }

   if (parent != NULL){
      parent->reachabilityChanged(status);
   }
}

+ (NetworkReachability*) reachabilityWithHostName: (NSString*) hostName withParent:(IPhoneConnectionManager*) parent;
{
	NetworkReachability* retVal = NULL;
	SCNetworkReachabilityRef reachability =
      SCNetworkReachabilityCreateWithName(NULL, [hostName UTF8String]);
   
	if(reachability!= NULL)
	{
		retVal= [[[self alloc] init] autorelease];
		if(retVal!= NULL)
		{
			retVal->reachabilityRef = reachability;
         retVal->parent = parent;
         retVal->hadNonTransientConnection = NO;
		}
	}
	return retVal;
}

- (BOOL) startNotifer
{
	BOOL retVal = NO;
	SCNetworkReachabilityContext context = {0, self, NULL, NULL, NULL};

   // Observe the kNetworkReachabilityChangedNotification.
   // When that notification is posted, the method "reachabilityChanged" will
   // be called. 
   [[NSNotificationCenter defaultCenter] addObserver: self selector: @selector(reachabilityChanged:) name: kReachabilityChangedNotification object: nil];
   
	if(SCNetworkReachabilitySetCallback(reachabilityRef, ReachabilityCallback,
                                       &context)) {
      
      // Assumes creation is done from the main run loop.
		if(SCNetworkReachabilityScheduleWithRunLoop(reachabilityRef,
                                                  CFRunLoopGetCurrent(),
                                                  kCFRunLoopDefaultMode)) {
			retVal = YES;
		}
	}
   
	return retVal;
}

- (void) stopNotifer
{
	if(reachabilityRef!= NULL) {
      // Must be called from the same run loop... =/
      // TODO: VERIFY THIS!
		SCNetworkReachabilityUnscheduleFromRunLoop(reachabilityRef,
                                                 CFRunLoopGetCurrent(),
                                                 kCFRunLoopDefaultMode);
	}
}

- (void) dealloc
{
	[self stopNotifer];
	if(reachabilityRef!= NULL)
	{
		CFRelease(reachabilityRef);
	}
	[super dealloc];
}

@end // End of NetworkReachability implementation.

struct IPhoneConnectionManager::Impl {
   NetworkReachability* reachability;
};

IPhoneConnectionManager::IPhoneConnectionManager()
{
   m_impl = new Impl;
   m_impl->reachability =
      [[NetworkReachability reachabilityWithHostName:@"www.google.se" withParent:this] retain];
   [m_impl->reachability startNotifer];
}

IPhoneConnectionManager::~IPhoneConnectionManager()
{
   [m_impl->reachability release];
   delete m_impl;
}

void IPhoneConnectionManager::reachabilityChanged(ReachabilityStatus status)
{
   switch (status){
      
   case ConnectionNeeded:
      [m_impl->reachability establishConnection];
      coreprintln("Establishing connection.");
      break;
      
   case NetworkAvailable:
      coreprintln("Network available.");
      notifyReadyForConnection();
      break;
      
   case ReconnectionNeeded:
      coreprintln("Network available, reconnection needed.");
      notifyReconnectionNeeded();
      break;
      
   case NetworkNotAvailable:
      notifyNoConnectionAvailable();
      coreprintln("No network available.");
      break;
   }

}
