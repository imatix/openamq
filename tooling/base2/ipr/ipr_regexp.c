/*---------------------------------------------------------------------------
    ipr_regexp.c - ipr_regexp component

    Provides an interface to the PCRE library.  This is a snippet
    from the PCRE documentation:

    There are two different sets of meta-characters: those  that
    are  recognized anywhere in the pattern except within square
    brackets, and those that are recognized in square  brackets.
    Outside square brackets, the meta-characters are as follows:

           general escape character with several uses
    ^      assert start of subject (or line, in multiline mode)
    $      assert end of subject (or line, in multiline mode)
    .      match any character except newline (by default)
    [      start character class definition
    |      start of alternative branch
    (      start subpattern
    )      end subpattern
    ?      extends the meaning of (
           also 0 or 1 quantifier
           also quantifier minimizer
    *      0 or more quantifier
    +      1 or more quantifier
    {      start min/max quantifier

    Part of a pattern that is in square brackets is called a
    "character class".  In a character class the only meta-
    characters are:

           general escape character
    ^      negate the class, but only if the first character
    -      indicates character range
    ]      terminates the character class
    Generated from ipr_regexp.icl by icl_gen using GSL/4.
    
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
#include "ipr_regexp.h"                 //  Definitions for our class

//  Shorthand for class type

#define self_t              ipr_regexp_t

//  Structure of the ipr_regexp object

struct _ipr_regexp_t {
#if (defined (BASE_THREADSAFE))
    icl_rwlock_t *
        rwlock;
#endif
    dbyte
        object_tag;                     //  Object validity marker
pcre
    *regexp;                        //  Parsed regular expression
char
    *pattern;                       //  Copy of regexp
int
    match_count;                    //  Number of pattern matches
int
    *match_list;                    //  Matches
int
    match_size;                     //  Size of match list
};

//  Shorthands for class methods

#define self_new            ipr_regexp_new
#define self_annihilate     ipr_regexp_annihilate
#define self_compile        ipr_regexp_compile
#define self_match          ipr_regexp_match
#define self_eq             ipr_regexp_eq
#define self_selftest       ipr_regexp_selftest
#define self_terminate      ipr_regexp_terminate
#define self_show           ipr_regexp_show
#define self_destroy        ipr_regexp_destroy
#define self_alloc          ipr_regexp_alloc
#define self_free           ipr_regexp_free
#define self_read_lock      ipr_regexp_read_lock
#define self_write_lock     ipr_regexp_write_lock
#define self_unlock         ipr_regexp_unlock
#define self_cache_initialise  ipr_regexp_cache_initialise
#define self_cache_purge    ipr_regexp_cache_purge
#define self_cache_terminate  ipr_regexp_cache_terminate
#define self_show_animation  ipr_regexp_show_animation
#define self_new_in_scope   ipr_regexp_new_in_scope

static void
    ipr_regexp_annihilate (
ipr_regexp_t * ( * self_p )             //  Reference to object reference
);

#define ipr_regexp_alloc()              ipr_regexp_alloc_ (__FILE__, __LINE__)
static ipr_regexp_t *
    ipr_regexp_alloc_ (
char * file,                            //  Source file for call
size_t line                             //  Line number for call
);

static void
    ipr_regexp_free (
ipr_regexp_t * self                     //  Object reference
);

static void
    ipr_regexp_cache_initialise (
void);

static void
    ipr_regexp_cache_purge (
void);

static void
    ipr_regexp_cache_terminate (
void);

Bool
    ipr_regexp_animating = TRUE;        //  Animation enabled by default
static icl_cache_t
    *s_cache = NULL;


/*  -------------------------------------------------------------------------
    ipr_regexp_new

    Type: Component method
    Creates and initialises a new ipr_regexp_t object, returns a
    reference to the created object.
    Create new regular expression.  Note that we allow '`' to be used
    in place of '\' which causes difficulties for C and code generation.
    If you really want to use ` in the expression, double the character.
    If pattern is null, creates a new regexp object without expression.
    Before using the regexp you must compile it using the compile method.
    -------------------------------------------------------------------------
 */

