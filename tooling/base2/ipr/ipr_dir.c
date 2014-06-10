/*---------------------------------------------------------------------------
    ipr_dir.c - ipr_dir component

    Provides a directory listing object that can be resorted in various
    ways.
    Generated from ipr_dir.icl by icl_gen using GSL/4.
    
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
#include "ipr_dir.h"                    //  Definitions for our class

//  Shorthand for class type

#define self_t              ipr_dir_t

//  Structure of the ipr_dir object

struct _ipr_dir_t {
dbyte
    object_tag;                     //  Object validity marker
ipr_finfo_list_t
    *finfo_list;                    //  List of files in directory
apr_pool_t
    *pool;                          //  Pool for all allocations
    uint index_size;                    //  Number of files in directory
    ipr_finfo_t ** index;               //  Sorted list of finfo items
};

//  Shorthands for class methods

#define self_new            ipr_dir_new
#define self_annihilate     ipr_dir_annihilate
#define self_resort         ipr_dir_resort
#define self_dump           ipr_dir_dump
#define self_create         ipr_dir_create
#define self_remove         ipr_dir_remove
#define self_selftest       ipr_dir_selftest
#define self_index_size     ipr_dir_index_size
#define self_index          ipr_dir_index
#define self_terminate      ipr_dir_terminate
#define self_show           ipr_dir_show
#define self_destroy        ipr_dir_destroy
#define self_alloc          ipr_dir_alloc
#define self_free           ipr_dir_free
#define self_cache_initialise  ipr_dir_cache_initialise
#define self_cache_purge    ipr_dir_cache_purge
#define self_cache_terminate  ipr_dir_cache_terminate
#define self_show_animation  ipr_dir_show_animation
#define self_new_in_scope   ipr_dir_new_in_scope

static void
    ipr_dir_annihilate (
ipr_dir_t * ( * self_p )                //  Reference to object reference
);

#define ipr_dir_alloc()                 ipr_dir_alloc_ (__FILE__, __LINE__)
static ipr_dir_t *
    ipr_dir_alloc_ (
char * file,                            //  Source file for call
size_t line                             //  Line number for call
);

static void
    ipr_dir_free (
ipr_dir_t * self                        //  Object reference
);

static void
    ipr_dir_cache_initialise (
void);

static void
    ipr_dir_cache_purge (
void);

static void
    ipr_dir_cache_terminate (
void);

Bool
    ipr_dir_animating = TRUE;           //  Animation enabled by default
static icl_cache_t
    *s_cache = NULL;


static int
    s_sort_by_name       (const void *info1, const void *info2);
static int
    s_sort_by_time       (const void *info1, const void *info2);
static int
    s_sort_by_time_desc  (const void *info1, const void *info2);
static int
    s_sort_by_size       (const void *info1, const void *info2);
static int
    s_sort_by_size_desc  (const void *info1, const void *info2);
/*  -------------------------------------------------------------------------
    ipr_dir_new

    Type: Component method
    Creates and initialises a new ipr_dir_t object, returns a
    reference to the created object.
    -------------------------------------------------------------------------
 */

ipr_dir_t *
    ipr_dir_new_ (
    char * file,                        //  Source file for call
    size_t line,                        //  Line number for call
    char * path,                        //  Directory path to load
    char sortby                         //  Sort key
)
{
apr_dir_t
    *dir = NULL;
apr_status_t
    rc;
apr_finfo_t
    apr_finfo;
ipr_finfo_t
    *finfo;
    ipr_dir_t *
        self = NULL;                    //  Object reference

    self = ipr_dir_alloc_ (file, line);
    if (self) {
        self->object_tag   = IPR_DIR_ALIVE;
#if defined (DEBUG)
        icl_mem_set_callback (self, ipr_dir_show_);
#endif

//
apr_pool_create (&self->pool, NULL);
self->finfo_list = ipr_finfo_list_new ();

rc = apr_dir_open (&dir, path, self->pool);
if (rc == APR_SUCCESS) {
    rc = apr_dir_read (&apr_finfo, APR_FINFO_MIN + APR_FINFO_OWNER, dir);
    while (rc == APR_SUCCESS || rc == APR_INCOMPLETE) {
        finfo = ipr_finfo_new (&apr_finfo);
        ipr_finfo_list_push (self->finfo_list, finfo);
        ipr_finfo_unlink (&finfo);
        rc = apr_dir_read (&apr_finfo, APR_FINFO_MIN + APR_FINFO_OWNER, dir);
    }
    apr_dir_close (dir);
    self_resort (self, sortby);
}
else
    self_destroy (&self);
}

    return (self);
}
/*  -------------------------------------------------------------------------
    ipr_dir_annihilate

    Type: Component method
    -------------------------------------------------------------------------
 */

