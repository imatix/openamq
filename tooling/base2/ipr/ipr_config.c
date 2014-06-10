/*---------------------------------------------------------------------------
    ipr_config.c - ipr_config component

    Generated from ipr_config.icl by icl_gen using GSL/4.
    
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
#include "ipr_config.h"                 //  Definitions for our class

//  Shorthand for class type

#define self_t              ipr_config_t

//  Shorthands for class methods

#define self_new            ipr_config_new
#define self_annihilate     ipr_config_annihilate
#define self_dup            ipr_config_dup
#define self_load           ipr_config_load
#define self_load_bucket    ipr_config_load_bucket
#define self_save           ipr_config_save
#define self_locate         ipr_config_locate
#define self_next           ipr_config_next
#define self_item           ipr_config_item
#define self_value          ipr_config_value
#define self_get            ipr_config_get
#define self_getn           ipr_config_getn
#define self_getp           ipr_config_getp
#define self_put            ipr_config_put
#define self_putp           ipr_config_putp
#define self_selftest       ipr_config_selftest
#define self_terminate      ipr_config_terminate
#define self_show           ipr_config_show
#define self_destroy        ipr_config_destroy
#define self_alloc          ipr_config_alloc
#define self_free           ipr_config_free
#define self_read_lock      ipr_config_read_lock
#define self_write_lock     ipr_config_write_lock
#define self_unlock         ipr_config_unlock
#define self_cache_initialise  ipr_config_cache_initialise
#define self_cache_purge    ipr_config_cache_purge
#define self_cache_terminate  ipr_config_cache_terminate
#define self_show_animation  ipr_config_show_animation
#define self_new_in_scope   ipr_config_new_in_scope

static void
    ipr_config_annihilate (
ipr_config_t * ( * self_p )             //  Reference to object reference
);

#define ipr_config_alloc()              ipr_config_alloc_ (__FILE__, __LINE__)
static ipr_config_t *
    ipr_config_alloc_ (
char * file,                            //  Source file for call
size_t line                             //  Line number for call
);

static void
    ipr_config_free (
ipr_config_t * self                     //  Object reference
);

static void
    ipr_config_cache_initialise (
void);

static void
    ipr_config_cache_purge (
void);

static void
    ipr_config_cache_terminate (
void);

Bool
    ipr_config_animating = TRUE;        //  Animation enabled by default
static icl_cache_t
    *s_cache = NULL;


static void
s_merge_trees (ipr_xml_t *xml_dest, ipr_xml_t *xml_source);
#include "ipr_xmll.h"
/*  -------------------------------------------------------------------------
    ipr_config_new

    Type: Component method
    Creates and initialises a new ipr_config_t object, returns a
    reference to the created object.
    If a filename is specified, loads the configuration table from the
    XML file (see ipr_config_load method).
    -------------------------------------------------------------------------
 */

ipr_config_t *
    ipr_config_new_ (
    char * file,                        //  Source file for call
    size_t line,                        //  Line number for call
    char * filename,                    //  Name of configuration file
    Bool required                       //  Complain if file not found
)
{
    ipr_config_t *
        self = NULL;                    //  Object reference

    self = ipr_config_alloc_ (file, line);
    if (self) {
        self->object_tag   = IPR_CONFIG_ALIVE;
#if (defined (BASE_THREADSAFE))
        self->rwlock = icl_rwlock_new ();
#endif
#if defined (DEBUG)
        icl_mem_set_callback (self, ipr_config_show_);
#endif

//
//  Empty config tree has just a root item
self->xml_root = ipr_xml_new (NULL, "root", "");
if (filename)
    self_load (self, filename, required);
}

    return (self);
}
/*  -------------------------------------------------------------------------
    ipr_config_annihilate

    Type: Component method
    -------------------------------------------------------------------------
 */

