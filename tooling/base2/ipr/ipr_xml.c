/*---------------------------------------------------------------------------
    ipr_xml.c - ipr_xml component

    Implements XML objects.
    Generated from ipr_xml.icl by icl_gen using GSL/4.
    
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
#include "ipr_xml.h"                    //  Definitions for our class

//  Shorthand for class type

#define self_t              ipr_xml_t

//  Shorthands for class methods

#define self_new            ipr_xml_new
#define self_annihilate     ipr_xml_annihilate
#define self_parse_file     ipr_xml_parse_file
#define self_parse_string   ipr_xml_parse_string
#define self_parse_bucket   ipr_xml_parse_bucket
#define self_attach_child   ipr_xml_attach_child
#define self_attach_sibling  ipr_xml_attach_sibling
#define self_detach         ipr_xml_detach
#define self_first_child    ipr_xml_first_child
#define self_next_sibling   ipr_xml_next_sibling
#define self_parent         ipr_xml_parent
#define self_name           ipr_xml_name
#define self_value          ipr_xml_value
#define self_rename         ipr_xml_rename
#define self_value_set      ipr_xml_value_set
#define self_text           ipr_xml_text
#define self_find_item      ipr_xml_find_item
#define self_next_item      ipr_xml_next_item
#define self_attr           ipr_xml_attr
#define self_selftest       ipr_xml_selftest
#define self_remove_from_all_containers  ipr_xml_remove_from_all_containers
#define self_show           ipr_xml_show
#define self_terminate      ipr_xml_terminate
#define self_destroy        ipr_xml_destroy
#define self_alloc          ipr_xml_alloc
#define self_free           ipr_xml_free
#define self_read_lock      ipr_xml_read_lock
#define self_write_lock     ipr_xml_write_lock
#define self_unlock         ipr_xml_unlock
#define self_link           ipr_xml_link
#define self_unlink         ipr_xml_unlink
#define self_cache_initialise  ipr_xml_cache_initialise
#define self_cache_purge    ipr_xml_cache_purge
#define self_cache_terminate  ipr_xml_cache_terminate
#define self_show_animation  ipr_xml_show_animation
#define self_new_in_scope   ipr_xml_new_in_scope

#define ipr_xml_annihilate(self)        ipr_xml_annihilate_ (self, __FILE__, __LINE__)
static void
    ipr_xml_annihilate_ (
ipr_xml_t * ( * self_p ),               //  Reference to object reference
char * file,                            //  Source file
size_t line                             //  Line number
);

#define ipr_xml_alloc()                 ipr_xml_alloc_ (__FILE__, __LINE__)
static ipr_xml_t *
    ipr_xml_alloc_ (
char * file,                            //  Source file for call
size_t line                             //  Line number for call
);

#define ipr_xml_free(self)              ipr_xml_free_ (self, __FILE__, __LINE__)
static void
    ipr_xml_free_ (
ipr_xml_t * self,                       //  Object reference
char * file,                            //  Source file
size_t line                             //  Line number
);

static void
    ipr_xml_cache_initialise (
void);

static void
    ipr_xml_cache_purge (
void);

static void
    ipr_xml_cache_terminate (
void);

Bool
    ipr_xml_animating = TRUE;           //  Animation enabled by default
static icl_cache_t
    *s_cache = NULL;


#include "ipr_bucket.h"
#include "ipr_xmll.h"
/*  -------------------------------------------------------------------------
    ipr_xml_new

    Type: Component method
    Creates and initialises a new ipr_xml_t object, returns a
    reference to the created object.
    Initialises the new item to be a valid list item.
    -------------------------------------------------------------------------
 */

