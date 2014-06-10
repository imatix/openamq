/*---------------------------------------------------------------------------
    ipr_dict.c - ipr_dict component

Implements a symbol-table dictionary.  The dictionary holds string
values indexed by string keys.  The string values are duplicated.
All access to the dictionary is unlocked, and dictionary items do
not use links, so the dictionary should not be used from multiple
threads in parallel.
    Generated from ipr_dict.icl by icl_gen using GSL/4.
    
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
#include "ipr_dict.h"                   //  Definitions for our class

//  Shorthand for class type

#define self_t              ipr_dict_t

//  Shorthands for class methods

#define self_new            ipr_dict_new
#define self_annihilate     ipr_dict_annihilate
#define self_update         ipr_dict_update
#define self_assume         ipr_dict_assume
#define self_assume_fmt     ipr_dict_assume_fmt
#define self_selftest       ipr_dict_selftest
#define self_remove_from_all_containers  ipr_dict_remove_from_all_containers
#define self_show           ipr_dict_show
#define self_terminate      ipr_dict_terminate
#define self_destroy        ipr_dict_destroy
#define self_alloc          ipr_dict_alloc
#define self_free           ipr_dict_free
#define self_cache_initialise  ipr_dict_cache_initialise
#define self_cache_purge    ipr_dict_cache_purge
#define self_cache_terminate  ipr_dict_cache_terminate
#define self_show_animation  ipr_dict_show_animation
#define self_new_in_scope   ipr_dict_new_in_scope

static void
    ipr_dict_annihilate (
ipr_dict_t * ( * self_p )               //  Reference to object reference
);

#define ipr_dict_alloc()                ipr_dict_alloc_ (__FILE__, __LINE__)
static ipr_dict_t *
    ipr_dict_alloc_ (
char * file,                            //  Source file for call
size_t line                             //  Line number for call
);

static void
    ipr_dict_free (
ipr_dict_t * self                       //  Object reference
);

static void
    ipr_dict_cache_initialise (
void);

static void
    ipr_dict_cache_purge (
void);

static void
    ipr_dict_cache_terminate (
void);

Bool
    ipr_dict_animating = TRUE;          //  Animation enabled by default
static icl_cache_t
    *s_cache = NULL;


/*  -------------------------------------------------------------------------
    ipr_dict_new

    Type: Component method
    Creates and initialises a new ipr_dict_t object, returns a
    reference to the created object.
    Initialises a new hash table item and plases it into the specified hash
    table, if not null.
    Initialises the new item to be a valid list item.
    -------------------------------------------------------------------------
 */

ipr_dict_t *
    ipr_dict_new_ (
    char * file,                        //  Source file for call
    size_t line,                        //  Line number for call
    ipr_dict_table_t * table,           //  Table to insert into
    char * key,                         //  Hash key
    char * value                        //  Not documented
)
{
    ipr_dict_t *
        self = NULL;                    //  Object reference

    self = ipr_dict_alloc_ (file, line);
    if (self) {
        self->object_tag   = IPR_DICT_ALIVE;
#if defined (DEBUG)
        icl_mem_set_callback (self, ipr_dict_show_);
#endif

self->list_next = self;
self->list_prev = self;
self->list_head = NULL;
self->table_head = NULL;
self->table_index = 0;              //  Will be set by container
self->value = icl_mem_strdup (value);
if (table && self && ipr_dict_table_insert (table, key, self))
    ipr_dict_destroy (&self);
}

    return (self);
}
/*  -------------------------------------------------------------------------
    ipr_dict_annihilate

    Type: Component method
    -------------------------------------------------------------------------
 */

static void
    ipr_dict_annihilate (
    ipr_dict_t * ( * self_p )           //  Reference to object reference
)
{

    ipr_dict_t *
        self = *self_p;                 //  Dereferenced Reference to object reference

IPR_DICT_ASSERT_SANE (self);
ipr_dict_remove_from_all_containers (self);

icl_mem_free (self->value);

}
/*  -------------------------------------------------------------------------
    ipr_dict_update

    Type: Component method
    Accepts a ipr_dict_t reference and returns zero in case of success,
    1 in case of errors.
    -------------------------------------------------------------------------
 */

