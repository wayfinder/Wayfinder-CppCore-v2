/*
Copyright (c) 1999 - 2010, Vodafone Group Services Ltd
All rights reserved.

Redistribution and use in source and binary forms, with or without modification, are permitted provided that the following conditions are met:

    * Redistributions of source code must retain the above copyright notice, this list of conditions and the following disclaimer.
    * Redistributions in binary form must reproduce the above copyright notice, this list of conditions and the following disclaimer in the documentation and/or other materials provided with the distribution.
    * Neither the name of the Vodafone Group Services Ltd nor the names of its contributors may be used to endorse or promote products derived from this software without specific prior written permission.

THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS" AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE ARE DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT HOLDER OR CONTRIBUTORS BE LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
*/


#ifndef IP_LBS_LISTENER_H
#define IP_LBS_LISTENER_H

namespace WFAPI {

class IPLBSListener
{
public:
   
   /**
    * This callback function is called when ever the system has got new
    * positions from the lbsManager. It then reports them as NMEA sentences
    * to NAV2.
    *
    *  @param latitude The latitude
    *  @param longitude The longitude
    *  @param altitude The Altitude
    *  @param speed The speed
    *  @param track The Track angle
    *  @param pdop The position (3D) delusion of precision
    *  @param timestamp The timestamp of the position, seconds since unix epoch
    */
   virtual void lbsCallback(double latitude, double longitude, double altitude,
                            double speed, double track,
                            double horizontalAccuracy, double verticalAccuracy,
                            double timestamp) = 0;


   /**
    * This callback function is called when ever the system is having a hard
    * time starting the lbs engine, typically if the user doesn't accept our apps''
    * request to use the CoreLocation Framework
    */
   virtual void lbsFailed() = 0;

};

} //end of namespace WFAPI

#endif /* IP_LBS_LISTENER_H */
