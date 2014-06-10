/*---------------------------------------------------------------------------
    base.h - iMatix base header file

    Defines the standard preprocessing environment for C development.  Also
    defines a set of portability macros for functionality that is not part
    of the APR layer we are using for most portability.

    This file is largely equivalent to the prelude.h file provided by the
    (deprecated) iMatix SFL library.

    It does the following:

    - defines a standard set of platform macros
    - includes standard ANSI C library headers
    - includes most commonly used system headers
    - defines a set of useful macros such as streq and FOREVER
    - 'fixes' various older platforms to make them more POSIX compatible

     Copyright (c) 1996-2009 iMatix Corporation

    This program is free software; you can redistribute it and/or modify
    it under the terms of the GNU General Public License as published by
    the Free Software Foundation; either version 2 of the License, or (at
    your option) any later version.

    This program is distributed in the hope that it will be useful, but
    WITHOUT ANY WARRANTY; without even the implied warranty of
    MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
    General Public License for more details.

    For information on alternative licensing for OEMs, please contact
    iMatix Corporation.
*---------------------------------------------------------------------------*/

#ifndef INCLUDE_IMATIX_BASE
#define INCLUDE_IMATIX_BASE

/*  The first part of this file is a subset of the old SFL prelude file
    without the networking capabilities.  We include it conditionally so
    that calling applications can choose to include sfl.h first if wanted.

    IMPORTANT: any new definitions go after the END_PRELUDE comment.
 */

#ifndef PRELUDE_INCLUDED               //  Allow multiple inclusions
#define PRELUDE_INCLUDED


//- Establish the compiler and computer system ------------------------------
/*
 *  Defines zero or more of these symbols, for use in any non-portable
 *  code:
 *
 *  __WINDOWS__         Microsoft C/C++ with Windows calls
 *  __MSDOS__           System is MS-DOS (set if __WINDOWS__ set)
 *  __VMS__             System is VAX/VMS or Alpha/OpenVMS
 *  __UNIX__            System is UNIX
 *  __OS2__             System is OS/2
 *
 *  __IS_32BIT__        OS/compiler is 32 bits
 *  __IS_64BIT__        OS/compiler is 64 bits
 *
 *  When __UNIX__ is defined, we also define exactly one of these:
 *
 *  __UTYPE_AUX         Apple AUX
 *  __UTYPE_BEOS        BeOS
 *  __UTYPE_BSDOS       BSD/OS
 *  __UTYPE_DECALPHA    Digital UNIX (Alpha)
 *  __UTYPE_IBMAIX      IBM RS/6000 AIX
 *  __UTYPE_FREEBSD     FreeBSD
 *  __UTYPE_HPUX        HP/UX
 *  __UTYPE_LINUX       Linux
 *  __UTYPE_MIPS        MIPS (BSD 4.3/System V mixture)
 *  __UTYPE_NETBSD      NetBSD
 *  __UTYPE_NEXT        NeXT
 *  __UTYPE_OPENBSD     OpenBSD
 *  __UTYPE_OSX         Apple Macintosh OS X
 *  __UTYPE_QNX         QNX
 *  __UTYPE_IRIX        Silicon Graphics IRIX
 *  __UTYPE_SINIX       SINIX-N (Siemens-Nixdorf Unix)
 *  __UTYPE_SUNOS       SunOS
 *  __UTYPE_SUNSOLARIS  Sun Solaris
 *  __UTYPE_UNIXWARE    SCO UnixWare
 *                      ... these are the ones I know about so far.
 *  __UTYPE_GENERIC     Any other UNIX
 *
 *  When __VMS__ is defined, we may define one or more of these:
 *
 *  __VMS_XOPEN         Supports XOPEN functions
 */

#if (defined (__64BIT__) || defined (__x86_64__))
#    define __IS_64BIT__                //  May have 64-bit OS/compiler
#else
#    define __IS_32BIT__                //  Else assume 32-bit OS/compiler
#endif

#if (defined WIN32 || defined _WIN32)
#   undef __WINDOWS__
#   define __WINDOWS__
#   undef __MSDOS__
#   define __MSDOS__
#endif

#if (defined WINDOWS || defined _WINDOWS || defined __WINDOWS__)
#   undef __WINDOWS__
#   define __WINDOWS__
#   undef __MSDOS__
#   define __MSDOS__
#   if _MSC_VER == 1500
#       ifndef _CRT_SECURE_NO_DEPRECATE
#           define _CRT_SECURE_NO_DEPRECATE   1
#       endif
#       pragma warning(disable: 4996)
#   endif
#endif

//  MSDOS               Microsoft C
//  _MSC_VER            Microsoft C
#if (defined (MSDOS) || defined (_MSC_VER))
#   undef __MSDOS__
#   define __MSDOS__
#   if (defined (_DEBUG) && !defined (DEBUG))
#       define DEBUG
#   endif
#endif

#if (defined (__EMX__) && defined (__i386__))
#   undef __OS2__
#   define __OS2__
#endif

//  VMS                 VAX C (VAX/VMS)
//  __VMS               Dec C (Alpha/OpenVMS)
//  __vax__             gcc
#if (defined (VMS) || defined (__VMS) || defined (__vax__))
#   undef __VMS__
#   define __VMS__
#   if (__VMS_VER >= 70000000)
#       define __VMS_XOPEN
#   endif
#endif

//  Try to define a __UTYPE_xxx symbol...
//  unix                SunOS at least
//  __unix__            gcc
//  _POSIX_SOURCE is various UNIX systems, maybe also VAX/VMS
#if (defined (unix) || defined (__unix__) || defined (_POSIX_SOURCE))
#   if (!defined (__VMS__))
#       undef __UNIX__
#       define __UNIX__
#       if (defined (__alpha))          //  Digital UNIX is 64-bit
#           undef  __IS_32BIT__
#           define __IS_64BIT__
#           define __UTYPE_DECALPHA
#       endif
#   endif
#endif

#if (defined (_AUX))
#   define __UTYPE_AUX
#   define __UNIX__
#elif (defined (__BEOS__))
#   define __UTYPE_BEOS
#   define __UNIX__
#elif (defined (__hpux))
#   define __UTYPE_HPUX
#   define __UNIX__
#   define _INCLUDE_HPUX_SOURCE
#   define _INCLUDE_XOPEN_SOURCE
#   define _INCLUDE_POSIX_SOURCE
#elif (defined (_AIX) || defined (AIX))
#   define __UTYPE_IBMAIX
#   define __UNIX__
#elif (defined (BSD) || defined (bsd))
#   define __UTYPE_BSDOS
#   define __UNIX__
#elif (defined (APPLE) || defined (__APPLE__))
#   define __UTYPE_GENERIC
#   define __UNIX__
#elif (defined (linux))
#   define __UTYPE_LINUX
#   define __UNIX__
#   ifndef __NO_CTYPE
#   define __NO_CTYPE                   //  Suppress warnings on tolower()
#   endif
#elif (defined (Mips))
#   define __UTYPE_MIPS
#   define __UNIX__
#elif (defined (FreeBSD) || defined (__FreeBSD__))
#   define __UTYPE_FREEBSD
#   define __UNIX__
#elif (defined (NetBSD) || defined (__NetBSD__))
#   define __UTYPE_NETBSD
#   define __UNIX__
#elif (defined (OpenBSD) || defined (__OpenBSD__))
#   define __UTYPE_OPENBSD
#   define __UNIX__
#elif (defined (NeXT))
#   define __UTYPE_NEXT
#   define __UNIX__
#elif (defined (__QNX__))
#   define __UTYPE_QNX
#   define __UNIX__
#elif (defined (sgi))
#   define __UTYPE_IRIX
#   define __UNIX__
#elif (defined (sinix))
#   define __UTYPE_SINIX
#   define __UNIX__
#elif (defined (SOLARIS) || defined (__SRV4))
#   define __UTYPE_SUNSOLARIS
#   define __UNIX__
#elif (defined (SUNOS) || defined (SUN) || defined (sun))
#   define __UTYPE_SUNOS
#   define __UNIX__
#elif (defined (__USLC__) || defined (UnixWare))
#   define __UTYPE_UNIXWARE
#   define __UNIX__
#elif (defined (__CYGWIN__))
#   define __UTYPE_CYGWIN
#   define __UNIX__
#elif (defined (__UNIX__))
#   define __UTYPE_GENERIC
#endif

//- Standard ANSI include files ---------------------------------------------

#include <ctype.h>
#include <limits.h>
#include <stdarg.h>
#include <stdio.h>
#include <stdlib.h>
#include <stddef.h>
#include <string.h>
#include <time.h>
#include <errno.h>
#include <float.h>
#include <math.h>
#include <signal.h>
#include <setjmp.h>
#include <assert.h>


//- System-specific include files -------------------------------------------

#if (defined (__MSDOS__))
#   if (defined (__WINDOWS__))          //  When __WINDOWS__ is defined,
#       if (defined (FD_SETSIZE))
#           error "winsock.h must not be included before base.h"
#       endif
#       define FD_SETSIZE     1024      //  Max. filehandles/sockets
#       include <direct.h>
#       include <windows.h>
#       include <winsock.h>             //  May cause trouble on VC 1.x
#       include <process.h>
#   endif
#   include <malloc.h>
#   include <dos.h>
#   include <io.h>
#   include <fcntl.h>
#   include <sys\types.h>
#   include <sys\stat.h>
#   include <sys\utime.h>
#   include <share.h>
#   if _MSC_VER == 1500
#       ifndef _CRT_SECURE_NO_DEPRECATE
#           define _CRT_SECURE_NO_DEPRECATE   1
#       endif
#       pragma warning(disable: 4996)
#   endif
#endif

#if (defined (__UNIX__))
#   if defined (__GNUC__) && (__GNUC__ >= 2)
#       define __STRICT_ANSI__
#   endif
#   include <fcntl.h>
#   include <netdb.h>
#   include <unistd.h>
#   include <dirent.h>
#   include <pwd.h>
#   include <grp.h>
#   include <utime.h>
#   include <syslog.h>
#   include <sys/types.h>
#   include <sys/param.h>
#   include <sys/socket.h>
#   include <sys/time.h>
#   include <sys/stat.h>
#   include <sys/ioctl.h>
#   include <sys/file.h>
#   include <sys/wait.h>
#   include <netinet/in.h>              //  Must come before arpa/inet.h
#   if (!defined (__UTYPE_BEOS))
#       include <arpa/inet.h>
#       if (!defined (TCP_NODELAY))
#           include <netinet/tcp.h>
#       endif
#   endif
#   if (defined (__UTYPE_IBMAIX) || defined(__UTYPE_QNX))
#       include <sys/select.h>
#   endif
#   if (defined (__UTYPE_BEOS))
#       include <NetKit.h>
#   endif
#   if ((defined (_XOPEN_REALTIME) && (_XOPEN_REALTIME >= 1)) || \
        (defined (_POSIX_VERSION)  && (_POSIX_VERSION  >= 199309L)))
#       include <sched.h>
#   endif
#   if (defined (__UTYPE_OSX))
#       include <crt_externs.h>         /* For _NSGetEnviron()               */
#   endif
#endif

#if (defined (__VMS__))
#   if (!defined (vaxc))
#       include <fcntl.h>               //  Not provided by Vax C
#   endif
#   include <netdb.h>
#   include <unistd.h>
#   include <unixio.h>
#   include <unixlib.h>
#   include <types.h>
#   include <file.h>
#   include <socket.h>
#   include <dirent.h>
#   include <time.h>
#   include <pwd.h>
#   include <stat.h>
#   include <in.h>
#   include <inet.h>
#endif

#if (defined (__OS2__))
#   include <sys/types.h>               //  Required near top
#   include <fcntl.h>
#   include <malloc.h>
#   include <netdb.h>
#   include <unistd.h>
#   include <dirent.h>
#   include <pwd.h>
#   include <grp.h>
#   include <io.h>
#   include <process.h>
#   include <sys/param.h>
#   include <sys/socket.h>
#   include <sys/select.h>
#   include <sys/time.h>
#   include <sys/stat.h>
#   include <sys/ioctl.h>
#   include <sys/file.h>
#   include <sys/wait.h>
#   include <netinet/in.h>              //  Must come before arpa/inet.h
#   include <arpa/inet.h>
#   include <utime.h>
#   if (!defined (TCP_NODELAY))
#       include <netinet/tcp.h>
#   endif
#endif


//- Check compiler data type sizes ------------------------------------------

#if (UCHAR_MAX != 0xFF)
#   error "Cannot compile: must change definition of 'byte'."
#endif
#if (USHRT_MAX != 0xFFFFU)
#    error "Cannot compile: must change definition of 'dbyte'."
#endif
#if (UINT_MAX != 0xFFFFFFFFU)
#    error "Cannot compile: must change definition of 'qbyte'."
#endif


//- Data types --------------------------------------------------------------

typedef          int    Bool;           //  Boolean TRUE/FALSE value
typedef unsigned char   byte;           //  Single unsigned byte = 8 bits
typedef unsigned short  dbyte;          //  Double byte = 16 bits
typedef unsigned int    qbyte;          //  Quad byte = 32 bits

/*  The following structures are needed to invoke certain SFL
    modules, and can be removed once SFL is completely replaced.
    -- PH 2005/03/12
 */
typedef unsigned short  word;           //  Alternative for double-byte
typedef unsigned long   dword;          //  Double word >= 32 bits
typedef void (*function) (void);        //  Address of simple function

//- Pseudo-functions --------------------------------------------------------

#define FOREVER             for (;;)            //  FOREVER { ... }
#define until(expr)         while (!(expr))     //  do { ... } until (expr)
#define streq(s1,s2)        (!strcmp ((s1), (s2)))
#define strneq(s1,s2)       (strcmp ((s1), (s2)))
#define strused(s)          (*(s) != 0)
#define strnull(s)          (*(s) == 0)
#define strclr(s)           (*(s) = 0)
#define strlast(s)          ((s) [strlen (s) - 1])
#define strterm(s)          ((s) [strlen (s)])

#define tblsize(x)          (sizeof (x) / sizeof ((x) [0]))
#define tbllast(x)          (x [tblsize (x) - 1])

#define randomof(num)       (int) (((float) num) * rand () / (RAND_MAX + 1.0))
#define randomize()         srand ((uint) apr_time_usec (apr_time_now ()))

#if (!defined (MIN))
#   define MIN(a,b)         (((a) < (b))? (a): (b))
#   define MAX(a,b)         (((a) > (b))? (a): (b))
#endif

//- Assertion that pointer value is as expect -------------------------------

#define assert_eq(value,const) \
if ((value) != (const)) {\
    printf ("Assertion failed, expected=%d actual=%d", (const), (value));\
    assert ((value) == (const));\
}

//- Boolean operators and constants -----------------------------------------

#if (!defined (TRUE))
#    define TRUE        1               //  ANSI standard
#    define FALSE       0
#endif


//- Symbolic constants ------------------------------------------------------

#if (!defined (EXIT_SUCCESS))           //  ANSI, and should be in stdlib.h
#   define EXIT_SUCCESS 0               //    but not defined on SunOs with
#   define EXIT_FAILURE 1               //    GCC, sometimes.
#endif


//- Lock status enumerated type ---------------------------------------------

typedef enum {
    ICL_LOCK_NONE,
    ICL_LOCK_READ,
    ICL_LOCK_WRITE
} icl_lock_t;

//- System-specific definitions ---------------------------------------------
/*  Most of these are to bring older systems up to an acceptable POSIX or
    C99 standard.
 */

//  A number of C99 keywords and data types
#if (defined (__WINDOWS__))
#   define inline __inline
#   define strtoull _strtoui64
    typedef unsigned long ulong;
    typedef unsigned int  uint;
    typedef __int64 int64_t;
#elif (defined (__VMS__))
    typedef __int64 int64_t;
#endif

