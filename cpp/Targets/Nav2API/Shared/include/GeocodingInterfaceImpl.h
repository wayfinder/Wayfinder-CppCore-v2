/*
Copyright (c) 1999 - 2010, Vodafone Group Services Ltd
All rights reserved.

Redistribution and use in source and binary forms, with or without modification, are permitted provided that the following conditions are met:

    * Redistributions of source code must retain the above copyright notice, this list of conditions and the following disclaimer.
    * Redistributions in binary form must reproduce the above copyright notice, this list of conditions and the following disclaimer in the documentation and/or other materials provided with the distribution.
    * Neither the name of the Vodafone Group Services Ltd nor the names of its contributors may be used to endorse or promote products derived from this software without specific prior written permission.

THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS" AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE ARE DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT HOLDER OR CONTRIBUTORS BE LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
*/


#ifndef GEOCODINGINTERFACEIMPL_H
#define GEOCODINGINTERFACEIMPL_H

#include "InterfaceImpl.h"
#include "AsynchronousStatus.h"
#include "GeocodingListener.h"
#include <set>

#include <map>

// Forward declares
namespace isab{
class DataGuiMess;
}

namespace WFAPI {
class WGS84Coordinate;


class GeocodingInterfaceImpl : public InterfaceImpl {
public:

   /**
    * Constructor.
    *
    * @param api A pointer to the Nav2APIImpl.
    */
   GeocodingInterfaceImpl(Nav2APIImpl* api);

   /**
    * Destructor.
    */
   virtual ~GeocodingInterfaceImpl();
   
   /**
    * Function called when sending reply from Nav2. This is called when
    * receiving replies from asynchronous requests.
    *
    * @param guiProtMess The returning data of the reply.
    * @param requestID The RequestID for the guiProtMess.
    */
   void receiveAsynchronousReply(const isab::GuiProtMess& guiProtMess,
                                 RequestID requestID);
   
   /**
    * Add a listener for geocoding updates. 
    * @see GeocodingListener
    *
    * @param listener The GeocodingListener to add. 
    *                 It may not be deleted until it is removed by
    *                 calling removeGeocodingListener.
    */
   void addGeocodingListener(GeocodingListener* listener);

   /**
    * Remove a listener.
    *
    * @param listener The GeocodingListener to remove.
    *                 It should now be deleted by caller, unless caller
    *                 keeps it around for later use.
    */
   void removeGeocodingListener(GeocodingListener* listener);
   
   /**
    * Request a reverse geocoding from a specified coordinate with
    * the specified precision.
    * The reply will asynchronously be sent to listener.
    *
    * @param coord The coordinate to reverse geocode.
    * @param desiredPrecision The desired precision of the lookup.
    * @param listener The listener that will receive the response.
    * @return A status indicating whether the request was successful or not.
    */
   AsynchronousStatus requestReverseGeocoding(
      const WGS84Coordinate& coord,
      GeocodingInformation::Precision desiredPrecision);
   
   /**
    * Handles the reverse geocoding reply from the server, will
    * call the previously supplied geocoding listener.
    *
    * @param mess The data message received from the server.
    */
   void handleRevGeocodingReply(const isab::DataGuiMess& mess,
                                const RequestID& id);
   
private:

   /// Set for the geocoding listeners.
   typedef std::set<GeocodingListener*> ListenerCont;

   /// Holds all geocoding listeners.
   ListenerCont m_geoListeners;

   /// Map that holds information about all geocoding requests.
   typedef std::map<const RequestID, GeocodingInformation::Precision>
   RequestHolder;

   /// Holds all information about requests.
   RequestHolder m_requestHolder;
   
};

} // End of namespace WFAPI.

#endif // GEOCODINGINTERFACEIMPL_H
