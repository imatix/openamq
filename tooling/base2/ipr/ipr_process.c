/*---------------------------------------------------------------------------
    ipr_process.c - ipr_process component

    Generated from ipr_process.icl by icl_gen using GSL/4.
    
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
#include "ipr_process.h"                //  Definitions for our class

//  Shorthand for class type

#define self_t              ipr_process_t

//  Structure of the ipr_process object

struct _ipr_process_t {
dbyte
    object_tag;                     //  Object validity marker
apr_pool_t
    *pool;                          //  Pool for all allocations
apr_procattr_t
    *attr;                          //  Process attributes
apr_file_t
    *in_file,                       //  Redirected input, if any
    *out_file,                      //  Redirected output, if any
    *err_file;                      //  Redirected error, if any
char
    **argv;                         //  Arguments array
apr_proc_t
    proc;                           //  APR process object
    Bool finished;                      //  Process has finished
    int exitcode;                       //  Process exit code (!0 = failed)
};

//  Shorthands for class methods

#define self_new            ipr_process_new
#define self_annihilate     ipr_process_annihilate
#define self_start          ipr_process_start
#define self_wait           ipr_process_wait
#define self_kill           ipr_process_kill
#define self_server         ipr_process_server
#define self_id             ipr_process_id
#define self_selftest       ipr_process_selftest
#define self_finished       ipr_process_finished
#define self_exitcode       ipr_process_exitcode
#define self_terminate      ipr_process_terminate
#define self_show           ipr_process_show
#define self_destroy        ipr_process_destroy
#define self_alloc          ipr_process_alloc
#define self_free           ipr_process_free
#define self_cache_initialise  ipr_process_cache_initialise
#define self_cache_purge    ipr_process_cache_purge
#define self_cache_terminate  ipr_process_cache_terminate
#define self_show_animation  ipr_process_show_animation
#define self_new_in_scope   ipr_process_new_in_scope

static void
    ipr_process_annihilate (
ipr_process_t * ( * self_p )            //  Reference to object reference
);

#define ipr_process_alloc()             ipr_process_alloc_ (__FILE__, __LINE__)
static ipr_process_t *
    ipr_process_alloc_ (
char * file,                            //  Source file for call
size_t line                             //  Line number for call
);

static void
    ipr_process_free (
ipr_process_t * self                    //  Object reference
);

static void
    ipr_process_cache_initialise (
void);

static void
    ipr_process_cache_purge (
void);

static void
    ipr_process_cache_terminate (
void);

Bool
    ipr_process_animating = TRUE;       //  Animation enabled by default
static icl_cache_t
    *s_cache = NULL;


/*  Macros:
 *  FILEHANDLE_MAX      Maximum possible number of open files
 */

//  getdtablesize () is not available on all systems
#if (defined (__UNIX__))
#   if (defined (__UTYPE_UNIXWARE))
#       define FILEHANDLE_MAX   sysconf (_SC_OPEN_MAX)
#   elif (defined (__UTYPE_HPUX))
#       define FILEHANDLE_MAX   FD_SETSIZE
#   elif (defined (__UTYPE_SINIX))
#       define FILEHANDLE_MAX   FD_SETSIZE
#   else
#       define FILEHANDLE_MAX   getdtablesize ()
#   endif
#elif (defined (FD_SETSIZE))
#   define FILEHANDLE_MAX       FD_SETSIZE
#else
#   define FILEHANDLE_MAX       32      //  Arbitrary for toy OSes
#endif
/*  -------------------------------------------------------------------------
    ipr_process_new

    Type: Component method
    Creates and initialises a new ipr_process_t object, returns a
    reference to the created object.
    Prepares a new process.  To start the process, use the start
    method.
    -------------------------------------------------------------------------
 */

