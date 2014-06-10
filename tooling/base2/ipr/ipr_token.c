/*---------------------------------------------------------------------------
    ipr_token.c - ipr_token component

Provides string tokenisation functions.  Use this to split a string
into a list of words (delimited by spaces) so that each word can be
manipulated separately.
    Generated from ipr_token.icl by icl_gen using GSL/4.
    
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
#include "ipr_token.h"                  //  Definitions for our class

//  Shorthand for class type

#define self_t              ipr_token_t

//  Shorthands for class methods

#define self_split          ipr_token_split
#define self_split_rich     ipr_token_split_rich
#define self_selftest       ipr_token_selftest
#define self_new            ipr_token_new
#define self_remove_from_all_containers  ipr_token_remove_from_all_containers
#define self_show           ipr_token_show
#define self_terminate      ipr_token_terminate
#define self_destroy        ipr_token_destroy
#define self_annihilate     ipr_token_annihilate
#define self_alloc          ipr_token_alloc
#define self_free           ipr_token_free
#define self_link           ipr_token_link
#define self_unlink         ipr_token_unlink
#define self_cache_initialise  ipr_token_cache_initialise
#define self_cache_purge    ipr_token_cache_purge
#define self_cache_terminate  ipr_token_cache_terminate
#define self_show_animation  ipr_token_show_animation
#define self_new_in_scope   ipr_token_new_in_scope

#define ipr_token_annihilate(self)      ipr_token_annihilate_ (self, __FILE__, __LINE__)
static void
    ipr_token_annihilate_ (
ipr_token_t * ( * self_p ),             //  Reference to object reference
char * file,                            //  Source file
size_t line                             //  Line number
);

#define ipr_token_alloc()               ipr_token_alloc_ (__FILE__, __LINE__)
static ipr_token_t *
    ipr_token_alloc_ (
char * file,                            //  Source file for call
size_t line                             //  Line number for call
);

#define ipr_token_free(self)            ipr_token_free_ (self, __FILE__, __LINE__)
static void
    ipr_token_free_ (
ipr_token_t * self,                     //  Object reference
char * file,                            //  Source file
size_t line                             //  Line number
);

static void
    ipr_token_cache_initialise (
void);

static void
    ipr_token_cache_purge (
void);

static void
    ipr_token_cache_terminate (
void);

Bool
    ipr_token_animating = TRUE;         //  Animation enabled by default
static icl_cache_t
    *s_cache = NULL;


/*  -------------------------------------------------------------------------
    ipr_token_split

    Type: Component method
    Splits a string into a list of tokens delimited by whitespace.  Returns a new
    ipr_token_list item, which the caller must destroy.
    -------------------------------------------------------------------------
 */

ipr_token_list_t *
    ipr_token_split (
    char * string                       //  String to parse
)
{
char
    *word_start,
    *word_end;
ipr_token_t
    *token;
    ipr_token_list_t *
        tokens;                         //  Returned list

//
tokens = ipr_token_list_new ();
word_start = string;
while (*word_start) {
    while (isspace (*word_start))
        word_start++;
    word_end = word_start;
    while (*word_end && !isspace (*word_end))
        word_end++;

    if (word_end > word_start) {
        token = ipr_token_new ();
        icl_shortstr_ncpy (token->value, word_start, word_end - word_start);
        ipr_token_list_queue (tokens, token);
        ipr_token_unlink (&token);
    }
    word_start = word_end;
}

    return (tokens);
}
/*  -------------------------------------------------------------------------
    ipr_token_split_rich

    Type: Component method
    Splits a string into a list of tokens delimited by a set of user-specified
    delimiters.  Returns a new ipr_token_list item, which the caller must destroy.
    -------------------------------------------------------------------------
 */

