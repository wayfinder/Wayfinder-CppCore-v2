//
//  EAGLView.m
//  MapClient
//
//  Created by Dan Nilsson on 2/26/10.
//  Copyright __MyCompanyName__ 2010. All rights reserved.
//

#import "EAGLView.h"

#import "ES1Renderer.h"
#import "ES2Renderer.h"

#import "MapLibAPI.h"
#import "MapOperationInterface.h"
#import "MapLibKeyInterface.h"
#import "WGS84Coordinate.h"
#import "ScreenPoint.h"
#import "WindowEventListener.h"
#import "MapDrawingCallback.h"
#import "DrawingContext.h"
#import "PointerEvent.h"
#import "Control/ImageMapControl.h"
#import "Control/RegressionTestControl.h"
#include "IPhoneFactory.h"
#include "MapLibNetworkContext.h"
#include "BufferRequester/BufferRequesterSettings.h"
#include "BufferRequester/ResourceDirectorySettings.h"
#include "DummyControl.h"
#include "WindowBridge.h"

#include <sys/time.h>

#define START_IMAGE_MAP_CONTROL
// #define START_REGRESSION_TEST_CONTROL

using namespace WFAPI;

std::string getBundlePath() {
	NSAutoreleasePool *pool = [[NSAutoreleasePool alloc] init]; 
	
	NSString *dirPath = [[NSBundle mainBundle] resourcePath];
	
	const char* pathPtr = [dirPath cString];
	
	std::string ret( pathPtr );
	
	printf("Bundle-path: %s\n", ret.c_str());
	
	[pool release];
	
	return ret;
}

std::vector<PointerEvent::State> getPointerStates(UIView* view, NSSet* touches)
{
	std::vector<PointerEvent::State> ret;
	for (unsigned int i = 0; i < [touches count]; ++i) {
		
		UITouch *touch = [[touches allObjects] objectAtIndex:i];
		CGPoint loc = [touch locationInView:view];
		
		PointerEvent::Type pointerType;
		
		switch ([touch phase]) {
			case UITouchPhaseBegan:
				pointerType = PointerEvent::POINTER_DOWN;
				break;
			case UITouchPhaseMoved:
				pointerType = PointerEvent::POINTER_MOVE;
				break;
			case UITouchPhaseStationary:
				pointerType = PointerEvent::POINTER_STATIONARY;
				break;
			case UITouchPhaseEnded:
			case UITouchPhaseCancelled:
				pointerType = PointerEvent::POINTER_UP;
				break;
		}
		
		struct timeval tv;
		gettimeofday(&tv, NULL);
		unsigned int timestampMillis = (tv.tv_usec/1000 + tv.tv_sec * 1000);
		
		PointerEvent::State s = {0,
			PointerEvent::FINGER,
			pointerType,
			[touch tapCount],
			ScreenPoint(loc.x, loc.y),
			timestampMillis};
		ret.push_back(s);
	}
	
	return ret;
}

@implementation EAGLView

@synthesize animating;
@synthesize initialTouchLocation, startingDistance, lastTouchDistance;	
@synthesize bridge;
@synthesize sampleControl;
@synthesize netContext;

@dynamic animationFrameInterval;

// You must implement this method
+ (Class) layerClass
{
    return [CAEAGLLayer class];
}

//The GL view is stored in the nib file. When it's unarchived it's sent -initWithCoder:
- (id) initWithCoder:(NSCoder*)coder
{    
    if ((self = [super initWithCoder:coder]))
	{
		[self setMultipleTouchEnabled:YES];
        // Get the layer
        CAEAGLLayer *eaglLayer = (CAEAGLLayer *)self.layer;
        
        eaglLayer.opaque = TRUE;
        eaglLayer.drawableProperties = [NSDictionary dictionaryWithObjectsAndKeys:
                                        [NSNumber numberWithBool:FALSE],
										kEAGLDrawablePropertyRetainedBacking,
										kEAGLColorFormatRGBA8,
										kEAGLDrawablePropertyColorFormat, nil];		
		
        renderer = [[ES1Renderer alloc] init];
		
        if (!renderer)
        {
			[self release];
			return nil;
        }
		
		netContext = WFAPI::IPhoneFactory::createHTTPContext();
		
		
		bridge = new WindowBridge;
		bridge->setView(self);
		
#ifdef START_IMAGE_MAP_CONTROL
		BufferRequesterSettings bufReqSettings;
		
		std::string path = getBundlePath() + "/";
		
		bufReqSettings.setResourceDirectorySettings(
													ResourceDirectorySettings(path.c_str(), false));
		
		sampleControl = new ImageMapControl(bridge,
											path,
											bufReqSettings,
											netContext->getConnection(),
											NULL);
		
#elif defined START_REGRESSION_TEST_CONTROL
		std::string path = getBundlePath() + "/IPhoneSuite.txt";
		
		sampleControl = new RegressionTestControl(bridge,
												  NULL,
												  path);
#endif
		
		[renderer setWindowEventListener:sampleControl];
		
		animating = FALSE;
		displayLinkSupported = FALSE;
		animationFrameInterval = 1;
		displayLink = nil;
		animationTimer = nil;
		
		// A system version of 3.1 or greater is required to use CADisplayLink. The NSTimer
		// class is used as fallback when it isn't available.
		NSString *reqSysVer = @"3.1";
		NSString *currSysVer = [[UIDevice currentDevice] systemVersion];
		if ([currSysVer compare:reqSysVer options:NSNumericSearch] != NSOrderedAscending)
			displayLinkSupported = TRUE;
    }
	
    return self;
}