static void
    ipr_dir_annihilate (
    ipr_dir_t * ( * self_p )            //  Reference to object reference
)
{

    ipr_dir_t *
        self = *self_p;                 //  Dereferenced Reference to object reference

IPR_DIR_ASSERT_SANE (self);

icl_mem_free (self->index);
ipr_finfo_list_destroy (&self->finfo_list);
apr_pool_destroy (self->pool);
self->pool = NULL;

}
/*  -------------------------------------------------------------------------
    ipr_dir_resort

    Type: Component method
    Accepts a ipr_dir_t reference and returns zero in case of success,
    1 in case of errors.
    -------------------------------------------------------------------------
 */

int
    ipr_dir_resort (
    ipr_dir_t * self,                   //  Reference to object
    char sortby                         //  Sort key
)
{
ipr_finfo_t
    *finfo;
uint
    finfo_nbr;
    int
        rc = 0;                         //  Return code

IPR_DIR_ASSERT_SANE (self);

//
//  Allocate an index to sort the finfo entries
if (self->index)
    icl_mem_free (self->index);
self->index_size = ipr_finfo_list_count (self->finfo_list);
self->index = icl_mem_alloc (self->index_size * sizeof (ipr_finfo_t *));

finfo = ipr_finfo_list_first (self->finfo_list);
for (finfo_nbr = 0; finfo_nbr < self->index_size; finfo_nbr++) {
    self->index [finfo_nbr] = finfo;
    finfo = ipr_finfo_list_next (&finfo);
}
qsort (self->index, self->index_size, sizeof (ipr_finfo_t *),
    sortby == 'n'? s_sort_by_name:
    sortby == 't'? s_sort_by_time:
    sortby == 'T'? s_sort_by_time_desc:
    sortby == 's'? s_sort_by_size:
    sortby == 'S'? s_sort_by_size_desc:
                   s_sort_by_name);


    return (rc);
}
/*  -------------------------------------------------------------------------
    ipr_dir_dump

    Type: Component method
    Accepts a ipr_dir_t reference and returns zero in case of success,
    1 in case of errors.
    -------------------------------------------------------------------------
 */

int
    ipr_dir_dump (
    ipr_dir_t * self                    //  Reference to object
)
{
ipr_finfo_t
    *finfo;
uint
    finfo_nbr;
icl_shortstr_t
    buffer;                         //  Formatted directory entry
    int
        rc = 0;                         //  Return code

IPR_DIR_ASSERT_SANE (self);

//
assert (self->index);
for (finfo_nbr = 0; finfo_nbr < self->index_size; finfo_nbr++) {
    finfo = self->index [finfo_nbr];
    ipr_finfo_print (finfo, buffer);
    icl_console_print (buffer);
}


    return (rc);
}
/*  -------------------------------------------------------------------------
    ipr_dir_create

    Type: Component method
    Create a new directory.  Returns 0 if the directory was created;
    -1 if there was an error.  Under Windows and OpenVMS, accepts directory
    names with '/'.  Will create multiple levels of directory if required.
    -------------------------------------------------------------------------
 */

