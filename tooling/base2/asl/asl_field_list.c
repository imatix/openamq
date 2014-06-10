/*---------------------------------------------------------------------------
    asl_field_list.c - asl_field_list component

    Provides functionality to implement the ASL RFC 'Field Table'
    data type.  This class provides functions at the field-table level.
    Each field table is represented as a list of fields.  This class
    provides methods to transform field lists to and from aslP binary
    formats, and lookup and operate on field lists.
    Generated from asl_field_list.icl by icl_gen using GSL/4.
    
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
#include "asl_field_list.h"             //  Definitions for our class

//  Shorthand for class type

#define self_t              asl_field_list_t

//  Shorthands for class methods

#define self_new            asl_field_list_new
#define self_annihilate     asl_field_list_annihilate
#define self_parse          asl_field_list_parse
#define self_flatten        asl_field_list_flatten
#define self_search         asl_field_list_search
#define self_cpy            asl_field_list_cpy
#define self_string         asl_field_list_string
#define self_integer        asl_field_list_integer
#define self_print          asl_field_list_print
#define self_dump           asl_field_list_dump
#define self_to_tree        asl_field_list_to_tree
#define self_build          asl_field_list_build
#define self_selftest       asl_field_list_selftest
#define self_isempty        asl_field_list_isempty
#define self_push           asl_field_list_push
#define self_pop            asl_field_list_pop
#define self_queue          asl_field_list_queue
#define self_relink_before  asl_field_list_relink_before
#define self_remove         asl_field_list_remove
#define self_first          asl_field_list_first
#define self_last           asl_field_list_last
#define self_next           asl_field_list_next
#define self_prev           asl_field_list_prev
#define self_rehook         asl_field_list_rehook
#define self_unhook         asl_field_list_unhook
#define self_terminate      asl_field_list_terminate
#define self_show           asl_field_list_show
#define self_destroy        asl_field_list_destroy
#define self_alloc          asl_field_list_alloc
#define self_free           asl_field_list_free
#define self_link           asl_field_list_link
#define self_unlink         asl_field_list_unlink
#define self_show_animation  asl_field_list_show_animation
#define self_new_in_scope   asl_field_list_new_in_scope

#define asl_field_list_annihilate(self)  asl_field_list_annihilate_ (self, __FILE__, __LINE__)
static void
    asl_field_list_annihilate_ (
asl_field_list_t * ( * self_p ),        //  Reference to object reference
char * file,                            //  Source file
size_t line                             //  Line number
);

static inline void
    asl_field_list_rehook (
asl_field_t * item,                     //  Not documented
asl_field_t * left,                     //  Not documented
asl_field_t * right                     //  Not documented
);

static inline asl_field_t *
    asl_field_list_unhook (
asl_field_t * item                      //  Not documented
);

#define asl_field_list_alloc()          asl_field_list_alloc_ (__FILE__, __LINE__)
static asl_field_list_t *
    asl_field_list_alloc_ (
char * file,                            //  Source file for call
size_t line                             //  Line number for call
);

#define asl_field_list_free(self)       asl_field_list_free_ (self, __FILE__, __LINE__)
static void
    asl_field_list_free_ (
asl_field_list_t * self,                //  Object reference
char * file,                            //  Source file
size_t line                             //  Line number
);

Bool
    asl_field_list_animating = TRUE;    //  Animation enabled by default

/*  -------------------------------------------------------------------------
    asl_field_list_new

    Type: Component method
    Creates and initialises a new asl_field_list_t object, returns a
    reference to the created object.
    If the field table argument is not null, parses the field table into
    the field list.  If the field table contained invalid data, destroys
    self.
    -------------------------------------------------------------------------
 */

asl_field_list_t *
    asl_field_list_new_ (
    char * file,                        //  Source file for call
    size_t line,                        //  Line number for call
    icl_longstr_t * field_table         //  Field table
)
{
    asl_field_list_t *
        self = NULL;                    //  Object reference

    self = asl_field_list_alloc_ (file, line);
    if (self) {
        self->object_tag   = ASL_FIELD_LIST_ALIVE;
        self->links        = 1;
        self->zombie       = FALSE;
#if defined (DEBUG) || defined (BASE_HISTORY) || defined (BASE_HISTORY_ASL_FIELD_LIST)
        self->history_last = 0;

        //  Track possession operation in history
        self->history_file  [0] = file;
        self->history_line  [0] = line;
        self->history_type  [0] = "new";
        self->history_links [0] = self->links;
#endif
#if defined (DEBUG)
        icl_mem_set_callback (self, asl_field_list_show_);
#endif

self->asl_field.list_next = &self->asl_field;
self->asl_field.list_prev = &self->asl_field;
self->asl_field.list_head = self;

//
if (field_table)
    if (self_parse (self, field_table))
        self_destroy (&self);
}

    return (self);
}
/*  -------------------------------------------------------------------------
    asl_field_list_annihilate

    Type: Component method
    Destroys the list and unlinks all attached items.
    -------------------------------------------------------------------------
 */

