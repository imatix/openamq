/*---------------------------------------------------------------------------
    icl_console.c - icl_console component

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
 *---------------------------------------------------------------------------*/

#include "icl.h"                        //  iCL base classes
#include "icl_console.h"                //  Definitions for our class

//  Shorthands for class methods

#define self_mode           icl_console_mode
#define self_redirect       icl_console_redirect
#define self_print          icl_console_print
#define self_out            icl_console_out
#define self_print_thread_time  icl_console_print_thread_time
#define self_print_thread_time_argptr  icl_console_print_thread_time_argptr
#define self_selftest       icl_console_selftest

static void
    icl_console_print_thread_time_argptr (
FILE * stream,                          //  Default output stream
icl_os_thread_t os_thread,              //  The thread info to print
apr_time_t time,                        //  The date/time to print
char * format,                          //  Format specifier
va_list argptr                          //  The variable argument pointer
);

#define LINE_LENGTH 1024

#include "icl_mem.h"

static Bool
    s_thread = FALSE,
    s_date   = FALSE,
    s_time   = TRUE,
    s_milli  = FALSE,
    s_micro  = FALSE,
    s_flush  = TRUE,
    s_quiet  = FALSE;                   //  Console switched off

static FILE
    *s_file = NULL;
/*  -------------------------------------------------------------------------
    icl_console_mode

    Type: Component method
    Console modes change the way the text is presented.  For example, you
    can prepend every string with a date.  The following modes are
    available:

    ICL_CONSOLE_DATE:
        Prints the current date.

    ICL_CONSOLE_TIME:
        Prints the current time.  Enabled by default.

    ICL_CONSOLE_MILLI:
        Prints the current time, with milliseconds.

    ICL_CONSOLE_MICRO:
        Prints the current time, with microseconds.  Takes precedence over
        ICL_CONSOLE_MILLI if set.

    ICL_CONSOLE_THREAD:
        Prints the calling OS thread ID.

    ICL_CONSOLE_FLUSH:
        Flushes the output stream after each call.  Enabled by default.

    Modes can be combined together by OR-ing the values.  The mode setting
    lasts for the lifetime of the icl_console class.

    Note that ICL_CONSOLE_MILLI and ICL_CONSOLE_MICRO depend on
    ICL_CONSOLE_DATE or ICL_CONSOLE_TIME being TRUE.
    -------------------------------------------------------------------------
 */

void
    icl_console_mode (
    icl_console_option_t attr,          //  The option to set or clear
    Bool value                          //  The value for the option
)
{

switch (attr) {
    case ICL_CONSOLE_THREAD: s_thread = value; break;
    case ICL_CONSOLE_DATE  : s_date   = value; break;
    case ICL_CONSOLE_TIME  : s_time   = value; break;
    case ICL_CONSOLE_MILLI : s_milli  = value; break;
    case ICL_CONSOLE_MICRO : s_micro  = value; break;
    case ICL_CONSOLE_FLUSH : s_flush  = value; break;
    case ICL_CONSOLE_QUIET : s_quiet  = value; break;
}
}
/*  -------------------------------------------------------------------------
    icl_console_redirect

    Type: Component method
    Redirects console output to a FILE stream.  Default is standard error.
    -------------------------------------------------------------------------
 */

void
    icl_console_redirect (
    FILE * file                         //  The stream to redirect the console to
)
{

s_file = file;
}
/*  -------------------------------------------------------------------------
    icl_console_print

    Type: Component method
    Print a message to the console.

    The arguments to this function are similar to printf(), for example:

        icl_console_print ("%s: %d", "string", 123);

    If a mode is set so the time is printed, it will use the current time.
    A newline is added automatically.

    To redirect output from the default of standard error, see
    icl_console_redirect.
    -------------------------------------------------------------------------
 */

