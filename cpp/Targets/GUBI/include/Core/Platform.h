
#ifndef PLATFORM_H
#define PLATFORM_H

#if defined (__unix__) || defined (__MACH__) || defined (__SYMBIAN32__)
#include <sys/time.h>
#elif defined(ARCH_OS_WINDOWS)
/* needed for compiling on Win32 as there is no sys\time.h */
#elif defined(_WIN32)
#include <windows.h>
#else
#error "Unknown system"
#endif

#ifndef NULL
#ifdef __cplusplus
#define NULL 0
#else
#define NULL ((void*)0)
#endif
#endif

inline unsigned int GetTimeTick()
{
#if defined (__unix__) || defined (__MACH__) || defined(IPHONE)
   struct timeval tv;
   gettimeofday(&tv, NULL);
   return (tv.tv_usec/1000 + tv.tv_sec * 1000);
#elif defined(__SYMBIAN32__)
   TTime now;
   now.UniversalTime(); // or UniversalTime
#if defined NAV2_CLIENT_UIQ3 || defined NAV2_CLIENT_SERIES60_V3
   return LOW(now.Int64() / 1000);
#else
   return (now.Int64() / 1000).GetTInt();
#endif
#elif defined(ARCH_OS_WINDOWS) || defined(_WIN32)
   return GetTickCount();
#endif
}

template<class _InIt, class _Cl, class _Ar>
void for_each(
   _InIt _FirstIt,
   _InIt _LastIt,
   _Cl& _Class,
   void (_Cl::*_Func)(_Ar) )
{
   for (; _FirstIt != _LastIt; ++_FirstIt)
   {
      (_Class.*_Func)(*_FirstIt);
   }
}

#endif // PLATFORM_H