/*  On most systems, 'timezone' is an external long variable.  On a few, it
 *  is a function that returns a string.  We define TIMEZONE to be the long
 *  value.                                                                   */

#undef  TIMEZONE
#if (defined (__WINDOWS__) || defined (__CYGWIN__))
#   define TIMEZONE   _timezone
#else
#   define TIMEZONE    timezone         //  Unless redefined later
#endif

/*  UNIX defines sleep() in terms of second; Win32 defines Sleep() in
 *  terms of milliseconds.  We want to be able to use sleep() anywhere.      */

#if (defined (__WINDOWS__))
#   undef sleep
#   if (defined (WIN32))
#       define sleep(a) Sleep(a*1000)   //  UNIX sleep() is seconds
#   else
#       define sleep(a)                 //  Do nothing?
#   endif

/*  On SunOs, the ANSI C compiler costs extra, so many people install gcc
 *  but using the standard non-ANSI C library.  We have to make a few extra
 *  definitions for this case.  (Here we defined just what we needed for
 *  Libero and SMT -- we'll add more code as required.)                      */

#elif (defined (__UTYPE_SUNOS) || defined (__UTYPE_SUNSOLARIS))
#   if (!defined (_SIZE_T))             //  Non-ANSI headers/libraries
#       define strerror(n)      sys_errlist [n]
#       define memmove(d,s,l)   bcopy (s,d,l)
        extern char *sys_errlist [];
#   endif

#elif (defined (__UTYPE_BSDOS) || defined (__UTYPE_FREEBSD) \
    || defined (__UTYPE_NETBSD) || defined (__UTYPE_OPENBSD) \
    || defined (__UTYPE_OSX))
#   undef TIMEZONE
#endif

#if (defined (__UNIX__) || defined (__VMS__))
#   if (defined (__UTYPE_OSX))
#       define environ (* _NSGetEnviron())
#   else
        extern char **environ;          /*  Not defined in include files     */
#   endif
#endif

//  On some systems O_NDELAY is used instead of O_NONBLOCK

#if (!defined (O_NONBLOCK))
#   if (!defined (O_NDELAY))
#       define O_NDELAY 0
#   endif
#   if (defined (__VMS__))
#       define O_NONBLOCK 0             //  Can't use O_NONBLOCK on files
#   else
#       define O_NONBLOCK O_NDELAY
#   endif
#endif

/*  We define constants for the way the current system formats filenames;
 *  we assume that the system has some type of path concept.                 */

#if (defined (WIN32))                   //  Windows 95/NT
#   define PATHSEP      ";"             //  Separates path components
#   define PATHEND      '\\'            //  Delimits directory and filename
#   define PATHFOLD     FALSE           //  Convert pathvalue to uppercase?
#   define NAMEFOLD     FALSE           //  Convert filenames to uppercase?
#   define GATES_FILESYSTEM             //  MS-DOS derivative
#elif (defined (__MSDOS__))             //  16-bit Windows, MS-DOS
#   define PATHSEP      ";"
#   define PATHEND      '\\'
#   if defined LFN                      //  Support DRDOS long file names
#       define PATHFOLD FALSE
#       define NAMEFOLD FALSE
#   else
#       define PATHFOLD TRUE
#       define NAMEFOLD TRUE
#   endif
#   define GATES_FILESYSTEM             //  MS-DOS derivative
#elif (defined (__VMS__))               //  Digital OpenVMS
#   define PATHSEP      ","             //    We work with POSIX filenames
#   define PATHEND      '/'
#   define PATHFOLD     TRUE
#   define NAMEFOLD     TRUE
#elif (defined (__UNIX__))              //  All UNIXes
#   define PATHSEP      ":"
#   define PATHEND      '/'
#   define PATHFOLD     FALSE
#   define NAMEFOLD     FALSE
#elif (defined (__OS2__))               //  OS/2 using EMX/GCC
#   define PATHSEP      ";"             //  EDM 96/05/28
#   define PATHEND      '\\'
#   define PATHFOLD     TRUE
#   define NAMEFOLD     FALSE
#   define GATES_FILESYSTEM             //  MS-DOS derivative
#else
#   error "No definitions for PATH constants"
#endif


//- Capability definitions --------------------------------------------------
/*
 *  Defines zero or more of these symbols, for use in any non-portable
 *  code:
 *
 *  DOES_SOCKETS         We can use (at least some) BSD socket functions
 *  DOES_UID             We can use (at least some) uid access functions
 *  DOES_BSDSIGNALS      Supports BSD signal model (e.g. siginterrupt)
 *  DOES_SETENV          Supports setenv/unsetenv (else we provide our own)
 */

#if (defined (AF_INET))
#   define DOES_SOCKETS                 //  System supports BSD sockets
#else
#   undef  DOES_SOCKETS
#endif

#if (defined (__UNIX__))
#   define DOES_UID                     //  System supports uid functions
#else
#   undef  DOES_UID
    typedef int gid_t;                  //  Group id type
    typedef int uid_t;                  //  User id type
#endif

#if (defined (__OS2__) || defined (__UTYPE_SCO) || defined (__UTYPE_LINUX) \
 || defined(__UTYPE_OSX) ||  defined (__CYGWIN__))
#   define DOES_SNPRINTF
#elif (defined (__WINDOWS__))
#   define DOES_SNPRINTF
#   define snprintf  _snprintf
#   define vsnprintf _vsnprintf
#else
#   undef DOES_SNPRINTF
#endif

//  SunOS 5 (Solaris) does not support BSD-style signal handling
#if (!defined (__UTYPE_SUNSOLARIS))
#   define DOES_BSDSIGNALS
#else
#   undef  DOES_BSDSIGNALS
#endif


//- Error reporting ---------------------------------------------------------
// If the compiler is GCC or supports C99, include enclosing function
// in iCL assertions
#if defined (__STDC_VERSION__) && (__STDC_VERSION__ >= 199901L)
#   define ICL_ASSERT_SANE_FUNCTION    __func__
#elif defined (__GNUC__) && (__GNUC__ >= 2)
#   define ICL_ASSERT_SANE_FUNCTION    __FUNCTION__
#else
#   define ICL_ASSERT_SANE_FUNCTION    "<unknown>"
#endif

// Safe replacement for malloc() which asserts if we run out of heap
static inline void *
    safe_malloc (
    size_t size,
    char *file,
    unsigned line,
    const char *func)
{
    void
        *mem;

    mem = malloc (size);
    if (mem == NULL) {
        fprintf (stderr, "FATAL ERROR at %s:%u, in %s\n", file, line, func);
        fprintf (stderr, "OUT OF MEMORY (malloc returned NULL)\n");
        fflush (stderr);
        abort ();
    }
    return mem;
}

#define malloc(size) safe_malloc(size, __FILE__, __LINE__, ICL_ASSERT_SANE_FUNCTION)

#endif                                  //  END_PRELUDE

#endif
/*---------------------------------------------------------------------------
    icl_apr.h - icl_apr component

    Provides iCL wrappers for some APR functions.
    Generated from icl_apr.icl by icl_gen using GSL/4.
    
    Copyright (c) 1996-2009 iMatix Corporation
    All rights reserved.
    
    This file is licensed under the BSD license as follows:
    
    Redistribution and use in source and binary forms, with or without
    modification, are permitted provided that the following conditions
    are met:
    
    * Redistributions of source code must retain the above copyright
      notice, this list of conditions and the following disclaimer.
    * Redistributions in binary form must reproduce the above copyright
      notice, this list of conditions and the following disclaimer in
      the documentation and/or other materials provided with the
      distribution.
    * Neither the name of iMatix Corporation nor the names of its
      contributors may be used to endorse or promote products derived
      from this software without specific prior written permission.
    
    THIS SOFTWARE IS PROVIDED BY IMATIX CORPORATION "AS IS" AND ANY
    EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE
    IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR
    PURPOSE ARE DISCLAIMED. IN NO EVENT SHALL IMATIX CORPORATION BE
    LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR
    CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF
    SUBSTITUTE GOODS OR SERVICES; LOSS OF USE, DATA, OR PROFITS; OR
    BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY OF
    LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT (INCLUDING
    NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF THIS
    SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.

    These header files use macros to implement a split inclusion in which all
    safe definitions (those that do not depend on the presence of other
    definitions) are done first, and all unsafe definitions are done in a
    second pass through the same headers.

    The first header file included from the main C program defines itself as
    the "root" and thus controls the inclusion of the safe/unsafe pieces of
    the other headers.
 *---------------------------------------------------------------------------*/

#if !defined (ICL_IMPORT_HEADERS) || (ICL_IMPORT_HEADERS == 1)
# ifndef INCLUDE_ICL_APR_SAFE
#   define INCLUDE_ICL_APR_SAFE
#   define INCLUDE_ICL_APR_ACTIVE
#   if !defined (ICL_IMPORT_HEADERS)
#       define ICL_IMPORT_ICL_APR_ROOT
#       define ICL_IMPORT_HEADERS 1
#   endif
#   ifdef __cplusplus
extern "C" {
#   endif

#include "base_apr.h"

//  Macros                                                                     

#if (defined (BASE_THREADSAFE))
#define icl_os_thread_t apr_os_thread_t
#else
#define icl_os_thread_t qbyte
#endif

//  Global variables                                                           

extern apr_pool_t *
    icl_global_pool;
extern apr_allocator_t *
    icl_global_allocator;

#if (defined (BASE_THREADSAFE))
extern apr_thread_mutex_t *
    icl_global_mutex;
extern apr_thread_mutex_t *
    icl_global_allocator_mutex;
#endif
#   ifdef __cplusplus
}
#   endif

#   ifdef __cplusplus
extern "C" {
#   endif

#   ifdef __cplusplus
}
#   endif
#   undef INCLUDE_ICL_APR_ACTIVE
#   if defined (ICL_IMPORT_ICL_APR_ROOT)
#       undef  ICL_IMPORT_HEADERS
#       define ICL_IMPORT_HEADERS 2
#   endif
# endif
#endif

#if !defined (ICL_IMPORT_HEADERS) || (ICL_IMPORT_HEADERS == 2)
# ifndef INCLUDE_ICL_APR_UNSAFE
#   define INCLUDE_ICL_APR_UNSAFE
#   define INCLUDE_ICL_APR_ACTIVE


#   ifdef __cplusplus
extern "C" {
#   endif
//  Global variables

extern Bool
    icl_apr_animating;
#   ifdef __cplusplus
}
#   endif
#   ifdef __cplusplus
extern "C" {
#   endif
//  Method prototypes
#define icl_apr_assert(status)          icl_apr_assert_ (status, __FILE__, __LINE__)
static inline void
    icl_apr_assert_ (
apr_status_t status,                    //  The status code to print
char * file,                            //  Source file for call
size_t line                             //  Line number for call
);

#define icl_apr_error_print(status)     icl_apr_error_print_ (status, __FILE__, __LINE__)
void
    icl_apr_error_print_ (
apr_status_t status,                    //  The status code to print
char * file,                            //  Source file for call
size_t line                             //  Line number for call
);

static inline qbyte
    icl_atomic_inc32 (
volatile qbyte * mem                    //  The memory address
);

static inline Bool
    icl_atomic_dec32 (
volatile qbyte * mem                    //  The memory address
);

static inline qbyte
    icl_atomic_get32 (
volatile qbyte * mem                    //  The memory address
);

static inline void
    icl_atomic_set32 (
volatile qbyte * mem,                   //  The memory address
qbyte val                               //  The value
);

static inline qbyte
    icl_atomic_add32 (
volatile qbyte * mem,                   //  The memory address
qbyte add_val                           //  The value to add
);

static inline void
    icl_atomic_sub32 (
volatile qbyte * mem,                   //  The memory address
qbyte val                               //  The value to subtract
);

static inline qbyte
    icl_atomic_cas32 (
volatile qbyte * mem,                   //  The memory address
qbyte new_val,                          //  The new value
qbyte cmp_val                           //  The value to compare with
);

static inline volatile void *
    icl_atomic_casptr (
volatile void ** mem,                   //  The memory address
void * new_val,                         //  The new value
void * cmp_val                          //  The value to compare with
);

static inline qbyte
    icl_atomic_swap (
volatile qbyte * mem,                   //  The memory address
qbyte new_val                           //  The new value
);

void
    icl_selftest (
void);

void
    icl_show_animation (
Bool enabled                            //  Are we enabling or disabling animation?
);


#   ifdef __cplusplus
}
#   endif
#   undef INCLUDE_ICL_APR_ACTIVE
#   if defined (ICL_IMPORT_ICL_APR_ROOT)
#       undef  ICL_IMPORT_HEADERS
#       define ICL_IMPORT_HEADERS 3
#   endif
# endif
#endif

#if !defined (ICL_IMPORT_HEADERS) || (ICL_IMPORT_HEADERS == 3)
# if !defined (INCLUDE_ICL_APR_INLINE)
#   define INCLUDE_ICL_APR_INLINE
#   define INCLUDE_ICL_APR_ACTIVE


