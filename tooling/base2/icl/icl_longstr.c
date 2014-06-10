/*---------------------------------------------------------------------------
    icl_longstr.c - icl_longstr component

    We define the icl_longstr_t type in order to provide a secure way of
    storing and serialising long strings.  A "long string" is defined
    as one that can be serialised as a 32-bit length indicator
    plus 0-4Gb-1 octets of data. The icl_longstr_t type is held in memory
    as descriptor consisting of a size field and a data reference.
    Generated from icl_longstr.icl by icl_gen using GSL/4.
    
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
#include "icl_longstr.h"                //  Definitions for our class

//  Shorthand for class type

#define self_t              icl_longstr_t

//  Shorthands for class methods

#define self_new            icl_longstr_new
#define self_destroy        icl_longstr_destroy
#define self_resize         icl_longstr_resize
#define self_eq             icl_longstr_eq
#define self_dup            icl_longstr_dup
#define self_fmt            icl_longstr_fmt
#define self_cat            icl_longstr_cat
#define self_find           icl_longstr_find
#define self_selftest       icl_longstr_selftest
#define self_show_animation  icl_longstr_show_animation

Bool
    icl_longstr_animating = TRUE;       //  Animation enabled by default
/*  -------------------------------------------------------------------------
    icl_longstr_new

    Type: Component method
    Creates and initialises a new icl_longstr_t object, returns a
    reference to the created object.
    Allocates an icl_longstr_t block for a specified block of data.  Returns
    a pointer to an allocated icl_longstr_t, or NULL if there was not enough
    memory. If the data_block argument is not null, its contents are copied
    into the newly allocated memory area.
    -------------------------------------------------------------------------
 */

icl_longstr_t *
    icl_longstr_new_ (
    char * file,                        //  Source file for call
    size_t line,                        //  Line number for call
    void * data,                        //  Data to copy
    size_t size                         //  Length of data
)
{
    icl_longstr_t *
        self = NULL;                    //  Object reference

self = (icl_longstr_t *) icl_mem_alloc_ (sizeof (icl_longstr_t) + size, file, line);
if (self) {
    self->object_tag = ICL_LONGSTR_ALIVE;
    self->max_size = size;
    self->data = (byte *) self + sizeof (icl_longstr_t);
    if (data) {
        self->cur_size = size;
        memcpy (self->data, data, size);
    }
    else
        self->cur_size = 0;
}

    return (self);
}
/*  -------------------------------------------------------------------------
    icl_longstr_destroy

    Type: Component method
    Destroys a icl_longstr_t object. Takes the address of a
    icl_longstr_t reference (a pointer to a pointer) and nullifies the
    reference after use.  Does nothing if the reference is already
    null.
    -------------------------------------------------------------------------
 */

void
    icl_longstr_destroy_ (
    icl_longstr_t * ( * self_p ),       //  Reference to object reference
    char * file,                        //  Source file
    size_t line                         //  Line number
)
{
    icl_longstr_t *
        self = *self_p;                 //  Dereferenced Reference to object reference

if (self) {
    //  If data was reallocated independently, free it independently
    self->object_tag = ICL_LONGSTR_DEAD;
    if (self->data != (byte *) self + sizeof (icl_longstr_t))
        icl_mem_free (self->data);
    icl_mem_free (self);
    *self_p = NULL;
}
}
/*  -------------------------------------------------------------------------
    icl_longstr_resize

    Type: Component method
    Accepts a icl_longstr_t reference and returns zero in case of success,
    1 in case of errors.
    Increases or decreases the allocated size of the string as requested
    by the size argument.  Sets the max_size property to size, and the
    cur_size property to zero.
    -------------------------------------------------------------------------
 */

int
    icl_longstr_resize_ (
    icl_longstr_t * self,               //  Reference to object
    size_t size,                        //  New length of data
    char * file,                        //  Source file for call
    size_t line                         //  Line number for call
)
{
    int
        rc = 0;                         //  Return code

ICL_LONGSTR_ASSERT_SANE (self);
//  If data was reallocated independently, free it independently
if (self->data != (byte *) self + sizeof (icl_longstr_t))
    icl_mem_free (self->data);

self->data = icl_mem_alloc_ (size, file, line);
self->max_size = size;
self->cur_size = 0;

    return (rc);
}
/*  -------------------------------------------------------------------------
    icl_longstr_eq

    Type: Component method
    Returns TRUE if the two specified longstrings are equal.  Either
    of the strings may be null references.
    -------------------------------------------------------------------------
 */