void
    icl_console_print (
    char * format,                      //  Format specifier
...                                     //  Variable arguments
)
{
    va_list
        argptr;
    va_start (argptr, format);
    icl_console_print_v (format, argptr );
    va_end (argptr);
}
/*  -------------------------------------------------------------------------
    icl_console_print_v

    Type: Component method
    Print a message to the console.

    The arguments to this function are similar to printf(), for example:

        icl_console_print ("%s: %d", "string", 123);

    If a mode is set so the time is printed, it will use the current time.
    A newline is added automatically.

    To redirect output from the default of standard error, see
    icl_console_redirect.
    -------------------------------------------------------------------------
 */

void
    icl_console_print_v (
    char * format,                      //  Format specifier
va_list argptr                          //  Variable arguments
)
{

    //
#if (defined (BASE_THREADSAFE))
    icl_console_print_thread_time_argptr (
        stderr, apr_os_thread_current (), apr_time_now (), format, argptr);
#else
    icl_console_print_thread_time_argptr (stderr, 0, apr_time_now (), format, argptr);
#endif
    va_end (argptr);
}
/*  -------------------------------------------------------------------------
    icl_console_out

    Type: Component method
    Print a message to the console.

    Works identically to icl_console_print but uses standard output rather
    than standard error by default.
    -------------------------------------------------------------------------
 */

void
    icl_console_out (
    char * format,                      //  Format specifier
...                                     //  Variable arguments
)
{
    va_list
        argptr;
    va_start (argptr, format);
    icl_console_out_v (format, argptr );
    va_end (argptr);
}
/*  -------------------------------------------------------------------------
    icl_console_out_v

    Type: Component method
    Print a message to the console.

    Works identically to icl_console_print but uses standard output rather
    than standard error by default.
    -------------------------------------------------------------------------
 */

void
    icl_console_out_v (
    char * format,                      //  Format specifier
va_list argptr                          //  Variable arguments
)
{

    //
#if (defined (BASE_THREADSAFE))
    icl_console_print_thread_time_argptr (
        stdout, apr_os_thread_current (), apr_time_now (), format, argptr);
#else
    icl_console_print_thread_time_argptr (stdout, 0, apr_time_now (), format, argptr);
#endif
    va_end (argptr);
}
/*  -------------------------------------------------------------------------
    icl_console_print_thread_time

    Type: Component method
    Print a message to the console.

    Works identically to icl_console_print but allows the caller to
    specify the time and OS thread ID to print in the message.
    -------------------------------------------------------------------------
 */

void
    icl_console_print_thread_time (
    icl_os_thread_t os_thread,          //  The thread info to print
    apr_time_t time,                    //  The date/time to print
    char * format,                      //  Format specifier
...                                     //  Variable arguments
)
{
    va_list
        argptr;
    va_start (argptr, format);
    icl_console_print_thread_time_v (os_thread, time, format, argptr );
    va_end (argptr);
}
/*  -------------------------------------------------------------------------
    icl_console_print_thread_time_v

    Type: Component method
    Print a message to the console.

    Works identically to icl_console_print but allows the caller to
    specify the time and OS thread ID to print in the message.
    -------------------------------------------------------------------------
 */

void
    icl_console_print_thread_time_v (
    icl_os_thread_t os_thread,          //  The thread info to print
    apr_time_t time,                    //  The date/time to print
    char * format,                      //  Format specifier
va_list argptr                          //  Variable arguments
)
{

//
icl_console_print_thread_time_argptr (stderr, os_thread, time, format, argptr);
}
/*  -------------------------------------------------------------------------
    icl_console_print_thread_time_argptr

    Type: Component method
    -------------------------------------------------------------------------
 */

