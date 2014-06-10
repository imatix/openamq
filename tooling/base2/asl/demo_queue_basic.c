/*---------------------------------------------------------------------------
    demo_queue_basic.c - demo_queue_basic component

    This class implements the basic content queue manager.
    Generated from demo_queue_basic.icl by icl_gen using GSL/4.
    
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
#include "demo_queue_basic.h"           //  Definitions for our class

//  Shorthand for class type                                                   

#define self_t              demo_queue_basic_t

//  Shorthands for class methods                                               

#define self_new            demo_queue_basic_new
#define self_annihilate     demo_queue_basic_annihilate
#define self_publish        demo_queue_basic_publish
#define self_get            demo_queue_basic_get
#define self_selftest       demo_queue_basic_selftest
#define self_terminate      demo_queue_basic_terminate
#define self_show           demo_queue_basic_show
#define self_destroy        demo_queue_basic_destroy
#define self_alloc          demo_queue_basic_alloc
#define self_free           demo_queue_basic_free
#define self_cache_initialise  demo_queue_basic_cache_initialise
#define self_cache_purge    demo_queue_basic_cache_purge
#define self_cache_terminate  demo_queue_basic_cache_terminate
#define self_animate        demo_queue_basic_animate
#define self_new_in_scope   demo_queue_basic_new_in_scope

#if (defined (BASE_STATS)  ||  defined (BASE_STATS_DEMO_QUEUE_BASIC)  ||  defined (BASE_STATS_DEMO_QUEUE_BASIC_NEW))
static icl_stats_t *s_demo_queue_basic_new_stats = NULL;
#endif
#if (defined (BASE_STATS)  ||  defined (BASE_STATS_DEMO_QUEUE_BASIC)  ||  defined (BASE_STATS_DEMO_QUEUE_BASIC_DESTROY))
static icl_stats_t *s_demo_queue_basic_annihilate_stats = NULL;
#endif
#if (defined (BASE_STATS)  ||  defined (BASE_STATS_DEMO_QUEUE_BASIC)  ||  defined (BASE_STATS_DEMO_QUEUE_BASIC_PUBLISH))
static icl_stats_t *s_demo_queue_basic_publish_stats = NULL;
#endif
#if (defined (BASE_STATS)  ||  defined (BASE_STATS_DEMO_QUEUE_BASIC)  ||  defined (BASE_STATS_DEMO_QUEUE_BASIC_GET))
static icl_stats_t *s_demo_queue_basic_get_stats = NULL;
#endif
#if (defined (BASE_STATS)  ||  defined (BASE_STATS_DEMO_QUEUE_BASIC)  ||  defined (BASE_STATS_DEMO_QUEUE_BASIC_SELFTEST))
static icl_stats_t *s_demo_queue_basic_selftest_stats = NULL;
#endif
#if (defined (BASE_STATS)  ||  defined (BASE_STATS_DEMO_QUEUE_BASIC)  ||  defined (BASE_STATS_DEMO_QUEUE_BASIC_TERMINATE))
static icl_stats_t *s_demo_queue_basic_terminate_stats = NULL;
#endif
#if (defined (BASE_STATS)  ||  defined (BASE_STATS_DEMO_QUEUE_BASIC)  ||  defined (BASE_STATS_DEMO_QUEUE_BASIC_SHOW))
static icl_stats_t *s_demo_queue_basic_show_stats = NULL;
#endif
#if (defined (BASE_STATS)  ||  defined (BASE_STATS_DEMO_QUEUE_BASIC)  ||  defined (BASE_STATS_DEMO_QUEUE_BASIC_DESTROY_PUBLIC))
static icl_stats_t *s_demo_queue_basic_destroy_stats = NULL;
#endif
#if (defined (BASE_STATS)  ||  defined (BASE_STATS_DEMO_QUEUE_BASIC)  ||  defined (BASE_STATS_DEMO_QUEUE_BASIC_ALLOC))
static icl_stats_t *s_demo_queue_basic_alloc_stats = NULL;
#endif
#if (defined (BASE_STATS)  ||  defined (BASE_STATS_DEMO_QUEUE_BASIC)  ||  defined (BASE_STATS_DEMO_QUEUE_BASIC_FREE))
static icl_stats_t *s_demo_queue_basic_free_stats = NULL;
#endif
#if (defined (BASE_STATS)  ||  defined (BASE_STATS_DEMO_QUEUE_BASIC)  ||  defined (BASE_STATS_DEMO_QUEUE_BASIC_CACHE_INITIALISE))
static icl_stats_t *s_demo_queue_basic_cache_initialise_stats = NULL;
#endif
#if (defined (BASE_STATS)  ||  defined (BASE_STATS_DEMO_QUEUE_BASIC)  ||  defined (BASE_STATS_DEMO_QUEUE_BASIC_CACHE_PURGE))
static icl_stats_t *s_demo_queue_basic_cache_purge_stats = NULL;
#endif
#if (defined (BASE_STATS)  ||  defined (BASE_STATS_DEMO_QUEUE_BASIC)  ||  defined (BASE_STATS_DEMO_QUEUE_BASIC_CACHE_TERMINATE))
static icl_stats_t *s_demo_queue_basic_cache_terminate_stats = NULL;
#endif
#if (defined (BASE_STATS)  ||  defined (BASE_STATS_DEMO_QUEUE_BASIC)  ||  defined (BASE_STATS_DEMO_QUEUE_BASIC_ANIMATE))
static icl_stats_t *s_demo_queue_basic_animate_stats = NULL;
#endif
#if (defined (BASE_STATS)  ||  defined (BASE_STATS_DEMO_QUEUE_BASIC)  ||  defined (BASE_STATS_DEMO_QUEUE_BASIC_NEW_IN_SCOPE))
static icl_stats_t *s_demo_queue_basic_new_in_scope_stats = NULL;
#endif
static void
    demo_queue_basic_annihilate (
demo_queue_basic_t * ( * self_p )       //  Reference to object reference
);

#define demo_queue_basic_alloc()        demo_queue_basic_alloc_ (__FILE__, __LINE__)
static demo_queue_basic_t *
    demo_queue_basic_alloc_ (
char * file,                            //  Source file for call
size_t line                             //  Line number for call
);

static void
    demo_queue_basic_free (
demo_queue_basic_t * self               //  Object reference
);

static void
    demo_queue_basic_cache_initialise (
void);

static void
    demo_queue_basic_cache_purge (
void);

static void
    demo_queue_basic_cache_terminate (
void);

Bool
    demo_queue_basic_animating = TRUE;  //  Animation enabled by default
static icl_cache_t
    *s_cache = NULL;


/*  -------------------------------------------------------------------------
    demo_queue_basic_new

    Type: Component method
    Creates and initialises a new demo_queue_basic_t object, returns a
    reference to the created object.
    -------------------------------------------------------------------------
 */

