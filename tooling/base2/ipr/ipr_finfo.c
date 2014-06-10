/*---------------------------------------------------------------------------
    ipr_finfo.c - ipr_finfo component

    This class wraps the APR finfo object so that we can use it in iCL
    lists, etc.  Generates a list container - ipr_finfo_list.
    Generated from ipr_finfo.icl by icl_gen using GSL/4.
    
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
#include "ipr_finfo.h"                  //  Definitions for our class

//  Shorthand for class type

#define self_t              ipr_finfo_t

//  Shorthands for class methods

#define self_new            ipr_finfo_new
#define self_matches        ipr_finfo_matches
#define self_print          ipr_finfo_print
#define self_selftest       ipr_finfo_selftest
#define self_remove_from_all_containers  ipr_finfo_remove_from_all_containers
#define self_show           ipr_finfo_show
#define self_terminate      ipr_finfo_terminate
#define self_destroy        ipr_finfo_destroy
#define self_annihilate     ipr_finfo_annihilate
#define self_alloc          ipr_finfo_alloc
#define self_free           ipr_finfo_free
#define self_link           ipr_finfo_link
#define self_unlink         ipr_finfo_unlink
#define self_cache_initialise  ipr_finfo_cache_initialise
#define self_cache_purge    ipr_finfo_cache_purge
#define self_cache_terminate  ipr_finfo_cache_terminate
#define self_show_animation  ipr_finfo_show_animation
#define self_new_in_scope   ipr_finfo_new_in_scope

#define ipr_finfo_annihilate(self)      ipr_finfo_annihilate_ (self, __FILE__, __LINE__)
static void
    ipr_finfo_annihilate_ (
ipr_finfo_t * ( * self_p ),             //  Reference to object reference
char * file,                            //  Source file
size_t line                             //  Line number
);

#define ipr_finfo_alloc()               ipr_finfo_alloc_ (__FILE__, __LINE__)
static ipr_finfo_t *
    ipr_finfo_alloc_ (
char * file,                            //  Source file for call
size_t line                             //  Line number for call
);

#define ipr_finfo_free(self)            ipr_finfo_free_ (self, __FILE__, __LINE__)
static void
    ipr_finfo_free_ (
ipr_finfo_t * self,                     //  Object reference
char * file,                            //  Source file
size_t line                             //  Line number
);

static void
    ipr_finfo_cache_initialise (
void);

static void
    ipr_finfo_cache_purge (
void);

static void
    ipr_finfo_cache_terminate (
void);

Bool
    ipr_finfo_animating = TRUE;         //  Animation enabled by default
static icl_cache_t
    *s_cache = NULL;


/*  -------------------------------------------------------------------------
    ipr_finfo_new

    Type: Component method
    Creates and initialises a new ipr_finfo_t object, returns a
    reference to the created object.
    Initialises the new item to be a valid list item.
    -------------------------------------------------------------------------
 */

ipr_finfo_t *
    ipr_finfo_new_ (
    char * file,                        //  Source file for call
    size_t line,                        //  Line number for call
    apr_finfo_t * finfo                 //  APR finfo struct
)
{
    ipr_finfo_t *
        self = NULL;                    //  Object reference

    self = ipr_finfo_alloc_ (file, line);
    if (self) {
        self->object_tag   = IPR_FINFO_ALIVE;
        self->links        = 1;
        self->zombie       = FALSE;
#if defined (DEBUG) || defined (BASE_HISTORY) || defined (BASE_HISTORY_IPR_FINFO)
        self->history_last = 0;

        //  Track possession operation in history
        self->history_file  [0] = file;
        self->history_line  [0] = line;
        self->history_type  [0] = "new";
        self->history_links [0] = self->links;
#endif
#if defined (DEBUG)
        icl_mem_set_callback (self, ipr_finfo_show_);
#endif

self->list_next = self;
self->list_prev = self;
self->list_head = NULL;
icl_shortstr_cpy (self->name, (char *) finfo->name);
self->protection = finfo->protection;
self->filetype   = finfo->filetype;
self->inode      = finfo->inode;
self->device     = finfo->device;
self->nlink      = finfo->nlink;
self->user       = finfo->user;
self->group      = finfo->group;
self->size       = finfo->size;
self->atime      = finfo->atime;
self->mtime      = finfo->mtime;
self->ctime      = finfo->ctime;
}

    return (self);
}
/*  -------------------------------------------------------------------------
    ipr_finfo_matches

    Type: Component method
    Accepts a ipr_finfo_t reference and returns zero in case of success,
    1 in case of errors.
    Returns TRUE if the file name matches ths specified pattern which is
    a wildcard that can contain ? (one character) and * (zero or more
    characters).  On Windows, does a case-insensitive comparison.
    -------------------------------------------------------------------------
 */

