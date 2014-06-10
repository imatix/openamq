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
    name      = "icl_system"
    comment   = "Class system class, manages all cache classed objects"
    version   = "1.1"
    script    = "icl_gen"
    >
<doc>
    Implements the metacache manager.  Provides two functions for
    application developers: icl_system_purge(), which does garbage
    collection, and icl_system_terminate(), which does final destruction
    of all active and cached objects. Provides one function for iCL
    developers: icl_system_register(), which registers a class for
    purging and destroying.

    Also responsible for system-wide initialisation, comprising
    icl_global_pool and icl_global_mutex.  Note that to avoid circular
    dependencies, these two are declared in icl_apr.
</doc>

<inherit class = "icl_object">
    <option name = "alloc"  value = "direct" />
    <option name = "links"  value = "0" />
    <option name = "rwlock" value = "0" />
</inherit>
<inherit class = "icl_list_item" >
    <option name = "alloc"  value = "direct" />
    <option name = "links"  value = "0" />
    <option name = "rwlock" value = "0" />
</inherit>

<import class = "icl_destroy_list" />

<public name = "header">
//  Types

typedef void (icl_system_fn) (void);    //  Call-back function

//  Global variables

extern int
    g_argc;
extern char
    **g_argv;
</public>

<private name = "header">
//  Global variables

int
    g_argc = 0;
char
    **g_argv;
</private>

<method name = "initialise" inherit = "none">
    <doc>
    This method initialises the basic iCL structures.  It must be called
    before any other iCL object can be used.
    </doc>
    <argument name = "argc" type = "int">The number of program arguments</argument>
    <argument name = "argv" type = "char **">The program arguments</argument>
    //  Make sure we aren't already initialised.
    if (!s_icl_system_active) {
        g_argc = argc;
        g_argv = argv;

        //  Initialise APR
        apr_initialize ();

        //  Create our own allocator so that we can set a threshold for when to
        //  give memory back to the system
        icl_apr_assert (apr_allocator_create (&icl_global_allocator));
        //  TODO:  Hard-coded at 1 MB, make this value configurable
        apr_allocator_max_free_set (icl_global_allocator, 1 * (1024 * 1024));
        icl_apr_assert (apr_pool_create_ex (&icl_global_pool, NULL, NULL, icl_global_allocator));
        apr_pool_tag   (icl_global_pool, "icl_global_pool");
        apr_allocator_owner_set (icl_global_allocator, icl_global_pool);
#if (defined (BASE_THREADSAFE))
        icl_apr_assert (apr_atomic_init (icl_global_pool));
        icl_apr_assert (apr_thread_mutex_create (&icl_global_allocator_mutex, APR_THREAD_MUTEX_DEFAULT, icl_global_pool));
        apr_allocator_mutex_set (icl_global_allocator, icl_global_allocator_mutex);
        icl_apr_assert (apr_thread_mutex_create (&icl_global_mutex, APR_THREAD_MUTEX_DEFAULT, icl_global_pool));
#endif

        //  Initialise memory allocator and cache list classes
        icl_mem_initialise ();
        icl_cache_initialise ();
        s_icl_system_active = TRUE;
    }
</method>

<context>
    icl_system_fn *purge;               //  Purge cache call-back
    icl_system_fn *destroy;             //  Destroy cache call-back
</context>

<private>
static Bool
    s_icl_system_active = FALSE;

//  List of caches that we manage
static $(selfname)_list_t *
    s_metacache = NULL;                 //  List of registered caches
</private>

<method name = "new" />

<method name = "destroy" />

