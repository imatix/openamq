/*---------------------------------------------------------------------------
    ipr_tree.c - ipr_tree component

    Implements an XML tree builder - an easy way to construct XML trees
    for delivery to applications.  To load or modify XML trees, use the
    ipr_xml class interface.
    Generated from ipr_tree.icl by icl_gen using GSL/4.
    
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
#include "ipr_tree.h"                   //  Definitions for our class

//  Shorthand for class type

#define self_t              ipr_tree_t

//  Structure of the ipr_tree object

struct _ipr_tree_t {
dbyte
    object_tag;                     //  Object validity marker
ipr_xml_t
    *root_item,                     //  Tree root item
    *cur_branch;                    //  Current branch of tree
};

//  Shorthands for class methods

#define self_new            ipr_tree_new
#define self_annihilate     ipr_tree_annihilate
#define self_open           ipr_tree_open
#define self_leaf           ipr_tree_leaf
#define self_shut           ipr_tree_shut
#define self_save_xml       ipr_tree_save_xml
#define self_save_json      ipr_tree_save_json
#define self_selftest       ipr_tree_selftest
#define self_terminate      ipr_tree_terminate
#define self_show           ipr_tree_show
#define self_destroy        ipr_tree_destroy
#define self_alloc          ipr_tree_alloc
#define self_free           ipr_tree_free
#define self_cache_initialise  ipr_tree_cache_initialise
#define self_cache_purge    ipr_tree_cache_purge
#define self_cache_terminate  ipr_tree_cache_terminate
#define self_show_animation  ipr_tree_show_animation
#define self_new_in_scope   ipr_tree_new_in_scope

static void
    ipr_tree_annihilate (
ipr_tree_t * ( * self_p )               //  Reference to object reference
);

#define ipr_tree_alloc()                ipr_tree_alloc_ (__FILE__, __LINE__)
static ipr_tree_t *
    ipr_tree_alloc_ (
char * file,                            //  Source file for call
size_t line                             //  Line number for call
);

static void
    ipr_tree_free (
ipr_tree_t * self                       //  Object reference
);

static void
    ipr_tree_cache_initialise (
void);

static void
    ipr_tree_cache_purge (
void);

static void
    ipr_tree_cache_terminate (
void);

Bool
    ipr_tree_animating = TRUE;          //  Animation enabled by default
static icl_cache_t
    *s_cache = NULL;


void s_save_xml_item  (icl_longstr_t *output, ipr_xml_t *item, size_t indent);
void s_save_json_item (icl_longstr_t *output, ipr_xml_t *item, size_t indent,
    Bool glue_prev, Bool glue_next);
#include "ipr_xmll.h"
/*  -------------------------------------------------------------------------
    ipr_tree_new

    Type: Component method
    Creates and initialises a new ipr_tree_t object, returns a
    reference to the created object.
    Creates a new tree, which will be empty except for a root item.  To add
    properties to the root item, use "leaf".
    -------------------------------------------------------------------------
 */

ipr_tree_t *
    ipr_tree_new_ (
    char * file,                        //  Source file for call
    size_t line,                        //  Line number for call
    char * name                         //  The name of the root item
)
{
    ipr_tree_t *
        self = NULL;                    //  Object reference

    self = ipr_tree_alloc_ (file, line);
    if (self) {
        self->object_tag   = IPR_TREE_ALIVE;
#if defined (DEBUG)
        icl_mem_set_callback (self, ipr_tree_show_);
#endif

//
self->root_item  = ipr_xml_new (NULL, name, NULL);
self->cur_branch = ipr_xml_link (self->root_item);
assert (self->root_item);
}

    return (self);
}
/*  -------------------------------------------------------------------------
    ipr_tree_annihilate

    Type: Component method
    -------------------------------------------------------------------------
 */

static void
    ipr_tree_annihilate (
    ipr_tree_t * ( * self_p )           //  Reference to object reference
)
{

    ipr_tree_t *
        self = *self_p;                 //  Dereferenced Reference to object reference

IPR_TREE_ASSERT_SANE (self);

ipr_xml_unlink  (&self->cur_branch);
ipr_xml_destroy (&self->root_item);

}
/*  -------------------------------------------------------------------------
    ipr_tree_open

    Type: Component method
    Accepts a ipr_tree_t reference and returns zero in case of success,
    1 in case of errors.
    Adds a new branch to the current branch, and automatically opens that new
    branch.  New leafs and branches will be added to this branch until you do
    a close.  The normal sequence of operations is open, leaf, leaf... close.
    -------------------------------------------------------------------------
 */

