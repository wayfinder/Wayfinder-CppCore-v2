/*
  Copyright (c) 1999 - 2010, Vodafone Group Services Ltd
  All rights reserved.

  Redistribution and use in source and binary forms, with or without modification, are permitted provided that the following conditions are met:

  * Redistributions of source code must retain the above copyright notice, this list of conditions and the following disclaimer.
  * Redistributions in binary form must reproduce the above copyright notice, this list of conditions and the following disclaimer in the documentation and/or other materials provided with the distribution.
  * Neither the name of the Vodafone Group Services Ltd nor the names of its contributors may be used to endorse or promote products derived from this software without specific prior written permission.

  THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS" AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE ARE DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT HOLDER OR CONTRIBUTORS BE LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
*/

#define CORE_LOGGING_MODULE_NAME "IPhoneLocationUtil"
//#define CORE_LOGGING_MODULE_LEVEL CL_ALL_FILE
//#define CORE_LOGGING_MODULE_LEVEL CL_ALL

//XXX: dont use READ_POS_FROM_DISK and CL_ALL_FILE at the same time!
//either you write to the log file, or you read from the log file, not both
//#define READ_POS_FROM_DISK
#define BUF_SIZE 1024

#include "CoreDebugPrint.h"
#include "IPLocationInterfaceImpl.h"
#include "IPLBSListener.h"
#include <string>
#import <Foundation/Foundation.h>
#import <UIKit/UIKit.h>
#import <CoreLocation/CoreLocation.h>
#import	"IPhoneLocationUtil.h"
#import  "IPhonePathUtil.h"
#include <stdio.h>
#include <vector>


namespace WFAPI {
   class IPLBSListener;
}

//since we want to hide the objective c stuff we put everything 
//here instead of having some of it in a header file
@interface MyCLController : NSObject <CLLocationManagerDelegate> {
   @public
   CLLocationManager *locationManager;
   @private
   std::vector<WFAPI::IPLBSListener*> m_listeners;

   /// A member holding the file we use to read positions from disk, if enabled
   FILE* m_positions_file;
}

@property (nonatomic, retain) CLLocationManager *locationManager;

- (void)locationManager:(CLLocationManager *)manager
    didUpdateToLocation:(CLLocation *)newLocation
           fromLocation:(CLLocation *)oldLocation;

- (void)locationManager:(CLLocationManager *)manager
       didFailWithError:(NSError *)error;

- (void)addListener:(WFAPI::IPLBSListener *)listener;

@end

@implementation MyCLController

@synthesize locationManager;

- (id) init {
    self = [super init];
    if (self != nil) {
        self.locationManager = [[[CLLocationManager alloc] init] autorelease];
        self.locationManager.delegate = self; // send loc updates to myself

#ifdef READ_POS_FROM_DISK
        char fileBuf[BUF_SIZE];
        sprintf(fileBuf, "%s/CORE_LOG_ALL.txt", IPhonePathUtil::getDocumentsPath().c_str());
        m_positions_file = fopen(fileBuf, "r");
#endif
    }
    return self;
}