static void
    asl_field_list_annihilate_ (
    asl_field_list_t * ( * self_p ),    //  Reference to object reference
    char * file,                        //  Source file
    size_t line                         //  Line number
)
{
#if defined (DEBUG) || defined (BASE_HISTORY) || defined (BASE_HISTORY_ASL_FIELD_LIST)
    int
        history_last;
#endif

asl_field_t
    *item;
    asl_field_list_t *
        self = *self_p;                 //  Dereferenced Reference to object reference

#if defined (DEBUG) || defined (BASE_HISTORY) || defined (BASE_HISTORY_ASL_FIELD_LIST)
    //  Track possession operation in history
    history_last = icl_atomic_inc32 ((volatile qbyte *) &self->history_last) + 1;
    self->history_file  [history_last % ASL_FIELD_LIST_HISTORY_LENGTH] = file;
    self->history_line  [history_last % ASL_FIELD_LIST_HISTORY_LENGTH] = line;
    self->history_type  [history_last % ASL_FIELD_LIST_HISTORY_LENGTH] = "destroy";
    self->history_links [history_last % ASL_FIELD_LIST_HISTORY_LENGTH] = self->links;
#endif

    ASL_FIELD_LIST_ASSERT_SANE (self);

item = (asl_field_t *) self->asl_field.list_next;
while (item != &self->asl_field) {
    //  Unlink child without lock (we already have the lock)
    asl_field_list_unhook (item);

    asl_field_unlink (&item);
    item = (asl_field_t *) self->asl_field.list_next;
}

}
/*  -------------------------------------------------------------------------
    asl_field_list_parse

    Type: Component method
    Accepts a asl_field_list_t reference and returns zero in case of success,
    1 in case of errors.
    Parses a longstr string into a list of fields.  Does not empty the
    list before appending new fields to it.  Returns the number of fields
    parsed, or -1 if the supplied string was invalid.
    -------------------------------------------------------------------------
 */

int
    asl_field_list_parse (
    asl_field_list_t * self,            //  Reference to object
    icl_longstr_t * field_table         //  Field table
)
{
byte
    *input,                         //  Current position in string
    *limit;                         //  Limit of string (max + 1)
    int
        rc = 0;                         //  Return code

ASL_FIELD_LIST_ASSERT_SANE (self);
if (!self->zombie) {

//
assert (field_table);

input = field_table->data;
limit = input + field_table->cur_size;
while (input < limit) {
    input = asl_field_parse (self, input, limit);
    if (!input) {
        rc = -1;                    //  Error in fields
        break;
    }
}
}
else
    rc = -1;                        //  Return error on zombie object.


    return (rc);
}
/*  -------------------------------------------------------------------------
    asl_field_list_flatten

    Type: Component method
    Encodes the field list as a longstr string.
    -------------------------------------------------------------------------
 */

icl_longstr_t *
    asl_field_list_flatten (
    asl_field_list_t * self             //  Reference to object
)
{
size_t
    flat_size = 0;
asl_field_t
    *field;                         //  Field object
    icl_longstr_t *
        field_table;                    //  Field table

//
//  Compute required total size for all fields
field = self_first (self);
while (field) {
    flat_size += asl_field_flat_size (field);
    field = self_next (&field);
}
//  Flatten all fields into a string
field_table = icl_longstr_new (NULL, flat_size);
field = self_first (self);
while (field) {
    asl_field_flatten (field, field_table);
    field = self_next (&field);
}

    return (field_table);
}
/*  -------------------------------------------------------------------------
    asl_field_list_search

    Type: Component method
    Looks for a field with the specified name, returns the field object
    if found, else NULL.  Field names are case-significant.
    -------------------------------------------------------------------------
 */

asl_field_t *
    asl_field_list_search (
    asl_field_list_t * self,            //  Reference to object
    char * name                         //  Field name
)
{
    asl_field_t *
        field;                          //  Field object

//
field = self_first (self);
while (field) {
    if (streq (field->name, name))
        break;
    field = self_next (&field);
}

    return (field);
}
/*  -------------------------------------------------------------------------
    asl_field_list_cpy

    Type: Component method
    Accepts a asl_field_list_t reference and returns zero in case of success,
    1 in case of errors.
    Looks for a field with the specified name, if found copies the
    field value into the specified string, else clears the string.
    -------------------------------------------------------------------------
 */