ipr_xml_t *
    ipr_xml_new_ (
    char * file,                        //  Source file for call
    size_t line,                        //  Line number for call
    ipr_xml_t * parent,                 //  The parent of the new item
    char * name,                        //  The name of the new item
    char * value                        //  The value of the new item
)
{
    ipr_xml_t *
        self = NULL;                    //  Object reference

    self = ipr_xml_alloc_ (file, line);
    if (self) {
        self->object_tag   = IPR_XML_ALIVE;
        self->links        = 1;
        self->zombie       = FALSE;
#if defined (DEBUG) || defined (BASE_HISTORY) || defined (BASE_HISTORY_IPR_XML)
        self->history_last = 0;

        //  Track possession operation in history
        self->history_file  [0] = file;
        self->history_line  [0] = line;
        self->history_type  [0] = "new";
        self->history_links [0] = self->links;
#endif
#if (defined (BASE_THREADSAFE))
        self->rwlock = icl_rwlock_new ();
#endif
#if defined (DEBUG)
        icl_mem_set_callback (self, ipr_xml_show_);
#endif

self->list_next = self;
self->list_prev = self;
self->list_head = NULL;
if (name)
    self->name  = icl_mem_strdup (name);
if (value)
    self->value = icl_mem_strdup (value);
self->children  = ipr_xml_list_new ();
self->attrs     = ipr_xml_attr_list_new ();
if (parent)
    ipr_xml_attach_child (parent, self);
}

    return (self);
}
/*  -------------------------------------------------------------------------
    ipr_xml_annihilate

    Type: Component method
    -------------------------------------------------------------------------
 */

static void
    ipr_xml_annihilate_ (
    ipr_xml_t * ( * self_p ),           //  Reference to object reference
    char * file,                        //  Source file
    size_t line                         //  Line number
)
{
#if (defined (BASE_THREADSAFE))
    icl_rwlock_t
        *rwlock;
#endif
#if defined (DEBUG) || defined (BASE_HISTORY) || defined (BASE_HISTORY_IPR_XML)
    int
        history_last;
#endif

    ipr_xml_t *
        self = *self_p;                 //  Dereferenced Reference to object reference

#if defined (DEBUG) || defined (BASE_HISTORY) || defined (BASE_HISTORY_IPR_XML)
    //  Track possession operation in history
    history_last = icl_atomic_inc32 ((volatile qbyte *) &self->history_last) + 1;
    self->history_file  [history_last % IPR_XML_HISTORY_LENGTH] = file;
    self->history_line  [history_last % IPR_XML_HISTORY_LENGTH] = line;
    self->history_type  [history_last % IPR_XML_HISTORY_LENGTH] = "destroy";
    self->history_links [history_last % IPR_XML_HISTORY_LENGTH] = self->links;
#endif

    IPR_XML_ASSERT_SANE (self);
    ipr_xml_remove_from_all_containers (self);
#if (defined (BASE_THREADSAFE))
    rwlock = self->rwlock;
    if (rwlock)
         icl_rwlock_write_lock (rwlock);
#endif

icl_mem_free (self->name);
icl_mem_free (self->value);
ipr_xml_list_destroy      (&self->children);
ipr_xml_attr_list_destroy (&self->attrs);
#if (defined (BASE_THREADSAFE))
    if (rwlock)
        icl_rwlock_unlock (rwlock);
#endif

}
/*  -------------------------------------------------------------------------
    ipr_xml_parse_file

    Type: Component method
    -------------------------------------------------------------------------
 */

ipr_xml_t *
    ipr_xml_parse_file (
    char * path,                        //  Not documented
    char * filename                     //  Not documented
)
{
    ipr_xml_t *
        self;                           //  Not documented

//
self = NULL;
if (ipr_xml_load_file (&self, path, filename, FALSE))
    self = self_new (NULL, "", NULL);

    return (self);
}
/*  -------------------------------------------------------------------------
    ipr_xml_parse_string

    Type: Component method
    -------------------------------------------------------------------------
 */

ipr_xml_t *
    ipr_xml_parse_string (
    char * string                       //  Not documented
)
{
    ipr_xml_t *
        self;                           //  Not documented

//
self = NULL;
if (ipr_xml_load_string (&self, string, FALSE))
    self = self_new (NULL, "", NULL);

    return (self);
}
/*  -------------------------------------------------------------------------
    ipr_xml_parse_bucket

    Type: Component method
    -------------------------------------------------------------------------
 */

