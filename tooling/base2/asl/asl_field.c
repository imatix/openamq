/*---------------------------------------------------------------------------
    asl_field.c - asl_field component

    Provides functionality to implement the ASL RFC 'Field Table'
    data type.  This class provides functions at a per-field level.
    Generated from asl_field.icl by icl_gen using GSL/4.
    
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
#include "asl_field.h"                  //  Definitions for our class

//  Shorthand for class type

#define self_t              asl_field_t

//  Shorthands for class methods

#define self_new            asl_field_new
#define self_annihilate     asl_field_annihilate
#define self_new_string     asl_field_new_string
#define self_new_longstr    asl_field_new_longstr
#define self_new_integer    asl_field_new_integer
#define self_new_decimal    asl_field_new_decimal
#define self_new_time       asl_field_new_time
#define self_new_field_table  asl_field_new_field_table
#define self_new_void       asl_field_new_void
#define self_assume         asl_field_assume
#define self_parse          asl_field_parse
#define self_string         asl_field_string
#define self_integer        asl_field_integer
#define self_field_table    asl_field_field_table
#define self_flatten        asl_field_flatten
#define self_flat_size      asl_field_flat_size
#define self_print          asl_field_print
#define self_selftest       asl_field_selftest
#define self_remove_from_all_containers  asl_field_remove_from_all_containers
#define self_show           asl_field_show
#define self_terminate      asl_field_terminate
#define self_destroy        asl_field_destroy
#define self_alloc          asl_field_alloc
#define self_free           asl_field_free
#define self_link           asl_field_link
#define self_unlink         asl_field_unlink
#define self_cache_initialise  asl_field_cache_initialise
#define self_cache_purge    asl_field_cache_purge
#define self_cache_terminate  asl_field_cache_terminate
#define self_show_animation  asl_field_show_animation
#define self_new_in_scope   asl_field_new_in_scope

#define asl_field_annihilate(self)      asl_field_annihilate_ (self, __FILE__, __LINE__)
static void
    asl_field_annihilate_ (
asl_field_t * ( * self_p ),             //  Reference to object reference
char * file,                            //  Source file
size_t line                             //  Line number
);

#define asl_field_alloc()               asl_field_alloc_ (__FILE__, __LINE__)
static asl_field_t *
    asl_field_alloc_ (
char * file,                            //  Source file for call
size_t line                             //  Line number for call
);

#define asl_field_free(self)            asl_field_free_ (self, __FILE__, __LINE__)
static void
    asl_field_free_ (
asl_field_t * self,                     //  Object reference
char * file,                            //  Source file
size_t line                             //  Line number
);

static void
    asl_field_cache_initialise (
void);

static void
    asl_field_cache_purge (
void);

static void
    asl_field_cache_terminate (
void);

Bool
    asl_field_animating = TRUE;         //  Animation enabled by default
static icl_cache_t
    *s_cache = NULL;


//  Network byte ordering                                                      

#define ASL_FIELD_PUT_LONG(net, host) \
    (net) [0] = (byte) (((host) >> 24) & 255); \
    (net) [1] = (byte) (((host) >> 16) & 255); \
    (net) [2] = (byte) (((host) >> 8)  & 255); \
    (net) [3] = (byte) (((host))       & 255); \
    (net) += 4
#define ASL_FIELD_GET_LONG(host, net) \
    (host) = ((net) [0] << 24) \
           + ((net) [1] << 16) \
           + ((net) [2] << 8) \
           +  (net) [3]; \
    (net)  += 4
/*  -------------------------------------------------------------------------
    asl_field_new

    Type: Component method
    Creates and initialises a new asl_field_t object, returns a
    reference to the created object.
    Initialises the new item to be a valid list item.
    -------------------------------------------------------------------------
 */

asl_field_t *
    asl_field_new_ (
    char * file,                        //  Source file for call
    size_t line                         //  Line number for call
)
{
    asl_field_t *
        self = NULL;                    //  Object reference

    self = asl_field_alloc_ (file, line);
    if (self) {
        self->object_tag   = ASL_FIELD_ALIVE;
        self->links        = 1;
        self->zombie       = FALSE;
#if defined (DEBUG) || defined (BASE_HISTORY) || defined (BASE_HISTORY_ASL_FIELD)
        self->history_last = 0;

        //  Track possession operation in history
        self->history_file  [0] = file;
        self->history_line  [0] = line;
        self->history_type  [0] = "new";
        self->history_links [0] = self->links;
#endif
#if defined (DEBUG)
        icl_mem_set_callback (self, asl_field_show_);
#endif

self->list_next = self;
self->list_prev = self;
self->list_head = NULL;
strclr (self->name);
self->type     = 0;
self->decimals = 0;
self->integer  = 0;
self->string   = NULL;
}

    return (self);
}
/*  -------------------------------------------------------------------------
    asl_field_annihilate

    Type: Component method
    -------------------------------------------------------------------------
 */