int
    ipr_finfo_matches (
    ipr_finfo_t * self,                 //  Reference to object
    char * pattern                      //  Wildcard pattern
)
{
char
    *pattern_ptr,                   //  Points to pattern
    *filename_ptr;                  //  Points to filename
    int
        rc = 0;                         //  Return code

IPR_FINFO_ASSERT_SANE (self);
if (!self->zombie) {

    //
    assert (pattern);

    filename_ptr = (char *) self->name;
    pattern_ptr = pattern;
    FOREVER {
        //  If at end of pattern and filename, we have match
        if (*pattern_ptr == 0 && *filename_ptr == 0) {
            rc = TRUE;                  //  Successful match
            break;
        }
        //  If end of either but not both, match failed
        if (*pattern_ptr == 0 || *filename_ptr == 0) {
            rc = FALSE;                 //  No match
            break;
        }
        //  If pattern character is '?', we match one char
        if (*pattern_ptr == '?'
#if (defined (__WINDOWS__))
        ||  toupper (*pattern_ptr) == toupper (*filename_ptr)) {
#else
        ||  *pattern_ptr == *filename_ptr) {
#endif
            pattern_ptr++;
            filename_ptr++;
        }
        else
        //  If pattern character is '*', we match as much as possible
        if (*pattern_ptr == '*') {
            pattern_ptr++;              //  Try to match following char
            while (*filename_ptr && *filename_ptr != *pattern_ptr)
                filename_ptr++;
        }
        else {
            rc = FALSE;                 //  No match
            break;
        }
    }
}
else
    rc = -1;                        //  Return error on zombie object.


    return (rc);
}
/*  -------------------------------------------------------------------------
    ipr_finfo_print

    Type: Component method
    Accepts a ipr_finfo_t reference and returns zero in case of success,
    1 in case of errors.
    Formats the file information into a printable line of text. The result
    is formatted into a icl_shortstr_t provided by the caller.
    -------------------------------------------------------------------------
 */

int
    ipr_finfo_print (
    ipr_finfo_t * self,                 //  Reference to object
    char * buffer                       //  Buffer to prepare
)
{
icl_shortstr_t
    formatted_time;
    int
        rc = 0;                         //  Return code

IPR_FINFO_ASSERT_SANE (self);
if (!self->zombie) {

//
ipr_time_iso8601 (
    self->mtime, ipr_date_format_minute, 0, ipr_time_zone (), formatted_time);

icl_shortstr_fmt (buffer, "%c%c%c%c%c%c%c%c%c%c %8ld %s %s%c",
    self->filetype == APR_DIR? 'd': '-',
    self->protection & APR_FPROT_UREAD?    'r': '-',
    self->protection & APR_FPROT_UWRITE?   'w': '-',
    self->protection & APR_FPROT_UEXECUTE? 'x': '-',
    self->protection & APR_FPROT_GREAD?    'r': '-',
    self->protection & APR_FPROT_GWRITE?   'w': '-',
    self->protection & APR_FPROT_GEXECUTE? 'x': '-',
    self->protection & APR_FPROT_WREAD?    'r': '-',
    self->protection & APR_FPROT_WWRITE?   'w': '-',
    self->protection & APR_FPROT_WEXECUTE? 'x': '-',
    (long) self->size,
    formatted_time,
    self->name,
    self->filetype == APR_DIR? '/':
        self->protection & APR_FPROT_UEXECUTE? '*':
        ' ');
}
else
    rc = -1;                        //  Return error on zombie object.


    return (rc);
}
/*  -------------------------------------------------------------------------
    ipr_finfo_selftest

    Type: Component method
    -------------------------------------------------------------------------
 */