ipr_regexp_t *
    ipr_regexp_new_ (
    char * file,                        //  Source file for call
    size_t line,                        //  Line number for call
    char * pattern                      //  Not documented
)
{
    ipr_regexp_t *
        self = NULL;                    //  Object reference

    self = ipr_regexp_alloc_ (file, line);
    if (self) {
        self->object_tag   = IPR_REGEXP_ALIVE;
#if (defined (BASE_THREADSAFE))
        self->rwlock = icl_rwlock_new ();
#endif
#if defined (DEBUG)
        icl_mem_set_callback (self, ipr_regexp_show_);
#endif

//
if (pattern)
    self_compile (self, pattern, TRUE);
}

    return (self);
}
/*  -------------------------------------------------------------------------
    ipr_regexp_annihilate

    Type: Component method
    -------------------------------------------------------------------------
 */

static void
    ipr_regexp_annihilate (
    ipr_regexp_t * ( * self_p )         //  Reference to object reference
)
{
#if (defined (BASE_THREADSAFE))
    icl_rwlock_t
        *rwlock;
#endif

    ipr_regexp_t *
        self = *self_p;                 //  Dereferenced Reference to object reference

    IPR_REGEXP_ASSERT_SANE (self);
#if (defined (BASE_THREADSAFE))
    rwlock = self->rwlock;
    if (rwlock)
         icl_rwlock_write_lock (rwlock);
#endif

icl_mem_free (self->pattern);
icl_mem_free (self->match_list);
if (self->regexp)
    pcre_free (self->regexp);
#if (defined (BASE_THREADSAFE))
    if (rwlock)
        icl_rwlock_unlock (rwlock);
#endif

}
/*  -------------------------------------------------------------------------
    ipr_regexp_compile

    Type: Component method
    Accepts a ipr_regexp_t reference and returns zero in case of success,
    1 in case of errors.
    Create new regular expression.  If csafe is set, allows '`' to be used
    in place of '\' which causes difficulties for C and code generation.
    If you really want to use ` in the expression, double the character.
    Do not call compile twice on one regexp object.  Returns -1 if the
    regexp was invalid.
    -------------------------------------------------------------------------
 */

int
    ipr_regexp_compile (
    ipr_regexp_t * self,                //  Reference to object
    char * pattern,                     //  Not documented
    Bool csafe                          //  Not documented
)
{
int
    pcre_offset;
char
    *scan,
    *pcre_error;                    //  Last error if any
    int
        rc = 0;                         //  Return code

IPR_REGEXP_ASSERT_SANE (self);

//
//  Copy the pattern so we can play with it
assert (!self->pattern);
self->pattern = icl_mem_strdup (pattern);
if (csafe) {
    scan = self->pattern;
    while (*scan) {
        if (*scan == '`') {
            *scan = '\\';
            if (*(scan + 1) == '`')
                scan++;                 //  Allow `` to mean `
        }
        scan++;
    }
}
self->regexp = pcre_compile (self->pattern, 0, (const char **) &pcre_error, &pcre_offset, NULL);
if (self->regexp) {
    pcre_fullinfo (self->regexp, NULL, PCRE_INFO_CAPTURECOUNT, &self->match_count);
    self->match_size = (self->match_count + 1) * 3;
    self->match_list = (int *) icl_mem_alloc (self->match_size * sizeof (int));
}
else
    rc = -1;


    return (rc);
}
/*  -------------------------------------------------------------------------
    ipr_regexp_match

    Type: Component method
    Accepts a ipr_regexp_t reference and returns zero in case of success,
    1 in case of errors.
    Parses a string using a regular expression and loads zero or more
    patterns into supplied strings. Returns the number of matched
    strings, or 1 if no strings were supplied and the expression
    matched. Returns zero if the expression did not match at all.
    -------------------------------------------------------------------------
 */

int
    ipr_regexp_match (
    ipr_regexp_t * self,                //  Reference to object
    char * string,                      //  Not documented
...                                     //  Not documented
)
{
    int
        _rc;
    va_list
        argptr;
    va_start (argptr, string);
    _rc = ipr_regexp_match_v (self, string, argptr );
    va_end (argptr);
    return _rc;
}
/*  -------------------------------------------------------------------------
    ipr_regexp_match_v

    Type: Component method
    Accepts a ipr_regexp_t reference and returns zero in case of success,
    1 in case of errors.
    Parses a string using a regular expression and loads zero or more
    patterns into supplied strings. Returns the number of matched
    strings, or 1 if no strings were supplied and the expression
    matched. Returns zero if the expression did not match at all.
    -------------------------------------------------------------------------
 */

