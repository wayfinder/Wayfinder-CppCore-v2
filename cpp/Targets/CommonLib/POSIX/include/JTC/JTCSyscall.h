// **********************************************************************
//
// Copyright (c) 2001
// IONA Technologies, Inc.
// Waltham, MA, USA
//
// All Rights Reserved
//
// **********************************************************************

#ifndef JTC_SYSCALL_H
#define JTC_SYSCALL_H

#include "JTC/JTCException.h"

#ifndef HAVE_JTC_NO_IOSTREAM
#  if defined(HAVE_SSTREAM)
#     include <sstream>
#  else
#     if defined(HAVE_STRSTREA_H)
#         include <strstrea.h>
#     else
#         include <strstream.h>
#     endif
#  endif
#else // HAVE_JTC_NO_IOSTREAM
#  include <stdio.h>
#endif // !HAVE_JTC_NO_IOSTREAM

#include "JTC/JTCException.h"

#ifndef HAVE_JTC_NO_IOSTREAM

#ifndef HAVE_SSTREAM

#  define JTC_THROW_EXCEPTION(CODE,MSG) \
   { \
       long error = CODE; \
       char msg[ 512 ]; \
       msg[sizeof(msg)-1] = '\0'; \
       JTC_STD(ostrstream) stream(msg, sizeof(msg)-1); \
       stream << MSG << " == " << error << " [" << __FILE__ \
              << ':' << __LINE__ << ']' << JTC_STD(ends); \
       throw JTCSystemCallException(msg, error); \
   }

#else // HAVE_SSTREAM

#  define JTC_THROW_EXCEPTION(CODE,MSG) \
   { \
       long error = CODE; \
       JTC_STD(ostringstream) stream; \
       stream << MSG << " == " << error << " [" << __FILE__ \
              << ':' << __LINE__ << ']' << JTC_STD(ends); \
       throw JTCSystemCallException(stream.str().c_str(), error); \
   }

#endif // HAVE_SSTREAM

#else // HAVE_JTC_NO_IOSTREAM

#  define JTC_THROW_EXCEPTION(CODE,MSG) \
   { \
       long error = CODE; \
       const char* file = __FILE__; \
       int line = __LINE__; \
       char msg[ 1024 ]; \
       msg[sizeof(msg)-1] = '\0'; \
       sprintf(msg, "ERRNO %ld [%s:%d]", error, file, line); \
       throw JTCSystemCallException(msg, error); \
   }

#endif // !HAVE_JTC_NO_IOSTREAM

#define JTC_CAT_WNL(ARG) \
  "                " << (ARG) << ", // " << #ARG << "\n"

#define JTC_CAT(ARG) \
  "                " << (ARG) << "  // " << #ARG

#define JTC_FMT_ARG_0 \
  " ()"

#define JTC_FMT_ARG_1(A1) \
  "\n                (\n" << JTC_CAT(A1) << "\n                )"

#define JTC_FMT_ARG_2(A1,A2) \
  "\n                (\n" << JTC_CAT_WNL(A1) << JTC_CAT(A2) \
  << "\n                )"

#define JTC_FMT_ARG_3(A1,A2,A3) \
  "\n                (\n" << JTC_CAT_WNL(A1) << JTC_CAT_WNL(A2) \
  << JTC_CAT(A3) << "\n                )"

#define JTC_FMT_ARG_4(A1,A2,A3,A4) \
  "\n                (\n" << JTC_CAT_WNL(A1) << JTC_CAT_WNL(A2) \
  << JTC_CAT_WNL(A3) << JTC_CAT(A4) << "\n                )"

#define JTC_FMT_ARG_5(A1,A2,A3,A4,A5) \
  "\n                (\n" << JTC_CAT_WNL(A1) << JTC_CAT_WNL(A2) \
  << JTC_CAT_WNL(A3) << JTC_CAT_WNL(A4) << JTC_CAT(A5) << "\n                )"