static void
    asl_field_annihilate_ (
    asl_field_t * ( * self_p ),         //  Reference to object reference
    char * file,                        //  Source file
    size_t line                         //  Line number
)
{
#if defined (DEBUG) || defined (BASE_HISTORY) || defined (BASE_HISTORY_ASL_FIELD)
    int
        history_last;
#endif

    asl_field_t *
        self = *self_p;                 //  Dereferenced Reference to object reference

#if defined (DEBUG) || defined (BASE_HISTORY) || defined (BASE_HISTORY_ASL_FIELD)
    //  Track possession operation in history
    history_last = icl_atomic_inc32 ((volatile qbyte *) &self->history_last) + 1;
    self->history_file  [history_last % ASL_FIELD_HISTORY_LENGTH] = file;
    self->history_line  [history_last % ASL_FIELD_HISTORY_LENGTH] = line;
    self->history_type  [history_last % ASL_FIELD_HISTORY_LENGTH] = "destroy";
    self->history_links [history_last % ASL_FIELD_HISTORY_LENGTH] = self->links;
#endif

    ASL_FIELD_ASSERT_SANE (self);
    asl_field_remove_from_all_containers (self);

icl_longstr_destroy (&self->string);

}
/*  -------------------------------------------------------------------------
    asl_field_new_string

    Type: Component method
    -------------------------------------------------------------------------
 */

void
    asl_field_new_string_ (
    char * file,                        //  Source file for call
    size_t line,                        //  Line number for call
    asl_field_list_t * list,            //  Parent field list
    char * name,                        //  Field name
    char * value                        //  Field string value
)
{
asl_field_t
    *self;

//
assert (strlen (name) <= ASL_FIELD_NAME_MAX);

self = asl_field_new_ (file, line);
strcpy (self->name, name);
self->type   = ASL_FIELD_TYPE_STRING;
self->string = icl_longstr_new (value, strlen (value) + 1);
self->string->data [strlen (value)] = '\0';
asl_field_list_queue (list, self);
asl_field_unlink (&self);
}
/*  -------------------------------------------------------------------------
    asl_field_new_longstr

    Type: Component method
    -------------------------------------------------------------------------
 */

void
    asl_field_new_longstr_ (
    char * file,                        //  Source file for call
    size_t line,                        //  Line number for call
    asl_field_list_t * list,            //  Parent field list
    char * name,                        //  Field name
    icl_longstr_t * value               //  Field string value
)
{
asl_field_t
    *self;

//
assert (strlen (name) <= ASL_FIELD_NAME_MAX);

self = asl_field_new_ (file, line);
strcpy (self->name, name);
self->type   = ASL_FIELD_TYPE_STRING;
self->string = icl_longstr_dup (value);
asl_field_list_queue (list, self);
asl_field_unlink (&self);
}
/*  -------------------------------------------------------------------------
    asl_field_new_integer

    Type: Component method
    -------------------------------------------------------------------------
 */

void
    asl_field_new_integer_ (
    char * file,                        //  Source file for call
    size_t line,                        //  Line number for call
    asl_field_list_t * list,            //  Parent field list
    char * name,                        //  Field name
    int value                           //  Field integer value
)
{
asl_field_t
    *self;

//
assert (strlen (name) <= ASL_FIELD_NAME_MAX);

self = asl_field_new_ (file, line);
strcpy (self->name, name);
self->type    = ASL_FIELD_TYPE_INTEGER;
self->integer = value;
asl_field_list_queue (list, self);
asl_field_unlink (&self);
}
/*  -------------------------------------------------------------------------
    asl_field_new_decimal

    Type: Component method
    -------------------------------------------------------------------------
 */

void
    asl_field_new_decimal_ (
    char * file,                        //  Source file for call
    size_t line,                        //  Line number for call
    asl_field_list_t * list,            //  Parent field list
    char * name,                        //  Field name
    long value,                         //  Field decimal value
    int decimals                        //  Number of decimal places
)
{
asl_field_t
    *self;

//
assert (strlen (name) <= ASL_FIELD_NAME_MAX);

self = asl_field_new_ (file, line);
strcpy (self->name, name);
self->type     = ASL_FIELD_TYPE_DECIMAL;
self->integer  = value;
self->decimals = decimals;
asl_field_list_queue (list, self);
asl_field_unlink (&self);
}
/*  -------------------------------------------------------------------------
    asl_field_new_time

    Type: Component method
    -------------------------------------------------------------------------
 */

