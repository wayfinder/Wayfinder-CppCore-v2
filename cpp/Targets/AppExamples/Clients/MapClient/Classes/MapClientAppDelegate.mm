//
//  MapClientAppDelegate.m
//  MapClient
//
//  Created by wayfinder on 6/15/10.
//  Copyright __MyCompanyName__ 2010. All rights reserved.
//

#import "MapClientAppDelegate.h"
#import "EAGLView.h"

@implementation MapClientAppDelegate

@synthesize window;
@synthesize glView;

- (BOOL)application:(UIApplication *)application didFinishLaunchingWithOptions:(NSDictionary *)launchOptions
{
    [glView startAnimation];
    return YES;
}

- (void)applicationWillResignActive:(UIApplication *)application
{
    [glView stopAnimation];
}

- (void)applicationDidBecomeActive:(UIApplication *)application
{
    [glView startAnimation];
}

- (void)applicationWillTerminate:(UIApplication *)application
{
    [glView stopAnimation];
}

- (void)dealloc
{
    [window release];
    [glView release];

    [super dealloc];
}

@end