int
    ipr_regexp_match_v (
    ipr_regexp_t * self,                //  Reference to object
    char * string,                      //  Not documented
va_list argptr                          //  Not documented
)
{
char
    **symbol;                       //  Passed to the function
int
    match_okay,                     //  Did string match pattern?
    match_index,                    //  Work through each match...
    start,
    length;
    int
        rc = 0;                         //  Return code

IPR_REGEXP_ASSERT_SANE (self);

assert (string);
match_okay = pcre_exec (
    self->regexp, NULL, string, strlen (string), 0, 0,
    self->match_list, self->match_size);

for (match_index = 0; match_index < self->match_count; match_index++) {
    symbol = va_arg (argptr, char **);
    if (symbol) {
        if (match_okay >= 0) {
            icl_mem_strfree (symbol);
            start  = self->match_list [match_index * 2 + 2];
            length = self->match_list [match_index * 2 + 3] - start;
            *symbol = (char *) icl_mem_alloc (length + 1);
            if (length > 0)
                memcpy (*symbol, string + start, length);
            (*symbol) [length] = 0;
        }
        else {
            //  No match, so return empty string
            if (*symbol) {
                icl_mem_strfree (symbol);
                *symbol = icl_mem_strdup ("");
            }
        }
    }
}
if (match_okay >= 0) {
    if (self->match_count)
        rc = self->match_count;     //  Matched N items
    else
        rc = 1;                     //  Match succeeded
}
else
    rc = 0;                         //  Match failed


    return (rc);
}
/*  -------------------------------------------------------------------------
    ipr_regexp_eq

    Type: Component method
    Matches a string with a regular expression and returns TRUE
    if the string matches, FALSE if not.  Does not require any
    set-up, but is slower than the match method when comparing
    many strings, since a ipr_regexp object is created and
    destroyed for each call.
    -------------------------------------------------------------------------
 */

int
    ipr_regexp_eq (
    char * regexp,                      //  Regular expression
    char * string                       //  String to compare
)
{
ipr_regexp_t
    *self;                          //  The compiled regexp
    int
        rc;                             //  Return value

self = self_new (regexp);
if (ipr_regexp_match (self, string))
    rc = TRUE;
else
    rc = FALSE;

self_destroy (&self);

    return (rc);
}
/*  -------------------------------------------------------------------------
    ipr_regexp_selftest

    Type: Component method
    -------------------------------------------------------------------------
 */

void
    ipr_regexp_selftest (
void)
{

char
    *regexp_str;                    //  The regexp string
ipr_regexp_t
    *regexp;                        //  The compiled regexp
char
    *item = NULL;                   //  A matched item
int
    nbr_items;                      //  Number of matched items

//  This is an example of a Perl5-compatible regular expression
//  Note that ipr_regexp lets us use ` instead of a back slash,
//  since back-slashes require a lot of escaping in iCL code.
//  The ( ) groups indicate matched items.  {} is used to define
//  min/max occurrences.
//
regexp_str = "^[0-9]{3}`-[0-9]{3}`-[0-9]{3,6}$";

//  The simplest way to get a yes/no match is to use ipr_regexp_eq
assert (ipr_regexp_eq  (regexp_str, "123-456-789"));
assert (ipr_regexp_eq  (regexp_str, "123-456-789123"));
assert (!ipr_regexp_eq (regexp_str, "123-456-7891239"));
assert (!ipr_regexp_eq (regexp_str, "123-456-abc"));

//  Here is an expression with three subpatterns:
regexp_str = "([0-9]+)`-([0-9]+)`-([0-9]+)";

//  We compile our regular expression into an ipr_regexp object
//  If we did a lot of matching on a single expression, it would
//  be worth keeping this object around.
//
regexp = ipr_regexp_new (regexp_str);

//  Now we do the matching
//  Note that item must be initialised to NULL, otherwise the
//  match method will free it first (to allow reuse).
//
nbr_items = ipr_regexp_match (regexp, "123-456-789", NULL, &item, NULL);

//  We must have matched all three items and extracted the second one
//
assert (nbr_items == 3);
assert (streq (item, "456"));

//  Clean up - destroy all objects and free allocated memory
//
ipr_regexp_destroy (&regexp);
icl_mem_free (item);

//  Let's try some more complex expressions
//
regexp_str = "^`w+`.eur$";
regexp = ipr_regexp_new (regexp_str);
assert (ipr_regexp_match (regexp, "stocks.eur") == 1);
assert (ipr_regexp_match (regexp, "stocks.usd") == 0);
ipr_regexp_destroy (&regexp);

//  To define subsequences that are not captured, we use (?: ... )
//
regexp_str = "^`w+(?:[`./]`w+)*[`./]eur$";
regexp = ipr_regexp_new (regexp_str);
assert (ipr_regexp_match (regexp, "stocks/aapl/eur") == 1);
assert (ipr_regexp_match (regexp, "stocks.msft/eur") == 1);
assert (ipr_regexp_match (regexp, "stocks/csco.usd") == 0);
ipr_regexp_destroy (&regexp);

regexp = ipr_regexp_new ("^^[a-zA-Z0-9-_.:]*$$");
for (nbr_items = 0; nbr_items < 10000; nbr_items++)
    ipr_regexp_match (regexp, "This-is-a-queue");
ipr_regexp_destroy (&regexp);

//  Heap stress test
for (nbr_items = 0; nbr_items < 25000; nbr_items++) {
    regexp = ipr_regexp_new ("^^[a-zA-Z0-9-_.:]*$$");
    ipr_regexp_destroy (&regexp);
}

regexp = ipr_regexp_new (NULL);
ipr_regexp_compile (regexp, "`w", FALSE);
assert (ipr_regexp_match (regexp, "`w"));
ipr_regexp_destroy (&regexp);
}
/*  -------------------------------------------------------------------------
    ipr_regexp_terminate

    Type: Component method
    -------------------------------------------------------------------------
 */