#   ifdef __cplusplus
extern "C" {
#   endif

/*  -------------------------------------------------------------------------
    icl_apr_assert

    Type: Component method
    -------------------------------------------------------------------------
 */

static inline void
    icl_apr_assert_ (
    apr_status_t status,                //  The status code to print
    char * file,                        //  Source file for call
    size_t line                         //  Line number for call
)
{

if (status != APR_SUCCESS) {
    icl_apr_error_print_ (status, file, line);
    abort ();
}
}
/*  -------------------------------------------------------------------------
    icl_atomic_inc32

    Type: Component method
    -------------------------------------------------------------------------
 */

static inline qbyte
    icl_atomic_inc32 (
    volatile qbyte * mem                //  The memory address
)
{
    qbyte
        val;                            //  The old value

#if (defined (BASE_THREADSAFE))
    val = (qbyte) apr_atomic_inc32 (mem);
#else
    val = (*mem)++;
#endif

    return (val);
}
/*  -------------------------------------------------------------------------
    icl_atomic_dec32

    Type: Component method
    -------------------------------------------------------------------------
 */

static inline Bool
    icl_atomic_dec32 (
    volatile qbyte * mem                //  The memory address
)
{
    Bool
        non_zero;                       //  True if value becomes non-zero

#if (defined (BASE_THREADSAFE))
    non_zero = (apr_atomic_dec32 (mem) != 0);
#else
    non_zero = (--(*mem) != 0);
#endif

    return (non_zero);
}
/*  -------------------------------------------------------------------------
    icl_atomic_get32

    Type: Component method
    -------------------------------------------------------------------------
 */

static inline qbyte
    icl_atomic_get32 (
    volatile qbyte * mem                //  The memory address
)
{
    qbyte
        val;                            //  The value

#if (defined (BASE_THREADSAFE))
    val = (qbyte) apr_atomic_read32 (mem);
#else
    val = *mem;
#endif

    return (val);
}
/*  -------------------------------------------------------------------------
    icl_atomic_set32

    Type: Component method
    -------------------------------------------------------------------------
 */

static inline void
    icl_atomic_set32 (
    volatile qbyte * mem,               //  The memory address
    qbyte val                           //  The value
)
{

#if (defined (BASE_THREADSAFE))
    apr_atomic_set32 (mem, val);
#else
    *mem = val;
#endif
}
/*  -------------------------------------------------------------------------
    icl_atomic_add32

    Type: Component method
    -------------------------------------------------------------------------
 */

static inline qbyte
    icl_atomic_add32 (
    volatile qbyte * mem,               //  The memory address
    qbyte add_val                       //  The value to add
)
{
    qbyte
        old_val;                        //  The old value

#if (defined (BASE_THREADSAFE))
    old_val = apr_atomic_add32 (mem, add_val);
#else
    old_val = *mem;
    *mem += add_val;
#endif

    return (old_val);
}
/*  -------------------------------------------------------------------------
    icl_atomic_sub32

    Type: Component method
    -------------------------------------------------------------------------
 */

static inline void
    icl_atomic_sub32 (
    volatile qbyte * mem,               //  The memory address
    qbyte val                           //  The value to subtract
)
{

#if (defined (BASE_THREADSAFE))
    apr_atomic_sub32 (mem, val);
#else
    *mem -= val;
#endif
}
/*  -------------------------------------------------------------------------
    icl_atomic_cas32

    Type: Component method
    -------------------------------------------------------------------------
 */

static inline qbyte
    icl_atomic_cas32 (
    volatile qbyte * mem,               //  The memory address
    qbyte new_val,                      //  The new value
    qbyte cmp_val                       //  The value to compare with
)
{
    qbyte
        old_val;                        //  The old value

#if (defined (BASE_THREADSAFE))
    old_val = apr_atomic_cas32 (mem, new_val, cmp_val);
#else
    old_val = *mem;
    if (*mem == cmp_val)
        *mem = new_val;
#endif

    return (old_val);
}
/*  -------------------------------------------------------------------------
    icl_atomic_casptr

    Type: Component method
    -------------------------------------------------------------------------
 */

static inline volatile void *
    icl_atomic_casptr (
    volatile void ** mem,               //  The memory address
    void * new_val,                     //  The new value
    void * cmp_val                      //  The value to compare with
)
{
    volatile void *
        old_val;                        //  The old value

#if (defined (BASE_THREADSAFE))
    old_val = apr_atomic_casptr (mem, new_val, cmp_val);
#else
    old_val = *mem;
    if (*mem == cmp_val)
        *mem = new_val;
#endif

    return (old_val);
}
/*  -------------------------------------------------------------------------
    icl_atomic_swap

    Type: Component method
    -------------------------------------------------------------------------
 */

static inline qbyte
    icl_atomic_swap (
    volatile qbyte * mem,               //  The memory address
    qbyte new_val                       //  The new value
)
{
    qbyte
        old_val;                        //  The old value

#if (defined (BASE_THREADSAFE))
    old_val = apr_atomic_xchg32 (mem, new_val);
#else
    old_val = *mem;
    *mem = new_val;
#endif

    return (old_val);
}
#   ifdef __cplusplus
}
#   endif
#   undef INCLUDE_ICL_APR_ACTIVE
#   if defined (ICL_IMPORT_ICL_APR_ROOT)
#       undef ICL_IMPORT_HEADERS
#       undef ICL_IMPORT_ICL_APR_ROOT
#   endif
# endif
#endif
/*---------------------------------------------------------------------------
    icl_mutex.h - icl_mutex component

    Defines wrappers to manage mutex objects.
    Generated from icl_mutex.icl by icl_gen using GSL/4.
    
    Copyright (c) 1996-2009 iMatix Corporation
    All rights reserved.
    
    This file is licensed under the BSD license as follows:
    
    Redistribution and use in source and binary forms, with or without
    modification, are permitted provided that the following conditions
    are met:
    
    * Redistributions of source code must retain the above copyright
      notice, this list of conditions and the following disclaimer.
    * Redistributions in binary form must reproduce the above copyright
      notice, this list of conditions and the following disclaimer in
      the documentation and/or other materials provided with the
      distribution.
    * Neither the name of iMatix Corporation nor the names of its
      contributors may be used to endorse or promote products derived
      from this software without specific prior written permission.
    
    THIS SOFTWARE IS PROVIDED BY IMATIX CORPORATION "AS IS" AND ANY
    EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE
    IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR
    PURPOSE ARE DISCLAIMED. IN NO EVENT SHALL IMATIX CORPORATION BE
    LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR
    CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF
    SUBSTITUTE GOODS OR SERVICES; LOSS OF USE, DATA, OR PROFITS; OR
    BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY OF
    LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT (INCLUDING
    NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF THIS
    SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.

    These header files use macros to implement a split inclusion in which all
    safe definitions (those that do not depend on the presence of other
    definitions) are done first, and all unsafe definitions are done in a
    second pass through the same headers.

    The first header file included from the main C program defines itself as
    the "root" and thus controls the inclusion of the safe/unsafe pieces of
    the other headers.
 *---------------------------------------------------------------------------*/

#if !defined (ICL_IMPORT_HEADERS) || (ICL_IMPORT_HEADERS == 1)
# ifndef INCLUDE_ICL_MUTEX_SAFE
#   define INCLUDE_ICL_MUTEX_SAFE
#   define INCLUDE_ICL_MUTEX_ACTIVE
#   if !defined (ICL_IMPORT_HEADERS)
#       define ICL_IMPORT_ICL_MUTEX_ROOT
#       define ICL_IMPORT_HEADERS 1
#   endif
#   ifdef __cplusplus
extern "C" {
#   endif

//  Our own class typedef

typedef struct _icl_mutex_t icl_mutex_t;

#define ICL_MUTEX_ALIVE                 0xFABB
#define ICL_MUTEX_DEAD                  0xDEAD

#define ICL_MUTEX_ASSERT_SANE(self)\
{\
    if (!self) {\
        icl_system_panic ("", "FATAL ERROR at %s:%u, in %s\n", __FILE__, __LINE__, ICL_ASSERT_SANE_FUNCTION);\
        icl_system_panic ("", "Attempting to work with a NULL icl_mutex\n");\
        icl_system_panic ("", "Please report this to openamq-dev@lists.openamq.org\n");\
        abort ();\
    }\
    if (self->object_tag != ICL_MUTEX_ALIVE) {\
        icl_system_panic ("", "FATAL ERROR at %s:%u, in %s\n", __FILE__, __LINE__, ICL_ASSERT_SANE_FUNCTION);\
        icl_system_panic ("", "icl_mutex at %p expected object_tag=0x%x, actual object_tag=0x%x\n", self, ICL_MUTEX_ALIVE, self->object_tag);\
        icl_system_panic ("", "Please report this to openamq-dev@lists.openamq.org\n");\
        abort ();\
    }\
}
#   ifdef __cplusplus
}
#   endif

#   include "icl_mem.h"
#   include "icl_system.h"
#   ifdef __cplusplus
extern "C" {
#   endif

#   ifdef __cplusplus
}
#   endif
#   undef INCLUDE_ICL_MUTEX_ACTIVE
#   if defined (ICL_IMPORT_ICL_MUTEX_ROOT)
#       undef  ICL_IMPORT_HEADERS
#       define ICL_IMPORT_HEADERS 2
#   endif
# endif
#endif

#if !defined (ICL_IMPORT_HEADERS) || (ICL_IMPORT_HEADERS == 2)
# ifndef INCLUDE_ICL_MUTEX_UNSAFE
#   define INCLUDE_ICL_MUTEX_UNSAFE
#   define INCLUDE_ICL_MUTEX_ACTIVE

#   include "icl_mem.h"
#   include "icl_system.h"

#   ifdef __cplusplus
extern "C" {
#   endif
//  Global variables

extern Bool
    icl_mutex_animating;

//  Structure of the icl_mutex object

struct _icl_mutex_t {
dbyte
    object_tag;                     //  Object validity marker
#if (defined (BASE_THREADSAFE))
    apr_thread_mutex_t
        *mutex;
    apr_pool_t
        *pool;
#else
    qbyte
        filler;
#endif
};

#   ifdef __cplusplus
}
#   endif
#   ifdef __cplusplus
extern "C" {
#   endif
//  Method prototypes
static inline void
    icl_mutex_lock (
icl_mutex_t * self                      //  Reference to object
);

static inline void
    icl_mutex_unlock (
icl_mutex_t * self                      //  Reference to object
);

void
    icl_mutex_selftest (
void);

void
    icl_mutex_terminate (
void);

#define icl_mutex_new()                 icl_mutex_new_ (__FILE__, __LINE__)
icl_mutex_t *
    icl_mutex_new_ (
char * file,                            //  Source file for call
size_t line                             //  Line number for call
);

#define icl_mutex_show(item,opcode,trace_file)  icl_mutex_show_ (item, opcode, trace_file, __FILE__, __LINE__)
void
    icl_mutex_show_ (
void * item,                            //  The opaque pointer
int opcode,                             //  The callback opcode
FILE * trace_file,                      //  File to print to
char * file,                            //  Source file
size_t line                             //  Line number
);

#define icl_mutex_destroy(self)         icl_mutex_destroy_ (self, __FILE__, __LINE__)
void
    icl_mutex_destroy_ (
icl_mutex_t * ( * self_p ),             //  Reference to object reference
char * file,                            //  Source fileSource file
size_t line                             //  Line numberLine number
);

void
    icl_mutex_show_animation (
Bool enabled                            //  Are we enabling or disabling animation?
);

#define icl_mutex_new_in_scope(self_p,_scope)  icl_mutex_new_in_scope_ (self_p, _scope, __FILE__, __LINE__)
void
    icl_mutex_new_in_scope_ (
icl_mutex_t * * self_p,                 //  Not documented
icl_scope_t * _scope,                   //  Not documented
char * file,                            //  Source file for call
size_t line                             //  Line number for call
);


#   ifdef __cplusplus
}
#   endif
#   undef INCLUDE_ICL_MUTEX_ACTIVE
#   if defined (ICL_IMPORT_ICL_MUTEX_ROOT)
#       undef  ICL_IMPORT_HEADERS
#       define ICL_IMPORT_HEADERS 3
#   endif
# endif
#endif

#if !defined (ICL_IMPORT_HEADERS) || (ICL_IMPORT_HEADERS == 3)
# if !defined (INCLUDE_ICL_MUTEX_INLINE)
#   define INCLUDE_ICL_MUTEX_INLINE
#   define INCLUDE_ICL_MUTEX_ACTIVE

#   include "icl_mem.h"
#   include "icl_system.h"

#   ifdef __cplusplus
extern "C" {
#   endif

/*  -------------------------------------------------------------------------
    icl_mutex_lock

    Type: Component method
    -------------------------------------------------------------------------
 */

static inline void
    icl_mutex_lock (
    icl_mutex_t * self                  //  Reference to object
)
{

    ICL_MUTEX_ASSERT_SANE (self);
#if (defined (BASE_THREADSAFE))
    assert (self->mutex);
    icl_apr_assert (apr_thread_mutex_lock (self->mutex));
#endif
}
/*  -------------------------------------------------------------------------
    icl_mutex_unlock

    Type: Component method
    -------------------------------------------------------------------------
 */

static inline void
    icl_mutex_unlock (
    icl_mutex_t * self                  //  Reference to object
)
{

    ICL_MUTEX_ASSERT_SANE (self);
#if (defined (BASE_THREADSAFE))
    assert (self->mutex);
    icl_apr_assert (apr_thread_mutex_unlock (self->mutex));
#endif
}
#   ifdef __cplusplus
}
#   endif
#   undef INCLUDE_ICL_MUTEX_ACTIVE
#   if defined (ICL_IMPORT_ICL_MUTEX_ROOT)
#       undef ICL_IMPORT_HEADERS
#       undef ICL_IMPORT_ICL_MUTEX_ROOT
#   endif
# endif
#endif
/*---------------------------------------------------------------------------
    icl_cond.h - icl_cond component

    Defines wrappers to manage condition variable objects.
    Generated from icl_cond.icl by icl_gen using GSL/4.
    
    Copyright (c) 1996-2009 iMatix Corporation
    All rights reserved.
    
    This file is licensed under the BSD license as follows:
    
    Redistribution and use in source and binary forms, with or without
    modification, are permitted provided that the following conditions
    are met:
    
    * Redistributions of source code must retain the above copyright
      notice, this list of conditions and the following disclaimer.
    * Redistributions in binary form must reproduce the above copyright
      notice, this list of conditions and the following disclaimer in
      the documentation and/or other materials provided with the
      distribution.
    * Neither the name of iMatix Corporation nor the names of its
      contributors may be used to endorse or promote products derived
      from this software without specific prior written permission.
    
    THIS SOFTWARE IS PROVIDED BY IMATIX CORPORATION "AS IS" AND ANY
    EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE
    IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR
    PURPOSE ARE DISCLAIMED. IN NO EVENT SHALL IMATIX CORPORATION BE
    LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR
    CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF
    SUBSTITUTE GOODS OR SERVICES; LOSS OF USE, DATA, OR PROFITS; OR
    BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY OF
    LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT (INCLUDING
    NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF THIS
    SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.

    These header files use macros to implement a split inclusion in which all
    safe definitions (those that do not depend on the presence of other
    definitions) are done first, and all unsafe definitions are done in a
    second pass through the same headers.

    The first header file included from the main C program defines itself as
    the "root" and thus controls the inclusion of the safe/unsafe pieces of
    the other headers.
 *---------------------------------------------------------------------------*/

#if !defined (ICL_IMPORT_HEADERS) || (ICL_IMPORT_HEADERS == 1)
# ifndef INCLUDE_ICL_COND_SAFE
#   define INCLUDE_ICL_COND_SAFE
#   define INCLUDE_ICL_COND_ACTIVE
#   if !defined (ICL_IMPORT_HEADERS)
#       define ICL_IMPORT_ICL_COND_ROOT
#       define ICL_IMPORT_HEADERS 1
#   endif
#   ifdef __cplusplus
extern "C" {
#   endif

//  Our own class typedef

typedef struct _icl_cond_t icl_cond_t;

#define ICL_COND_ALIVE                  0xFABB
#define ICL_COND_DEAD                   0xDEAD

#define ICL_COND_ASSERT_SANE(self)\
{\
    if (!self) {\
        icl_system_panic ("", "FATAL ERROR at %s:%u, in %s\n", __FILE__, __LINE__, ICL_ASSERT_SANE_FUNCTION);\
        icl_system_panic ("", "Attempting to work with a NULL icl_cond\n");\
        icl_system_panic ("", "Please report this to openamq-dev@lists.openamq.org\n");\
        abort ();\
    }\
    if (self->object_tag != ICL_COND_ALIVE) {\
        icl_system_panic ("", "FATAL ERROR at %s:%u, in %s\n", __FILE__, __LINE__, ICL_ASSERT_SANE_FUNCTION);\
        icl_system_panic ("", "icl_cond at %p expected object_tag=0x%x, actual object_tag=0x%x\n", self, ICL_COND_ALIVE, self->object_tag);\
        icl_system_panic ("", "Please report this to openamq-dev@lists.openamq.org\n");\
        abort ();\
    }\
}
#   ifdef __cplusplus
}
#   endif

#   include "icl_mem.h"
#   include "icl_system.h"
#   ifdef __cplusplus
extern "C" {
#   endif

#   ifdef __cplusplus
}
#   endif
#   undef INCLUDE_ICL_COND_ACTIVE
#   if defined (ICL_IMPORT_ICL_COND_ROOT)
#       undef  ICL_IMPORT_HEADERS
#       define ICL_IMPORT_HEADERS 2
#   endif
# endif
#endif

#if !defined (ICL_IMPORT_HEADERS) || (ICL_IMPORT_HEADERS == 2)
# ifndef INCLUDE_ICL_COND_UNSAFE
#   define INCLUDE_ICL_COND_UNSAFE
#   define INCLUDE_ICL_COND_ACTIVE

#   include "icl_mem.h"
#   include "icl_system.h"