void
    asl_field_new_time_ (
    char * file,                        //  Source file for call
    size_t line,                        //  Line number for call
    asl_field_list_t * list,            //  Parent field list
    char * name,                        //  Field name
    time_t value                        //  Field time value
)
{
asl_field_t
    *self;

//
assert (strlen (name) <= ASL_FIELD_NAME_MAX);

self = asl_field_new_ (file, line);
strcpy (self->name, name);
self->type    = ASL_FIELD_TYPE_TIME;
self->integer = value;
asl_field_list_queue (list, self);
asl_field_unlink (&self);
}
/*  -------------------------------------------------------------------------
    asl_field_new_field_table

    Type: Component method
    -------------------------------------------------------------------------
 */

void
    asl_field_new_field_table_ (
    char * file,                        //  Source file for call
    size_t line,                        //  Line number for call
    asl_field_list_t * list,            //  Parent field list
    char * name,                        //  Field name
    asl_field_list_t * value            //  The value
)
{
asl_field_t
    *self;
icl_longstr_t
    *flattened;

//
assert (strlen (name) <= ASL_FIELD_NAME_MAX);

flattened = asl_field_list_flatten (value);
self = asl_field_new_ (file, line);
strcpy (self->name, name);
self->type   = ASL_FIELD_TYPE_FIELD_TABLE;
self->string = icl_longstr_dup (flattened);
asl_field_list_queue (list, self);
asl_field_unlink (&self);
icl_longstr_destroy (&flattened);
}
/*  -------------------------------------------------------------------------
    asl_field_new_void

    Type: Component method
    -------------------------------------------------------------------------
 */

void
    asl_field_new_void_ (
    char * file,                        //  Source file for call
    size_t line,                        //  Line number for call
    asl_field_list_t * list,            //  Parent field list
    char * name                         //  Field name
)
{
asl_field_t
    *self;

//
assert (strlen (name) <= ASL_FIELD_NAME_MAX);

self = asl_field_new_ (file, line);
strcpy (self->name, name);
self->type = ASL_FIELD_TYPE_VOID;
asl_field_list_queue (list, self);
asl_field_unlink (&self);
}
/*  -------------------------------------------------------------------------
    asl_field_assume

    Type: Component method
    Looks for a field with the specified name, if not found, creates a
    new field with the specified value, else updates the field's value.
    -------------------------------------------------------------------------
 */

void
    asl_field_assume (
    asl_field_list_t * list,            //  Parent field list
    char * name,                        //  Field name
    char * value                        //  Field string value
)
{
asl_field_t
    *field;

//
field = asl_field_list_search (list, name);
if (field)
    self_destroy (&field);
asl_field_new_string (list, name, value);
}
/*  -------------------------------------------------------------------------
    asl_field_parse

    Type: Component method
    Parses a field definition. Returns the address of the first octet after
    the field definition, or NULL if there was an error parsing the field.
    The limit points to the octet following the last legal octet of the string.
    -------------------------------------------------------------------------
 */

byte *
    asl_field_parse (
    asl_field_list_t * list,            //  Parent field list
    byte * input,                       //  Field definition structure
    byte * limit                        //  End of field-table string
)
{
asl_field_t
    *self;
size_t
    string_size;
    byte *
        field_end;                      //  Calculated end position

//
assert (list);
self = self_new ();

//  Copy field name                                                        
string_size = *input;
input += 1;
memcpy (self->name, input, string_size);
self->name [string_size] = 0;
input += string_size;

assert (input <= limit);
self->type = *input;
input += 1;

if (self->type == ASL_FIELD_TYPE_STRING) {
    ASL_FIELD_GET_LONG (string_size, input);

    //  Ensure that the longstr->data is a valid C string
    self->string = icl_longstr_new (NULL, string_size + 1);
    memcpy (self->string->data, input, string_size);
    self->string->data [string_size] = '\0';
    self->string->cur_size = string_size;
    input += string_size;
    field_end = input;
}
else
if (self->type == ASL_FIELD_TYPE_INTEGER) {
    ASL_FIELD_GET_LONG (self->integer, input);
    field_end = input;
}
else
if (self->type == ASL_FIELD_TYPE_DECIMAL) {
    self->decimals = *input;
    input += 1;
    ASL_FIELD_GET_LONG (self->integer, input);
    field_end = input;
}
else
if (self->type == ASL_FIELD_TYPE_TIME) {
    ASL_FIELD_GET_LONG (self->integer, input);
    field_end = input;
}
else
if (self->type == ASL_FIELD_TYPE_FIELD_TABLE) {
    ASL_FIELD_GET_LONG (string_size, input);

    //  Ensure that the longstr->data is a valid C string
    self->string = icl_longstr_new (NULL, string_size);
    memcpy (self->string->data, input, string_size);
    self->string->cur_size = string_size;
    input += string_size;
    field_end = input;
}
else
if (self->type == ASL_FIELD_TYPE_VOID) {
    field_end = input;
}
else {
    icl_console_print ("asl_field: invalid field type - '%c'", *input);
    field_end = NULL;
}
if (field_end == NULL || input > limit) {
    icl_console_print ("asl_field: field definition badly formed - rejected");
    field_end = NULL;
    self_destroy (&self);
}
else
    asl_field_list_queue (list, self);

asl_field_unlink (&self);

    return (field_end);
}
/*  -------------------------------------------------------------------------
    asl_field_string

    Type: Component method
    Returns string value for field.  If the field is not a string, will
    attempt to convert the field value into a string. The returned string
    is always be null-terminated.  NULLs in the string will be replaced
    by binary 255 - this will modify binary string values.
    -------------------------------------------------------------------------
 */