<method name = "register">
    <argument name = "file" type = "char *" precalc = "__FILE__">Source file for call</argument>
    <argument name = "line" type = "size_t" precalc = "__LINE__">Line number for call</argument>
    <doc>
    Registers a class with the metacache manager. The class registers
    two call-back functions, one to purge data and one to destroy data.
    Purging means returning unused memory to the system heap; destroying
    means destroying all known objects and then returning all unused
    memory to the heap (destroy always implies purge).
    </doc>
    <argument name = "purge" type = "icl_system_fn *">
        Address of cache purge function in calling class
    </argument>
    <argument name = "destroy" type = "icl_system_fn *">
        Address of cache destroy function in calling class
    </argument>
    <local>
    $(selftype)
        *self;
    Bool
        exists = FALSE;
    </local>
    //  Initialise metacache if necessary
    if (!s_metacache)
        s_metacache = $(selfname)_list_new ();

    //  Ignore duplicate registration requests
    self = $(selfname)_list_first (s_metacache);
    while (self) {
        if (self->purge   == purge
        &&  self->destroy == destroy) {
            exists = TRUE;
            break;
        }
        self = $(selfname)_list_next (&self);
    }
    if (!exists) {
        self = $(selfname)_new_ (file, line);
        self->purge   = purge;
        self->destroy = destroy;
        $(selfname)_list_queue (s_metacache, self);
    }
</method>

<method name = "purge">
    <doc>
    Free unused memory. You can call this method at any time to perform
    garbage collection, especially if a memory allocation somewhere in the
    application failed.
    </doc>
    <local>
    $(selftype)
        *self;
    </local>
    if (s_metacache) {
        //  Purge each cache that we have registered
        self = $(selfname)_list_last (s_metacache);
        while (self) {
            if (self->purge)
                self->purge ();

            self = $(selfname)_list_prev (&self);
        }
    }
</method>

<method name = "terminate" private = "0">
    <doc>
    Destroy all cached objects and free their memory.  This method can be
    used at the end of an application, or when resetting the entire application
    state. Note that it invalidates all currently active cached objects.
    </doc>
    <local>
    $(selftype)
        *self;
    </local>
    if (s_icl_system_active) {
        if (s_metacache) {
            //  Purge each cache that we have registered
            self = $(selfname)_list_last (s_metacache);
            while (self) {
                if (self->destroy)
                    self->destroy ();
                $(selfname)_destroy (&self);
                self = $(selfname)_list_last (s_metacache);
            }
            $(selfname)_list_destroy (&s_metacache);
            s_metacache = NULL;
        }

#if (defined (BASE_THREADSAFE))
        icl_apr_assert (apr_thread_mutex_destroy (icl_global_mutex));
        icl_global_mutex = NULL;
#endif
        apr_pool_destroy      (icl_global_pool);
        icl_global_pool       = NULL;
        //  Don't worry about destroying the mutex in the allocator, it will
        //  have been destroyed during apr_pool_destroy ().
        icl_global_allocator  = NULL;
#if (defined (BASE_THREADSAFE))
        icl_global_allocator_mutex = NULL;
#endif
        //  Terminate APR
        apr_terminate ();
        s_icl_system_active = FALSE;
    }
</method>

<method name = "panic">
    <doc>
    Appends an error message to the panic file.  The panic file is named by
    the environment variable PANIC, or "panic.log" otherwise.
    </doc>
    <argument name = "caller" type = "char *" >Name of caller</argument>
    <argument name = "format" type = "char *" >Format specifier</argument>
    <argument name = "args"   type = "...">Arguments to print</argument>
    <local>
    char
        *panic_file;
    FILE
        *panic_fh;
    apr_time_exp_t
        date_time;
    icl_shortstr_t
        formatted;
    apr_size_t
        flen;
    </local>
    //
    //  Open the panic file
    if (apr_env_get (&panic_file, "PANIC", icl_global_pool))
        panic_file = "panic.log";
    panic_fh = fopen (panic_file, "a");
    fprintf (stderr,   "-----------------------------------------------------------------------------\\n");
    fprintf (panic_fh, "-----------------------------------------------------------------------------\\n");

    //  Print date and time of panic
    apr_time_exp_lt (&date_time, apr_time_now ());
    apr_strftime (formatted, &flen, ICL_SHORTSTR_MAX, "%Y-%m-%d %T", &date_time);
    fprintf (stderr,   "Panic at %s from %s\\n", formatted, caller);
    fprintf (panic_fh, "Panic at %s from %s\\n", formatted, caller);

    //  Print panic message
    apr_vsnprintf (formatted, ICL_SHORTSTR_MAX, format, args);
    fprintf (stderr,   "%s\\n", formatted);
    fprintf (panic_fh, "%s\\n", formatted);

    //  Flush & close panic file
    fflush (stderr);
    fflush (panic_fh);
    fclose (panic_fh);