#   ifdef __cplusplus
extern "C" {
#   endif
//  Global variables

extern Bool
    icl_cond_animating;

//  Structure of the icl_cond object

struct _icl_cond_t {
dbyte
    object_tag;                     //  Object validity marker
#if (defined (BASE_THREADSAFE))
    apr_thread_cond_t
        *cond;
    apr_pool_t
        *pool;
#else
    qbyte
        filler;
#endif
};

#   ifdef __cplusplus
}
#   endif
#   ifdef __cplusplus
extern "C" {
#   endif
//  Method prototypes
static inline int
    icl_cond_signal (
icl_cond_t * self                       //  Reference to object
);

static inline int
    icl_cond_broadcast (
icl_cond_t * self                       //  Reference to object
);

static inline int
    icl_cond_wait (
icl_cond_t * self,                      //  Reference to object
icl_mutex_t * mutex_p                   //  The mutex pointer
);

static inline int
    icl_cond_timed_wait (
icl_cond_t * self,                      //  Reference to object
icl_mutex_t * mutex_p,                  //  The mutex pointer
int64_t usecs                           //  Timeout for wait
);

void
    icl_cond_selftest (
void);

void
    icl_cond_terminate (
void);

#define icl_cond_new()                  icl_cond_new_ (__FILE__, __LINE__)
icl_cond_t *
    icl_cond_new_ (
char * file,                            //  Source file for call
size_t line                             //  Line number for call
);

#define icl_cond_show(item,opcode,trace_file)  icl_cond_show_ (item, opcode, trace_file, __FILE__, __LINE__)
void
    icl_cond_show_ (
void * item,                            //  The opaque pointer
int opcode,                             //  The callback opcode
FILE * trace_file,                      //  File to print to
char * file,                            //  Source file
size_t line                             //  Line number
);

#define icl_cond_destroy(self)          icl_cond_destroy_ (self, __FILE__, __LINE__)
void
    icl_cond_destroy_ (
icl_cond_t * ( * self_p ),              //  Reference to object reference
char * file,                            //  Source fileSource file
size_t line                             //  Line numberLine number
);

void
    icl_cond_show_animation (
Bool enabled                            //  Are we enabling or disabling animation?
);

#define icl_cond_new_in_scope(self_p,_scope)  icl_cond_new_in_scope_ (self_p, _scope, __FILE__, __LINE__)
void
    icl_cond_new_in_scope_ (
icl_cond_t * * self_p,                  //  Not documented
icl_scope_t * _scope,                   //  Not documented
char * file,                            //  Source file for call
size_t line                             //  Line number for call
);


#   ifdef __cplusplus
}
#   endif
#   undef INCLUDE_ICL_COND_ACTIVE
#   if defined (ICL_IMPORT_ICL_COND_ROOT)
#       undef  ICL_IMPORT_HEADERS
#       define ICL_IMPORT_HEADERS 3
#   endif
# endif
#endif

#if !defined (ICL_IMPORT_HEADERS) || (ICL_IMPORT_HEADERS == 3)
# if !defined (INCLUDE_ICL_COND_INLINE)
#   define INCLUDE_ICL_COND_INLINE
#   define INCLUDE_ICL_COND_ACTIVE

#   include "icl_mem.h"
#   include "icl_system.h"

#   ifdef __cplusplus
extern "C" {
#   endif

/*  -------------------------------------------------------------------------
    icl_cond_signal

    Type: Component method
    Accepts a icl_cond_t reference and returns zero in case of success,
    1 in case of errors.
    -------------------------------------------------------------------------
 */

static inline int
    icl_cond_signal (
    icl_cond_t * self                   //  Reference to object
)
{
    int
        rc = 0;                         //  Return code

ICL_COND_ASSERT_SANE (self);

#if (defined (BASE_THREADSAFE))
    assert (self->cond);
    icl_apr_assert (apr_thread_cond_signal (self->cond));
#endif


    return (rc);
}
/*  -------------------------------------------------------------------------
    icl_cond_broadcast

    Type: Component method
    Accepts a icl_cond_t reference and returns zero in case of success,
    1 in case of errors.
    -------------------------------------------------------------------------
 */

static inline int
    icl_cond_broadcast (
    icl_cond_t * self                   //  Reference to object
)
{
    int
        rc = 0;                         //  Return code

ICL_COND_ASSERT_SANE (self);

#if (defined (BASE_THREADSAFE))
    assert (self->cond);
    icl_apr_assert (apr_thread_cond_broadcast (self->cond));
#endif


    return (rc);
}
/*  -------------------------------------------------------------------------
    icl_cond_wait

    Type: Component method
    Accepts a icl_cond_t reference and returns zero in case of success,
    1 in case of errors.
    -------------------------------------------------------------------------
 */

static inline int
    icl_cond_wait (
    icl_cond_t * self,                  //  Reference to object
    icl_mutex_t * mutex_p               //  The mutex pointer
)
{
    int
        rc = 0;                         //  Return code

ICL_COND_ASSERT_SANE (self);

#if (defined (BASE_THREADSAFE))
    assert (self->cond);
    assert (mutex_p);
    assert (mutex_p->mutex);
    icl_apr_assert (apr_thread_cond_wait (self->cond, mutex_p->mutex));
#endif


    return (rc);
}
/*  -------------------------------------------------------------------------
    icl_cond_timed_wait

    Type: Component method
    Accepts a icl_cond_t reference and returns zero in case of success,
    1 in case of errors.
    Returns -1 on timeout; zero otherwise.
    -------------------------------------------------------------------------
 */

static inline int
    icl_cond_timed_wait (
    icl_cond_t * self,                  //  Reference to object
    icl_mutex_t * mutex_p,              //  The mutex pointer
    int64_t usecs                       //  Timeout for wait
)
{
#if (defined (BASE_THREADSAFE))
    apr_status_t
        status;
#endif
    int
        rc = 0;                         //  Return codeReturn code

ICL_COND_ASSERT_SANE (self);

#if (defined (BASE_THREADSAFE))
    assert (self->cond);
    assert (mutex_p);
    assert (mutex_p->mutex);
    status = apr_thread_cond_timedwait (self->cond, mutex_p->mutex, usecs);
    if (APR_STATUS_IS_TIMEUP (status)) {
        status = APR_SUCCESS;
        rc = -1;
    }
    icl_apr_assert (status);
#endif


    return (rc);
}
#   ifdef __cplusplus
}
#   endif
#   undef INCLUDE_ICL_COND_ACTIVE
#   if defined (ICL_IMPORT_ICL_COND_ROOT)
#       undef ICL_IMPORT_HEADERS
#       undef ICL_IMPORT_ICL_COND_ROOT
#   endif
# endif
#endif
/*---------------------------------------------------------------------------
    icl_rwlock.h - icl_rwlock component

    Defines wrappers to managed read-write lock objects.
    Generated from icl_rwlock.icl by icl_gen using GSL/4.
    
    Copyright (c) 1996-2009 iMatix Corporation
    All rights reserved.
    
    This file is licensed under the BSD license as follows:
    
    Redistribution and use in source and binary forms, with or without
    modification, are permitted provided that the following conditions
    are met:
    
    * Redistributions of source code must retain the above copyright
      notice, this list of conditions and the following disclaimer.
    * Redistributions in binary form must reproduce the above copyright
      notice, this list of conditions and the following disclaimer in
      the documentation and/or other materials provided with the
      distribution.
    * Neither the name of iMatix Corporation nor the names of its
      contributors may be used to endorse or promote products derived
      from this software without specific prior written permission.
    
    THIS SOFTWARE IS PROVIDED BY IMATIX CORPORATION "AS IS" AND ANY
    EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE
    IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR
    PURPOSE ARE DISCLAIMED. IN NO EVENT SHALL IMATIX CORPORATION BE
    LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR
    CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF
    SUBSTITUTE GOODS OR SERVICES; LOSS OF USE, DATA, OR PROFITS; OR
    BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY OF
    LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT (INCLUDING
    NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF THIS
    SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.

    These header files use macros to implement a split inclusion in which all
    safe definitions (those that do not depend on the presence of other
    definitions) are done first, and all unsafe definitions are done in a
    second pass through the same headers.

    The first header file included from the main C program defines itself as
    the "root" and thus controls the inclusion of the safe/unsafe pieces of
    the other headers.
 *---------------------------------------------------------------------------*/

#if !defined (ICL_IMPORT_HEADERS) || (ICL_IMPORT_HEADERS == 1)
# ifndef INCLUDE_ICL_RWLOCK_SAFE
#   define INCLUDE_ICL_RWLOCK_SAFE
#   define INCLUDE_ICL_RWLOCK_ACTIVE
#   if !defined (ICL_IMPORT_HEADERS)
#       define ICL_IMPORT_ICL_RWLOCK_ROOT
#       define ICL_IMPORT_HEADERS 1
#   endif
#   ifdef __cplusplus
extern "C" {
#   endif

//  Our own class typedef

typedef struct _icl_rwlock_t icl_rwlock_t;

#define ICL_RWLOCK_ALIVE                0xFABB
#define ICL_RWLOCK_DEAD                 0xDEAD

#define ICL_RWLOCK_ASSERT_SANE(self)\
{\
    if (!self) {\
        icl_system_panic ("", "FATAL ERROR at %s:%u, in %s\n", __FILE__, __LINE__, ICL_ASSERT_SANE_FUNCTION);\
        icl_system_panic ("", "Attempting to work with a NULL icl_rwlock\n");\
        icl_system_panic ("", "Please report this to openamq-dev@lists.openamq.org\n");\
        abort ();\
    }\
    if (self->object_tag != ICL_RWLOCK_ALIVE) {\
        icl_system_panic ("", "FATAL ERROR at %s:%u, in %s\n", __FILE__, __LINE__, ICL_ASSERT_SANE_FUNCTION);\
        icl_system_panic ("", "icl_rwlock at %p expected object_tag=0x%x, actual object_tag=0x%x\n", self, ICL_RWLOCK_ALIVE, self->object_tag);\
        icl_rwlock_show (self, ICL_CALLBACK_DUMP, stderr);\
        icl_system_panic ("", "Please report this to openamq-dev@lists.openamq.org\n");\
        abort ();\
    }\
}
#   ifdef __cplusplus
}
#   endif

#   include "icl_mem.h"
#   include "icl_system.h"
#   ifdef __cplusplus
extern "C" {
#   endif

#   ifdef __cplusplus
}
#   endif
#   undef INCLUDE_ICL_RWLOCK_ACTIVE
#   if defined (ICL_IMPORT_ICL_RWLOCK_ROOT)
#       undef  ICL_IMPORT_HEADERS
#       define ICL_IMPORT_HEADERS 2
#   endif
# endif
#endif

#if !defined (ICL_IMPORT_HEADERS) || (ICL_IMPORT_HEADERS == 2)
# ifndef INCLUDE_ICL_RWLOCK_UNSAFE
#   define INCLUDE_ICL_RWLOCK_UNSAFE
#   define INCLUDE_ICL_RWLOCK_ACTIVE

#   include "icl_mem.h"
#   include "icl_system.h"

#   ifdef __cplusplus
extern "C" {
#   endif
//  Global variables

extern Bool
    icl_rwlock_animating;

//  Structure of the icl_rwlock object

struct _icl_rwlock_t {
dbyte
    object_tag;                     //  Object validity marker
#if (defined (BASE_THREADSAFE))
    apr_thread_rwlock_t
        *rwlock;
    apr_pool_t
        *pool;
#else
    qbyte
        filler;
#endif
};

#   ifdef __cplusplus
}
#   endif
#   ifdef __cplusplus
extern "C" {
#   endif
//  Method prototypes
static inline void
    icl_rwlock_read_lock (
icl_rwlock_t * self                     //  Not documented
);

static inline void
    icl_rwlock_write_lock (
icl_rwlock_t * self                     //  Not documented
);

static inline void
    icl_rwlock_unlock (
icl_rwlock_t * self                     //  Not documented
);

void
    icl_rwlock_selftest (
void);

void
    icl_rwlock_terminate (
void);

#define icl_rwlock_new()                icl_rwlock_new_ (__FILE__, __LINE__)
icl_rwlock_t *
    icl_rwlock_new_ (
char * file,                            //  Source file for call
size_t line                             //  Line number for call
);

#define icl_rwlock_show(item,opcode,trace_file)  icl_rwlock_show_ (item, opcode, trace_file, __FILE__, __LINE__)
void
    icl_rwlock_show_ (
void * item,                            //  The opaque pointer
int opcode,                             //  The callback opcode
FILE * trace_file,                      //  File to print to
char * file,                            //  Source file
size_t line                             //  Line number
);

#define icl_rwlock_destroy(self)        icl_rwlock_destroy_ (self, __FILE__, __LINE__)
void
    icl_rwlock_destroy_ (
icl_rwlock_t * ( * self_p ),            //  Reference to object reference
char * file,                            //  Source fileSource file
size_t line                             //  Line numberLine number
);

void
    icl_rwlock_show_animation (
Bool enabled                            //  Are we enabling or disabling animation?
);

#define icl_rwlock_new_in_scope(self_p,_scope)  icl_rwlock_new_in_scope_ (self_p, _scope, __FILE__, __LINE__)
void
    icl_rwlock_new_in_scope_ (
icl_rwlock_t * * self_p,                //  Not documented
icl_scope_t * _scope,                   //  Not documented
char * file,                            //  Source file for call
size_t line                             //  Line number for call
);


#   ifdef __cplusplus
}
#   endif
#   undef INCLUDE_ICL_RWLOCK_ACTIVE
#   if defined (ICL_IMPORT_ICL_RWLOCK_ROOT)
#       undef  ICL_IMPORT_HEADERS
#       define ICL_IMPORT_HEADERS 3
#   endif
# endif
#endif

#if !defined (ICL_IMPORT_HEADERS) || (ICL_IMPORT_HEADERS == 3)
# if !defined (INCLUDE_ICL_RWLOCK_INLINE)
#   define INCLUDE_ICL_RWLOCK_INLINE
#   define INCLUDE_ICL_RWLOCK_ACTIVE

#   include "icl_mem.h"
#   include "icl_system.h"

#   ifdef __cplusplus
extern "C" {
#   endif

/*  -------------------------------------------------------------------------
    icl_rwlock_read_lock

    Type: Component method
    -------------------------------------------------------------------------
 */

static inline void
    icl_rwlock_read_lock (
    icl_rwlock_t * self                 //  Not documented
)
{

#if (defined (BASE_THREADSAFE))
    icl_apr_assert (apr_thread_rwlock_rdlock (self->rwlock));
#endif
}
/*  -------------------------------------------------------------------------
    icl_rwlock_write_lock

    Type: Component method
    -------------------------------------------------------------------------
 */

static inline void
    icl_rwlock_write_lock (
    icl_rwlock_t * self                 //  Not documented
)
{

#if (defined (BASE_THREADSAFE))
    icl_apr_assert (apr_thread_rwlock_wrlock (self->rwlock));
#endif
}
/*  -------------------------------------------------------------------------
    icl_rwlock_unlock

    Type: Component method
    -------------------------------------------------------------------------
 */

static inline void
    icl_rwlock_unlock (
    icl_rwlock_t * self                 //  Not documented
)
{

#if (defined (BASE_THREADSAFE))
    icl_apr_assert (apr_thread_rwlock_unlock (self->rwlock));
#endif
}
#   ifdef __cplusplus
}
#   endif
#   undef INCLUDE_ICL_RWLOCK_ACTIVE
#   if defined (ICL_IMPORT_ICL_RWLOCK_ROOT)
#       undef ICL_IMPORT_HEADERS
#       undef ICL_IMPORT_ICL_RWLOCK_ROOT
#   endif
# endif
#endif
/*---------------------------------------------------------------------------
    icl_console.h - icl_console component

    Defines the console interface for iCL.
    Generated from icl_console.icl by icl_gen using GSL/4.
    
    Copyright (c) 1996-2009 iMatix Corporation
    All rights reserved.
    
    This file is licensed under the BSD license as follows:
    
    Redistribution and use in source and binary forms, with or without
    modification, are permitted provided that the following conditions
    are met:
    
    * Redistributions of source code must retain the above copyright
      notice, this list of conditions and the following disclaimer.
    * Redistributions in binary form must reproduce the above copyright
      notice, this list of conditions and the following disclaimer in
      the documentation and/or other materials provided with the
      distribution.
    * Neither the name of iMatix Corporation nor the names of its
      contributors may be used to endorse or promote products derived
      from this software without specific prior written permission.
    
    THIS SOFTWARE IS PROVIDED BY IMATIX CORPORATION "AS IS" AND ANY
    EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE
    IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR
    PURPOSE ARE DISCLAIMED. IN NO EVENT SHALL IMATIX CORPORATION BE
    LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR
    CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF
    SUBSTITUTE GOODS OR SERVICES; LOSS OF USE, DATA, OR PROFITS; OR
    BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY OF
    LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT (INCLUDING
    NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF THIS
    SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.

    These header files use macros to implement a split inclusion in which all
    safe definitions (those that do not depend on the presence of other
    definitions) are done first, and all unsafe definitions are done in a
    second pass through the same headers.

    The first header file included from the main C program defines itself as
    the "root" and thus controls the inclusion of the safe/unsafe pieces of
    the other headers.
 *---------------------------------------------------------------------------*/