Bool
    ipr_dir_create (
    char * path_to_create               //  Directory path to create
)
{
    icl_shortstr_t
        path;
#   if (defined (GATES_FILESYSTEM))
    char
        *char_ptr;
#   endif
    char
        *slash;
    Bool
        rc = 0;                         //  Not documented

    //  Get working copy so we can mangle it a little
    icl_shortstr_cpy (path, path_to_create);
#   if (defined (GATES_FILESYSTEM))
    for (char_ptr = path; *char_ptr; char_ptr++)
        if (*char_ptr == '/')
            *char_ptr = '\\';

    //  Handle \systemdrive specially
    if (path [0] == '\\' && path [1] == '\\') {
        slash = strchr (path + 2, '\\');
        if (slash)
            slash = strchr (slash + 1, '\\');
    }
    else
#   endif
    slash = strchr (path + 1, PATHEND);

    //  Create each component of directory as required
    FOREVER {                           //  Create any parent directories
        if (slash)
            *slash = '\0';              //  Cut at slash
        if (!ipr_file_is_directory (path)) {
#   if (defined (__UNIX__) || defined (__VMS_XOPEN) || defined (__OS2__))
            rc = mkdir (path, 0775);    //  User RWE Group RWE World RE

#   elif (defined (WIN32))
            if (CreateDirectory (path, NULL))
                rc = 0;
            else
                rc = -1;
#   elif (defined (GATES_FILESYSTEM))
            rc = mkdir (path);          //  Protection?  What's that?
#   else
            rc = -1;                    //  Not a known system
#   endif
            if (rc)                     //  End if error
                break;
        }
        if (slash == NULL)              //  End if last directory
            break;
        *slash = PATHEND;               //  Restore path name
        slash = strchr (slash + 1, PATHEND);
    }

    return (rc);
}
/*  -------------------------------------------------------------------------
    ipr_dir_remove

    Type: Component method
    Remove a directory.  Returns 0 if the directory could be
    removed; -1 if there was an error.  Under MS-DOS and OpenVMS accepts
    a directory name in UNIX format, i.e. containing '/' delimiters.  The
    directory does not need to be empty to be removed; any child directories
    and contained files will be deleted.
    -------------------------------------------------------------------------
 */

Bool
    ipr_dir_remove (
    char * path                         //  Directory path to remove
)
{
    ipr_dir_t
        *dir;                           //  Get list of contained files
    uint
        finfo_nbr;
    ipr_finfo_t
        *finfo;
    icl_shortstr_t
        path_filename;                  //  Path/filename
#   if (defined (GATES_FILESYSTEM))
    icl_shortstr_t
        copy_path;
    char
        *char_ptr;
#   endif
    Bool
        rc = 0;                         //  Not documented

    //
    //  Delete all child directories and contained files
    dir = ipr_dir_new (path, 'n');
    assert (dir->index);
    for (finfo_nbr = 0; finfo_nbr < dir->index_size; finfo_nbr++) {
        finfo = dir->index [finfo_nbr];
        icl_shortstr_fmt (path_filename, "%s/%s", path, finfo->name);
        if (finfo->filetype == APR_DIR) {
            if (*finfo->name != '.')
                ipr_dir_remove (path_filename);
        }
        else
            ipr_file_delete (path_filename);
    }
    ipr_dir_destroy (&dir);

#   if (defined (__UNIX__) || defined (__VMS_XOPEN) || defined (__OS2__))
    if (ipr_file_is_directory (path))
        rc = rmdir (path);
    else
        rc = -1;

#   elif (defined (GATES_FILESYSTEM))
    //  Check that directory exists
    if (ipr_file_is_directory (path)) {
        icl_shortstr_cpy (copy_path, path);
        for (char_ptr = copy_path; *char_ptr; char_ptr++)
            if (*char_ptr == '/')
                *char_ptr = '\\';
#   if (defined (WIN32))
        if (RemoveDirectory (copy_path))
            rc = 0;
        else
            rc = -1;
#   else
        rc = rmdir (copy_path);
#   endif
    }
    else
        rc = -1;
#   else
    rc = -1;                            //  Unsupported platform
#   endif

    return (rc);
}
/*  -------------------------------------------------------------------------
    ipr_dir_selftest

    Type: Component method
    -------------------------------------------------------------------------
 */

void
    ipr_dir_selftest (
void)
{
ipr_dir_t
    *dir;

//
dir = ipr_dir_new (".", 'n');
ipr_dir_resort (dir, 't');
ipr_dir_resort (dir, 'T');
ipr_dir_resort (dir, 's');
ipr_dir_resort (dir, 'S');
ipr_dir_dump (dir);
ipr_dir_destroy (&dir);

assert (ipr_dir_create ("level1/level2/level3") == 0);
assert (ipr_file_is_directory ("level1/level2/level3"));
assert (ipr_dir_remove ("level1") == 0);
}
/*  -------------------------------------------------------------------------
    ipr_dir_index_size

    Type: Component method
    -------------------------------------------------------------------------
 */

uint
    ipr_dir_index_size (
    ipr_dir_t * self                    //  Not documented
)
{
    uint
        index_size;                     //  Not documented

index_size = self->index_size;

    return (index_size);
}
/*  -------------------------------------------------------------------------
    ipr_dir_index

    Type: Component method
    -------------------------------------------------------------------------
 */