- (void) drawView:(id)sender
{
	[renderer render];
}

- (void) layoutSubviews
{
	[renderer resizeFromLayer:(CAEAGLLayer*)self.layer];
	[self drawView:nil];
}

- (NSInteger) animationFrameInterval
{
	return animationFrameInterval;
}

- (void) setAnimationFrameInterval:(NSInteger)frameInterval
{
	// Frame interval defines how many display frames must pass between each time the
	// display link fires. The display link will only fire 30 times a second when the
	// frame internal is two on a display that refreshes 60 times a second. The default
	// frame interval setting of one will fire 60 times a second when the display refreshes
	// at 60 times a second. A frame interval setting of less than one results in undefined
	// behavior.
	if (frameInterval >= 1)
	{
		animationFrameInterval = frameInterval;
		
		if (animating)
		{
			[self stopAnimation];
			[self startAnimation];
		}
	}
}

- (void) startAnimation
{
	if (!animating)
	{
		if (displayLinkSupported)
		{
			// CADisplayLink is API new to iPhone SDK 3.1. Compiling against
			// earlier versions will result in a warning, but can be dismissed
			// 
			// if the system version runtime check for CADisplayLink exists in -initWithCoder:. The runtime check ensures this code will
			// not be called in system versions earlier than 3.1.
			
			displayLink = [NSClassFromString(@"CADisplayLink") displayLinkWithTarget:self selector:@selector(drawView:)];
			[displayLink setFrameInterval:animationFrameInterval];
			[displayLink addToRunLoop:[NSRunLoop currentRunLoop] forMode:NSDefaultRunLoopMode];
		}
		else
			animationTimer = [NSTimer scheduledTimerWithTimeInterval:(NSTimeInterval)((1.0 / 60.0) * animationFrameInterval) target:self selector:@selector(drawView:) userInfo:nil repeats:TRUE];
		
		animating = TRUE;
	}
}

- (void)stopAnimation
{
	if (animating)
	{
		if (displayLinkSupported)
		{
			[displayLink invalidate];
			displayLink = nil;
		}
		else
		{
			[animationTimer invalidate];
			animationTimer = nil;
		}
		
		animating = FALSE;
	}
}

- (void) dealloc
{
    [renderer release];
	
    [super dealloc];
}

- (void)touchesBegan:(NSSet *)touches withEvent:(UIEvent *)event
{
	PointerEvent p;
	
	p.modStates = getPointerStates(self, touches);
	NSSet* touchesComplete = [event touchesForView:self];
	//   NSLog(@"touchesBegin %d %d touches", [touches count], [touchesComplete
	//                                                            count]);
	p.allStates = getPointerStates(self, touchesComplete);
	
	//NSLog(@"touchesBegan modified %d, all %d", p.modStates.size(), p.allStates.size());
	bridge->getListener()->onPointerDown(p);
}

- (void)touchesMoved:(NSSet *)touches withEvent:(UIEvent *)event
{  
	PointerEvent p;
	
	p.modStates = getPointerStates(self, touches);
	NSSet* touchesComplete = [event touchesForView:self];
	//   NSLog(@"touchesMoved %d %d touches", [touches count], [touchesComplete
	//                                                            count]);
	p.allStates = getPointerStates(self, touchesComplete);
	
	//NSLog(@"touchesMoved modified %d, all %d", p.modStates.size(),
	//       p.allStates.size());
	
	bridge->getListener()->onPointerMove(p);
}


- (int) getWidth {
	return [renderer getWidth];
}

- (int) getHeight {
	return [renderer getHeight];
}

- (WFAPI::DrawingContext*)getDrawingContext {
	return [renderer getDrawingContext];
}

- (void)touchesEnded:(NSSet *)touches withEvent:(UIEvent *)event
{
	PointerEvent p;
	
	p.modStates = getPointerStates(self, touches);
	NSSet* touchesComplete = [event touchesForView:self];
	p.allStates = getPointerStates(self, touchesComplete);
	
	// NSLog(@"touchesEnded modified %d, all %d", p.modStates.size(), p.allStates.size());
	bridge->getListener()->onPointerUp(p);
}

@end