int
    asl_field_list_cpy (
    asl_field_list_t * self,            //  Reference to objectReference to object
    char * dest,                        //  Destination shortstr
    char * name                         //  Field name
)
{
asl_field_t
    *field;
    int
        rc = 0;                         //  Return code

ASL_FIELD_LIST_ASSERT_SANE (self);
if (!self->zombie) {

//
field = asl_field_list_search (self, name);
if (field) {
    icl_shortstr_cpy (dest, asl_field_string (field));
    asl_field_unlink (&field);
}
else
    strclr (dest);
}
else
    rc = -1;                        //  Return error on zombie object.


    return (rc);
}
/*  -------------------------------------------------------------------------
    asl_field_list_string

    Type: Component method
    Looks for a field with the specified name, returns the field's string
    value if found, else NULL.
    -------------------------------------------------------------------------
 */

char *
    asl_field_list_string (
    asl_field_list_t * self,            //  Reference to object
    char * name                         //  Field name
)
{
asl_field_t
    *field;
    char *
        value;                          //  String value to return

//
field = self_search (self, name);
if (field) {
    value = asl_field_string (field);
    asl_field_unlink (&field);
}
else
    value = NULL;

    return (value);
}
/*  -------------------------------------------------------------------------
    asl_field_list_integer

    Type: Component method
    Looks for a field with the specified name, returns the field's integer
    value if found, else zero.
    -------------------------------------------------------------------------
 */

long
    asl_field_list_integer (
    asl_field_list_t * self,            //  Reference to object
    char * name                         //  Field name
)
{
asl_field_t
    *field;
    long
        value;                          //  Integer value to return

//
field = self_search (self, name);
if (field) {
    value = asl_field_integer (field);
    asl_field_unlink (&field);
}
else
    value = 0;

    return (value);
}
/*  -------------------------------------------------------------------------
    asl_field_list_print

    Type: Component method
    Accepts a asl_field_list_t reference and returns zero in case of success,
    1 in case of errors.
    Dumps the contents of the field list for debugging purposes.
    -------------------------------------------------------------------------
 */

int
    asl_field_list_print (
    asl_field_list_t * self             //  Reference to object
)
{
asl_field_t
    *field;                         //  Field object
    int
        rc = 0;                         //  Return code

ASL_FIELD_LIST_ASSERT_SANE (self);
if (!self->zombie) {

//
field = self_first (self);
while (field) {
    asl_field_print (field);
    field = self_next (&field);
}
}
else
    rc = -1;                        //  Return error on zombie object.


    return (rc);
}
/*  -------------------------------------------------------------------------
    asl_field_list_dump

    Type: Component method
    Accepts a asl_field_list_t reference and returns zero in case of success,
    1 in case of errors.
    Dumps the contents of the field list as a NULL terminated icl_shortstr_t
    of NAME=VALUE pairs separated by spaces.
    -------------------------------------------------------------------------
 */

int
    asl_field_list_dump (
    asl_field_list_t * self,            //  Reference to object
    char * dest                         //  Not documented
)
{
asl_field_t
    *field;                         //  Field object
icl_longstr_t
    *string;                        //  Used to build up the final string
    int
        rc = 0;                         //  Return code

ASL_FIELD_LIST_ASSERT_SANE (self);
if (!self->zombie) {

//
//  Create empty longstr to store the result
string = icl_longstr_new (NULL, ICL_SHORTSTR_MAX + 1);
string->data [0] = 0;

//  Dump individual fields of the list
field = self_first (self);
while (field) {
    //  Need a copy here since string may get reallocated
    char
        *append = icl_mem_strdup ((char *) string->data);

    icl_longstr_fmt (string, "%s%s=%s ", append, field->name, asl_field_string (field));
    field = self_next (&field);
    icl_mem_strfree (&append);
}
string->data [ICL_SHORTSTR_MAX] = 0;
icl_shortstr_cpy (dest, (char *) string->data);
icl_longstr_destroy (&string);
}
else
    rc = -1;                        //  Return error on zombie object.


    return (rc);
}
/*  -------------------------------------------------------------------------
    asl_field_list_to_tree

    Type: Component method
    Accepts a asl_field_list_t reference and returns zero in case of success,
    1 in case of errors.
    Converts the field list to a set of leaves in the provided tree.  
    The tree is built using the ipr_tree class.
    -------------------------------------------------------------------------
 */

int
    asl_field_list_to_tree (
    asl_field_list_t * self,            //  Reference to object
    ipr_tree_t * tree                   //  Not documented
)
{
asl_field_t
    *field;                         //  Field object
    int
        rc = 0;                         //  Return code

ASL_FIELD_LIST_ASSERT_SANE (self);
if (!self->zombie) {

//
//  Dump individual fields of the list
field = self_first (self);
while (field) {
    ipr_tree_leaf (tree, field->name, asl_field_string (field));
    field = self_next (&field);
}
}
else
    rc = -1;                        //  Return error on zombie object.


    return (rc);
}
/*  -------------------------------------------------------------------------
    asl_field_list_build

    Type: Component method
    Builds a flattened field list suitable for sending across a socket
    or writing to a file.  The field list is limited to string values.
    Accepts a series of name, value argument pairs, ending in a null
    name. Returns a icl_longstr, which the caller must destroy after use.
    -------------------------------------------------------------------------
 */