demo_queue_basic_t *
    demo_queue_basic_new_ (
    char * file,                        //  Source file for call
    size_t line                         //  Line number for call
)
{
    demo_queue_basic_t *
        self = NULL;                    //  Object reference

#if (defined (BASE_ANIMATE)  ||  defined (BASE_ANIMATE_DEMO_QUEUE_BASIC)  ||  defined (BASE_ANIMATE_DEMO_QUEUE_BASIC_NEW))
    if (demo_queue_basic_animating)
        icl_console_print ("<demo_queue_basic_new_start"
#if (defined (BASE_THREADSAFE))
" thread=\"%pp\""
#endif
" file=\"%s\""
" line=\"%u\""
"/>"
#if (defined (BASE_THREADSAFE))
, apr_os_thread_current ()
#endif
, file, line);
#endif

#if (defined (BASE_TRACE)   || defined (BASE_TRACE_DEMO_QUEUE_BASIC)   || defined (BASE_TRACE_DEMO_QUEUE_BASIC_NEW))
    icl_trace_record (NULL, demo_queue_basic_dump, 1);
#endif

#if (defined (BASE_STATS)  ||  defined (BASE_STATS_DEMO_QUEUE_BASIC)  ||  defined (BASE_STATS_DEMO_QUEUE_BASIC_NEW))
    icl_stats_inc ("demo_queue_basic_new", &s_demo_queue_basic_new_stats);
#endif

    self = demo_queue_basic_alloc_ (file, line);
    if (self) {
        self->object_tag   = DEMO_QUEUE_BASIC_ALIVE;
#if defined (DEBUG)
        icl_mem_set_callback (self, demo_queue_basic_show_);
#endif

self->content_list = demo_content_basic_list_new ();
}
#if (defined (BASE_TRACE)   || defined (BASE_TRACE_DEMO_QUEUE_BASIC)   || defined (BASE_TRACE_DEMO_QUEUE_BASIC_NEW))
    icl_trace_record (NULL, demo_queue_basic_dump, 0x10000 + 1);
#endif

#if (defined (BASE_ANIMATE)   || defined (BASE_ANIMATE_DEMO_QUEUE_BASIC)   || defined (BASE_ANIMATE_DEMO_QUEUE_BASIC_NEW))
    if (demo_queue_basic_animating)
        icl_console_print ("<demo_queue_basic_new_finish"
#if (defined (BASE_THREADSAFE))
" thread=\"%pp\""
#endif
" file=\"%s\""
" line=\"%u\""
" self=\"%pp\""
"/>"
#if (defined (BASE_THREADSAFE))
, apr_os_thread_current ()
#endif
, file, line, self);
#endif


    return (self);
}
/*  -------------------------------------------------------------------------
    demo_queue_basic_annihilate

    Type: Component method
    -------------------------------------------------------------------------
 */

static void
    demo_queue_basic_annihilate (
    demo_queue_basic_t * ( * self_p )   //  Reference to object reference
)
{

    demo_queue_basic_t *
        self = *self_p;                 //  Dereferenced Reference to object reference

#if (defined (BASE_ANIMATE)  ||  defined (BASE_ANIMATE_DEMO_QUEUE_BASIC)  ||  defined (BASE_ANIMATE_DEMO_QUEUE_BASIC_DESTROY))
    if (demo_queue_basic_animating)
        icl_console_print ("<demo_queue_basic_destroy_start"
#if (defined (BASE_THREADSAFE))
" thread=\"%pp\""
#endif
" self=\"%pp\""
" self=\"%pp\""
"/>"
#if (defined (BASE_THREADSAFE))
, apr_os_thread_current ()
#endif
, self, self);
#endif

#if (defined (BASE_TRACE)   || defined (BASE_TRACE_DEMO_QUEUE_BASIC)   || defined (BASE_TRACE_DEMO_QUEUE_BASIC_DESTROY))
    icl_trace_record (NULL, demo_queue_basic_dump, 2);
#endif

#if (defined (BASE_STATS)  ||  defined (BASE_STATS_DEMO_QUEUE_BASIC)  ||  defined (BASE_STATS_DEMO_QUEUE_BASIC_DESTROY))
    icl_stats_inc ("demo_queue_basic_annihilate", &s_demo_queue_basic_annihilate_stats);
#endif

DEMO_QUEUE_BASIC_ASSERT_SANE (self);

demo_content_basic_list_destroy (&self->content_list);

#if (defined (BASE_TRACE)   || defined (BASE_TRACE_DEMO_QUEUE_BASIC)   || defined (BASE_TRACE_DEMO_QUEUE_BASIC_DESTROY))
    icl_trace_record (NULL, demo_queue_basic_dump, 0x10000 + 2);
#endif

#if (defined (BASE_ANIMATE)   || defined (BASE_ANIMATE_DEMO_QUEUE_BASIC)   || defined (BASE_ANIMATE_DEMO_QUEUE_BASIC_DESTROY))
    if (demo_queue_basic_animating)
        icl_console_print ("<demo_queue_basic_destroy_finish"
#if (defined (BASE_THREADSAFE))
" thread=\"%pp\""
#endif
" self=\"%pp\""
" self=\"%pp\""
"/>"
#if (defined (BASE_THREADSAFE))
, apr_os_thread_current ()
#endif
, self, self);
#endif

}
/*  -------------------------------------------------------------------------
    demo_queue_basic_publish

    Type: Component method
    Accepts a demo_queue_basic_t reference and returns zero in case of success,
    1 in case of errors.
    Accept message content onto queue.
    -------------------------------------------------------------------------
 */