ipr_process_t *
    ipr_process_new_ (
    char * file,                        //  Source file for call
    size_t line,                        //  Line number for call
    char * command,                     //  Full command-line to run
    char * in_file,                     //  Provide standard input, or empty/NULL
    char * out_file,                    //  Capture standard output, or empty/NULL
    char * err_file                     //  Capture standard error, or empty/NULL
)
{
apr_status_t
    rc;
    ipr_process_t *
        self = NULL;                    //  Object reference

    self = ipr_process_alloc_ (file, line);
    if (self) {
        self->object_tag   = IPR_PROCESS_ALIVE;
#if defined (DEBUG)
        icl_mem_set_callback (self, ipr_process_show_);
#endif

//
//  All resources for the process are in a dedicated APR pool
icl_apr_assert (apr_pool_create (&self->pool, icl_global_pool));
apr_pool_tag (self->pool, "ipr_process_new");

rc = apr_procattr_create (&self->attr, self->pool);
assert (rc == APR_SUCCESS);
apr_procattr_cmdtype_set (self->attr, APR_PROGRAM_PATH);
if (in_file && *in_file) {
    apr_file_open (&self->in_file, in_file,
        APR_READ, APR_OS_DEFAULT, self->pool);
    if (self->in_file)              //  File is optional...
        apr_procattr_child_in_set (self->attr, self->in_file, NULL);
}
if (out_file && *out_file) {
    apr_file_open (&self->out_file, out_file,
        APR_READ | APR_WRITE | APR_CREATE | APR_TRUNCATE, APR_OS_DEFAULT, self->pool);
    apr_procattr_child_out_set (self->attr, self->out_file, NULL);
}
if (err_file && *err_file) {
    apr_file_open (&self->err_file, err_file,
        APR_READ | APR_WRITE | APR_CREATE | APR_TRUNCATE, APR_OS_DEFAULT, self->pool);
    apr_procattr_child_err_set (self->attr, self->err_file, NULL);
}
//  Parse commands, build argument list
apr_tokenize_to_argv (command, &self->argv, self->pool);
}

    return (self);
}
/*  -------------------------------------------------------------------------
    ipr_process_annihilate

    Type: Component method
    Ends and destroys the process. If the process was running, sends
    it a signal and waits for it to end.
    -------------------------------------------------------------------------
 */

static void
    ipr_process_annihilate (
    ipr_process_t * ( * self_p )        //  Reference to object reference
)
{

    ipr_process_t *
        self = *self_p;                 //  Dereferenced Reference to object reference

IPR_PROCESS_ASSERT_SANE (self);

//
self_kill (self);
apr_pool_destroy (self->pool);

}
/*  -------------------------------------------------------------------------
    ipr_process_start

    Type: Component method
    Accepts a ipr_process_t reference and returns zero in case of success,
    1 in case of errors.
    Starts the process as specified.  Returns 0 if the process was started
    succesfully, else prints an error message and returns -1.
    -------------------------------------------------------------------------
 */

int
    ipr_process_start (
    ipr_process_t * self,               //  Reference to object
    char * directory                    //  Working directory, or null
)
{
    int
        rc = 0;                         //  Return code

IPR_PROCESS_ASSERT_SANE (self);

//
if (directory && *directory)
    apr_procattr_dir_set (self->attr, directory);

if (apr_proc_create (
    &self->proc,
    self->argv [0],
    (const char *const *) self->argv,
    NULL,
    self->attr,
    self->pool)
== APR_SUCCESS)
    self->finished = FALSE;
else {
    self->exitcode = -1;
    rc = -1;
}


    return (rc);
}
/*  -------------------------------------------------------------------------
    ipr_process_wait

    Type: Component method
    Accepts a ipr_process_t reference and returns zero in case of success,
    1 in case of errors.
    Wait for process to complete and collect its status. If the "sync" argument
    is false, just checks if the process is running.  Returns true if the
    process is still running, false if the process has ended (successfully or
    failed).  The process exit status is in process->exitcode.
    -------------------------------------------------------------------------
 */

int
    ipr_process_wait (
    ipr_process_t * self,               //  Reference to object
    Bool sync                           //  Wait for process to end?
)
{
int
    apr_rc;
apr_exit_why_e
    exitwhy;                        //  Reason for exit
    int
        rc = 0;                         //  Return code

IPR_PROCESS_ASSERT_SANE (self);

//
if (!self->finished) {
    apr_rc = apr_proc_wait (
        &self->proc,
        &self->exitcode,
        &exitwhy,
        sync? APR_WAIT: APR_NOWAIT);
    if (apr_rc == APR_CHILD_NOTDONE)
        rc = TRUE;
    else
    if (apr_rc == APR_CHILD_DONE) {
        rc = FALSE;
        self->finished = TRUE;
    }
}


    return (rc);
}
/*  -------------------------------------------------------------------------
    ipr_process_kill

    Type: Component method
    Accepts a ipr_process_t reference and returns zero in case of success,
    1 in case of errors.
    Kills the process, if running.
    -------------------------------------------------------------------------
 */