icl_longstr_t *
    asl_field_list_build (
    char * start,                       //  First field name
...                                     //  Variable arguments
)
{
    icl_longstr_t *
        _rc;
    va_list
        argptr;
    va_start (argptr, start);
    _rc = asl_field_list_build_v (start, argptr );
    va_end (argptr);
    return _rc;
}
/*  -------------------------------------------------------------------------
    asl_field_list_build_v

    Type: Component method
    Builds a flattened field list suitable for sending across a socket
    or writing to a file.  The field list is limited to string values.
    Accepts a series of name, value argument pairs, ending in a null
    name. Returns a icl_longstr, which the caller must destroy after use.
    -------------------------------------------------------------------------
 */

icl_longstr_t *
    asl_field_list_build_v (
    char * start,                       //  First field name
va_list argptr                          //  Variable arguments
)
{
char
    *field_name,
    *field_value;
asl_field_list_t
    *field_list;
    icl_longstr_t *
        string;                         //  Flat field list

//
field_name = start;
field_list = self_new (NULL);
while (field_name) {
    field_value = va_arg (argptr, char *);
    asl_field_new_string (field_list, field_name, field_value);
    field_name  = va_arg (argptr, char *);
}
string = asl_field_list_flatten (field_list);
asl_field_list_destroy (&field_list);

    return (string);
}
/*  -------------------------------------------------------------------------
    asl_field_list_selftest

    Type: Component method
    -------------------------------------------------------------------------
 */

void
    asl_field_list_selftest (
void)
{
asl_field_list_t
    *list;
icl_longstr_t
    *string1,
    *string2;
icl_shortstr_t
    shortstr;

//
list = asl_field_list_new (NULL);
asl_field_new_string  (list, "testfield0", "Collateral damage");
asl_field_new_integer (list, "testfield1", 1234567890);
asl_field_new_decimal (list, "testfield2", 199900, 2);
asl_field_new_time    (list, "testfield3", time (NULL));
asl_field_new_string  (list, "testfield4", "S1");
asl_field_new_string  (list, "testfield5", "P12");
string1 = asl_field_list_flatten (list);
asl_field_list_dump (list, shortstr);
asl_field_list_destroy (&list);

list = asl_field_list_new (string1);
string2 = asl_field_list_flatten (list);
asl_field_list_destroy (&list);

assert (string1->cur_size == string2->cur_size);
icl_longstr_destroy (&string1);
icl_longstr_destroy (&string2);

//  We can build a flattened field list in one step
string1 = asl_field_list_build (
    "host",  "Sleeper Service",
    "guest", "My Homework Ate My Dog",
    NULL);
icl_longstr_destroy (&string1);
}
/*  -------------------------------------------------------------------------
    asl_field_list_isempty

    Type: Component method
    Accepts a asl_field_list_t reference and returns zero in case of success,
    1 in case of errors.
    Returns true if the list is empty, else returns false.
    -------------------------------------------------------------------------
 */

int
    asl_field_list_isempty (
    asl_field_list_t * self             //  Reference to object
)
{
    int
        rc = 0;                         //  Return code

ASL_FIELD_LIST_ASSERT_SANE (self);
if (!self->zombie) {

rc = (self->asl_field.list_next == &self->asl_field);
}
else
    rc = -1;                        //  Return error on zombie object.


    return (rc);
}
/*  -------------------------------------------------------------------------
    asl_field_list_push

    Type: Component method
    Accepts a asl_field_list_t reference and returns zero in case of success,
    1 in case of errors.
    Attaches the item to the start of the specified list. If the item
    was on another list, it is first detached.  After this method,
    list->next points to the item.
    -------------------------------------------------------------------------
 */

int
    asl_field_list_push (
    asl_field_list_t * self,            //  Reference to object
    asl_field_t * item                  //  Not documented
)
{


    int
        rc = 0;                         //  Return code



ASL_FIELD_LIST_ASSERT_SANE (self);
if (!self->zombie) {

//  If list_head is non-null then we are moving within the
//  list to another so don't need to adjust links. Otherwise it
//  is illegitimate to move from one list to another in one step,
//  because we don't have a lock on the other list.
if (! item->list_head) {
    asl_field_link (item);
}
else
if (item->list_head != self) {
    icl_console_print ("E: asl_field item inserted into multiple list containers");
    assert (item->list_head == self);
}
asl_field_list_unhook (item);
asl_field_list_rehook (item, &self->asl_field, (asl_field_t *) self->asl_field.list_next);
}
else
    rc = -1;                        //  Return error on zombie object.




    return (rc);
}
/*  -------------------------------------------------------------------------
    asl_field_list_pop

    Type: Component method
    Removes the next item in the list, if any, and returns it.  If
    the list was empty, returns NULL.

    The number of links is unchanged; removing from the list and returning
    the value cancel each other out.
    -------------------------------------------------------------------------
 */

