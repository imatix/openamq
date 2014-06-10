/*---------------------------------------------------------------------------
    icl_system.c - icl_system component

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
 *---------------------------------------------------------------------------*/

#include "icl.h"                        //  iCL base classes
#include "icl_system.h"                 //  Definitions for our class

//  Shorthand for class type

#define self_t              icl_system_t

//  Shorthands for class methods

#define self_initialise     icl_system_initialise
#define self_new            icl_system_new
#define self_annihilate     icl_system_annihilate
#define self_register       icl_system_register
#define self_purge          icl_system_purge
#define self_terminate      icl_system_terminate
#define self_panic          icl_system_panic
#define self_name           icl_system_name
#define self_selftest       icl_system_selftest
#define self_remove_from_all_containers  icl_system_remove_from_all_containers
#define self_destroy        icl_system_destroy
#define self_alloc          icl_system_alloc
#define self_free           icl_system_free
#define self_show_animation  icl_system_show_animation
#define self_new_in_scope   icl_system_new_in_scope

static void
    icl_system_annihilate (
icl_system_t * ( * self_p )             //  Reference to object reference
);

#define icl_system_alloc()              icl_system_alloc_ (__FILE__, __LINE__)
static icl_system_t *
    icl_system_alloc_ (
char * file,                            //  Source file for call
size_t line                             //  Line number for call
);

static void
    icl_system_free (
icl_system_t * self                     //  Object reference
);

Bool
    icl_system_animating = TRUE;        //  Animation enabled by default

//  Global variables

int
    g_argc = 0;
char
    **g_argv;
static Bool
    s_icl_system_active = FALSE;

//  List of caches that we manage
static icl_system_list_t *
    s_metacache = NULL;                 //  List of registered caches
/*  -------------------------------------------------------------------------
    icl_system_initialise

    Type: Component method
    This method initialises the basic iCL structures.  It must be called
    before any other iCL object can be used.
    -------------------------------------------------------------------------
 */

void
    icl_system_initialise (
    int argc,                           //  The number of program arguments
    char ** argv                        //  The program arguments
)
{

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
}
/*  -------------------------------------------------------------------------
    icl_system_new

    Type: Component method
    Creates and initialises a new icl_system_t object, returns a
    reference to the created object.
    Initialises the new item to be a valid list item.
    -------------------------------------------------------------------------
 */

icl_system_t *
    icl_system_new_ (
    char * file,                        //  Source file for call
    size_t line                         //  Line number for call
)
{
    icl_system_t *
        self = NULL;                    //  Object reference

self = icl_system_alloc_ (file, line);
if (self) {
    self->object_tag   = ICL_SYSTEM_ALIVE;

self->list_next = self;
self->list_prev = self;
self->list_head = NULL;
}

    return (self);
}
/*  -------------------------------------------------------------------------
    icl_system_annihilate

    Type: Component method
    -------------------------------------------------------------------------
 */

static void
    icl_system_annihilate (
    icl_system_t * ( * self_p )         //  Reference to object reference
)
{

    icl_system_t *
        self = *self_p;                 //  Dereferenced Reference to object reference

ICL_SYSTEM_ASSERT_SANE (self);
icl_system_remove_from_all_containers (self);


}
/*  -------------------------------------------------------------------------
    icl_system_register

    Type: Component method
    Registers a class with the metacache manager. The class registers
    two call-back functions, one to purge data and one to destroy data.
    Purging means returning unused memory to the system heap; destroying
    means destroying all known objects and then returning all unused
    memory to the heap (destroy always implies purge).
    -------------------------------------------------------------------------
 */

