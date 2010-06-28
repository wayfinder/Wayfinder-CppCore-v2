/*
  Copyright (c) 1999 - 2010, Vodafone Group Services Ltd
  All rights reserved.

  Redistribution and use in source and binary forms, with or without modification, are permitted provided that the following conditions are met:

  * Redistributions of source code must retain the above copyright notice, this list of conditions and the following disclaimer.
  * Redistributions in binary form must reproduce the above copyright notice, this list of conditions and the following disclaimer in the documentation and/or other materials provided with the distribution.
  * Neither the name of the Vodafone Group Services Ltd nor the names of its contributors may be used to endorse or promote products derived from this software without specific prior written permission.

  THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS" AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE ARE DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT HOLDER OR CONTRIBUTORS BE LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
*/

/*
 * machine.h
 *
 * Define global types for the i386-redhat-linux systems.
 *
 *
 */

#ifndef I386_REDHAT_LINUX_MACHINE_H
#define I386_REDHAT_LINUX_MACHINE_H
#include <limits.h>
#include <float.h>
#include <sys/param.h>
#include "../factored/systemindependenttypes.h"
//integer bit width and sign type definitions.
typedef unsigned char      uint8;
typedef signed   char       int8;
typedef unsigned short     uint16;
typedef signed   short      int16;
typedef unsigned int       uint32;
typedef signed   int        int32;
#ifdef __GNUC__
typedef unsigned long long uint64;
typedef signed   long long  int64;
#endif


// macros for proper printf formats
// could already be defined in inttypes.h from C99
#ifndef PRId8
# define PRId8 "hhd"
# define PRIi8 "hhi"
# define PRIo8 "hho"
# define PRIu8 "hhu"
# define PRIx8 "hhx"
# define PRIX8 "hhX"
#endif
#ifndef PRId16
# define PRId16 "hd"
# define PRIi16 "hi"
# define PRIo16 "ho"
# define PRIu16 "hu"
# define PRIx16 "hx"
# define PRIX16 "hX"
#endif
#ifndef PRId32
# define PRId32 "d"
# define PRIi32 "i"
# define PRIo32 "o"
# define PRIu32 "u"
# define PRIx32 "x"
# define PRIX32 "X"
#endif
#ifndef PRId64
# define PRId64 "lld"
# define PRIi64 "lli"
# define PRIo64 "llo"
# define PRIu64 "llu"
# define PRIx64 "llx"
# define PRIX64 "llX"
#endif


#ifndef SCNd8
# define SCNd8 "hhd"
# define SCNi8 "hhi"
# define SCNo8 "hho"
# define SCNu8 "hhu"
# define SCNx8 "hhx"
#endif
#ifndef SCNd16
# define SCNd16 "hd"
# define SCNi16 "hi"
# define SCNo16 "ho"
# define SCNu16 "hu"
# define SCNx16 "hx"
#endif
#ifndef SCNd32
# define SCNd32 "d"
# define SCNi32 "i"
# define SCNo32 "o"
# define SCNu32 "u"
# define SCNx32 "x"
#endif
// SCNd64
// SCNi64
// SCNo64
// SCNu64
// SCNx64

// floating point types
typedef float  float32;
typedef double float64;

// Convenience types
typedef uint8 byte;


//integer MAX and MINS
#define MAX_UINT8  UCHAR_MAX
#define MAX_INT8   SCHAR_MAX
#define MIN_INT8   SCHAR_MIN

#define MAX_UINT16 USHRT_MAX
#define MAX_INT16  SHRT_MAX
#define MIN_INT16  SHRT_MIN

#define MAX_UINT32 UINT_MAX
#define MAX_INT32  INT_MAX
#define MIN_INT32  INT_MIN
#ifdef __GNUC__ // It proved troublesome to find usable constants.
#  define MAX_UINT64 0xffffffffffffffffULL
#  define MAX_INT64  0x7fffffffffffffffLL
#  define MIN_INT64  (-MAX_INT64 - 1LL)
#endif
#define MAX_BYTE MAX_UINT8