asl_field_t *
    asl_field_list_pop (
    asl_field_list_t * self             //  The list
)
{


    asl_field_t *
        item = NULL;                    //  Not documented



ASL_FIELD_LIST_ASSERT_SANE (self);
if (!self->zombie) {

if (self->asl_field.list_next == &self->asl_field)
    item = NULL;
else {
    item = asl_field_list_unhook ((asl_field_t *) self->asl_field.list_next);
}
}




    return (item);
}
/*  -------------------------------------------------------------------------
    asl_field_list_queue

    Type: Component method
    Accepts a asl_field_list_t reference and returns zero in case of success,
    1 in case of errors.
    Attaches the item to the end of the specified list.  The item may not
    be in another list of the same class.

    The number of links is automatically adjusted - if the item was already
    in the list, it remains unchanged; otherwise it is incremented.
    -------------------------------------------------------------------------
 */

int
    asl_field_list_queue (
    asl_field_list_t * self,            //  Reference to object
    asl_field_t * item                  //  Not documented
)
{


    int
        rc = 0;                         //  Return code



ASL_FIELD_LIST_ASSERT_SANE (self);
if (!self->zombie) {

//  If list_head is non-null then we are moving within the
//  list to another so don't need to adjust links. Otherwise it
//  is illegitimate to move from one list to another in one step,
//  because we don't have a lock on the other list.
if (! item->list_head) {
    asl_field_link (item);
}
else
if (item->list_head != self) {
    icl_console_print ("E: asl_field item inserted into multiple list containers");
    assert (item->list_head == self);
}
asl_field_list_unhook (item);
asl_field_list_rehook (item, (asl_field_t *) self->asl_field.list_prev, &self->asl_field);
}
else
    rc = -1;                        //  Return error on zombie object.




    return (rc);
}
/*  -------------------------------------------------------------------------
    asl_field_list_relink_before

    Type: Component method
    Accepts a asl_field_list_t reference and returns zero in case of success,
    1 in case of errors.
    Attaches the item before a specified item. If the item was on another list,
    it is first detached.

    The number of links is automatically adjusted - if the item was previously
    in a list, it remains unchanged; otherwise it is incremented.
    -------------------------------------------------------------------------
 */

int
    asl_field_list_relink_before (
    asl_field_t * item,                 //  Not documented
    asl_field_t * where                 //  Not documented
)
{


    int
        rc = 0;                         //  Return code
    asl_field_list_t *
        self = (asl_field_list_t *) where->list_head;  //  The list



ASL_FIELD_LIST_ASSERT_SANE (self);
if (!self->zombie) {

//  If list_head is non-null then we are moving within the
//  list to another so don't need to adjust links. Otherwise it
//  is illegitimate to move from one list to another in one step,
//  because we don't have a lock on the other list.
if (! item->list_head) {
    asl_field_link (item);
}
else
if (item->list_head != self) {
    icl_console_print ("E: asl_field item inserted into multiple list containers");
    assert (item->list_head == self);
}
asl_field_list_unhook (item);
asl_field_list_rehook (item, (asl_field_t *) where->list_prev, where);
}
else
    rc = -1;                        //  Return error on zombie object.




    return (rc);
}
/*  -------------------------------------------------------------------------
    asl_field_list_remove

    Type: Component method
    Removes the item from any list it is in.

    If it was in a list, the number of links is decremented.
    -------------------------------------------------------------------------
 */

int
    asl_field_list_remove (
    asl_field_t * item                  //  Not documented
)
{


    asl_field_list_t *
        self = (asl_field_list_t *) item->list_head;  //  The list
    int
        rc = 0;                         //  Not documented



if (self
&&  self == item->list_head) { //       Catch case where item moved
    asl_field_list_unhook (item);
    asl_field_unlink (&item);
}



    return (rc);
}
/*  -------------------------------------------------------------------------
    asl_field_list_first

    Type: Component method
    Returns the first asl_field item on the list.     If the list is empty,
    returns null.
    -------------------------------------------------------------------------
 */

asl_field_t *
    asl_field_list_first_ (
    asl_field_list_t * self,            //  The list
    char * file,                        //  Source file for call
    size_t line                         //  Line number for call
)
{


    asl_field_t *
        item = NULL;                    //  Not documented



ASL_FIELD_LIST_ASSERT_SANE (self);
if (!self->zombie) {

item = (asl_field_t *) self->asl_field.list_next;
if (item == &self->asl_field)
    item = NULL;
else
    asl_field_link (item);

}




    return (item);
}
/*  -------------------------------------------------------------------------
    asl_field_list_last

    Type: Component method
    Returns the last asl_field item on the list.     If the list is empty,
    returns null.
    -------------------------------------------------------------------------
 */

