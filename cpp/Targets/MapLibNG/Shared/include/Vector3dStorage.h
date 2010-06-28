/*
Copyright (c) 1999 - 2010, Vodafone Group Services Ltd
All rights reserved.

Redistribution and use in source and binary forms, with or without modification, are permitted provided that the following conditions are met:

    * Redistributions of source code must retain the above copyright notice, this list of conditions and the following disclaimer.
    * Redistributions in binary form must reproduce the above copyright notice, this list of conditions and the following disclaimer in the documentation and/or other materials provided with the distribution.
    * Neither the name of the Vodafone Group Services Ltd nor the names of its contributors may be used to endorse or promote products derived from this software without specific prior written permission.

THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS" AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE ARE DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT HOLDER OR CONTRIBUTORS BE LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
*/


#ifndef VECTOR3DSTORAGE_H
#define VECTOR3DSTORAGE_H

#include "config.h"
#include "FixedPointType.h"
#include <math.h>

#undef min
#undef max

class DoubleStorageType {
public:
   typedef double Type;
   typedef double UpperType;

   
   static const Type fromFloat( float f ) {
      return f;
   }

   
   static const Type zero() {
      return 0.0;
   }
   
   static const Type one()  {
      return 1.0;
   }
   
   static const Type precision() {
      return 0.00000001;
   }

   static const Type max() {
      return 1.79769313486231570e+308;
   }

   static const Type min() {
      return 4.94065645841246544e-324;
   }

   static const Type abs( Type val ) {
      return ::fabs( val );
   }

   static const Type sqrt( Type val ) {
      return ::sqrt( val );
   }
};

class FloatStorageType {
public:
   typedef float Type;
   typedef double UpperType;
   
   static const Type zero() {
      return 0.0f;
   }

   static const Type fromFloat( float f ) {
      return f;
   }
   
   static const Type one()  {
      return 1.0f;
   }
   
   static const Type precision() {
      return 0.00000001f;
   }

   static const Type max() {
      return 3.40282347e+37f;
   }

   static const Type min() {
      return 1.40239846e-45f;
   }

   static const Type abs( Type val ) {
      return ::fabs( val );
   }

   static const Type sqrt( Type val ) {
      return ::sqrt( val );
   }
};


class FixedStorageType {
public:
   typedef fixedp Type;
   typedef fixedp UpperType;

   static const Type zero() {
	   return Type(Type::RAW, 0);
   }
   
   static const Type fromFloat( float f ) {
      return Type( f );
   }
   
   static const Type one()  {
	   return Type(Type::RAW, 65536);
   }
   
   static const Type precision() {
      return Type(0.0001f);
   }

   static const Type max() {
      return Type(Type::RAW, (int)MAX_INT32 );
   }

   static const Type min() {
      return Type(Type::RAW, (int)MIN_INT32 );
   }

   static const Type abs( Type val ) {
      if( val < zero() ) {
         return fixedp(0)-val;
      } else {
         return val;
      }
   }

   static const Type sqrt( Type val ) {
      return sqrt((float)val);
   }
};



#endif /* VECTOR3DSTORAGE_H */