int
    ipr_process_kill (
    ipr_process_t * self                //  Reference to object
)
{
    int
        rc = 0;                         //  Return code

IPR_PROCESS_ASSERT_SANE (self);

while (self_wait (self, FALSE)) {
    apr_proc_kill (&self->proc, SIGINT);
    self_wait (self, TRUE);
}
if (self->in_file)
    apr_file_close (self->in_file);
if (self->out_file)
    apr_file_close (self->out_file);
if (self->err_file)
    apr_file_close (self->err_file);


    return (rc);
}
/*  -------------------------------------------------------------------------
    ipr_process_server

    Type: Component method
    Converts the process from an interactive foreground process
    into a background process.  Depending on the system either the existing
    process becomes a background process, or a new process is started as
    a background process and the existing process terminates.

    Requires the original command-line arguments as passed to the main()
    function.  If a new process is started, any arguments that match any
    of the values in the (NULL terminated) 'sswitch' array will be omitted
    from the arguments to the new process.  You should specify the
    command-line switch(es) your main program uses to run as a background
    service.   If it returns, returns 0 if okay, and returns -1 if there
    was an error.

    The precise effect of this function depends on the system.  On UNIX,
    does this:

    - switches the process to run as a background job, under init
    - closes all open files
    - moves to a safe, known working directory, if required
    - sets the umask for new files
    - opens stdin, out_file, and err_file to the null device
    - enforces exclusive execution through a lock file, if required
    - logs the process id in the lock file
    - ignores the hangup unwanted signals

    On other systems does nothing except return 0.
    -------------------------------------------------------------------------
 */

int
    ipr_process_server (
    char * workdir,                     //  Where server runs, or NULL/""
    char * lockfile,                    //  For exclusive execution
    int argc,                           //  Original command-line arguments
    char ** argv,                       //  Original command-line arguments
    char ** sswitch                     //  Filter these options from argv
)
{
    int
        rc = 0;                         //  Not documented

#   if (defined (__UNIX__))
    int
        fork_result,
        file_handle;
    char
        pid_buffer [10];
    struct flock
        lock_file;                      //  flock() argument block
    int
        argi = 0,                       //  Input arguments iterator
        argo = 0;                       //  Output arguments iterator
    char
        **newargv;                      //  Array of new arguments

    //  Copy the arguments across, skipping any in sswitch
    newargv = malloc (argc * sizeof (char *));
    assert (newargv);
    for (argi = argo = 0; argi < argc; argi++) {
        Bool
            copy_argument = TRUE;
        int
            argn = 0;

        for (argn = 0; sswitch != NULL && sswitch [argn] != NULL; argn++) {
            if (strcmp (argv [argi], sswitch [argn]) == 0)
                copy_argument = FALSE;
        }
        if (copy_argument)
            newargv [argo++] = argv [argi];
    }
    //  Terminate the new argument array
    newargv [argo] = NULL;

    /*  Recreate our process as a child of init.  The process continues
        to exit in the background.  UNIX calls wait() for all children of
        the init process, so the server will exit cleanly.                   */

    fork_result = fork ();
    if (fork_result == 0) {
        /*  Close all open file descriptors that may have been inherited
            from the parent process.  This reduces the resources used        */
        for (file_handle = FILEHANDLE_MAX - 1; file_handle >= 0; file_handle--)
            close (file_handle);        //  Ignore errors

        /*  Move to a safe and known directory, which is supplied as an
            argument to this function (or not, if workdir is NULL/empty)     */
        if (workdir && strused (workdir))
            assert (chdir (workdir) == 0);

        //  Set umask so that new files are given mode 750 octal
        umask (027);                    //  Complement of 0750

        /*  Set standard input and output to the null device so that any
            functions that assume that these files are open can still work   */
        file_handle = open ("/dev/null", O_RDWR);    //  stdin = handle 0
        assert (dup (file_handle) != -1);            //  out_file = handle 1
        assert (dup (file_handle) != -1);            //  err_file = handle 2

        //  We ignore any hangup signal from the controlling TTY
        signal (SIGHUP, SIG_IGN);

        /*  Enforce a lock on the lockfile, if specified, so only one copy
            of the server can run at once                                    */
        if (lockfile && strused (lockfile)) {
            file_handle = open (lockfile, O_RDWR | O_CREAT, 0640);
            if (file_handle < 0)
                rc = -1;                //  Could not open lock file
            else {
                lock_file.l_type = F_WRLCK;
                if (fcntl (file_handle, F_SETLK, &lock_file))
                    rc = -1;            //  Could not obtain a lock
            }
            if (rc == 0) {
                //  Record the server's process id in the lock file
                apr_snprintf (pid_buffer, sizeof (pid_buffer), "%6d\n", (int) getpid ());
                assert (write (file_handle, pid_buffer, strlen (pid_buffer)) != -1);
            }
        }
    }
    else
    if (fork_result < 0)                //  < 0 is an error
        rc = -1;                        //  Could not fork
    else
    if (fork_result > 0)                //  > 0 is the parent process
        exit (EXIT_SUCCESS);            //  End parent process
#   else
    rc = 0;
#   endif

    return (rc);
}
/*  -------------------------------------------------------------------------
    ipr_process_id

    Type: Component method
    Returns the current process id, which is an integer on all supported
    systems.  Returns 0 if the system does not support a getpid call.
    -------------------------------------------------------------------------
 */