asl_field_t *
    asl_field_list_last_ (
    asl_field_list_t * self,            //  The list
    char * file,                        //  Source file for call
    size_t line                         //  Line number for call
)
{


    asl_field_t *
        item = NULL;                    //  Not documented



ASL_FIELD_LIST_ASSERT_SANE (self);
if (!self->zombie) {

item = (asl_field_t *) self->asl_field.list_prev;
if (item == &self->asl_field)
    item = NULL;
else
    asl_field_link (item);

}




    return (item);
}
/*  -------------------------------------------------------------------------
    asl_field_list_next

    Type: Component method
    Returns the following item on the asl_field list. If the provided item
    is null, returns the first item.  If there were no (more) items, returns
    null.
    -------------------------------------------------------------------------
 */

asl_field_t *
    asl_field_list_next_ (
    asl_field_t * ( * item_p ),         //  Not documented
    char * file,                        //  Source file for call
    size_t line                         //  Line number for call
)
{


    asl_field_t *
        item = *item_p;                 //  Dereferenced item
    asl_field_t *
        next = NULL;                    //  The item to return
    asl_field_list_t *
        self = (asl_field_list_t *) item->list_head;  //  The list



if (self
&&  self == item->list_head) { //       Catch case where item moved
    next = (asl_field_t *) item->list_next;
    if (next == &self->asl_field)
        next = NULL;
    else
        asl_field_link (next);

}
asl_field_unlink (item_p);




    return (next);
}
/*  -------------------------------------------------------------------------
    asl_field_list_prev

    Type: Component method
    Returns the preceding item on the asl_field list. If the provided item
    is null, returns the last item.  If there were no (more) items, returns
    null.
    -------------------------------------------------------------------------
 */

asl_field_t *
    asl_field_list_prev_ (
    asl_field_t * ( * item_p ),         //  Not documented
    char * file,                        //  Source file for call
    size_t line                         //  Line number for call
)
{


    asl_field_t *
        item = *item_p;                 //  Dereferenced item
    asl_field_t *
        prev = NULL;                    //  The item to return
    asl_field_list_t *
        self = (asl_field_list_t *) item->list_head;  //  The list



if (self
&&  self == item->list_head) { //       Catch case where item moved
    prev = (asl_field_t *) item->list_prev;
    if (prev == &self->asl_field)
        prev = NULL;
    else
        asl_field_link (prev);

}
asl_field_unlink (item_p);




    return (prev);
}
/*  -------------------------------------------------------------------------
    asl_field_list_rehook

    Type: Component method
    -------------------------------------------------------------------------
 */

static inline void
    asl_field_list_rehook (
    asl_field_t * item,                 //  Not documented
    asl_field_t * left,                 //  Not documented
    asl_field_t * right                 //  Not documented
)
{
asl_field_t
    *swap;

swap = (asl_field_t *) left->list_next;
left->list_next = item->list_next;
item->list_next = swap;

swap = (asl_field_t *) item->list_prev;
item->list_prev = right->list_prev;
right->list_prev = swap;

item->list_head = left->list_head;
}
/*  -------------------------------------------------------------------------
    asl_field_list_unhook

    Type: Component method
    -------------------------------------------------------------------------
 */

static inline asl_field_t *
    asl_field_list_unhook (
    asl_field_t * item                  //  Not documented
)
{

asl_field_list_rehook (item, (asl_field_t *) item->list_prev, (asl_field_t *) item->list_next);
item->list_head = NULL;

    return (item);
}
/*  -------------------------------------------------------------------------
    asl_field_list_terminate

    Type: Component method
    -------------------------------------------------------------------------
 */

void
    asl_field_list_terminate (
void)
{


}
/*  -------------------------------------------------------------------------
    asl_field_list_show

    Type: Component method
    -------------------------------------------------------------------------
 */