ipr_xml_t *
    ipr_xml_parse_bucket (
    ipr_bucket_t * bucket               //  Not documented
)
{
    ipr_xml_t *
        self;                           //  Not documented

//
self = NULL;
if (bucket && ipr_xml_load_bucket (&self, bucket, FALSE))
    self = self_new (NULL, "", NULL);

    return (self);
}
/*  -------------------------------------------------------------------------
    ipr_xml_attach_child

    Type: Component method
    -------------------------------------------------------------------------
 */

void
    ipr_xml_attach_child (
    ipr_xml_t * parent,                 //  The new parent of the item
    ipr_xml_t * self                    //  Not documented
)
{

if (self->parent)
    ipr_xml_list_remove (self);
self->parent = parent;
ipr_xml_list_queue (parent->children, self);
}
/*  -------------------------------------------------------------------------
    ipr_xml_attach_sibling

    Type: Component method
    -------------------------------------------------------------------------
 */

void
    ipr_xml_attach_sibling (
    ipr_xml_t * sibling,                //  Item inserted before the sibling
    ipr_xml_t * self                    //  The item
)
{
ipr_xml_t
    *sibling_link,
    *parent_link;

if (self->parent)
    ipr_xml_list_remove (self);

sibling_link = ipr_xml_link (sibling);
self->parent = ipr_xml_parent (&sibling_link);
ipr_xml_list_relink_before (self, sibling);

//  A bit messy.  We don't keep a link for our parent.
parent_link = self->parent;
ipr_xml_unlink (&parent_link);
}
/*  -------------------------------------------------------------------------
    ipr_xml_detach

    Type: Component method
    -------------------------------------------------------------------------
 */

void
    ipr_xml_detach (
    ipr_xml_t * self                    //  The item
)
{

if (self->parent)
    ipr_xml_list_remove (self);
self->parent = NULL;
}
/*  -------------------------------------------------------------------------
    ipr_xml_first_child

    Type: Component method
    -------------------------------------------------------------------------
 */

ipr_xml_t *
    ipr_xml_first_child (
    ipr_xml_t * self                    //  Not documented
)
{
    ipr_xml_t *
        child;                          //  Not documented

child = ipr_xml_list_first (self->children);

    return (child);
}
/*  -------------------------------------------------------------------------
    ipr_xml_next_sibling

    Type: Component method
    -------------------------------------------------------------------------
 */

ipr_xml_t *
    ipr_xml_next_sibling (
    ipr_xml_t ** pself                  //  Not documented
)
{
    ipr_xml_t *
        sibling;                        //  Not documented

sibling = ipr_xml_list_next (pself);

    return (sibling);
}
/*  -------------------------------------------------------------------------
    ipr_xml_parent

    Type: Component method
    -------------------------------------------------------------------------
 */

ipr_xml_t *
    ipr_xml_parent (
    ipr_xml_t ** pself                  //  Not documented
)
{
    ipr_xml_t *
        parent;                         //  Not documented

parent = (*pself)->parent;
ipr_xml_link (parent);
ipr_xml_unlink (pself);

    return (parent);
}
/*  -------------------------------------------------------------------------
    ipr_xml_name

    Type: Component method
    -------------------------------------------------------------------------
 */

char *
    ipr_xml_name (
    ipr_xml_t * self                    //  Not documented
)
{
    char *
        name;                           //  Not documented

name = self->name;

    return (name);
}
/*  -------------------------------------------------------------------------
    ipr_xml_value

    Type: Component method
    Returns the value for a node.  Do not use to get the value for an
    element, use text() for that.
    -------------------------------------------------------------------------
 */

char *
    ipr_xml_value (
    ipr_xml_t * self                    //  Not documented
)
{
    char *
        value;                          //  Not documented

value = self->value;

    return (value);
}
/*  -------------------------------------------------------------------------
    ipr_xml_rename

    Type: Component method
    -------------------------------------------------------------------------
 */

void
    ipr_xml_rename (
    ipr_xml_t * self,                   //  Not documented
    char * name                         //  Not documented
)
{

icl_mem_strfree (&self->name);
if (name)
    self->name=icl_mem_strdup (name);
}
/*  -------------------------------------------------------------------------
    ipr_xml_value_set

    Type: Component method
    Sets the value for a node.  Do not use to set the value for an element,
    but create a child node with no name, and set the value on the child
    node.
    -------------------------------------------------------------------------
 */