inline uint32 low32bits(uint64 biggie)
{
   uint32 low = biggie & 0x0ffffffffULL;
   return low;
}

inline uint32 low32bits(int64 biggie)
{
   return low32bits(uint64(biggie));
}

inline uint32 high32bits(uint64 biggie)
{
   return low32bits(biggie >> 32);
}

inline uint32 high32bits(int64 biggie)
{
   return high32bits(uint64(biggie));
}

inline int64 MakeInt64(uint32 high, uint32 low) 
{
   uint64 ret = high;
   ret <<= 32;
   ret |= low;
   return int64(ret);
}

#define HIGH(i64) high32bits(i64)
#define LOW(i64)  low32bits(i64)
#define SET(i64, high, low) do{ i64 = MakeInt64(high, low); }while(0)

//floating point max and mins
#define MAX_FLOAT32 FLT_MAX
#define MIN_FLOAT32 FLT_MIN
#define MAX_FLOAT64 DBL_MAX
#define MIN_FLOAT64 DBL_MIN

namespace isab {

      /* 
       * Support routines for converting litte- and big-endian numbers to
       * native form.
       */

      static inline uint16 conv_betoh_uint16(uint8 *ptr)
      {
        uint16 value;

        *(((uint8 *) &value)+1) = *ptr++;
        *(((uint8 *) &value)+0) = *ptr++;
        return value;
      }

      static inline void conv_htobe_uint16(uint16 value, uint8 *ptr)
      {
        *ptr++=*(((uint8 *) &value)+1);
        *ptr++=*(((uint8 *) &value)+0);
      }

      static inline int16 conv_betoh_int16(uint8 *ptr)
      {
        int16 value;

        *(((uint8 *) &value)+1) = *ptr++;
        *(((uint8 *) &value)+0) = *ptr++;
        return value;
      }

      static inline void conv_htobe_int16(int16 value, uint8 *ptr)
      {
        *ptr++=*(((uint8 *) &value)+1);
        *ptr++=*(((uint8 *) &value)+0);
      }

      static inline int32 conv_betoh_int32(uint8 *ptr)
      {
        int32 value;

        *(((uint8 *) &value)+3) = *ptr++;
        *(((uint8 *) &value)+2) = *ptr++;
        *(((uint8 *) &value)+1) = *ptr++;
        *(((uint8 *) &value)+0) = *ptr++;
        return value;
      }

      static inline uint32 conv_betoh_uint32(uint8 *ptr)
      {
        uint32 value;

        *(((uint8 *) &value)+3) = *ptr++;
        *(((uint8 *) &value)+2) = *ptr++;
        *(((uint8 *) &value)+1) = *ptr++;
        *(((uint8 *) &value)+0) = *ptr++;
        return value;
      }

      static inline float conv_betoh_float(uint8 *ptr)
      {
        float value;

        *(((uint8 *) &value)+3) = *ptr++;
        *(((uint8 *) &value)+2) = *ptr++;
        *(((uint8 *) &value)+1) = *ptr++;
        *(((uint8 *) &value)+0) = *ptr++;
        return value;
      }

      static inline void conv_htobe_float(float value, uint8 *ptr)
      {
        *ptr++=*(((uint8 *) &value)+3);
        *ptr++=*(((uint8 *) &value)+2);
        *ptr++=*(((uint8 *) &value)+1);
        *ptr++=*(((uint8 *) &value)+0);
      }

      static inline double conv_betoh_double(uint8 *ptr)
      {
        double value;

        *(((uint8 *) &value)+3) = *ptr++;
        *(((uint8 *) &value)+2) = *ptr++;
        *(((uint8 *) &value)+1) = *ptr++;
        *(((uint8 *) &value)+0) = *ptr++;
        return value;
      }



};

#endif