char *
    asl_field_string (
    asl_field_t * self                  //  Reference to object
)
{
    char *
        value;                          //  Returned string value

//
ASL_FIELD_ASSERT_SANE (self);
if (self->type == ASL_FIELD_TYPE_STRING) {
    //  Suppress nulls in string value
    int
        index = 0;

    for (index = 0; index < (int) self->string->cur_size - 1; index++)
        if (self->string->data [index] == 0)
            self->string->data [index] = 255;
}
else {
    //  Add a string value and format the field accordingly
    if (!self->string)
        self->string = icl_longstr_new (NULL, 30);

    if (self->type == ASL_FIELD_TYPE_INTEGER)
        icl_longstr_fmt (self->string, "%d", (long) self->integer);
    else
    if (self->type == ASL_FIELD_TYPE_DECIMAL)
        icl_longstr_fmt (self->string, "%g",
            (double) self->integer / exp (self->decimals * log (10.0)));
    else
    if (self->type == ASL_FIELD_TYPE_TIME) {
        struct tm
            *time_struct;
        time_struct = gmtime ((time_t *) &self->integer);
        icl_longstr_fmt (self->string, "%4d/%02d/%02dT%02d:%02d:%02dZ",
            self->name,
            time_struct->tm_year + 1900,
            time_struct->tm_mon + 1,
            time_struct->tm_mday,
            time_struct->tm_hour,
            time_struct->tm_min,
            time_struct->tm_sec);
    }
    else
    if (self->type == ASL_FIELD_TYPE_FIELD_TABLE)
        icl_longstr_fmt (self->string, "(table)");
    else
    if (self->type == ASL_FIELD_TYPE_VOID)
        icl_longstr_fmt (self->string, "(void)");
}
value = (char *) self->string->data;

    return (value);
}
/*  -------------------------------------------------------------------------
    asl_field_integer

    Type: Component method
    Returns integer value for field.  If the field is not an integer, will
    attempt to convert the field into an integer.
    -------------------------------------------------------------------------
 */

long
    asl_field_integer (
    asl_field_t * self                  //  Reference to object
)
{
    long
        value;                          //  Returned integer value

//
ASL_FIELD_ASSERT_SANE (self);

value = 0;
if (self->type == ASL_FIELD_TYPE_STRING)
    value = atol ((char *) self->string->data);
else
if (self->type == ASL_FIELD_TYPE_INTEGER)
    value = (long) self->integer;
else
if (self->type == ASL_FIELD_TYPE_DECIMAL)
    value = (long) (self->integer / exp (self->decimals * log (10.0)));
else
if (self->type == ASL_FIELD_TYPE_TIME)
    value = (long) self->integer;
else
    value = 0;

    return (value);
}
/*  -------------------------------------------------------------------------
    asl_field_field_table

    Type: Component method
    Returns field table stored in the field. If field value is not a field
    table it returns NULL.
    -------------------------------------------------------------------------
 */

asl_field_list_t*
    asl_field_field_table (
    asl_field_t * self                  //  Reference to object
)
{
    asl_field_list_t*
        value;                          //  Returned field table

//
ASL_FIELD_ASSERT_SANE (self);

if (self->type == ASL_FIELD_TYPE_FIELD_TABLE)
    value = asl_field_list_new (self->string);
else
    value = NULL;

    return (value);
}
/*  -------------------------------------------------------------------------
    asl_field_flatten

    Type: Component method
    Accepts a asl_field_t reference and returns zero in case of success,
    1 in case of errors.
    Creates a field definition string from the field. Writes the
    field definition to the provided string.  Returns -1 if the field
    was invalid or there was not enough space to write the field.
    -------------------------------------------------------------------------
 */

