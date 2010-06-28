//
//  ES1Renderer.m
//  MapClient
//
//  Created by Dan Nilsson on 2/26/10.
//  Copyright __MyCompanyName__ 2010. All rights reserved.
//

#include "Profiler.h"
#import "ES1Renderer.h"
#include "IPhoneOpenGLESDrawer.h"
#include "WindowEventListener.h"

#include <sys/time.h>

using namespace WFAPI;

@implementation ES1Renderer

@synthesize glesDrawer;
@synthesize windowListener;

// Create an ES 1.1 context
- (id) init
{
	if (self = [super init])
	{
		context = [[EAGLContext alloc] initWithAPI:kEAGLRenderingAPIOpenGLES1];
        
        if (!context || ![EAGLContext setCurrentContext:context])
		{
            [self release];
            return nil;
        }
		
		// Create default framebuffer object. The backing will be allocated for the current layer in -resizeFromLayer
		glGenFramebuffersOES(1, &defaultFramebuffer);
		glGenRenderbuffersOES(1, &colorRenderbuffer);
		glBindFramebufferOES(GL_FRAMEBUFFER_OES, defaultFramebuffer);
		glBindRenderbufferOES(GL_RENDERBUFFER_OES, colorRenderbuffer);
		glFramebufferRenderbufferOES(GL_FRAMEBUFFER_OES,
									 GL_COLOR_ATTACHMENT0_OES, GL_RENDERBUFFER_OES, colorRenderbuffer);
		
		windowListener = NULL;
		
		glesDrawer = new IPhoneOpenGLESDrawer;
		
		glesDrawer->updateContext(colorRenderbuffer,
								  defaultFramebuffer,
								  context);
	}
	
	return self;
}


- (void) render
{  
    // Replace the implementation of this method to do your own custom drawing
	
	// This application only creates a single context which is already set current at this point.
	// This call is redundant, but needed if dealing with multiple contexts.
    // [EAGLContext setCurrentContext:context];
    
	// This application only creates a single default framebuffer which is already bound at this point.
	// This call is redundant, but needed if dealing with multiple framebuffers.
    // glBindFramebufferOES(GL_FRAMEBUFFER_OES, defaultFramebuffer);
    
	// This application only creates a single color renderbuffer which is already bound at this point.
	// This call is redundant, but needed if dealing with multiple
	// renderbuffers.
    // TODO: optimize away
    // glBindRenderbufferOES(GL_RENDERBUFFER_OES, colorRenderbuffer);
	
	if(windowListener) {
		windowListener->onRedraw();       
	}
	
    {
		Prof(presentBuffer);
		[context presentRenderbuffer:GL_RENDERBUFFER_OES];       
    }
}

- (BOOL) resizeFromLayer:(CAEAGLLayer *)layer
{	
	// Allocate color buffer backing based on the current layer size
    glBindRenderbufferOES(GL_RENDERBUFFER_OES, colorRenderbuffer);
    [context renderbufferStorage:GL_RENDERBUFFER_OES fromDrawable:layer];
	glGetRenderbufferParameterivOES(GL_RENDERBUFFER_OES, GL_RENDERBUFFER_WIDTH_OES, &backingWidth);
    glGetRenderbufferParameterivOES(GL_RENDERBUFFER_OES, GL_RENDERBUFFER_HEIGHT_OES, &backingHeight);
	
    if (glCheckFramebufferStatusOES(GL_FRAMEBUFFER_OES) != GL_FRAMEBUFFER_COMPLETE_OES)
	{
		NSLog(@"Failed to make complete framebuffer object %x", glCheckFramebufferStatusOES(GL_FRAMEBUFFER_OES));
        return NO;
    }
	
    if(windowListener) {
		windowListener->onWindowResize(backingWidth, backingHeight);       
    }
    
    return YES;
}

- (void) dealloc
{
	// Tear down GL
	if (defaultFramebuffer)
	{
		glDeleteFramebuffersOES(1, &defaultFramebuffer);
		defaultFramebuffer = 0;
	}
	
	if (colorRenderbuffer)
	{
		glDeleteRenderbuffersOES(1, &colorRenderbuffer);
		colorRenderbuffer = 0;
	}
	
	// Tear down context
	if ([EAGLContext currentContext] == context)
        [EAGLContext setCurrentContext:nil];
	
	[context release];
	context = nil;
	
	[super dealloc];
}

- (WFAPI::DrawingContext*)getDrawingContext {
	if(!glesDrawer) {
		return NULL;
	} else {
		return glesDrawer->getDrawingContext();      
	}
}

- (int) getWidth {
	return backingWidth;
}

- (int) getHeight {
	return backingHeight;
}

- (void) setWindowEventListener:(WindowEventListener*)listener
{
	self->windowListener = listener;
}

@end