void
    ipr_finfo_selftest (
void)
{

ipr_finfo_list_t
    *finfo_list;                    //  List of files in directory
apr_pool_t
    *pool;                          //  Pool for all allocations
apr_dir_t
    *dir = NULL;
apr_status_t
    rc;
apr_finfo_t
    apr_finfo;
ipr_finfo_t
    *finfo;
icl_shortstr_t
    finfo_print;
//
//  Load current directory into finfo list
apr_pool_create (&pool, NULL);
assert (apr_dir_open (&dir, ".", pool) == APR_SUCCESS);

finfo_list = ipr_finfo_list_new ();
rc = apr_dir_read (&apr_finfo, APR_FINFO_MIN + APR_FINFO_OWNER, dir);
while (rc == APR_SUCCESS || rc == APR_INCOMPLETE) {
    finfo = ipr_finfo_new (&apr_finfo);
    if (ipr_finfo_matches (finfo, "*.icl")) {
        ipr_finfo_list_push (finfo_list, finfo);
        ipr_finfo_print (finfo, finfo_print);
    }
    ipr_finfo_unlink (&finfo);
    rc = apr_dir_read (&apr_finfo, APR_FINFO_MIN + APR_FINFO_OWNER, dir);
}
apr_dir_close (dir);

ipr_finfo_list_destroy (&finfo_list);
apr_pool_destroy (pool);
}
/*  -------------------------------------------------------------------------
    ipr_finfo_remove_from_all_containers

    Type: Component method
    The method to call to remove an item from its container.  Is called by
    the 'destroy' method if the possession count hits zero.
    -------------------------------------------------------------------------
 */

void
    ipr_finfo_remove_from_all_containers (
    ipr_finfo_t * self                  //  The item
)
{

ipr_finfo_list_remove (self);
}
/*  -------------------------------------------------------------------------
    ipr_finfo_show

    Type: Component method
    -------------------------------------------------------------------------
 */

void
    ipr_finfo_show_ (
    void * item,                        //  The opaque pointer
    int opcode,                         //  The callback opcode
    FILE * trace_file,                  //  File to print to
    char * file,                        //  Source file
    size_t line                         //  Line number
)
{
    ipr_finfo_t
        *self;
    int
        container_links;
#if defined (DEBUG) || defined (BASE_HISTORY) || defined (BASE_HISTORY_IPR_FINFO)
    qbyte
        history_index;
#endif


self = item;
container_links = 0;
if (self->list_head)
   container_links++;
    assert (opcode == ICL_CALLBACK_DUMP);

    fprintf (trace_file, "    <ipr_finfo zombie = \"%u\" links = \"%u\" containers = \"%u\" file = \"%s\" line = \"%lu\"  pointer = \"%p\" />\n", self->zombie, self->links, container_links, file, (unsigned long) line, self);
#if defined (DEBUG) || defined (BASE_HISTORY) || defined (BASE_HISTORY_IPR_FINFO)
    if (self->history_last > IPR_FINFO_HISTORY_LENGTH) {
        fprintf (trace_file, "        <!-- possess history too large (%d) - call iMatix-tech -->\n",
            self->history_last);
        history_index = (self->history_last + 1) % IPR_FINFO_HISTORY_LENGTH;
        self->history_last %= IPR_FINFO_HISTORY_LENGTH;
    }
    else
        history_index = 0;

    for (; history_index != self->history_last; history_index = (history_index + 1) % IPR_FINFO_HISTORY_LENGTH) {
        fprintf (trace_file, "       <%s file = \"%s\" line = \"%lu\" links = \"%lu\" />\n",
            self->history_type [history_index],
            self->history_file [history_index],
            (unsigned long) self->history_line  [history_index],
            (unsigned long) self->history_links [history_index]);
    }
    fprintf (trace_file, "    </ipr_finfo>\n");
#endif

}
/*  -------------------------------------------------------------------------
    ipr_finfo_terminate

    Type: Component method
    -------------------------------------------------------------------------
 */