void
    ipr_regexp_terminate (
void)
{


}
/*  -------------------------------------------------------------------------
    ipr_regexp_show

    Type: Component method
    -------------------------------------------------------------------------
 */

void
    ipr_regexp_show_ (
    void * item,                        //  The opaque pointer
    int opcode,                         //  The callback opcode
    FILE * trace_file,                  //  File to print to
    char * file,                        //  Source file
    size_t line                         //  Line number
)
{
ipr_regexp_t
    *self;
int
    container_links;


self = item;
container_links = 0;
assert (opcode == ICL_CALLBACK_DUMP);

fprintf (trace_file, "    <ipr_regexp file = \"%s\" line = \"%lu\"  pointer = \"%p\" />\n", file, (unsigned long) line, self);

}
/*  -------------------------------------------------------------------------
    ipr_regexp_destroy

    Type: Component method
    Destroys a ipr_regexp_t object. Takes the address of a
    ipr_regexp_t reference (a pointer to a pointer) and nullifies the
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
    ipr_regexp_destroy_ (
    ipr_regexp_t * ( * self_p ),        //  Reference to object reference
    char * file,                        //  Source fileSource file
    size_t line                         //  Line numberLine number
)
{
    ipr_regexp_t *
        self = *self_p;                 //  Dereferenced Reference to object reference

if (self) {
    ipr_regexp_annihilate (self_p);
    ipr_regexp_free ((ipr_regexp_t *) self);
    *self_p = NULL;
}
}
/*  -------------------------------------------------------------------------
    ipr_regexp_alloc

    Type: Component method
    -------------------------------------------------------------------------
 */

static ipr_regexp_t *
    ipr_regexp_alloc_ (
    char * file,                        //  Source file for call
    size_t line                         //  Line number for call
)
{

    ipr_regexp_t *
        self = NULL;                    //  Object reference

//  Initialise cache if necessary
if (!s_cache)
    ipr_regexp_cache_initialise ();

self = (ipr_regexp_t *) icl_mem_cache_alloc_ (s_cache, file, line);
if (self)
    memset (self, 0, sizeof (ipr_regexp_t));



    return (self);
}
/*  -------------------------------------------------------------------------
    ipr_regexp_free

    Type: Component method
    Freess a ipr_regexp_t object.
    -------------------------------------------------------------------------
 */

static void
    ipr_regexp_free (
    ipr_regexp_t * self                 //  Object reference
)
{


if (self) {

#if (defined (BASE_THREADSAFE))
    if (self->rwlock)
        icl_rwlock_destroy (&self->rwlock);
#endif
        memset (&self->object_tag, 0, sizeof (ipr_regexp_t) - ((byte *) &self->object_tag - (byte *) self));
//        memset (self, 0, sizeof (ipr_regexp_t));
        self->object_tag = IPR_REGEXP_DEAD;
        icl_mem_free (self);
    }
    self = NULL;
}
/*  -------------------------------------------------------------------------
    ipr_regexp_read_lock

    Type: Component method
    Accepts a ipr_regexp_t reference and returns zero in case of success,
    1 in case of errors.
    -------------------------------------------------------------------------
 */

