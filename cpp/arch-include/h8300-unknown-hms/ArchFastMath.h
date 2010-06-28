/*
  Copyright (c) 1999 - 2010, Vodafone Group Services Ltd
  All rights reserved.

  Redistribution and use in source and binary forms, with or without modification, are permitted provided that the following conditions are met:

  * Redistributions of source code must retain the above copyright notice, this list of conditions and the following disclaimer.
  * Redistributions in binary form must reproduce the above copyright notice, this list of conditions and the following disclaimer in the documentation and/or other materials provided with the distribution.
  * Neither the name of the Vodafone Group Services Ltd nor the names of its contributors may be used to endorse or promote products derived from this software without specific prior written permission.

  THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS" AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE ARE DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT HOLDER OR CONTRIBUTORS BE LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
*/

/* Fast math functions and defines.
 * (arch specific parts)
 */

#ifndef ARCH_ArchFastMath_H
#define ARCH_ArchFastMath_H

/*
 *  Inline magic.
 */
#ifdef SHARED_ArchFastMath_C
#define INLINE inline
#else
#define INLINE extern inline
#endif

namespace isab {


/*+ Define if the cpu has a uin16/uint16 divide that is 
 * significantly faster than a uint32/uint16 divide  +*/
#undef HAS_UINT16_UINT16_DIV


   INLINE uint16 fast_div_uint32_uint16_to_uint16(uint32 a, uint16 b)  
      __attribute__ ((const));
   /**
   * Divide an uint32 by an uint16 to a uint16
   * 
   * @param a   The numerator
   * @param b   The divisor
   *
   * This is a fast assembler implementation for the h8300h. It uses
   * potentially unsafe assembler constraints. the output and first 
   * input are set to use the same register even though they are 
   * different sizes. This seems to do the right thing on h8300h.
   */
   INLINE uint16 fast_div_uint32_uint16_to_uint16(uint32 a, uint16 b)
   {
      uint32 tmp;

      asm( "\n"
            "        divxu.w %2,%1                   \n"
            : "=&r" (tmp)
            : "0" ((uint32)a), "r" (b)
            : "cc");
      return tmp;
   }


   INLINE int32 fast_sqr_int16_to_int32(int16 a)  __attribute__ ((const));
   /**
   * Square an int16 to an int32.
   * 
   * @param a   The argument to be squared
   *
   * This is a fast assembler implementation for the h8300h. It uses
   * potentially unsafe assembler constraints. the output and first 
   * input are set to use the same register even though they are 
   * different sizes. This seems to do the right thing on h8300h.
   */
   INLINE int32 fast_sqr_int16_to_int32(int16 a)
   {
      int32 tmp;

      asm( "\n"
            "        mulxs.w %1,%0                   \n"
            : "=&r" (tmp)
            : "0" (a)
            : "cc");
      return tmp;
   }


   INLINE uint32 fast_sqr_uint16_to_uint32(uint16 a)  __attribute__ ((const));
   /**
   * Square an uint16 to an uint32.
   * 
   * @param a   The argument to be squared
   *
   * This is a fast assembler implementation for the h8300h. It uses
   * potentially unsafe assembler constraints. the output and first 
   * input are set to use the same register even though they are 
   * different sizes. This seems to do the right thing on h8300h.
   */
   INLINE uint32 fast_sqr_uint16_to_uint32(uint16 a)
   {
      uint32 tmp;

      asm( "\n"
            "        mulxu.w %1,%0                   \n"
            : "=&r" (tmp)
            : "0" (a)
            : "cc");
      return tmp;
   }


   INLINE uint32 fast_mul_uint16_uint16_to_uint32(uint16 a, uint16 b)  __attribute__ ((const));
   /**
   * Multiply two uint16 to an uint32.
   * 
   * @param a   The first factor
   * @param b   The second factor
   *
   * This is a fast assembler implementation for the h8300h. It uses
   * potentially unsafe assembler constraints. the output and first 
   * input are set to use the same register even though they are 
   * different sizes. This seems to do the right thing on h8300h.
   */
   INLINE uint32 fast_mul_uint16_uint16_to_uint32(uint16 a, uint16 b)
   {
      uint32 tmp;

      asm( "\n"
            "        mulxu.w %2,%0                   \n"
            : "=&r" (tmp)
            : "%0" (a), "r" (b)
            : "cc");
      return tmp;
   }


   INLINE int32 fast_mul_int16_int16_to_int32(int16 a, int16 b)  __attribute__ ((const));
   /**
   * Multiply two int16 to an int32.
   * 
   * @param a   The first factor
   * @param b   The second factor
   *
   * This is a fast assembler implementation for the h8300h. It uses
   * potentially unsafe assembler constraints. the output and first 
   * input are set to use the same register even though they are 
   * different sizes. This seems to do the right thing on h8300h.
   */
   INLINE int32 fast_mul_int16_int16_to_int32(int16 a, int16 b)
   {
      int32 tmp;

      asm( "\n"
            "        mulxs.w %2,%0                   \n"
            : "=&r" (tmp)
            : "%0" (a), "r" (b)
            : "cc");
      return tmp;
   }


   /**
    * Calculate the (truncated) sqruare root of an integer
    * 
    * @param R
    *           The number to take the square root of. Only
    *           non-negative numbers are allowed.
    *
    * @return the integer part (truncated) of the square root
    * of R. NOTE! The return value os not correctly rounded, it is
    * truncated.
    *
    * Based on the "Application Brief AB-34" "Integer Square Root 
    * Routine for the 8096" by Lionel Smith from Intel (Intel order
    * number 270532-001).
    *
    * This function handles large input paramters by prescaling 
    * with 1/4 using the fact that sqrt(R)=2*sqrt(R/4). This leads to
    * a maximum error of 2 for large R and 1 for R<0x80000000.
    * This scaling is needed since the first division of 
    * (2^32-1) / (65535) = 65537 which is not a 16 bit number.
    *
    */
   uint16 sqrt_int_fast(int32 R) __attribute__ ((const));


};


#undef INLINE

#endif /* ARCH_ArchFastMath_H */

