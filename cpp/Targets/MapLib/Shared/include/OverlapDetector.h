/*
  Copyright (c) 1999 - 2010, Vodafone Group Services Ltd
  All rights reserved.

  Redistribution and use in source and binary forms, with or without modification, are permitted provided that the following conditions are met:

  * Redistributions of source code must retain the above copyright notice, this list of conditions and the following disclaimer.
  * Redistributions in binary form must reproduce the above copyright notice, this list of conditions and the following disclaimer in the documentation and/or other materials provided with the distribution.
  * Neither the name of the Vodafone Group Services Ltd nor the names of its contributors may be used to endorse or promote products derived from this software without specific prior written permission.

  THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS" AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE ARE DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT HOLDER OR CONTRIBUTORS BE LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
*/

#ifndef OVERLAPDETECTOR
#define OVERLAPDETECTOR

#include <vector>
#include "config.h"

/**
 *   Contains pixelboxes to be checked for overlap
 *   
 */
template<class BBOX> class OverlapDetector {
   
   public:

      /**
       *   Clear everything.
       */
      void clear() {
         m_boxes.clear();
      }

      /**
       *   Add the bbox if it does not overlap anything in
       *   the OverlapDetector.
       */
      bool addIfNotOverlapping( const BBOX& inbox ) {
         typename storage_t::const_iterator it;
         for( it = m_boxes.begin();
              it != m_boxes.end();
              ++it ) {
            if ( inbox.overlaps(*it) ) {
               return false;
            }
         }
         m_boxes.push_back(inbox);
         return true;
      }

      /**
       *   If none of the supplied bboxes overlap the boxes
       *   in the list, then add all. If one overlaps, add none.
       */
      bool addIfNotOverlapping( const std::vector<BBOX>& inboxes ) {
         typename storage_t::const_iterator it;
         for( it = m_boxes.begin();
              it != m_boxes.end();
              ++it ) {
            typename std::vector<BBOX>::const_iterator jt;
            for ( jt = inboxes.begin();
                  jt != inboxes.end();
                  ++jt ) {
               if ( (*jt).overlaps(*it) ) {
                  return false;
               }
            }
         }
         m_boxes.insert(m_boxes.end(), inboxes.begin(), inboxes.end());
         return true;
      }

   private:

      /// Type of storage
      typedef std::vector<BBOX> storage_t;
   
      /// The non-overlapping boxes are stored here.
      std::vector<BBOX> m_boxes;
      
};

#endif