ipr_token_list_t *
    ipr_token_split_rich (
    char * string,                      //  String to parse
    char * delims                       //  Delimiters
)
{
char
    *word_start,
    *word_end;
ipr_token_t
    *token;
    ipr_token_list_t *
        tokens;                         //  Returned list

//
tokens = ipr_token_list_new ();
word_start = string;
while (*word_start) {
    while (strchr (delims, *word_start))
        word_start++;
    word_end = word_start;
    while (*word_end && !strchr (delims, *word_end))
        word_end++;

    if (word_end > word_start) {
        token = ipr_token_new ();
        icl_shortstr_ncpy (token->value, word_start, word_end - word_start);
        ipr_token_list_queue (tokens, token);
        ipr_token_unlink (&token);
    }
    word_start = word_end;
}

    return (tokens);
}
/*  -------------------------------------------------------------------------
    ipr_token_selftest

    Type: Component method
    -------------------------------------------------------------------------
 */

void
    ipr_token_selftest (
void)
{
ipr_token_list_t
    *tokens;

//
tokens = ipr_token_split ("this is a word");
assert (ipr_token_list_count (tokens) == 4);
ipr_token_list_destroy (&tokens);

tokens = ipr_token_split ("  this  is  a  word  ");
assert (ipr_token_list_count (tokens) == 4);
ipr_token_list_destroy (&tokens);

tokens = ipr_token_split ("  ");
assert (ipr_token_list_count (tokens) == 0);
ipr_token_list_destroy (&tokens);

tokens = ipr_token_split ("  this-is-a-word  ");
assert (ipr_token_list_count (tokens) == 1);
ipr_token_list_destroy (&tokens);

tokens = ipr_token_split ("this-is-a-word");
assert (ipr_token_list_count (tokens) == 1);
ipr_token_list_destroy (&tokens);

tokens = ipr_token_split_rich ("this is a word", " ");
assert (ipr_token_list_count (tokens) == 4);
ipr_token_list_destroy (&tokens);

tokens = ipr_token_split_rich ("this  is;a  word", ";-");
assert (ipr_token_list_count (tokens) == 2);
ipr_token_list_destroy (&tokens);
}
/*  -------------------------------------------------------------------------
    ipr_token_new

    Type: Component method
    Creates and initialises a new ipr_token_t object, returns a
    reference to the created object.
    Initialises the new item to be a valid list item.
    -------------------------------------------------------------------------
 */

ipr_token_t *
    ipr_token_new_ (
    char * file,                        //  Source file for call
    size_t line                         //  Line number for call
)
{
    ipr_token_t *
        self = NULL;                    //  Object reference

    self = ipr_token_alloc_ (file, line);
    if (self) {
        self->object_tag   = IPR_TOKEN_ALIVE;
        self->links        = 1;
        self->zombie       = FALSE;
#if defined (DEBUG) || defined (BASE_HISTORY) || defined (BASE_HISTORY_IPR_TOKEN)
        self->history_last = 0;

        //  Track possession operation in history
        self->history_file  [0] = file;
        self->history_line  [0] = line;
        self->history_type  [0] = "new";
        self->history_links [0] = self->links;
#endif
#if defined (DEBUG)
        icl_mem_set_callback (self, ipr_token_show_);
#endif

self->list_next = self;
self->list_prev = self;
self->list_head = NULL;
}

    return (self);
}
/*  -------------------------------------------------------------------------
    ipr_token_remove_from_all_containers

    Type: Component method
    The method to call to remove an item from its container.  Is called by
    the 'destroy' method if the possession count hits zero.
    -------------------------------------------------------------------------
 */

void
    ipr_token_remove_from_all_containers (
    ipr_token_t * self                  //  The item
)
{

ipr_token_list_remove (self);
}
/*  -------------------------------------------------------------------------
    ipr_token_show

    Type: Component method
    -------------------------------------------------------------------------
 */