void
    ipr_finfo_terminate (
void)
{


}
/*  -------------------------------------------------------------------------
    ipr_finfo_destroy

    Type: Component method
    Destroys a ipr_finfo_t object. Takes the address of a
    ipr_finfo_t reference (a pointer to a pointer) and nullifies the
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
    ipr_finfo_destroy_ (
    ipr_finfo_t * ( * self_p ),         //  Reference to object reference
    char * file,                        //  Source fileSource file
    size_t line                         //  Line numberLine number
)
{
    ipr_finfo_t *
        self = *self_p;                 //  Dereferenced Reference to object reference

if (self) {
    if (icl_atomic_cas32 (&self->zombie, TRUE, FALSE) == FALSE)
        ipr_finfo_annihilate_ (self_p, file, line);

    if (self->links == 0) {
        icl_system_panic ("ipr_finfo", "E: missing link on ipr_finfo object");
        ipr_finfo_show_ (self, ICL_CALLBACK_DUMP, stderr, file, line);
        abort ();
    }

    if (icl_atomic_dec32 ((volatile qbyte *) &self->links) == 0)
        ipr_finfo_free_ ((ipr_finfo_t *) self, file, line);
    *self_p = NULL;
}
}
/*  -------------------------------------------------------------------------
    ipr_finfo_annihilate

    Type: Component method
    -------------------------------------------------------------------------
 */

static void
    ipr_finfo_annihilate_ (
    ipr_finfo_t * ( * self_p ),         //  Reference to object reference
    char * file,                        //  Source file
    size_t line                         //  Line number
)
{
#if defined (DEBUG) || defined (BASE_HISTORY) || defined (BASE_HISTORY_IPR_FINFO)
    int
        history_last;
#endif

    ipr_finfo_t *
        self = *self_p;                 //  Dereferenced Reference to object reference

#if defined (DEBUG) || defined (BASE_HISTORY) || defined (BASE_HISTORY_IPR_FINFO)
    //  Track possession operation in history
    history_last = icl_atomic_inc32 ((volatile qbyte *) &self->history_last) + 1;
    self->history_file  [history_last % IPR_FINFO_HISTORY_LENGTH] = file;
    self->history_line  [history_last % IPR_FINFO_HISTORY_LENGTH] = line;
    self->history_type  [history_last % IPR_FINFO_HISTORY_LENGTH] = "destroy";
    self->history_links [history_last % IPR_FINFO_HISTORY_LENGTH] = self->links;
#endif

    IPR_FINFO_ASSERT_SANE (self);
    ipr_finfo_remove_from_all_containers (self);


}
/*  -------------------------------------------------------------------------
    ipr_finfo_alloc

    Type: Component method
    -------------------------------------------------------------------------
 */

static ipr_finfo_t *
    ipr_finfo_alloc_ (
    char * file,                        //  Source file for call
    size_t line                         //  Line number for call
)
{

    ipr_finfo_t *
        self = NULL;                    //  Object reference

//  Initialise cache if necessary
if (!s_cache)
    ipr_finfo_cache_initialise ();

self = (ipr_finfo_t *) icl_mem_cache_alloc_ (s_cache, file, line);
if (self)
    memset (self, 0, sizeof (ipr_finfo_t));



    return (self);
}
/*  -------------------------------------------------------------------------
    ipr_finfo_free

    Type: Component method
    Freess a ipr_finfo_t object.
    -------------------------------------------------------------------------
 */

