/*---------------------------------------------------------------------------
    smt_log.c - smt_log component

    Defines a log. This is a lock-free asynchronous class.
    Generated from smt_log.icl by smt_object_gen using GSL/4.
    
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
#include "smt_log.h"                    //  Definitions for our class

//  Shorthand for class type

#define self_t              smt_log_t

//  Shorthands for class methods

#define self_new            smt_log_new
#define self_annihilate     smt_log_annihilate
#define self_open           smt_log_open
#define self_close          smt_log_close
#define self_set_syslog     smt_log_set_syslog
#define self_set_copyto     smt_log_set_copyto
#define self_set_stderr     smt_log_set_stderr
#define self_set_raw        smt_log_set_raw
#define self_print          smt_log_print
#define self_write_bucket   smt_log_write_bucket
#define self_cycle          smt_log_cycle
#define self_dump           smt_log_dump
#define self_dump_env       smt_log_dump_env
#define self_selftest       smt_log_selftest
#define self_destroy        smt_log_destroy
#define self_unlink         smt_log_unlink
#define self_show_animation  smt_log_show_animation
#define self_free           smt_log_free
#define self_initialise     smt_log_initialise
#define self_terminate      smt_log_terminate
#define self_show           smt_log_show
#define self_alloc          smt_log_alloc
#define self_link           smt_log_link
#define self_cache_initialise  smt_log_cache_initialise
#define self_cache_purge    smt_log_cache_purge
#define self_cache_terminate  smt_log_cache_terminate
#define self_new_in_scope   smt_log_new_in_scope

#define smt_log_annihilate(self)        smt_log_annihilate_ (self, __FILE__, __LINE__)
static int
    smt_log_annihilate_ (
smt_log_t * ( * self_p ),               //  Reference to object reference
char * file,                            //  Source file
size_t line                             //  Line number
);

static void
    smt_log_initialise (
void);

#define smt_log_alloc()                 smt_log_alloc_ (__FILE__, __LINE__)
static smt_log_t *
    smt_log_alloc_ (
char * file,                            //  Source file for call
size_t line                             //  Line number for call
);

static void
    smt_log_cache_initialise (
void);

static void
    smt_log_cache_purge (
void);

static void
    smt_log_cache_terminate (
void);

Bool
    smt_log_animating = TRUE;           //  Animation enabled by default
static Bool
    s_smt_log_active = FALSE;
#if (defined (BASE_THREADSAFE))
static icl_mutex_t
    *s_smt_log_mutex          = NULL;
#endif
static icl_cache_t
    *s_cache = NULL;


/*  -------------------------------------------------------------------------
    smt_log_new

    Type: Component method
    Creates and initialises a new smt_log_t object, returns a
    reference to the created object.
    -------------------------------------------------------------------------
 */

smt_log_t *
    smt_log_new_ (
    char * file,                        //  Source file for call
    size_t line                         //  Line number for call
)
{
    smt_log_t *
        self = NULL;                    //  Object reference

if (!s_smt_log_active)
    self_initialise ();
    self = smt_log_alloc_ (file, line);
    if (self) {
        self->object_tag   = SMT_LOG_ALIVE;
        self->links        = 1;
        self->zombie       = FALSE;
#if defined (DEBUG) || defined (BASE_HISTORY) || defined (BASE_HISTORY_SMT_LOG)
        self->history_last = 0;

        //  Track possession operation in history
        self->history_file  [0] = file;
        self->history_line  [0] = line;
        self->history_type  [0] = "new";
        self->history_links [0] = self->links;
#endif
#if defined (DEBUG)
        icl_mem_set_callback (self, smt_log_show_);
#endif

self->thread = smt_log_agent_class_thread_new (self);
self->thread->animate = TRUE;
}

    return (self);
}
/*  -------------------------------------------------------------------------
    smt_log_annihilate

    Type: Component method
    -------------------------------------------------------------------------
 */

