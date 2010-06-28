/*
Copyright (c) 1999 - 2010, Vodafone Group Services Ltd
All rights reserved.

Redistribution and use in source and binary forms, with or without modification, are permitted provided that the following conditions are met:

    * Redistributions of source code must retain the above copyright notice, this list of conditions and the following disclaimer.
    * Redistributions in binary form must reproduce the above copyright notice, this list of conditions and the following disclaimer in the documentation and/or other materials provided with the distribution.
    * Neither the name of the Vodafone Group Services Ltd nor the names of its contributors may be used to endorse or promote products derived from this software without specific prior written permission.

THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS" AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE ARE DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT HOLDER OR CONTRIBUTORS BE LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
*/


#ifndef SELECTEDMAPOBJECTNOTIFIER_H
#define SELECTEDMAPOBJECTNOTIFIER_H

#include "MapObjectInterface.h"
#include <vector>

namespace WFAPI {
class SelectedMapObjectListener;
class MapObjectInfo;
class WGS84Coordinate;   
class OverlayItem;
}


class SelectedMapObjectNotifier : public WFAPI::MapObjectInterface {
public:
   /**
    * Add a SelectedMapObjectListener. This listener will receive a
    * callback when a map object has been selected on the map (for
    * instance because it's clicked).
    *
    * @param listener The listener to add.
    */
   virtual void
   addSelectedMapObjectListener(WFAPI::SelectedMapObjectListener* listener);
   
   /**
    * Remove a SelectedMapObjectListener.
    *
    * @param listener The listener to remove.
    */
   virtual void
   removeSelectedMapObjectListener(WFAPI::SelectedMapObjectListener* listener);

   /**
    * Notifies all of the registered listeners of a selection event.
    *
    * @param mapObjectInfo MapObjectInfo containing details about the selected
    *                      map object (always set).
    * @param overlayItem In case an OverlayItem was selected, then a pointer
    *                    to the OverlayItem will be passed as parameter, otherwise
    *                    NULL.
    * @param longPress True if the selection was the result of a long press.         
    */ 
   void notifyListeners(const WFAPI::MapObjectInfo& mapObjectInfo,
                        WFAPI::OverlayItem* overlayItem,
                        const WFAPI::WGS84Coordinate& coord,
                        bool longPress);

   /**
    * Notifies all of the registered listeners that a stacked dialog has been opened.
    */
   void notifyListenersStackedDialogOpened();

   /**
    * Notifies all of the registered listeners that a stacked dialog has been closed.
    */
   void notifyListenersStackedDialogClosed();
   
private:
   typedef std::vector<WFAPI::SelectedMapObjectListener*> ListenerVector;
   ListenerVector m_listeners;
};


#endif // SELECTEDMAPOBJECTNOTIFIER_H