int
    asl_field_flatten (
    asl_field_t * self,                 //  Reference to object
    icl_longstr_t * string              //  String to write to
)
{
byte
    *output;
size_t
    name_length;
    int
        rc = 0;                         //  Return code

ASL_FIELD_ASSERT_SANE (self);
if (!self->zombie) {

//
if (string->cur_size + self_flat_size (self) <= string->max_size) {
    //  Set-up pointer to current writing position in string               
    output = string->data + string->cur_size;

    //  Write field name                                                   
    name_length = strlen (self->name);
    *output = (byte) name_length;
    output += 1;
    memcpy (output, self->name, name_length);
    output += name_length;

    //  Write field type                                                   
    *(char *) output = self->type;
    output += 1;

    if (self->type == ASL_FIELD_TYPE_STRING) {
        ASL_FIELD_PUT_LONG (output, self->string->cur_size);
        memcpy (output, self->string->data, self->string->cur_size);
        output += self->string->cur_size;
    }
    else
    if (self->type == ASL_FIELD_TYPE_INTEGER) {
        ASL_FIELD_PUT_LONG (output, self->integer);
    }
    else
    if (self->type == ASL_FIELD_TYPE_DECIMAL) {
        *output = self->decimals;
        output += 1;
        ASL_FIELD_PUT_LONG (output, self->integer);
    }
    else
    if (self->type == ASL_FIELD_TYPE_TIME) {
        ASL_FIELD_PUT_LONG (output, self->integer);
    }
    else
    if (self->type == ASL_FIELD_TYPE_FIELD_TABLE) {
        ASL_FIELD_PUT_LONG (output, self->string->cur_size);
        memcpy (output, self->string->data, self->string->cur_size);
        output += self->string->cur_size;
    }
    else
    if (self->type == ASL_FIELD_TYPE_VOID) {
        //  Do nothing
    }
    else
        rc = -1;

    if (rc == 0)
        string->cur_size = output - string->data;
}
else
    rc = -1;
}
else
    rc = -1;                        //  Return error on zombie object.


    return (rc);
}
/*  -------------------------------------------------------------------------
    asl_field_flat_size

    Type: Component method
    Accepts a asl_field_t reference and returns zero in case of success,
    1 in case of errors.
    Returns the flattened size of the field, in octets.
    -------------------------------------------------------------------------
 */

int
    asl_field_flat_size (
    asl_field_t * self                  //  Reference to object
)
{
    int
        rc = 0;                         //  Return code

ASL_FIELD_ASSERT_SANE (self);
if (!self->zombie) {

//
//  All fields have a length-specified name and a 1-octet type             
rc = strlen (self->name) + 2;
if (self->type == ASL_FIELD_TYPE_STRING)
    rc += self->string->cur_size + ASL_LSTR_HSIZE;
else
if (self->type == ASL_FIELD_TYPE_INTEGER)
    rc += 4;
else
if (self->type == ASL_FIELD_TYPE_DECIMAL)
    rc += 4 + 1;
else
if (self->type == ASL_FIELD_TYPE_TIME)
    rc += 4;
else
if (self->type == ASL_FIELD_TYPE_FIELD_TABLE)
    rc += self->string->cur_size + ASL_LSTR_HSIZE;
else
if (self->type == ASL_FIELD_TYPE_VOID)
    rc += 0;
}
else
    rc = -1;                        //  Return error on zombie object.


    return (rc);
}
/*  -------------------------------------------------------------------------
    asl_field_print

    Type: Component method
    Accepts a asl_field_t reference and returns zero in case of success,
    1 in case of errors.
    Prints the field format and contents to output (using icl_console_print).
    -------------------------------------------------------------------------
 */

int
    asl_field_print (
    asl_field_t * self                  //  Reference to object
)
{
    int
        rc = 0;                         //  Return code

ASL_FIELD_ASSERT_SANE (self);
if (!self->zombie) {

//
if (self->type == ASL_FIELD_TYPE_STRING) {
    icl_shortstr_t
        string_value;
    icl_shortstr_cpy (string_value, self_string (self));
    if (self->string->cur_size > 20) {
        string_value [20] = 0;
        icl_shortstr_cat (string_value, "...");
    }
    icl_console_print (" - %s - string, length=%d content=%s", 
        self->name, self->string->cur_size, string_value);
}
else
if (self->type == ASL_FIELD_TYPE_INTEGER)
    icl_console_print (" - %s - integer, value=%d", self->name, (int) self->integer);
else
if (self->type == ASL_FIELD_TYPE_DECIMAL)
    icl_console_print (" - %s - decimal, value=%g", self->name,
        (double) self->integer / exp (self->decimals * log (10.0)));
else
if (self->type == ASL_FIELD_TYPE_TIME) {
    struct tm
        *time_struct;
    time_struct = gmtime ((time_t *) &self->integer);
    icl_console_print (" - %s - time, value=%4d/%02d/%02dT%02d:%02d:%02dZ",
        self->name,
        time_struct->tm_year + 1900,
        time_struct->tm_mon + 1,
        time_struct->tm_mday,
        time_struct->tm_hour,
        time_struct->tm_min,
        time_struct->tm_sec);
}
else
if (self->type == ASL_FIELD_TYPE_FIELD_TABLE)
    icl_console_print (" - %s - field table", self->name);
else
if (self->type == ASL_FIELD_TYPE_VOID)
    icl_console_print (" - %s - void", self->name);
else
    icl_console_print (" - undefined field type");
}
else
    rc = -1;                        //  Return error on zombie object.


    return (rc);
}
/*  -------------------------------------------------------------------------
    asl_field_selftest

    Type: Component method
    -------------------------------------------------------------------------
 */

