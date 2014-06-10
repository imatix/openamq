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