static int
    smt_log_annihilate_ (
    smt_log_t * ( * self_p ),           //  Reference to object reference
    char * file,                        //  Source file
    size_t line                         //  Line number
)
{
#if defined (DEBUG) || defined (BASE_HISTORY) || defined (BASE_HISTORY_SMT_LOG)
    int
        history_last;
#endif

    smt_log_t *
        self = *self_p;                 //  Dereferenced Reference to object reference
    int
        rc = 0;                         //  Return code

#if defined (DEBUG) || defined (BASE_HISTORY) || defined (BASE_HISTORY_SMT_LOG)
    //  Track possession operation in history
    history_last = icl_atomic_inc32 ((volatile qbyte *) &self->history_last) + 1;
    self->history_file  [history_last % SMT_LOG_HISTORY_LENGTH] = file;
    self->history_line  [history_last % SMT_LOG_HISTORY_LENGTH] = line;
    self->history_type  [history_last % SMT_LOG_HISTORY_LENGTH] = "destroy";
    self->history_links [history_last % SMT_LOG_HISTORY_LENGTH] = self->links;
#endif


if (self) {
    assert (self->thread);
    if (smt_log_agent_destroy (self->thread,file,line)) {
        //icl_console_print ("Error sending 'destroy' method to smt_log agent");
        rc = -1;
    }
}
else
    rc = -1;

    return (rc);
}
/*  -------------------------------------------------------------------------
    smt_log_open

    Type: Component method
    Accepts a smt_log_t reference and returns zero in case of success,
    1 in case of errors.
    Standard function template for asynchronous functions.
    Opens the log file output stream, so that data is written to disk.
    If you do not open the log file, data sent to it will be thrown
    away.
    -------------------------------------------------------------------------
 */

int
    smt_log_open (
    smt_log_t * self,                   //  Reference to object
    char * filename,                    //  Log filename
    char * log_path,                    //  Log path name
    char * cycle_cmd                    //  Cycle command
)
{
    int
        rc = 0;                         //  Return code

SMT_LOG_ASSERT_SANE (self);
if (!self->zombie) {

    //

    //

if (self) {
    assert (self->thread);
    if (smt_log_agent_open (self->thread,filename,log_path,cycle_cmd)) {
        //icl_console_print ("Error sending 'open' method to smt_log agent");
        rc = -1;
    }
}
else
    rc = -1;
}
else
    rc = -1;                        //  Return error on zombie object.


    return (rc);
}
/*  -------------------------------------------------------------------------
    smt_log_close

    Type: Component method
    Accepts a smt_log_t reference and returns zero in case of success,
    1 in case of errors.
    Standard function template for asynchronous functions.
    Closes the log file output, if any.
    -------------------------------------------------------------------------
 */

int
    smt_log_close (
    smt_log_t * self                    //  Reference to object
)
{
    int
        rc = 0;                         //  Return code

SMT_LOG_ASSERT_SANE (self);
if (!self->zombie) {

    //

if (self) {
    assert (self->thread);
    if (smt_log_agent_close (self->thread)) {
        //icl_console_print ("Error sending 'close' method to smt_log agent");
        rc = -1;
    }
}
else
    rc = -1;
}
else
    rc = -1;                        //  Return error on zombie object.


    return (rc);
}
/*  -------------------------------------------------------------------------
    smt_log_set_syslog

    Type: Component method
    Accepts a smt_log_t reference and returns zero in case of success,
    1 in case of errors.
    Standard function template for asynchronous functions.
    Sets the log file to write to syslog, and the priority for messages
    sent to syslog via this log file.  All log files are sent to the
    same syslog facility (LOG_DAEMON).  Do not combine with normal log
    file open.
    -------------------------------------------------------------------------
 */