void
    icl_system_register_ (
    char * file,                        //  Source file for call
    size_t line,                        //  Line number for call
    icl_system_fn * purge,              //  Address of cache purge function in calling class
    icl_system_fn * destroy             //  Address of cache destroy function in calling class
)
{
icl_system_t
    *self;
Bool
    exists = FALSE;

//  Initialise metacache if necessary
if (!s_metacache)
    s_metacache = icl_system_list_new ();

//  Ignore duplicate registration requests
self = icl_system_list_first (s_metacache);
while (self) {
    if (self->purge   == purge
    &&  self->destroy == destroy) {
        exists = TRUE;
        break;
    }
    self = icl_system_list_next (&self);
}
if (!exists) {
    self = icl_system_new_ (file, line);
    self->purge   = purge;
    self->destroy = destroy;
    icl_system_list_queue (s_metacache, self);
}
}
/*  -------------------------------------------------------------------------
    icl_system_purge

    Type: Component method
    Free unused memory. You can call this method at any time to perform
    garbage collection, especially if a memory allocation somewhere in the
    application failed.
    -------------------------------------------------------------------------
 */

void
    icl_system_purge (
void)
{
icl_system_t
    *self;

if (s_metacache) {
    //  Purge each cache that we have registered
    self = icl_system_list_last (s_metacache);
    while (self) {
        if (self->purge)
            self->purge ();

        self = icl_system_list_prev (&self);
    }
}
}
/*  -------------------------------------------------------------------------
    icl_system_terminate

    Type: Component method
    Destroy all cached objects and free their memory.  This method can be
    used at the end of an application, or when resetting the entire application
    state. Note that it invalidates all currently active cached objects.
    -------------------------------------------------------------------------
 */