ipr_finfo_t **
    ipr_dir_index (
    ipr_dir_t * self                    //  Not documented
)
{
    ipr_finfo_t **
        index;                          //  Not documented

index = self->index;

    return (index);
}
/*  -------------------------------------------------------------------------
    ipr_dir_terminate

    Type: Component method
    -------------------------------------------------------------------------
 */

void
    ipr_dir_terminate (
void)
{


}
/*  -------------------------------------------------------------------------
    ipr_dir_show

    Type: Component method
    -------------------------------------------------------------------------
 */

void
    ipr_dir_show_ (
    void * item,                        //  The opaque pointer
    int opcode,                         //  The callback opcode
    FILE * trace_file,                  //  File to print to
    char * file,                        //  Source file
    size_t line                         //  Line number
)
{
ipr_dir_t
    *self;
int
    container_links;


self = item;
container_links = 0;
assert (opcode == ICL_CALLBACK_DUMP);

fprintf (trace_file, "    <ipr_dir file = \"%s\" line = \"%lu\"  pointer = \"%p\" />\n", file, (unsigned long) line, self);

}
/*  -------------------------------------------------------------------------
    ipr_dir_destroy

    Type: Component method
    Destroys a ipr_dir_t object. Takes the address of a
    ipr_dir_t reference (a pointer to a pointer) and nullifies the
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
    ipr_dir_destroy_ (
    ipr_dir_t * ( * self_p ),           //  Reference to object reference
    char * file,                        //  Source fileSource file
    size_t line                         //  Line numberLine number
)
{
    ipr_dir_t *
        self = *self_p;                 //  Dereferenced Reference to object reference

if (self) {
    ipr_dir_annihilate (self_p);
    ipr_dir_free ((ipr_dir_t *) self);
    *self_p = NULL;
}
}
/*  -------------------------------------------------------------------------
    ipr_dir_alloc

    Type: Component method
    -------------------------------------------------------------------------
 */

static ipr_dir_t *
    ipr_dir_alloc_ (
    char * file,                        //  Source file for call
    size_t line                         //  Line number for call
)
{

    ipr_dir_t *
        self = NULL;                    //  Object reference

//  Initialise cache if necessary
if (!s_cache)
    ipr_dir_cache_initialise ();

self = (ipr_dir_t *) icl_mem_cache_alloc_ (s_cache, file, line);
if (self)
    memset (self, 0, sizeof (ipr_dir_t));



    return (self);
}
/*  -------------------------------------------------------------------------
    ipr_dir_free

    Type: Component method
    Freess a ipr_dir_t object.
    -------------------------------------------------------------------------
 */

static void
    ipr_dir_free (
    ipr_dir_t * self                    //  Object reference
)
{


if (self) {

        memset (&self->object_tag, 0, sizeof (ipr_dir_t) - ((byte *) &self->object_tag - (byte *) self));
//        memset (self, 0, sizeof (ipr_dir_t));
        self->object_tag = IPR_DIR_DEAD;
        icl_mem_free (self);
    }
    self = NULL;
}
/*  -------------------------------------------------------------------------
    ipr_dir_cache_initialise

    Type: Component method
    Initialise the cache and register purge method with the meta-cache.
    -------------------------------------------------------------------------
 */

static void
    ipr_dir_cache_initialise (
void)
{

s_cache = icl_cache_get (sizeof (ipr_dir_t));
icl_system_register (ipr_dir_cache_purge, ipr_dir_cache_terminate);
}
/*  -------------------------------------------------------------------------
    ipr_dir_cache_purge

    Type: Component method
    -------------------------------------------------------------------------
 */

static void
    ipr_dir_cache_purge (
void)
{

icl_mem_cache_purge (s_cache);

}
/*  -------------------------------------------------------------------------
    ipr_dir_cache_terminate

    Type: Component method
    -------------------------------------------------------------------------
 */

static void
    ipr_dir_cache_terminate (
void)
{

s_cache = NULL;

}
/*  -------------------------------------------------------------------------
    ipr_dir_show_animation

    Type: Component method
    Enables animation of the component. Animation is sent to stdout.
    To enable animation you must generate using the option -animate:1.
    -------------------------------------------------------------------------
 */

void
    ipr_dir_show_animation (
    Bool enabled                        //  Are we enabling or disabling animation?
)
{

ipr_dir_animating = enabled;
}
/*  -------------------------------------------------------------------------
    ipr_dir_new_in_scope

    Type: Component method
    -------------------------------------------------------------------------
 */