int
    demo_queue_basic_publish (
    demo_queue_basic_t * self,          //  Reference to object
    demo_content_basic_t * content      //  Message content
)
{
    int
        rc = 0;                         //  Return code

#if (defined (BASE_ANIMATE)  ||  defined (BASE_ANIMATE_DEMO_QUEUE_BASIC)  ||  defined (BASE_ANIMATE_DEMO_QUEUE_BASIC_PUBLISH))
    if (demo_queue_basic_animating)
        icl_console_print ("<demo_queue_basic_publish_start"
#if (defined (BASE_THREADSAFE))
" thread=\"%pp\""
#endif
" self=\"%pp\""
" content=\"%pp\""
"/>"
#if (defined (BASE_THREADSAFE))
, apr_os_thread_current ()
#endif
, self, content);
#endif

#if (defined (BASE_TRACE)   || defined (BASE_TRACE_DEMO_QUEUE_BASIC)   || defined (BASE_TRACE_DEMO_QUEUE_BASIC_PUBLISH))
    icl_trace_record (NULL, demo_queue_basic_dump, 3);
#endif

#if (defined (BASE_STATS)  ||  defined (BASE_STATS_DEMO_QUEUE_BASIC)  ||  defined (BASE_STATS_DEMO_QUEUE_BASIC_PUBLISH))
    icl_stats_inc ("demo_queue_basic_publish", &s_demo_queue_basic_publish_stats);
#endif

DEMO_QUEUE_BASIC_ASSERT_SANE (self);

demo_content_basic_list_queue (self->content_list, content);

#if (defined (BASE_TRACE)   || defined (BASE_TRACE_DEMO_QUEUE_BASIC)   || defined (BASE_TRACE_DEMO_QUEUE_BASIC_PUBLISH))
    icl_trace_record (NULL, demo_queue_basic_dump, 0x10000 + 3);
#endif

#if (defined (BASE_ANIMATE)   || defined (BASE_ANIMATE_DEMO_QUEUE_BASIC)   || defined (BASE_ANIMATE_DEMO_QUEUE_BASIC_PUBLISH))
    if (demo_queue_basic_animating)
        icl_console_print ("<demo_queue_basic_publish_finish"
#if (defined (BASE_THREADSAFE))
" thread=\"%pp\""
#endif
" self=\"%pp\""
" content=\"%pp\""
" rc=\"%i\""
"/>"
#if (defined (BASE_THREADSAFE))
, apr_os_thread_current ()
#endif
, self, content, rc);
#endif


    return (rc);
}
/*  -------------------------------------------------------------------------
    demo_queue_basic_get

    Type: Component method
    Accepts a demo_queue_basic_t reference and returns zero in case of success,
    1 in case of errors.
    Returns next message off queue, if any.
    -------------------------------------------------------------------------
 */

int
    demo_queue_basic_get (
    demo_queue_basic_t * self,          //  Reference to object
    demo_server_channel_t * channel     //  The channel in question
)
{
demo_content_basic_t
    *content;                       //  Content object reference
    int
        rc = 0;                         //  Return code

#if (defined (BASE_ANIMATE)  ||  defined (BASE_ANIMATE_DEMO_QUEUE_BASIC)  ||  defined (BASE_ANIMATE_DEMO_QUEUE_BASIC_GET))
    if (demo_queue_basic_animating)
        icl_console_print ("<demo_queue_basic_get_start"
#if (defined (BASE_THREADSAFE))
" thread=\"%pp\""
#endif
" self=\"%pp\""
" channel=\"%pp\""
"/>"
#if (defined (BASE_THREADSAFE))
, apr_os_thread_current ()
#endif
, self, channel);
#endif

#if (defined (BASE_TRACE)   || defined (BASE_TRACE_DEMO_QUEUE_BASIC)   || defined (BASE_TRACE_DEMO_QUEUE_BASIC_GET))
    icl_trace_record (NULL, demo_queue_basic_dump, 4);
#endif

#if (defined (BASE_STATS)  ||  defined (BASE_STATS_DEMO_QUEUE_BASIC)  ||  defined (BASE_STATS_DEMO_QUEUE_BASIC_GET))
    icl_stats_inc ("demo_queue_basic_get", &s_demo_queue_basic_get_stats);
#endif

DEMO_QUEUE_BASIC_ASSERT_SANE (self);

//  Get next message off list, if any
if (!demo_content_basic_list_isempty (self->content_list)) {

    content = demo_content_basic_list_pop (self->content_list);
    demo_server_agent_basic_get_ok (
        channel->connection->thread,
        channel->number,
        content,
        content->exchange,
        content->routing_key,
        demo_content_basic_list_count (self->content_list),
        NULL);
    demo_content_basic_unlink (&content);
}
else
    demo_server_agent_basic_get_empty (
        channel->connection->thread,
        channel->number);

#if (defined (BASE_TRACE)   || defined (BASE_TRACE_DEMO_QUEUE_BASIC)   || defined (BASE_TRACE_DEMO_QUEUE_BASIC_GET))
    icl_trace_record (NULL, demo_queue_basic_dump, 0x10000 + 4);
#endif

#if (defined (BASE_ANIMATE)   || defined (BASE_ANIMATE_DEMO_QUEUE_BASIC)   || defined (BASE_ANIMATE_DEMO_QUEUE_BASIC_GET))
    if (demo_queue_basic_animating)
        icl_console_print ("<demo_queue_basic_get_finish"
#if (defined (BASE_THREADSAFE))
" thread=\"%pp\""
#endif
" self=\"%pp\""
" channel=\"%pp\""
" rc=\"%i\""
"/>"
#if (defined (BASE_THREADSAFE))
, apr_os_thread_current ()
#endif
, self, channel, rc);
#endif


    return (rc);
}
/*  -------------------------------------------------------------------------
    demo_queue_basic_selftest

    Type: Component method
    -------------------------------------------------------------------------
 */

