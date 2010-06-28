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
 * Define global types for the h8300-unknown-hms system.
 * 
 *
 */

#ifndef H8300_UNKNOWN_HMS_MACHINE_H
#define H8300_UNKNOWN_HMS_MACHINE_H
#include <limits.h>
#include <float.h>
//integer bit width and sign type definitions.
typedef unsigned char      uint8;
typedef signed   char       int8;
typedef unsigned short     uint16;
typedef signed   short      int16;
typedef unsigned long      uint32;
typedef signed   long       int32;
#ifdef __GNUC__
typedef unsigned long long uint64;
typedef signed   long long  int64;
#endif
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

//floating point max and mins
#define MAX_FLOAT32 FLT_MAX
#define MIN_FLOAT32 FLT_MIN
#define MAX_FLOAT64 DBL_MAX
#define MIN_FLOAT64 DBL_MIN

#endif
