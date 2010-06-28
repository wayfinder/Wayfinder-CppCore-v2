//
//  EAGLView.h
//  MapClient
//
//  Created by Dan Nilsson on 2/26/10.
//  Copyright __MyCompanyName__ 2010. All rights reserved.
//

#import <UIKit/UIKit.h>
#import <QuartzCore/QuartzCore.h>

#import "ESRenderer.h"

class WindowEventListener;
class MapDrawCallbackWrapper;
class WindowBridge;

namespace WFAPI {
	class MapLibNetworkContext;
	class DrawingContext;
}

// This class wraps the CAEAGLLayer from CoreAnimation into a convenient UIView subclass.
// The view content is basically an EAGL surface you render your OpenGL scene into.
// Note that setting the view non-opaque will only work if the EAGL surface has an alpha channel.
@interface EAGLView : UIView
{    
@private
	id <ESRenderer> renderer;
	
	BOOL animating;
	BOOL displayLinkSupported;
	NSInteger animationFrameInterval;
	// Use of the CADisplayLink class is the preferred method for controlling your animation timing.
	// CADisplayLink will link to the main display and fire every vsync when added to a given run-loop.
	// The NSTimer class is used only as fallback when running on a pre 3.1 device where CADisplayLink
	// isn't available.
	id displayLink;
    NSTimer *animationTimer;
	
    /// The bridge between EAGLView and AppWindow
    WindowBridge* bridge;
	
    /// The listener for window events
    WindowEventListener* sampleControl;
	
    WFAPI::MapLibNetworkContext* netContext;
	
    /// Keeps track of the distance when pinching
    CGFloat lastTouchDistance;
	
    /* The point where the touch event started. */
    CGPoint initialTouchLocation;
	
    /* If the initial touch event occured using two fingers, this */
    /* is the distance in pixels between them. */
    CGFloat startingDistance;
}

@property (nonatomic, assign) CGPoint initialTouchLocation;
@property (nonatomic, assign) CGFloat startingDistance;
@property (nonatomic, assign) CGFloat lastTouchDistance;

@property (readonly, nonatomic, getter=isAnimating) BOOL animating;
@property (nonatomic) NSInteger animationFrameInterval;

- (void) startAnimation;
- (void) stopAnimation;
- (void) drawView:(id)sender;

@property (nonatomic, assign) WindowBridge* bridge;
@property (nonatomic, assign) WindowEventListener* sampleControl;
@property (nonatomic, assign) WFAPI::MapLibNetworkContext* netContext;

- (int) getWidth;
- (int) getHeight;
- (WFAPI::DrawingContext*)getDrawingContext;

@end