#define JTC_FMT_ARG_6(A1,A2,A3,A4,A5,A6) \
  "\n                (\n" << JTC_CAT_WNL(A1) << JTC_CAT_WNL(A2) \
  << JTC_CAT_WNL(A3) << JTC_CAT_WNL(A4) << JTC_CAT_WNL(A5) << JTC_CAT(A6) \
  << "\n                )"

#define JTC_FMT_ARG_7(A1,A2,A3,A4,A5,A6,A7) \
  "\n                (\n" << JTC_CAT_WNL(A1) << JTC_CAT_WNL(A2) \
  << JTC_CAT_WNL(A3) << JTC_CAT_WNL(A4) << JTC_CAT_WNL(A5) << JTC_CAT_WNL(A6) \
  << JTC_CAT(A7) << "\n                )"

#define JTC_FMT_ARG_8(A1,A2,A3,A4,A5,A6,A7,A8) \
  "\n                (\n" << JTC_CAT_WNL(A1) << JTC_CAT_WNL(A2) \
  << JTC_CAT_WNL(A3) << JTC_CAT_WNL(A4) << JTC_CAT_WNL(A5) << JTC_CAT_WNL(A6) \
  << JTC_CAT_WNL(A7) << JTC_CAT(A8) << "\n                )"

#define JTC_FMT_ARG_9(A1,A2,A3,A4,A5,A6,A7,A8,A9) \
  "\n                (\n" << JTC_CAT_WNL(A1) << JTC_CAT_WNL(A2) \
  << JTC_CAT_WNL(A3) << JTC_CAT_WNL(A4) << JTC_CAT_WNL(A5) << JTC_CAT_WNL(A6) \
  << JTC_CAT_WNL(A7) << JTC_CAT_WNL(A8) << JTC_CAT(A9) << "\n                )"

// TODO JTC_CAT_WNL(A5) 

#define JTC_FMT_ARG_15(A1,A2,A3,A4,A5,A6,A7,A8,A9,A10,A11,A12,A13,A14,A15) \
  "\n                (\n" << JTC_CAT_WNL(A1) << JTC_CAT_WNL(A2) \
  << JTC_CAT_WNL(A3) << JTC_CAT_WNL(A4) \
  << JTC_CAT_WNL(A6) << JTC_CAT_WNL(A7) << JTC_CAT_WNL(A8) \
  << JTC_CAT_WNL(A9) << JTC_CAT_WNL(A10) << JTC_CAT_WNL(A11) \
  << JTC_CAT_WNL(A12) << JTC_CAT_WNL(A13) << JTC_CAT_WNL(A14) \
  << JTC_CAT(A15) << "\n                )"

#if defined(WIN32)
#   define ERRNO GetLastError()
#   define DECLARE_ERRNO
#   define ASSIGN_ERRNO
#else
#   if defined(__hpux) && defined(HAVE_DCE_THREADS)
#       define ERRNO errno
#   else
#       define ERRNO _jtc_syscallError
#   endif
#   define DECLARE_ERRNO int _jtc_syscallError;
#   define ASSIGN_ERRNO _jtc_syscallError =
#endif