void
    asl_field_selftest (
void)
{
asl_field_list_t
    *list,                          //  List of fields
    *embedded_list;                 //  List embedded in the top list
asl_field_t
    *field,                         //  One field in list
    *embedded_field;                //  Field in the embedded list
icl_longstr_t
    *string;                        //  Serialised field list

//
//  We create a new empty list by passing it a null string
//
list = asl_field_list_new (NULL);

//  To add fields to the list, we use these methods, which
//  do not return any values.
//
asl_field_new_string  (list, "string",  "ring a ding ding");
asl_field_new_integer (list, "integer", 123456789);
asl_field_new_decimal (list, "decimal", 123456789, 3);
asl_field_new_time    (list, "time",    123456789);
asl_field_new_void    (list, "void");
embedded_list = asl_field_list_new (NULL);
asl_field_new_string  (embedded_list, "string",  "hello world");
asl_field_new_integer (embedded_list, "integer", 654321);
asl_field_new_field_table (list, "table", embedded_list);
asl_field_list_unlink (&embedded_list);

//  When we want to send a field list, we flatten it into a
//  string, which the list_flatten method allocates for us.
//
string = asl_field_list_flatten (list);
asl_field_list_destroy (&list);

//  Let's recreate the list and check that it's correct
list = asl_field_list_new (string);

field = asl_field_list_search (list, "string");
assert (streq (asl_field_string (field), "ring a ding ding"));
asl_field_unlink (&field);

field = asl_field_list_search (list, "integer");
assert (asl_field_integer (field) == 123456789);
assert (streq (asl_field_string (field), "123456789"));
asl_field_unlink (&field);

field = asl_field_list_search (list, "decimal");
assert (asl_field_integer (field) == 123456);
assert (streq (asl_field_string (field), "123457"));
asl_field_unlink (&field);

field = asl_field_list_search (list, "time");
assert (asl_field_integer (field) == 123456789);
asl_field_unlink (&field);

field = asl_field_list_search (list, "void");
assert (asl_field_integer (field) == 0);
asl_field_unlink (&field);

field = asl_field_list_search (list, "table");
embedded_list = asl_field_field_table (field);
embedded_field = asl_field_list_search (embedded_list, "string");
assert (streq (asl_field_string (embedded_field), "hello world"));
asl_field_unlink (&embedded_field);
embedded_field = asl_field_list_search (embedded_list, "integer");
assert (asl_field_integer (embedded_field) == 654321);
asl_field_unlink (&embedded_field);
asl_field_list_unlink (&embedded_list);
asl_field_unlink (&field);

//  We must destroy the flat string when finished with it
asl_field_list_destroy (&list);
icl_longstr_destroy (&string);
}
/*  -------------------------------------------------------------------------
    asl_field_remove_from_all_containers

    Type: Component method
    The method to call to remove an item from its container.  Is called by
    the 'destroy' method if the possession count hits zero.
    -------------------------------------------------------------------------
 */

void
    asl_field_remove_from_all_containers (
    asl_field_t * self                  //  The item
)
{

asl_field_list_remove (self);
}
/*  -------------------------------------------------------------------------
    asl_field_show

    Type: Component method
    -------------------------------------------------------------------------
 */

void
    asl_field_show_ (
    void * item,                        //  The opaque pointer
    int opcode,                         //  The callback opcode
    FILE * trace_file,                  //  File to print to
    char * file,                        //  Source file
    size_t line                         //  Line number
)
{
    asl_field_t
        *self;
    int
        container_links;
#if defined (DEBUG) || defined (BASE_HISTORY) || defined (BASE_HISTORY_ASL_FIELD)
    qbyte
        history_index;
#endif


self = item;
container_links = 0;
if (self->list_head)
   container_links++;
    assert (opcode == ICL_CALLBACK_DUMP);

    fprintf (trace_file, "    <asl_field zombie = \"%u\" links = \"%u\" containers = \"%u\" file = \"%s\" line = \"%lu\"  pointer = \"%p\" />\n", self->zombie, self->links, container_links, file, (unsigned long) line, self);
#if defined (DEBUG) || defined (BASE_HISTORY) || defined (BASE_HISTORY_ASL_FIELD)
    if (self->history_last > ASL_FIELD_HISTORY_LENGTH) {
        fprintf (trace_file, "        <!-- possess history too large (%d) - call iMatix-tech -->\n",
            self->history_last);
        history_index = (self->history_last + 1) % ASL_FIELD_HISTORY_LENGTH;
        self->history_last %= ASL_FIELD_HISTORY_LENGTH;
    }
    else
        history_index = 0;

    for (; history_index != self->history_last; history_index = (history_index + 1) % ASL_FIELD_HISTORY_LENGTH) {
        fprintf (trace_file, "       <%s file = \"%s\" line = \"%lu\" links = \"%lu\" />\n",
            self->history_type [history_index],
            self->history_file [history_index],
            (unsigned long) self->history_line  [history_index],
            (unsigned long) self->history_links [history_index]);
    }
    fprintf (trace_file, "    </asl_field>\n");
#endif

}
/*  -------------------------------------------------------------------------
    asl_field_terminate

    Type: Component method
    -------------------------------------------------------------------------
 */