static void
    ipr_config_annihilate (
    ipr_config_t * ( * self_p )         //  Reference to object reference
)
{
#if (defined (BASE_THREADSAFE))
    icl_rwlock_t
        *rwlock;
#endif

    ipr_config_t *
        self = *self_p;                 //  Dereferenced Reference to object reference

    IPR_CONFIG_ASSERT_SANE (self);
#if (defined (BASE_THREADSAFE))
    rwlock = self->rwlock;
    if (rwlock)
         icl_rwlock_write_lock (rwlock);
#endif

if (self->duplicated)
    ipr_xml_unlink (&self->xml_root);
else
    ipr_xml_destroy (&self->xml_root);

ipr_xml_unlink (&self->xml_item);
icl_mem_free (self->cur_value);
#if (defined (BASE_THREADSAFE))
    if (rwlock)
        icl_rwlock_unlock (rwlock);
#endif

}
/*  -------------------------------------------------------------------------
    ipr_config_dup

    Type: Component method
    Duplicates a configuration tree, used to create a local copy
    for navigation.  Copies the exact tree state.
    -------------------------------------------------------------------------
 */

ipr_config_t *
    ipr_config_dup (
    ipr_config_t * self                 //  Reference to object
)
{
    ipr_config_t *
        copy;                           //  New instance

//
IPR_CONFIG_ASSERT_SANE (self);
copy = self_new (NULL, FALSE);
ipr_xml_destroy (&copy->xml_root);
copy->xml_root   = ipr_xml_link (self->xml_root);
copy->xml_item   = ipr_xml_link (self->xml_item);
copy->located    = self->located;
copy->duplicated = TRUE;
if (copy->xml_item)
    copy->cur_value = ipr_xml_text (copy->xml_item);

    return (copy);
}
/*  -------------------------------------------------------------------------
    ipr_config_load

    Type: Component method
    Accepts a ipr_config_t reference and returns zero in case of success,
    1 in case of errors.
    Loads an XML file into the current configuration tree.  The load method
    merges data in the loaded file with any data already in the tree.  This
    allows configuration file inheritence.  The merge process works by
    matching and merging items at each level in the tree.  Two items with the
    same item name are matched if they both have the same 'name' attribute or
    both have no 'name' attribute, and no other attributes.  The default action
    is to merge items.  Merging means that all attributes of the new item are
    applied to the existing one, and all children of the two items are in turn
    matched and merged.  If the item has the property 'config_meta' set to
    'discrete', matching children are not merged but replaced.  By definition
    a configuration tree cannot have two items with the same name at any given
    level.  Apart from this, configuration trees can have any XML syntax.
    Returns 0 if the configuration file was loaded, else -1.  Searches for the
    XML file on the PATH and reports the full path of the file loaded.
    -------------------------------------------------------------------------
 */

int
    ipr_config_load (
    ipr_config_t * self,                //  Reference to object
    char * filename,                    //  Name of configuration file
    Bool required                       //  Complain if file not found
)
{
ipr_xml_t
    *xml_root = NULL;                //  Tree of file we've loaded
ipr_xml_t
    *child;
    int
        rc = 0;                         //  Return code

IPR_CONFIG_ASSERT_SANE (self);

//
rc = ipr_xml_load_file (&xml_root, "PATH", filename, FALSE);
switch (rc) {
    case IPR_XML_NOERROR:            //  Just a long name for zero :-/
        child = ipr_xml_first_child (self->xml_root);
        icl_console_print ("I: loaded configuration from %s",
            ipr_xml_attr_get (xml_root, "fullname", "missing?"));
        ipr_xml_unlink (&child);

        s_merge_trees (self->xml_root, xml_root);
        ipr_xml_unlink (&self->xml_item);
        self->xml_item = ipr_xml_first_child (self->xml_root);
        break;
    case IPR_XML_FILEERROR:
        if (required) {
            icl_console_print ("ipr_config E: '%s' not found", filename);
            icl_console_print ("ipr_config W: %s", ipr_xml_error ());
            rc = -1;
        }
        break;
    case IPR_XML_LOADERROR:
        icl_console_print ("ipr_config W: syntax error in config file '%s'", filename);
        icl_console_print ("ipr_config W: %s", ipr_xml_error ());
        rc = -1;
        break;
}
ipr_xml_destroy (&xml_root);


    return (rc);
}
/*  -------------------------------------------------------------------------
    ipr_config_load_bucket

    Type: Component method
    Accepts a ipr_config_t reference and returns zero in case of success,
    1 in case of errors.
    Loads a bucket of XML data into the current configuration tree.  Merges
    data in the loaded data with any data already in the tree, as for the
    load method.  Returns 0 if the data was loaded, else -1.
    -------------------------------------------------------------------------
 */