void
    ipr_token_show_ (
    void * item,                        //  The opaque pointer
    int opcode,                         //  The callback opcode
    FILE * trace_file,                  //  File to print to
    char * file,                        //  Source file
    size_t line                         //  Line number
)
{
    ipr_token_t
        *self;
    int
        container_links;
#if defined (DEBUG) || defined (BASE_HISTORY) || defined (BASE_HISTORY_IPR_TOKEN)
    qbyte
        history_index;
#endif


self = item;
container_links = 0;
if (self->list_head)
   container_links++;
    assert (opcode == ICL_CALLBACK_DUMP);

    fprintf (trace_file, "    <ipr_token zombie = \"%u\" links = \"%u\" containers = \"%u\" file = \"%s\" line = \"%lu\"  pointer = \"%p\" />\n", self->zombie, self->links, container_links, file, (unsigned long) line, self);
#if defined (DEBUG) || defined (BASE_HISTORY) || defined (BASE_HISTORY_IPR_TOKEN)
    if (self->history_last > IPR_TOKEN_HISTORY_LENGTH) {
        fprintf (trace_file, "        <!-- possess history too large (%d) - call iMatix-tech -->\n",
            self->history_last);
        history_index = (self->history_last + 1) % IPR_TOKEN_HISTORY_LENGTH;
        self->history_last %= IPR_TOKEN_HISTORY_LENGTH;
    }
    else
        history_index = 0;

    for (; history_index != self->history_last; history_index = (history_index + 1) % IPR_TOKEN_HISTORY_LENGTH) {
        fprintf (trace_file, "       <%s file = \"%s\" line = \"%lu\" links = \"%lu\" />\n",
            self->history_type [history_index],
            self->history_file [history_index],
            (unsigned long) self->history_line  [history_index],
            (unsigned long) self->history_links [history_index]);
    }
    fprintf (trace_file, "    </ipr_token>\n");
#endif

}
/*  -------------------------------------------------------------------------
    ipr_token_terminate

    Type: Component method
    -------------------------------------------------------------------------
 */

void
    ipr_token_terminate (
void)
{


}
/*  -------------------------------------------------------------------------
    ipr_token_destroy

    Type: Component method
    Destroys a ipr_token_t object. Takes the address of a
    ipr_token_t reference (a pointer to a pointer) and nullifies the
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
    ipr_token_destroy_ (
    ipr_token_t * ( * self_p ),         //  Reference to object reference
    char * file,                        //  Source fileSource file
    size_t line                         //  Line numberLine number
)
{
    ipr_token_t *
        self = *self_p;                 //  Dereferenced Reference to object reference

if (self) {
    if (icl_atomic_cas32 (&self->zombie, TRUE, FALSE) == FALSE)
        ipr_token_annihilate_ (self_p, file, line);

    if (self->links == 0) {
        icl_system_panic ("ipr_token", "E: missing link on ipr_token object");
        ipr_token_show_ (self, ICL_CALLBACK_DUMP, stderr, file, line);
        abort ();
    }

    if (icl_atomic_dec32 ((volatile qbyte *) &self->links) == 0)
        ipr_token_free_ ((ipr_token_t *) self, file, line);
    *self_p = NULL;
}
}
/*  -------------------------------------------------------------------------
    ipr_token_annihilate

    Type: Component method
    -------------------------------------------------------------------------
 */

static void
    ipr_token_annihilate_ (
    ipr_token_t * ( * self_p ),         //  Reference to object reference
    char * file,                        //  Source file
    size_t line                         //  Line number
)
{
#if defined (DEBUG) || defined (BASE_HISTORY) || defined (BASE_HISTORY_IPR_TOKEN)
    int
        history_last;
#endif

    ipr_token_t *
        self = *self_p;                 //  Dereferenced Reference to object reference

#if defined (DEBUG) || defined (BASE_HISTORY) || defined (BASE_HISTORY_IPR_TOKEN)
    //  Track possession operation in history
    history_last = icl_atomic_inc32 ((volatile qbyte *) &self->history_last) + 1;
    self->history_file  [history_last % IPR_TOKEN_HISTORY_LENGTH] = file;
    self->history_line  [history_last % IPR_TOKEN_HISTORY_LENGTH] = line;
    self->history_type  [history_last % IPR_TOKEN_HISTORY_LENGTH] = "destroy";
    self->history_links [history_last % IPR_TOKEN_HISTORY_LENGTH] = self->links;
#endif

    IPR_TOKEN_ASSERT_SANE (self);
    ipr_token_remove_from_all_containers (self);


}
/*  -------------------------------------------------------------------------
    ipr_token_alloc

    Type: Component method
    -------------------------------------------------------------------------
 */

