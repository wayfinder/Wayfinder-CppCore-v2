/*
  Copyright (c) 1999 - 2010, Vodafone Group Services Ltd
  All rights reserved.

  Redistribution and use in source and binary forms, with or without modification, are permitted provided that the following conditions are met:

  * Redistributions of source code must retain the above copyright notice, this list of conditions and the following disclaimer.
  * Redistributions in binary form must reproduce the above copyright notice, this list of conditions and the following disclaimer in the documentation and/or other materials provided with the distribution.
  * Neither the name of the Vodafone Group Services Ltd nor the names of its contributors may be used to endorse or promote products derived from this software without specific prior written permission.

  THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS" AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE ARE DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT HOLDER OR CONTRIBUTORS BE LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
*/

#ifndef WFAPI_SELECTEDMAPOBJECTLISTENER_H
#define WFAPI_SELECTEDMAPOBJECTLISTENER_H

#include <vector>

namespace WFAPI {

class OverlayItem;
class MapObjectInfo;
class WGS84Coordinate;
   
/**
 * Listener class for handling when a map object has been selected in the map,
 * for instance by clicking.
 */
class SelectedMapObjectListener {
public:
 
   /**
    * Typedef for vector of OverlayItems.
    */
   typedef std::vector<const OverlayItem*> OverlayItemVector;
   
   /**
    * Destructor.
    */
   virtual ~SelectedMapObjectListener() {};
   
   /**
    * Handle that a map object has been selected in the map.
    * In case an OverlayItem has been selected, then a pointer to the
    * OverlayItem is supplied as a parameter, otherwise NULL.
    * 
    * @param mapObjectInfo MapObjectInfo containing details about the selected
    *                      map object (always set).
    * @param overlayItem In case an OverlayItem was selected, then a pointer
    *                    to the OverlayItem will be passed as parameter, otherwise
    *                    NULL.
    * @param coord The coordinate for the selected object.                   
    * @param longPress True if the selection was the result of a long press action.  
    */
   virtual void handleSelectedMapObject(const MapObjectInfo& mapObjectInfo,
                                        OverlayItem* overlayItem,
                                        const WGS84Coordinate& coord,
                                        bool longPress) = 0;

   /**
    * Handle that a stacked dialog will be opened when a stacked item has
    * been clicked.
    */
   virtual void handleStackedDialogOpened() = 0;

   /**
    * Handle that an opened stacked dialog will be closed.
    */
   virtual void handleStackedDialogClosed() = 0;
   
};

} // End of namespace WFAPI

#endif // WFAPI_SELECTEDMAPOBJECTLISTENER_H