int
    smt_log_set_syslog (
    smt_log_t * self,                   //  Reference to object
    int priority                        //  Syslog priority
)
{
    int
        rc = 0;                         //  Return code

SMT_LOG_ASSERT_SANE (self);
if (!self->zombie) {

if (self) {
    assert (self->thread);
    if (smt_log_agent_set_syslog (self->thread,priority)) {
        //icl_console_print ("Error sending 'set syslog' method to smt_log agent");
        rc = -1;
    }
}
else
    rc = -1;
}
else
    rc = -1;                        //  Return error on zombie object.


    return (rc);
}
/*  -------------------------------------------------------------------------
    smt_log_set_copyto

    Type: Component method
    Accepts a smt_log_t reference and returns zero in case of success,
    1 in case of errors.
    Set a copy-to log file.  All data printed to the current log file
    will also be sent to the copyto log, in an identical format.  If
    null, the copy-to functionality for this log file is disabled.  Not
    threadsafe; this method should not be called from multiple threads
    at once. It is implemented as a synchronous method so that it takes
    effect immediately.
    -------------------------------------------------------------------------
 */

int
    smt_log_set_copyto (
    smt_log_t * self,                   //  Reference to object
    smt_log_t * copyto                  //  Copy to logfile
)
{
    int
        rc = 0;                         //  Return code

SMT_LOG_ASSERT_SANE (self);
if (!self->zombie) {

//
//  Don't duplicate when sending to syslog, there's no point
if (!self->tosyslog)
    self->copyto = copyto;
}
else
    rc = -1;                        //  Return error on zombie object.


    return (rc);
}
/*  -------------------------------------------------------------------------
    smt_log_set_stderr

    Type: Component method
    Accepts a smt_log_t reference and returns zero in case of success,
    1 in case of errors.
    Set the stderr option, in which all data sent to the log file will also
    be copied to stderr, and flushed. This is most useful for errors and
    traces, where you don't want to lose cached output if the application
    ends unexpectedly.  Not threadsafe; this method should not be called
    from multiple threads at once. It is implemented as a synchronous
    method so that it takes effect immediately.
    -------------------------------------------------------------------------
 */

int
    smt_log_set_stderr (
    smt_log_t * self,                   //  Reference to object
    Bool option                         //  TRUE or FALSE
)
{
    int
        rc = 0;                         //  Return code

SMT_LOG_ASSERT_SANE (self);
if (!self->zombie) {

//
self->tostderr = option;
}
else
    rc = -1;                        //  Return error on zombie object.


    return (rc);
}
/*  -------------------------------------------------------------------------
    smt_log_set_raw

    Type: Component method
    Accepts a smt_log_t reference and returns zero in case of success,
    1 in case of errors.
    Set the raw option, in which data will not be printed with date
    and time.
    -------------------------------------------------------------------------
 */

int
    smt_log_set_raw (
    smt_log_t * self,                   //  Reference to object
    Bool option                         //  TRUE or FALSE
)
{
    int
        rc = 0;                         //  Return code

SMT_LOG_ASSERT_SANE (self);
if (!self->zombie) {

//
self->raw = option;
}
else
    rc = -1;                        //  Return error on zombie object.


    return (rc);
}
/*  -------------------------------------------------------------------------
    smt_log_print

    Type: Component method
    Writes a line of text to the log file, formatting the string using the
    provided format and arguments. The string is prefixed by the current
    date and time unless the raw option was set.  The log file object can
    be null, in which case this method does nothing.
    -------------------------------------------------------------------------
 */

void
    smt_log_print (
    smt_log_t * self,                   //  Log file object
    char * format,                      //  Format specifier
...                                     //  Variable arguments
)
{
    va_list
        args;
    va_start (args, format);
    smt_log_print_v (self, format, args );
    va_end (args);
}
/*  -------------------------------------------------------------------------
    smt_log_print_v

    Type: Component method
    Writes a line of text to the log file, formatting the string using the
    provided format and arguments. The string is prefixed by the current
    date and time unless the raw option was set.  The log file object can
    be null, in which case this method does nothing.
    -------------------------------------------------------------------------
 */