void
    icl_system_terminate (
void)
{
icl_system_t
    *self;


    if (s_icl_system_active) {
        if (s_metacache) {
            //  Purge each cache that we have registered
            self = icl_system_list_last (s_metacache);
            while (self) {
                if (self->destroy)
                    self->destroy ();
                icl_system_destroy (&self);
                self = icl_system_list_last (s_metacache);
            }
            icl_system_list_destroy (&s_metacache);
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
}
/*  -------------------------------------------------------------------------
    icl_system_panic

    Type: Component method
    Appends an error message to the panic file.  The panic file is named by
    the environment variable PANIC, or "panic.log" otherwise.
    -------------------------------------------------------------------------
 */

void
    icl_system_panic (
    char * caller,                      //  Name of caller
    char * format,                      //  Format specifier
...                                     //  Arguments to print
)
{
    va_list
        args;
    va_start (args, format);
    icl_system_panic_v (caller, format, args );
    va_end (args);
}
/*  -------------------------------------------------------------------------
    icl_system_panic_v

    Type: Component method
    Appends an error message to the panic file.  The panic file is named by
    the environment variable PANIC, or "panic.log" otherwise.
    -------------------------------------------------------------------------
 */

void
    icl_system_panic_v (
    char * caller,                      //  Name of caller
    char * format,                      //  Format specifier
va_list args                            //  Arguments to print
)
{
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

//
//  Open the panic file
if (apr_env_get (&panic_file, "PANIC", icl_global_pool))
    panic_file = "panic.log";
panic_fh = fopen (panic_file, "a");
fprintf (stderr,   "-----------------------------------------------------------------------------\n");
fprintf (panic_fh, "-----------------------------------------------------------------------------\n");

//  Print date and time of panic
apr_time_exp_lt (&date_time, apr_time_now ());
apr_strftime (formatted, &flen, ICL_SHORTSTR_MAX, "%Y-%m-%d %T", &date_time);
fprintf (stderr,   "Panic at %s from %s\n", formatted, caller);
fprintf (panic_fh, "Panic at %s from %s\n", formatted, caller);

//  Print panic message
apr_vsnprintf (formatted, ICL_SHORTSTR_MAX, format, args);
fprintf (stderr,   "%s\n", formatted);
fprintf (panic_fh, "%s\n", formatted);

//  Flush & close panic file
fflush (stderr);
fflush (panic_fh);
fclose (panic_fh);
}
/*  -------------------------------------------------------------------------
    icl_system_name

    Type: Component method
    This method formats a string that identifies the current platform
    on which the application is running.  The caller must provide a string
    into which the formatted name will be placed.
    -------------------------------------------------------------------------
 */

char *
    icl_system_name (
    char * name                         //  Name of platform
)
{

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

    return (name);
}
/*  -------------------------------------------------------------------------
    icl_system_selftest

    Type: Component method
    -------------------------------------------------------------------------
 */

void
    icl_system_selftest (
void)
{

icl_shortstr_t
    name;

icl_system_name (name);
assert (*name);

//icl_system_panic ("icl_system_test", "This is a panic test");
}
/*  -------------------------------------------------------------------------
    icl_system_remove_from_all_containers

    Type: Component method
    The method to call to remove an item from its container.  Is called by
    the 'destroy' method if the possession count hits zero.
    -------------------------------------------------------------------------
 */

void
    icl_system_remove_from_all_containers (
    icl_system_t * self                 //  The item
)
{

icl_system_list_remove (self);
}
/*  -------------------------------------------------------------------------
    icl_system_destroy

    Type: Component method
    Destroys a icl_system_t object. Takes the address of a
    icl_system_t reference (a pointer to a pointer) and nullifies the
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
    icl_system_destroy_ (
    icl_system_t * ( * self_p ),        //  Reference to object reference
    char * file,                        //  Source fileSource file
    size_t line                         //  Line numberLine number
)
{
    icl_system_t *
        self = *self_p;                 //  Dereferenced Reference to object reference

if (self) {
    icl_system_annihilate (self_p);
    icl_system_free ((icl_system_t *) self);
    *self_p = NULL;
}
}
/*  -------------------------------------------------------------------------
    icl_system_alloc

    Type: Component method
    -------------------------------------------------------------------------
 */

static icl_system_t *
    icl_system_alloc_ (
    char * file,                        //  Source file for call
    size_t line                         //  Line number for call
)
{

    icl_system_t *
        self = NULL;                    //  Object reference

self = (icl_system_t *) malloc (sizeof (icl_system_t));
if (self)
    memset (self, 0, sizeof (icl_system_t));



    return (self);
}
/*  -------------------------------------------------------------------------
    icl_system_free

    Type: Component method
    Freess a icl_system_t object.
    -------------------------------------------------------------------------
 */

static void
    icl_system_free (
    icl_system_t * self                 //  Object reference
)
{


if (self) {

        memset (&self->object_tag, 0, sizeof (icl_system_t) - ((byte *) &self->object_tag - (byte *) self));
//        memset (self, 0, sizeof (icl_system_t));
        self->object_tag = ICL_SYSTEM_DEAD;
        free (self);
    }
    self = NULL;
}
/*  -------------------------------------------------------------------------
    icl_system_show_animation

    Type: Component method
    Enables animation of the component. Animation is sent to stdout.
    To enable animation you must generate using the option -animate:1.
    -------------------------------------------------------------------------
 */

void
    icl_system_show_animation (
    Bool enabled                        //  Are we enabling or disabling animation?
)
{

icl_system_animating = enabled;
}
/*  -------------------------------------------------------------------------
    icl_system_new_in_scope

    Type: Component method
    -------------------------------------------------------------------------
 */

void
    icl_system_new_in_scope_ (
    icl_system_t * * self_p,            //  Not documented
    icl_scope_t * _scope,               //  Not documented
    char * file,                        //  Source file for call
    size_t line                         //  Line number for call
)
{
    icl_destroy_t *
        _destroy;                       //  Not documented

*self_p = icl_system_new_ (file,line);

if (*self_p) {
    _destroy = icl_destroy_new   ((void * *) self_p, (icl_destructor_fn *) icl_system_destroy_);
    icl_destroy_list_queue (_scope, _destroy);
    icl_destroy_unlink (&_destroy);
}
}
//  Embed the version information in the resulting binary

char *EMBED__icl_system_version_start     = "VeRsIoNsTaRt:ipc";
char *EMBED__icl_system_component         = "icl_system ";
char *EMBED__icl_system_version           = "1.1 ";
char *EMBED__icl_system_copyright         = "Copyright (c) 1996-2009 iMatix Corporation";
char *EMBED__icl_system_filename          = "icl_system.icl ";
char *EMBED__icl_system_builddate         = "2011/03/01 ";
char *EMBED__icl_system_version_end       = "VeRsIoNeNd:ipc";

