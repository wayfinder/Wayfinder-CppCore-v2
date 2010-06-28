/*
  Copyright (c) 1999 - 2010, Vodafone Group Services Ltd
  All rights reserved.

  Redistribution and use in source and binary forms, with or without modification, are permitted provided that the following conditions are met:

  * Redistributions of source code must retain the above copyright notice, this list of conditions and the following disclaimer.
  * Redistributions in binary form must reproduce the above copyright notice, this list of conditions and the following disclaimer in the documentation and/or other materials provided with the distribution.
  * Neither the name of the Vodafone Group Services Ltd nor the names of its contributors may be used to endorse or promote products derived from this software without specific prior written permission.

  THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS" AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE ARE DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT HOLDER OR CONTRIBUTORS BE LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
*/

#ifndef WFAPI_REFCOUNTED_H
#define WFAPI_REFCOUNTED_H

namespace WFAPI {

/**
 * Class that keeps track of dynamically allocated
 * resources which are shared between MapLib and the
 * application.
 *
 * This class is ideally used for objects which are
 * inserted multiple times into MapLib. These objects
 * need to be still allocated under all insertions,
 * even if MapLib no longer holds a reference at the
 * point in time of an insertion. As long as the
 * application has not released its reference to them,
 * this can be guaranteed.
 */ 
class RefCounted {
public:
   
   /**
    * Adds a reference to the object. This increases
    * the reference count by 1. Initially the reference
    * count is set to 1. Const so that we can still add
    * references to const objects.
    */ 
   void addReference() const;

   /**
    * Removes a reference to the object. This decreases the
    * reference count by 1. If the reference count
    * is then equal to 0, the object is deallocated.
    * Const so that we can still remove references
    * to const objects.
    */ 
   void removeReference() const;

   /**
    * @ return the number of references associated with this
    * object.
    */ 
   unsigned int numRefs() const;
   
protected:
   
   /**
    * Constructor. Made protected so that this class
    * can only be instantiated if derived from.
    */ 
   RefCounted();
   
   /**
    * Destructor is made protected so that the object
    * cannot directly be deleted by the user, which
    * enforces the usage of removeReference.
    */ 
   virtual ~RefCounted() = 0;
private:
   /**
    * Hinder copying.
    */ 
   RefCounted(const RefCounted& rhs);

   /**
    * Hinder copying.
    */ 
   RefCounted& operator=(const RefCounted& rhs);
private:
   mutable unsigned int m_numRefs;
};

} // End of namespace WFAPI

#endif // WFAPI_REFCOUNTED_H