void
    smt_log_print_v (
    smt_log_t * self,                   //  Log file object
    char * format,                      //  Format specifier
va_list args                            //  Variable arguments
)
{
apr_time_exp_t
    time_exp;
apr_size_t
    fmtsize;                        //  Size of formatted string
ipr_bucket_t
    *bucket;                        //  Bucket to write

//
if (self) {
    bucket = ipr_bucket_new (SMT_LOG_LINEMAX + 1);
    if (self->raw)
        apr_vsnprintf ((char *) bucket->data, SMT_LOG_LINEMAX, format, args);
    else {
        apr_time_exp_lt (&time_exp, apr_time_now ());
        apr_strftime  ((char *) (bucket->data), &fmtsize, SMT_LOG_LINEMAX, "%Y-%m-%d %T: ", &time_exp);
        apr_vsnprintf ((char *) (bucket->data) + strlen ((char *) (bucket->data)),
            SMT_LOG_LINEMAX - strlen ((char *) (bucket->data)), format, args);
    }
    if (self->tostderr) {
        //  Always printf into %s to guard against %'s in data
        fprintf (stderr, "%s", (char *) (bucket->data));
        fprintf (stderr, "\n");
        fflush  (stderr);
    }
    smt_log_write_bucket (self, bucket);
    if (self->copyto)
        smt_log_write_bucket (self->copyto, bucket);
    ipr_bucket_unlink (&bucket);
}
}
/*  -------------------------------------------------------------------------
    smt_log_write_bucket

    Type: Component method
    Accepts a smt_log_t reference and returns zero in case of success,
    1 in case of errors.
    Standard function template for asynchronous functions.
    Writes a bucket of text to the log file.  The text is not modified or
    prefixed in any way.  Does not flush the log file, so if the server
    is halted abruptly, logging data may be lost.  The bucket text must
    be null-terminated correctly.
    -------------------------------------------------------------------------
 */

int
    smt_log_write_bucket (
    smt_log_t * self,                   //  Reference to object
    ipr_bucket_t * bucket               //  Bucket data to print
)
{
    int
        rc = 0;                         //  Return code

SMT_LOG_ASSERT_SANE (self);
if (!self->zombie) {

    //

if (self) {
    assert (self->thread);
    if (smt_log_agent_write_bucket (self->thread,bucket)) {
        //icl_console_print ("Error sending 'write bucket' method to smt_log agent");
        rc = -1;
    }
}
else
    rc = -1;
}
else
    rc = -1;                        //  Return error on zombie object.


    return (rc);
}
/*  -------------------------------------------------------------------------
    smt_log_cycle

    Type: Component method
    Accepts a smt_log_t reference and returns zero in case of success,
    1 in case of errors.
    Standard function template for asynchronous functions.
    Archives the log file explicitly.
    -------------------------------------------------------------------------
 */

int
    smt_log_cycle (
    smt_log_t * self                    //  Reference to object
)
{
    int
        rc = 0;                         //  Return code

SMT_LOG_ASSERT_SANE (self);
if (!self->zombie) {

if (self) {
    assert (self->thread);
    if (smt_log_agent_cycle (self->thread)) {
        //icl_console_print ("Error sending 'cycle' method to smt_log agent");
        rc = -1;
    }
}
else
    rc = -1;
}
else
    rc = -1;                        //  Return error on zombie object.


    return (rc);
}
/*  -------------------------------------------------------------------------
    smt_log_dump

    Type: Component method
    Accepts a smt_log_t reference and returns zero in case of success,
    1 in case of errors.
    Prints the contents of a bucket to the specified log file.  Filters
    unprintable characters.  Does not apply any specific formatting.
    -------------------------------------------------------------------------
 */