void
    asl_field_terminate (
void)
{


}
/*  -------------------------------------------------------------------------
    asl_field_destroy

    Type: Component method
    Destroys a asl_field_t object. Takes the address of a
    asl_field_t reference (a pointer to a pointer) and nullifies the
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
    asl_field_destroy_ (
    asl_field_t * ( * self_p ),         //  Reference to object reference
    char * file,                        //  Source fileSource file
    size_t line                         //  Line numberLine number
)
{
    asl_field_t *
        self = *self_p;                 //  Dereferenced Reference to object reference

if (self) {
    if (icl_atomic_cas32 (&self->zombie, TRUE, FALSE) == FALSE)
        asl_field_annihilate_ (self_p, file, line);

    if (self->links == 0) {
        icl_system_panic ("asl_field", "E: missing link on asl_field object");
        asl_field_show_ (self, ICL_CALLBACK_DUMP, stderr, file, line);
        abort ();
    }

    if (icl_atomic_dec32 ((volatile qbyte *) &self->links) == 0)
        asl_field_free_ ((asl_field_t *) self, file, line);
    *self_p = NULL;
}
}
/*  -------------------------------------------------------------------------
    asl_field_alloc

    Type: Component method
    -------------------------------------------------------------------------
 */

static asl_field_t *
    asl_field_alloc_ (
    char * file,                        //  Source file for call
    size_t line                         //  Line number for call
)
{

    asl_field_t *
        self = NULL;                    //  Object reference

//  Initialise cache if necessary
if (!s_cache)
    asl_field_cache_initialise ();

self = (asl_field_t *) icl_mem_cache_alloc_ (s_cache, file, line);



    return (self);
}
/*  -------------------------------------------------------------------------
    asl_field_free

    Type: Component method
    Freess a asl_field_t object.
    -------------------------------------------------------------------------
 */

static void
    asl_field_free_ (
    asl_field_t * self,                 //  Object reference
    char * file,                        //  Source file
    size_t line                         //  Line number
)
{
#if defined (DEBUG) || defined (BASE_HISTORY) || defined (BASE_HISTORY_ASL_FIELD)
    int
        history_last;
#endif


    if (self) {
#if defined (DEBUG) || defined (BASE_HISTORY) || defined (BASE_HISTORY_ASL_FIELD)
        //  Track possession operation in history
        history_last = icl_atomic_inc32 ((volatile qbyte *) &self->history_last) + 1;
        self->history_file  [history_last % ASL_FIELD_HISTORY_LENGTH] = file;
        self->history_line  [history_last % ASL_FIELD_HISTORY_LENGTH] = line;
        self->history_type  [history_last % ASL_FIELD_HISTORY_LENGTH] = "free";
        self->history_links [history_last % ASL_FIELD_HISTORY_LENGTH] = self->links;
#endif

    self->object_tag = ASL_FIELD_DEAD;
    icl_mem_free (self);
}
self = NULL;
}
/*  -------------------------------------------------------------------------
    asl_field_link

    Type: Component method
    Adds a link (reference count) to an object.

    If the object has been zombified (ie destroyed while extra links were present),
    this method returns NULL and does not add any links.

    This method does not lock the object.
    -------------------------------------------------------------------------
 */

asl_field_t *
    asl_field_link_ (
    asl_field_t * self,                 //  Not documented
    char * file,                        //  Source file for call
    size_t line                         //  Line number for call
)
{
#if defined (DEBUG) || defined (BASE_HISTORY) || defined (BASE_HISTORY_ASL_FIELD)
    int
        history_last;
#endif

    if (self) {
        ASL_FIELD_ASSERT_SANE (self);
        if (file)
            icl_mem_possess_ (self, file, line);
        icl_atomic_inc32 ((volatile qbyte *) &self->links);
#if defined (DEBUG) || defined (BASE_HISTORY) || defined (BASE_HISTORY_ASL_FIELD)
        //  Track possession operation in history
        history_last = icl_atomic_inc32 ((volatile qbyte *) &self->history_last) + 1;
        self->history_file  [history_last % ASL_FIELD_HISTORY_LENGTH] = file;
        self->history_line  [history_last % ASL_FIELD_HISTORY_LENGTH] = line;
        self->history_type  [history_last % ASL_FIELD_HISTORY_LENGTH] = "link";
        self->history_links [history_last % ASL_FIELD_HISTORY_LENGTH] = self->links;
#endif
    }

    return (self);
}
/*  -------------------------------------------------------------------------
    asl_field_unlink

    Type: Component method
    Removes a link (reference count) to an object.  Sets the object pointer to NULL
    to indicate that it is no longer valid.
    -------------------------------------------------------------------------
 */