int
    ipr_tree_open (
    ipr_tree_t * self,                  //  Reference to object
    char * name                         //  The name of the new branch
)
{
ipr_xml_t
    *new_branch;
    int
        rc = 0;                         //  Return code

IPR_TREE_ASSERT_SANE (self);

//
assert (self->cur_branch);
new_branch = ipr_xml_new (self->cur_branch, name, NULL);
ipr_xml_unlink (&self->cur_branch);
self->cur_branch = new_branch;


    return (rc);
}
/*  -------------------------------------------------------------------------
    ipr_tree_leaf

    Type: Component method
    Accepts a ipr_tree_t reference and returns zero in case of success,
    1 in case of errors.
    Adds a new property (leaf) to the current branch.  A leaf has a name and a
    value.  The value may be a format string, and be followed by arguments.  If
    the name is NULL, the value is used as the branch content value.
    -------------------------------------------------------------------------
 */

int
    ipr_tree_leaf (
    ipr_tree_t * self,                  //  Reference to object
    char * name,                        //  The name of the new leaf
    char * format,                      //  Format specifier
...                                     //  Variable arguments
)
{
    int
        _rc;
    va_list
        args;
    va_start (args, format);
    _rc = ipr_tree_leaf_v (self, name, format, args );
    va_end (args);
    return _rc;
}
/*  -------------------------------------------------------------------------
    ipr_tree_leaf_v

    Type: Component method
    Accepts a ipr_tree_t reference and returns zero in case of success,
    1 in case of errors.
    Adds a new property (leaf) to the current branch.  A leaf has a name and a
    value.  The value may be a format string, and be followed by arguments.  If
    the name is NULL, the value is used as the branch content value.
    -------------------------------------------------------------------------
 */

int
    ipr_tree_leaf_v (
    ipr_tree_t * self,                  //  Reference to object
    char * name,                        //  The name of the new leaf
    char * format,                      //  Format specifier
va_list args                            //  Variable arguments
)
{
char
    *string;                        //  Holds formatted value
int
    str_size = 1024,
    fmt_size;
    int
        rc = 0;                         //  Return code

IPR_TREE_ASSERT_SANE (self);

//

//
assert (self->cur_branch);

//  Format a string with the value
string = icl_mem_alloc (str_size + 1);
fmt_size = apr_vsnprintf (string, str_size, format, args);
if (fmt_size >= str_size) {
    //  If formatted value overflowed string, resize and repeat
    icl_mem_free (string);
    str_size = fmt_size;
    string = icl_mem_alloc (str_size + 1);
    fmt_size = apr_vsnprintf (string, str_size, format, args);
}
if (name)
    ipr_xml_attr_set (self->cur_branch, name, string);
else
    ipr_xml_value_set (self->cur_branch, string);
icl_mem_free (string);


    return (rc);
}
/*  -------------------------------------------------------------------------
    ipr_tree_shut

    Type: Component method
    Accepts a ipr_tree_t reference and returns zero in case of success,
    1 in case of errors.
    Climbs back along the current branch.  A close must match a previous open.
    Once you have shut a branch there is no way to reopen it.  Doing too many
    shuts will cause an assertion failure.
    -------------------------------------------------------------------------
 */

int
    ipr_tree_shut (
    ipr_tree_t * self                   //  Reference to object
)
{
    int
        rc = 0;                         //  Return code

IPR_TREE_ASSERT_SANE (self);

//
assert (self->cur_branch && self->cur_branch != self->root_item);
self->cur_branch = ipr_xml_parent (&self->cur_branch);


    return (rc);
}
/*  -------------------------------------------------------------------------
    ipr_tree_save_xml

    Type: Component method
    Format the XML tree as a user-friendly XML string and return it in a
    fresh icl_longstr_t. The caller should destroy the longstr when finished
    using it. Note that this method does not do meta characters escaping.
    -------------------------------------------------------------------------
 */