int
    smt_log_dump (
    smt_log_t * self,                   //  Reference to object
    ipr_bucket_t * bucket,              //  Bucket to dump
    char * prefix                       //  Prefix for each line
)
{
#   define BYTES_PER_LINE   (16)
    static const char
        *hex_digit = "0123456789abcdef";
    size_t
        size = bucket->cur_size,
        count,
        repeat_count = 0;
    byte
        *ucbuffer = bucket->data,
        cur_byte;
    char
        last_buffer [(BYTES_PER_LINE * 2) + 1],
        hex_buffer  [(BYTES_PER_LINE * 2) + 1],
        text_buffer [ BYTES_PER_LINE + 1],
        *hex,
        *text;
    int
        remainder;
    int
        rc = 0;                         //  Return code

SMT_LOG_ASSERT_SANE (self);
if (!self->zombie) {

//
memset (last_buffer, 0, sizeof (last_buffer));
while (size > 0) {
    hex       = hex_buffer;
    text      = text_buffer;
    count     = BYTES_PER_LINE;
    remainder = BYTES_PER_LINE - size;
    while (size > 0 && count > 0) {
        size--;
        count--;
        cur_byte = *ucbuffer;

        //  Is current byte a printable character?
        *text++ = (cur_byte < 32 || cur_byte > 127) ? '.' : cur_byte;
        *hex++  = hex_digit [cur_byte >> 4];
        *hex++  = hex_digit [cur_byte & 0x0f];
        ucbuffer++;
    }
    while (remainder > 0) {
        *hex++ = ' ';
        *hex++ = ' ';
        remainder--;
    }
    *hex = '\0';
    *text = '\0';
    if (memcmp (last_buffer, hex_buffer, sizeof (last_buffer)) == 0)
        repeat_count++;
    else {
        if (repeat_count) {
            self_print (self, "%s[repeated %d times]", prefix, repeat_count);
            repeat_count = 0;
        }
        self_print (self, "%s%s | %s", prefix, hex_buffer, text_buffer);
        memcpy (last_buffer, hex_buffer, sizeof (last_buffer));
    }
}
if (repeat_count)
    self_print (self, "%s[repeated %d times]", prefix, repeat_count);
}
else
    rc = -1;                        //  Return error on zombie object.


    return (rc);
}
/*  -------------------------------------------------------------------------
    smt_log_dump_env

    Type: Component method
    Accepts a smt_log_t reference and returns zero in case of success,
    1 in case of errors.
    Prints the current environment to the log file.
    -------------------------------------------------------------------------
 */

int
    smt_log_dump_env (
    smt_log_t * self                    //  Reference to object
)
{
int
    string_nbr;                     //  Index into string table
    int
        rc = 0;                         //  Return code

SMT_LOG_ASSERT_SANE (self);
if (!self->zombie) {

//
self_print (self, "****************  Process Environment Variables  ****************");
for (string_nbr = 0; environ [string_nbr]; string_nbr++)
    self_print (self, "%s", environ [string_nbr]);
self_print (self, "");
}
else
    rc = -1;                        //  Return error on zombie object.


    return (rc);
}
/*  -------------------------------------------------------------------------
    smt_log_selftest

    Type: Component method
    -------------------------------------------------------------------------
 */

void
    smt_log_selftest (
void)
{

}
/*  -------------------------------------------------------------------------
    smt_log_destroy

    Type: Component method
    -------------------------------------------------------------------------
 */

int
    smt_log_destroy_ (
    smt_log_t * ( * self_p ),           //  Reference to object reference
    char * file,                        //  Source file
    size_t line                         //  Line number
)
{
    smt_log_t *
        self = *self_p;                 //  Dereferenced Reference to object reference
    int
        rc = 0;                         //  Return code

if (self) {
    if (icl_atomic_cas32 (&self->zombie, TRUE, FALSE) == FALSE)
        rc = smt_log_annihilate_ (self_p, file, line);
    else
    if (icl_atomic_dec32 ((volatile qbyte *) &self->links) == 0)
        smt_log_free (self);
    *self_p = NULL;
}

    return (rc);
}
/*  -------------------------------------------------------------------------
    smt_log_unlink

    Type: Component method
    Removes a link (reference count) to an object.  Sets the object pointer to NULL
    to indicate that it is no longer valid.
    -------------------------------------------------------------------------
 */