#define JTC_SYSCALL_0(FN,COND) \
do { \
    DECLARE_ERRNO \
    if ((ASSIGN_ERRNO (FN())) COND) \
        JTC_THROW_EXCEPTION( ERRNO, #FN << JTC_FMT_ARG_0 ) \
} while (0);

#define JTC_SYSCALL_1(FN,A1,COND) \
{ \
    DECLARE_ERRNO \
    if ((ASSIGN_ERRNO (FN (A1))) COND) \
        JTC_THROW_EXCEPTION( ERRNO, #FN << JTC_FMT_ARG_1(A1) ) \
} while (0);

#define JTC_SYSCALL_2(FN,A1,A2,COND) \
do { \
    DECLARE_ERRNO \
    if ((ASSIGN_ERRNO (FN (A1,A2))) COND) \
        JTC_THROW_EXCEPTION( ERRNO, #FN << JTC_FMT_ARG_2(A1,A2) ) \
} while (0);

#define JTC_SYSCALL_3(FN,A1,A2,A3,COND) \
do { \
    DECLARE_ERRNO \
    if ((ASSIGN_ERRNO (FN (A1,A2,A3))) COND) \
        JTC_THROW_EXCEPTION( ERRNO, #FN << JTC_FMT_ARG_3(A1,A2,A3) ) \
} while (0);

#define JTC_SYSCALL_4(FN,A1,A2,A3,A4,COND) \
do { \
    DECLARE_ERRNO \
    if ((ASSIGN_ERRNO (FN (A1,A2,A3,A4))) COND) \
        JTC_THROW_EXCEPTION( ERRNO, #FN << JTC_FMT_ARG_4(A1,A2,A3,A4) ) \
} while (0);

#define JTC_SYSCALL_5(FN,A1,A2,A3,A4,A5,COND) \
do { \
    DECLARE_ERRNO \
    if ((ASSIGN_ERRNO (FN (A1,A2,A3,A4,A5))) COND) \
        JTC_THROW_EXCEPTION( ERRNO, #FN << JTC_FMT_ARG_5(A1,A2,A3,A4,A5) ) \
} while (0);

#define JTC_SYSCALL_6(FN,A1,A2,A3,A4,A5,A6,COND) \
do { \
    DECLARE_ERRNO \
    if ((ASSIGN_ERRNO (FN (A1,A2,A3,A4,A5,A6))) COND) \
        JTC_THROW_EXCEPTION( ERRNO, #FN << JTC_FMT_ARG_6(A1,A2,A3,A4,A5,A6) ) \
} while (0);

#define JTC_SYSCALL_7(FN,A1,A2,A3,A4,A5,A6,A7,COND) \
do { \
    DECLARE_ERRNO \
    if ((ASSIGN_ERRNO (FN (A1,A2,A3,A4,A5,A6,A7))) COND) \
        JTC_THROW_EXCEPTION( ERRNO, #FN << \
			     JTC_FMT_ARG_7(A1,A2,A3,A4,A5,A6,A7) ) \
} while (0);


#define JTC_SYSCALL_8(FN,A1,A2,A3,A4,A5,A6,A7,A8,COND) \
do { \
    DECLARE_ERRNO \
    if ((ASSIGN_ERRNO (FN (A1,A2,A3,A4,A5,A6,A7,A8))) COND) \
        JTC_THROW_EXCEPTION( ERRNO, #FN << \
			     JTC_FMT_ARG_8(A1,A2,A3,A4,A5,A6,A7,A8) ) \
} while (0);


#define JTC_SYSCALL_9(FN,A1,A2,A3,A4,A5,A6,A7,A8,A9,COND) \
do { \
    DECLARE_ERRNO \
    if ((ASSIGN_ERRNO (FN (A1,A2,A3,A4,A5,A6,A7,A8,A9))) COND) \
        JTC_THROW_EXCEPTION( ERRNO, #FN << \
			     JTC_FMT_ARG_9(A1,A2,A3,A4,A5,A6,A7,A8,A9) ) \
} while (0);

#define JTC_SYSCALL_15(FN,A1,A2,A3,A4,A5,A6,A7,A8,A9,A10, \
                       A11,A12,A13,A14,A15, COND) \
do { \
    DECLARE_ERRNO \
    if ((ASSIGN_ERRNO (FN (A1,A2,A3,A4,A5,A6,A7,A8,A9,A10, \
                       A11,A12,A13,A14,A15))) COND) \
        JTC_THROW_EXCEPTION( ERRNO, #FN << \
			     JTC_FMT_ARG_15(A1,A2,A3,A4,A5,A6,A7,A8,A9,A10, \
                                           A11,A12,A13,A14,A15) ) \
} while (0);

#endif