int
    icl_longstr_eq (
    icl_longstr_t * self,               //  First string
    icl_longstr_t * second              //  String to compare with
)
{
    int
        rc = 0;                         //  Return code

//
if (self == NULL || second == NULL)
    rc = (self == second);
else {
    ICL_LONGSTR_ASSERT_SANE (self);
    ICL_LONGSTR_ASSERT_SANE (second);
    if (self->cur_size == second->cur_size
    &&  memcmp (self->data, second->data, self->cur_size) == 0)
        rc = TRUE;
}

    return (rc);
}
/*  -------------------------------------------------------------------------
    icl_longstr_dup

    Type: Component method
    Returns a new icl_longstr duplicating the supplied string.
    -------------------------------------------------------------------------
 */

icl_longstr_t *
    icl_longstr_dup (
    icl_longstr_t * self                //  Source string
)
{

//
if (self) {
    ICL_LONGSTR_ASSERT_SANE (self);
    self = self_new (self->data, self->cur_size);
}
else
    self = NULL;

    return (self);
}
/*  -------------------------------------------------------------------------
    icl_longstr_fmt

    Type: Component method
    Accepts a icl_longstr_t reference and returns zero in case of success,
    1 in case of errors.
    Formats an icl_longstr_t with the specified format and arguments.  If
    the string was too short for the requested output, it is reallocated
    with some elasticity.  Always terminates the longstr data with a null.
    -------------------------------------------------------------------------
 */

int
    icl_longstr_fmt (
    icl_longstr_t * self,               //  Reference to object
    char * format,                      //  Format specifier
...                                     //  Variable arguments
)
{
    int
        _rc;
    va_list
        args;
    va_start (args, format);
    _rc = icl_longstr_fmt_v (self, format, args );
    va_end (args);
    return _rc;
}
/*  -------------------------------------------------------------------------
    icl_longstr_fmt_v

    Type: Component method
    Accepts a icl_longstr_t reference and returns zero in case of success,
    1 in case of errors.
    Formats an icl_longstr_t with the specified format and arguments.  If
    the string was too short for the requested output, it is reallocated
    with some elasticity.  Always terminates the longstr data with a null.
    -------------------------------------------------------------------------
 */

int
    icl_longstr_fmt_v (
    icl_longstr_t * self,               //  Reference to object
    char * format,                      //  Format specifier
va_list args                            //  Variable arguments
)
{
    int
        rc = 0;                         //  Return code

ICL_LONGSTR_ASSERT_SANE (self);
//
self->cur_size = apr_vsnprintf ((char *) self->data, self->max_size, format, args);
if (self->cur_size >= self->max_size) {
    self_resize (self, self->cur_size * 2);
    self->cur_size = apr_vsnprintf ((char *) self->data, self->max_size, format, args);
}

    return (rc);
}
/*  -------------------------------------------------------------------------
    icl_longstr_cat

    Type: Component method
    Accepts a icl_longstr_t reference and returns zero in case of success,
    1 in case of errors.
    Appends a formatted string to the specified longstring and updates it
    current size.  The string is automatically resized if needed.
    -------------------------------------------------------------------------
 */

int
    icl_longstr_cat (
    icl_longstr_t * self,               //  Reference to object
    char * format,                      //  Format specifier
...                                     //  Variable arguments
)
{
    int
        _rc;
    va_list
        args;
    va_start (args, format);
    _rc = icl_longstr_cat_v (self, format, args );
    va_end (args);
    return _rc;
}
/*  -------------------------------------------------------------------------
    icl_longstr_cat_v

    Type: Component method
    Accepts a icl_longstr_t reference and returns zero in case of success,
    1 in case of errors.
    Appends a formatted string to the specified longstring and updates it
    current size.  The string is automatically resized if needed.
    -------------------------------------------------------------------------
 */

int
    icl_longstr_cat_v (
    icl_longstr_t * self,               //  Reference to object
    char * format,                      //  Format specifier
va_list args                            //  Variable arguments
)
{
size_t
    cat_size;
    int
        rc = 0;                         //  Return code

ICL_LONGSTR_ASSERT_SANE (self);
//
cat_size = apr_vsnprintf (
    (char *) self->data + self->cur_size,
    self->max_size - self->cur_size, format, args);
if (self->cur_size + cat_size + 1 > self->max_size) {
    self_resize (self, (self->cur_size + cat_size) * 2);
    cat_size = apr_vsnprintf (
        (char *) self->data + self->cur_size,
        self->max_size - self->cur_size, format, args);
}
self->cur_size += cat_size;

    return (rc);
}
/*  -------------------------------------------------------------------------
    icl_longstr_find

    Type: Component method
    Searches the longstr for a pattern, using the Boyer-Moore-Horspool-Sunday
    algorithm.  Returns a pointer to the pattern if found within the longstr,
    or NULL if the pattern was not found.
    -------------------------------------------------------------------------
 */