#if !defined (ICL_IMPORT_HEADERS) || (ICL_IMPORT_HEADERS == 1)
# ifndef INCLUDE_ICL_CONSOLE_SAFE
#   define INCLUDE_ICL_CONSOLE_SAFE
#   define INCLUDE_ICL_CONSOLE_ACTIVE
#   if !defined (ICL_IMPORT_HEADERS)
#       define ICL_IMPORT_ICL_CONSOLE_ROOT
#       define ICL_IMPORT_HEADERS 1
#   endif
#   ifdef __cplusplus
extern "C" {
#   endif

#include "base_apr.h"

//- Constants ---------------------------------------------------------------

typedef enum {
    ICL_CONSOLE_DATE,
    ICL_CONSOLE_TIME,
    ICL_CONSOLE_MILLI,
    ICL_CONSOLE_MICRO,
    ICL_CONSOLE_THREAD,
    ICL_CONSOLE_FLUSH,
    ICL_CONSOLE_QUIET
} icl_console_option_t;

//  Shortcuts

#define _icp        icl_console_print
#   ifdef __cplusplus
}
#   endif

#   ifdef __cplusplus
extern "C" {
#   endif

#   ifdef __cplusplus
}
#   endif
#   undef INCLUDE_ICL_CONSOLE_ACTIVE
#   if defined (ICL_IMPORT_ICL_CONSOLE_ROOT)
#       undef  ICL_IMPORT_HEADERS
#       define ICL_IMPORT_HEADERS 2
#   endif
# endif
#endif

#if !defined (ICL_IMPORT_HEADERS) || (ICL_IMPORT_HEADERS == 2)
# ifndef INCLUDE_ICL_CONSOLE_UNSAFE
#   define INCLUDE_ICL_CONSOLE_UNSAFE
#   define INCLUDE_ICL_CONSOLE_ACTIVE


#   ifdef __cplusplus
extern "C" {
#   endif
#   ifdef __cplusplus
}
#   endif
#   ifdef __cplusplus
extern "C" {
#   endif
//  Method prototypes
void
    icl_console_mode (
icl_console_option_t attr,              //  The option to set or clear
Bool value                              //  The value for the option
);

void
    icl_console_redirect (
FILE * file                             //  The stream to redirect the console to
);

void
    icl_console_print (
char * format,                          //  Format specifier
...                                     //  Variable arguments
);

void
    icl_console_print_v (
char * format,                          //  Format specifier
va_list argptr                          //  Variable arguments
);

void
    icl_console_out (
char * format,                          //  Format specifier
...                                     //  Variable arguments
);

void
    icl_console_out_v (
char * format,                          //  Format specifier
va_list argptr                          //  Variable arguments
);

void
    icl_console_print_thread_time (
icl_os_thread_t os_thread,              //  The thread info to print
apr_time_t time,                        //  The date/time to print
char * format,                          //  Format specifier
...                                     //  Variable arguments
);

void
    icl_console_print_thread_time_v (
icl_os_thread_t os_thread,              //  The thread info to print
apr_time_t time,                        //  The date/time to print
char * format,                          //  Format specifier
va_list argptr                          //  Variable arguments
);

void
    icl_console_selftest (
void);


#   ifdef __cplusplus
}
#   endif
#   undef INCLUDE_ICL_CONSOLE_ACTIVE
#   if defined (ICL_IMPORT_ICL_CONSOLE_ROOT)
#       undef  ICL_IMPORT_HEADERS
#       define ICL_IMPORT_HEADERS 3
#   endif
# endif
#endif

#if !defined (ICL_IMPORT_HEADERS) || (ICL_IMPORT_HEADERS == 3)
# if !defined (INCLUDE_ICL_CONSOLE_INLINE)
#   define INCLUDE_ICL_CONSOLE_INLINE
#   define INCLUDE_ICL_CONSOLE_ACTIVE


#   ifdef __cplusplus
extern "C" {
#   endif

#   ifdef __cplusplus
}
#   endif
#   undef INCLUDE_ICL_CONSOLE_ACTIVE
#   if defined (ICL_IMPORT_ICL_CONSOLE_ROOT)
#       undef ICL_IMPORT_HEADERS
#       undef ICL_IMPORT_ICL_CONSOLE_ROOT
#   endif
# endif
#endif
/*---------------------------------------------------------------------------
    icl_stats.h - icl_stats component

    Provides statistical reporting for other classes.

    Statistics are held in a shared memory block so they can easily be
    examined from another process, even if the running process is blocked.
    Generated from icl_stats.icl by icl_gen using GSL/4.
    
    Copyright (c) 1996-2009 iMatix Corporation
    All rights reserved.
    
    This file is licensed under the BSD license as follows:
    
    Redistribution and use in source and binary forms, with or without
    modification, are permitted provided that the following conditions
    are met:
    
    * Redistributions of source code must retain the above copyright
      notice, this list of conditions and the following disclaimer.
    * Redistributions in binary form must reproduce the above copyright
      notice, this list of conditions and the following disclaimer in
      the documentation and/or other materials provided with the
      distribution.
    * Neither the name of iMatix Corporation nor the names of its
      contributors may be used to endorse or promote products derived
      from this software without specific prior written permission.
    
    THIS SOFTWARE IS PROVIDED BY IMATIX CORPORATION "AS IS" AND ANY
    EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE
    IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR
    PURPOSE ARE DISCLAIMED. IN NO EVENT SHALL IMATIX CORPORATION BE
    LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR
    CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF
    SUBSTITUTE GOODS OR SERVICES; LOSS OF USE, DATA, OR PROFITS; OR
    BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY OF
    LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT (INCLUDING
    NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF THIS
    SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.

    These header files use macros to implement a split inclusion in which all
    safe definitions (those that do not depend on the presence of other
    definitions) are done first, and all unsafe definitions are done in a
    second pass through the same headers.

    The first header file included from the main C program defines itself as
    the "root" and thus controls the inclusion of the safe/unsafe pieces of
    the other headers.
 *---------------------------------------------------------------------------*/

#if !defined (ICL_IMPORT_HEADERS) || (ICL_IMPORT_HEADERS == 1)
# ifndef INCLUDE_ICL_STATS_SAFE
#   define INCLUDE_ICL_STATS_SAFE
#   define INCLUDE_ICL_STATS_ACTIVE
#   if !defined (ICL_IMPORT_HEADERS)
#       define ICL_IMPORT_ICL_STATS_ROOT
#       define ICL_IMPORT_HEADERS 1
#   endif
#   ifdef __cplusplus
extern "C" {
#   endif

typedef volatile unsigned int icl_stats_t;
#   ifdef __cplusplus
}
#   endif

#   include "icl_apr.h"
#   include "icl_system.h"
#   ifdef __cplusplus
extern "C" {
#   endif

#   ifdef __cplusplus
}
#   endif
#   undef INCLUDE_ICL_STATS_ACTIVE
#   if defined (ICL_IMPORT_ICL_STATS_ROOT)
#       undef  ICL_IMPORT_HEADERS
#       define ICL_IMPORT_HEADERS 2
#   endif
# endif
#endif

#if !defined (ICL_IMPORT_HEADERS) || (ICL_IMPORT_HEADERS == 2)
# ifndef INCLUDE_ICL_STATS_UNSAFE
#   define INCLUDE_ICL_STATS_UNSAFE
#   define INCLUDE_ICL_STATS_ACTIVE

#   include "icl_apr.h"
#   include "icl_system.h"

#   ifdef __cplusplus
extern "C" {
#   endif
//  Global variables

extern Bool
    icl_stats_animating;
#   ifdef __cplusplus
}
#   endif
#   ifdef __cplusplus
extern "C" {
#   endif
//  Method prototypes
icl_stats_t *
    icl_stats_new (
const char * name                       //  The name of the statistic
);

void
    icl_stats_dump (
void);

static inline void
    icl_stats_inc (
const char * name,                      //  The name of the statistic
icl_stats_t ** stat                     //  The statistic to increment
);

static inline void
    icl_stats_dec (
const char * name,                      //  The name of the statistic
icl_stats_t ** stat                     //  The statistic to decrement
);

static inline void
    icl_stats_add (
const char * name,                      //  The name of the statistic
icl_stats_t ** stat,                    //  The statistic to add to
icl_stats_t value                       //  The value to add to the statistic
);

static inline void
    icl_stats_assign (
const char * name,                      //  The name of the statistic
icl_stats_t ** stat,                    //  The statistic to assign to
icl_stats_t value                       //  The value to assign to the statistic
);

static inline void
    icl_stats_max (
const char * name,                      //  The name of the statistic
icl_stats_t ** stat,                    //  The statistic to add to
icl_stats_t value                       //  The value to add to the statistic
);

static inline void
    icl_stats_min (
const char * name,                      //  The name of the statistic
icl_stats_t ** stat,                    //  The statistic to add to
icl_stats_t value                       //  The value to add to the statistic
);

void
    icl_stats_selftest (
void);

void
    icl_stats_show_animation (
Bool enabled                            //  Are we enabling or disabling animation?
);


#   ifdef __cplusplus
}
#   endif
#   undef INCLUDE_ICL_STATS_ACTIVE
#   if defined (ICL_IMPORT_ICL_STATS_ROOT)
#       undef  ICL_IMPORT_HEADERS
#       define ICL_IMPORT_HEADERS 3
#   endif
# endif
#endif

#if !defined (ICL_IMPORT_HEADERS) || (ICL_IMPORT_HEADERS == 3)
# if !defined (INCLUDE_ICL_STATS_INLINE)
#   define INCLUDE_ICL_STATS_INLINE
#   define INCLUDE_ICL_STATS_ACTIVE

#   include "icl_apr.h"
#   include "icl_system.h"

#   ifdef __cplusplus
extern "C" {
#   endif

/*  -------------------------------------------------------------------------
    icl_stats_inc

    Type: Component method
    -------------------------------------------------------------------------
 */

static inline void
    icl_stats_inc (
    const char * name,                  //  The name of the statistic
    icl_stats_t ** stat                 //  The statistic to increment
)
{

if (!*stat)
    *stat = icl_stats_new (name);
icl_atomic_inc32 (*stat);
}
/*  -------------------------------------------------------------------------
    icl_stats_dec

    Type: Component method
    -------------------------------------------------------------------------
 */

static inline void
    icl_stats_dec (
    const char * name,                  //  The name of the statistic
    icl_stats_t ** stat                 //  The statistic to decrement
)
{

if (!*stat)
    *stat = icl_stats_new (name);
icl_atomic_dec32 (*stat);
}
/*  -------------------------------------------------------------------------
    icl_stats_add

    Type: Component method
    -------------------------------------------------------------------------
 */

static inline void
    icl_stats_add (
    const char * name,                  //  The name of the statistic
    icl_stats_t ** stat,                //  The statistic to add to
    icl_stats_t value                   //  The value to add to the statistic
)
{

if (!*stat)
    *stat = icl_stats_new (name);
icl_atomic_add32 (*stat, value);
}
/*  -------------------------------------------------------------------------
    icl_stats_assign

    Type: Component method
    -------------------------------------------------------------------------
 */

static inline void
    icl_stats_assign (
    const char * name,                  //  The name of the statistic
    icl_stats_t ** stat,                //  The statistic to assign to
    icl_stats_t value                   //  The value to assign to the statistic
)
{

if (!*stat)
    *stat = icl_stats_new (name);
icl_atomic_set32 (*stat, value);
}
/*  -------------------------------------------------------------------------
    icl_stats_max

    Type: Component method
    -------------------------------------------------------------------------
 */

static inline void
    icl_stats_max (
    const char * name,                  //  The name of the statistic
    icl_stats_t ** stat,                //  The statistic to add to
    icl_stats_t value                   //  The value to add to the statistic
)
{
icl_stats_t
    cur_value,
    new_value;

if (!*stat)
    *stat = icl_stats_new (name);
cur_value = icl_atomic_get32 (*stat);
while (value > cur_value) {
    new_value = icl_atomic_cas32 (*stat, value, cur_value);
    if (new_value == cur_value)
        cur_value = value;
    else
        cur_value = new_value;
}
}
/*  -------------------------------------------------------------------------
    icl_stats_min

    Type: Component method
    -------------------------------------------------------------------------
 */

static inline void
    icl_stats_min (
    const char * name,                  //  The name of the statistic
    icl_stats_t ** stat,                //  The statistic to add to
    icl_stats_t value                   //  The value to add to the statistic
)
{
icl_stats_t
    cur_value,
    new_value;

if (!*stat)
    *stat = icl_stats_new (name);
cur_value = icl_atomic_get32 (*stat);
while (value < cur_value) {
    new_value = icl_atomic_cas32 (*stat, value, cur_value);
    if (cur_value == cur_value)
        cur_value = value;
    else
        cur_value = new_value;
}
}
#   ifdef __cplusplus
}
#   endif
#   undef INCLUDE_ICL_STATS_ACTIVE
#   if defined (ICL_IMPORT_ICL_STATS_ROOT)
#       undef ICL_IMPORT_HEADERS
#       undef ICL_IMPORT_ICL_STATS_ROOT
#   endif
# endif
#endif
/*---------------------------------------------------------------------------
    icl_trace.h - icl_trace component

    Class to enable trace of program behaviour.
    To trace any program operation, call the trace method during operation.
    The trace is replayed via the dump method.
    Generated from icl_trace.icl by icl_gen using GSL/4.
    
    Copyright (c) 1996-2009 iMatix Corporation
    All rights reserved.
    
    This file is licensed under the BSD license as follows:
    
    Redistribution and use in source and binary forms, with or without
    modification, are permitted provided that the following conditions
    are met:
    
    * Redistributions of source code must retain the above copyright
      notice, this list of conditions and the following disclaimer.
    * Redistributions in binary form must reproduce the above copyright
      notice, this list of conditions and the following disclaimer in
      the documentation and/or other materials provided with the
      distribution.
    * Neither the name of iMatix Corporation nor the names of its
      contributors may be used to endorse or promote products derived
      from this software without specific prior written permission.
    
    THIS SOFTWARE IS PROVIDED BY IMATIX CORPORATION "AS IS" AND ANY
    EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE
    IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR
    PURPOSE ARE DISCLAIMED. IN NO EVENT SHALL IMATIX CORPORATION BE
    LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR
    CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF
    SUBSTITUTE GOODS OR SERVICES; LOSS OF USE, DATA, OR PROFITS; OR
    BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY OF
    LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT (INCLUDING
    NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF THIS
    SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.

    These header files use macros to implement a split inclusion in which all
    safe definitions (those that do not depend on the presence of other
    definitions) are done first, and all unsafe definitions are done in a
    second pass through the same headers.

    The first header file included from the main C program defines itself as
    the "root" and thus controls the inclusion of the safe/unsafe pieces of
    the other headers.
 *---------------------------------------------------------------------------*/

