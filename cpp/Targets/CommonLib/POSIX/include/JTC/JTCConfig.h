/* include/JTC/Config.h.  Generated automatically by configure.  */
/* include/JTC/Config.h.in.  Generated automatically from configure.in by autoheader.  */
/***********************************************************************
 *
 * Copyright (c) 2000
 * IONA Technologies, Inc.
 * Waltham, MA, USA
 *
 * All Rights Reserved
 *
 ***********************************************************************/



/* Define if your c++ compiler doesn't know mutable. */
/* #undef HAVE_NO_MUTABLE */

/* Define if your c++ compiler doesn't have namespaces. */
/* #undef HAVE_NO_NAMESPACE */

/* Define if your c++ compiler supports iostream in namespace std. */
#define HAVE_STD_IOSTREAM 1

/* Define if your c++ compiler supports STL in namespace std. */
#define HAVE_STD_STL 1

/* Define if your c++ compiler can explicitly instanciate templates. */
/* #undef HAVE_NO_EXPLICIT_TEMPLATES */

/* Define if your c++ compiler does not have RTTI. */
/* #undef HAVE_NO_RTTI */

/* Define if your c++ compiler can't overload const type conversions. */
/* #undef HAVE_NO_CONST_TYPE_CONVERSION_OVERLOAD */

/* Define if your c++ compiler doesn't have typename. */
/* #undef HAVE_NO_TYPENAME */

/* Define if you don't have a prototype for gethostname(). */
/* #undef HAVE_NO_GETHOSTNAME_PROTOTYPE */

/* Define if socklen_t type is known. */
/* #undef HAVE_SOCKLEN_T */

/* Define if there is no declaration for h_errno */
/* #undef HAVE_NO_H_ERRNO_DECLARATION */

/* Define if socket operations require address length of type size_t. */
/* #undef HAVE_SOCKETS_WITH_SIZE_T */

/* Define if h_errno has a broken declaration. */
/* (HPUX 11.x defines h_errno without an extern "C" block.) */
/* #undef HAVE_BROKEN_H_ERRNO_DECL */

/* Define if OS supports MIT threads. */
/* #undef HAVE_MIT_THREADS */

/* Define if OS supports FSU threads. */
/* #undef HAVE_FSU_THREADS */

/* Define if OS supports posix threads. */
#define HAVE_POSIX_THREADS 1

/* Define if OS supports DCE threads. */
/* #undef HAVE_DCE_THREADS */

/* Define if OS supports pthread_attr_setstacksize. */
#define HAVE_PTHREAD_ATTR_SETSTACKSIZE 1

/* Define if OS supports pthread_delay_np. */
/* #undef HAVE_PTHREAD_DELAY_NP */

/* Define if OS supports pthread_sched_yield. */
#define HAVE_SCHED_YIELD 1

/* Define if OS supports pthread_yield. */
/* #undef HAVE_PTHREAD_YIELD */

/* Define if JThreads/C++ is available. */
/* #undef HAVE_JTC */

/* Define if JThreads/C++ has no support for iostreams. */
/* #undef HAVE_JTC_NO_IOSTREAM */

/* Define if the Event Service source is available. */
/* #undef HAVE_OBEVENT */

/* Define if the Notification Service is available. */
/* #undef HAVE_OBNOTIFY */

/* Define if you have the <iostream> header file.  */
#define HAVE_IOSTREAM 1

/* Define if you have the <sstream> header file.  */
#define HAVE_SSTREAM 1

/* Define if you have the deprecated (!) <strstream> header file.  */
#define HAVE_STRSTREAM 1

/* Define if you have the <fstream> header file.  */
#define HAVE_FSTREAM 1

/* Define if you have the <bstring.h> header file. */
/* #undef HAVE_BSTRING_H */

/* Define if you have the <exception> header file. */
#define HAVE_EXCEPTION 1