byte *
    icl_longstr_find (
    icl_longstr_t * self,               //  Reference to longstr
    size_t offset,                      //  Start searching at this offset
    byte * pattern,                     //  Pattern to look for
    size_t patsize                      //  Size of pattern, in octets
)
{
size_t
    shift [256];                    //  Shift distance for each value
size_t
    byte_nbr,                       //  Distance through pattern
    match_size;                     //  Size of matched part
byte
    *data_start,                    //  Where to start looking
    *match_base = NULL,             //  Base of match of pattern
    *match_ptr = NULL,              //  Point within current match
    *limit = NULL;                  //  Last potiental match point
    byte *
        location;                       //  Location of pattern, or null

//
assert (pattern);

//  Pattern must be smaller or equal in size to string
location = NULL;
data_start = self->data + offset;
if (patsize == 0)              //  Empty patterns match at start
    location = data_start;
else
if (self->cur_size >= patsize) {
    //  The shift table determines how far to shift before trying to match
    //  again, if a match at this point fails.  If the byte after where the
    //  end of our pattern falls is not in our pattern, then we start to
    //  match again after that byte; otherwise we line up the last occurence
    //  of that byte in our pattern under that byte, and try match again.
    //
    for (byte_nbr = 0; byte_nbr < 256; byte_nbr++)
        shift [byte_nbr] = patsize + 1;
    for (byte_nbr = 0; byte_nbr < patsize; byte_nbr++)
        shift [(byte) pattern [byte_nbr]] = patsize - byte_nbr;

    //  Search for the block, each time jumping up by the amount
    //  computed in the shift table
    limit = data_start + (self->cur_size - patsize + 1);
    assert (limit > data_start);

    for (match_base = data_start; match_base < limit && !location; match_base += shift [*(match_base + patsize)]) {
        match_ptr  = match_base;
        match_size = 0;

        //  Compare pattern until it all matches, or we find a difference
        while (*match_ptr++ == pattern [match_size++]) {
            assert (match_size <= patsize && match_ptr == (match_base + match_size));
            //  If we found a match, return the start address
            if (match_size >= patsize) {
                location = match_base;
                break;
            }
        }
    }
}

    return (location);
}
/*  -------------------------------------------------------------------------
    icl_longstr_selftest

    Type: Component method
    -------------------------------------------------------------------------
 */

void
    icl_longstr_selftest (
void)
{
icl_longstr_t
    *longstr;
byte
    *location;

    //
    longstr = icl_longstr_new ("....:....:....:....:", 20);
    icl_longstr_cat (longstr, "This is more");
    icl_longstr_cat (longstr, "We're now adding a whole lot more");
    assert (longstr->cur_size <= longstr->max_size);
    icl_longstr_destroy (&longstr);

#   define TEST_DATA  "   This is a string, a string I say, a string!   "
    longstr = icl_longstr_new (TEST_DATA, strlen (TEST_DATA));
    location = icl_longstr_find (longstr, 0, (byte *) "string", 6);
    assert (location - longstr->data == 13);
    icl_longstr_destroy (&longstr);
}
/*  -------------------------------------------------------------------------
    icl_longstr_show_animation

    Type: Component method
    Enables animation of the component. Animation is sent to stdout.
    To enable animation you must generate using the option -animate:1.
    -------------------------------------------------------------------------
 */

void
    icl_longstr_show_animation (
    Bool enabled                        //  Are we enabling or disabling animation?
)
{

icl_longstr_animating = enabled;
}
//  Embed the version information in the resulting binary

char *EMBED__icl_longstr_version_start    = "VeRsIoNsTaRt:ipc";
char *EMBED__icl_longstr_component        = "icl_longstr ";
char *EMBED__icl_longstr_version          = "1.0a1 ";
char *EMBED__icl_longstr_copyright        = "Copyright (c) 1996-2009 iMatix Corporation";
char *EMBED__icl_longstr_filename         = "icl_longstr.icl ";
char *EMBED__icl_longstr_builddate        = "2011/03/01 ";
char *EMBED__icl_longstr_version_end      = "VeRsIoNeNd:ipc";