void
    ipr_xml_value_set (
    ipr_xml_t * self,                   //  Not documented
    char * value                        //  Not documented
)
{

if (!self->value) {
    if (value)
        self->value = icl_mem_strdup (value);
}
else
if (value) {
    if (strneq (value, self->value)) {
        icl_mem_strfree (&self->value);
        self->value = icl_mem_strdup (value);
    }
}
else
    icl_mem_strfree (&self->value);
}
/*  -------------------------------------------------------------------------
    ipr_xml_text

    Type: Component method
    -------------------------------------------------------------------------
 */

char *
    ipr_xml_text (
    ipr_xml_t * self                    //  Not documented
)
{
ipr_xml_t
    *child;
long
    length;
char
    *childvalue,
    *target,
    *source;
    char *
        value = NULL;                   //  Not documented

//
length = 0;
IPR_FOR_XML_VALUES (child, self) {
    childvalue = ipr_xml_value (child);
    if (childvalue)
        length += strlen (childvalue);
}
if (length > 0) {
    value = icl_mem_alloc (length + 1);
    if (value != NULL) {
        target = value;
        IPR_FOR_XML_VALUES (child, self) {
            source = ipr_xml_value (child);
            if (source) {
                while (*source)
                   *target++ = *source++;
            }
        }
        *target  = 0;
    }
}

    return (value);
}
/*  -------------------------------------------------------------------------
    ipr_xml_find_item

    Type: Component method
    Locates a child item according to a path, which is name/name/name.
    Returns the item if found, else null.
    -------------------------------------------------------------------------
 */

ipr_xml_t *
    ipr_xml_find_item (
    ipr_xml_t * xml_root,               //  Not documented
    char * p_path                       //  Not documented
)
{
ipr_xml_t
    *xml_item,
    *xml_child;
char
    *path,
    *cur_path,
    *next_path;
    ipr_xml_t *
        xml_found;                      //  Not documented

//
path = icl_mem_strdup ((byte *) p_path);
cur_path = path;
xml_item = xml_root;
xml_child = NULL;
xml_found = NULL;

/*  Traverse the XML Tree, starting at xml_root passed to us, keeping
 *  track of our current position in the path passed to us and in the
 *  XML Tree.  Stop when we have run out of either.
 */
while (cur_path) {
    next_path = strchr (cur_path, '/');
    if (next_path)
        *next_path++ = 0;

    IPR_FOR_XML_CHILDREN (xml_child, xml_item) {
        if (strcmp (ipr_xml_name (xml_child), cur_path) == 0) {
            if (next_path == NULL) {
                xml_found = xml_child;
                break;
            }
            else
                xml_item = xml_child;
        }
    }
    cur_path = next_path;
}
icl_mem_strfree (&path);

    return (xml_found);
}
/*  -------------------------------------------------------------------------
    ipr_xml_next_item

    Type: Component method
    Returns the next sibling with the same item name as the provided
    item.  Use to walk through a collection of elements and process all
    those of a specific type.
    -------------------------------------------------------------------------
 */

ipr_xml_t *
    ipr_xml_next_item (
    ipr_xml_t ** pself                  //  Not documented
)
{
char
    *item_name;
    ipr_xml_t *
        sibling;                        //  Not documented

//
assert (pself && *pself);
item_name = ipr_xml_name (*pself);
sibling = ipr_xml_list_next (pself);
while (sibling && strneq (ipr_xml_name (sibling), item_name))
    sibling = ipr_xml_list_next (&sibling);

    return (sibling);
}
/*  -------------------------------------------------------------------------
    ipr_xml_attr

    Type: Component method
    -------------------------------------------------------------------------
 */