int
    ipr_config_load_bucket (
    ipr_config_t * self,                //  Reference to object
    ipr_bucket_t * bucket               //  Bucket of XML data
)
{
ipr_xml_t
    *xml_root = NULL;                //  Tree of file we've loaded
    int
        rc = 0;                         //  Return code

IPR_CONFIG_ASSERT_SANE (self);

//
rc = ipr_xml_load_bucket (&xml_root, bucket, FALSE);
switch (rc) {
    case IPR_XML_NOERROR:
        s_merge_trees (self->xml_root, xml_root);
        ipr_xml_unlink (&self->xml_item);
        self->xml_item = ipr_xml_first_child (self->xml_root);
        break;
    case IPR_XML_LOADERROR:
        rc = -1;
        break;
}
ipr_xml_destroy (&xml_root);


    return (rc);
}
/*  -------------------------------------------------------------------------
    ipr_config_save

    Type: Component method
    Accepts a ipr_config_t reference and returns zero in case of success,
    1 in case of errors.
    Saves the configuration tree to the specified file.
    -------------------------------------------------------------------------
 */

int
    ipr_config_save (
    ipr_config_t * self,                //  Reference to object
    char * filename                     //  Name of configuration file
)
{
ipr_xml_t
    *xml_item;                      //  Current item in our new tree
    int
        rc = 0;                         //  Return code

IPR_CONFIG_ASSERT_SANE (self);

//
xml_item = ipr_xml_first_child (self->xml_root);
ipr_xml_save_file (xml_item, filename);
ipr_xml_unlink (&xml_item);


    return (rc);
}
/*  -------------------------------------------------------------------------
    ipr_config_locate

    Type: Component method
    Accepts a ipr_config_t reference and returns zero in case of success,
    1 in case of errors.
    Locates an item in the configuration tree.  We use a simple path
    notation. The path to an item is specified as an absolute path:
    "/item/item..." or a relative path (with no leading slash). The
    last item in the path may be selected by its 'name' attribute.
    You can use ".." in the path to move to the parent item, or "+"
    to move to the first child item.  The name may be null. Having
    located an item you can locate further items relative to it, or
    access the item's value and attributes.  Returns 0 if the item
    was located successfully, -1 if not.  After a locate the located
    property also indicates whether the item was found or not.
    -------------------------------------------------------------------------
 */