static void
    ipr_finfo_free_ (
    ipr_finfo_t * self,                 //  Object reference
    char * file,                        //  Source file
    size_t line                         //  Line number
)
{
#if defined (DEBUG) || defined (BASE_HISTORY) || defined (BASE_HISTORY_IPR_FINFO)
    int
        history_last;
#endif


    if (self) {
#if defined (DEBUG) || defined (BASE_HISTORY) || defined (BASE_HISTORY_IPR_FINFO)
        //  Track possession operation in history
        history_last = icl_atomic_inc32 ((volatile qbyte *) &self->history_last) + 1;
        self->history_file  [history_last % IPR_FINFO_HISTORY_LENGTH] = file;
        self->history_line  [history_last % IPR_FINFO_HISTORY_LENGTH] = line;
        self->history_type  [history_last % IPR_FINFO_HISTORY_LENGTH] = "free";
        self->history_links [history_last % IPR_FINFO_HISTORY_LENGTH] = self->links;
#endif

        memset (&self->object_tag, 0, sizeof (ipr_finfo_t) - ((byte *) &self->object_tag - (byte *) self));
//        memset (self, 0, sizeof (ipr_finfo_t));
        self->object_tag = IPR_FINFO_DEAD;
        icl_mem_free (self);
    }
    self = NULL;
}
/*  -------------------------------------------------------------------------
    ipr_finfo_link

    Type: Component method
    Adds a link (reference count) to an object.

    If the object has been zombified (ie destroyed while extra links were present),
    this method returns NULL and does not add any links.

    This method does not lock the object.
    -------------------------------------------------------------------------
 */

ipr_finfo_t *
    ipr_finfo_link_ (
    ipr_finfo_t * self,                 //  Not documented
    char * file,                        //  Source file for call
    size_t line                         //  Line number for call
)
{
#if defined (DEBUG) || defined (BASE_HISTORY) || defined (BASE_HISTORY_IPR_FINFO)
    int
        history_last;
#endif

    if (self) {
        IPR_FINFO_ASSERT_SANE (self);
        if (file)
            icl_mem_possess_ (self, file, line);
        icl_atomic_inc32 ((volatile qbyte *) &self->links);
#if defined (DEBUG) || defined (BASE_HISTORY) || defined (BASE_HISTORY_IPR_FINFO)
        //  Track possession operation in history
        history_last = icl_atomic_inc32 ((volatile qbyte *) &self->history_last) + 1;
        self->history_file  [history_last % IPR_FINFO_HISTORY_LENGTH] = file;
        self->history_line  [history_last % IPR_FINFO_HISTORY_LENGTH] = line;
        self->history_type  [history_last % IPR_FINFO_HISTORY_LENGTH] = "link";
        self->history_links [history_last % IPR_FINFO_HISTORY_LENGTH] = self->links;
#endif
    }

    return (self);
}
/*  -------------------------------------------------------------------------
    ipr_finfo_unlink

    Type: Component method
    Removes a link (reference count) to an object.  Sets the object pointer to NULL
    to indicate that it is no longer valid.
    -------------------------------------------------------------------------
 */