void
    smt_log_unlink_ (
    smt_log_t * ( * self_p ),           //  Reference to object reference
    char * file,                        //  Source file for call
    size_t line                         //  Line number for call
)
{
#if defined (DEBUG) || defined (BASE_HISTORY) || defined (BASE_HISTORY_SMT_LOG)
    int
        history_last;
#endif
    smt_log_t *
        self = *self_p;                 //  Dereferenced Reference to object reference

    if (self) {
        if (self->links == 0) {
            icl_console_print ("Missing link on smt_log object");
            smt_log_show_ (self, ICL_CALLBACK_DUMP, stderr, file, line);
        }
        assert (self->links > 0);

#if defined (DEBUG) || defined (BASE_HISTORY) || defined (BASE_HISTORY_SMT_LOG)
        //  Track possession operation in history.  Pre-empt value of links
        //  after operation; otherwise race condition can result in writing
        //  to freed memory.
        history_last = icl_atomic_inc32 ((volatile qbyte *) &self->history_last) + 1;
        self->history_file  [history_last % SMT_LOG_HISTORY_LENGTH] = file;
        self->history_line  [history_last % SMT_LOG_HISTORY_LENGTH] = line;
        self->history_type  [history_last % SMT_LOG_HISTORY_LENGTH] = "unlink";
        self->history_links [history_last % SMT_LOG_HISTORY_LENGTH] = self->links - 1;
#endif

        if (icl_atomic_dec32 ((volatile qbyte *) &self->links) == 0) {
            if (self->zombie)
                smt_log_free (self);
            else {
                //  JS: Have to make the object look like it was called from the
                //      application.  _destroy will decrement links again.
                icl_atomic_inc32 ((volatile qbyte *) &self->links);
                smt_log_destroy_ (self_p, file, line);
            }
        }
        else
            *self_p = NULL;
    }
}
/*  -------------------------------------------------------------------------
    smt_log_show_animation

    Type: Component method
    Enables animation of the component. Animation is sent to stdout.
    To enable animation you must generate using the option -animate:1.
    -------------------------------------------------------------------------
 */

void
    smt_log_show_animation (
    Bool enabled                        //  Are we enabling or disabling animation?
)
{

smt_log_animating = enabled;

smt_log_agent_animate (enabled);
}
/*  -------------------------------------------------------------------------
    smt_log_free

    Type: Component method
    Freess a smt_log_t object.
    -------------------------------------------------------------------------
 */

void
    smt_log_free_ (
    smt_log_t * self,                   //  Object reference
    char * file,                        //  Source file
    size_t line                         //  Line number
)
{
#if defined (DEBUG) || defined (BASE_HISTORY) || defined (BASE_HISTORY_SMT_LOG)
    int
        history_last;
#endif


    if (self) {
#if defined (DEBUG) || defined (BASE_HISTORY) || defined (BASE_HISTORY_SMT_LOG)
        //  Track possession operation in history
        history_last = icl_atomic_inc32 ((volatile qbyte *) &self->history_last) + 1;
        self->history_file  [history_last % SMT_LOG_HISTORY_LENGTH] = file;
        self->history_line  [history_last % SMT_LOG_HISTORY_LENGTH] = line;
        self->history_type  [history_last % SMT_LOG_HISTORY_LENGTH] = "free";
        self->history_links [history_last % SMT_LOG_HISTORY_LENGTH] = self->links;
#endif

smt_thread_unlink (&self->thread);
        memset (&self->object_tag, 0, sizeof (smt_log_t) - ((byte *) &self->object_tag - (byte *) self));
//        memset (self, 0, sizeof (smt_log_t));
        self->object_tag = SMT_LOG_DEAD;
        icl_mem_free (self);
    }
    self = NULL;
}
/*  -------------------------------------------------------------------------
    smt_log_initialise

    Type: Component method
    -------------------------------------------------------------------------
 */