</method>

<method name = "name" return = "name">
    <doc>
    This method formats a string that identifies the current platform
    on which the application is running.  The caller must provide a string
    into which the formatted name will be placed.
    </doc>
    <argument name = "name" type = "char *">Name of platform</argument>
#if (defined (__WINDOWS__))
#   if (defined (WIN32))
    OSVERSIONINFO
        version_info;

    version_info.dwOSVersionInfoSize = sizeof (OSVERSIONINFO);
    if (GetVersionEx (&version_info)) {
        if (version_info.dwPlatformId == VER_PLATFORM_WIN32_WINDOWS) {
            if (version_info.dwMinorVersion == 0)
                strcpy (name, "Windows 95");
            else
            if (version_info.dwMinorVersion == 10)
                strcpy (name, "Windows 98");
            else
            if (version_info.dwMinorVersion == 90)
                strcpy (name, "Windows ME");
        }
        else
        if (version_info.dwPlatformId == VER_PLATFORM_WIN32_NT) {
            if (version_info.dwMajorVersion == 5 && version_info.dwMinorVersion == 0)
                icl_shortstr_fmt (name, "Windows 2000 With %s", version_info.szCSDVersion);
            else
            if (version_info.dwMajorVersion == 5 && version_info.dwMinorVersion == 1)
                icl_shortstr_fmt (name, "Windows XP %s", version_info.szCSDVersion);
            else
            if (version_info.dwMajorVersion <= 4)
                icl_shortstr_fmt (name, "Windows NT %d.%d with %s",
                    version_info.dwMajorVersion, version_info.dwMinorVersion,
                    version_info.szCSDVersion);
            else
                icl_shortstr_fmt (name, "Windows %d.%d",
                    version_info.dwMajorVersion, version_info.dwMinorVersion);
        }
        else
            icl_shortstr_fmt (name, "Windows %d.%d with win32s",
                version_info.dwMajorVersion, version_info.dwMinorVersion);

    }
#   else
        strcpy (name, "Windows 3.x");
#   endif
#elif (defined (__UNIX__))
#   if (defined (__UTYPE_AUX))
        strcpy (name, "Apple AUX");
#   elif (defined (__UTYPE_DECALPHA))
        strcpy (name, "Digital UNIX (Alpha)");
#   elif (defined (__UTYPE_IBMAIX))
        strcpy (name, "IBM RS/6000 AIX");
#   elif (defined (__UTYPE_HPUX))
        strcpy (name, "HP/UX");
#   elif (defined (__UTYPE_LINUX))
        strcpy (name, "Linux");
#   elif (defined (__UTYPE_MIPS))
        strcpy (name, "MIPS");
#   elif (defined (__UTYPE_NETBSD))
        strcpy (name, "NetBSD");
#   elif (defined (__UTYPE_NEXT))
        strcpy (name, "NeXT");
#   elif (defined (__UTYPE_SCO))
        strcpy (name, "SCO UNIX");
#   elif (defined (__UTYPE_IRIX))
        strcpy (name, "Silicon Graphics IRIX");
#   elif (defined (__UTYPE_SUNOS))
        strcpy (name, "SunOS");
#   elif (defined (__UTYPE_SUNSOLARIS))
        strcpy (name, "Sun Solaris");
#   elif (defined (__UTYPE_UNIXWARE))
        strcpy (name, "SCO UNIXWare");
#   else
        strcpy (name, "UNIX");
#   endif
#elif (defined (__VMS__))
    strcpy (name, "Digital OpenVMS");
#elif (defined (__OS2__))
    strcpy (name, "IBM OS/2");
#elif (defined (__MSDOS__))
    strcpy (name, "MS-DOS");
#else
    strcpy (name, "Unknown");
#endif
</method>

<method name = "selftest" export = "none">
    icl_shortstr_t
        name;

    icl_system_name (name);
    assert (*name);

    //icl_system_panic ("icl_system_test", "This is a panic test");
</method>

</class>
