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
    name      = "ipr_process"
    comment   = "Process management"
    version   = "1.0"
    script    = "icl_gen"
    opaque    = "1"
    >

<inherit class = "icl_object">
    <option name = "alloc" value = "cache" />
</inherit>

<context>
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

    <property name = "finished" type = "Bool" readonly = "1">Process has finished</property>
    <property name = "exitcode" type = "int" readonly = "1">Process exit code (!0 = failed)</property>
</context>

<method name = "new">
    <doc>
    Prepares a new process.  To start the process, use the start
    method.
    </doc>
    <argument name = "command"  type = "char *">Full command-line to run</argument>
    <argument name = "in_file"  type = "char *">Provide standard input, or empty/NULL</argument>
    <argument name = "out_file" type = "char *">Capture standard output, or empty/NULL</argument>
    <argument name = "err_file" type = "char *">Capture standard error, or empty/NULL</argument>
    <local>
    apr_status_t
        rc;
    </local>
    //
    //  All resources for the process are in a dedicated APR pool
    icl_apr_assert (apr_pool_create (&self->pool, icl_global_pool));
    apr_pool_tag (self->pool, "$(selfname)_$(method.name)");

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
</method>

<method name = "destroy">
    <doc>
    Ends and destroys the process. If the process was running, sends
    it a signal and waits for it to end.
    </doc>
    //
    self_kill (self);
    apr_pool_destroy (self->pool);
</method>

<method name = "start" template = "function">
    <doc>
    Starts the process as specified.  Returns 0 if the process was started
    succesfully, else prints an error message and returns -1.
    </doc>
    <argument name = "directory" type = "char *">Working directory, or null</argument>
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
</method>

<method name = "wait" template = "function">
    <doc>
    Wait for process to complete and collect its status. If the "sync" argument
    is false, just checks if the process is running.  Returns true if the
    process is still running, false if the process has ended (successfully or
    failed).  The process exit status is in process->exitcode.
    </doc>
    <argument name = "sync" type = "Bool">Wait for process to end?</argument>
    <local>
    int
        apr_rc;
    apr_exit_why_e
        exitwhy;                        //  Reason for exit
    </local>
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
</method>

<method name = "kill" template = "function">
    <doc>
    Kills the process, if running.
    </doc>
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
</method>

<private>
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
</private>

<method name = "server" return = "rc">
    <doc>
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
    </doc>
    <argument name = "workdir"  type = "char *" >Where server runs, or NULL/""</argument>
    <argument name = "lockfile" type = "char *" >For exclusive execution</argument>
    <argument name = "argc"     type = "int"    >Original command-line arguments</argument>
    <argument name = "argv"     type = "char **">Original command-line arguments</argument>
    <argument name = "sswitch"  type = "char **">Filter these options from argv</argument>
    <declare name = "rc" type = "int" default = "0" />
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
    for (argi = argo = 0; argi &lt; argc; argi++) {
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
                apr_snprintf (pid_buffer, sizeof (pid_buffer), "%6d\\n", (int) getpid ());
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
</method>

<method name = "id" return = "pid">
    <doc>
    Returns the current process id, which is an integer on all supported
    systems.  Returns 0 if the system does not support a getpid call.
    </doc>
    <declare name = "pid" type = "int" />
#if (defined (__WINDOWS__))
    pid = GetCurrentProcessId();
#elif (defined (__UNIX__))
    pid = getpid ();
#else
    pid = 0;
#endif
</method>

<method name = "selftest">
    <local>
    ipr_process_t
        *process;
    apr_pool_t
        *pool;
    </local>
    process = ipr_process_new ("lsnf", NULL, "ls.out", "ls.err");
    ipr_process_start   (process, ".");
    ipr_process_wait    (process, TRUE);
    assert (ipr_process_exitcode (process) != 0);
    ipr_process_destroy (&process);

    icl_apr_assert (apr_pool_create (&pool, icl_global_pool));
    apr_file_remove ("ls.out", pool);
    apr_file_remove ("ls.err", pool);
    apr_pool_destroy (pool);
</method>

</class>