int
    ipr_process_id (
void)
{
    int
        pid;                            //  Not documented

#if (defined (__WINDOWS__))
    pid = GetCurrentProcessId();
#elif (defined (__UNIX__))
    pid = getpid ();
#else
    pid = 0;
#endif

    return (pid);
}
/*  -------------------------------------------------------------------------
    ipr_process_selftest

    Type: Component method
    -------------------------------------------------------------------------
 */

void
    ipr_process_selftest (
void)
{
ipr_process_t
    *process;
apr_pool_t
    *pool;

process = ipr_process_new ("lsnf", NULL, "ls.out", "ls.err");
ipr_process_start   (process, ".");
ipr_process_wait    (process, TRUE);
assert (ipr_process_exitcode (process) != 0);
ipr_process_destroy (&process);

icl_apr_assert (apr_pool_create (&pool, icl_global_pool));
apr_file_remove ("ls.out", pool);
apr_file_remove ("ls.err", pool);
apr_pool_destroy (pool);
}
/*  -------------------------------------------------------------------------
    ipr_process_finished

    Type: Component method
    -------------------------------------------------------------------------
 */

Bool
    ipr_process_finished (
    ipr_process_t * self                //  Not documented
)
{
    Bool
        finished;                       //  Not documented

finished = self->finished;

    return (finished);
}
/*  -------------------------------------------------------------------------
    ipr_process_exitcode

    Type: Component method
    -------------------------------------------------------------------------
 */

int
    ipr_process_exitcode (
    ipr_process_t * self                //  Not documented
)
{
    int
        exitcode;                       //  Not documented

exitcode = self->exitcode;

    return (exitcode);
}
/*  -------------------------------------------------------------------------
    ipr_process_terminate

    Type: Component method
    -------------------------------------------------------------------------
 */

void
    ipr_process_terminate (
void)
{


}
/*  -------------------------------------------------------------------------
    ipr_process_show

    Type: Component method
    -------------------------------------------------------------------------
 */

void
    ipr_process_show_ (
    void * item,                        //  The opaque pointer
    int opcode,                         //  The callback opcode
    FILE * trace_file,                  //  File to print to
    char * file,                        //  Source file
    size_t line                         //  Line number
)
{
ipr_process_t
    *self;
int
    container_links;


self = item;
container_links = 0;
assert (opcode == ICL_CALLBACK_DUMP);

fprintf (trace_file, "    <ipr_process file = \"%s\" line = \"%lu\"  pointer = \"%p\" />\n", file, (unsigned long) line, self);

}
/*  -------------------------------------------------------------------------
    ipr_process_destroy

    Type: Component method
    Destroys a ipr_process_t object. Takes the address of a
    ipr_process_t reference (a pointer to a pointer) and nullifies the
    reference after use.  Does nothing if the reference is already
    null.
    Performs an agressive destroy of an object.  This involves:
    1. Removing the object from any containers it is in.
    2. Making the object an zombie
    3. Decrementing the object's link count
    4. If the link count is zero then freeing the object.
    -------------------------------------------------------------------------
 */

