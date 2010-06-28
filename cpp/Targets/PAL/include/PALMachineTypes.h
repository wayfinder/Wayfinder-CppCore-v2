/*
  Copyright (c) 1999 - 2010, Vodafone Group Services Ltd
  All rights reserved.

  Redistribution and use in source and binary forms, with or without modification, are permitted provided that the following conditions are met:

  * Redistributions of source code must retain the above copyright notice, this list of conditions and the following disclaimer.
  * Redistributions in binary form must reproduce the above copyright notice, this list of conditions and the following disclaimer in the documentation and/or other materials provided with the distribution.
  * Neither the name of the Vodafone Group Services Ltd nor the names of its contributors may be used to endorse or promote products derived from this software without specific prior written permission.

  THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS" AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE ARE DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT HOLDER OR CONTRIBUTORS BE LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
*/

#ifndef PAL_MACHINETYPES_H
#define PAL_MACHINETYPES_H

/*
 *   This file contains the machine specific type defines that are
 *   necessary for consistent data type size and sign. Classes that
 *   are to be exported as part of the external API will need to have
 *   their types defined here.
 */

#if (defined _WIN32_WCE)

// Microsoft workaround
#ifdef RGB
#undef RGB
#endif

/* Arch Data Types for Win32 Platform */

#define WFAPI_IOSTREAM_AVAILABLE

/* integer types */
namespace pal {

typedef char wf_char;
typedef char wf_int8;
typedef unsigned char wf_uint8;
typedef unsigned short wf_uint16; 
typedef signed short wf_int16; 
typedef unsigned int wf_uint32; 
typedef int wf_int32; 
typedef unsigned __int64 wf_uint64; 
typedef __int64 wf_int64; 
typedef unsigned int wf_uint;
typedef int wf_int;
typedef void (*wf_dealloc_func)(void*);
typedef void* (*wf_alloc_func)(size_t);
   
/* floating-point types */
typedef float wf_float32; 
typedef double wf_float64; 

static const wf_uint8 WF_MAX_UINT8 = wf_uint8(-1);
static const wf_uint16 WF_MAX_UINT16 = wf_uint16(-1);
static const wf_int32 WF_MIN_INT32 = wf_int32(0x80000000);
static const wf_int32 WF_MAX_INT32 = wf_int32(0x7fffffff);
static const wf_uint32 WF_MAX_UINT32 = wf_uint32(-1);
static const wf_int64 WF_MAX_INT64 = 0x7fffffffffffffffLL;
static const wf_int64 WF_MIN_INT64 = (-WF_MAX_INT64 - 1LL);

}

#elif (defined __SYMBIAN32__)
   /// For symbian specific types

   #include <e32def.h>

   namespace pal {
   typedef char wf_char;
   typedef TUint8 wf_uint8;
   /// A flotating point number of 32 bits.
   typedef TReal32 wf_float32;
   typedef TReal64 wf_float64;
   
   typedef TInt32 wf_int32;
   typedef TInt64 wf_int64;
   typedef TInt32 wf_int;
   typedef TUint16 wf_uint16;
   typedef TUint32 wf_uint32;
   typedef TUint32 wf_uint;

   typedef void (*wf_dealloc_func)(void*);
   typedef void* (*wf_alloc_func)(unsigned int);
   
   static const wf_uint8 WF_MAX_UINT8 = wf_uint8(-1);
   static const wf_uint16 WF_MAX_UINT16 = wf_uint16(-1);
   static const wf_int32 WF_MIN_INT32 = wf_int32(0x80000000);
   static const wf_int32 WF_MAX_INT32 = wf_int32(0x7fffffff);
   static const wf_uint32 WF_MAX_UINT32 = wf_uint32(-1);
   static const wf_int64 WF_MAX_INT64 = 0x7fffffffffffffffLL;
   static const wf_int64 WF_MIN_INT64 = (-WF_MAX_INT64 - 1LL);

   } // End of namespace pal

#elif defined(__unix__) || defined(__MACH__)
   // For linux/unix specific types.

   #define WFAPI_IOSTREAM_AVAILABLE
#ifndef __STDC_LIMIT_MACROS
   #define __STDC_LIMIT_MACROS
#endif

   #include <stdint.h>
   #include <limits.h>
   // Defines NULL
   #include <stdio.h>
   
   // Get the format macros from inttypes.
   #ifndef  __STDC_FORMAT_MACROS
     #define __STDC_FORMAT_MACROS
   #endif
   #include  <inttypes.h>

   /**
    * The namespace for the platform abstraction layer
    */
   namespace pal {
   
   /// A signed 16 bit integer.
   typedef short int	wf_int16;
   /// A signed 32 bit integer.
   typedef int	wf_int32;
   /// A signed 64 bit integer.
   typedef long long int wf_int64;
   /// A signed integer of system dependant size.
   typedef int	wf_int;
   /// A character type
   typedef char wf_char;
   /// A usigned 8 bit integer.
   typedef unsigned char wf_uint8;
   /// A usigned 16 bit integer.
   typedef unsigned short int	wf_uint16;
   /// A usigned 32 bit integer.
   typedef unsigned int	wf_uint32;
   /// A usigned 64 bit integer.
   typedef unsigned long long int wf_uint64;
   /// A usigned integer of system dependant size.
   typedef unsigned int	wf_uint;
   /// A flotating point number of 32 bits.
   typedef float wf_float32;
   /// A flotating point number of 64 bits.
   typedef double wf_float64;

   static const wf_uint8 WF_MAX_UINT8 = wf_uint8(-1);
   static const wf_uint16 WF_MAX_UINT16 = wf_uint16(-1);
   static const wf_int32 WF_MIN_INT32 = INT_MIN;
   static const wf_int32 WF_MAX_INT32 = INT_MAX;
   static const wf_uint32 WF_MAX_UINT32 = wf_uint32(-1);
   static const wf_int64 WF_MAX_INT64 = 0x7fffffffffffffffLL;
   static const wf_int64 WF_MIN_INT64 = (-WF_MAX_INT64 - 1LL);
   static const wf_uint64 WF_MAX_UINT64 = wf_uint64(-1);

   typedef void (*wf_dealloc_func)(void*);
   typedef void* (*wf_alloc_func)(size_t);
   
   } // End of namespace pal
#endif

namespace WFAPI {
using namespace pal;
}


#endif // PAL_MACHINETYPES_H