static void
    smt_log_initialise (
void)
{

    //  Test for already active before applying any locks; avoids deadlock in
    //  some classes
    if (!s_smt_log_active) {

#if (defined (BASE_THREADSAFE))
        //  First make sure the object mutex has been created
        if (!icl_global_mutex) {
            icl_system_panic ("icl_init", "iCL not initialised - call icl_system_initialise()\n");
            abort ();
        }
        apr_thread_mutex_lock (icl_global_mutex);
        if (!s_smt_log_mutex)
            s_smt_log_mutex = icl_mutex_new ();
        apr_thread_mutex_unlock (icl_global_mutex);

        //  Now lock the object mutex
        icl_mutex_lock   (s_smt_log_mutex);

        //  Test again for already active now that we hold the lock
        if (!s_smt_log_active) {
#endif
            //  Register the class termination call-back functions
            icl_system_register (NULL, self_terminate);

smt_log_agent_init ();
            s_smt_log_active = TRUE;
#if (defined (BASE_THREADSAFE))
        }
        icl_mutex_unlock (s_smt_log_mutex);
#endif

    }
}
/*  -------------------------------------------------------------------------
    smt_log_terminate

    Type: Component method
    -------------------------------------------------------------------------
 */

void
    smt_log_terminate (
void)
{

if (s_smt_log_active) {

#if (defined (BASE_THREADSAFE))
        icl_mutex_destroy (&s_smt_log_mutex);
#endif
        s_smt_log_active = FALSE;
    }
}
/*  -------------------------------------------------------------------------
    smt_log_show

    Type: Component method
    -------------------------------------------------------------------------
 */

void
    smt_log_show_ (
    void * item,                        //  The opaque pointer
    int opcode,                         //  The callback opcode
    FILE * trace_file,                  //  File to print to
    char * file,                        //  Source file
    size_t line                         //  Line number
)
{
    smt_log_t
        *self;
    int
        container_links;
#if defined (DEBUG) || defined (BASE_HISTORY) || defined (BASE_HISTORY_SMT_LOG)
    qbyte
        history_index;
#endif


self = item;
container_links = 0;
    assert (opcode == ICL_CALLBACK_DUMP);

    fprintf (trace_file, "    <smt_log zombie = \"%u\" links = \"%u\" containers = \"%u\" file = \"%s\" line = \"%lu\"  pointer = \"%p\" />\n", self->zombie, self->links, container_links, file, (unsigned long) line, self);
#if defined (DEBUG) || defined (BASE_HISTORY) || defined (BASE_HISTORY_SMT_LOG)
    if (self->history_last > SMT_LOG_HISTORY_LENGTH) {
        fprintf (trace_file, "        <!-- possess history too large (%d) - call iMatix-tech -->\n",
            self->history_last);
        history_index = (self->history_last + 1) % SMT_LOG_HISTORY_LENGTH;
        self->history_last %= SMT_LOG_HISTORY_LENGTH;
    }
    else
        history_index = 0;

    for (; history_index != self->history_last; history_index = (history_index + 1) % SMT_LOG_HISTORY_LENGTH) {
        fprintf (trace_file, "       <%s file = \"%s\" line = \"%lu\" links = \"%lu\" />\n",
            self->history_type [history_index],
            self->history_file [history_index],
            (unsigned long) self->history_line  [history_index],
            (unsigned long) self->history_links [history_index]);
    }
    fprintf (trace_file, "    </smt_log>\n");
#endif

}
/*  -------------------------------------------------------------------------
    smt_log_alloc

    Type: Component method
    -------------------------------------------------------------------------
 */