int
    ipr_config_locate (
    ipr_config_t * self,                //  Reference to object
    char * path,                        //  Path to item
    char * name                         //  Name selector
)
{
char
    *s_path,
    *token_start,
    *token_end;
int
    token_nbr;
ipr_xml_t
    *xml_item,
    *xml_child;
Bool
    have_match;
    int
        rc = 0;                         //  Return code

IPR_CONFIG_ASSERT_SANE (self);

//
assert (path);
if (*path == '/') {
    path++;
    if (self->xml_item)                 //  Remove ref to old item
        ipr_xml_unlink (&self->xml_item);
    self->xml_item = self->xml_root;
    ipr_xml_link (self->xml_item);      //  Add ref to item
}
//  Now process path unless XML tree is empty
if (self->xml_item) {
    xml_item  = self->xml_item;
    ipr_xml_link (xml_item);
    token_nbr = 0;
    s_path = icl_mem_strdup (path);
    token_end = s_path;
    do {
        token_start = token_end;
        token_end   = strchr (token_end, '/');
        if (token_end)
            *token_end = 0;

        if (streq (token_start, "..")) {
            if (xml_item != self->xml_root)
                xml_item = ipr_xml_parent (&xml_item);
        }
        else
        if (streq (token_start, "+")) {
            xml_child = ipr_xml_first_child (xml_item);
            if (xml_child) {
                ipr_xml_unlink (&xml_item);
                xml_item = xml_child;
            }
            else
                rc = -1;                //  No child items
        }
        else {
            //  Find first child item with name of this token
            have_match = FALSE;
            IPR_FOR_XML_CHILDREN (xml_child, xml_item) {
                if (streq (ipr_xml_name (xml_child), token_start)) {
                    //  If at end of path, use name selector
                    if ((!token_end) && name) {
                        if (streq (ipr_xml_attr_get (xml_child, "name", ""), name))
                            have_match = TRUE;
                    }
                    else
                        have_match = TRUE;
                }
                if (have_match) {
                    ipr_xml_unlink (&xml_item);
                    xml_item = xml_child;
                    break;
                }
            }
            if (!have_match) {
                rc = -1;
                break;              //  No match, quit searching
            }
        }
        if (token_end)
            token_end++;            //  Bump to after slash

    } while (token_end);            //  Until we end the path

    if (rc == 0) {
        ipr_xml_unlink (&self->xml_item);   //  Remove ref to old item
        self->xml_item = xml_item;
    }
    else
        ipr_xml_unlink (&xml_item);

    icl_mem_free (s_path);
}
else
    rc = -1;

self->located = (rc == 0);


    return (rc);
}
/*  -------------------------------------------------------------------------
    ipr_config_next

    Type: Component method
    Accepts a ipr_config_t reference and returns zero in case of success,
    1 in case of errors.
    Locates the next sibling item in the configuration tree.  This
    will be the next item at the same level as the current item, if
    any.  Returns 0 if there was a next item, -1 if not.
    -------------------------------------------------------------------------
 */

int
    ipr_config_next (
    ipr_config_t * self                 //  Reference to object
)
{
ipr_xml_t
    *xml_item;
    int
        rc = 0;                         //  Return code

IPR_CONFIG_ASSERT_SANE (self);

//
IPR_CONFIG_ASSERT_SANE (self);
xml_item = ipr_xml_link (self->xml_item);
self->xml_item = ipr_xml_next_sibling (&self->xml_item);
if (self->xml_item)
    ipr_xml_unlink (&xml_item);
else {
    self->xml_item = xml_item;
    rc = -1;
}
self->located = (rc == 0);


    return (rc);
}
/*  -------------------------------------------------------------------------
    ipr_config_item

    Type: Component method
    Returns the item name of the current item. You should not modify
    the returned string.
    -------------------------------------------------------------------------
 */

char *
    ipr_config_item (
    ipr_config_t * self                 //  Reference to object
)
{
    char *
        item;                           //  Returned name

//
IPR_CONFIG_ASSERT_SANE (self);
assert (self->xml_item);
item = ipr_xml_name (self->xml_item);

    return (item);
}
/*  -------------------------------------------------------------------------
    ipr_config_value

    Type: Component method
    Returns the value of the current item.  This can include newlines
    and leading and trailing spaces.  You should not modify the returned
    string.  The returned string is valid until you request another item
    value.
    -------------------------------------------------------------------------
 */