- (void)locationManager:(CLLocationManager *)manager
    didUpdateToLocation:(CLLocation *)newLocation
           fromLocation:(CLLocation *)oldLocation
{
   //NSLog(@"Location: %@", [newLocation description]);

   //std::string docPath = IPhonePathUtil::getDocumentsPath();
   double latitude = newLocation.coordinate.latitude;
   double longitude = newLocation.coordinate.longitude;
   double altitude = newLocation.altitude;
   double speed = newLocation.speed;
   double track = newLocation.course;

   //XXX: don't confuse pdop with horizontalAccuracy/verticalAccuracy
   // verticalAccuracy gives an altitude uncertanty in meters,
   // and pdop can roughly be defined as a ratio of 3D uncertanty.
   double verticalAccuracy = newLocation.verticalAccuracy;       
   double horizontalAccuracy = newLocation.horizontalAccuracy;

   double timestamp = [newLocation.timestamp timeIntervalSince1970];

#ifdef CORE_LOGGING_MODULE_LEVEL
   //If logging is enabled, we output the positions that the
   //CLLocationManager gives us, and in some cases this will be
   //written to a log file on disk.
   NSDateFormatter* formatter = [[[NSDateFormatter alloc] init] autorelease];
   [formatter setDateFormat:@"yyyy-MM-dd HH:mm:ss"];
   NSString* str = [formatter stringFromDate:newLocation.timestamp];
   coreprintln("date: %s latitude=%f, longitude=%f, altitude=%f, speed=%f, track=%f, horizontalAccuracy=%f, verticalAccuracy=%f, timestamp=%f",
               [str cStringUsingEncoding:NSUTF8StringEncoding], latitude,
               longitude, altitude, speed, track, horizontalAccuracy,
               verticalAccuracy, timestamp);
#endif
   
#ifdef READ_POS_FROM_DISK
   //we want to read positions stored in a log file on the disk
   //this block reads a log in the format exactly as the above coreprintln
   //and tries to read lines until it finds one it can parse.
   //when it reaches the end of the file it gives up and real positions from
   //CLLocationManager is reported instead.
   //Also note that this block relies on CLLocationManager reporting positions
   if ( m_positions_file != NULL ) {
      char line[BUF_SIZE]; //buffer holding each line read from file
      int sscanfret = 0; // return value from the sscanf, which reads a line
   
      while (sscanfret <= 0) { //read from file until we get a line that matches our search critera
         if( fgets ( line, sizeof line, m_positions_file ) != NULL ) { //read one line

            sscanfret = sscanf(line, "%*s %*s %*s %*u-%*u-%*u %*u:%*u:%*u latitude=%lf, longitude=%lf, altitude=%lf, speed=%lf, track=%lf, horizontalAccuracy=%lf, verticalAccuracy=%lf, timestamp=%lf",
                               &latitude, &longitude, &altitude, &speed, &track,
                               &horizontalAccuracy, &verticalAccuracy,
                               &timestamp);
         }
         else {
            coreprintln("Reached end of positions file");
            fclose ( m_positions_file );
            m_positions_file = NULL;
            break; //stop going through the while loop
         }
      }
   }
   else {
      coreprintln("filename CORE_LOG_ALL.txt didn't seem to open correctly, skipping reading of stored positions");
   }

   coreprintln("position read from file: latitude: %f, longitude: %f, altitude: %f, speed: %f, track: %f, horizontalAccuracy: %f, verticalAccuracy: %f, timestamp: %f",
               latitude, longitude, altitude, speed, track, horizontalAccuracy,
               verticalAccuracy, timestamp);

#endif

   //now report positions either from CLLocationManager or read from disk
   for(std::vector<WFAPI::IPLBSListener*>::iterator itr = m_listeners.begin();
       itr != m_listeners.end(); itr++)
   {
       //assume our listener has implemented lbsCallback(...);
       (*itr)->lbsCallback(latitude, longitude, altitude, speed, track,
                           horizontalAccuracy, verticalAccuracy, timestamp);
    }
}

- (void)locationManager:(CLLocationManager *)manager
           didFailWithError:(NSError *)error
{
	NSLog(@"Error: %@", [error description]);
   for(std::vector<WFAPI::IPLBSListener*>::iterator itr = m_listeners.begin();
       itr != m_listeners.end(); itr++)
   {
      //assume our listener has implemented lbsFailed();
      //lbsFailed could have accepted the error message as an argument if we
      //wanted to.
      (*itr)->lbsFailed();
   }
}

- (void)addListener:(WFAPI::IPLBSListener*)listener
{
   m_listeners.push_back(listener);
}

- (void)dealloc {
    m_listeners.clear();
    [self.locationManager release];
    [super dealloc];
}

@end


struct IPhoneLocationUtil::posReqImpl
{
   MyCLController* internal;
};


IPhoneLocationUtil::IPhoneLocationUtil()
{
   m_posReqImpl = new posReqImpl;
   m_posReqImpl->internal = [[MyCLController alloc] init];
}

IPhoneLocationUtil::~IPhoneLocationUtil()
{
   [m_posReqImpl->internal.locationManager stopUpdatingLocation];
   [m_posReqImpl->internal release];

}
bool IPhoneLocationUtil::addListener(WFAPI::IPLBSListener* listener)
{
   if(m_posReqImpl->internal) {
      [m_posReqImpl->internal addListener:listener];
      return TRUE;
   }
   else {
      return FALSE;
   }
}

void IPhoneLocationUtil::startRequesting(bool alsoUseGps)
{
   if(m_posReqImpl)
   {
      if(m_posReqImpl->internal) {
         if(m_posReqImpl->internal->locationManager) {
            if(alsoUseGps) { //use all hardware we got
               m_posReqImpl->internal->locationManager.distanceFilter =
                  kCLDistanceFilterNone;
               m_posReqImpl->internal->locationManager.desiredAccuracy =
                  kCLLocationAccuracyBest;
            }
            else { //only use cellid/wlanPos
               m_posReqImpl->internal->locationManager.distanceFilter =
                  25.0; //XXX: is 25 meters a good threshold?
               m_posReqImpl->internal->locationManager.desiredAccuracy =
                  kCLLocationAccuracyHundredMeters;
            }
            //always try to stop first
            [m_posReqImpl->internal->locationManager stopUpdatingLocation];
            //then try to start
            [m_posReqImpl->internal->locationManager startUpdatingLocation];
         }
      }
   }
}

void IPhoneLocationUtil::stopRequesting()
{
   [m_posReqImpl->internal.locationManager stopUpdatingLocation];
}
