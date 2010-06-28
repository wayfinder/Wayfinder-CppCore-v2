/*
Copyright (c) 1999 - 2010, Vodafone Group Services Ltd
All rights reserved.

Redistribution and use in source and binary forms, with or without modification, are permitted provided that the following conditions are met:

    * Redistributions of source code must retain the above copyright notice, this list of conditions and the following disclaimer.
    * Redistributions in binary form must reproduce the above copyright notice, this list of conditions and the following disclaimer in the documentation and/or other materials provided with the distribution.
    * Neither the name of the Vodafone Group Services Ltd nor the names of its contributors may be used to endorse or promote products derived from this software without specific prior written permission.

THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS" AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE ARE DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT HOLDER OR CONTRIBUTORS BE LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
*/


#include "FixedPointType.h"


ostream& operator << (ostream& oStream, const fixedp& val) {
   oStream << (float)val;
   return oStream;
}

fixedp operator-(const fixedp a)
{
   fixedp ret = a;
   ret.fx = -ret.fx;
   return ret; 
}


// /**
//  *   Computes the integer square root of a number.
//  *
//  *
//  */ 
   
// // shift a to the left by n bits (between 0 and 31),
// // return the result as 64 bit.
// int64 __inline shift_left (uint32 a, uint32 n)
// {
//    union {
//       uint32 ui[2];
//       int64  ll;
//    } c;
//    if (n) {
//       c.ui[0] = a <<n;
//       c.ui[1] = a >>(32-n);
//       return c.ll;
//    } else
//       return a;
// }

// static inline unsigned int isqrt64 (int64 value) {
//    unsigned int g = 0;                                
//    unsigned int bshft = 31;                           
//    unsigned int b = 1<<31;
//    do {               
//       int64 temp = shift_left (g+g+b, bshft);

//       if (value >= temp) {                                        
//          g += b;                                
//          value -= temp;
//       }
//       b>>=1;                                   
//    } while (bshft--);

//    return g;
// }   
   
// /**
//  *   Converts a fixed point number to double.
//  *   
//  *   @param num The double number.
//  *
//  */
   
// template<typename Primitive>
// double fixedToDouble( Primitive num  ) {
//    return static_cast<double>( (num) * FXFACTOR );
// }
   
// /**
//  *   Converts a fixed point 16.16 integer to integer format.
//  *   
//  *   @param num The fixed point number.
//  *
//  */

// template<typename Primitive>
// Primitive fixedToInt( Primitive num ) {
//    return num >> FIXED_SHIFT;
// }
   
// /**
//  *   Converts a standard integer to 16.16 fixed point format.
//  *   
//  *   @param num The integer number.
//  *
//  */

// template<typename Primitive>
// Primitive intToFixed( Primitive num ) {
//    Primitive ret = num;
//    ret = ret << FIXED_SHIFT;
//    return ret;
// }
   
// /**
//  *   Converts a double number to 16.16 fixed point integer format.
//  *   
//  *   @param num The double number.
//  *
//  */

// template<typename Primitive>
// Primitive doubleToFixed( double num  ) {
//    return static_cast<Primitive>(( num *FXINV ));
// }
   
// /**
//  *
//  *    Divides two 16.16 numbers.
//  *
//  *    @param lhs left hand side of division.
//  *    @param rhs right hand side of division
//  *
//  */ 

// template<typename Primitive>
// Primitive fixedDiv(const Primitive lhs, const Primitive rhs) {
//    int64 lhsLong = static_cast<int64>(lhs);
//    return static_cast<Primitive>( (lhsLong << FIXED_SHIFT) / rhs );
// }

// template<typename Primitive>
// Primitive fixedMul( const Primitive lhs, const Primitive rhs ) {
//    int64 r = ( (int64) lhs ) * rhs;
//    return r >> FIXED_SHIFT;
// }

// template<>
// inline int fixedMul( const int lhs, const int rhs ) {
//    int64 r = ( (int64) lhs ) * rhs;
//    return static_cast<int>( r >> FIXED_SHIFT );
// }