void
    asl_field_list_show_ (
    void * item,                        //  The opaque pointer
    int opcode,                         //  The callback opcode
    FILE * trace_file,                  //  File to print to
    char * file,                        //  Source file
    size_t line                         //  Line number
)
{
    asl_field_list_t
        *self;
    int
        container_links;
#if defined (DEBUG) || defined (BASE_HISTORY) || defined (BASE_HISTORY_ASL_FIELD_LIST)
    qbyte
        history_index;
#endif


self = item;
container_links = 0;
    assert (opcode == ICL_CALLBACK_DUMP);

    fprintf (trace_file, "    <asl_field_list zombie = \"%u\" links = \"%u\" containers = \"%u\" file = \"%s\" line = \"%lu\"  pointer = \"%p\" />\n", self->zombie, self->links, container_links, file, (unsigned long) line, self);
#if defined (DEBUG) || defined (BASE_HISTORY) || defined (BASE_HISTORY_ASL_FIELD_LIST)
    if (self->history_last > ASL_FIELD_LIST_HISTORY_LENGTH) {
        fprintf (trace_file, "        <!-- possess history too large (%d) - call iMatix-tech -->\n",
            self->history_last);
        history_index = (self->history_last + 1) % ASL_FIELD_LIST_HISTORY_LENGTH;
        self->history_last %= ASL_FIELD_LIST_HISTORY_LENGTH;
    }
    else
        history_index = 0;

    for (; history_index != self->history_last; history_index = (history_index + 1) % ASL_FIELD_LIST_HISTORY_LENGTH) {
        fprintf (trace_file, "       <%s file = \"%s\" line = \"%lu\" links = \"%lu\" />\n",
            self->history_type [history_index],
            self->history_file [history_index],
            (unsigned long) self->history_line  [history_index],
            (unsigned long) self->history_links [history_index]);
    }
    fprintf (trace_file, "    </asl_field_list>\n");
#endif

}
/*  -------------------------------------------------------------------------
    asl_field_list_destroy

    Type: Component method
    Destroys a asl_field_list_t object. Takes the address of a
    asl_field_list_t reference (a pointer to a pointer) and nullifies the
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
    asl_field_list_destroy_ (
    asl_field_list_t * ( * self_p ),    //  Reference to object reference
    char * file,                        //  Source fileSource file
    size_t line                         //  Line numberLine number
)
{
    asl_field_list_t *
        self = *self_p;                 //  Dereferenced Reference to object reference

if (self) {
    if (icl_atomic_cas32 (&self->zombie, TRUE, FALSE) == FALSE)
        asl_field_list_annihilate_ (self_p, file, line);

    if (self->links == 0) {
        icl_system_panic ("asl_field_list", "E: missing link on asl_field_list object");
        asl_field_list_show_ (self, ICL_CALLBACK_DUMP, stderr, file, line);
        abort ();
    }

    if (icl_atomic_dec32 ((volatile qbyte *) &self->links) == 0)
        asl_field_list_free_ ((asl_field_list_t *) self, file, line);
    *self_p = NULL;
}
}
/*  -------------------------------------------------------------------------
    asl_field_list_alloc

    Type: Component method
    -------------------------------------------------------------------------
 */

static asl_field_list_t *
    asl_field_list_alloc_ (
    char * file,                        //  Source file for call
    size_t line                         //  Line number for call
)
{

    asl_field_list_t *
        self = NULL;                    //  Object reference

self = (asl_field_list_t *) icl_mem_alloc_ (sizeof (asl_field_list_t), file, line);
if (self)
    memset (self, 0, sizeof (asl_field_list_t));



    return (self);
}
/*  -------------------------------------------------------------------------
    asl_field_list_free

    Type: Component method
    Freess a asl_field_list_t object.
    -------------------------------------------------------------------------
 */

static void
    asl_field_list_free_ (
    asl_field_list_t * self,            //  Object reference
    char * file,                        //  Source file
    size_t line                         //  Line number
)
{
#if defined (DEBUG) || defined (BASE_HISTORY) || defined (BASE_HISTORY_ASL_FIELD_LIST)
    int
        history_last;
#endif


    if (self) {
#if defined (DEBUG) || defined (BASE_HISTORY) || defined (BASE_HISTORY_ASL_FIELD_LIST)
        //  Track possession operation in history
        history_last = icl_atomic_inc32 ((volatile qbyte *) &self->history_last) + 1;
        self->history_file  [history_last % ASL_FIELD_LIST_HISTORY_LENGTH] = file;
        self->history_line  [history_last % ASL_FIELD_LIST_HISTORY_LENGTH] = line;
        self->history_type  [history_last % ASL_FIELD_LIST_HISTORY_LENGTH] = "free";
        self->history_links [history_last % ASL_FIELD_LIST_HISTORY_LENGTH] = self->links;
#endif

        memset (&self->object_tag, 0, sizeof (asl_field_list_t) - ((byte *) &self->object_tag - (byte *) self));
//        memset (self, 0, sizeof (asl_field_list_t));
        self->object_tag = ASL_FIELD_LIST_DEAD;
        icl_mem_free (self);
    }
    self = NULL;
}
/*  -------------------------------------------------------------------------
    asl_field_list_link

    Type: Component method
    Adds a link (reference count) to an object.

    If the object has been zombified (ie destroyed while extra links were present),
    this method returns NULL and does not add any links.

    This method does not lock the object.
    -------------------------------------------------------------------------
 */