ipr_xml_attr_t*
    ipr_xml_attr (
    ipr_xml_t* item,                    //  Not documented
    const char* name                    //  Not documented
)
{
    ipr_xml_attr_t*
        attr = NULL;                    //  Not documented

//
assert (item);
assert (name);
IPR_FOR_XML_ATTRIBUTES (attr, item)
    if (attr-> name? strcmp (attr-> name, name) == 0: FALSE)
        break;

    return (attr);
}
/*  -------------------------------------------------------------------------
    ipr_xml_selftest

    Type: Component method
    -------------------------------------------------------------------------
 */

void
    ipr_xml_selftest (
void)
{
ipr_xml_t
    *item,
    *child;
char
    *value;
int
    rc;

    //
    item = ipr_xml_new (NULL, "item", NULL);
    value = ipr_xml_attr_get (item, "attr", NULL);
    assert (value == NULL);
    ipr_xml_attr_set (item, "attr", "value");
    value = ipr_xml_attr_get (item, "attr", "");
    assert (streq (value, "value"));

    child = ipr_xml_new (item, "child", NULL);
    ipr_xml_value_set (child, "New value for child");
    child = ipr_xml_parent (&child);
    assert (child == item);
    ipr_xml_unlink (&child);

    child = ipr_xml_first_child (item);
    assert (streq (ipr_xml_value (child), "New value for child"));
    ipr_xml_unlink (&child);
    ipr_xml_destroy (&item);

    ipr_xml_load_string (&item, "<restms><feed name=\"test.fanout\"/>", FALSE);
    ipr_xml_destroy (&item);

    value =
"<message address=\"address.multi.1\" message_id=\"ID1\">" //
"    <header name=\"serial-number\" value=\"0001\" />" //
"    <content>It's too cold</content>" //
"</message>" //
"<message address=\"address.multi.2\" message_id=\"ID2\">" //
"    <header name=\"serial-number\" value=\"0002\" />" //
"    <content>It's too hot</content>" //
"</message>" //
"";
    rc = ipr_xml_load_string (&item, value, FALSE);
    icl_console_print ("RC=%d %s", rc, ipr_xml_error ());
    ipr_xml_destroy (&item);
}
/*  -------------------------------------------------------------------------
    ipr_xml_remove_from_all_containers

    Type: Component method
    The method to call to remove an item from its container.  Is called by
    the 'destroy' method if the possession count hits zero.
    -------------------------------------------------------------------------
 */

void
    ipr_xml_remove_from_all_containers (
    ipr_xml_t * self                    //  The item
)
{

ipr_xml_list_remove (self);
}
/*  -------------------------------------------------------------------------
    ipr_xml_show

    Type: Component method
    -------------------------------------------------------------------------
 */

void
    ipr_xml_show_ (
    void * item,                        //  The opaque pointer
    int opcode,                         //  The callback opcode
    FILE * trace_file,                  //  File to print to
    char * file,                        //  Source file
    size_t line                         //  Line number
)
{
    ipr_xml_t
        *self;
    int
        container_links;
#if defined (DEBUG) || defined (BASE_HISTORY) || defined (BASE_HISTORY_IPR_XML)
    qbyte
        history_index;
#endif


self = item;
container_links = 0;
if (self->list_head)
   container_links++;
    assert (opcode == ICL_CALLBACK_DUMP);

    fprintf (trace_file, "    <ipr_xml zombie = \"%u\" links = \"%u\" containers = \"%u\" file = \"%s\" line = \"%lu\"  pointer = \"%p\" />\n", self->zombie, self->links, container_links, file, (unsigned long) line, self);
#if defined (DEBUG) || defined (BASE_HISTORY) || defined (BASE_HISTORY_IPR_XML)
    if (self->history_last > IPR_XML_HISTORY_LENGTH) {
        fprintf (trace_file, "        <!-- possess history too large (%d) - call iMatix-tech -->\n",
            self->history_last);
        history_index = (self->history_last + 1) % IPR_XML_HISTORY_LENGTH;
        self->history_last %= IPR_XML_HISTORY_LENGTH;
    }
    else
        history_index = 0;

    for (; history_index != self->history_last; history_index = (history_index + 1) % IPR_XML_HISTORY_LENGTH) {
        fprintf (trace_file, "       <%s file = \"%s\" line = \"%lu\" links = \"%lu\" />\n",
            self->history_type [history_index],
            self->history_file [history_index],
            (unsigned long) self->history_line  [history_index],
            (unsigned long) self->history_links [history_index]);
    }
    fprintf (trace_file, "    </ipr_xml>\n");
#endif

}
/*  -------------------------------------------------------------------------
    ipr_xml_terminate

    Type: Component method
    -------------------------------------------------------------------------
 */