#if !defined (ICL_IMPORT_HEADERS) || (ICL_IMPORT_HEADERS == 1)
# ifndef INCLUDE_ICL_TRACE_SAFE
#   define INCLUDE_ICL_TRACE_SAFE
#   define INCLUDE_ICL_TRACE_ACTIVE
#   if !defined (ICL_IMPORT_HEADERS)
#       define ICL_IMPORT_ICL_TRACE_ROOT
#       define ICL_IMPORT_HEADERS 1
#   endif
#   ifdef __cplusplus
extern "C" {
#   endif

//  Our own class typedef

typedef struct _icl_trace_t icl_trace_t;

#define ICL_TRACE_ALIVE                 0xFABB
#define ICL_TRACE_DEAD                  0xDEAD

#define ICL_TRACE_ASSERT_SANE(self)\
{\
    if (!self) {\
        icl_system_panic ("", "FATAL ERROR at %s:%u, in %s\n", __FILE__, __LINE__, ICL_ASSERT_SANE_FUNCTION);\
        icl_system_panic ("", "Attempting to work with a NULL icl_trace\n");\
        icl_system_panic ("", "Please report this to openamq-dev@lists.openamq.org\n");\
        abort ();\
    }\
    if (self->object_tag != ICL_TRACE_ALIVE) {\
        icl_system_panic ("", "FATAL ERROR at %s:%u, in %s\n", __FILE__, __LINE__, ICL_ASSERT_SANE_FUNCTION);\
        icl_system_panic ("", "icl_trace at %p expected object_tag=0x%x, actual object_tag=0x%x\n", self, ICL_TRACE_ALIVE, self->object_tag);\
        icl_system_panic ("", "Please report this to openamq-dev@lists.openamq.org\n");\
        abort ();\
    }\
}
#   ifdef __cplusplus
}
#   endif

#   include "icl_mem.h"
#   include "icl_system.h"
#   ifdef __cplusplus
extern "C" {
#   endif

#   ifdef __cplusplus
}
#   endif
#   undef INCLUDE_ICL_TRACE_ACTIVE
#   if defined (ICL_IMPORT_ICL_TRACE_ROOT)
#       undef  ICL_IMPORT_HEADERS
#       define ICL_IMPORT_HEADERS 2
#   endif
# endif
#endif

#if !defined (ICL_IMPORT_HEADERS) || (ICL_IMPORT_HEADERS == 2)
# ifndef INCLUDE_ICL_TRACE_UNSAFE
#   define INCLUDE_ICL_TRACE_UNSAFE
#   define INCLUDE_ICL_TRACE_ACTIVE

#   include "icl_mem.h"
#   include "icl_system.h"

#   ifdef __cplusplus
extern "C" {
#   endif
//  Global variables

extern Bool
    icl_trace_animating;
//  Macros

#define ICL_TRACE_SIZE 256

//  Types

typedef void (icl_trace_dump_fn) (
    icl_os_thread_t thread,
    apr_time_t      time,
    qbyte           info);

//  Structure to hold an individual trace item

typedef struct {
    icl_trace_dump_fn
        *dump;                          //  The function to interpret trace
#if (defined (BASE_THREADSAFE))
    apr_os_thread_t
        thread;                         //  The OS thread
#endif
    apr_time_t
        time;                           //  The time of the trace
    qbyte
        info;                           //  The trace information
} icl_trace_item_t;

//  These variables are accessed only from inline functions, so are
//  extern but also named s_ to mark their private nature.

extern icl_trace_t
    *s_icl_trace;

//  Structure of the icl_trace object

struct _icl_trace_t {
dbyte
    object_tag;                     //  Object validity marker
icl_trace_item_t
    *array;                         //  Array of trace items
uint
    size,                           //  Total size of trace
    idx;                            //  Current index in trace
};

#   ifdef __cplusplus
}
#   endif
#   ifdef __cplusplus
extern "C" {
#   endif
//  Method prototypes
#define icl_trace_new(size)             icl_trace_new_ (__FILE__, __LINE__, size)
icl_trace_t *
    icl_trace_new_ (
char * file,                            //  Source file for call
size_t line,                            //  Line number for call
uint size                               //  Size for this trace
);

static inline void
    icl_trace_record (
icl_trace_t * self,                     //  The trace structure
icl_trace_dump_fn * dump,               //  The function to dump the trace
qbyte info                              //  Thd ID of the event, place or action
);

void
    icl_trace_dump (
icl_trace_t * self                      //  The trace structure
);

void
    icl_trace_terminate (
void);

void
    icl_trace_selftest (
void);

#define icl_trace_show(item,opcode,trace_file)  icl_trace_show_ (item, opcode, trace_file, __FILE__, __LINE__)
void
    icl_trace_show_ (
void * item,                            //  The opaque pointer
int opcode,                             //  The callback opcode
FILE * trace_file,                      //  File to print to
char * file,                            //  Source file
size_t line                             //  Line number
);

#define icl_trace_destroy(self)         icl_trace_destroy_ (self, __FILE__, __LINE__)
void
    icl_trace_destroy_ (
icl_trace_t * ( * self_p ),             //  Reference to object reference
char * file,                            //  Source fileSource file
size_t line                             //  Line numberLine number
);

void
    icl_trace_show_animation (
Bool enabled                            //  Are we enabling or disabling animation?
);

#define icl_trace_new_in_scope(self_p,_scope,size)  icl_trace_new_in_scope_ (self_p, _scope, __FILE__, __LINE__, size)
void
    icl_trace_new_in_scope_ (
icl_trace_t * * self_p,                 //  Not documented
icl_scope_t * _scope,                   //  Not documented
char * file,                            //  Source file for call
size_t line,                            //  Line number for call
uint size                               //  Size for this trace
);


#   ifdef __cplusplus
}
#   endif
#   undef INCLUDE_ICL_TRACE_ACTIVE
#   if defined (ICL_IMPORT_ICL_TRACE_ROOT)
#       undef  ICL_IMPORT_HEADERS
#       define ICL_IMPORT_HEADERS 3
#   endif
# endif
#endif

#if !defined (ICL_IMPORT_HEADERS) || (ICL_IMPORT_HEADERS == 3)
# if !defined (INCLUDE_ICL_TRACE_INLINE)
#   define INCLUDE_ICL_TRACE_INLINE
#   define INCLUDE_ICL_TRACE_ACTIVE

#   include "icl_mem.h"
#   include "icl_system.h"

#   ifdef __cplusplus
extern "C" {
#   endif

/*  -------------------------------------------------------------------------
    icl_trace_record

    Type: Component method
    Call this method to register an event.
    
    On playback, the dump callback will be called with the
    thread and time of the trace call along with the info.
    -------------------------------------------------------------------------
 */

static inline void
    icl_trace_record (
    icl_trace_t * self,                 //  The trace structure
    icl_trace_dump_fn * dump,           //  The function to dump the trace
    qbyte info                          //  Thd ID of the event, place or action
)
{
uint
    idx;

    if (self == NULL) {
        if (!s_icl_trace)
            s_icl_trace = icl_trace_new (ICL_TRACE_SIZE);

        self = s_icl_trace;
    }

    idx = icl_atomic_inc32 ((qbyte*) &self->idx) % self->size;
    self->array[idx].dump = dump;
#if (defined (BASE_THREADSAFE))
    self->array[idx].thread      = apr_os_thread_current ();
#endif
    self->array[idx].time        = apr_time_now ();
    self->array[idx].info        = info;
}
#   ifdef __cplusplus
}
#   endif
#   undef INCLUDE_ICL_TRACE_ACTIVE
#   if defined (ICL_IMPORT_ICL_TRACE_ROOT)
#       undef ICL_IMPORT_HEADERS
#       undef ICL_IMPORT_ICL_TRACE_ROOT
#   endif
# endif
#endif
/*---------------------------------------------------------------------------
    icl_mem.h - icl_mem component

    Provides API for memory allocator.  Methods are implemented in icl_mem_fat
    and icl_mem_thin.
    Generated from icl_mem.icl by icl_gen using GSL/4.
    
    Copyright (c) 1996-2009 iMatix Corporation
    All rights reserved.
    
    This file is licensed under the BSD license as follows:
    
    Redistribution and use in source and binary forms, with or without
    modification, are permitted provided that the following conditions
    are met:
    
    * Redistributions of source code must retain the above copyright
      notice, this list of conditions and the following disclaimer.
    * Redistributions in binary form must reproduce the above copyright
      notice, this list of conditions and the following disclaimer in
      the documentation and/or other materials provided with the
      distribution.
    * Neither the name of iMatix Corporation nor the names of its
      contributors may be used to endorse or promote products derived
      from this software without specific prior written permission.
    
    THIS SOFTWARE IS PROVIDED BY IMATIX CORPORATION "AS IS" AND ANY
    EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE
    IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR
    PURPOSE ARE DISCLAIMED. IN NO EVENT SHALL IMATIX CORPORATION BE
    LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR
    CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF
    SUBSTITUTE GOODS OR SERVICES; LOSS OF USE, DATA, OR PROFITS; OR
    BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY OF
    LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT (INCLUDING
    NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF THIS
    SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.

    These header files use macros to implement a split inclusion in which all
    safe definitions (those that do not depend on the presence of other
    definitions) are done first, and all unsafe definitions are done in a
    second pass through the same headers.

    The first header file included from the main C program defines itself as
    the "root" and thus controls the inclusion of the safe/unsafe pieces of
    the other headers.
 *---------------------------------------------------------------------------*/

#if !defined (ICL_IMPORT_HEADERS) || (ICL_IMPORT_HEADERS == 1)
# ifndef INCLUDE_ICL_MEM_SAFE
#   define INCLUDE_ICL_MEM_SAFE
#   define INCLUDE_ICL_MEM_ACTIVE
#   if !defined (ICL_IMPORT_HEADERS)
#       define ICL_IMPORT_ICL_MEM_ROOT
#       define ICL_IMPORT_HEADERS 1
#   endif
#   ifdef __cplusplus
extern "C" {
#   endif

//  Types

typedef void (icl_mem_free_fn) (void *data);

typedef enum {
    ICL_MEM_UNDEFINED,
    ICL_MEM_FAT,
    ICL_MEM_THIN,
    ICL_MEM_DIRECT
} icl_mem_allocator_t;

//  Global variables

extern icl_mem_allocator_t
    icl_mem_allocator;
extern volatile void *
    icl_mem_total_used;
#   ifdef __cplusplus
}
#   endif

#   include "icl_mem_fat.h"
#   include "icl_mem_thin.h"
#   include "icl_system.h"
#   ifdef __cplusplus
extern "C" {
#   endif

#   ifdef __cplusplus
}
#   endif
#   undef INCLUDE_ICL_MEM_ACTIVE
#   if defined (ICL_IMPORT_ICL_MEM_ROOT)
#       undef  ICL_IMPORT_HEADERS
#       define ICL_IMPORT_HEADERS 2
#   endif
# endif
#endif

#if !defined (ICL_IMPORT_HEADERS) || (ICL_IMPORT_HEADERS == 2)
# ifndef INCLUDE_ICL_MEM_UNSAFE
#   define INCLUDE_ICL_MEM_UNSAFE
#   define INCLUDE_ICL_MEM_ACTIVE

#   include "icl_mem_fat.h"
#   include "icl_mem_thin.h"
#   include "icl_system.h"

#   ifdef __cplusplus
extern "C" {
#   endif
#   ifdef __cplusplus
}
#   endif
#   ifdef __cplusplus
extern "C" {
#   endif
//  Method prototypes
void
    icl_set_allocator (
icl_mem_allocator_t allocator           //  The allocator to use
);

void
    icl_mem_initialise (
void);

#define icl_mem_cache_reuse(cache)      icl_mem_cache_reuse_ (cache, __FILE__, __LINE__)
static inline void *
    icl_mem_cache_reuse_ (
icl_cache_t * cache,                    //  The cache for the allocation
char * file,                            //  Source file for call
size_t line                             //  Line number for call
);

#define icl_mem_cache_alloc(cache)      icl_mem_cache_alloc_ (cache, __FILE__, __LINE__)
static inline void *
    icl_mem_cache_alloc_ (
icl_cache_t * cache,                    //  The cache for the allocation
char * file,                            //  Source file for call
size_t line                             //  Line number for call
);

#define icl_mem_cache_pop(cache)        icl_mem_cache_pop_ (cache, __FILE__, __LINE__)
static inline void *
    icl_mem_cache_pop_ (
icl_cache_t * cache,                    //  The cache for the allocation
char * file,                            //  Source file for call
size_t line                             //  Line number for call
);

#define icl_mem_cache_new(cache)        icl_mem_cache_new_ (cache, __FILE__, __LINE__)
static inline void *
    icl_mem_cache_new_ (
icl_cache_t * cache,                    //  The cache for the allocation
char * file,                            //  Source file for call
size_t line                             //  Line number for call
);

#define icl_mem_alloc(size)             icl_mem_alloc_ (size, __FILE__, __LINE__)
static inline void *
    icl_mem_alloc_ (
size_t size,                            //  How much memory to allocate
char * file,                            //  Source file for call
size_t line                             //  Line number for call
);

#define icl_mem_realloc(item,size)      icl_mem_realloc_ (item, size, __FILE__, __LINE__)
static inline void *
    icl_mem_realloc_ (
void * item,                            //  Cient buffer to reallocate
qbyte size,                             //  How much memory to allocate
char * file,                            //  Source file for call
size_t line                             //  Line number for call
);

static inline void
    icl_mem_free (
void * item                             //  Client buffer to free
);

#define icl_mem_possess(item)           icl_mem_possess_ (item, __FILE__, __LINE__)
static inline void
    icl_mem_possess_ (
void * item,                            //  Client buffer to free
char * file,                            //  Source file for call
size_t line                             //  Line number for call
);

static inline void
    icl_mem_set_callback (
void * item,                            //  Client buffer to free
icl_callback_fn * callback              //  Callback function to set
);

void
    icl_mem_cache_purge (
icl_cache_t * cache                     //  The cache to purge
);

static inline void
    icl_mem_increment_used (
size_t size                             //  The amount by which to increment the total
);

static inline void
    icl_mem_decrement_used (
size_t size                             //  The amount by which to decrement the total
);

size_t
    icl_mem_used (
void);

#define icl_mem_strdup(string)          icl_mem_strdup_ (string, __FILE__, __LINE__)
static inline void *
    icl_mem_strdup_ (
void * string,                          //  String to duplicate
char * file,                            //  Source file for call
size_t line                             //  Line number for call
);

static inline void
    icl_mem_strfree (
char * ( * client_p )                   //  Client buffer to free
);

void
    icl_mem_selftest (
void);


#   ifdef __cplusplus
}
#   endif
#   undef INCLUDE_ICL_MEM_ACTIVE
#   if defined (ICL_IMPORT_ICL_MEM_ROOT)
#       undef  ICL_IMPORT_HEADERS
#       define ICL_IMPORT_HEADERS 3
#   endif
# endif
#endif

#if !defined (ICL_IMPORT_HEADERS) || (ICL_IMPORT_HEADERS == 3)
# if !defined (INCLUDE_ICL_MEM_INLINE)
#   define INCLUDE_ICL_MEM_INLINE
#   define INCLUDE_ICL_MEM_ACTIVE

#   include "icl_mem_fat.h"
#   include "icl_mem_thin.h"
#   include "icl_system.h"

