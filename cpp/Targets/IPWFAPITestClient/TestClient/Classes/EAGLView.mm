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
#import "WGS84Coordinate.h"
#import "ScreenPoint.h"
#import "ConfigInterface.h"

using namespace WFAPI;

#define USE_DEPTH_BUFFER 0

// A class extension to declare private methods
@interface EAGLView ()

@property (nonatomic, retain) EAGLContext *context;
float curStep;
float curInc;
NSTimer *timer;

- (BOOL) createFramebuffer;
- (void) destroyFramebuffer;
- (void) smooth3d;
@end

@implementation EAGLView

@synthesize context;
@synthesize wfSession;
@synthesize initialTouchLocation, startingDistance, lastTouchDistance;	
@synthesize label;
@synthesize initialAngle;
@synthesize initialTransform;
@synthesize angleDiff;

// You must implement this method
+ (Class)layerClass {
    return [CAEAGLLayer class];
}

- (void) smooth3d {
   curStep += curInc;

   if ( curStep > 1.0f){
      curStep = 1.0f;
      curInc = -0.01f;
      wfSession->getMapLib()->getMapOperationInterface()->setMovementMode(false);
      [timer invalidate];
      timer = nil;
   } else if (curStep < 0.0f){
      curStep = 0.0f;
      curInc = 0.01f;
      wfSession->getMapLib()->getMapOperationInterface()->setMovementMode(false);
      [timer invalidate];
      timer = nil;
   }
   wfSession->getMapLib()->getConfigInterface()->setVariable3dMode(curStep);

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
        curInc = -0.01f;
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
	MapLibAPI* mapLib =  wfSession->getMapLib();
	if (mapLib != NULL) {
		MapOperationInterface* operationInterface = mapLib->getMapOperationInterface();
		operationInterface->setMovementMode(true);
    }
		if ([touches count] == 3) {
        NSArray *twoTouches = [touches allObjects];
        UITouch *t1 = [twoTouches objectAtIndex:0];
        UITouch *t2 = [twoTouches objectAtIndex:1];
		UITouch *t3 = [twoTouches objectAtIndex:2];
        self.initialAngle = angleBetweenPoints([t1 locationInView:self], [t3 locationInView:self]);
        self.initialTransform  = label.transform;
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
         mapLib->getMapOperationInterface()->
            move(prevLoc.x - loc.x, prevLoc.y - loc.y);
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
    else if ([touches count] == 3) {
		
		// Get the MapLib from the wfSession.
		MapLibAPI* mapLib =  wfSession->getMapLib();
		if (mapLib != NULL) {
			// Get the touches for both fingers.
			UITouch *t1 = [[touches allObjects] objectAtIndex:0];
			UITouch *t2 = [[touches allObjects] objectAtIndex:1];
			UITouch *t3 = [[touches allObjects] objectAtIndex:2];
			
			// Get the scrreen positions for the touch points
			CGPoint loc1 = [t1 locationInView:self];
			CGPoint loc2 = [t2 locationInView:self];
			CGPoint loc3 = [t3 locationInView:self];
			
			// Get the screen points for the previous touch points
			CGPoint prevLoc1 = [t1 previousLocationInView:self];
			CGPoint prevLoc2 = [t2 previousLocationInView:self];        
			CGPoint prevLoc3 = [t3 previousLocationInView:self];        
			
			//move and zoom t1 versus t2
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
			ScreenPoint centerPt1(center.x, center.y);
			operationInterface->screenToWorld(centerCoord, centerPt1);
			
			// Zoom, in or out depends on the result of lastDist / dist
			operationInterface->zoom(
									 lastDist / dist, centerCoord, centerPt1);
			
			
			
			//move and zoom t1 versus t3
			
			// Zoom center
			center = CGPointMake(
										 (loc1.x + loc3.x) / 2, (loc1.y + loc3.y) / 2);
			prevCenter = CGPointMake(
											 (prevLoc1.x + prevLoc3.x) / 2, (prevLoc1.y + prevLoc3.y) / 2);
			
			// Zoom amount
			lastDist = hypot(prevLoc1.x - prevLoc3.x,
									 prevLoc1.y - prevLoc3.y);
			dist = hypot(loc1.x - loc3.x, loc1.y - loc3.y);
			
			lastTouchDistance += fabs(lastDist - dist);
			
			// Move the map to keep the center
			operationInterface->move(prevCenter.x - center.x,
									 prevCenter.y - center.y);
			
			// Convert the center screen point to world coordinates
			ScreenPoint centerPt(center.x, center.y);
			operationInterface->screenToWorld(centerCoord, centerPt);
			
			// Zoom, in or out depends on the result of lastDist / dist
			operationInterface->zoom(
									 lastDist / dist, centerCoord, centerPt);

			
			
			

			//rotation
			//NSArray *twoTouches = [touches allObjects];
			//UITouch *first = [twoTouches objectAtIndex:0];
			//UITouch *second = [twoTouches objectAtIndex:2];
			CGFloat currentAngle = angleBetweenPoints([t1 locationInView:self], [t3 locationInView:self]);
			//NSLog(@"currentAngle: %f", currentAngle);
			if (self.initialAngle == 0.0)
			{
				self.initialAngle = currentAngle;
				self.initialTransform = label.transform;
			}
			else
			{
				//CGFloat tmpAngleDiff = angleDiff;
				angleDiff = self.initialAngle - currentAngle;
				//NSLog(@"initialAngle: %f, currentAngle: %f, tmpAngleDiff: %f, angleDiff: %f, angleDiffDegrees %f, getAngle: %f, setAngle: %f", initialAngle*180/3.14, currentAngle*180/3.14, angleDiff*180/3.14, angleDiff*180/3.14, angleDiff*180/3.14, operationInterface->getAngle(), operationInterface->getAngle()-angleDiff*180/3.14);
				CGAffineTransform newTransform = CGAffineTransformRotate(initialTransform, angleDiff);
				label.transform = newTransform;
				
				operationInterface->setAngle(operationInterface->getAngle()-angleDiff*180/3.14, centerPt);
				initialAngle = currentAngle;
			}	   
			
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
   
   MapLibAPI* mapLib = wfSession->getMapLib();


   if ([touches count] == 3) {
      if (timer != nil){
         [timer invalidate];
      }
      mapLib->getMapOperationInterface()->setMovementMode(true);
      timer = [NSTimer timerWithTimeInterval:0.01
                       target:self
                       selector:@selector(smooth3d)
                       userInfo:nil
                       repeats:YES];

      [[NSRunLoop currentRunLoop] addTimer:timer forMode:NSDefaultRunLoopMode];      
   }

   
   lastTouchDistance = 0;
   self.initialAngle = 0.0;
   angleDiff = 0;

	if (mapLib != NULL) {
		MapOperationInterface* operationInterface = mapLib->getMapOperationInterface();
		operationInterface->setMovementMode(false);
    }
	
}

@end