int
    ipr_dict_update (
    ipr_dict_t * self,                  //  Reference to object
    char * value                        //  Not documented
)
{
    int
        rc = 0;                         //  Return code

IPR_DICT_ASSERT_SANE (self);

icl_mem_free (self->value);
self->value = icl_mem_strdup (value);


    return (rc);
}
/*  -------------------------------------------------------------------------
    ipr_dict_assume

    Type: Component method
    Create or update symbol with specified value.  Returns TRUE if the symbol
    was created, FALSE if it already existed.
    -------------------------------------------------------------------------
 */

Bool
    ipr_dict_assume (
    ipr_dict_table_t * table,           //  Symbol table
    char * name,                        //  Symbol name
    char * value                        //  Symbol value
)
{
ipr_dict_t
    *item;                          //  Inserted item
    Bool
        rc = FALSE;                     //  Not documented

item = ipr_dict_table_search (table, name);
if (item) {
    if (value && strneq (value, item->value))
        ipr_dict_update (item, value);
}
else {
    ipr_dict_new (table, name, value);
    rc = TRUE;
}

    return (rc);
}
/*  -------------------------------------------------------------------------
    ipr_dict_assume_fmt

    Type: Component method
    Create or update symbol with formatted value. Allows printf-style
    formatting to be done on a value prior to inserting it into the
    symbol table.  That the formatted value may not be larger than a
    shortstr, or it will be truncated.
    -------------------------------------------------------------------------
 */

void
    ipr_dict_assume_fmt (
    ipr_dict_table_t * table,           //  Symbol table
    char * name,                        //  Symbol name
    char * format,                      //  Format specifier
...                                     //  Variable arguments
)
{
    va_list
        args;
    va_start (args, format);
    ipr_dict_assume_fmt_v (table, name, format, args );
    va_end (args);
}
/*  -------------------------------------------------------------------------
    ipr_dict_assume_fmt_v

    Type: Component method
    Create or update symbol with formatted value. Allows printf-style
    formatting to be done on a value prior to inserting it into the
    symbol table.  That the formatted value may not be larger than a
    shortstr, or it will be truncated.
    -------------------------------------------------------------------------
 */

void
    ipr_dict_assume_fmt_v (
    ipr_dict_table_t * table,           //  Symbol table
    char * name,                        //  Symbol name
    char * format,                      //  Format specifier
va_list args                            //  Variable arguments
)
{
icl_shortstr_t
    value;

//
apr_vsnprintf (value, ICL_SHORTSTR_MAX, format, args);
self_assume (table, name, value);
}
/*  -------------------------------------------------------------------------
    ipr_dict_selftest

    Type: Component method
    -------------------------------------------------------------------------
 */

void
    ipr_dict_selftest (
void)
{

//  selftest is in ipr_dict_table.icl
}
/*  -------------------------------------------------------------------------
    ipr_dict_remove_from_all_containers

    Type: Component method
    The method to call to remove an item from its container.  Is called by
    the 'destroy' method if the possession count hits zero.
    The method to call to remove an item from its container.  Is called by
    the 'destroy' method if the possession count hits zero.
    -------------------------------------------------------------------------
 */

void
    ipr_dict_remove_from_all_containers (
    ipr_dict_t * self                   //  The itemThe item
)
{

IPR_DICT_ASSERT_SANE (self);
ipr_dict_table_remove (self);
ipr_dict_list_remove (self);
}
/*  -------------------------------------------------------------------------
    ipr_dict_show

    Type: Component method
    -------------------------------------------------------------------------
 */

void
    ipr_dict_show_ (
    void * item,                        //  The opaque pointer
    int opcode,                         //  The callback opcode
    FILE * trace_file,                  //  File to print to
    char * file,                        //  Source file
    size_t line                         //  Line number
)
{
ipr_dict_t
    *self;
int
    container_links;


self = item;
container_links = 0;
if (self->table_head)
   container_links++;
if (self->list_head)
   container_links++;
assert (opcode == ICL_CALLBACK_DUMP);

fprintf (trace_file, "    <ipr_dict file = \"%s\" line = \"%lu\"  pointer = \"%p\" />\n", file, (unsigned long) line, self);

}
/*  -------------------------------------------------------------------------
    ipr_dict_terminate

    Type: Component method
    -------------------------------------------------------------------------
 */