icl_longstr_t *
    ipr_tree_save_xml (
    ipr_tree_t * self                   //  Reference to tree
)
{
    icl_longstr_t *
        output;                         //  Not documented

//
output = icl_longstr_new (NULL, 2000);
icl_longstr_cat (output, "<?xml version=\"1.0\"?>\n");
s_save_xml_item (output, self->root_item, 0);

    return (output);
}
/*  -------------------------------------------------------------------------
    ipr_tree_save_json

    Type: Component method
    Format the XML tree as a user-friendly JSON string and return it in a
    fresh icl_longstr_t. The caller should destroy the longstr when finished
    using it. Note that this method does not do meta characters escaping.
    -------------------------------------------------------------------------
 */

icl_longstr_t *
    ipr_tree_save_json (
    ipr_tree_t * self                   //  Reference to tree
)
{
    icl_longstr_t *
        output;                         //  Not documented

//
output = icl_longstr_new (NULL, 2000);
icl_longstr_cat (output, "{\n");
s_save_json_item (output, self->root_item, 0, FALSE, FALSE);
icl_longstr_cat (output, "\n}\n");

    return (output);
}
/*  -------------------------------------------------------------------------
    ipr_tree_selftest

    Type: Component method
    -------------------------------------------------------------------------
 */

void
    ipr_tree_selftest (
void)
{
ipr_tree_t
    *tree;
icl_longstr_t
    *longstr;

//
tree = ipr_tree_new ("restms");
assert (tree);
ipr_tree_leaf (tree, "version", "1.0");
ipr_tree_leaf (tree, "status", "ok");

  ipr_tree_open (tree, "pipe_class");
    ipr_tree_leaf (tree, "name", "name");
    ipr_tree_leaf (tree, "uri", "uri");
    ipr_tree_leaf (tree, "size", "24");
  ipr_tree_shut (tree);

  ipr_tree_open (tree, "feed_class");
    ipr_tree_leaf (tree, "name", "name");
    ipr_tree_leaf (tree, "uri", "uri");
      ipr_tree_open (tree, "feed");
        ipr_tree_leaf (tree, "name", "name1");
        ipr_tree_leaf (tree, "uri", "uri1");
      ipr_tree_shut (tree);
      ipr_tree_open (tree, "feed");
        ipr_tree_leaf (tree, "name", "name2");
        ipr_tree_leaf (tree, "uri", "uri2");
      ipr_tree_shut (tree);
      ipr_tree_open (tree, "feed");
        ipr_tree_leaf (tree, "name", "name3");
        ipr_tree_leaf (tree, "uri", "uri3");
      ipr_tree_shut (tree);
  ipr_tree_shut (tree);

  ipr_tree_open (tree, "pipe");
    ipr_tree_leaf (tree, "name", "name");
    ipr_tree_leaf (tree, "uri", "uri");
    ipr_tree_leaf (tree, "size", "0");
      ipr_tree_open (tree, "join");
        ipr_tree_leaf (tree, "address", "pattern");
        ipr_tree_leaf (tree, "feed", "name");
        ipr_tree_leaf (tree, "uri", "uri");
      ipr_tree_shut (tree);
      ipr_tree_open (tree, "nozzle");
        ipr_tree_leaf (tree, "name", "name");
        ipr_tree_leaf (tree, "size", "123");
      ipr_tree_shut (tree);
  ipr_tree_shut (tree);

  ipr_tree_open (tree, "feed");
    ipr_tree_leaf (tree, "name", "name");
    ipr_tree_leaf (tree, "uri", "uri");
  ipr_tree_shut (tree);

longstr = ipr_tree_save_xml (tree);
icl_longstr_destroy (&longstr);

longstr = ipr_tree_save_json (tree);
icl_longstr_destroy (&longstr);

ipr_tree_destroy (&tree);
}
/*  -------------------------------------------------------------------------
    ipr_tree_terminate

    Type: Component method
    -------------------------------------------------------------------------
 */

void
    ipr_tree_terminate (
void)
{


}
/*  -------------------------------------------------------------------------
    ipr_tree_show

    Type: Component method
    -------------------------------------------------------------------------
 */