int
    ipr_regexp_read_lock (
    ipr_regexp_t * self                 //  Reference to object
)
{
    int
        rc = 0;                         //  Return code

IPR_REGEXP_ASSERT_SANE (self);

#if (defined (BASE_THREADSAFE))
    icl_rwlock_read_lock (self->rwlock);
#endif


    return (rc);
}
/*  -------------------------------------------------------------------------
    ipr_regexp_write_lock

    Type: Component method
    Accepts a ipr_regexp_t reference and returns zero in case of success,
    1 in case of errors.
    -------------------------------------------------------------------------
 */

int
    ipr_regexp_write_lock (
    ipr_regexp_t * self                 //  Reference to object
)
{
    int
        rc = 0;                         //  Return code

IPR_REGEXP_ASSERT_SANE (self);

#if (defined (BASE_THREADSAFE))
    icl_rwlock_write_lock (self->rwlock);
#endif


    return (rc);
}
/*  -------------------------------------------------------------------------
    ipr_regexp_unlock

    Type: Component method
    Accepts a ipr_regexp_t reference and returns zero in case of success,
    1 in case of errors.
    -------------------------------------------------------------------------
 */

int
    ipr_regexp_unlock (
    ipr_regexp_t * self                 //  Reference to object
)
{
    int
        rc = 0;                         //  Return code

IPR_REGEXP_ASSERT_SANE (self);

#if (defined (BASE_THREADSAFE))
    icl_rwlock_unlock (self->rwlock);
#endif



    return (rc);
}
/*  -------------------------------------------------------------------------
    ipr_regexp_cache_initialise

    Type: Component method
    Initialise the cache and register purge method with the meta-cache.
    -------------------------------------------------------------------------
 */

static void
    ipr_regexp_cache_initialise (
void)
{

s_cache = icl_cache_get (sizeof (ipr_regexp_t));
icl_system_register (ipr_regexp_cache_purge, ipr_regexp_cache_terminate);
}
/*  -------------------------------------------------------------------------
    ipr_regexp_cache_purge

    Type: Component method
    -------------------------------------------------------------------------
 */

static void
    ipr_regexp_cache_purge (
void)
{

icl_mem_cache_purge (s_cache);

}
/*  -------------------------------------------------------------------------
    ipr_regexp_cache_terminate

    Type: Component method
    -------------------------------------------------------------------------
 */

static void
    ipr_regexp_cache_terminate (
void)
{

s_cache = NULL;

}
/*  -------------------------------------------------------------------------
    ipr_regexp_show_animation

    Type: Component method
    Enables animation of the component. Animation is sent to stdout.
    To enable animation you must generate using the option -animate:1.
    -------------------------------------------------------------------------
 */

void
    ipr_regexp_show_animation (
    Bool enabled                        //  Are we enabling or disabling animation?
)
{

ipr_regexp_animating = enabled;
}
/*  -------------------------------------------------------------------------
    ipr_regexp_new_in_scope

    Type: Component method
    -------------------------------------------------------------------------
 */

void
    ipr_regexp_new_in_scope_ (
    ipr_regexp_t * * self_p,            //  Not documented
    icl_scope_t * _scope,               //  Not documented
    char * file,                        //  Source file for call
    size_t line,                        //  Line number for call
    char * pattern                      //  Not documented
)
{
    icl_destroy_t *
        _destroy;                       //  Not documented

*self_p = ipr_regexp_new_ (file,line,pattern);

if (*self_p) {
    _destroy = icl_destroy_new   ((void * *) self_p, (icl_destructor_fn *) ipr_regexp_destroy_);
    icl_destroy_list_queue (_scope, _destroy);
    icl_destroy_unlink (&_destroy);
}
}
//  Embed the version information in the resulting binary

char *EMBED__ipr_regexp_version_start     = "VeRsIoNsTaRt:ipc";
char *EMBED__ipr_regexp_component         = "ipr_regexp ";
char *EMBED__ipr_regexp_version           = "1.0 ";
char *EMBED__ipr_regexp_copyright         = "Copyright (c) 1996-2009 iMatix Corporation";
char *EMBED__ipr_regexp_filename          = "ipr_regexp.icl ";
char *EMBED__ipr_regexp_builddate         = "2011/03/01 ";
char *EMBED__ipr_regexp_version_end       = "VeRsIoNeNd:ipc";

