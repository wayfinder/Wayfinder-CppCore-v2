/*
  Copyright (c) 1999 - 2010, Vodafone Group Services Ltd
  All rights reserved.

  Redistribution and use in source and binary forms, with or without modification, are permitted provided that the following conditions are met:

  * Redistributions of source code must retain the above copyright notice, this list of conditions and the following disclaimer.
  * Redistributions in binary form must reproduce the above copyright notice, this list of conditions and the following disclaimer in the documentation and/or other materials provided with the distribution.
  * Neither the name of the Vodafone Group Services Ltd nor the names of its contributors may be used to endorse or promote products derived from this software without specific prior written permission.

  THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS" AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE ARE DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT HOLDER OR CONTRIBUTORS BE LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
*/

#ifndef _IPHONELOCATIONUTIL_H_
#define _IPHONELOCATIONUTIL_H_

#include <string>
#include "IPLBSListener.h"

namespace WFAPI {

//Forward declarations
class IPLBSListener;

}

/**
 * IPhone specific lbs utility class. Handles gps communication for
 * the Apple IPhone platform by acting as a layer between objectiv c and
 * regular c++ code..
 */
class IPhoneLocationUtil {
public:
   
   /**
    * Constructor.
    * Upon creation the IPhoneLocationUtil object will create a CoreLocation
    * Controller, which in turn creates a CoreLocation Manager.
    */
   IPhoneLocationUtil();

   /**
    * Destructor, tries to stop position updates if ongoing and
    * deletes the CoreLocation objects.
    */
   ~IPhoneLocationUtil();

   /**
    * Adds a listener to the utility.
    * The listener gets lbsCallbacks and lbsFailed notifications, which it may
    * report to its listeners.
    *
    * @param listener The listener to add.
    *
    * @return true if successfully added, false otherwise.
    */
   bool addListener(WFAPI::IPLBSListener *listener);

   /**
    * Start getting location updates from the location manager.
    *
    * @param alsoUseGps True if we want to use gps as well as cellid etc. (gps
    *                   consumes most power, hence we provide this setting).
    *
    */
   void startRequesting(bool alsoUseGps);

   /**
    * Stop getting location updates from the location manager.
    */
   void stopRequesting(void);

private:

   /// A struct containing an objective C pointer to the CoreLocation Controller.
   struct posReqImpl;

   /// A member struct containing an objective C pointer to the CoreLocation Controller.
   posReqImpl* m_posReqImpl;
};


#endif /* _IPHONELOCATIONUTIL_H_ */