void
    ipr_dict_terminate (
void)
{


}
/*  -------------------------------------------------------------------------
    ipr_dict_destroy

    Type: Component method
    Destroys a ipr_dict_t object. Takes the address of a
    ipr_dict_t reference (a pointer to a pointer) and nullifies the
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
    ipr_dict_destroy_ (
    ipr_dict_t * ( * self_p ),          //  Reference to object reference
    char * file,                        //  Source fileSource file
    size_t line                         //  Line numberLine number
)
{
    ipr_dict_t *
        self = *self_p;                 //  Dereferenced Reference to object reference

if (self) {
    ipr_dict_annihilate (self_p);
    ipr_dict_free ((ipr_dict_t *) self);
    *self_p = NULL;
}
}
/*  -------------------------------------------------------------------------
    ipr_dict_alloc

    Type: Component method
    -------------------------------------------------------------------------
 */

static ipr_dict_t *
    ipr_dict_alloc_ (
    char * file,                        //  Source file for call
    size_t line                         //  Line number for call
)
{

    ipr_dict_t *
        self = NULL;                    //  Object reference

//  Initialise cache if necessary
if (!s_cache)
    ipr_dict_cache_initialise ();

self = (ipr_dict_t *) icl_mem_cache_alloc_ (s_cache, file, line);
if (self)
    memset (self, 0, sizeof (ipr_dict_t));



    return (self);
}
/*  -------------------------------------------------------------------------
    ipr_dict_free

    Type: Component method
    Freess a ipr_dict_t object.
    -------------------------------------------------------------------------
 */

static void
    ipr_dict_free (
    ipr_dict_t * self                   //  Object reference
)
{


if (self) {

        memset (&self->object_tag, 0, sizeof (ipr_dict_t) - ((byte *) &self->object_tag - (byte *) self));
//        memset (self, 0, sizeof (ipr_dict_t));
        self->object_tag = IPR_DICT_DEAD;
        icl_mem_free (self);
    }
    self = NULL;
}
/*  -------------------------------------------------------------------------
    ipr_dict_cache_initialise

    Type: Component method
    Initialise the cache and register purge method with the meta-cache.
    -------------------------------------------------------------------------
 */

static void
    ipr_dict_cache_initialise (
void)
{

s_cache = icl_cache_get (sizeof (ipr_dict_t));
icl_system_register (ipr_dict_cache_purge, ipr_dict_cache_terminate);
}
/*  -------------------------------------------------------------------------
    ipr_dict_cache_purge

    Type: Component method
    -------------------------------------------------------------------------
 */

static void
    ipr_dict_cache_purge (
void)
{

icl_mem_cache_purge (s_cache);

}
/*  -------------------------------------------------------------------------
    ipr_dict_cache_terminate

    Type: Component method
    -------------------------------------------------------------------------
 */

static void
    ipr_dict_cache_terminate (
void)
{

s_cache = NULL;

}
/*  -------------------------------------------------------------------------
    ipr_dict_show_animation

    Type: Component method
    Enables animation of the component. Animation is sent to stdout.
    To enable animation you must generate using the option -animate:1.
    -------------------------------------------------------------------------
 */

void
    ipr_dict_show_animation (
    Bool enabled                        //  Are we enabling or disabling animation?
)
{

ipr_dict_animating = enabled;
}
/*  -------------------------------------------------------------------------
    ipr_dict_new_in_scope

    Type: Component method
    -------------------------------------------------------------------------
 */

void
    ipr_dict_new_in_scope_ (
    ipr_dict_t * * self_p,              //  Not documented
    icl_scope_t * _scope,               //  Not documented
    char * file,                        //  Source file for call
    size_t line,                        //  Line number for call
    ipr_dict_table_t * table,           //  Table to insert into
    char * key,                         //  Hash key
    char * value                        //  Not documented
)
{
    icl_destroy_t *
        _destroy;                       //  Not documented

*self_p = ipr_dict_new_ (file,line,table,key,value);

if (*self_p) {
    _destroy = icl_destroy_new   ((void * *) self_p, (icl_destructor_fn *) ipr_dict_destroy_);
    icl_destroy_list_queue (_scope, _destroy);
    icl_destroy_unlink (&_destroy);
}
}
//  Embed the version information in the resulting binary

char *EMBED__ipr_dict_version_start       = "VeRsIoNsTaRt:ipc";
char *EMBED__ipr_dict_component           = "ipr_dict ";
char *EMBED__ipr_dict_version             = "1.0 ";
char *EMBED__ipr_dict_copyright           = "Copyright (c) 1996-2009 iMatix Corporation";
char *EMBED__ipr_dict_filename            = "ipr_dict.icl ";
char *EMBED__ipr_dict_builddate           = "2011/03/01 ";
char *EMBED__ipr_dict_version_end         = "VeRsIoNeNd:ipc";