static void
    icl_console_print_thread_time_argptr (
    FILE * stream,                      //  Default output stream
    icl_os_thread_t os_thread,          //  The thread info to print
    apr_time_t time,                    //  The date/time to print
    char * format,                      //  Format specifier
    va_list argptr                      //  The variable argument pointer
)
{
#   define DATE_FORMAT "%Y-%m-%d"
#   define TIME_FORMAT "%T"
    char
        date_time_format [sizeof (DATE_FORMAT " " TIME_FORMAT)],
        fstring [24],
        buffer [LINE_LENGTH];
    uint
        length = 0;
    apr_size_t
        flen;
    apr_time_exp_t
        date_time;
    char
        *last;

    //
#if (defined (BASE_THREADSAFE))
    if (s_thread) {
        length = apr_snprintf (buffer + length, LINE_LENGTH - length, "[%li] ", (long int) os_thread);
        if (length > LINE_LENGTH)
            length = LINE_LENGTH;
    }
#endif
    if (s_date || s_time) {
        icl_apr_assert (apr_time_exp_lt (&date_time, time));
        date_time_format[0] = 0;
        if (s_date)
            strcat (date_time_format, DATE_FORMAT);
        if (s_time)
            strcat (date_time_format, date_time_format[0] ? " " TIME_FORMAT : TIME_FORMAT);
        icl_apr_assert (apr_strftime (fstring, &flen, sizeof (fstring), date_time_format, &date_time));
        length += apr_snprintf (buffer + length, LINE_LENGTH - length, "%s", fstring);
        if (length > LINE_LENGTH)
            length = LINE_LENGTH;

        if (s_micro)
            length += apr_snprintf (buffer + length, LINE_LENGTH - length, ".%06lu", (unsigned long) apr_time_usec (time));
        else
        if (s_milli)
            length += apr_snprintf (buffer + length, LINE_LENGTH - length, ".%03lu", (unsigned long) apr_time_msec (time));

        if (length > LINE_LENGTH)
            length = LINE_LENGTH;
        length += apr_snprintf (buffer + length, LINE_LENGTH - length, ": ");
        if (length > LINE_LENGTH)
            length = LINE_LENGTH;
    }
    length += apr_vsnprintf (buffer + length, LINE_LENGTH - length, format, argptr);
    if (length > LINE_LENGTH)
        length = LINE_LENGTH;
    length += apr_snprintf (buffer + length, LINE_LENGTH - length, "\n");
    if (length > LINE_LENGTH)
        length = LINE_LENGTH;
    buffer [LINE_LENGTH - 1] = 0;

    //  Crop trailing spaces in buffer
    last = buffer + strlen (buffer);
    while (last > buffer) {
        if (*(last - 1) != ' ')
            break;
        last--;
    }
    *last = 0;
    if (!s_quiet) {
        fprintf (s_file? s_file: stream, "%s", buffer);
        if (s_flush)
            fflush (s_file? s_file: stream);
    }
}
/*  -------------------------------------------------------------------------
    icl_console_selftest

    Type: Component method
    -------------------------------------------------------------------------
 */

void
    icl_console_selftest (
void)
{
FILE
    *logfile;                       //  Console output will be sent here

//  Redirect output to log file
logfile = fopen ("icl_console.log","w");
icl_console_redirect (logfile);

//  Print a simple 'hello world'
icl_console_print ("hello world");

//  Print using the printf format specifiers
icl_console_print ("hello planet '%s'","earth");

//  Turn on ICL_CONSOLE_MICRO
icl_console_mode (ICL_CONSOLE_MICRO, TRUE);
icl_console_print ("printing with console mode ICL_CONSOLE_MICRO == TRUE");

//  Combine it with another mode, ie: ICL_CONSOLE_DATE
icl_console_mode (ICL_CONSOLE_DATE, TRUE);
icl_console_print ("printing with console mode ICL_CONSOLE_MICRO == TRUE"   " and ICL_CONSOLE_DATE == TRUE");

//  Turn both modes off again
icl_console_mode (ICL_CONSOLE_MICRO, FALSE);
icl_console_mode (ICL_CONSOLE_DATE, FALSE);

//  Redirect output to STDOUT
fclose (logfile);
icl_console_redirect (stdout);
}
//  Embed the version information in the resulting binary

char *EMBED__icl_console_version_start    = "VeRsIoNsTaRt:ipc";
char *EMBED__icl_console_component        = "icl_console ";
char *EMBED__icl_console_version          = "1.1 ";
char *EMBED__icl_console_copyright        = "Copyright (c) 1996-2009 iMatix Corporation";
char *EMBED__icl_console_filename         = "icl_console.icl ";
char *EMBED__icl_console_builddate        = "2011/03/01 ";
char *EMBED__icl_console_version_end      = "VeRsIoNeNd:ipc";