void
    demo_queue_basic_selftest (
void)
{

#if (defined (BASE_ANIMATE)  ||  defined (BASE_ANIMATE_DEMO_QUEUE_BASIC)  ||  defined (BASE_ANIMATE_DEMO_QUEUE_BASIC_SELFTEST))
    if (demo_queue_basic_animating)
        icl_console_print ("<demo_queue_basic_selftest_start"
#if (defined (BASE_THREADSAFE))
" thread=\"%pp\""
#endif
"/>"
#if (defined (BASE_THREADSAFE))
, apr_os_thread_current ()
#endif
);
#endif

#if (defined (BASE_TRACE)   || defined (BASE_TRACE_DEMO_QUEUE_BASIC)   || defined (BASE_TRACE_DEMO_QUEUE_BASIC_SELFTEST))
    icl_trace_record (NULL, demo_queue_basic_dump, 5);
#endif

#if (defined (BASE_STATS)  ||  defined (BASE_STATS_DEMO_QUEUE_BASIC)  ||  defined (BASE_STATS_DEMO_QUEUE_BASIC_SELFTEST))
    icl_stats_inc ("demo_queue_basic_selftest", &s_demo_queue_basic_selftest_stats);
#endif

#if (defined (BASE_TRACE)   || defined (BASE_TRACE_DEMO_QUEUE_BASIC)   || defined (BASE_TRACE_DEMO_QUEUE_BASIC_SELFTEST))
    icl_trace_record (NULL, demo_queue_basic_dump, 0x10000 + 5);
#endif

#if (defined (BASE_ANIMATE)   || defined (BASE_ANIMATE_DEMO_QUEUE_BASIC)   || defined (BASE_ANIMATE_DEMO_QUEUE_BASIC_SELFTEST))
    if (demo_queue_basic_animating)
        icl_console_print ("<demo_queue_basic_selftest_finish"
#if (defined (BASE_THREADSAFE))
" thread=\"%pp\""
#endif
"/>"
#if (defined (BASE_THREADSAFE))
, apr_os_thread_current ()
#endif
);
#endif

}
/*  -------------------------------------------------------------------------
    demo_queue_basic_terminate

    Type: Component method
    -------------------------------------------------------------------------
 */

void
    demo_queue_basic_terminate (
void)
{

#if (defined (BASE_ANIMATE)  ||  defined (BASE_ANIMATE_DEMO_QUEUE_BASIC)  ||  defined (BASE_ANIMATE_DEMO_QUEUE_BASIC_TERMINATE))
    if (demo_queue_basic_animating)
        icl_console_print ("<demo_queue_basic_terminate_start"
#if (defined (BASE_THREADSAFE))
" thread=\"%pp\""
#endif
"/>"
#if (defined (BASE_THREADSAFE))
, apr_os_thread_current ()
#endif
);
#endif

#if (defined (BASE_TRACE)   || defined (BASE_TRACE_DEMO_QUEUE_BASIC)   || defined (BASE_TRACE_DEMO_QUEUE_BASIC_TERMINATE))
    icl_trace_record (NULL, demo_queue_basic_dump, 6);
#endif

#if (defined (BASE_STATS)  ||  defined (BASE_STATS_DEMO_QUEUE_BASIC)  ||  defined (BASE_STATS_DEMO_QUEUE_BASIC_TERMINATE))
    icl_stats_inc ("demo_queue_basic_terminate", &s_demo_queue_basic_terminate_stats);
#endif


#if (defined (BASE_TRACE)   || defined (BASE_TRACE_DEMO_QUEUE_BASIC)   || defined (BASE_TRACE_DEMO_QUEUE_BASIC_TERMINATE))
    icl_trace_record (NULL, demo_queue_basic_dump, 0x10000 + 6);
#endif

#if (defined (BASE_ANIMATE)   || defined (BASE_ANIMATE_DEMO_QUEUE_BASIC)   || defined (BASE_ANIMATE_DEMO_QUEUE_BASIC_TERMINATE))
    if (demo_queue_basic_animating)
        icl_console_print ("<demo_queue_basic_terminate_finish"
#if (defined (BASE_THREADSAFE))
" thread=\"%pp\""
#endif
"/>"
#if (defined (BASE_THREADSAFE))
, apr_os_thread_current ()
#endif
);
#endif

}
/*  -------------------------------------------------------------------------
    demo_queue_basic_show

    Type: Component method
    -------------------------------------------------------------------------
 */