char *
    ipr_config_value (
    ipr_config_t * self                 //  Reference to object
)
{
    char *
        value;                          //  Returned value

//
IPR_CONFIG_ASSERT_SANE (self);
assert (self->xml_item);
if (self->cur_value)
    icl_mem_free (self->cur_value);
self->cur_value = ipr_xml_text (self->xml_item);
value = self->cur_value;

    return (value);
}
/*  -------------------------------------------------------------------------
    ipr_config_get

    Type: Component method
    Returns the specified attribute, if it exists. The attribute value
    can (but does usually not) include newlines. You should not modify
    the returned string. Returns the default value if the attribute is
    not defined.
    -------------------------------------------------------------------------
 */

char *
    ipr_config_get (
    ipr_config_t * self,                //  Reference to object
    char * name,                        //  Attribute name
    char * dflt                         //  Default value
)
{
    char *
        value;                          //  Returned value

//
IPR_CONFIG_ASSERT_SANE (self);
value = ipr_xml_attr_get (self->xml_item, name, dflt);

    return (value);
}
/*  -------------------------------------------------------------------------
    ipr_config_getn

    Type: Component method
    Returns the numeric value for the specified attribute, or zero if
    not defined.
    -------------------------------------------------------------------------
 */

long
    ipr_config_getn (
    ipr_config_t * self,                //  Reference to object
    char * name                         //  Attribute name
)
{
    long
        value;                          //  Returned value

//
IPR_CONFIG_ASSERT_SANE (self);
value = atol (ipr_xml_attr_get (self->xml_item, name, "0"));

    return (value);
}
/*  -------------------------------------------------------------------------
    ipr_config_getp

    Type: Component method
    Locates a path in the configuration tree and returns an attribute
    of that item.  Combines the ipr_config_locate and ipr_config_get
    methods and leaves the current object in the same state as the locate
    method would.
    -------------------------------------------------------------------------
 */

char *
    ipr_config_getp (
    ipr_config_t * self,                //  Reference to object
    char * path,                        //  Path to item
    char * name,                        //  Attribute name
    char * dflt                         //  Default value
)
{
    char *
        value;                          //  Returned value

//
IPR_CONFIG_ASSERT_SANE (self);
self_locate (self, path, NULL);
value = self_get (self, name, dflt);

    return (value);
}
/*  -------------------------------------------------------------------------
    ipr_config_put

    Type: Component method
    Updates or inserts an attribute value at the current position in
    the configuration file.
    -------------------------------------------------------------------------
 */

char *
    ipr_config_put (
    ipr_config_t * self,                //  Reference to object
    char * name,                        //  Attribute name
    char * value                        //  Attribute value
)
{

//
IPR_CONFIG_ASSERT_SANE (self);
ipr_xml_attr_set (self->xml_item, name, value);

    return (value);
}
/*  -------------------------------------------------------------------------
    ipr_config_putp

    Type: Component method
    Locates a path in the configuration tree and updates an attribute
    of that item.  Combines the ipr_config_locate and ipr_config_put
    methods and leaves the current object in the same state as the locate
    method would.
    -------------------------------------------------------------------------
 */

char *
    ipr_config_putp (
    ipr_config_t * self,                //  Reference to object
    char * path,                        //  Path to item
    char * name,                        //  Attribute name
    char * value                        //  Attribute value
)
{

//
IPR_CONFIG_ASSERT_SANE (self);
//TODO: should create path if it's not already defined
self_locate (self, path, NULL);
self_put    (self, name, value);

    return (value);
}
/*  -------------------------------------------------------------------------
    ipr_config_selftest

    Type: Component method
    -------------------------------------------------------------------------
 */

