/*
  Copyright (c) 1999 - 2010, Vodafone Group Services Ltd
  All rights reserved.

  Redistribution and use in source and binary forms, with or without modification, are permitted provided that the following conditions are met:

  * Redistributions of source code must retain the above copyright notice, this list of conditions and the following disclaimer.
  * Redistributions in binary form must reproduce the above copyright notice, this list of conditions and the following disclaimer in the documentation and/or other materials provided with the distribution.
  * Neither the name of the Vodafone Group Services Ltd nor the names of its contributors may be used to endorse or promote products derived from this software without specific prior written permission.

  THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS" AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE ARE DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT HOLDER OR CONTRIBUTORS BE LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
*/

#include <stdio.h>
#import "IPhoneOpenGLESDrawer.h"
#import <QuartzCore/QuartzCore.h>
#import <OpenGLES/EAGLDrawable.h>

#import "EAGLView.h"
#import "IPWFSession.h"
#import "MapLibAPI.h"
#import "MapOperationInterface.h"
#import "MapLibKeyInterface.h"
#import "WGS84Coordinate.h"
#import "ScreenPoint.h"

using namespace WFAPI;

#define USE_DEPTH_BUFFER 0

// A class extension to declare private methods
@interface EAGLView ()

@property (nonatomic, retain) EAGLContext *context;

- (BOOL) createFramebuffer;
- (void) destroyFramebuffer;

@end

@implementation EAGLView

@synthesize context;
@synthesize wfSession;
@synthesize initialTouchLocation, startingDistance, lastTouchDistance;	

// You must implement this method
+ (Class)layerClass {
    return [CAEAGLLayer class];
}


//The GL view is stored in the nib file.
//When it's unarchived it's sent -initWithCoder:
- (id)initWithCoder:(NSCoder*)coder {
    
    if ((self = [super initWithCoder:coder])) {

       [self setMultipleTouchEnabled:YES];
       
        // Get the layer
        CAEAGLLayer *eaglLayer = (CAEAGLLayer *)self.layer;
        
        eaglLayer.opaque = YES;
        eaglLayer.drawableProperties =
           [NSDictionary dictionaryWithObjectsAndKeys:
                             [NSNumber numberWithBool:NO],
                         kEAGLDrawablePropertyRetainedBacking,
                         kEAGLColorFormatRGBA8,
                         kEAGLDrawablePropertyColorFormat, nil];
        
        context =
           [[EAGLContext alloc] initWithAPI:kEAGLRenderingAPIOpenGLES1];
        
        if (!context || ![EAGLContext setCurrentContext:context]) {
            [self release];
            return nil;
        }

        // Once the view has been unarchived we can create and initialize
        // our WF session
        wfSession = new IPWFSession(self);
        wfSession->init();
    }
    return self;
}


-(void)drawRect:(CGRect)rect {   

   // This function is called whenever the window needs to be repainted.
   // MapLib will invalidate the window when it has drawn a map, triggering
   // a repaint. If the application writer wants to draw any OpenGL ES
   // content on top of the map, this is the place to do it.

   // Add custom drawing here.
   
   // When we are ready with our custom drawing (if any) we need to swap
   // the buffer. The buffer to swap needs to be the same as the one supplied
   // to IPWFSession.
   glBindRenderbufferOES(GL_RENDERBUFFER_OES, viewRenderbuffer);
   [context presentRenderbuffer:GL_RENDERBUFFER_OES];
}

- (void)drawView {
   [self drawRect:[self bounds]];
}

- (void)layoutSubviews {
    [EAGLContext setCurrentContext:context];
    [self destroyFramebuffer];
    [self createFramebuffer];
    [self drawView];
}


- (BOOL)createFramebuffer {
    
    glGenFramebuffersOES(1, &viewFramebuffer);
    glGenRenderbuffersOES(1, &viewRenderbuffer);
    
    glBindFramebufferOES(GL_FRAMEBUFFER_OES, viewFramebuffer);
    glBindRenderbufferOES(GL_RENDERBUFFER_OES, viewRenderbuffer);
    [context renderbufferStorage:GL_RENDERBUFFER_OES fromDrawable:(CAEAGLLayer*)self.layer];
    glFramebufferRenderbufferOES(GL_FRAMEBUFFER_OES, GL_COLOR_ATTACHMENT0_OES, GL_RENDERBUFFER_OES, viewRenderbuffer);
    
    glGetRenderbufferParameterivOES(GL_RENDERBUFFER_OES, GL_RENDERBUFFER_WIDTH_OES, &backingWidth);
    glGetRenderbufferParameterivOES(GL_RENDERBUFFER_OES, GL_RENDERBUFFER_HEIGHT_OES, &backingHeight);
    
    if (USE_DEPTH_BUFFER) {
        glGenRenderbuffersOES(1, &depthRenderbuffer);
        glBindRenderbufferOES(GL_RENDERBUFFER_OES, depthRenderbuffer);
        glRenderbufferStorageOES(GL_RENDERBUFFER_OES, GL_DEPTH_COMPONENT16_OES, backingWidth, backingHeight);
        glFramebufferRenderbufferOES(GL_FRAMEBUFFER_OES, GL_DEPTH_ATTACHMENT_OES, GL_RENDERBUFFER_OES, depthRenderbuffer);
    }
    
    if(glCheckFramebufferStatusOES(GL_FRAMEBUFFER_OES) != GL_FRAMEBUFFER_COMPLETE_OES) {
        NSLog(@"failed to make complete framebuffer object %x", glCheckFramebufferStatusOES(GL_FRAMEBUFFER_OES));
        return NO;
    }

    self.wfSession->initMapDrawer(viewRenderbuffer, viewFramebuffer, context);


    // Once we have created a frame buffer, we clear the screen so that we
    // do not see any old content by mistake.
    [EAGLContext setCurrentContext:context];
    
    glBindFramebufferOES(GL_FRAMEBUFFER_OES, viewFramebuffer);
    glViewport(0, 0, backingWidth, backingHeight);

    glClearColor( 0.0f, 0.0f, 0.0f, 1.0f );
    glClear (GL_COLOR_BUFFER_BIT);
    glBindRenderbufferOES(GL_RENDERBUFFER_OES, viewRenderbuffer);
    
    [context presentRenderbuffer:GL_RENDERBUFFER_OES];
    
    return YES;
}


