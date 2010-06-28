/*
  Copyright (c) 1999 - 2010, Vodafone Group Services Ltd
  All rights reserved.

  Redistribution and use in source and binary forms, with or without modification, are permitted provided that the following conditions are met:

  * Redistributions of source code must retain the above copyright notice, this list of conditions and the following disclaimer.
  * Redistributions in binary form must reproduce the above copyright notice, this list of conditions and the following disclaimer in the documentation and/or other materials provided with the distribution.
  * Neither the name of the Vodafone Group Services Ltd nor the names of its contributors may be used to endorse or promote products derived from this software without specific prior written permission.

  THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS" AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE ARE DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT HOLDER OR CONTRIBUTORS BE LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
*/

#ifndef WFAPI_WFSTRING_H
#define WFAPI_WFSTRING_H

#include "PALMachineTypes.h"

#ifdef WFAPI_IOSTREAM_AVAILABLE
#include <iostream>
#endif //WFAPI_IOSTREAM_AVAILABLE

namespace WFAPI {

/**
 * Simple string type for resource management purposes.
 * Conceptually it only wraps const char* pointers and
 * deals with allocation and deallocation as a standard
 * C++ class would.
 */
class WFString {
public:
   /**
    *   Creates an empty string.
    */
   WFString();

   /**
    *   Creates a copy of the supplied string and stores it
    *   inside.
    */
   WFString(const char* str);

   /**
    *   Creates a copy of the supplied WFString.
    */
   WFString(const WFString& other);

   /**
    *   Destructor. Deletes the internal char-array.
    */
   ~WFString();
   
   /**
    *   Returns a pointer to the string owned by this object.
    */
   const char* c_str() const;

   /**
    *   Assignment operator.
    */
   WFString& operator=(const WFString& other);
   
   /**
    *   Assignment operator.
    */
   WFString& operator=(const char* str);   
   
   /**
    *   Comparison function which can be used when the string
    *   is put into e.g. maps.
    */
   bool operator<(const WFString& other) const;

   /**
    *   Comparison function which can be used when the string
    *   is put into e.g. maps.
    */
   bool operator<(const char* str) const;

   /**
    *   Comparison function.
    */
   bool operator>(const WFString& other) const;

   /**
    *   Comparison function.
    */
   bool operator>(const char* str) const;

   /**
    *   Returns true if the two strings are lexically the same.
    */
   bool operator==(const WFString& other) const;

   /**
    *   Returns true if the two strings are lexically the same.
    */
   bool operator==(const char* other ) const;

   /**
    *   Returns false if the two strings are lexically the same.
    */
   bool operator!=(const WFString& other) const;

   /**
    *   Returns false if the two strings are lexically the same.
    */
   bool operator!=(const char* other) const;

   /**
    *   Returns the character at position i.
    */
   char operator[] (int i) const;
   
   /**
    *   Returns the length of the string.
    */
   unsigned int length() const;

   /**
    *   Returns true if the string is empty.
    */
   int empty() const;

   /**
    *   Clears the string.
    */
   void clear();
   
#ifdef WFAPI_IOSTREAM_AVAILABLE
   
   /**
    * Print on ostream.
    *
    * @param stream The stream to print on.
    * @param str The WFString to print.
    * @return The stream.
    */
   friend std::ostream& operator<< (std::ostream& stream, 
                                    const WFString& str)
   {
      return stream << str.c_str();
   }

#endif //WFAPI_IOSTREAM_AVAILABLE
   
private:
   /**
    *   Initializes the WFString.
    */
   void init(const char* str);

   /**
    *   Initializes the WFString.
    */
   void init(const WFString& other);

   
   /// Empty string. Not copied etc.
   static const char* const c_emptyString;
      
   /**
    *   The string itself.
    */
   char* m_str;

   /**
    *   The length of the string.
    */
   unsigned int m_len;
};

} // End namespace WFAPI

#endif // WFAPI_WFSTRING_H