void
    ipr_config_selftest (
void)
{
ipr_config_t
    *config;
int
    items;

//
config = ipr_config_new ("ipr_config_test1.xml", TRUE);
ipr_config_load (config, "ipr_config_test2.xml", TRUE);
ipr_config_load (config, "ipr_config_test3.xml", TRUE);
ipr_config_load (config, "no-such-file.xml",    FALSE);
ipr_config_save (config, "ipr_config_save.xml");

//  Test data has four queues, iterate through them
ipr_config_locate (config, "/config/queues/queue", "");
for (items = 0; items < 4; items++) {
    assert (config->located);
    ipr_config_next (config);
}
assert (config->located == FALSE);

//  Try some navigation
ipr_config_locate (config, "/config/general/security/profile", "custom");
assert (config->located);
ipr_config_locate (config, "..", "default");
assert (config->located);

//  Test config_meta = "discrete", we should see example profile from test3
ipr_config_locate (config, "/config/profiles/profile", "example");
assert (config->located);
ipr_config_locate (config, "limit", NULL);
for (items = 0; items < 3; items++) {
    assert (config->located);
    ipr_config_next (config);
}
assert (config->located == FALSE);

ipr_config_locate (config, "/config/queues/queue", "");
ipr_config_locate (config, "/config/queues/queue", "test");
assert (streq (ipr_config_value (config), "This is a test queue"));
assert (streq (ipr_config_get (config, "overflow", "?"), "2000"));

ipr_config_destroy (&config);
}
/*  -------------------------------------------------------------------------
    ipr_config_terminate

    Type: Component method
    -------------------------------------------------------------------------
 */

void
    ipr_config_terminate (
void)
{


}
/*  -------------------------------------------------------------------------
    ipr_config_show

    Type: Component method
    -------------------------------------------------------------------------
 */

void
    ipr_config_show_ (
    void * item,                        //  The opaque pointer
    int opcode,                         //  The callback opcode
    FILE * trace_file,                  //  File to print to
    char * file,                        //  Source file
    size_t line                         //  Line number
)
{
ipr_config_t
    *self;
int
    container_links;


self = item;
container_links = 0;
assert (opcode == ICL_CALLBACK_DUMP);

fprintf (trace_file, "    <ipr_config file = \"%s\" line = \"%lu\"  pointer = \"%p\" />\n", file, (unsigned long) line, self);

}
/*  -------------------------------------------------------------------------
    ipr_config_destroy

    Type: Component method
    Destroys a ipr_config_t object. Takes the address of a
    ipr_config_t reference (a pointer to a pointer) and nullifies the
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
    ipr_config_destroy_ (
    ipr_config_t * ( * self_p ),        //  Reference to object reference
    char * file,                        //  Source fileSource file
    size_t line                         //  Line numberLine number
)
{
    ipr_config_t *
        self = *self_p;                 //  Dereferenced Reference to object reference

if (self) {
    ipr_config_annihilate (self_p);
    ipr_config_free ((ipr_config_t *) self);
    *self_p = NULL;
}
}
/*  -------------------------------------------------------------------------
    ipr_config_alloc

    Type: Component method
    -------------------------------------------------------------------------
 */

static ipr_config_t *
    ipr_config_alloc_ (
    char * file,                        //  Source file for call
    size_t line                         //  Line number for call
)
{

    ipr_config_t *
        self = NULL;                    //  Object reference

//  Initialise cache if necessary
if (!s_cache)
    ipr_config_cache_initialise ();

self = (ipr_config_t *) icl_mem_cache_alloc_ (s_cache, file, line);
if (self)
    memset (self, 0, sizeof (ipr_config_t));



    return (self);
}
/*  -------------------------------------------------------------------------
    ipr_config_free

    Type: Component method
    Freess a ipr_config_t object.
    -------------------------------------------------------------------------
 */

static void
    ipr_config_free (
    ipr_config_t * self                 //  Object reference
)
{


if (self) {

#if (defined (BASE_THREADSAFE))
    if (self->rwlock)
        icl_rwlock_destroy (&self->rwlock);
#endif
        memset (&self->object_tag, 0, sizeof (ipr_config_t) - ((byte *) &self->object_tag - (byte *) self));
//        memset (self, 0, sizeof (ipr_config_t));
        self->object_tag = IPR_CONFIG_DEAD;
        icl_mem_free (self);
    }
    self = NULL;
}
/*  -------------------------------------------------------------------------
    ipr_config_read_lock

    Type: Component method
    Accepts a ipr_config_t reference and returns zero in case of success,
    1 in case of errors.
    -------------------------------------------------------------------------
 */