void
    ipr_tree_show_ (
    void * item,                        //  The opaque pointer
    int opcode,                         //  The callback opcode
    FILE * trace_file,                  //  File to print to
    char * file,                        //  Source file
    size_t line                         //  Line number
)
{
ipr_tree_t
    *self;
int
    container_links;


self = item;
container_links = 0;
assert (opcode == ICL_CALLBACK_DUMP);

fprintf (trace_file, "    <ipr_tree file = \"%s\" line = \"%lu\"  pointer = \"%p\" />\n", file, (unsigned long) line, self);

}
/*  -------------------------------------------------------------------------
    ipr_tree_destroy

    Type: Component method
    Destroys a ipr_tree_t object. Takes the address of a
    ipr_tree_t reference (a pointer to a pointer) and nullifies the
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
    ipr_tree_destroy_ (
    ipr_tree_t * ( * self_p ),          //  Reference to object reference
    char * file,                        //  Source fileSource file
    size_t line                         //  Line numberLine number
)
{
    ipr_tree_t *
        self = *self_p;                 //  Dereferenced Reference to object reference

if (self) {
    ipr_tree_annihilate (self_p);
    ipr_tree_free ((ipr_tree_t *) self);
    *self_p = NULL;
}
}
/*  -------------------------------------------------------------------------
    ipr_tree_alloc

    Type: Component method
    -------------------------------------------------------------------------
 */

static ipr_tree_t *
    ipr_tree_alloc_ (
    char * file,                        //  Source file for call
    size_t line                         //  Line number for call
)
{

    ipr_tree_t *
        self = NULL;                    //  Object reference

//  Initialise cache if necessary
if (!s_cache)
    ipr_tree_cache_initialise ();

self = (ipr_tree_t *) icl_mem_cache_alloc_ (s_cache, file, line);
if (self)
    memset (self, 0, sizeof (ipr_tree_t));



    return (self);
}
/*  -------------------------------------------------------------------------
    ipr_tree_free

    Type: Component method
    Freess a ipr_tree_t object.
    -------------------------------------------------------------------------
 */

static void
    ipr_tree_free (
    ipr_tree_t * self                   //  Object reference
)
{


if (self) {

        memset (&self->object_tag, 0, sizeof (ipr_tree_t) - ((byte *) &self->object_tag - (byte *) self));
//        memset (self, 0, sizeof (ipr_tree_t));
        self->object_tag = IPR_TREE_DEAD;
        icl_mem_free (self);
    }
    self = NULL;
}
/*  -------------------------------------------------------------------------
    ipr_tree_cache_initialise

    Type: Component method
    Initialise the cache and register purge method with the meta-cache.
    -------------------------------------------------------------------------
 */

static void
    ipr_tree_cache_initialise (
void)
{

s_cache = icl_cache_get (sizeof (ipr_tree_t));
icl_system_register (ipr_tree_cache_purge, ipr_tree_cache_terminate);
}
/*  -------------------------------------------------------------------------
    ipr_tree_cache_purge

    Type: Component method
    -------------------------------------------------------------------------
 */

static void
    ipr_tree_cache_purge (
void)
{

icl_mem_cache_purge (s_cache);

}
/*  -------------------------------------------------------------------------
    ipr_tree_cache_terminate

    Type: Component method
    -------------------------------------------------------------------------
 */

static void
    ipr_tree_cache_terminate (
void)
{

s_cache = NULL;

}
/*  -------------------------------------------------------------------------
    ipr_tree_show_animation

    Type: Component method
    Enables animation of the component. Animation is sent to stdout.
    To enable animation you must generate using the option -animate:1.
    -------------------------------------------------------------------------
 */

void
    ipr_tree_show_animation (
    Bool enabled                        //  Are we enabling or disabling animation?
)
{

ipr_tree_animating = enabled;
}
/*  -------------------------------------------------------------------------
    ipr_tree_new_in_scope

    Type: Component method
    -------------------------------------------------------------------------
 */