void
    ipr_xml_terminate (
void)
{


}
/*  -------------------------------------------------------------------------
    ipr_xml_destroy

    Type: Component method
    Destroys a ipr_xml_t object. Takes the address of a
    ipr_xml_t reference (a pointer to a pointer) and nullifies the
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
    ipr_xml_destroy_ (
    ipr_xml_t * ( * self_p ),           //  Reference to object reference
    char * file,                        //  Source fileSource file
    size_t line                         //  Line numberLine number
)
{
    ipr_xml_t *
        self = *self_p;                 //  Dereferenced Reference to object reference

if (self) {
    if (icl_atomic_cas32 (&self->zombie, TRUE, FALSE) == FALSE)
        ipr_xml_annihilate_ (self_p, file, line);

    if (self->links == 0) {
        icl_system_panic ("ipr_xml", "E: missing link on ipr_xml object");
        ipr_xml_show_ (self, ICL_CALLBACK_DUMP, stderr, file, line);
        abort ();
    }

    if (icl_atomic_dec32 ((volatile qbyte *) &self->links) == 0)
        ipr_xml_free_ ((ipr_xml_t *) self, file, line);
    *self_p = NULL;
}
}
/*  -------------------------------------------------------------------------
    ipr_xml_alloc

    Type: Component method
    -------------------------------------------------------------------------
 */

static ipr_xml_t *
    ipr_xml_alloc_ (
    char * file,                        //  Source file for call
    size_t line                         //  Line number for call
)
{

    ipr_xml_t *
        self = NULL;                    //  Object reference

//  Initialise cache if necessary
if (!s_cache)
    ipr_xml_cache_initialise ();

self = (ipr_xml_t *) icl_mem_cache_alloc_ (s_cache, file, line);
if (self)
    memset (self, 0, sizeof (ipr_xml_t));



    return (self);
}
/*  -------------------------------------------------------------------------
    ipr_xml_free

    Type: Component method
    Freess a ipr_xml_t object.
    -------------------------------------------------------------------------
 */

static void
    ipr_xml_free_ (
    ipr_xml_t * self,                   //  Object reference
    char * file,                        //  Source file
    size_t line                         //  Line number
)
{
#if defined (DEBUG) || defined (BASE_HISTORY) || defined (BASE_HISTORY_IPR_XML)
    int
        history_last;
#endif


    if (self) {
#if defined (DEBUG) || defined (BASE_HISTORY) || defined (BASE_HISTORY_IPR_XML)
        //  Track possession operation in history
        history_last = icl_atomic_inc32 ((volatile qbyte *) &self->history_last) + 1;
        self->history_file  [history_last % IPR_XML_HISTORY_LENGTH] = file;
        self->history_line  [history_last % IPR_XML_HISTORY_LENGTH] = line;
        self->history_type  [history_last % IPR_XML_HISTORY_LENGTH] = "free";
        self->history_links [history_last % IPR_XML_HISTORY_LENGTH] = self->links;
#endif

#if (defined (BASE_THREADSAFE))
    if (self->rwlock)
        icl_rwlock_destroy (&self->rwlock);
#endif
        memset (&self->object_tag, 0, sizeof (ipr_xml_t) - ((byte *) &self->object_tag - (byte *) self));
//        memset (self, 0, sizeof (ipr_xml_t));
        self->object_tag = IPR_XML_DEAD;
        icl_mem_free (self);
    }
    self = NULL;
}
/*  -------------------------------------------------------------------------
    ipr_xml_read_lock

    Type: Component method
    Accepts a ipr_xml_t reference and returns zero in case of success,
    1 in case of errors.
    -------------------------------------------------------------------------
 */