void
    demo_queue_basic_show_ (
    void * item,                        //  The opaque pointer
    int opcode,                         //  The callback opcode
    FILE * trace_file,                  //  File to print to
    char * file,                        //  Source file
    size_t line                         //  Line number
)
{
demo_queue_basic_t
    *self;
int
    container_links;


#if (defined (BASE_ANIMATE)  ||  defined (BASE_ANIMATE_DEMO_QUEUE_BASIC)  ||  defined (BASE_ANIMATE_DEMO_QUEUE_BASIC_SHOW))
    if (demo_queue_basic_animating)
        icl_console_print ("<demo_queue_basic_show_start"
#if (defined (BASE_THREADSAFE))
" thread=\"%pp\""
#endif
" item=\"%pp\""
" opcode=\"%i\""
" trace_file=\"%pp\""
" file=\"%s\""
" line=\"%u\""
"/>"
#if (defined (BASE_THREADSAFE))
, apr_os_thread_current ()
#endif
, item, opcode, trace_file, file, line);
#endif

#if (defined (BASE_TRACE)   || defined (BASE_TRACE_DEMO_QUEUE_BASIC)   || defined (BASE_TRACE_DEMO_QUEUE_BASIC_SHOW))
    icl_trace_record (NULL, demo_queue_basic_dump, 7);
#endif

#if (defined (BASE_STATS)  ||  defined (BASE_STATS_DEMO_QUEUE_BASIC)  ||  defined (BASE_STATS_DEMO_QUEUE_BASIC_SHOW))
    icl_stats_inc ("demo_queue_basic_show", &s_demo_queue_basic_show_stats);
#endif

self = item;
container_links = 0;
assert (opcode == ICL_CALLBACK_DUMP);

fprintf (trace_file, "    <demo_queue_basic file = \"%s\" line = \"%lu\"  pointer = \"%p\" />\n", file, (unsigned long) line, self);

#if (defined (BASE_TRACE)   || defined (BASE_TRACE_DEMO_QUEUE_BASIC)   || defined (BASE_TRACE_DEMO_QUEUE_BASIC_SHOW))
    icl_trace_record (NULL, demo_queue_basic_dump, 0x10000 + 7);
#endif

#if (defined (BASE_ANIMATE)   || defined (BASE_ANIMATE_DEMO_QUEUE_BASIC)   || defined (BASE_ANIMATE_DEMO_QUEUE_BASIC_SHOW))
    if (demo_queue_basic_animating)
        icl_console_print ("<demo_queue_basic_show_finish"
#if (defined (BASE_THREADSAFE))
" thread=\"%pp\""
#endif
" item=\"%pp\""
" opcode=\"%i\""
" trace_file=\"%pp\""
" file=\"%s\""
" line=\"%u\""
"/>"
#if (defined (BASE_THREADSAFE))
, apr_os_thread_current ()
#endif
, item, opcode, trace_file, file, line);
#endif

}
/*  -------------------------------------------------------------------------
    demo_queue_basic_destroy

    Type: Component method
    Destroys a demo_queue_basic_t object. Takes the address of a
    demo_queue_basic_t reference (a pointer to a pointer) and nullifies the
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
    demo_queue_basic_destroy_ (
    demo_queue_basic_t * ( * self_p ),   //  Reference to object reference
    char * file,                        //  Source fileSource file
    size_t line                         //  Line numberLine number
)
{
    demo_queue_basic_t *
        self = *self_p;                 //  Dereferenced Reference to object reference

#if (defined (BASE_ANIMATE)  ||  defined (BASE_ANIMATE_DEMO_QUEUE_BASIC)  ||  defined (BASE_ANIMATE_DEMO_QUEUE_BASIC_DESTROY_PUBLIC))
    if (demo_queue_basic_animating)
        icl_console_print ("<demo_queue_basic_destroy_public_start"
#if (defined (BASE_THREADSAFE))
" thread=\"%pp\""
#endif
" self=\"%pp\""
" self=\"%pp\""
" file=\"%s\""
" line=\"%i\""
"/>"
#if (defined (BASE_THREADSAFE))
, apr_os_thread_current ()
#endif
, self, self, file, line);
#endif

#if (defined (BASE_TRACE)   || defined (BASE_TRACE_DEMO_QUEUE_BASIC)   || defined (BASE_TRACE_DEMO_QUEUE_BASIC_DESTROY_PUBLIC))
    icl_trace_record (NULL, demo_queue_basic_dump, 8);
#endif

#if (defined (BASE_STATS)  ||  defined (BASE_STATS_DEMO_QUEUE_BASIC)  ||  defined (BASE_STATS_DEMO_QUEUE_BASIC_DESTROY_PUBLIC))
    icl_stats_inc ("demo_queue_basic_destroy", &s_demo_queue_basic_destroy_stats);
#endif

if (self) {
    demo_queue_basic_annihilate (self_p);
    demo_queue_basic_free ((demo_queue_basic_t *) self);
    *self_p = NULL;
}
#if (defined (BASE_TRACE)   || defined (BASE_TRACE_DEMO_QUEUE_BASIC)   || defined (BASE_TRACE_DEMO_QUEUE_BASIC_DESTROY_PUBLIC))
    icl_trace_record (NULL, demo_queue_basic_dump, 0x10000 + 8);
#endif

#if (defined (BASE_ANIMATE)   || defined (BASE_ANIMATE_DEMO_QUEUE_BASIC)   || defined (BASE_ANIMATE_DEMO_QUEUE_BASIC_DESTROY_PUBLIC))
    if (demo_queue_basic_animating)
        icl_console_print ("<demo_queue_basic_destroy_public_finish"
#if (defined (BASE_THREADSAFE))
" thread=\"%pp\""
#endif
" self=\"%pp\""
" self=\"%pp\""
" file=\"%s\""
" line=\"%i\""
"/>"
#if (defined (BASE_THREADSAFE))
, apr_os_thread_current ()
#endif
, self, self, file, line);
#endif

}
/*  -------------------------------------------------------------------------
    demo_queue_basic_alloc

    Type: Component method
    -------------------------------------------------------------------------
 */

static demo_queue_basic_t *
    demo_queue_basic_alloc_ (
    char * file,                        //  Source file for call
    size_t line                         //  Line number for call
)
{

    demo_queue_basic_t *
        self = NULL;                    //  Object reference

#if (defined (BASE_ANIMATE)  ||  defined (BASE_ANIMATE_DEMO_QUEUE_BASIC)  ||  defined (BASE_ANIMATE_DEMO_QUEUE_BASIC_ALLOC))
    if (demo_queue_basic_animating)
        icl_console_print ("<demo_queue_basic_alloc_start"
#if (defined (BASE_THREADSAFE))
" thread=\"%pp\""
#endif
" file=\"%s\""
" line=\"%u\""
"/>"
#if (defined (BASE_THREADSAFE))
, apr_os_thread_current ()
#endif
, file, line);
#endif

#if (defined (BASE_TRACE)   || defined (BASE_TRACE_DEMO_QUEUE_BASIC)   || defined (BASE_TRACE_DEMO_QUEUE_BASIC_ALLOC))
    icl_trace_record (NULL, demo_queue_basic_dump, 9);
#endif

#if (defined (BASE_STATS)  ||  defined (BASE_STATS_DEMO_QUEUE_BASIC)  ||  defined (BASE_STATS_DEMO_QUEUE_BASIC_ALLOC))
    icl_stats_inc ("demo_queue_basic_alloc", &s_demo_queue_basic_alloc_stats);
#endif

//  Initialise cache if necessary
if (!s_cache)
    demo_queue_basic_cache_initialise ();

self = (demo_queue_basic_t *) icl_mem_cache_alloc_ (s_cache, file, line);
if (self)
    memset (self, 0, sizeof (demo_queue_basic_t));


#if (defined (BASE_TRACE)   || defined (BASE_TRACE_DEMO_QUEUE_BASIC)   || defined (BASE_TRACE_DEMO_QUEUE_BASIC_ALLOC))
    icl_trace_record (NULL, demo_queue_basic_dump, 0x10000 + 9);
#endif

#if (defined (BASE_ANIMATE)   || defined (BASE_ANIMATE_DEMO_QUEUE_BASIC)   || defined (BASE_ANIMATE_DEMO_QUEUE_BASIC_ALLOC))
    if (demo_queue_basic_animating)
        icl_console_print ("<demo_queue_basic_alloc_finish"
#if (defined (BASE_THREADSAFE))
" thread=\"%pp\""
#endif
" file=\"%s\""
" line=\"%u\""
" self=\"%pp\""
"/>"
#if (defined (BASE_THREADSAFE))
, apr_os_thread_current ()
#endif
, file, line, self);
#endif


    return (self);
}
/*  -------------------------------------------------------------------------
    demo_queue_basic_free

    Type: Component method
    Freess a demo_queue_basic_t object.
    -------------------------------------------------------------------------
 */