/* Define if you have the <exception.h> header file. */
/* #undef HAVE_EXCEPTION_H */

/* Define if you have the <fstream> header file. */
#define HAVE_FSTREAM 1

/* Define if you have the <inttypes.h> header file. */
#define HAVE_INTTYPES_H 1

/* Define if you have the <iomanip> header file. */
#define HAVE_IOMANIP 1

/* Define if you have the <iostream> header file. */
#define HAVE_IOSTREAM 1

/* Define if you have the `malloc' library (-lmalloc). */
/* #undef HAVE_LIBMALLOC */

/* Define if you have the <memory.h> header file. */
#define HAVE_MEMORY_H 1

/* Define if you have the <new> header file. */
#define HAVE_NEW 1

/* Define if you have the <sched.h> header file. */
#define HAVE_SCHED_H 1

/* Define if you have the <sstream> header file. */
#define HAVE_SSTREAM 1

/* Define if you have the <stddef.h> header file. */
#define HAVE_STDDEF_H 1

/* Define if you have the <stdint.h> header file. */
#define HAVE_STDINT_H 1

/* Define if you have the <stdlib.h> header file. */
#define HAVE_STDLIB_H 1

/* Define if you have the `strcasecmp' function. */
#define HAVE_STRCASECMP 1

/* Define if you have the `strerror' function. */
#define HAVE_STRERROR 1

/* Define if you have the <strings.h> header file. */
#define HAVE_STRINGS_H 1

/* Define if you have the <string.h> header file. */
#define HAVE_STRING_H 1

/* Define if you have the `strncasecmp' function. */
#define HAVE_STRNCASECMP 1

/* Define if you have the <strstream> header file. */
#define HAVE_STRSTREAM 1

/* Define if you have the <sys/sched.h> header file. */
/* #undef HAVE_SYS_SCHED_H */

/* Define if you have the <sys/select.h> header file. */
#define HAVE_SYS_SELECT_H 1

/* Define if you have the <sys/stat.h> header file. */
#define HAVE_SYS_STAT_H 1

/* Define if you have the <sys/time.h> header file. */
#define HAVE_SYS_TIME_H 1

/* Define if you have the <sys/types.h> header file. */
#define HAVE_SYS_TYPES_H 1

/* Define if you have the <terminate.h> header file. */
/* #undef HAVE_TERMINATE_H */

/* Define if you have the <termios.h> header file. */
#define HAVE_TERMIOS_H 1

/* Define if you have the <termio.h> header file. */
#define HAVE_TERMIO_H 1

/* Define if you have the <unexpected.h> header file. */
/* #undef HAVE_UNEXPECTED_H */

/* Define if you have the <unistd.h> header file. */
#define HAVE_UNISTD_H 1

/* The size of a `bool', as computed by sizeof. */
#define SIZEOF_BOOL 1

/* The size of a `char', as computed by sizeof. */
#define SIZEOF_CHAR 1

/* The size of a `double', as computed by sizeof. */
#define SIZEOF_DOUBLE 8

/* The size of a `float', as computed by sizeof. */
#define SIZEOF_FLOAT 4

/* The size of a `int', as computed by sizeof. */
#define SIZEOF_INT 4

/* The size of a `long', as computed by sizeof. */
#define SIZEOF_LONG 4

/* The size of a `long long', as computed by sizeof. */
#define SIZEOF_LONG_LONG 8

/* The size of a `short', as computed by sizeof. */
#define SIZEOF_SHORT 2

/* Define if you have the ANSI C header files. */
#define STDC_HEADERS 1

/* Define if your processor stores words with the most significant byte first
   (like Motorola and SPARC, unlike Intel and VAX). */
/* #undef WORDS_BIGENDIAN */

/* Define if `lex' declares `yytext' as a `char *' by default, not a `char[]'.
   */
#define YYTEXT_POINTER 1

/* Define if you need to in order for stat and other things to work. */
/* #undef _POSIX_SOURCE */