void
    ipr_dir_new_in_scope_ (
    ipr_dir_t * * self_p,               //  Not documented
    icl_scope_t * _scope,               //  Not documented
    char * file,                        //  Source file for call
    size_t line,                        //  Line number for call
    char * path,                        //  Directory path to load
    char sortby                         //  Sort key
)
{
    icl_destroy_t *
        _destroy;                       //  Not documented

*self_p = ipr_dir_new_ (file,line,path,sortby);

if (*self_p) {
    _destroy = icl_destroy_new   ((void * *) self_p, (icl_destructor_fn *) ipr_dir_destroy_);
    icl_destroy_list_queue (_scope, _destroy);
    icl_destroy_unlink (&_destroy);
}
}
//  Sort by name
static int
s_sort_by_name (const void *info1, const void *info2)
{
    int
        rc;
    ipr_finfo_t
        *finfo1 = *(ipr_finfo_t **) info1,
        *finfo2 = *(ipr_finfo_t **) info2;

#if defined (__WINDOWS__)
    rc = ipr_str_lexcmp (finfo1->name, finfo2->name);
#else
    rc = strcmp (finfo1->name, finfo2->name);
#endif
    return (rc);
}

//  Sort by date/time then name
static int
s_sort_by_time (const void *info1, const void *info2)
{
    int
        rc;
    ipr_finfo_t
        *finfo1 = *(ipr_finfo_t **) info1,
        *finfo2 = *(ipr_finfo_t **) info2;

    if (finfo1->mtime < finfo2->mtime)
        rc = -1;
    else
    if (finfo1->mtime > finfo2->mtime)
        rc = 1;
    else
#if defined (__WINDOWS__)
        rc = ipr_str_lexcmp (finfo1->name, finfo2->name);
#else
        rc = strcmp (finfo1->name, finfo2->name);
#endif
    return (rc);
}

//  Sort by descending date/time, then name
static int
s_sort_by_time_desc (const void *info1, const void *info2)
{
    int
        rc;
    ipr_finfo_t
        *finfo1 = *(ipr_finfo_t **) info1,
        *finfo2 = *(ipr_finfo_t **) info2;

    if (finfo1->mtime < finfo2->mtime)
        rc = 1;
    else
    if (finfo1->mtime > finfo2->mtime)
        rc = -1;
    else
#if defined (__WINDOWS__)
        rc = ipr_str_lexcmp (finfo1->name, finfo2->name);
#else
        rc = strcmp (finfo1->name, finfo2->name);
#endif
    return (rc);
}

//  Sort by size then name
static int
s_sort_by_size (const void *info1, const void *info2)
{
    int
        rc;
    ipr_finfo_t
        *finfo1 = *(ipr_finfo_t **) info1,
        *finfo2 = *(ipr_finfo_t **) info2;

    if (finfo1->size < finfo2->size)
        rc = -1;
    else
    if (finfo1->size > finfo2->size)
        rc = 1;
    else
#if defined (__WINDOWS__)
        rc = ipr_str_lexcmp (finfo1->name, finfo2->name);
#else
        rc = strcmp (finfo1->name, finfo2->name);
#endif
    return (rc);
}

//  Sort by descending size, then name
static int
s_sort_by_size_desc (const void *info1, const void *info2)
{
    int
        rc;
    ipr_finfo_t
        *finfo1 = *(ipr_finfo_t **) info1,
        *finfo2 = *(ipr_finfo_t **) info2;

    if (finfo1->size < finfo2->size)
        rc = 1;
    else
    if (finfo1->size > finfo2->size)
        rc = -1;
    else
#if defined (__WINDOWS__)
        rc = ipr_str_lexcmp (finfo1->name, finfo2->name);
#else
        rc = strcmp (finfo1->name, finfo2->name);
#endif
    return (rc);
}

//  Embed the version information in the resulting binary

char *EMBED__ipr_dir_version_start        = "VeRsIoNsTaRt:ipc";
char *EMBED__ipr_dir_component            = "ipr_dir ";
char *EMBED__ipr_dir_version              = "1.0 ";
char *EMBED__ipr_dir_copyright            = "Copyright (c) 1996-2009 iMatix Corporation";
char *EMBED__ipr_dir_filename             = "ipr_dir.icl ";
char *EMBED__ipr_dir_builddate            = "2011/03/01 ";
char *EMBED__ipr_dir_version_end          = "VeRsIoNeNd:ipc";

