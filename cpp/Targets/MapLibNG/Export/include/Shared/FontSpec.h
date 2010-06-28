/*
Copyright (c) 1999 - 2010, Vodafone Group Services Ltd
All rights reserved.

Redistribution and use in source and binary forms, with or without modification, are permitted provided that the following conditions are met:

    * Redistributions of source code must retain the above copyright notice, this list of conditions and the following disclaimer.
    * Redistributions in binary form must reproduce the above copyright notice, this list of conditions and the following disclaimer in the documentation and/or other materials provided with the distribution.
    * Neither the name of the Vodafone Group Services Ltd nor the names of its contributors may be used to endorse or promote products derived from this software without specific prior written permission.

THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS" AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE ARE DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT HOLDER OR CONTRIBUTORS BE LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
*/


#ifndef _FONTSPEC_H_
#define _FONTSPEC_H_

#include "RefCounted.h"
#include "WFString.h"

namespace WFAPI {

/**
 * Structure which holds font information.
 */ 
class FontSpec : public RefCounted {
public:
   /**
    *  Constructs a default font specification. Using this specification
    *  means that the default font for the platform is chosen, which is
    *  guaranteed to be valid.
    *
    *  Initially the reference count will be set to 1. Once you are done
    *  with this object, you will need to call removeReference to ensure
    *  that the object is properly deallocated.
    */
   static FontSpec* allocate();

   /**
    *  Constructs a font specification. Depending on the platform,
    *  the string syntax is different. Some platforms also embed
    *  the size within the font string itself. In these cases,
    *  the fontSize argument is not considered. 
    *
    *  Initially the reference count will be set to 1. Once you are done
    *  with this object, you will need to call removeReference to ensure
    *  that the object is properly deallocated.
    *
    * @param fontString The name of the font to use
    * @param fontSize The size of the font to use
    */
   static FontSpec* allocate(const WFString& fontString, unsigned int fontSize);

   /**
    * Destructor.
    */ 
   virtual ~FontSpec();
   
   /**
    *  @return The font string associated with this spec.
    */ 
   const WFString& getFontString();

   /**
    *  @return The font size. Dimension is platform specific.
    */ 
   unsigned int getFontSize();

   /**
    *  Equality comparison function for the font.
    */ 
   bool operator==(const FontSpec& rhs);

   /**
    *  Non-equality comparison function for the font.
    */ 
   bool operator!=(const FontSpec& rhs);

   /**
    *  Less-than operator for the font.
    */ 
   bool operator<(const FontSpec& rhs);
protected:

   /**
    * Protected constructor. Please use allocator function instead.
    *
    * @param fontString The name of the font to use
    * @param fontSize The size of the font to use
    */
   FontSpec(const WFString& fontString, unsigned int fontSize);

   /**
    * Default constructor.
    */
   FontSpec();

   /// Name of the font
   WFString m_fontString;

   /// Size of the font
   unsigned int m_fontSize;
};
   
}

#endif /* _FONTSPEC_H_ */