static smt_log_t *
    smt_log_alloc_ (
    char * file,                        //  Source file for call
    size_t line                         //  Line number for call
)
{

    smt_log_t *
        self = NULL;                    //  Object reference

//  Initialise cache if necessary
if (!s_cache)
    smt_log_cache_initialise ();

self = (smt_log_t *) icl_mem_cache_alloc_ (s_cache, file, line);
if (self)
    memset (self, 0, sizeof (smt_log_t));



    return (self);
}
/*  -------------------------------------------------------------------------
    smt_log_link

    Type: Component method
    Adds a link (reference count) to an object.

    If the object has been zombified (ie destroyed while extra links were present),
    this method returns NULL and does not add any links.

    This method does not lock the object.
    -------------------------------------------------------------------------
 */

smt_log_t *
    smt_log_link_ (
    smt_log_t * self,                   //  Not documented
    char * file,                        //  Source file for call
    size_t line                         //  Line number for call
)
{
#if defined (DEBUG) || defined (BASE_HISTORY) || defined (BASE_HISTORY_SMT_LOG)
    int
        history_last;
#endif

    if (self) {
        SMT_LOG_ASSERT_SANE (self);
        if (file)
            icl_mem_possess_ (self, file, line);
        icl_atomic_inc32 ((volatile qbyte *) &self->links);
#if defined (DEBUG) || defined (BASE_HISTORY) || defined (BASE_HISTORY_SMT_LOG)
        //  Track possession operation in history
        history_last = icl_atomic_inc32 ((volatile qbyte *) &self->history_last) + 1;
        self->history_file  [history_last % SMT_LOG_HISTORY_LENGTH] = file;
        self->history_line  [history_last % SMT_LOG_HISTORY_LENGTH] = line;
        self->history_type  [history_last % SMT_LOG_HISTORY_LENGTH] = "link";
        self->history_links [history_last % SMT_LOG_HISTORY_LENGTH] = self->links;
#endif
    }

    return (self);
}
/*  -------------------------------------------------------------------------
    smt_log_cache_initialise

    Type: Component method
    Initialise the cache and register purge method with the meta-cache.
    -------------------------------------------------------------------------
 */

static void
    smt_log_cache_initialise (
void)
{

s_cache = icl_cache_get (sizeof (smt_log_t));
icl_system_register (smt_log_cache_purge, smt_log_cache_terminate);
}
/*  -------------------------------------------------------------------------
    smt_log_cache_purge

    Type: Component method
    -------------------------------------------------------------------------
 */

static void
    smt_log_cache_purge (
void)
{

icl_mem_cache_purge (s_cache);

}
/*  -------------------------------------------------------------------------
    smt_log_cache_terminate

    Type: Component method
    -------------------------------------------------------------------------
 */

static void
    smt_log_cache_terminate (
void)
{

s_cache = NULL;

}
/*  -------------------------------------------------------------------------
    smt_log_new_in_scope

    Type: Component method
    -------------------------------------------------------------------------
 */

void
    smt_log_new_in_scope_ (
    smt_log_t * * self_p,               //  Not documented
    icl_scope_t * _scope,               //  Not documented
    char * file,                        //  Source file for call
    size_t line                         //  Line number for call
)
{
    icl_destroy_t *
        _destroy;                       //  Not documented

*self_p = smt_log_new_ (file,line);

if (*self_p) {
    _destroy = icl_destroy_new   ((void * *) self_p, (icl_destructor_fn *) smt_log_destroy_);
    icl_destroy_list_queue (_scope, _destroy);
    icl_destroy_unlink (&_destroy);
}
}
//  Embed the version information in the resulting binary

char *EMBED__smt_log_version_start        = "VeRsIoNsTaRt:ipc";
char *EMBED__smt_log_component            = "smt_log ";
char *EMBED__smt_log_version              = "1.0 ";
char *EMBED__smt_log_copyright            = "Copyright (c) 1996-2009 iMatix Corporation";
char *EMBED__smt_log_filename             = "smt_log.icl ";
char *EMBED__smt_log_builddate            = "2011/03/01 ";
char *EMBED__smt_log_version_end          = "VeRsIoNeNd:ipc";