int
    ipr_xml_read_lock (
    ipr_xml_t * self                    //  Reference to object
)
{
    int
        rc = 0;                         //  Return code

IPR_XML_ASSERT_SANE (self);
if (!self->zombie) {

#if (defined (BASE_THREADSAFE))
    icl_rwlock_read_lock (self->rwlock);
#endif
}
else
    rc = -1;                        //  Return error on zombie object.


    return (rc);
}
/*  -------------------------------------------------------------------------
    ipr_xml_write_lock

    Type: Component method
    Accepts a ipr_xml_t reference and returns zero in case of success,
    1 in case of errors.
    -------------------------------------------------------------------------
 */

int
    ipr_xml_write_lock (
    ipr_xml_t * self                    //  Reference to object
)
{
    int
        rc = 0;                         //  Return code

IPR_XML_ASSERT_SANE (self);
if (!self->zombie) {

#if (defined (BASE_THREADSAFE))
    icl_rwlock_write_lock (self->rwlock);
#endif
}
else
    rc = -1;                        //  Return error on zombie object.


    return (rc);
}
/*  -------------------------------------------------------------------------
    ipr_xml_unlock

    Type: Component method
    Accepts a ipr_xml_t reference and returns zero in case of success,
    1 in case of errors.
    -------------------------------------------------------------------------
 */

int
    ipr_xml_unlock (
    ipr_xml_t * self                    //  Reference to object
)
{
    int
        rc = 0;                         //  Return code

IPR_XML_ASSERT_SANE (self);
if (!self->zombie) {

#if (defined (BASE_THREADSAFE))
    icl_rwlock_unlock (self->rwlock);
#endif

}
else
    rc = -1;                        //  Return error on zombie object.


    return (rc);
}
/*  -------------------------------------------------------------------------
    ipr_xml_link

    Type: Component method
    Adds a link (reference count) to an object.

    If the object has been zombified (ie destroyed while extra links were present),
    this method returns NULL and does not add any links.

    This method does not lock the object.
    -------------------------------------------------------------------------
 */

ipr_xml_t *
    ipr_xml_link_ (
    ipr_xml_t * self,                   //  Not documented
    char * file,                        //  Source file for call
    size_t line                         //  Line number for call
)
{
#if defined (DEBUG) || defined (BASE_HISTORY) || defined (BASE_HISTORY_IPR_XML)
    int
        history_last;
#endif

    if (self) {
        IPR_XML_ASSERT_SANE (self);
        if (file)
            icl_mem_possess_ (self, file, line);
        icl_atomic_inc32 ((volatile qbyte *) &self->links);
#if defined (DEBUG) || defined (BASE_HISTORY) || defined (BASE_HISTORY_IPR_XML)
        //  Track possession operation in history
        history_last = icl_atomic_inc32 ((volatile qbyte *) &self->history_last) + 1;
        self->history_file  [history_last % IPR_XML_HISTORY_LENGTH] = file;
        self->history_line  [history_last % IPR_XML_HISTORY_LENGTH] = line;
        self->history_type  [history_last % IPR_XML_HISTORY_LENGTH] = "link";
        self->history_links [history_last % IPR_XML_HISTORY_LENGTH] = self->links;
#endif
    }

    return (self);
}
/*  -------------------------------------------------------------------------
    ipr_xml_unlink

    Type: Component method
    Removes a link (reference count) to an object.  Sets the object pointer to NULL
    to indicate that it is no longer valid.
    -------------------------------------------------------------------------
 */