static void
    demo_queue_basic_free (
    demo_queue_basic_t * self           //  Object reference
)
{


#if (defined (BASE_ANIMATE)  ||  defined (BASE_ANIMATE_DEMO_QUEUE_BASIC)  ||  defined (BASE_ANIMATE_DEMO_QUEUE_BASIC_FREE))
    if (demo_queue_basic_animating)
        icl_console_print ("<demo_queue_basic_free_start"
#if (defined (BASE_THREADSAFE))
" thread=\"%pp\""
#endif
" self=\"%pp\""
"/>"
#if (defined (BASE_THREADSAFE))
, apr_os_thread_current ()
#endif
, self);
#endif

#if (defined (BASE_TRACE)   || defined (BASE_TRACE_DEMO_QUEUE_BASIC)   || defined (BASE_TRACE_DEMO_QUEUE_BASIC_FREE))
    icl_trace_record (NULL, demo_queue_basic_dump, 10);
#endif

#if (defined (BASE_STATS)  ||  defined (BASE_STATS_DEMO_QUEUE_BASIC)  ||  defined (BASE_STATS_DEMO_QUEUE_BASIC_FREE))
    icl_stats_inc ("demo_queue_basic_free", &s_demo_queue_basic_free_stats);
#endif

if (self) {

        memset (&self->object_tag, 0, sizeof (demo_queue_basic_t) - ((byte *) &self->object_tag - (byte *) self));
//        memset (self, 0, sizeof (demo_queue_basic_t));
        self->object_tag = DEMO_QUEUE_BASIC_DEAD;
        icl_mem_free (self);
    }
    self = NULL;
#if (defined (BASE_TRACE)   || defined (BASE_TRACE_DEMO_QUEUE_BASIC)   || defined (BASE_TRACE_DEMO_QUEUE_BASIC_FREE))
    icl_trace_record (NULL, demo_queue_basic_dump, 0x10000 + 10);
#endif

#if (defined (BASE_ANIMATE)   || defined (BASE_ANIMATE_DEMO_QUEUE_BASIC)   || defined (BASE_ANIMATE_DEMO_QUEUE_BASIC_FREE))
    if (demo_queue_basic_animating)
        icl_console_print ("<demo_queue_basic_free_finish"
#if (defined (BASE_THREADSAFE))
" thread=\"%pp\""
#endif
" self=\"%pp\""
"/>"
#if (defined (BASE_THREADSAFE))
, apr_os_thread_current ()
#endif
, self);
#endif

}
/*  -------------------------------------------------------------------------
    demo_queue_basic_cache_initialise

    Type: Component method
    Initialise the cache and register purge method with the meta-cache.
    -------------------------------------------------------------------------
 */

static void
    demo_queue_basic_cache_initialise (
void)
{

#if (defined (BASE_ANIMATE)  ||  defined (BASE_ANIMATE_DEMO_QUEUE_BASIC)  ||  defined (BASE_ANIMATE_DEMO_QUEUE_BASIC_CACHE_INITIALISE))
    if (demo_queue_basic_animating)
        icl_console_print ("<demo_queue_basic_cache_initialise_start"
#if (defined (BASE_THREADSAFE))
" thread=\"%pp\""
#endif
"/>"
#if (defined (BASE_THREADSAFE))
, apr_os_thread_current ()
#endif
);
#endif

#if (defined (BASE_TRACE)   || defined (BASE_TRACE_DEMO_QUEUE_BASIC)   || defined (BASE_TRACE_DEMO_QUEUE_BASIC_CACHE_INITIALISE))
    icl_trace_record (NULL, demo_queue_basic_dump, 11);
#endif

#if (defined (BASE_STATS)  ||  defined (BASE_STATS_DEMO_QUEUE_BASIC)  ||  defined (BASE_STATS_DEMO_QUEUE_BASIC_CACHE_INITIALISE))
    icl_stats_inc ("demo_queue_basic_cache_initialise", &s_demo_queue_basic_cache_initialise_stats);
#endif

s_cache = icl_cache_get (sizeof (demo_queue_basic_t));
icl_system_register (demo_queue_basic_cache_purge, demo_queue_basic_cache_terminate);
#if (defined (BASE_TRACE)   || defined (BASE_TRACE_DEMO_QUEUE_BASIC)   || defined (BASE_TRACE_DEMO_QUEUE_BASIC_CACHE_INITIALISE))
    icl_trace_record (NULL, demo_queue_basic_dump, 0x10000 + 11);
#endif

#if (defined (BASE_ANIMATE)   || defined (BASE_ANIMATE_DEMO_QUEUE_BASIC)   || defined (BASE_ANIMATE_DEMO_QUEUE_BASIC_CACHE_INITIALISE))
    if (demo_queue_basic_animating)
        icl_console_print ("<demo_queue_basic_cache_initialise_finish"
#if (defined (BASE_THREADSAFE))
" thread=\"%pp\""
#endif
"/>"
#if (defined (BASE_THREADSAFE))
, apr_os_thread_current ()
#endif
);
#endif

}
/*  -------------------------------------------------------------------------
    demo_queue_basic_cache_purge

    Type: Component method
    -------------------------------------------------------------------------
 */

