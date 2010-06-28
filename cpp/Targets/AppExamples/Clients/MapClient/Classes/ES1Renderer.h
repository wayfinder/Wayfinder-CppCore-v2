//
//  ES1Renderer.h
//  MapClient
//
//  Created by wayfinder on 6/15/10.
//  Copyright __MyCompanyName__ 2010. All rights reserved.
//

#import "ESRenderer.h"

#import <OpenGLES/ES1/gl.h>
#import <OpenGLES/ES1/glext.h>

namespace WFAPI {
	class IPhoneOpenGLESDrawer;
	class DrawingContext;
}

class WindowEventListener;

@interface ES1Renderer : NSObject <ESRenderer>
{
@private
    EAGLContext *context;

    // The pixel dimensions of the CAEAGLLayer
    GLint backingWidth;
    GLint backingHeight;

    // The OpenGL ES names for the framebuffer and renderbuffer used to render to this view
    GLuint defaultFramebuffer, colorRenderbuffer;
	
	/// The drawer
	WFAPI::IPhoneOpenGLESDrawer* glesDrawer;
	
	/// The listener for window events
	WindowEventListener* windowListener;
}

@property (nonatomic, assign) WFAPI::IPhoneOpenGLESDrawer* glesDrawer;
@property (nonatomic, assign) WindowEventListener* windowListener;

- (void) render;
- (BOOL) resizeFromLayer:(CAEAGLLayer *)layer;
- (WFAPI::DrawingContext*)getDrawingContext;
- (void) setWindowEventListener:(WindowEventListener *)listener;
- (int) getWidth;
- (int) getHeight;


@end