- (void)destroyFramebuffer {
    
    glDeleteFramebuffersOES(1, &viewFramebuffer);
    viewFramebuffer = 0;
    glDeleteRenderbuffersOES(1, &viewRenderbuffer);
    viewRenderbuffer = 0;
    
    if(depthRenderbuffer) {
        glDeleteRenderbuffersOES(1, &depthRenderbuffer);
        depthRenderbuffer = 0;
    }
}


- (void)dealloc {
    
    if ([EAGLContext currentContext] == context) {
        [EAGLContext setCurrentContext:nil];
    }
    
    [context release];
    delete wfSession;
    [super dealloc];
}

- (void)touchesBegan:(NSSet *)touches withEvent:(UIEvent *)event
{
	if ([touches count] == 1) {
      UITouch *touch = [[touches allObjects] objectAtIndex:0];
      CGPoint loc = [touch locationInView:self];
      MapLibAPI* mapLib =  wfSession->getMapLib();
      if (mapLib != NULL) {
         MapLibKeyInterface* keyInterface =
            mapLib->getKeyInterface();
         keyInterface->handlePointerEvent(
            MapLibKeyInterface::DRAG_TO,
            MapLibKeyInterface::POINTER_DOWN_EVENT,
            ScreenPoint(loc.x, loc.y));
      }
   }
}


- (void)touchesMoved:(NSSet *)touches withEvent:(UIEvent *)event
{
	//get position, update the map 
	if ([touches count] == 1) {
      UITouch *touch = [[touches allObjects] objectAtIndex:0];
      CGPoint loc = [touch locationInView:self];
      CGPoint prevLoc = [touch previousLocationInView:self];

      MapLibAPI* mapLib =  wfSession->getMapLib();
      if (mapLib != NULL) {
         MapLibKeyInterface* keyInterface =
            mapLib->getKeyInterface();
         keyInterface->handlePointerEvent(
            MapLibKeyInterface::DRAG_TO,
            MapLibKeyInterface::POINTER_UPDATE_EVENT,
            ScreenPoint(loc.x, loc.y));
         
         // mapLib->getMapOperationInterface()->
         //    move(prevLoc.x - loc.x, prevLoc.y - loc.y);
      }
   } else if ([touches count] == 2) {
      // Get the MapLib from the wfSession.
      MapLibAPI* mapLib =  wfSession->getMapLib();
      if (mapLib != NULL) {
         // Get the touches for both fingers.
         UITouch *t1 = [[touches allObjects] objectAtIndex:0];
         UITouch *t2 = [[touches allObjects] objectAtIndex:1];

         // Get the scrreen positions for the touch points
         CGPoint loc1 = [t1 locationInView:self];
         CGPoint loc2 = [t2 locationInView:self];

         // Get the screen points for the previous touch points
         CGPoint prevLoc1 = [t1 previousLocationInView:self];
         CGPoint prevLoc2 = [t2 previousLocationInView:self];        
        
         // Zoom center
         CGPoint center = CGPointMake(
            (loc1.x + loc2.x) / 2, (loc1.y + loc2.y) / 2);
         CGPoint prevCenter = CGPointMake(
            (prevLoc1.x + prevLoc2.x) / 2, (prevLoc1.y + prevLoc2.y) / 2);

         // Zoom amount
         CGFloat lastDist = hypot(prevLoc1.x - prevLoc2.x,
                                  prevLoc1.y - prevLoc2.y);
         CGFloat dist = hypot(loc1.x - loc2.x, loc1.y - loc2.y);

         lastTouchDistance += fabs(lastDist - dist);

         // Move the map to keep the center
         MapOperationInterface* operationInterface =
            mapLib->getMapOperationInterface();
         operationInterface->move(prevCenter.x - center.x,
                                  prevCenter.y - center.y);
         
         // Convert the center screen point to world coordinates
         WGS84Coordinate centerCoord;
         ScreenPoint centerPt(center.x, center.y);
         operationInterface->screenToWorld(centerCoord, centerPt);

         // Zoom, in or out depends on the result of lastDist / dist
         operationInterface->zoom(
            lastDist / dist, centerCoord, centerPt);
		}
	}
}


- (void)touchesEnded:(NSSet *)touches withEvent:(UIEvent *)event
{
   if ([touches count] == [[event touchesForView:self] count]) 
   {
      // last finger has lifted....
      // TODO: Add tap and double tap logic here
   }

   
   MapLibAPI* mapLib =  wfSession->getMapLib();
   UITouch *touch = [[touches allObjects] objectAtIndex:0];
   CGPoint loc = [touch locationInView:self];
   if (mapLib != NULL) {
      MapLibKeyInterface* keyInterface =
         mapLib->getKeyInterface();
      keyInterface->handlePointerEvent(
         MapLibKeyInterface::DRAG_TO,
         MapLibKeyInterface::POINTER_UP_EVENT,
         ScreenPoint(loc.x, loc.y));
   }

}

@end