void
    ipr_finfo_unlink_ (
    ipr_finfo_t * ( * self_p ),         //  Reference to object reference
    char * file,                        //  Source file for call
    size_t line                         //  Line number for call
)
{
#if defined (DEBUG) || defined (BASE_HISTORY) || defined (BASE_HISTORY_IPR_FINFO)
    int
        history_last;
#endif

    ipr_finfo_t *
        self = *self_p;                 //  Dereferenced Reference to object reference

    if (self) {
        IPR_FINFO_ASSERT_SANE (self);
        if (self->links == 0) {
            icl_system_panic ("ipr_finfo", "E: missing link on ipr_finfo object");
            ipr_finfo_show_ (self, ICL_CALLBACK_DUMP, stderr, file, line);
            abort ();
        }

#if defined (DEBUG) || defined (BASE_HISTORY) || defined (BASE_HISTORY_IPR_FINFO)
        //  Track possession operation in history.  Pre-empt value of links
        //  after operation; otherwise race condition can result in writing
        //  to freed memory.
        history_last = icl_atomic_inc32 ((volatile qbyte *) &self->history_last) + 1;
        self->history_file  [history_last % IPR_FINFO_HISTORY_LENGTH] = file;
        self->history_line  [history_last % IPR_FINFO_HISTORY_LENGTH] = line;
        self->history_type  [history_last % IPR_FINFO_HISTORY_LENGTH] = "unlink";
        self->history_links [history_last % IPR_FINFO_HISTORY_LENGTH] = self->links - 1;
#endif
        if (icl_atomic_dec32 ((volatile qbyte *) &self->links) == 0) {

        if (icl_atomic_cas32 (&self->zombie, TRUE, FALSE) == FALSE)
            ipr_finfo_annihilate_ (self_p, file, line);
        ipr_finfo_free_ ((ipr_finfo_t *) self, file, line);
    }
    *self_p = NULL;
}
}
/*  -------------------------------------------------------------------------
    ipr_finfo_cache_initialise

    Type: Component method
    Initialise the cache and register purge method with the meta-cache.
    -------------------------------------------------------------------------
 */

static void
    ipr_finfo_cache_initialise (
void)
{

s_cache = icl_cache_get (sizeof (ipr_finfo_t));
icl_system_register (ipr_finfo_cache_purge, ipr_finfo_cache_terminate);
}
/*  -------------------------------------------------------------------------
    ipr_finfo_cache_purge

    Type: Component method
    -------------------------------------------------------------------------
 */

static void
    ipr_finfo_cache_purge (
void)
{

icl_mem_cache_purge (s_cache);

}
/*  -------------------------------------------------------------------------
    ipr_finfo_cache_terminate

    Type: Component method
    -------------------------------------------------------------------------
 */

static void
    ipr_finfo_cache_terminate (
void)
{

s_cache = NULL;

}
/*  -------------------------------------------------------------------------
    ipr_finfo_show_animation

    Type: Component method
    Enables animation of the component. Animation is sent to stdout.
    To enable animation you must generate using the option -animate:1.
    -------------------------------------------------------------------------
 */

void
    ipr_finfo_show_animation (
    Bool enabled                        //  Are we enabling or disabling animation?
)
{

ipr_finfo_animating = enabled;
}
/*  -------------------------------------------------------------------------
    ipr_finfo_new_in_scope

    Type: Component method
    -------------------------------------------------------------------------
 */

void
    ipr_finfo_new_in_scope_ (
    ipr_finfo_t * * self_p,             //  Not documented
    icl_scope_t * _scope,               //  Not documented
    char * file,                        //  Source file for call
    size_t line,                        //  Line number for call
    apr_finfo_t * finfo                 //  APR finfo struct
)
{
    icl_destroy_t *
        _destroy;                       //  Not documented

*self_p = ipr_finfo_new_ (file,line,finfo);

if (*self_p) {
    _destroy = icl_destroy_new   ((void * *) self_p, (icl_destructor_fn *) ipr_finfo_destroy_);
    icl_destroy_list_queue (_scope, _destroy);
    icl_destroy_unlink (&_destroy);
}
}
//  Embed the version information in the resulting binary

char *EMBED__ipr_finfo_version_start      = "VeRsIoNsTaRt:ipc";
char *EMBED__ipr_finfo_component          = "ipr_finfo ";
char *EMBED__ipr_finfo_version            = "1.0 ";
char *EMBED__ipr_finfo_copyright          = "Copyright (c) 1996-2009 iMatix Corporation";
char *EMBED__ipr_finfo_filename           = "ipr_finfo.icl ";
char *EMBED__ipr_finfo_builddate          = "2011/03/01 ";
char *EMBED__ipr_finfo_version_end        = "VeRsIoNeNd:ipc";