void
    ipr_xml_unlink_ (
    ipr_xml_t * ( * self_p ),           //  Reference to object reference
    char * file,                        //  Source file for call
    size_t line                         //  Line number for call
)
{
#if defined (DEBUG) || defined (BASE_HISTORY) || defined (BASE_HISTORY_IPR_XML)
    int
        history_last;
#endif

    ipr_xml_t *
        self = *self_p;                 //  Dereferenced Reference to object reference

    if (self) {
        IPR_XML_ASSERT_SANE (self);
        if (self->links == 0) {
            icl_system_panic ("ipr_xml", "E: missing link on ipr_xml object");
            ipr_xml_show_ (self, ICL_CALLBACK_DUMP, stderr, file, line);
            abort ();
        }

#if defined (DEBUG) || defined (BASE_HISTORY) || defined (BASE_HISTORY_IPR_XML)
        //  Track possession operation in history.  Pre-empt value of links
        //  after operation; otherwise race condition can result in writing
        //  to freed memory.
        history_last = icl_atomic_inc32 ((volatile qbyte *) &self->history_last) + 1;
        self->history_file  [history_last % IPR_XML_HISTORY_LENGTH] = file;
        self->history_line  [history_last % IPR_XML_HISTORY_LENGTH] = line;
        self->history_type  [history_last % IPR_XML_HISTORY_LENGTH] = "unlink";
        self->history_links [history_last % IPR_XML_HISTORY_LENGTH] = self->links - 1;
#endif
        if (icl_atomic_dec32 ((volatile qbyte *) &self->links) == 0) {

        if (icl_atomic_cas32 (&self->zombie, TRUE, FALSE) == FALSE)
            ipr_xml_annihilate_ (self_p, file, line);
        ipr_xml_free_ ((ipr_xml_t *) self, file, line);
    }
    *self_p = NULL;
}
}
/*  -------------------------------------------------------------------------
    ipr_xml_cache_initialise

    Type: Component method
    Initialise the cache and register purge method with the meta-cache.
    -------------------------------------------------------------------------
 */

static void
    ipr_xml_cache_initialise (
void)
{

s_cache = icl_cache_get (sizeof (ipr_xml_t));
icl_system_register (ipr_xml_cache_purge, ipr_xml_cache_terminate);
}
/*  -------------------------------------------------------------------------
    ipr_xml_cache_purge

    Type: Component method
    -------------------------------------------------------------------------
 */

static void
    ipr_xml_cache_purge (
void)
{

icl_mem_cache_purge (s_cache);

}
/*  -------------------------------------------------------------------------
    ipr_xml_cache_terminate

    Type: Component method
    -------------------------------------------------------------------------
 */

static void
    ipr_xml_cache_terminate (
void)
{

s_cache = NULL;

}
/*  -------------------------------------------------------------------------
    ipr_xml_show_animation

    Type: Component method
    Enables animation of the component. Animation is sent to stdout.
    To enable animation you must generate using the option -animate:1.
    -------------------------------------------------------------------------
 */

void
    ipr_xml_show_animation (
    Bool enabled                        //  Are we enabling or disabling animation?
)
{

ipr_xml_animating = enabled;
}
/*  -------------------------------------------------------------------------
    ipr_xml_new_in_scope

    Type: Component method
    -------------------------------------------------------------------------
 */

void
    ipr_xml_new_in_scope_ (
    ipr_xml_t * * self_p,               //  Not documented
    icl_scope_t * _scope,               //  Not documented
    char * file,                        //  Source file for call
    size_t line,                        //  Line number for call
    ipr_xml_t * parent,                 //  The parent of the new item
    char * name,                        //  The name of the new item
    char * value                        //  The value of the new item
)
{
    icl_destroy_t *
        _destroy;                       //  Not documented

*self_p = ipr_xml_new_ (file,line,parent,name,value);

if (*self_p) {
    _destroy = icl_destroy_new   ((void * *) self_p, (icl_destructor_fn *) ipr_xml_destroy_);
    icl_destroy_list_queue (_scope, _destroy);
    icl_destroy_unlink (&_destroy);
}
}
//  Embed the version information in the resulting binary

char *EMBED__ipr_xml_version_start        = "VeRsIoNsTaRt:ipc";
char *EMBED__ipr_xml_component            = "ipr_xml ";
char *EMBED__ipr_xml_version              = "1.1 ";
char *EMBED__ipr_xml_copyright            = "Copyright (c) 1996-2009 iMatix Corporation";
char *EMBED__ipr_xml_filename             = "ipr_xml.icl ";
char *EMBED__ipr_xml_builddate            = "2011/03/01 ";
char *EMBED__ipr_xml_version_end          = "VeRsIoNeNd:ipc";