int
    ipr_config_read_lock (
    ipr_config_t * self                 //  Reference to object
)
{
    int
        rc = 0;                         //  Return code

IPR_CONFIG_ASSERT_SANE (self);

#if (defined (BASE_THREADSAFE))
    icl_rwlock_read_lock (self->rwlock);
#endif


    return (rc);
}
/*  -------------------------------------------------------------------------
    ipr_config_write_lock

    Type: Component method
    Accepts a ipr_config_t reference and returns zero in case of success,
    1 in case of errors.
    -------------------------------------------------------------------------
 */

int
    ipr_config_write_lock (
    ipr_config_t * self                 //  Reference to object
)
{
    int
        rc = 0;                         //  Return code

IPR_CONFIG_ASSERT_SANE (self);

#if (defined (BASE_THREADSAFE))
    icl_rwlock_write_lock (self->rwlock);
#endif


    return (rc);
}
/*  -------------------------------------------------------------------------
    ipr_config_unlock

    Type: Component method
    Accepts a ipr_config_t reference and returns zero in case of success,
    1 in case of errors.
    -------------------------------------------------------------------------
 */

int
    ipr_config_unlock (
    ipr_config_t * self                 //  Reference to object
)
{
    int
        rc = 0;                         //  Return code

IPR_CONFIG_ASSERT_SANE (self);

#if (defined (BASE_THREADSAFE))
    icl_rwlock_unlock (self->rwlock);
#endif



    return (rc);
}
/*  -------------------------------------------------------------------------
    ipr_config_cache_initialise

    Type: Component method
    Initialise the cache and register purge method with the meta-cache.
    -------------------------------------------------------------------------
 */

static void
    ipr_config_cache_initialise (
void)
{

s_cache = icl_cache_get (sizeof (ipr_config_t));
icl_system_register (ipr_config_cache_purge, ipr_config_cache_terminate);
}
/*  -------------------------------------------------------------------------
    ipr_config_cache_purge

    Type: Component method
    -------------------------------------------------------------------------
 */

static void
    ipr_config_cache_purge (
void)
{

icl_mem_cache_purge (s_cache);

}
/*  -------------------------------------------------------------------------
    ipr_config_cache_terminate

    Type: Component method
    -------------------------------------------------------------------------
 */

static void
    ipr_config_cache_terminate (
void)
{

s_cache = NULL;

}
/*  -------------------------------------------------------------------------
    ipr_config_show_animation

    Type: Component method
    Enables animation of the component. Animation is sent to stdout.
    To enable animation you must generate using the option -animate:1.
    -------------------------------------------------------------------------
 */

void
    ipr_config_show_animation (
    Bool enabled                        //  Are we enabling or disabling animation?
)
{

ipr_config_animating = enabled;
}
/*  -------------------------------------------------------------------------
    ipr_config_new_in_scope

    Type: Component method
    -------------------------------------------------------------------------
 */