asl_field_list_t *
    asl_field_list_link_ (
    asl_field_list_t * self,            //  Not documented
    char * file,                        //  Source file for call
    size_t line                         //  Line number for call
)
{
#if defined (DEBUG) || defined (BASE_HISTORY) || defined (BASE_HISTORY_ASL_FIELD_LIST)
    int
        history_last;
#endif

    if (self) {
        ASL_FIELD_LIST_ASSERT_SANE (self);
        if (file)
            icl_mem_possess_ (self, file, line);
        icl_atomic_inc32 ((volatile qbyte *) &self->links);
#if defined (DEBUG) || defined (BASE_HISTORY) || defined (BASE_HISTORY_ASL_FIELD_LIST)
        //  Track possession operation in history
        history_last = icl_atomic_inc32 ((volatile qbyte *) &self->history_last) + 1;
        self->history_file  [history_last % ASL_FIELD_LIST_HISTORY_LENGTH] = file;
        self->history_line  [history_last % ASL_FIELD_LIST_HISTORY_LENGTH] = line;
        self->history_type  [history_last % ASL_FIELD_LIST_HISTORY_LENGTH] = "link";
        self->history_links [history_last % ASL_FIELD_LIST_HISTORY_LENGTH] = self->links;
#endif
    }

    return (self);
}
/*  -------------------------------------------------------------------------
    asl_field_list_unlink

    Type: Component method
    Removes a link (reference count) to an object.  Sets the object pointer to NULL
    to indicate that it is no longer valid.
    -------------------------------------------------------------------------
 */

void
    asl_field_list_unlink_ (
    asl_field_list_t * ( * self_p ),    //  Reference to object reference
    char * file,                        //  Source file for call
    size_t line                         //  Line number for call
)
{
#if defined (DEBUG) || defined (BASE_HISTORY) || defined (BASE_HISTORY_ASL_FIELD_LIST)
    int
        history_last;
#endif

    asl_field_list_t *
        self = *self_p;                 //  Dereferenced Reference to object reference

    if (self) {
        ASL_FIELD_LIST_ASSERT_SANE (self);
        if (self->links == 0) {
            icl_system_panic ("asl_field_list", "E: missing link on asl_field_list object");
            asl_field_list_show_ (self, ICL_CALLBACK_DUMP, stderr, file, line);
            abort ();
        }

#if defined (DEBUG) || defined (BASE_HISTORY) || defined (BASE_HISTORY_ASL_FIELD_LIST)
        //  Track possession operation in history.  Pre-empt value of links
        //  after operation; otherwise race condition can result in writing
        //  to freed memory.
        history_last = icl_atomic_inc32 ((volatile qbyte *) &self->history_last) + 1;
        self->history_file  [history_last % ASL_FIELD_LIST_HISTORY_LENGTH] = file;
        self->history_line  [history_last % ASL_FIELD_LIST_HISTORY_LENGTH] = line;
        self->history_type  [history_last % ASL_FIELD_LIST_HISTORY_LENGTH] = "unlink";
        self->history_links [history_last % ASL_FIELD_LIST_HISTORY_LENGTH] = self->links - 1;
#endif
        if (icl_atomic_dec32 ((volatile qbyte *) &self->links) == 0) {

        if (icl_atomic_cas32 (&self->zombie, TRUE, FALSE) == FALSE)
            asl_field_list_annihilate_ (self_p, file, line);
        asl_field_list_free_ ((asl_field_list_t *) self, file, line);
    }
    *self_p = NULL;
}
}
/*  -------------------------------------------------------------------------
    asl_field_list_show_animation

    Type: Component method
    Enables animation of the component. Animation is sent to stdout.
    To enable animation you must generate using the option -animate:1.
    -------------------------------------------------------------------------
 */

void
    asl_field_list_show_animation (
    Bool enabled                        //  Are we enabling or disabling animation?
)
{

asl_field_list_animating = enabled;
}
/*  -------------------------------------------------------------------------
    asl_field_list_new_in_scope

    Type: Component method
    -------------------------------------------------------------------------
 */

void
    asl_field_list_new_in_scope_ (
    asl_field_list_t * * self_p,        //  Not documented
    icl_scope_t * _scope,               //  Not documented
    char * file,                        //  Source file for call
    size_t line,                        //  Line number for call
    icl_longstr_t * field_table         //  Field table
)
{
    icl_destroy_t *
        _destroy;                       //  Not documented

*self_p = asl_field_list_new_ (file,line,field_table);

if (*self_p) {
    _destroy = icl_destroy_new   ((void * *) self_p, (icl_destructor_fn *) asl_field_list_destroy_);
    icl_destroy_list_queue (_scope, _destroy);
    icl_destroy_unlink (&_destroy);
}
}
//  Embed the version information in the resulting binary

char *EMBED__asl_field_list_version_start  = "VeRsIoNsTaRt:ipc";
char *EMBED__asl_field_list_component     = "asl_field_list ";
char *EMBED__asl_field_list_version       = "1.1 ";
char *EMBED__asl_field_list_copyright     = "Copyright (c) 1996-2009 iMatix Corporation";
char *EMBED__asl_field_list_filename      = "asl_field_list.icl ";
char *EMBED__asl_field_list_builddate     = "2011/03/01 ";
char *EMBED__asl_field_list_version_end   = "VeRsIoNeNd:ipc";