static ipr_token_t *
    ipr_token_alloc_ (
    char * file,                        //  Source file for call
    size_t line                         //  Line number for call
)
{

    ipr_token_t *
        self = NULL;                    //  Object reference

//  Initialise cache if necessary
if (!s_cache)
    ipr_token_cache_initialise ();

self = (ipr_token_t *) icl_mem_cache_alloc_ (s_cache, file, line);
if (self)
    memset (self, 0, sizeof (ipr_token_t));



    return (self);
}
/*  -------------------------------------------------------------------------
    ipr_token_free

    Type: Component method
    Freess a ipr_token_t object.
    -------------------------------------------------------------------------
 */

static void
    ipr_token_free_ (
    ipr_token_t * self,                 //  Object reference
    char * file,                        //  Source file
    size_t line                         //  Line number
)
{
#if defined (DEBUG) || defined (BASE_HISTORY) || defined (BASE_HISTORY_IPR_TOKEN)
    int
        history_last;
#endif


    if (self) {
#if defined (DEBUG) || defined (BASE_HISTORY) || defined (BASE_HISTORY_IPR_TOKEN)
        //  Track possession operation in history
        history_last = icl_atomic_inc32 ((volatile qbyte *) &self->history_last) + 1;
        self->history_file  [history_last % IPR_TOKEN_HISTORY_LENGTH] = file;
        self->history_line  [history_last % IPR_TOKEN_HISTORY_LENGTH] = line;
        self->history_type  [history_last % IPR_TOKEN_HISTORY_LENGTH] = "free";
        self->history_links [history_last % IPR_TOKEN_HISTORY_LENGTH] = self->links;
#endif

        memset (&self->object_tag, 0, sizeof (ipr_token_t) - ((byte *) &self->object_tag - (byte *) self));
//        memset (self, 0, sizeof (ipr_token_t));
        self->object_tag = IPR_TOKEN_DEAD;
        icl_mem_free (self);
    }
    self = NULL;
}
/*  -------------------------------------------------------------------------
    ipr_token_link

    Type: Component method
    Adds a link (reference count) to an object.

    If the object has been zombified (ie destroyed while extra links were present),
    this method returns NULL and does not add any links.

    This method does not lock the object.
    -------------------------------------------------------------------------
 */

ipr_token_t *
    ipr_token_link_ (
    ipr_token_t * self,                 //  Not documented
    char * file,                        //  Source file for call
    size_t line                         //  Line number for call
)
{
#if defined (DEBUG) || defined (BASE_HISTORY) || defined (BASE_HISTORY_IPR_TOKEN)
    int
        history_last;
#endif

    if (self) {
        IPR_TOKEN_ASSERT_SANE (self);
        if (file)
            icl_mem_possess_ (self, file, line);
        icl_atomic_inc32 ((volatile qbyte *) &self->links);
#if defined (DEBUG) || defined (BASE_HISTORY) || defined (BASE_HISTORY_IPR_TOKEN)
        //  Track possession operation in history
        history_last = icl_atomic_inc32 ((volatile qbyte *) &self->history_last) + 1;
        self->history_file  [history_last % IPR_TOKEN_HISTORY_LENGTH] = file;
        self->history_line  [history_last % IPR_TOKEN_HISTORY_LENGTH] = line;
        self->history_type  [history_last % IPR_TOKEN_HISTORY_LENGTH] = "link";
        self->history_links [history_last % IPR_TOKEN_HISTORY_LENGTH] = self->links;
#endif
    }

    return (self);
}
/*  -------------------------------------------------------------------------
    ipr_token_unlink

    Type: Component method
    Removes a link (reference count) to an object.  Sets the object pointer to NULL
    to indicate that it is no longer valid.
    -------------------------------------------------------------------------
 */