void
    ipr_config_new_in_scope_ (
    ipr_config_t * * self_p,            //  Not documented
    icl_scope_t * _scope,               //  Not documented
    char * file,                        //  Source file for call
    size_t line,                        //  Line number for call
    char * filename,                    //  Name of configuration file
    Bool required                       //  Complain if file not found
)
{
    icl_destroy_t *
        _destroy;                       //  Not documented

*self_p = ipr_config_new_ (file,line,filename,required);

if (*self_p) {
    _destroy = icl_destroy_new   ((void * *) self_p, (icl_destructor_fn *) ipr_config_destroy_);
    icl_destroy_list_queue (_scope, _destroy);
    icl_destroy_unlink (&_destroy);
}
}
static void
s_merge_trees (ipr_xml_t *xml_dest, ipr_xml_t *xml_source)
{
    ipr_xml_t
        *xml_item,                      //  Current item in our new tree
        *xml_item_link,                 //  Extra link to xml_item
        *xml_into,                      //  Destination item in config
        *xml_next;                      //  Next item on list
    ipr_xml_attr_t
        *xml_attr,                      //  Attribute of our current item
        *item_attr,                     //  First attribute of target item
        *into_attr;                     //  First attribute of destination item
    char
        *item_name,
        *into_name;
    Bool
        discrete,                       //  Replace children, don't merge
        processed;

    //  Check config_meta options
    discrete = streq (ipr_xml_attr_get (xml_dest, "config_meta", ""), "discrete");

    //  Merge attributes of source into dest
    IPR_FOR_XML_ATTRIBUTES (xml_attr, xml_source)
        ipr_xml_attr_set (xml_dest, ipr_xml_attr_name (xml_attr), ipr_xml_attr_value (xml_attr));

    //  Process each child item of xml_source
    xml_item = ipr_xml_first_child (xml_source);
    while (xml_item) {
        if (ipr_xml_name (xml_item)) {
            processed = FALSE;
            //  a) item is named, look for matching named item
            //  b) item is unnamed, merge only if both have no attributes
            item_name = ipr_xml_attr_get (xml_item, "name", NULL);
            item_attr = ipr_xml_attr_first (xml_item);
            IPR_FOR_XML_CHILDREN (xml_into, xml_dest) {
                if (streq (ipr_xml_name (xml_into), ipr_xml_name (xml_item))) {
                    into_name = ipr_xml_attr_get (xml_into, "name", NULL);
                    into_attr = ipr_xml_attr_first (xml_into);
                    //  Skip config_meta tag, on destination only
                    if (into_attr && streq (ipr_xml_attr_name (into_attr), "config_meta"))
                        into_attr = ipr_xml_attr_next (&into_attr);

                    if ((item_name && into_name && streq (into_name, item_name))
                    || (!item_name && !item_attr && !into_name && !into_attr)) {
                        //  We have a match, so now replace or merge item
                        if (discrete) {
                            xml_item_link = ipr_xml_link (xml_item);
                            ipr_xml_detach (xml_item);
                            ipr_xml_attach_sibling (xml_into, xml_item);
                            ipr_xml_unlink (&xml_item_link); //  Remove the extra link
                            xml_item = ipr_xml_next_sibling (&xml_item);
                            ipr_xml_destroy (&xml_into);
                        }
                        else
                            s_merge_trees (xml_into, xml_item);
                        processed = TRUE;
                    }
                    ipr_xml_attr_unlink (&into_attr);
                    if (processed) {
                        ipr_xml_unlink (&xml_into);
                        break;
                    }
                }
            }
            //  Move current item to destination if needed, and do next
            xml_item_link = ipr_xml_link (xml_item);
            xml_next = ipr_xml_next_sibling (&xml_item_link);
            if (!processed) {
                ipr_xml_detach (xml_item);
                ipr_xml_attach_child (xml_dest, xml_item);
            }
            ipr_xml_attr_unlink (&item_attr);
            ipr_xml_unlink (&xml_item);    //  Remove the extra link
            xml_item = xml_next;
        }
        else
            xml_item = ipr_xml_next_sibling (&xml_item);
    }
}

//  Embed the version information in the resulting binary

char *EMBED__ipr_config_version_start     = "VeRsIoNsTaRt:ipc";
char *EMBED__ipr_config_component         = "ipr_config ";
char *EMBED__ipr_config_version           = "1.1 ";
char *EMBED__ipr_config_copyright         = "Copyright (c) 1996-2009 iMatix Corporation";
char *EMBED__ipr_config_filename          = "ipr_config.icl ";
char *EMBED__ipr_config_builddate         = "2011/03/01 ";
char *EMBED__ipr_config_version_end       = "VeRsIoNeNd:ipc";