void
    ipr_process_destroy_ (
    ipr_process_t * ( * self_p ),       //  Reference to object reference
    char * file,                        //  Source fileSource file
    size_t line                         //  Line numberLine number
)
{
    ipr_process_t *
        self = *self_p;                 //  Dereferenced Reference to object reference

if (self) {
    ipr_process_annihilate (self_p);
    ipr_process_free ((ipr_process_t *) self);
    *self_p = NULL;
}
}
/*  -------------------------------------------------------------------------
    ipr_process_alloc

    Type: Component method
    -------------------------------------------------------------------------
 */

static ipr_process_t *
    ipr_process_alloc_ (
    char * file,                        //  Source file for call
    size_t line                         //  Line number for call
)
{

    ipr_process_t *
        self = NULL;                    //  Object reference

//  Initialise cache if necessary
if (!s_cache)
    ipr_process_cache_initialise ();

self = (ipr_process_t *) icl_mem_cache_alloc_ (s_cache, file, line);
if (self)
    memset (self, 0, sizeof (ipr_process_t));



    return (self);
}
/*  -------------------------------------------------------------------------
    ipr_process_free

    Type: Component method
    Freess a ipr_process_t object.
    -------------------------------------------------------------------------
 */

static void
    ipr_process_free (
    ipr_process_t * self                //  Object reference
)
{


if (self) {

        memset (&self->object_tag, 0, sizeof (ipr_process_t) - ((byte *) &self->object_tag - (byte *) self));
//        memset (self, 0, sizeof (ipr_process_t));
        self->object_tag = IPR_PROCESS_DEAD;
        icl_mem_free (self);
    }
    self = NULL;
}
/*  -------------------------------------------------------------------------
    ipr_process_cache_initialise

    Type: Component method
    Initialise the cache and register purge method with the meta-cache.
    -------------------------------------------------------------------------
 */

static void
    ipr_process_cache_initialise (
void)
{

s_cache = icl_cache_get (sizeof (ipr_process_t));
icl_system_register (ipr_process_cache_purge, ipr_process_cache_terminate);
}
/*  -------------------------------------------------------------------------
    ipr_process_cache_purge

    Type: Component method
    -------------------------------------------------------------------------
 */

static void
    ipr_process_cache_purge (
void)
{

icl_mem_cache_purge (s_cache);

}
/*  -------------------------------------------------------------------------
    ipr_process_cache_terminate

    Type: Component method
    -------------------------------------------------------------------------
 */

static void
    ipr_process_cache_terminate (
void)
{

s_cache = NULL;

}
/*  -------------------------------------------------------------------------
    ipr_process_show_animation

    Type: Component method
    Enables animation of the component. Animation is sent to stdout.
    To enable animation you must generate using the option -animate:1.
    -------------------------------------------------------------------------
 */

void
    ipr_process_show_animation (
    Bool enabled                        //  Are we enabling or disabling animation?
)
{

ipr_process_animating = enabled;
}
/*  -------------------------------------------------------------------------
    ipr_process_new_in_scope

    Type: Component method
    -------------------------------------------------------------------------
 */

void
    ipr_process_new_in_scope_ (
    ipr_process_t * * self_p,           //  Not documented
    icl_scope_t * _scope,               //  Not documented
    char * file,                        //  Source file for call
    size_t line,                        //  Line number for call
    char * command,                     //  Full command-line to run
    char * in_file,                     //  Provide standard input, or empty/NULL
    char * out_file,                    //  Capture standard output, or empty/NULL
    char * err_file                     //  Capture standard error, or empty/NULL
)
{
    icl_destroy_t *
        _destroy;                       //  Not documented

*self_p = ipr_process_new_ (file,line,command,in_file,out_file,err_file);

if (*self_p) {
    _destroy = icl_destroy_new   ((void * *) self_p, (icl_destructor_fn *) ipr_process_destroy_);
    icl_destroy_list_queue (_scope, _destroy);
    icl_destroy_unlink (&_destroy);
}
}
//  Embed the version information in the resulting binary

char *EMBED__ipr_process_version_start    = "VeRsIoNsTaRt:ipc";
char *EMBED__ipr_process_component        = "ipr_process ";
char *EMBED__ipr_process_version          = "1.0 ";
char *EMBED__ipr_process_copyright        = "Copyright (c) 1996-2009 iMatix Corporation";
char *EMBED__ipr_process_filename         = "ipr_process.icl ";
char *EMBED__ipr_process_builddate        = "2011/03/01 ";
char *EMBED__ipr_process_version_end      = "VeRsIoNeNd:ipc";