#   ifdef __cplusplus
extern "C" {
#   endif

/*  -------------------------------------------------------------------------
    icl_mem_cache_reuse

    Type: Component method
    Gets a block from a cache but does not allocate a new one if the cache is
    empty.
    -------------------------------------------------------------------------
 */

static inline void *
    icl_mem_cache_reuse_ (
    icl_cache_t * cache,                //  The cache for the allocation
    char * file,                        //  Source file for call
    size_t line                         //  Line number for call
)
{
    void *
        item = NULL;                    //  The returned client pointer

if (icl_mem_allocator == ICL_MEM_THIN)
    item = icl_mem_thin_cache_reuse_ (cache, file, line);
else
if (icl_mem_allocator == ICL_MEM_FAT)
    item = icl_mem_fat_cache_reuse_ (cache, file, line);

    return (item);
}
/*  -------------------------------------------------------------------------
    icl_mem_cache_alloc

    Type: Component method
    Allocate a block in a cache.  The size of the block is the size of the
    cache.
    -------------------------------------------------------------------------
 */

static inline void *
    icl_mem_cache_alloc_ (
    icl_cache_t * cache,                //  The cache for the allocation
    char * file,                        //  Source file for call
    size_t line                         //  Line number for call
)
{
    void *
        item = NULL;                    //  The returned client pointer

if (icl_mem_allocator == ICL_MEM_THIN)
    item = icl_mem_thin_cache_alloc_ (cache, file, line);
else
if (icl_mem_allocator == ICL_MEM_FAT)
    item = icl_mem_fat_cache_alloc_ (cache, file, line);
else
    item = malloc (cache->size);

    return (item);
}
/*  -------------------------------------------------------------------------
    icl_mem_cache_pop

    Type: Component method
    Pops a block from a cache.
    cache.
    -------------------------------------------------------------------------
 */

static inline void *
    icl_mem_cache_pop_ (
    icl_cache_t * cache,                //  The cache for the allocation
    char * file,                        //  Source file for call
    size_t line                         //  Line number for call
)
{
    void *
        item = NULL;                    //  The returned client pointer

if (icl_mem_allocator == ICL_MEM_THIN)
    item = icl_mem_thin_cache_pop_ (cache, file, line);
else
if (icl_mem_allocator == ICL_MEM_FAT)
    item = icl_mem_fat_cache_pop_ (cache, file, line);

    return (item);
}
/*  -------------------------------------------------------------------------
    icl_mem_cache_new

    Type: Component method
    Allocates a block in a cache.  The size of the block is the size of the
    cache.
    -------------------------------------------------------------------------
 */

static inline void *
    icl_mem_cache_new_ (
    icl_cache_t * cache,                //  The cache for the allocation
    char * file,                        //  Source file for call
    size_t line                         //  Line number for call
)
{
    void *
        item = NULL;                    //  The returned client pointer

if (icl_mem_allocator == ICL_MEM_THIN)
    item = icl_mem_thin_cache_new_ (cache, file, line);
else
if (icl_mem_allocator == ICL_MEM_FAT)
    item = icl_mem_fat_cache_new_ (cache, file, line);
else
    item = malloc (cache->size);

    return (item);
}
/*  -------------------------------------------------------------------------
    icl_mem_alloc

    Type: Component method
    Allocate a block in a cache.  The size of the block is the size of the
    cache.
    -------------------------------------------------------------------------
 */

static inline void *
    icl_mem_alloc_ (
    size_t size,                        //  How much memory to allocate
    char * file,                        //  Source file for call
    size_t line                         //  Line number for call
)
{
    void *
        item = NULL;                    //  The returned client pointer

if (icl_mem_allocator == ICL_MEM_THIN)
    item = icl_mem_thin_alloc_ (size, file, line);
else
if (icl_mem_allocator == ICL_MEM_FAT)
    item = icl_mem_fat_alloc_ (size, file, line);
else
    item = malloc (size);

    return (item);
}
/*  -------------------------------------------------------------------------
    icl_mem_realloc

    Type: Component method
    Adjust the size of the pointed to 'buffer' to the new 'size'.
    If 'buffer' is NULL, self call behaves as a regular alloc().
    -------------------------------------------------------------------------
 */

static inline void *
    icl_mem_realloc_ (
    void * item,                        //  Cient buffer to reallocate
    qbyte size,                         //  How much memory to allocate
    char * file,                        //  Source file for call
    size_t line                         //  Line number for call
)
{

if (icl_mem_allocator == ICL_MEM_THIN)
    item = icl_mem_thin_realloc_ (item, size, file, line);
else
if (icl_mem_allocator == ICL_MEM_FAT)
    item = icl_mem_fat_realloc_ (item, size, file, line);
else
    item = realloc (item, size);

    return (item);
}
/*  -------------------------------------------------------------------------
    icl_mem_free

    Type: Component method
    Destroys an allocation block.
    -------------------------------------------------------------------------
 */

static inline void
    icl_mem_free (
    void * item                         //  Client buffer to free
)
{

if (icl_mem_allocator == ICL_MEM_THIN)
    icl_mem_thin_free (item);
else
if (icl_mem_allocator == ICL_MEM_FAT)
    icl_mem_fat_free (item);
else
    free (item);
}
/*  -------------------------------------------------------------------------
    icl_mem_possess

    Type: Component method
    Sets file and line number of memory allocation.
    -------------------------------------------------------------------------
 */

static inline void
    icl_mem_possess_ (
    void * item,                        //  Client buffer to free
    char * file,                        //  Source file for call
    size_t line                         //  Line number for call
)
{

if (icl_mem_allocator == ICL_MEM_THIN)
    icl_mem_thin_possess_ (item, file, line);
else
if (icl_mem_allocator == ICL_MEM_FAT)
    icl_mem_fat_possess_ (item, file, line);
}
/*  -------------------------------------------------------------------------
    icl_mem_set_callback

    Type: Component method
    Sets callback of memory allocation.
    -------------------------------------------------------------------------
 */

static inline void
    icl_mem_set_callback (
    void * item,                        //  Client buffer to free
    icl_callback_fn * callback          //  Callback function to set
)
{

if (icl_mem_allocator == ICL_MEM_THIN)
    icl_mem_thin_set_callback (item, callback);
else
if (icl_mem_allocator == ICL_MEM_FAT)
    icl_mem_fat_set_callback (item, callback);
}
/*  -------------------------------------------------------------------------
    icl_mem_increment_used

    Type: Component method
    Increment the record of the total memory allocations.

    This is non-trivial because it must be thread-safe, and it must work on
    64 bit architectures.
    -------------------------------------------------------------------------
 */

static inline void
    icl_mem_increment_used (
    size_t size                         //  The amount by which to increment the total
)
{
void
    *old_used;

do
    old_used = (void *) icl_mem_total_used;
while (icl_atomic_casptr (&icl_mem_total_used, (byte *) old_used + size, old_used) != old_used);
}
/*  -------------------------------------------------------------------------
    icl_mem_decrement_used

    Type: Component method
    Decrement the record of the total memory allocations.

    This is non-trivial because it must be thread-safe, and it must work on
    64 bit architectures.
    -------------------------------------------------------------------------
 */

static inline void
    icl_mem_decrement_used (
    size_t size                         //  The amount by which to decrement the total
)
{
void
    *old_used;

do
    old_used = (void *) icl_mem_total_used;
while (icl_atomic_casptr (&icl_mem_total_used, (byte *) old_used - size, old_used) != old_used);
}
/*  -------------------------------------------------------------------------
    icl_mem_strdup

    Type: Component method
    Duplicates the provided string, returns a fresh copy.  Accepts a null string
    argument, in which case it returns null.
    -------------------------------------------------------------------------
 */

static inline void *
    icl_mem_strdup_ (
    void * string,                      //  String to duplicate
    char * file,                        //  Source file for call
    size_t line                         //  Line number for call
)
{
qbyte
    len;
    void *
        item = NULL;                    //  The returned client pointer

if (string) {
    len = strlen ((char*) string) + 1;
    item = icl_mem_alloc_ (len, file, line);
    if (item)
        strncpy ((char*) item, (char*) string, len);
}
else
    item = NULL;

    return (item);
}
/*  -------------------------------------------------------------------------
    icl_mem_strfree

    Type: Component method
    Frees the memory space pointed to by 'buffer'.
    -------------------------------------------------------------------------
 */

static inline void
    icl_mem_strfree (
    char * ( * client_p )               //  Client buffer to free
)
{
    char *
        client = *client_p;             //  Dereferenced Client buffer to free

if (client) {
    icl_mem_free (client);
    *client_p = NULL;
}
}
#   ifdef __cplusplus
}
#   endif
#   undef INCLUDE_ICL_MEM_ACTIVE
#   if defined (ICL_IMPORT_ICL_MEM_ROOT)
#       undef ICL_IMPORT_HEADERS
#       undef ICL_IMPORT_ICL_MEM_ROOT
#   endif
# endif
#endif
/*---------------------------------------------------------------------------
    icl_system.h - icl_system component

    Implements the metacache manager.  Provides two functions for
    application developers: icl_system_purge(), which does garbage
    collection, and icl_system_terminate(), which does final destruction
    of all active and cached objects. Provides one function for iCL
    developers: icl_system_register(), which registers a class for
    purging and destroying.

    Also responsible for system-wide initialisation, comprising
    icl_global_pool and icl_global_mutex.  Note that to avoid circular
    dependencies, these two are declared in icl_apr.
    Generated from icl_system.icl by icl_gen using GSL/4.
    
    Copyright (c) 1996-2009 iMatix Corporation
    All rights reserved.
    
    This file is licensed under the BSD license as follows:
    
    Redistribution and use in source and binary forms, with or without
    modification, are permitted provided that the following conditions
    are met:
    
    * Redistributions of source code must retain the above copyright
      notice, this list of conditions and the following disclaimer.
    * Redistributions in binary form must reproduce the above copyright
      notice, this list of conditions and the following disclaimer in
      the documentation and/or other materials provided with the
      distribution.
    * Neither the name of iMatix Corporation nor the names of its
      contributors may be used to endorse or promote products derived
      from this software without specific prior written permission.
    
    THIS SOFTWARE IS PROVIDED BY IMATIX CORPORATION "AS IS" AND ANY
    EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE
    IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR
    PURPOSE ARE DISCLAIMED. IN NO EVENT SHALL IMATIX CORPORATION BE
    LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR
    CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF
    SUBSTITUTE GOODS OR SERVICES; LOSS OF USE, DATA, OR PROFITS; OR
    BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY OF
    LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT (INCLUDING
    NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF THIS
    SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.

    These header files use macros to implement a split inclusion in which all
    safe definitions (those that do not depend on the presence of other
    definitions) are done first, and all unsafe definitions are done in a
    second pass through the same headers.

    The first header file included from the main C program defines itself as
    the "root" and thus controls the inclusion of the safe/unsafe pieces of
    the other headers.
 *---------------------------------------------------------------------------*/

#if !defined (ICL_IMPORT_HEADERS) || (ICL_IMPORT_HEADERS == 1)
# ifndef INCLUDE_ICL_SYSTEM_SAFE
#   define INCLUDE_ICL_SYSTEM_SAFE
#   define INCLUDE_ICL_SYSTEM_ACTIVE
#   if !defined (ICL_IMPORT_HEADERS)
#       define ICL_IMPORT_ICL_SYSTEM_ROOT
#       define ICL_IMPORT_HEADERS 1
#   endif
#   ifdef __cplusplus
extern "C" {
#   endif

//  Our own class typedef

typedef struct _icl_system_t icl_system_t;

#define ICL_SYSTEM_ALIVE                0xFABB
#define ICL_SYSTEM_DEAD                 0xDEAD

#define ICL_SYSTEM_ASSERT_SANE(self)\
{\
    if (!self) {\
        icl_system_panic ("", "FATAL ERROR at %s:%u, in %s\n", __FILE__, __LINE__, ICL_ASSERT_SANE_FUNCTION);\
        icl_system_panic ("", "Attempting to work with a NULL icl_system\n");\
        icl_system_panic ("", "Please report this to openamq-dev@lists.openamq.org\n");\
        abort ();\
    }\
    if (self->object_tag != ICL_SYSTEM_ALIVE) {\
        icl_system_panic ("", "FATAL ERROR at %s:%u, in %s\n", __FILE__, __LINE__, ICL_ASSERT_SANE_FUNCTION);\
        icl_system_panic ("", "icl_system at %p expected object_tag=0x%x, actual object_tag=0x%x\n", self, ICL_SYSTEM_ALIVE, self->object_tag);\
        icl_system_panic ("", "Please report this to openamq-dev@lists.openamq.org\n");\
        abort ();\
    }\
}
//  Types

typedef void (icl_system_fn) (void);    //  Call-back function

//  Global variables

extern int
    g_argc;
extern char
    **g_argv;
#   ifdef __cplusplus
}
#   endif

#   include "icl_destroy_list.h"
#   include "icl_system_list.h"
#   include "icl_mem.h"
#   include "icl_system.h"
#   ifdef __cplusplus
extern "C" {
#   endif

#   ifdef __cplusplus
}
#   endif
#   undef INCLUDE_ICL_SYSTEM_ACTIVE
#   if defined (ICL_IMPORT_ICL_SYSTEM_ROOT)
#       undef  ICL_IMPORT_HEADERS
#       define ICL_IMPORT_HEADERS 2
#   endif
# endif
#endif

#if !defined (ICL_IMPORT_HEADERS) || (ICL_IMPORT_HEADERS == 2)
# ifndef INCLUDE_ICL_SYSTEM_UNSAFE
#   define INCLUDE_ICL_SYSTEM_UNSAFE
#   define INCLUDE_ICL_SYSTEM_ACTIVE

#   include "icl_destroy_list.h"
#   include "icl_mem.h"
#   include "icl_system.h"

#   ifdef __cplusplus
extern "C" {
#   endif
//  Global variables

extern Bool
    icl_system_animating;

//  Structure of the icl_system object

struct _icl_system_t {
dbyte
    object_tag;                     //  Object validity marker
volatile icl_system_list_t *list_head;
volatile icl_system_t *list_prev;
volatile icl_system_t *list_next;
icl_system_fn *purge;               //  Purge cache call-back
icl_system_fn *destroy;             //  Destroy cache call-back
};

#   ifdef __cplusplus
}
#   endif
#   include "icl_system_list.h"
#   ifdef __cplusplus
extern "C" {
#   endif
//  Method prototypes
void
    icl_system_initialise (
int argc,                               //  The number of program arguments
char ** argv                            //  The program arguments
);

#define icl_system_new()                icl_system_new_ (__FILE__, __LINE__)
icl_system_t *
    icl_system_new_ (
char * file,                            //  Source file for call
size_t line                             //  Line number for call
);

#define icl_system_register(purge,destroy)  icl_system_register_ (__FILE__, __LINE__, purge, destroy)
void
    icl_system_register_ (
char * file,                            //  Source file for call
size_t line,                            //  Line number for call
icl_system_fn * purge,                  //  Address of cache purge function in calling class
icl_system_fn * destroy                 //  Address of cache destroy function in calling class
);

void
    icl_system_purge (
void);

void
    icl_system_terminate (
void);

void
    icl_system_panic (
char * caller,                          //  Name of caller
char * format,                          //  Format specifier
...                                     //  Arguments to print
);

void
    icl_system_panic_v (
char * caller,                          //  Name of caller
char * format,                          //  Format specifier
va_list args                            //  Arguments to print
);

char *
    icl_system_name (
char * name                             //  Name of platform
);

void
    icl_system_selftest (
void);

void
    icl_system_remove_from_all_containers (
icl_system_t * self                     //  The item
);

#define icl_system_destroy(self)        icl_system_destroy_ (self, __FILE__, __LINE__)
void
    icl_system_destroy_ (
icl_system_t * ( * self_p ),            //  Reference to object reference
char * file,                            //  Source fileSource file
size_t line                             //  Line numberLine number
);

void
    icl_system_show_animation (
Bool enabled                            //  Are we enabling or disabling animation?
);

#define icl_system_new_in_scope(self_p,_scope)  icl_system_new_in_scope_ (self_p, _scope, __FILE__, __LINE__)
void
    icl_system_new_in_scope_ (
icl_system_t * * self_p,                //  Not documented
icl_scope_t * _scope,                   //  Not documented
char * file,                            //  Source file for call
size_t line                             //  Line number for call
);


#   ifdef __cplusplus
}
#   endif
#   undef INCLUDE_ICL_SYSTEM_ACTIVE
#   if defined (ICL_IMPORT_ICL_SYSTEM_ROOT)
#       undef  ICL_IMPORT_HEADERS
#       define ICL_IMPORT_HEADERS 3
#   endif
# endif
#endif

