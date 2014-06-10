<?xml?>
<!--
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
 -->
<class
    name      = "icl_console"
    comment   = "iPR console class"
    version   = "1.1"
    script    = "icl_gen"
    target    = "stdc"
    >

<doc>
    Defines the console interface for iCL.
</doc>

<public name = "header">
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
</public>

<private>
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

</private>

<method name = "mode">
    <doc>
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
    </doc>
    <argument name = "attr"  type = "icl_console_option_t">The option to set or clear</argument>
    <argument name = "value" type = "Bool">The value for the option</argument>
    switch (attr) {
        case ICL_CONSOLE_THREAD: s_thread = value; break;
        case ICL_CONSOLE_DATE  : s_date   = value; break;
        case ICL_CONSOLE_TIME  : s_time   = value; break;
        case ICL_CONSOLE_MILLI : s_milli  = value; break;
        case ICL_CONSOLE_MICRO : s_micro  = value; break;
        case ICL_CONSOLE_FLUSH : s_flush  = value; break;
        case ICL_CONSOLE_QUIET : s_quiet  = value; break;
    }
</method>

<method name = "redirect">
    <doc>
    Redirects console output to a FILE stream.  Default is standard error.
    </doc>
    <argument name = "file" type = "FILE *">The stream to redirect the console to</argument>
    s_file = file;
</method>

<method name = "print">
    <doc>
    Print a message to the console.

    The arguments to this function are similar to printf(), for example:

        $(selfname)_print ("%s: %d", "string", 123);

    If a mode is set so the time is printed, it will use the current time.
    A newline is added automatically.

    To redirect output from the default of standard error, see
    $(selfname)_redirect.
    </doc>
    <argument name = "format" type = "char *" >Format specifier</argument>
    <argument name = "argptr" type = "...">Variable arguments</argument>
    //
#if (defined (BASE_THREADSAFE))
    icl_console_print_thread_time_argptr (
        stderr, apr_os_thread_current (), apr_time_now (), format, argptr);
#else
    icl_console_print_thread_time_argptr (stderr, 0, apr_time_now (), format, argptr);
#endif
    va_end (argptr);
</method>

<method name = "out">
    <doc>
    Print a message to the console.

    Works identically to $(selfname)_print but uses standard output rather
    than standard error by default.
    </doc>
    <argument name = "format" type = "char *" >Format specifier</argument>
    <argument name = "argptr" type = "...">Variable arguments</argument>
    //
#if (defined (BASE_THREADSAFE))
    icl_console_print_thread_time_argptr (
        stdout, apr_os_thread_current (), apr_time_now (), format, argptr);
#else
    icl_console_print_thread_time_argptr (stdout, 0, apr_time_now (), format, argptr);
#endif
    va_end (argptr);
</method>

<method name = "print thread time">
    <doc>
    Print a message to the console.

    Works identically to $(selfname)_print but allows the caller to
    specify the time and OS thread ID to print in the message.
    </doc>
    <argument name = "os thread" type = "icl_os_thread_t">The thread info to print</argument>
    <argument name = "time"   type = "apr_time_t">The date/time to print</argument>
    <argument name = "format" type = "char *" >Format specifier</argument>
    <argument name = "argptr" type = "...">Variable arguments</argument>
    //
    icl_console_print_thread_time_argptr (stderr, os_thread, time, format, argptr);
</method>

<method name = "print thread time argptr" private = "1">
    <argument name = "stream"    type = "FILE *">Default output stream</argument>
    <argument name = "os thread" type = "icl_os_thread_t">The thread info to print</argument>
    <argument name = "time"      type = "apr_time_t">The date/time to print</argument>
    <argument name = "format"    type = "char *" >Format specifier</argument>
    <argument name = "argptr"    type = "va_list">The variable argument pointer</argument>
    <local>
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
    </local>
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
    length += apr_snprintf (buffer + length, LINE_LENGTH - length, "\\n");
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
</method>

<method name = "selftest">
    <local>
    FILE
        *logfile;                       //  Console output will be sent here
    </local>

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
    icl_console_print ("printing with console mode ICL_CONSOLE_MICRO == TRUE" \
                                             " and ICL_CONSOLE_DATE == TRUE");

    //  Turn both modes off again
    icl_console_mode (ICL_CONSOLE_MICRO, FALSE);
    icl_console_mode (ICL_CONSOLE_DATE, FALSE);

    //  Redirect output to STDOUT
    fclose (logfile);
    icl_console_redirect (stdout);
</method>

</class>