static void
    demo_queue_basic_cache_purge (
void)
{

#if (defined (BASE_ANIMATE)  ||  defined (BASE_ANIMATE_DEMO_QUEUE_BASIC)  ||  defined (BASE_ANIMATE_DEMO_QUEUE_BASIC_CACHE_PURGE))
    if (demo_queue_basic_animating)
        icl_console_print ("<demo_queue_basic_cache_purge_start"
#if (defined (BASE_THREADSAFE))
" thread=\"%pp\""
#endif
"/>"
#if (defined (BASE_THREADSAFE))
, apr_os_thread_current ()
#endif
);
#endif

#if (defined (BASE_TRACE)   || defined (BASE_TRACE_DEMO_QUEUE_BASIC)   || defined (BASE_TRACE_DEMO_QUEUE_BASIC_CACHE_PURGE))
    icl_trace_record (NULL, demo_queue_basic_dump, 12);
#endif

#if (defined (BASE_STATS)  ||  defined (BASE_STATS_DEMO_QUEUE_BASIC)  ||  defined (BASE_STATS_DEMO_QUEUE_BASIC_CACHE_PURGE))
    icl_stats_inc ("demo_queue_basic_cache_purge", &s_demo_queue_basic_cache_purge_stats);
#endif

icl_mem_cache_purge (s_cache);

#if (defined (BASE_TRACE)   || defined (BASE_TRACE_DEMO_QUEUE_BASIC)   || defined (BASE_TRACE_DEMO_QUEUE_BASIC_CACHE_PURGE))
    icl_trace_record (NULL, demo_queue_basic_dump, 0x10000 + 12);
#endif

#if (defined (BASE_ANIMATE)   || defined (BASE_ANIMATE_DEMO_QUEUE_BASIC)   || defined (BASE_ANIMATE_DEMO_QUEUE_BASIC_CACHE_PURGE))
    if (demo_queue_basic_animating)
        icl_console_print ("<demo_queue_basic_cache_purge_finish"
#if (defined (BASE_THREADSAFE))
" thread=\"%pp\""
#endif
"/>"
#if (defined (BASE_THREADSAFE))
, apr_os_thread_current ()
#endif
);
#endif

}
/*  -------------------------------------------------------------------------
    demo_queue_basic_cache_terminate

    Type: Component method
    -------------------------------------------------------------------------
 */

static void
    demo_queue_basic_cache_terminate (
void)
{

#if (defined (BASE_ANIMATE)  ||  defined (BASE_ANIMATE_DEMO_QUEUE_BASIC)  ||  defined (BASE_ANIMATE_DEMO_QUEUE_BASIC_CACHE_TERMINATE))
    if (demo_queue_basic_animating)
        icl_console_print ("<demo_queue_basic_cache_terminate_start"
#if (defined (BASE_THREADSAFE))
" thread=\"%pp\""
#endif
"/>"
#if (defined (BASE_THREADSAFE))
, apr_os_thread_current ()
#endif
);
#endif

#if (defined (BASE_TRACE)   || defined (BASE_TRACE_DEMO_QUEUE_BASIC)   || defined (BASE_TRACE_DEMO_QUEUE_BASIC_CACHE_TERMINATE))
    icl_trace_record (NULL, demo_queue_basic_dump, 13);
#endif

#if (defined (BASE_STATS)  ||  defined (BASE_STATS_DEMO_QUEUE_BASIC)  ||  defined (BASE_STATS_DEMO_QUEUE_BASIC_CACHE_TERMINATE))
    icl_stats_inc ("demo_queue_basic_cache_terminate", &s_demo_queue_basic_cache_terminate_stats);
#endif

s_cache = NULL;

#if (defined (BASE_TRACE)   || defined (BASE_TRACE_DEMO_QUEUE_BASIC)   || defined (BASE_TRACE_DEMO_QUEUE_BASIC_CACHE_TERMINATE))
    icl_trace_record (NULL, demo_queue_basic_dump, 0x10000 + 13);
#endif

#if (defined (BASE_ANIMATE)   || defined (BASE_ANIMATE_DEMO_QUEUE_BASIC)   || defined (BASE_ANIMATE_DEMO_QUEUE_BASIC_CACHE_TERMINATE))
    if (demo_queue_basic_animating)
        icl_console_print ("<demo_queue_basic_cache_terminate_finish"
#if (defined (BASE_THREADSAFE))
" thread=\"%pp\""
#endif
"/>"
#if (defined (BASE_THREADSAFE))
, apr_os_thread_current ()
#endif
);
#endif

}
/*  -------------------------------------------------------------------------
    demo_queue_basic_animate

    Type: Component method
    Enables animation of the component. Animation is sent to stdout.
    To enable animation you must generate using the option -animate:1.
    -------------------------------------------------------------------------
 */

void
    demo_queue_basic_animate (
    Bool enabled                        //  Are we enabling or disabling animation?
)
{

#if (defined (BASE_ANIMATE)  ||  defined (BASE_ANIMATE_DEMO_QUEUE_BASIC)  ||  defined (BASE_ANIMATE_DEMO_QUEUE_BASIC_ANIMATE))
    if (demo_queue_basic_animating)
        icl_console_print ("<demo_queue_basic_animate_start"
#if (defined (BASE_THREADSAFE))
" thread=\"%pp\""
#endif
" enabled=\"%i\""
"/>"
#if (defined (BASE_THREADSAFE))
, apr_os_thread_current ()
#endif
, enabled);
#endif

#if (defined (BASE_TRACE)   || defined (BASE_TRACE_DEMO_QUEUE_BASIC)   || defined (BASE_TRACE_DEMO_QUEUE_BASIC_ANIMATE))
    icl_trace_record (NULL, demo_queue_basic_dump, 14);
#endif

#if (defined (BASE_STATS)  ||  defined (BASE_STATS_DEMO_QUEUE_BASIC)  ||  defined (BASE_STATS_DEMO_QUEUE_BASIC_ANIMATE))
    icl_stats_inc ("demo_queue_basic_animate", &s_demo_queue_basic_animate_stats);
#endif

demo_queue_basic_animating = enabled;
#if (defined (BASE_TRACE)   || defined (BASE_TRACE_DEMO_QUEUE_BASIC)   || defined (BASE_TRACE_DEMO_QUEUE_BASIC_ANIMATE))
    icl_trace_record (NULL, demo_queue_basic_dump, 0x10000 + 14);
#endif

#if (defined (BASE_ANIMATE)   || defined (BASE_ANIMATE_DEMO_QUEUE_BASIC)   || defined (BASE_ANIMATE_DEMO_QUEUE_BASIC_ANIMATE))
    if (demo_queue_basic_animating)
        icl_console_print ("<demo_queue_basic_animate_finish"
#if (defined (BASE_THREADSAFE))
" thread=\"%pp\""
#endif
" enabled=\"%i\""
"/>"
#if (defined (BASE_THREADSAFE))
, apr_os_thread_current ()
#endif
, enabled);
#endif

}
/*  -------------------------------------------------------------------------
    demo_queue_basic_new_in_scope

    Type: Component method
    -------------------------------------------------------------------------
 */