#if !defined (ICL_IMPORT_HEADERS) || (ICL_IMPORT_HEADERS == 3)
# if !defined (INCLUDE_ICL_SYSTEM_INLINE)
#   define INCLUDE_ICL_SYSTEM_INLINE
#   define INCLUDE_ICL_SYSTEM_ACTIVE

#   include "icl_destroy_list.h"
#   include "icl_system_list.h"
#   include "icl_mem.h"
#   include "icl_system.h"

#   ifdef __cplusplus
extern "C" {
#   endif

#   ifdef __cplusplus
}
#   endif
#   undef INCLUDE_ICL_SYSTEM_ACTIVE
#   if defined (ICL_IMPORT_ICL_SYSTEM_ROOT)
#       undef ICL_IMPORT_HEADERS
#       undef ICL_IMPORT_ICL_SYSTEM_ROOT
#   endif
# endif
#endif
/*---------------------------------------------------------------------------
    icl_shortstr.h - icl_shortstr component

    We define the icl_shortstr_t type in order to provide a simple way of
    storing and serialising short strings.  A "short string" is defined
    as one that can be serialised as a single-octet length indicator
    plus 0-255 octets of data.  The icl_shortstr_t type is held in memory
    as C-compatible null-terminated char arrays of 255+1 bytes.  There
    are no constructor/destructor methods for icl_shortstr_t variables since
    this would create incompatabilities with normal C strings.  See the
    selftest method for examples of using icl_shortstr_t.  When writing a
    icl_shortstr_t you should always use the icl_shortstr_cpy method to
    avoid the risk of overflows.
    Generated from icl_shortstr.icl by icl_gen using GSL/4.
    
    Copyright (c) 1996-2009 iMatix Corporation
    All rights reserved.
    
    This file is licensed under the BSD license as follows:
    
    Redistribution and use in source and binary forms, with or without
    modification, are permitted provided that the following conditions
    are met:
    
    * Redistributions of source code must retain the above copyright
      notice, this list of conditions and the following disclaimer.
    * Redistributions in binary form must reproduce the above copyright
      notice, this list of conditions and the following disclaimer in
      the documentation and/or other materials provided with the
      distribution.
    * Neither the name of iMatix Corporation nor the names of its
      contributors may be used to endorse or promote products derived
      from this software without specific prior written permission.
    
    THIS SOFTWARE IS PROVIDED BY IMATIX CORPORATION "AS IS" AND ANY
    EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE
    IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR
    PURPOSE ARE DISCLAIMED. IN NO EVENT SHALL IMATIX CORPORATION BE
    LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR
    CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF
    SUBSTITUTE GOODS OR SERVICES; LOSS OF USE, DATA, OR PROFITS; OR
    BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY OF
    LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT (INCLUDING
    NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF THIS
    SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.

    These header files use macros to implement a split inclusion in which all
    safe definitions (those that do not depend on the presence of other
    definitions) are done first, and all unsafe definitions are done in a
    second pass through the same headers.

    The first header file included from the main C program defines itself as
    the "root" and thus controls the inclusion of the safe/unsafe pieces of
    the other headers.
 *---------------------------------------------------------------------------*/

#if !defined (ICL_IMPORT_HEADERS) || (ICL_IMPORT_HEADERS == 1)
# ifndef INCLUDE_ICL_SHORTSTR_SAFE
#   define INCLUDE_ICL_SHORTSTR_SAFE
#   define INCLUDE_ICL_SHORTSTR_ACTIVE
#   if !defined (ICL_IMPORT_HEADERS)
#       define ICL_IMPORT_ICL_SHORTSTR_ROOT
#       define ICL_IMPORT_HEADERS 1
#   endif
#   ifdef __cplusplus
extern "C" {
#   endif

#define ICL_SHORTSTR_MAX    255
typedef char icl_shortstr_t [ICL_SHORTSTR_MAX + 1];
#   ifdef __cplusplus
}
#   endif

#   ifdef __cplusplus
extern "C" {
#   endif

#   ifdef __cplusplus
}
#   endif
#   undef INCLUDE_ICL_SHORTSTR_ACTIVE
#   if defined (ICL_IMPORT_ICL_SHORTSTR_ROOT)
#       undef  ICL_IMPORT_HEADERS
#       define ICL_IMPORT_HEADERS 2
#   endif
# endif
#endif

#if !defined (ICL_IMPORT_HEADERS) || (ICL_IMPORT_HEADERS == 2)
# ifndef INCLUDE_ICL_SHORTSTR_UNSAFE
#   define INCLUDE_ICL_SHORTSTR_UNSAFE
#   define INCLUDE_ICL_SHORTSTR_ACTIVE


#   ifdef __cplusplus
extern "C" {
#   endif
//  Global variables

extern Bool
    icl_shortstr_animating;
#   ifdef __cplusplus
}
#   endif
#   ifdef __cplusplus
extern "C" {
#   endif
//  Method prototypes
size_t
    icl_shortstr_cpy (
char * dest,                            //  String to copy into
char * src                              //  String to copy from
);

size_t
    icl_shortstr_cat (
char * dest,                            //  String to copy into
char * src                              //  String to copy from
);

size_t
    icl_shortstr_ncpy (
char * dest,                            //  String to copy into
char * src,                             //  String to copy from
size_t size                             //  Length of source string
);

size_t
    icl_shortstr_ncat (
char * dest,                            //  String to copy into
char * src,                             //  String to copy from
size_t size                             //  Length of source string
);

char *
    icl_shortstr_fmt (
char * dest,                            //  String to format into
char * format,                          //  Format specifier
...                                     //  Variable arguments
);

char *
    icl_shortstr_fmt_v (
char * dest,                            //  String to format into
char * format,                          //  Format specifier
va_list args                            //  Variable arguments
);

size_t
    icl_shortstr_read (
char * string,                          //  Short string to fill
FILE * file                             //  File to read from
);

void
    icl_shortstr_selftest (
void);

void
    icl_shortstr_show_animation (
Bool enabled                            //  Are we enabling or disabling animation?
);


#   ifdef __cplusplus
}
#   endif
#   undef INCLUDE_ICL_SHORTSTR_ACTIVE
#   if defined (ICL_IMPORT_ICL_SHORTSTR_ROOT)
#       undef  ICL_IMPORT_HEADERS
#       define ICL_IMPORT_HEADERS 3
#   endif
# endif
#endif

#if !defined (ICL_IMPORT_HEADERS) || (ICL_IMPORT_HEADERS == 3)
# if !defined (INCLUDE_ICL_SHORTSTR_INLINE)
#   define INCLUDE_ICL_SHORTSTR_INLINE
#   define INCLUDE_ICL_SHORTSTR_ACTIVE


#   ifdef __cplusplus
extern "C" {
#   endif

#   ifdef __cplusplus
}
#   endif
#   undef INCLUDE_ICL_SHORTSTR_ACTIVE
#   if defined (ICL_IMPORT_ICL_SHORTSTR_ROOT)
#       undef ICL_IMPORT_HEADERS
#       undef ICL_IMPORT_ICL_SHORTSTR_ROOT
#   endif
# endif
#endif
/*---------------------------------------------------------------------------
    icl_longstr.h - icl_longstr component

    We define the icl_longstr_t type in order to provide a secure way of
    storing and serialising long strings.  A "long string" is defined
    as one that can be serialised as a 32-bit length indicator
    plus 0-4Gb-1 octets of data. The icl_longstr_t type is held in memory
    as descriptor consisting of a size field and a data reference.
    Generated from icl_longstr.icl by icl_gen using GSL/4.
    
    Copyright (c) 1996-2009 iMatix Corporation
    All rights reserved.
    
    This file is licensed under the BSD license as follows:
    
    Redistribution and use in source and binary forms, with or without
    modification, are permitted provided that the following conditions
    are met:
    
    * Redistributions of source code must retain the above copyright
      notice, this list of conditions and the following disclaimer.
    * Redistributions in binary form must reproduce the above copyright
      notice, this list of conditions and the following disclaimer in
      the documentation and/or other materials provided with the
      distribution.
    * Neither the name of iMatix Corporation nor the names of its
      contributors may be used to endorse or promote products derived
      from this software without specific prior written permission.
    
    THIS SOFTWARE IS PROVIDED BY IMATIX CORPORATION "AS IS" AND ANY
    EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE
    IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR
    PURPOSE ARE DISCLAIMED. IN NO EVENT SHALL IMATIX CORPORATION BE
    LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR
    CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF
    SUBSTITUTE GOODS OR SERVICES; LOSS OF USE, DATA, OR PROFITS; OR
    BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY OF
    LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT (INCLUDING
    NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF THIS
    SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.

    These header files use macros to implement a split inclusion in which all
    safe definitions (those that do not depend on the presence of other
    definitions) are done first, and all unsafe definitions are done in a
    second pass through the same headers.

    The first header file included from the main C program defines itself as
    the "root" and thus controls the inclusion of the safe/unsafe pieces of
    the other headers.
 *---------------------------------------------------------------------------*/

#if !defined (ICL_IMPORT_HEADERS) || (ICL_IMPORT_HEADERS == 1)
# ifndef INCLUDE_ICL_LONGSTR_SAFE
#   define INCLUDE_ICL_LONGSTR_SAFE
#   define INCLUDE_ICL_LONGSTR_ACTIVE
#   if !defined (ICL_IMPORT_HEADERS)
#       define ICL_IMPORT_ICL_LONGSTR_ROOT
#       define ICL_IMPORT_HEADERS 1
#   endif
#   ifdef __cplusplus
extern "C" {
#   endif

//  Our own class typedef

typedef struct _icl_longstr_t icl_longstr_t;
#define IPR_LONGSTR(s)     { sizeof(s), (s) }
#define ICL_LONGSTR_ALIVE               0xFABB
#define ICL_LONGSTR_DEAD                0xDEAD
#define ICL_LONGSTR_ASSERT_SANE(self)\
{\
    if (!self) {\
        icl_system_panic ("", "FATAL ERROR at %s:%u, in %s\n", __FILE__, __LINE__, ICL_ASSERT_SANE_FUNCTION);\
        icl_system_panic ("", "Attempting to work with a NULL icl_longstr\n");\
        icl_system_panic ("", "Please report this to openamq-dev@lists.openamq.org\n");\
        abort ();\
    }\
    if (self->object_tag != ICL_LONGSTR_ALIVE) {\
        icl_system_panic ("", "FATAL ERROR at %s:%u, in %s\n", __FILE__, __LINE__, ICL_ASSERT_SANE_FUNCTION);\
        icl_system_panic ("", "icl_longstr at %p expected object_tag=0x%x, actual object_tag=0x%x\n", self, ICL_LONGSTR_ALIVE, self->object_tag);\
        icl_system_panic ("", "Please report this to openamq-dev@lists.openamq.org\n");\
        abort ();\
    }\
}
#   ifdef __cplusplus
}
#   endif

#   include "icl_mem.h"
#   include "icl_system.h"
#   ifdef __cplusplus
extern "C" {
#   endif

#   ifdef __cplusplus
}
#   endif
#   undef INCLUDE_ICL_LONGSTR_ACTIVE
#   if defined (ICL_IMPORT_ICL_LONGSTR_ROOT)
#       undef  ICL_IMPORT_HEADERS
#       define ICL_IMPORT_HEADERS 2
#   endif
# endif
#endif

#if !defined (ICL_IMPORT_HEADERS) || (ICL_IMPORT_HEADERS == 2)
# ifndef INCLUDE_ICL_LONGSTR_UNSAFE
#   define INCLUDE_ICL_LONGSTR_UNSAFE
#   define INCLUDE_ICL_LONGSTR_ACTIVE

#   include "icl_mem.h"
#   include "icl_system.h"

#   ifdef __cplusplus
extern "C" {
#   endif
//  Global variables

extern Bool
    icl_longstr_animating;

//  Structure of the icl_longstr object

struct _icl_longstr_t {
qbyte
    object_tag;                     //  Object validity marker

size_t cur_size;                    //  Current size of data part
size_t max_size;                    //  Maximum allocated size
byte  *data;                        //  Data part follows here
};

#   ifdef __cplusplus
}
#   endif
#   ifdef __cplusplus
extern "C" {
#   endif
//  Method prototypes
#define icl_longstr_new(data,size)      icl_longstr_new_ (__FILE__, __LINE__, data, size)
icl_longstr_t *
    icl_longstr_new_ (
char * file,                            //  Source file for call
size_t line,                            //  Line number for call
void * data,                            //  Data to copy
size_t size                             //  Length of data
);

#define icl_longstr_destroy(self)       icl_longstr_destroy_ (self, __FILE__, __LINE__)
void
    icl_longstr_destroy_ (
icl_longstr_t * ( * self_p ),           //  Reference to object reference
char * file,                            //  Source file
size_t line                             //  Line number
);

#define icl_longstr_resize(self,size)   icl_longstr_resize_ (self, size, __FILE__, __LINE__)
int
    icl_longstr_resize_ (
icl_longstr_t * self,                   //  Reference to object
size_t size,                            //  New length of data
char * file,                            //  Source file for call
size_t line                             //  Line number for call
);

int
    icl_longstr_eq (
icl_longstr_t * self,                   //  First string
icl_longstr_t * second                  //  String to compare with
);

icl_longstr_t *
    icl_longstr_dup (
icl_longstr_t * self                    //  Source string
);

int
    icl_longstr_fmt (
icl_longstr_t * self,                   //  Reference to object
char * format,                          //  Format specifier
...                                     //  Variable arguments
);

int
    icl_longstr_fmt_v (
icl_longstr_t * self,                   //  Reference to object
char * format,                          //  Format specifier
va_list args                            //  Variable arguments
);

int
    icl_longstr_cat (
icl_longstr_t * self,                   //  Reference to object
char * format,                          //  Format specifier
...                                     //  Variable arguments
);

int
    icl_longstr_cat_v (
icl_longstr_t * self,                   //  Reference to object
char * format,                          //  Format specifier
va_list args                            //  Variable arguments
);

byte *
    icl_longstr_find (
icl_longstr_t * self,                   //  Reference to longstr
size_t offset,                          //  Start searching at this offset
byte * pattern,                         //  Pattern to look for
size_t patsize                          //  Size of pattern, in octets
);

void
    icl_longstr_selftest (
void);

void
    icl_longstr_show_animation (
Bool enabled                            //  Are we enabling or disabling animation?
);


#   ifdef __cplusplus
}
#   endif
#   undef INCLUDE_ICL_LONGSTR_ACTIVE
#   if defined (ICL_IMPORT_ICL_LONGSTR_ROOT)
#       undef  ICL_IMPORT_HEADERS
#       define ICL_IMPORT_HEADERS 3
#   endif
# endif
#endif

#if !defined (ICL_IMPORT_HEADERS) || (ICL_IMPORT_HEADERS == 3)
# if !defined (INCLUDE_ICL_LONGSTR_INLINE)
#   define INCLUDE_ICL_LONGSTR_INLINE
#   define INCLUDE_ICL_LONGSTR_ACTIVE

#   include "icl_mem.h"
#   include "icl_system.h"

#   ifdef __cplusplus
extern "C" {
#   endif

#   ifdef __cplusplus
}
#   endif
#   undef INCLUDE_ICL_LONGSTR_ACTIVE
#   if defined (ICL_IMPORT_ICL_LONGSTR_ROOT)
#       undef ICL_IMPORT_HEADERS
#       undef ICL_IMPORT_ICL_LONGSTR_ROOT
#   endif
# endif
#endif