void
    asl_field_unlink_ (
    asl_field_t * ( * self_p ),         //  Reference to object reference
    char * file,                        //  Source file for call
    size_t line                         //  Line number for call
)
{
#if defined (DEBUG) || defined (BASE_HISTORY) || defined (BASE_HISTORY_ASL_FIELD)
    int
        history_last;
#endif

    asl_field_t *
        self = *self_p;                 //  Dereferenced Reference to object reference

    if (self) {
        ASL_FIELD_ASSERT_SANE (self);
        if (self->links == 0) {
            icl_system_panic ("asl_field", "E: missing link on asl_field object");
            asl_field_show_ (self, ICL_CALLBACK_DUMP, stderr, file, line);
            abort ();
        }

#if defined (DEBUG) || defined (BASE_HISTORY) || defined (BASE_HISTORY_ASL_FIELD)
        //  Track possession operation in history.  Pre-empt value of links
        //  after operation; otherwise race condition can result in writing
        //  to freed memory.
        history_last = icl_atomic_inc32 ((volatile qbyte *) &self->history_last) + 1;
        self->history_file  [history_last % ASL_FIELD_HISTORY_LENGTH] = file;
        self->history_line  [history_last % ASL_FIELD_HISTORY_LENGTH] = line;
        self->history_type  [history_last % ASL_FIELD_HISTORY_LENGTH] = "unlink";
        self->history_links [history_last % ASL_FIELD_HISTORY_LENGTH] = self->links - 1;
#endif
        if (icl_atomic_dec32 ((volatile qbyte *) &self->links) == 0) {

        if (icl_atomic_cas32 (&self->zombie, TRUE, FALSE) == FALSE)
            asl_field_annihilate_ (self_p, file, line);
        asl_field_free_ ((asl_field_t *) self, file, line);
    }
    *self_p = NULL;
}
}
/*  -------------------------------------------------------------------------
    asl_field_cache_initialise

    Type: Component method
    Initialise the cache and register purge method with the meta-cache.
    -------------------------------------------------------------------------
 */

static void
    asl_field_cache_initialise (
void)
{

s_cache = icl_cache_get (sizeof (asl_field_t));
icl_system_register (asl_field_cache_purge, asl_field_cache_terminate);
}
/*  -------------------------------------------------------------------------
    asl_field_cache_purge

    Type: Component method
    -------------------------------------------------------------------------
 */

static void
    asl_field_cache_purge (
void)
{

icl_mem_cache_purge (s_cache);

}
/*  -------------------------------------------------------------------------
    asl_field_cache_terminate

    Type: Component method
    -------------------------------------------------------------------------
 */

static void
    asl_field_cache_terminate (
void)
{

s_cache = NULL;

}
/*  -------------------------------------------------------------------------
    asl_field_show_animation

    Type: Component method
    Enables animation of the component. Animation is sent to stdout.
    To enable animation you must generate using the option -animate:1.
    -------------------------------------------------------------------------
 */

void
    asl_field_show_animation (
    Bool enabled                        //  Are we enabling or disabling animation?
)
{

asl_field_animating = enabled;
}
/*  -------------------------------------------------------------------------
    asl_field_new_in_scope

    Type: Component method
    -------------------------------------------------------------------------
 */

void
    asl_field_new_in_scope_ (
    asl_field_t * * self_p,             //  Not documented
    icl_scope_t * _scope,               //  Not documented
    char * file,                        //  Source file for call
    size_t line                         //  Line number for call
)
{
    icl_destroy_t *
        _destroy;                       //  Not documented

*self_p = asl_field_new_ (file,line);

if (*self_p) {
    _destroy = icl_destroy_new   ((void * *) self_p, (icl_destructor_fn *) asl_field_destroy_);
    icl_destroy_list_queue (_scope, _destroy);
    icl_destroy_unlink (&_destroy);
}
}
//  Embed the version information in the resulting binary

char *EMBED__asl_field_version_start      = "VeRsIoNsTaRt:ipc";
char *EMBED__asl_field_component          = "asl_field ";
char *EMBED__asl_field_version            = "1.0 ";
char *EMBED__asl_field_copyright          = "Copyright (c) 1996-2009 iMatix Corporation";
char *EMBED__asl_field_filename           = "asl_field.icl ";
char *EMBED__asl_field_builddate          = "2011/03/01 ";
char *EMBED__asl_field_version_end        = "VeRsIoNeNd:ipc";