void
    demo_queue_basic_new_in_scope_ (
    demo_queue_basic_t * * self_p,      //  Not documented
    icl_scope_t * _scope,               //  Not documented
    char * file,                        //  Source file for call
    size_t line                         //  Line number for call
)
{
    icl_destroy_t *
        _destroy;                       //  Not documented

#if (defined (BASE_ANIMATE)  ||  defined (BASE_ANIMATE_DEMO_QUEUE_BASIC)  ||  defined (BASE_ANIMATE_DEMO_QUEUE_BASIC_NEW_IN_SCOPE))
    if (demo_queue_basic_animating)
        icl_console_print ("<demo_queue_basic_new_in_scope_start"
#if (defined (BASE_THREADSAFE))
" thread=\"%pp\""
#endif
" self_p=\"%pp\""
" _scope=\"%pp\""
" file=\"%s\""
" line=\"%u\""
"/>"
#if (defined (BASE_THREADSAFE))
, apr_os_thread_current ()
#endif
, self_p, _scope, file, line);
#endif

#if (defined (BASE_TRACE)   || defined (BASE_TRACE_DEMO_QUEUE_BASIC)   || defined (BASE_TRACE_DEMO_QUEUE_BASIC_NEW_IN_SCOPE))
    icl_trace_record (NULL, demo_queue_basic_dump, 15);
#endif

#if (defined (BASE_STATS)  ||  defined (BASE_STATS_DEMO_QUEUE_BASIC)  ||  defined (BASE_STATS_DEMO_QUEUE_BASIC_NEW_IN_SCOPE))
    icl_stats_inc ("demo_queue_basic_new_in_scope", &s_demo_queue_basic_new_in_scope_stats);
#endif

*self_p = demo_queue_basic_new_ (file,line);

if (*self_p) {
    _destroy = icl_destroy_new   ((void * *) self_p, (icl_destructor_fn *) demo_queue_basic_destroy_);
    icl_destroy_list_queue (_scope, _destroy);
    icl_destroy_unlink (&_destroy);
}
#if (defined (BASE_TRACE)   || defined (BASE_TRACE_DEMO_QUEUE_BASIC)   || defined (BASE_TRACE_DEMO_QUEUE_BASIC_NEW_IN_SCOPE))
    icl_trace_record (NULL, demo_queue_basic_dump, 0x10000 + 15);
#endif

#if (defined (BASE_ANIMATE)   || defined (BASE_ANIMATE_DEMO_QUEUE_BASIC)   || defined (BASE_ANIMATE_DEMO_QUEUE_BASIC_NEW_IN_SCOPE))
    if (demo_queue_basic_animating)
        icl_console_print ("<demo_queue_basic_new_in_scope_finish"
#if (defined (BASE_THREADSAFE))
" thread=\"%pp\""
#endif
" self_p=\"%pp\""
" _scope=\"%pp\""
" file=\"%s\""
" line=\"%u\""
" _destroy=\"%pp\""
"/>"
#if (defined (BASE_THREADSAFE))
, apr_os_thread_current ()
#endif
, self_p, _scope, file, line, _destroy);
#endif

}
#if (defined (BASE_TRACE)   || defined (BASE_TRACE_DEMO_QUEUE_BASIC)   || defined (BASE_TRACE_DEMO_QUEUE_BASIC_NEW)   || defined (BASE_TRACE_DEMO_QUEUE_BASIC_DESTROY)   || defined (BASE_TRACE_DEMO_QUEUE_BASIC_PUBLISH)   || defined (BASE_TRACE_DEMO_QUEUE_BASIC_GET)   || defined (BASE_TRACE_DEMO_QUEUE_BASIC_SELFTEST)   || defined (BASE_TRACE_DEMO_QUEUE_BASIC_TERMINATE)   || defined (BASE_TRACE_DEMO_QUEUE_BASIC_SHOW)   || defined (BASE_TRACE_DEMO_QUEUE_BASIC_DESTROY_PUBLIC)   || defined (BASE_TRACE_DEMO_QUEUE_BASIC_ALLOC)   || defined (BASE_TRACE_DEMO_QUEUE_BASIC_FREE)   || defined (BASE_TRACE_DEMO_QUEUE_BASIC_CACHE_INITIALISE)   || defined (BASE_TRACE_DEMO_QUEUE_BASIC_CACHE_PURGE)   || defined (BASE_TRACE_DEMO_QUEUE_BASIC_CACHE_TERMINATE)   || defined (BASE_TRACE_DEMO_QUEUE_BASIC_ANIMATE)   || defined (BASE_TRACE_DEMO_QUEUE_BASIC_NEW_IN_SCOPE) )
void
demo_queue_basic_dump (icl_os_thread_t thread, apr_time_t time, qbyte info)
{
    dbyte
        method = info & 0xFFFF;
    char
        *method_name = NULL;
        
    switch (method) {
        case 1: method_name = "new"; break;
        case 2: method_name = "destroy"; break;
        case 3: method_name = "publish"; break;
        case 4: method_name = "get"; break;
        case 5: method_name = "selftest"; break;
        case 6: method_name = "terminate"; break;
        case 7: method_name = "show"; break;
        case 8: method_name = "destroy public"; break;
        case 9: method_name = "alloc"; break;
        case 10: method_name = "free"; break;
        case 11: method_name = "cache initialise"; break;
        case 12: method_name = "cache purge"; break;
        case 13: method_name = "cache terminate"; break;
        case 14: method_name = "animate"; break;
        case 15: method_name = "new in scope"; break;
    }
    icl_console_print_thread_time (thread, time,
                                   "demo_queue_basic %s%s",
                                   (info > 0xFFFF) ? "/" : "",
                                   method_name);
}
#endif

//  Embed the version information in the resulting binary                      

char *demo_queue_basic_version_start  = "VeRsIoNsTaRt:ipc";
char *demo_queue_basic_component   = "demo_queue_basic ";
char *demo_queue_basic_version     = "1.0 ";
char *demo_queue_basic_copyright   = "Copyright (c) 1996-2009 iMatix Corporation";
char *demo_queue_basic_filename    = "demo_queue_basic.icl ";
char *demo_queue_basic_builddate   = "2008/08/25 ";
char *demo_queue_basic_version_end  = "VeRsIoNeNd:ipc";