void
    ipr_tree_new_in_scope_ (
    ipr_tree_t * * self_p,              //  Not documented
    icl_scope_t * _scope,               //  Not documented
    char * file,                        //  Source file for call
    size_t line,                        //  Line number for call
    char * name                         //  The name of the root item
)
{
    icl_destroy_t *
        _destroy;                       //  Not documented

*self_p = ipr_tree_new_ (file,line,name);

if (*self_p) {
    _destroy = icl_destroy_new   ((void * *) self_p, (icl_destructor_fn *) ipr_tree_destroy_);
    icl_destroy_list_queue (_scope, _destroy);
    icl_destroy_unlink (&_destroy);
}
}
void
s_save_xml_item (icl_longstr_t *output, ipr_xml_t *item, size_t indent)
{
    ipr_xml_t
        *child;
    icl_shortstr_t
        blanks;
    ipr_xml_attr_t
        *attr;

    assert (ipr_xml_name (item));
    assert (indent < 256);
    child = ipr_xml_first_child (item);
    icl_longstr_cat (output, "%s<%s", ipr_str_block (blanks, ' ', indent), ipr_xml_name (item));
    IPR_FOR_XML_ATTRIBUTES (attr, item) {
        icl_longstr_cat (output, " %s = \"%s\"", ipr_xml_attr_name (attr), ipr_xml_attr_value (attr));
    }
    if (child) {
        icl_longstr_cat (output, ">\n");
        while (child) {
            s_save_xml_item (output, child, indent + 4);
            child = ipr_xml_next_sibling (&child);
        }
        icl_longstr_cat (output, "%s</%s>\n", ipr_str_block (blanks, ' ', indent), ipr_xml_name (item));
    }
    else
        icl_longstr_cat (output, " />\n");
}

void
s_save_json_item (icl_longstr_t *output, ipr_xml_t *item, size_t indent,
    Bool glue_prev, Bool glue_next)
{
    ipr_xml_t
        *child;                         //  Child items
    icl_shortstr_t
        blanks;                         //  Indentation spacing
    ipr_xml_attr_t
        *attr;
    char
        *comma = "";
    char
        *item_name,
        *prev_name,                     //  Previous child name
        *next_name;                     //  Following child name

    assert (ipr_xml_name (item));
    assert (indent < 256);
    child = ipr_xml_first_child (item);
    ipr_str_block (blanks, ' ', indent);

    if (glue_prev)
        icl_longstr_cat (output, "%s     {", blanks);
    else {
        icl_longstr_cat (output, "%s\"%s\":%s",
            blanks, ipr_xml_name (item), indent? "[ ": "");
        if (glue_next)
            icl_longstr_cat (output, "\n%s     {", blanks);
        else
            icl_longstr_cat (output, "{");
    }

    IPR_FOR_XML_ATTRIBUTES (attr, item) {
        if (ipr_str_numeric (ipr_xml_attr_value (attr)))
            icl_longstr_cat (output, "%s \"%s\":%s",
                comma, ipr_xml_attr_name (attr), ipr_xml_attr_value (attr));
        else
            icl_longstr_cat (output, "%s \"%s\":\"%s\"",
                comma, ipr_xml_attr_name (attr), ipr_xml_attr_value (attr));
        comma = ",";
    }
    if (child) {
        while (child) {
            icl_longstr_cat (output, "%s\n", comma);
            item_name = ipr_xml_name (child);
            prev_name = ipr_xml_name ((ipr_xml_t *) child->list_prev);
            next_name = ipr_xml_name ((ipr_xml_t *) child->list_next);
            s_save_json_item (output, child, indent + 4,
                prev_name && streq (item_name, prev_name),
                next_name && streq (item_name, next_name));
            child = ipr_xml_next_sibling (&child);
            comma = ",";
        }
        icl_longstr_cat (output, "\n%s    }", ipr_str_block (blanks, ' ', indent));
    }
    else
        icl_longstr_cat (output, " }");

    if (indent && !glue_next)
        icl_longstr_cat (output, " ]");
}

//  Embed the version information in the resulting binary

char *EMBED__ipr_tree_version_start       = "VeRsIoNsTaRt:ipc";
char *EMBED__ipr_tree_component           = "ipr_tree ";
char *EMBED__ipr_tree_version             = "1.1 ";
char *EMBED__ipr_tree_copyright           = "Copyright (c) 1996-2009 iMatix Corporation";
char *EMBED__ipr_tree_filename            = "ipr_tree.icl ";
char *EMBED__ipr_tree_builddate           = "2011/03/01 ";
char *EMBED__ipr_tree_version_end         = "VeRsIoNeNd:ipc";