void
    ipr_token_unlink_ (
    ipr_token_t * ( * self_p ),         //  Reference to object reference
    char * file,                        //  Source file for call
    size_t line                         //  Line number for call
)
{
#if defined (DEBUG) || defined (BASE_HISTORY) || defined (BASE_HISTORY_IPR_TOKEN)
    int
        history_last;
#endif

    ipr_token_t *
        self = *self_p;                 //  Dereferenced Reference to object reference

    if (self) {
        IPR_TOKEN_ASSERT_SANE (self);
        if (self->links == 0) {
            icl_system_panic ("ipr_token", "E: missing link on ipr_token object");
            ipr_token_show_ (self, ICL_CALLBACK_DUMP, stderr, file, line);
            abort ();
        }

#if defined (DEBUG) || defined (BASE_HISTORY) || defined (BASE_HISTORY_IPR_TOKEN)
        //  Track possession operation in history.  Pre-empt value of links
        //  after operation; otherwise race condition can result in writing
        //  to freed memory.
        history_last = icl_atomic_inc32 ((volatile qbyte *) &self->history_last) + 1;
        self->history_file  [history_last % IPR_TOKEN_HISTORY_LENGTH] = file;
        self->history_line  [history_last % IPR_TOKEN_HISTORY_LENGTH] = line;
        self->history_type  [history_last % IPR_TOKEN_HISTORY_LENGTH] = "unlink";
        self->history_links [history_last % IPR_TOKEN_HISTORY_LENGTH] = self->links - 1;
#endif
        if (icl_atomic_dec32 ((volatile qbyte *) &self->links) == 0) {

        if (icl_atomic_cas32 (&self->zombie, TRUE, FALSE) == FALSE)
            ipr_token_annihilate_ (self_p, file, line);
        ipr_token_free_ ((ipr_token_t *) self, file, line);
    }
    *self_p = NULL;
}
}
/*  -------------------------------------------------------------------------
    ipr_token_cache_initialise

    Type: Component method
    Initialise the cache and register purge method with the meta-cache.
    -------------------------------------------------------------------------
 */

static void
    ipr_token_cache_initialise (
void)
{

s_cache = icl_cache_get (sizeof (ipr_token_t));
icl_system_register (ipr_token_cache_purge, ipr_token_cache_terminate);
}
/*  -------------------------------------------------------------------------
    ipr_token_cache_purge

    Type: Component method
    -------------------------------------------------------------------------
 */

static void
    ipr_token_cache_purge (
void)
{

icl_mem_cache_purge (s_cache);

}
/*  -------------------------------------------------------------------------
    ipr_token_cache_terminate

    Type: Component method
    -------------------------------------------------------------------------
 */

static void
    ipr_token_cache_terminate (
void)
{

s_cache = NULL;

}
/*  -------------------------------------------------------------------------
    ipr_token_show_animation

    Type: Component method
    Enables animation of the component. Animation is sent to stdout.
    To enable animation you must generate using the option -animate:1.
    -------------------------------------------------------------------------
 */

void
    ipr_token_show_animation (
    Bool enabled                        //  Are we enabling or disabling animation?
)
{

ipr_token_animating = enabled;
}
/*  -------------------------------------------------------------------------
    ipr_token_new_in_scope

    Type: Component method
    -------------------------------------------------------------------------
 */

void
    ipr_token_new_in_scope_ (
    ipr_token_t * * self_p,             //  Not documented
    icl_scope_t * _scope,               //  Not documented
    char * file,                        //  Source file for call
    size_t line                         //  Line number for call
)
{
    icl_destroy_t *
        _destroy;                       //  Not documented

*self_p = ipr_token_new_ (file,line);

if (*self_p) {
    _destroy = icl_destroy_new   ((void * *) self_p, (icl_destructor_fn *) ipr_token_destroy_);
    icl_destroy_list_queue (_scope, _destroy);
    icl_destroy_unlink (&_destroy);
}
}
//  Embed the version information in the resulting binary

char *EMBED__ipr_token_version_start      = "VeRsIoNsTaRt:ipc";
char *EMBED__ipr_token_component          = "ipr_token ";
char *EMBED__ipr_token_version            = "1.0 ";
char *EMBED__ipr_token_copyright          = "Copyright (c) 1996-2009 iMatix Corporation";
char *EMBED__ipr_token_filename           = "ipr_token.icl ";
char *EMBED__ipr_token_builddate          = "2011/03/01 ";
char *EMBED__ipr_token_version_end        = "VeRsIoNeNd:ipc";

