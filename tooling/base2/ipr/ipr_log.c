/*---------------------------------------------------------------------------
    ipr_log.c - ipr_log component

    Generated from ipr_log.icl by icl_gen using GSL/4.
    
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
#include "ipr_log.h"                    //  Definitions for our class

//  Shorthands for class methods

#define self_cycle          ipr_log_cycle
#define self_sysopen        ipr_log_sysopen
#define self_sysprint       ipr_log_sysprint
#define self_sysclose       ipr_log_sysclose
#define self_selftest       ipr_log_selftest
#define self_show_animation  ipr_log_show_animation

Bool
    ipr_log_animating = TRUE;           //  Animation enabled by default
static void
    s_timestamp_filename (char *stamped_name, char *filename);
/*  -------------------------------------------------------------------------
    ipr_log_cycle

    Type: Component method
    Cycles an application log file:
     - the file must exist
     - if required we move the file to some directory
     - we generate a unique archive name for the file
     - we rename the file to the archive name
     - if required we run a command on the archived file
     - if the file is empty, it is deleted, and not archived
    -------------------------------------------------------------------------
 */

int
    ipr_log_cycle (
    char * filename,                    //  File to examine
    char * directory,                   //  Archive directory, or NULL
    char * command                      //  Command to run
)
{
icl_shortstr_t
    stamped_name,                   //  File name, time-stamped
    destination;                    //  New name of file
ipr_process_t
    *process;                       //  Cycler process
    int
        rc = 0;                         //  Not documented

//

//
assert (filename);
if (ipr_file_exists (filename)) {
    if (ipr_file_size (filename)) {
        icl_shortstr_cpy (destination, filename);
        if (directory) {
            ipr_file_set_path (destination, directory);
            ipr_file_rename (destination, filename);
        }
        s_timestamp_filename (stamped_name, destination);
        rc = ipr_file_rename (stamped_name, destination);
        if (command && *command) {
            icl_shortstr_fmt (destination, "%s %s", command, stamped_name);
            process = ipr_process_new (destination, NULL, NULL, NULL);
            ipr_process_start (process, ".");
            ipr_process_wait  (process, TRUE);
            rc = ipr_process_exitcode (process);
            ipr_process_destroy (&process);
        }
    }
    else
        //  To cycle an empty file, we delete it
        ipr_file_delete (filename);
}
else
    rc = -1;

    return (rc);
}
/*  -------------------------------------------------------------------------
    ipr_log_sysopen

    Type: Component method
    Prepares syslog usage, on systems where it is supported.  Should be
    called exactly once in an application.  Returns zero if ok, -1 if
    syslog is not available.
    -------------------------------------------------------------------------
 */

int
    ipr_log_sysopen (
    char * identifier                   //  Log identifier
)
{
    int
        rc = 0;                         //  Not documented

    //
#   if defined (LOG_DAEMON)
    openlog (identifier, 0, LOG_DAEMON);
#   else
    rc = -1;
#   endif

    return (rc);
}
/*  -------------------------------------------------------------------------
    ipr_log_sysprint

    Type: Component method
    Sends one line of data to syslog, using the specified priority.  If
    syslog is not available, prints to the console instead.  Any string
    formatting must be done by the caller, so the string may safely
    contain '%', it will not be expanded.
    -------------------------------------------------------------------------
 */

void
    ipr_log_sysprint (
    int priority,                       //  Log priority
    char * string                       //  String to log
)
{

    //
#if defined (LOG_DAEMON)
    syslog (priority, "%s", string);
#else
    icl_console_print ("%s", string);
#endif
}
/*  -------------------------------------------------------------------------
    ipr_log_sysclose

    Type: Component method
    Closes syslog usage, on systems where it is supported.  May be
    called at most once in an application.
    -------------------------------------------------------------------------
 */

void
    ipr_log_sysclose (
void)
{

#if defined (LOG_DAEMON)
    closelog ();
#endif
}
/*  -------------------------------------------------------------------------
    ipr_log_selftest

    Type: Component method
    -------------------------------------------------------------------------
 */

void
    ipr_log_selftest (
void)
{

//
//ipr_log_cycle ("logs/normal.log", "archive", "zip -m packed");
//ipr_log_cycle ("empty.log", "archive", "zip -m packed");
ipr_log_sysopen ("ipr_log_test");
ipr_log_sysprint (IPR_LOG_SYSERROR,   "This is an error %s!");
ipr_log_sysprint (IPR_LOG_SYSWARNING, "This is a warning");
ipr_log_sysprint (IPR_LOG_SYSINFO,    "This is information");
ipr_log_sysprint (IPR_LOG_SYSDEBUG,   "This is debug output");
ipr_log_sysclose ();
}
/*  -------------------------------------------------------------------------
    ipr_log_show_animation

    Type: Component method
    Enables animation of the component. Animation is sent to stdout.
    To enable animation you must generate using the option -animate:1.
    -------------------------------------------------------------------------
 */

void
    ipr_log_show_animation (
    Bool enabled                        //  Are we enabling or disabling animation?
)
{

ipr_log_animating = enabled;
}
//  Add current date and time to filename, before extension
static void
s_timestamp_filename (char *stamped_name, char *filename)
{
    char
        *extension,
        *end_of_name;
    icl_shortstr_t
        suffix;
    apr_time_t
        time_now;
    apr_time_exp_t
        time_exp;
    int
        instance = 0;

    //  Get base filename up to extension
    extension = strchr (filename, '.');
    if (extension)
        icl_shortstr_ncpy (stamped_name, filename, extension - filename);
    else
        icl_shortstr_cpy (stamped_name, filename);

    time_now = apr_time_now ();
    assert (apr_time_exp_lt (&time_exp, time_now) == APR_SUCCESS);
    icl_shortstr_fmt (suffix, "-%04d%02d%02d-%02d%02d%02d",
        time_exp.tm_year + 1900, time_exp.tm_mon  + 1,
        time_exp.tm_mday, time_exp.tm_hour,
        time_exp.tm_min, time_exp.tm_sec);

    //  Append time stamp to base filename
    icl_shortstr_cat (stamped_name, suffix);

    //  Append extension, if any
    if (extension)
        icl_shortstr_cat (stamped_name, extension);

    end_of_name = stamped_name + strlen (stamped_name);
    while (ipr_file_exists (stamped_name)) {
        *end_of_name = 0;               //  Remove any prior instance
        icl_shortstr_fmt (suffix, "#%d", ++instance);
        icl_shortstr_cat (stamped_name, suffix);
    }
}

//  Embed the version information in the resulting binary

char *EMBED__ipr_log_version_start        = "VeRsIoNsTaRt:ipc";
char *EMBED__ipr_log_component            = "ipr_log ";
char *EMBED__ipr_log_version              = "1.0 ";
char *EMBED__ipr_log_copyright            = "Copyright (c) 1996-2009 iMatix Corporation";
char *EMBED__ipr_log_filename             = "ipr_log.icl ";
char *EMBED__ipr_log_builddate            = "2011/03/01 ";
char *EMBED__ipr_log_version_end          = "VeRsIoNeNd:ipc";

