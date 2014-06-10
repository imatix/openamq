/*---------------------------------------------------------------------------
    smt_thread_by_status.c - smt_thread_by_status component

    This class implements the list container for smt_thread
    Generated from smt_thread_by_status.icl by icl_gen using GSL/4.
    
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
#include "smt_thread_by_status.h"       //  Definitions for our class

//  Shorthand for class type

#define self_t              smt_thread_by_status_t

//  Shorthands for class methods

#define self_remove         smt_thread_by_status_remove
#define self_selftest       smt_thread_by_status_selftest
#define self_new            smt_thread_by_status_new
#define self_annihilate     smt_thread_by_status_annihilate
#define self_isempty        smt_thread_by_status_isempty
#define self_push           smt_thread_by_status_push
#define self_pop            smt_thread_by_status_pop
#define self_queue          smt_thread_by_status_queue
#define self_relink_before  smt_thread_by_status_relink_before
#define self_first          smt_thread_by_status_first
#define self_last           smt_thread_by_status_last
#define self_next           smt_thread_by_status_next
#define self_prev           smt_thread_by_status_prev
#define self_rehook         smt_thread_by_status_rehook
#define self_unhook         smt_thread_by_status_unhook
#define self_terminate      smt_thread_by_status_terminate
#define self_show           smt_thread_by_status_show
#define self_destroy        smt_thread_by_status_destroy
#define self_alloc          smt_thread_by_status_alloc
#define self_free           smt_thread_by_status_free
#define self_link           smt_thread_by_status_link
#define self_unlink         smt_thread_by_status_unlink
#define self_cache_initialise  smt_thread_by_status_cache_initialise
#define self_cache_purge    smt_thread_by_status_cache_purge
#define self_cache_terminate  smt_thread_by_status_cache_terminate
#define self_show_animation  smt_thread_by_status_show_animation
#define self_new_in_scope   smt_thread_by_status_new_in_scope

#if (defined (BASE_STATS)  ||  defined (BASE_STATS_SMT_THREAD_BY_STATUS)  ||  defined (BASE_STATS_SMT_THREAD_BY_STATUS_REMOVE))
static icl_stats_t *s_smt_thread_by_status_remove_stats = NULL;
#endif
#if (defined (BASE_STATS)  ||  defined (BASE_STATS_SMT_THREAD_BY_STATUS)  ||  defined (BASE_STATS_SMT_THREAD_BY_STATUS_SELFTEST))
static icl_stats_t *s_smt_thread_by_status_selftest_stats = NULL;
#endif
#if (defined (BASE_STATS)  ||  defined (BASE_STATS_SMT_THREAD_BY_STATUS)  ||  defined (BASE_STATS_SMT_THREAD_BY_STATUS_NEW))
static icl_stats_t *s_smt_thread_by_status_new_stats = NULL;
#endif
#if (defined (BASE_STATS)  ||  defined (BASE_STATS_SMT_THREAD_BY_STATUS)  ||  defined (BASE_STATS_SMT_THREAD_BY_STATUS_DESTROY))
static icl_stats_t *s_smt_thread_by_status_annihilate_stats = NULL;
#endif
#if (defined (BASE_STATS)  ||  defined (BASE_STATS_SMT_THREAD_BY_STATUS)  ||  defined (BASE_STATS_SMT_THREAD_BY_STATUS_ISEMPTY))
static icl_stats_t *s_smt_thread_by_status_isempty_stats = NULL;
#endif
#if (defined (BASE_STATS)  ||  defined (BASE_STATS_SMT_THREAD_BY_STATUS)  ||  defined (BASE_STATS_SMT_THREAD_BY_STATUS_PUSH))
static icl_stats_t *s_smt_thread_by_status_push_stats = NULL;
#endif
#if (defined (BASE_STATS)  ||  defined (BASE_STATS_SMT_THREAD_BY_STATUS)  ||  defined (BASE_STATS_SMT_THREAD_BY_STATUS_POP))
static icl_stats_t *s_smt_thread_by_status_pop_stats = NULL;
#endif
#if (defined (BASE_STATS)  ||  defined (BASE_STATS_SMT_THREAD_BY_STATUS)  ||  defined (BASE_STATS_SMT_THREAD_BY_STATUS_QUEUE))
static icl_stats_t *s_smt_thread_by_status_queue_stats = NULL;
#endif
#if (defined (BASE_STATS)  ||  defined (BASE_STATS_SMT_THREAD_BY_STATUS)  ||  defined (BASE_STATS_SMT_THREAD_BY_STATUS_RELINK_BEFORE))
static icl_stats_t *s_smt_thread_by_status_relink_before_stats = NULL;
#endif
#if (defined (BASE_STATS)  ||  defined (BASE_STATS_SMT_THREAD_BY_STATUS)  ||  defined (BASE_STATS_SMT_THREAD_BY_STATUS_FIRST))
static icl_stats_t *s_smt_thread_by_status_first_stats = NULL;
#endif
#if (defined (BASE_STATS)  ||  defined (BASE_STATS_SMT_THREAD_BY_STATUS)  ||  defined (BASE_STATS_SMT_THREAD_BY_STATUS_LAST))
static icl_stats_t *s_smt_thread_by_status_last_stats = NULL;
#endif
#if (defined (BASE_STATS)  ||  defined (BASE_STATS_SMT_THREAD_BY_STATUS)  ||  defined (BASE_STATS_SMT_THREAD_BY_STATUS_NEXT))
static icl_stats_t *s_smt_thread_by_status_next_stats = NULL;
#endif
#if (defined (BASE_STATS)  ||  defined (BASE_STATS_SMT_THREAD_BY_STATUS)  ||  defined (BASE_STATS_SMT_THREAD_BY_STATUS_PREV))
static icl_stats_t *s_smt_thread_by_status_prev_stats = NULL;
#endif
#if (defined (BASE_STATS)  ||  defined (BASE_STATS_SMT_THREAD_BY_STATUS)  ||  defined (BASE_STATS_SMT_THREAD_BY_STATUS_REHOOK))
static icl_stats_t *s_smt_thread_by_status_rehook_stats = NULL;
#endif
#if (defined (BASE_STATS)  ||  defined (BASE_STATS_SMT_THREAD_BY_STATUS)  ||  defined (BASE_STATS_SMT_THREAD_BY_STATUS_UNHOOK))
static icl_stats_t *s_smt_thread_by_status_unhook_stats = NULL;
#endif
#if (defined (BASE_STATS)  ||  defined (BASE_STATS_SMT_THREAD_BY_STATUS)  ||  defined (BASE_STATS_SMT_THREAD_BY_STATUS_TERMINATE))
static icl_stats_t *s_smt_thread_by_status_terminate_stats = NULL;
#endif
#if (defined (BASE_STATS)  ||  defined (BASE_STATS_SMT_THREAD_BY_STATUS)  ||  defined (BASE_STATS_SMT_THREAD_BY_STATUS_SHOW))
static icl_stats_t *s_smt_thread_by_status_show_stats = NULL;
#endif
#if (defined (BASE_STATS)  ||  defined (BASE_STATS_SMT_THREAD_BY_STATUS)  ||  defined (BASE_STATS_SMT_THREAD_BY_STATUS_DESTROY_PUBLIC))
static icl_stats_t *s_smt_thread_by_status_destroy_stats = NULL;
#endif
#if (defined (BASE_STATS)  ||  defined (BASE_STATS_SMT_THREAD_BY_STATUS)  ||  defined (BASE_STATS_SMT_THREAD_BY_STATUS_ALLOC))
static icl_stats_t *s_smt_thread_by_status_alloc_stats = NULL;
#endif
#if (defined (BASE_STATS)  ||  defined (BASE_STATS_SMT_THREAD_BY_STATUS)  ||  defined (BASE_STATS_SMT_THREAD_BY_STATUS_FREE))
static icl_stats_t *s_smt_thread_by_status_free_stats = NULL;
#endif
#if (defined (BASE_STATS)  ||  defined (BASE_STATS_SMT_THREAD_BY_STATUS)  ||  defined (BASE_STATS_SMT_THREAD_BY_STATUS_LINK))
static icl_stats_t *s_smt_thread_by_status_link_stats = NULL;
#endif
#if (defined (BASE_STATS)  ||  defined (BASE_STATS_SMT_THREAD_BY_STATUS)  ||  defined (BASE_STATS_SMT_THREAD_BY_STATUS_UNLINK))
static icl_stats_t *s_smt_thread_by_status_unlink_stats = NULL;
#endif
#if (defined (BASE_STATS)  ||  defined (BASE_STATS_SMT_THREAD_BY_STATUS)  ||  defined (BASE_STATS_SMT_THREAD_BY_STATUS_CACHE_INITIALISE))
static icl_stats_t *s_smt_thread_by_status_cache_initialise_stats = NULL;
#endif
#if (defined (BASE_STATS)  ||  defined (BASE_STATS_SMT_THREAD_BY_STATUS)  ||  defined (BASE_STATS_SMT_THREAD_BY_STATUS_CACHE_PURGE))
static icl_stats_t *s_smt_thread_by_status_cache_purge_stats = NULL;
#endif
#if (defined (BASE_STATS)  ||  defined (BASE_STATS_SMT_THREAD_BY_STATUS)  ||  defined (BASE_STATS_SMT_THREAD_BY_STATUS_CACHE_TERMINATE))
static icl_stats_t *s_smt_thread_by_status_cache_terminate_stats = NULL;
#endif
#if (defined (BASE_STATS)  ||  defined (BASE_STATS_SMT_THREAD_BY_STATUS)  ||  defined (BASE_STATS_SMT_THREAD_BY_STATUS_SHOW_ANIMATION))
static icl_stats_t *s_smt_thread_by_status_show_animation_stats = NULL;
#endif
#if (defined (BASE_STATS)  ||  defined (BASE_STATS_SMT_THREAD_BY_STATUS)  ||  defined (BASE_STATS_SMT_THREAD_BY_STATUS_NEW_IN_SCOPE))
static icl_stats_t *s_smt_thread_by_status_new_in_scope_stats = NULL;
#endif
#define smt_thread_by_status_annihilate(self)  smt_thread_by_status_annihilate_ (self, __FILE__, __LINE__)
static void
    smt_thread_by_status_annihilate_ (
smt_thread_by_status_t * ( * self_p ),   //  Reference to object reference
char * file,                            //  Source file
size_t line                             //  Line number
);

static inline void
    smt_thread_by_status_rehook (
smt_thread_t * item,                    //  Not documented
smt_thread_t * left,                    //  Not documented
smt_thread_t * right                    //  Not documented
);

static inline smt_thread_t *
    smt_thread_by_status_unhook (
smt_thread_t * item                     //  Not documented
);

#define smt_thread_by_status_alloc()    smt_thread_by_status_alloc_ (__FILE__, __LINE__)
static smt_thread_by_status_t *
    smt_thread_by_status_alloc_ (
char * file,                            //  Source file for call
size_t line                             //  Line number for call
);

#define smt_thread_by_status_free(self)  smt_thread_by_status_free_ (self, __FILE__, __LINE__)
static void
    smt_thread_by_status_free_ (
smt_thread_by_status_t * self,          //  Object reference
char * file,                            //  Source file
size_t line                             //  Line number
);

static void
    smt_thread_by_status_cache_initialise (
void);

static void
    smt_thread_by_status_cache_purge (
void);

static void
    smt_thread_by_status_cache_terminate (
void);

Bool
    smt_thread_by_status_animating = TRUE;  //  Animation enabled by default
static icl_cache_t
    *s_cache = NULL;


/*  -------------------------------------------------------------------------
    smt_thread_by_status_remove

    Type: Component method
    Removes the item from any list it is in.

    If it was in a list, the number of links is decremented.
    This method removes a threadlet from a status list, updating the
    priority pointers if needed.
    -------------------------------------------------------------------------
 */

int
    smt_thread_by_status_remove (
    smt_thread_t * item                 //  Not documented
)
{


smt_os_thread_t *
    os_thread = item->os_thread;
smt_priority_t
    priority_index;
smt_thread_t
    *next_thread;
    smt_thread_by_status_t *
        self = (smt_thread_by_status_t *) item->by_status_head;  //  The list
    int
        rc = 0;                         //  Not documented

#if (defined (BASE_ANIMATE)  ||  defined (BASE_ANIMATE_SMT_THREAD_BY_STATUS)  ||  defined (BASE_ANIMATE_SMT_THREAD_BY_STATUS_REMOVE))
    if (smt_thread_by_status_animating)
        icl_console_print ("<smt_thread_by_status_remove_start"
#if (defined (BASE_THREADSAFE))
" thread=\"%pp\""
#endif
" item=\"%pp\""
"/>"
#if (defined (BASE_THREADSAFE))
, apr_os_thread_current ()
#endif
, item);
#endif

#if (defined (BASE_TRACE)   || defined (BASE_TRACE_SMT_THREAD_BY_STATUS)   || defined (BASE_TRACE_SMT_THREAD_BY_STATUS_REMOVE))
    icl_trace_record (1, smt_thread_by_status_dump, 1);
#endif

#if (defined (BASE_STATS)  ||  defined (BASE_STATS_SMT_THREAD_BY_STATUS)  ||  defined (BASE_STATS_SMT_THREAD_BY_STATUS_REMOVE))
    icl_stats_inc ("smt_thread_by_status_remove", &s_smt_thread_by_status_remove_stats);
#endif



#if (defined (BASE_THREADSAFE) && defined (DEBUG))
    if (os_thread)
        assert (apr_os_thread_current () == os_thread->apr_os_thread);
#endif
    if (os_thread
    &&  item->by_status_head == os_thread->waiting_list
    &&  item->priority > 0) {
        next_thread = item;
        next_thread = smt_thread_by_status_next (&next_thread);

        //  Adjust insertion points for higher priority
        priority_index = item->priority;
        while (priority_index > 0) {
            priority_index--;
            if (item == os_thread->waiting_list_entry [priority_index])
                os_thread->waiting_list_entry [priority_index] = next_thread;
            else
                break;
        }
    }
if (self
&&  self == item->by_status_head) { //  Catch case where item moved
    smt_thread_by_status_unhook (item);
}


#if (defined (BASE_TRACE)   || defined (BASE_TRACE_SMT_THREAD_BY_STATUS)   || defined (BASE_TRACE_SMT_THREAD_BY_STATUS_REMOVE))
    icl_trace_record (1, smt_thread_by_status_dump, 0x10000 + 1);
#endif

#if (defined (BASE_ANIMATE)   || defined (BASE_ANIMATE_SMT_THREAD_BY_STATUS)   || defined (BASE_ANIMATE_SMT_THREAD_BY_STATUS_REMOVE))
    if (smt_thread_by_status_animating)
        icl_console_print ("<smt_thread_by_status_remove_finish"
#if (defined (BASE_THREADSAFE))
" thread=\"%pp\""
#endif
" item=\"%pp\""
" self=\"%pp\""
" rc=\"%i\""
"/>"
#if (defined (BASE_THREADSAFE))
, apr_os_thread_current ()
#endif
, item, self, rc);
#endif


    return (rc);
}
/*  -------------------------------------------------------------------------
    smt_thread_by_status_selftest

    Type: Component method
    -------------------------------------------------------------------------
 */

void
    smt_thread_by_status_selftest (
void)
{

#if (defined (BASE_ANIMATE)  ||  defined (BASE_ANIMATE_SMT_THREAD_BY_STATUS)  ||  defined (BASE_ANIMATE_SMT_THREAD_BY_STATUS_SELFTEST))
    if (smt_thread_by_status_animating)
        icl_console_print ("<smt_thread_by_status_selftest_start"
#if (defined (BASE_THREADSAFE))
" thread=\"%pp\""
#endif
"/>"
#if (defined (BASE_THREADSAFE))
, apr_os_thread_current ()
#endif
);
#endif

#if (defined (BASE_TRACE)   || defined (BASE_TRACE_SMT_THREAD_BY_STATUS)   || defined (BASE_TRACE_SMT_THREAD_BY_STATUS_SELFTEST))
    icl_trace_record (1, smt_thread_by_status_dump, 2);
#endif

#if (defined (BASE_STATS)  ||  defined (BASE_STATS_SMT_THREAD_BY_STATUS)  ||  defined (BASE_STATS_SMT_THREAD_BY_STATUS_SELFTEST))
    icl_stats_inc ("smt_thread_by_status_selftest", &s_smt_thread_by_status_selftest_stats);
#endif

#if (defined (BASE_TRACE)   || defined (BASE_TRACE_SMT_THREAD_BY_STATUS)   || defined (BASE_TRACE_SMT_THREAD_BY_STATUS_SELFTEST))
    icl_trace_record (1, smt_thread_by_status_dump, 0x10000 + 2);
#endif

#if (defined (BASE_ANIMATE)   || defined (BASE_ANIMATE_SMT_THREAD_BY_STATUS)   || defined (BASE_ANIMATE_SMT_THREAD_BY_STATUS_SELFTEST))
    if (smt_thread_by_status_animating)
        icl_console_print ("<smt_thread_by_status_selftest_finish"
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
    smt_thread_by_status_new

    Type: Component method
    Creates and initialises a new smt_thread_by_status_t object, returns a
    reference to the created object.
    -------------------------------------------------------------------------
 */

smt_thread_by_status_t *
    smt_thread_by_status_new_ (
    char * file,                        //  Source file for call
    size_t line                         //  Line number for call
)
{
    smt_thread_by_status_t *
        self = NULL;                    //  Object reference

#if (defined (BASE_ANIMATE)  ||  defined (BASE_ANIMATE_SMT_THREAD_BY_STATUS)  ||  defined (BASE_ANIMATE_SMT_THREAD_BY_STATUS_NEW))
    if (smt_thread_by_status_animating)
        icl_console_print ("<smt_thread_by_status_new_start"
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

#if (defined (BASE_TRACE)   || defined (BASE_TRACE_SMT_THREAD_BY_STATUS)   || defined (BASE_TRACE_SMT_THREAD_BY_STATUS_NEW))
    icl_trace_record (1, smt_thread_by_status_dump, 3);
#endif

#if (defined (BASE_STATS)  ||  defined (BASE_STATS_SMT_THREAD_BY_STATUS)  ||  defined (BASE_STATS_SMT_THREAD_BY_STATUS_NEW))
    icl_stats_inc ("smt_thread_by_status_new", &s_smt_thread_by_status_new_stats);
#endif

    self = smt_thread_by_status_alloc_ (file, line);
    if (self) {
        self->object_tag   = SMT_THREAD_BY_STATUS_ALIVE;
        self->links        = 1;
        self->zombie       = FALSE;
#if defined (DEBUG) || defined (BASE_HISTORY) || defined (BASE_HISTORY_SMT_THREAD_BY_STATUS)
        self->history_last = 0;

        //  Track possession operation in history
        self->history_file  [0] = file;
        self->history_line  [0] = line;
        self->history_type  [0] = "new";
        self->history_links [0] = self->links;
#endif
#if defined (DEBUG)
        icl_mem_set_callback (self, smt_thread_by_status_show_);
#endif

self->smt_thread.by_status_next = &self->smt_thread;
self->smt_thread.by_status_prev = &self->smt_thread;
self->smt_thread.by_status_head = self;

}
#if (defined (BASE_TRACE)   || defined (BASE_TRACE_SMT_THREAD_BY_STATUS)   || defined (BASE_TRACE_SMT_THREAD_BY_STATUS_NEW))
    icl_trace_record (1, smt_thread_by_status_dump, 0x10000 + 3);
#endif

#if (defined (BASE_ANIMATE)   || defined (BASE_ANIMATE_SMT_THREAD_BY_STATUS)   || defined (BASE_ANIMATE_SMT_THREAD_BY_STATUS_NEW))
    if (smt_thread_by_status_animating)
        icl_console_print ("<smt_thread_by_status_new_finish"
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
    smt_thread_by_status_annihilate

    Type: Component method
    Destroys the list and unlinks all attached items.
    -------------------------------------------------------------------------
 */

static void
    smt_thread_by_status_annihilate_ (
    smt_thread_by_status_t * ( * self_p ),   //  Reference to object reference
    char * file,                        //  Source file
    size_t line                         //  Line number
)
{
#if defined (DEBUG) || defined (BASE_HISTORY) || defined (BASE_HISTORY_SMT_THREAD_BY_STATUS)
    int
        history_last;
#endif

smt_thread_t
    *item;
    smt_thread_by_status_t *
        self = *self_p;                 //  Dereferenced Reference to object reference

#if (defined (BASE_ANIMATE)  ||  defined (BASE_ANIMATE_SMT_THREAD_BY_STATUS)  ||  defined (BASE_ANIMATE_SMT_THREAD_BY_STATUS_DESTROY))
    if (smt_thread_by_status_animating)
        icl_console_print ("<smt_thread_by_status_destroy_start"
#if (defined (BASE_THREADSAFE))
" thread=\"%pp\""
#endif
" self=\"%pp\""
" file=\"%s\""
" line=\"%u\""
" self=\"%pp\""
"/>"
#if (defined (BASE_THREADSAFE))
, apr_os_thread_current ()
#endif
, self, file, line, self);
#endif

#if (defined (BASE_TRACE)   || defined (BASE_TRACE_SMT_THREAD_BY_STATUS)   || defined (BASE_TRACE_SMT_THREAD_BY_STATUS_DESTROY))
    icl_trace_record (1, smt_thread_by_status_dump, 4);
#endif

#if (defined (BASE_STATS)  ||  defined (BASE_STATS_SMT_THREAD_BY_STATUS)  ||  defined (BASE_STATS_SMT_THREAD_BY_STATUS_DESTROY))
    icl_stats_inc ("smt_thread_by_status_annihilate", &s_smt_thread_by_status_annihilate_stats);
#endif

#if defined (DEBUG) || defined (BASE_HISTORY) || defined (BASE_HISTORY_SMT_THREAD_BY_STATUS)
    //  Track possession operation in history
    history_last = icl_atomic_inc32 ((volatile qbyte *) &self->history_last) + 1;
    self->history_file  [history_last % SMT_THREAD_BY_STATUS_HISTORY_LENGTH] = file;
    self->history_line  [history_last % SMT_THREAD_BY_STATUS_HISTORY_LENGTH] = line;
    self->history_type  [history_last % SMT_THREAD_BY_STATUS_HISTORY_LENGTH] = "destroy";
    self->history_links [history_last % SMT_THREAD_BY_STATUS_HISTORY_LENGTH] = self->links;
#endif

    SMT_THREAD_BY_STATUS_ASSERT_SANE (self);

item = (smt_thread_t *) self->smt_thread.by_status_next;
while (item != &self->smt_thread) {
    //  Unlink child without lock (we already have the lock)
    smt_thread_by_status_unhook (item);

    smt_thread_destroy (&item);
    item = (smt_thread_t *) self->smt_thread.by_status_next;
}

#if (defined (BASE_TRACE)   || defined (BASE_TRACE_SMT_THREAD_BY_STATUS)   || defined (BASE_TRACE_SMT_THREAD_BY_STATUS_DESTROY))
    icl_trace_record (1, smt_thread_by_status_dump, 0x10000 + 4);
#endif

#if (defined (BASE_ANIMATE)   || defined (BASE_ANIMATE_SMT_THREAD_BY_STATUS)   || defined (BASE_ANIMATE_SMT_THREAD_BY_STATUS_DESTROY))
    if (smt_thread_by_status_animating)
        icl_console_print ("<smt_thread_by_status_destroy_finish"
#if (defined (BASE_THREADSAFE))
" thread=\"%pp\""
#endif
" self=\"%pp\""
" file=\"%s\""
" line=\"%u\""
" self=\"%pp\""
"/>"
#if (defined (BASE_THREADSAFE))
, apr_os_thread_current ()
#endif
, self, file, line, self);
#endif

}
/*  -------------------------------------------------------------------------
    smt_thread_by_status_isempty

    Type: Component method
    Accepts a smt_thread_by_status_t reference and returns zero in case of success,
    1 in case of errors.
    Returns true if the list is empty, else returns false.
    -------------------------------------------------------------------------
 */

int
    smt_thread_by_status_isempty (
    smt_thread_by_status_t * self       //  Reference to object
)
{
    int
        rc = 0;                         //  Return code

#if (defined (BASE_ANIMATE)  ||  defined (BASE_ANIMATE_SMT_THREAD_BY_STATUS)  ||  defined (BASE_ANIMATE_SMT_THREAD_BY_STATUS_ISEMPTY))
    if (smt_thread_by_status_animating)
        icl_console_print ("<smt_thread_by_status_isempty_start"
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

#if (defined (BASE_TRACE)   || defined (BASE_TRACE_SMT_THREAD_BY_STATUS)   || defined (BASE_TRACE_SMT_THREAD_BY_STATUS_ISEMPTY))
    icl_trace_record (1, smt_thread_by_status_dump, 5);
#endif

#if (defined (BASE_STATS)  ||  defined (BASE_STATS_SMT_THREAD_BY_STATUS)  ||  defined (BASE_STATS_SMT_THREAD_BY_STATUS_ISEMPTY))
    icl_stats_inc ("smt_thread_by_status_isempty", &s_smt_thread_by_status_isempty_stats);
#endif

SMT_THREAD_BY_STATUS_ASSERT_SANE (self);
if (!self->zombie) {

rc = (self->smt_thread.by_status_next == &self->smt_thread);
}
else
    rc = -1;                        //  Return error on zombie object.

#if (defined (BASE_TRACE)   || defined (BASE_TRACE_SMT_THREAD_BY_STATUS)   || defined (BASE_TRACE_SMT_THREAD_BY_STATUS_ISEMPTY))
    icl_trace_record (1, smt_thread_by_status_dump, 0x10000 + 5);
#endif

#if (defined (BASE_ANIMATE)   || defined (BASE_ANIMATE_SMT_THREAD_BY_STATUS)   || defined (BASE_ANIMATE_SMT_THREAD_BY_STATUS_ISEMPTY))
    if (smt_thread_by_status_animating)
        icl_console_print ("<smt_thread_by_status_isempty_finish"
#if (defined (BASE_THREADSAFE))
" thread=\"%pp\""
#endif
" self=\"%pp\""
" rc=\"%i\""
"/>"
#if (defined (BASE_THREADSAFE))
, apr_os_thread_current ()
#endif
, self, rc);
#endif


    return (rc);
}
/*  -------------------------------------------------------------------------
    smt_thread_by_status_push

    Type: Component method
    Accepts a smt_thread_by_status_t reference and returns zero in case of success,
    1 in case of errors.
    Attaches the item to the start of the specified list. If the item
    was on another list, it is first detached.  After this method,
    list->next points to the item.
    -------------------------------------------------------------------------
 */

int
    smt_thread_by_status_push (
    smt_thread_by_status_t * self,      //  Reference to object
    smt_thread_t * item                 //  Not documented
)
{


    int
        rc = 0;                         //  Return code

#if (defined (BASE_ANIMATE)  ||  defined (BASE_ANIMATE_SMT_THREAD_BY_STATUS)  ||  defined (BASE_ANIMATE_SMT_THREAD_BY_STATUS_PUSH))
    if (smt_thread_by_status_animating)
        icl_console_print ("<smt_thread_by_status_push_start"
#if (defined (BASE_THREADSAFE))
" thread=\"%pp\""
#endif
" self=\"%pp\""
" item=\"%pp\""
"/>"
#if (defined (BASE_THREADSAFE))
, apr_os_thread_current ()
#endif
, self, item);
#endif

#if (defined (BASE_TRACE)   || defined (BASE_TRACE_SMT_THREAD_BY_STATUS)   || defined (BASE_TRACE_SMT_THREAD_BY_STATUS_PUSH))
    icl_trace_record (1, smt_thread_by_status_dump, 6);
#endif

#if (defined (BASE_STATS)  ||  defined (BASE_STATS_SMT_THREAD_BY_STATUS)  ||  defined (BASE_STATS_SMT_THREAD_BY_STATUS_PUSH))
    icl_stats_inc ("smt_thread_by_status_push", &s_smt_thread_by_status_push_stats);
#endif



SMT_THREAD_BY_STATUS_ASSERT_SANE (self);
if (!self->zombie) {

//  If by_status_head is non-null then we are moving within the
//  list to another so don't need to adjust links. Otherwise it
//  is illegitimate to move from one list to another in one step,
//  because we don't have a lock on the other list.
if (! item->by_status_head) {
}
else
if (item->by_status_head != self) {
    icl_console_print ("E: smt_thread item inserted into multiple by_status containers");
    assert (item->by_status_head == self);
}
smt_thread_by_status_unhook (item);
smt_thread_by_status_rehook (item, &self->smt_thread, (smt_thread_t *) self->smt_thread.by_status_next);
}
else
    rc = -1;                        //  Return error on zombie object.



#if (defined (BASE_TRACE)   || defined (BASE_TRACE_SMT_THREAD_BY_STATUS)   || defined (BASE_TRACE_SMT_THREAD_BY_STATUS_PUSH))
    icl_trace_record (1, smt_thread_by_status_dump, 0x10000 + 6);
#endif

#if (defined (BASE_ANIMATE)   || defined (BASE_ANIMATE_SMT_THREAD_BY_STATUS)   || defined (BASE_ANIMATE_SMT_THREAD_BY_STATUS_PUSH))
    if (smt_thread_by_status_animating)
        icl_console_print ("<smt_thread_by_status_push_finish"
#if (defined (BASE_THREADSAFE))
" thread=\"%pp\""
#endif
" self=\"%pp\""
" item=\"%pp\""
" rc=\"%i\""
"/>"
#if (defined (BASE_THREADSAFE))
, apr_os_thread_current ()
#endif
, self, item, rc);
#endif


    return (rc);
}
/*  -------------------------------------------------------------------------
    smt_thread_by_status_pop

    Type: Component method
    Removes the next item in the list, if any, and returns it.  If
    the list was empty, returns NULL.

    The number of links is unchanged; removing from the list and returning
    the value cancel each other out.
    -------------------------------------------------------------------------
 */

smt_thread_t *
    smt_thread_by_status_pop (
    smt_thread_by_status_t * self       //  The list
)
{


    smt_thread_t *
        item = NULL;                    //  Not documented

#if (defined (BASE_ANIMATE)  ||  defined (BASE_ANIMATE_SMT_THREAD_BY_STATUS)  ||  defined (BASE_ANIMATE_SMT_THREAD_BY_STATUS_POP))
    if (smt_thread_by_status_animating)
        icl_console_print ("<smt_thread_by_status_pop_start"
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

#if (defined (BASE_TRACE)   || defined (BASE_TRACE_SMT_THREAD_BY_STATUS)   || defined (BASE_TRACE_SMT_THREAD_BY_STATUS_POP))
    icl_trace_record (1, smt_thread_by_status_dump, 7);
#endif

#if (defined (BASE_STATS)  ||  defined (BASE_STATS_SMT_THREAD_BY_STATUS)  ||  defined (BASE_STATS_SMT_THREAD_BY_STATUS_POP))
    icl_stats_inc ("smt_thread_by_status_pop", &s_smt_thread_by_status_pop_stats);
#endif



SMT_THREAD_BY_STATUS_ASSERT_SANE (self);
if (!self->zombie) {

if (self->smt_thread.by_status_next == &self->smt_thread)
    item = NULL;
else {
    item = smt_thread_by_status_unhook ((smt_thread_t *) self->smt_thread.by_status_next);
}
}



#if (defined (BASE_TRACE)   || defined (BASE_TRACE_SMT_THREAD_BY_STATUS)   || defined (BASE_TRACE_SMT_THREAD_BY_STATUS_POP))
    icl_trace_record (1, smt_thread_by_status_dump, 0x10000 + 7);
#endif

#if (defined (BASE_ANIMATE)   || defined (BASE_ANIMATE_SMT_THREAD_BY_STATUS)   || defined (BASE_ANIMATE_SMT_THREAD_BY_STATUS_POP))
    if (smt_thread_by_status_animating)
        icl_console_print ("<smt_thread_by_status_pop_finish"
#if (defined (BASE_THREADSAFE))
" thread=\"%pp\""
#endif
" self=\"%pp\""
" item=\"%pp\""
"/>"
#if (defined (BASE_THREADSAFE))
, apr_os_thread_current ()
#endif
, self, item);
#endif


    return (item);
}
/*  -------------------------------------------------------------------------
    smt_thread_by_status_queue

    Type: Component method
    Accepts a smt_thread_by_status_t reference and returns zero in case of success,
    1 in case of errors.
    Attaches the item to the end of the specified list.  The item may not
    be in another list of the same class.

    The number of links is automatically adjusted - if the item was already
    in the list, it remains unchanged; otherwise it is incremented.
    -------------------------------------------------------------------------
 */

int
    smt_thread_by_status_queue (
    smt_thread_by_status_t * self,      //  Reference to object
    smt_thread_t * item                 //  Not documented
)
{


    int
        rc = 0;                         //  Return code

#if (defined (BASE_ANIMATE)  ||  defined (BASE_ANIMATE_SMT_THREAD_BY_STATUS)  ||  defined (BASE_ANIMATE_SMT_THREAD_BY_STATUS_QUEUE))
    if (smt_thread_by_status_animating)
        icl_console_print ("<smt_thread_by_status_queue_start"
#if (defined (BASE_THREADSAFE))
" thread=\"%pp\""
#endif
" self=\"%pp\""
" item=\"%pp\""
"/>"
#if (defined (BASE_THREADSAFE))
, apr_os_thread_current ()
#endif
, self, item);
#endif

#if (defined (BASE_TRACE)   || defined (BASE_TRACE_SMT_THREAD_BY_STATUS)   || defined (BASE_TRACE_SMT_THREAD_BY_STATUS_QUEUE))
    icl_trace_record (1, smt_thread_by_status_dump, 8);
#endif

#if (defined (BASE_STATS)  ||  defined (BASE_STATS_SMT_THREAD_BY_STATUS)  ||  defined (BASE_STATS_SMT_THREAD_BY_STATUS_QUEUE))
    icl_stats_inc ("smt_thread_by_status_queue", &s_smt_thread_by_status_queue_stats);
#endif



SMT_THREAD_BY_STATUS_ASSERT_SANE (self);
if (!self->zombie) {

//  If by_status_head is non-null then we are moving within the
//  list to another so don't need to adjust links. Otherwise it
//  is illegitimate to move from one list to another in one step,
//  because we don't have a lock on the other list.
if (! item->by_status_head) {
}
else
if (item->by_status_head != self) {
    icl_console_print ("E: smt_thread item inserted into multiple by_status containers");
    assert (item->by_status_head == self);
}
smt_thread_by_status_unhook (item);
smt_thread_by_status_rehook (item, (smt_thread_t *) self->smt_thread.by_status_prev, &self->smt_thread);
}
else
    rc = -1;                        //  Return error on zombie object.



#if (defined (BASE_TRACE)   || defined (BASE_TRACE_SMT_THREAD_BY_STATUS)   || defined (BASE_TRACE_SMT_THREAD_BY_STATUS_QUEUE))
    icl_trace_record (1, smt_thread_by_status_dump, 0x10000 + 8);
#endif

#if (defined (BASE_ANIMATE)   || defined (BASE_ANIMATE_SMT_THREAD_BY_STATUS)   || defined (BASE_ANIMATE_SMT_THREAD_BY_STATUS_QUEUE))
    if (smt_thread_by_status_animating)
        icl_console_print ("<smt_thread_by_status_queue_finish"
#if (defined (BASE_THREADSAFE))
" thread=\"%pp\""
#endif
" self=\"%pp\""
" item=\"%pp\""
" rc=\"%i\""
"/>"
#if (defined (BASE_THREADSAFE))
, apr_os_thread_current ()
#endif
, self, item, rc);
#endif


    return (rc);
}
/*  -------------------------------------------------------------------------
    smt_thread_by_status_relink_before

    Type: Component method
    Accepts a smt_thread_by_status_t reference and returns zero in case of success,
    1 in case of errors.
    Attaches the item before a specified item. If the item was on another list,
    it is first detached.

    The number of links is automatically adjusted - if the item was previously
    in a list, it remains unchanged; otherwise it is incremented.
    -------------------------------------------------------------------------
 */

int
    smt_thread_by_status_relink_before (
    smt_thread_t * item,                //  Not documented
    smt_thread_t * where                //  Not documented
)
{


    int
        rc = 0;                         //  Return code
    smt_thread_by_status_t *
        self = (smt_thread_by_status_t *) where->by_status_head;  //  The list

#if (defined (BASE_ANIMATE)  ||  defined (BASE_ANIMATE_SMT_THREAD_BY_STATUS)  ||  defined (BASE_ANIMATE_SMT_THREAD_BY_STATUS_RELINK_BEFORE))
    if (smt_thread_by_status_animating)
        icl_console_print ("<smt_thread_by_status_relink_before_start"
#if (defined (BASE_THREADSAFE))
" thread=\"%pp\""
#endif
" item=\"%pp\""
" where=\"%pp\""
"/>"
#if (defined (BASE_THREADSAFE))
, apr_os_thread_current ()
#endif
, item, where);
#endif

#if (defined (BASE_TRACE)   || defined (BASE_TRACE_SMT_THREAD_BY_STATUS)   || defined (BASE_TRACE_SMT_THREAD_BY_STATUS_RELINK_BEFORE))
    icl_trace_record (1, smt_thread_by_status_dump, 9);
#endif

#if (defined (BASE_STATS)  ||  defined (BASE_STATS_SMT_THREAD_BY_STATUS)  ||  defined (BASE_STATS_SMT_THREAD_BY_STATUS_RELINK_BEFORE))
    icl_stats_inc ("smt_thread_by_status_relink_before", &s_smt_thread_by_status_relink_before_stats);
#endif



SMT_THREAD_BY_STATUS_ASSERT_SANE (self);
if (!self->zombie) {

//  If by_status_head is non-null then we are moving within the
//  list to another so don't need to adjust links. Otherwise it
//  is illegitimate to move from one list to another in one step,
//  because we don't have a lock on the other list.
if (! item->by_status_head) {
}
else
if (item->by_status_head != self) {
    icl_console_print ("E: smt_thread item inserted into multiple by_status containers");
    assert (item->by_status_head == self);
}
smt_thread_by_status_unhook (item);
smt_thread_by_status_rehook (item, (smt_thread_t *) where->by_status_prev, where);
}
else
    rc = -1;                        //  Return error on zombie object.



#if (defined (BASE_TRACE)   || defined (BASE_TRACE_SMT_THREAD_BY_STATUS)   || defined (BASE_TRACE_SMT_THREAD_BY_STATUS_RELINK_BEFORE))
    icl_trace_record (1, smt_thread_by_status_dump, 0x10000 + 9);
#endif

#if (defined (BASE_ANIMATE)   || defined (BASE_ANIMATE_SMT_THREAD_BY_STATUS)   || defined (BASE_ANIMATE_SMT_THREAD_BY_STATUS_RELINK_BEFORE))
    if (smt_thread_by_status_animating)
        icl_console_print ("<smt_thread_by_status_relink_before_finish"
#if (defined (BASE_THREADSAFE))
" thread=\"%pp\""
#endif
" item=\"%pp\""
" where=\"%pp\""
" rc=\"%i\""
" self=\"%pp\""
"/>"
#if (defined (BASE_THREADSAFE))
, apr_os_thread_current ()
#endif
, item, where, rc, self);
#endif


    return (rc);
}
/*  -------------------------------------------------------------------------
    smt_thread_by_status_first

    Type: Component method
    Returns the first smt_thread item on the list.    If the list is empty,
    returns null.
    -------------------------------------------------------------------------
 */

smt_thread_t *
    smt_thread_by_status_first_ (
    smt_thread_by_status_t * self,      //  The list
    char * file,                        //  Source file for call
    size_t line                         //  Line number for call
)
{


    smt_thread_t *
        item = NULL;                    //  Not documented

#if (defined (BASE_ANIMATE)  ||  defined (BASE_ANIMATE_SMT_THREAD_BY_STATUS)  ||  defined (BASE_ANIMATE_SMT_THREAD_BY_STATUS_FIRST))
    if (smt_thread_by_status_animating)
        icl_console_print ("<smt_thread_by_status_first_start"
#if (defined (BASE_THREADSAFE))
" thread=\"%pp\""
#endif
" self=\"%pp\""
" file=\"%s\""
" line=\"%u\""
"/>"
#if (defined (BASE_THREADSAFE))
, apr_os_thread_current ()
#endif
, self, file, line);
#endif

#if (defined (BASE_TRACE)   || defined (BASE_TRACE_SMT_THREAD_BY_STATUS)   || defined (BASE_TRACE_SMT_THREAD_BY_STATUS_FIRST))
    icl_trace_record (1, smt_thread_by_status_dump, 10);
#endif

#if (defined (BASE_STATS)  ||  defined (BASE_STATS_SMT_THREAD_BY_STATUS)  ||  defined (BASE_STATS_SMT_THREAD_BY_STATUS_FIRST))
    icl_stats_inc ("smt_thread_by_status_first", &s_smt_thread_by_status_first_stats);
#endif



SMT_THREAD_BY_STATUS_ASSERT_SANE (self);
if (!self->zombie) {

item = (smt_thread_t *) self->smt_thread.by_status_next;
if (item == &self->smt_thread)
    item = NULL;

}



#if (defined (BASE_TRACE)   || defined (BASE_TRACE_SMT_THREAD_BY_STATUS)   || defined (BASE_TRACE_SMT_THREAD_BY_STATUS_FIRST))
    icl_trace_record (1, smt_thread_by_status_dump, 0x10000 + 10);
#endif

#if (defined (BASE_ANIMATE)   || defined (BASE_ANIMATE_SMT_THREAD_BY_STATUS)   || defined (BASE_ANIMATE_SMT_THREAD_BY_STATUS_FIRST))
    if (smt_thread_by_status_animating)
        icl_console_print ("<smt_thread_by_status_first_finish"
#if (defined (BASE_THREADSAFE))
" thread=\"%pp\""
#endif
" self=\"%pp\""
" file=\"%s\""
" line=\"%u\""
" item=\"%pp\""
"/>"
#if (defined (BASE_THREADSAFE))
, apr_os_thread_current ()
#endif
, self, file, line, item);
#endif


    return (item);
}
/*  -------------------------------------------------------------------------
    smt_thread_by_status_last

    Type: Component method
    Returns the last smt_thread item on the list.    If the list is empty,
    returns null.
    -------------------------------------------------------------------------
 */

smt_thread_t *
    smt_thread_by_status_last_ (
    smt_thread_by_status_t * self,      //  The list
    char * file,                        //  Source file for call
    size_t line                         //  Line number for call
)
{


    smt_thread_t *
        item = NULL;                    //  Not documented

#if (defined (BASE_ANIMATE)  ||  defined (BASE_ANIMATE_SMT_THREAD_BY_STATUS)  ||  defined (BASE_ANIMATE_SMT_THREAD_BY_STATUS_LAST))
    if (smt_thread_by_status_animating)
        icl_console_print ("<smt_thread_by_status_last_start"
#if (defined (BASE_THREADSAFE))
" thread=\"%pp\""
#endif
" self=\"%pp\""
" file=\"%s\""
" line=\"%u\""
"/>"
#if (defined (BASE_THREADSAFE))
, apr_os_thread_current ()
#endif
, self, file, line);
#endif

#if (defined (BASE_TRACE)   || defined (BASE_TRACE_SMT_THREAD_BY_STATUS)   || defined (BASE_TRACE_SMT_THREAD_BY_STATUS_LAST))
    icl_trace_record (1, smt_thread_by_status_dump, 11);
#endif

#if (defined (BASE_STATS)  ||  defined (BASE_STATS_SMT_THREAD_BY_STATUS)  ||  defined (BASE_STATS_SMT_THREAD_BY_STATUS_LAST))
    icl_stats_inc ("smt_thread_by_status_last", &s_smt_thread_by_status_last_stats);
#endif



SMT_THREAD_BY_STATUS_ASSERT_SANE (self);
if (!self->zombie) {

item = (smt_thread_t *) self->smt_thread.by_status_prev;
if (item == &self->smt_thread)
    item = NULL;

}



#if (defined (BASE_TRACE)   || defined (BASE_TRACE_SMT_THREAD_BY_STATUS)   || defined (BASE_TRACE_SMT_THREAD_BY_STATUS_LAST))
    icl_trace_record (1, smt_thread_by_status_dump, 0x10000 + 11);
#endif

#if (defined (BASE_ANIMATE)   || defined (BASE_ANIMATE_SMT_THREAD_BY_STATUS)   || defined (BASE_ANIMATE_SMT_THREAD_BY_STATUS_LAST))
    if (smt_thread_by_status_animating)
        icl_console_print ("<smt_thread_by_status_last_finish"
#if (defined (BASE_THREADSAFE))
" thread=\"%pp\""
#endif
" self=\"%pp\""
" file=\"%s\""
" line=\"%u\""
" item=\"%pp\""
"/>"
#if (defined (BASE_THREADSAFE))
, apr_os_thread_current ()
#endif
, self, file, line, item);
#endif


    return (item);
}
/*  -------------------------------------------------------------------------
    smt_thread_by_status_next

    Type: Component method
    Returns the following item on the smt_thread list. If the provided item
    is null, returns the first item.  If there were no (more) items, returns
    null.
    -------------------------------------------------------------------------
 */

smt_thread_t *
    smt_thread_by_status_next_ (
    smt_thread_t * ( * item_p ),        //  Not documented
    char * file,                        //  Source file for call
    size_t line                         //  Line number for call
)
{


    smt_thread_t *
        item = *item_p;                 //  Dereferenced item
    smt_thread_t *
        next = NULL;                    //  The item to return
    smt_thread_by_status_t *
        self = (smt_thread_by_status_t *) item->by_status_head;  //  The list

#if (defined (BASE_ANIMATE)  ||  defined (BASE_ANIMATE_SMT_THREAD_BY_STATUS)  ||  defined (BASE_ANIMATE_SMT_THREAD_BY_STATUS_NEXT))
    if (smt_thread_by_status_animating)
        icl_console_print ("<smt_thread_by_status_next_start"
#if (defined (BASE_THREADSAFE))
" thread=\"%pp\""
#endif
" item=\"%pp\""
" file=\"%s\""
" line=\"%u\""
" item=\"%pp\""
"/>"
#if (defined (BASE_THREADSAFE))
, apr_os_thread_current ()
#endif
, item, file, line, item);
#endif

#if (defined (BASE_TRACE)   || defined (BASE_TRACE_SMT_THREAD_BY_STATUS)   || defined (BASE_TRACE_SMT_THREAD_BY_STATUS_NEXT))
    icl_trace_record (1, smt_thread_by_status_dump, 12);
#endif

#if (defined (BASE_STATS)  ||  defined (BASE_STATS_SMT_THREAD_BY_STATUS)  ||  defined (BASE_STATS_SMT_THREAD_BY_STATUS_NEXT))
    icl_stats_inc ("smt_thread_by_status_next", &s_smt_thread_by_status_next_stats);
#endif



if (self
&&  self == item->by_status_head) { //  Catch case where item moved
    next = (smt_thread_t *) item->by_status_next;
    if (next == &self->smt_thread)
        next = NULL;

}



#if (defined (BASE_TRACE)   || defined (BASE_TRACE_SMT_THREAD_BY_STATUS)   || defined (BASE_TRACE_SMT_THREAD_BY_STATUS_NEXT))
    icl_trace_record (1, smt_thread_by_status_dump, 0x10000 + 12);
#endif

#if (defined (BASE_ANIMATE)   || defined (BASE_ANIMATE_SMT_THREAD_BY_STATUS)   || defined (BASE_ANIMATE_SMT_THREAD_BY_STATUS_NEXT))
    if (smt_thread_by_status_animating)
        icl_console_print ("<smt_thread_by_status_next_finish"
#if (defined (BASE_THREADSAFE))
" thread=\"%pp\""
#endif
" item=\"%pp\""
" file=\"%s\""
" line=\"%u\""
" item=\"%pp\""
" next=\"%pp\""
" self=\"%pp\""
"/>"
#if (defined (BASE_THREADSAFE))
, apr_os_thread_current ()
#endif
, item, file, line, item, next, self);
#endif


    return (next);
}
/*  -------------------------------------------------------------------------
    smt_thread_by_status_prev

    Type: Component method
    Returns the preceding item on the smt_thread list. If the provided item
    is null, returns the last item.  If there were no (more) items, returns
    null.
    -------------------------------------------------------------------------
 */

smt_thread_t *
    smt_thread_by_status_prev_ (
    smt_thread_t * ( * item_p ),        //  Not documented
    char * file,                        //  Source file for call
    size_t line                         //  Line number for call
)
{


    smt_thread_t *
        item = *item_p;                 //  Dereferenced item
    smt_thread_t *
        prev = NULL;                    //  The item to return
    smt_thread_by_status_t *
        self = (smt_thread_by_status_t *) item->by_status_head;  //  The list

#if (defined (BASE_ANIMATE)  ||  defined (BASE_ANIMATE_SMT_THREAD_BY_STATUS)  ||  defined (BASE_ANIMATE_SMT_THREAD_BY_STATUS_PREV))
    if (smt_thread_by_status_animating)
        icl_console_print ("<smt_thread_by_status_prev_start"
#if (defined (BASE_THREADSAFE))
" thread=\"%pp\""
#endif
" item=\"%pp\""
" file=\"%s\""
" line=\"%u\""
" item=\"%pp\""
"/>"
#if (defined (BASE_THREADSAFE))
, apr_os_thread_current ()
#endif
, item, file, line, item);
#endif

#if (defined (BASE_TRACE)   || defined (BASE_TRACE_SMT_THREAD_BY_STATUS)   || defined (BASE_TRACE_SMT_THREAD_BY_STATUS_PREV))
    icl_trace_record (1, smt_thread_by_status_dump, 13);
#endif

#if (defined (BASE_STATS)  ||  defined (BASE_STATS_SMT_THREAD_BY_STATUS)  ||  defined (BASE_STATS_SMT_THREAD_BY_STATUS_PREV))
    icl_stats_inc ("smt_thread_by_status_prev", &s_smt_thread_by_status_prev_stats);
#endif



if (self
&&  self == item->by_status_head) { //  Catch case where item moved
    prev = (smt_thread_t *) item->by_status_prev;
    if (prev == &self->smt_thread)
        prev = NULL;

}



#if (defined (BASE_TRACE)   || defined (BASE_TRACE_SMT_THREAD_BY_STATUS)   || defined (BASE_TRACE_SMT_THREAD_BY_STATUS_PREV))
    icl_trace_record (1, smt_thread_by_status_dump, 0x10000 + 13);
#endif

#if (defined (BASE_ANIMATE)   || defined (BASE_ANIMATE_SMT_THREAD_BY_STATUS)   || defined (BASE_ANIMATE_SMT_THREAD_BY_STATUS_PREV))
    if (smt_thread_by_status_animating)
        icl_console_print ("<smt_thread_by_status_prev_finish"
#if (defined (BASE_THREADSAFE))
" thread=\"%pp\""
#endif
" item=\"%pp\""
" file=\"%s\""
" line=\"%u\""
" item=\"%pp\""
" prev=\"%pp\""
" self=\"%pp\""
"/>"
#if (defined (BASE_THREADSAFE))
, apr_os_thread_current ()
#endif
, item, file, line, item, prev, self);
#endif


    return (prev);
}
/*  -------------------------------------------------------------------------
    smt_thread_by_status_rehook

    Type: Component method
    -------------------------------------------------------------------------
 */

static inline void
    smt_thread_by_status_rehook (
    smt_thread_t * item,                //  Not documented
    smt_thread_t * left,                //  Not documented
    smt_thread_t * right                //  Not documented
)
{
smt_thread_t
    *swap;

#if (defined (BASE_ANIMATE)  ||  defined (BASE_ANIMATE_SMT_THREAD_BY_STATUS)  ||  defined (BASE_ANIMATE_SMT_THREAD_BY_STATUS_REHOOK))
    if (smt_thread_by_status_animating)
        icl_console_print ("<smt_thread_by_status_rehook_start"
#if (defined (BASE_THREADSAFE))
" thread=\"%pp\""
#endif
" item=\"%pp\""
" left=\"%pp\""
" right=\"%pp\""
"/>"
#if (defined (BASE_THREADSAFE))
, apr_os_thread_current ()
#endif
, item, left, right);
#endif

#if (defined (BASE_TRACE_SMT_THREAD_BY_STATUS)   || defined (BASE_TRACE_SMT_THREAD_BY_STATUS_REHOOK))
    icl_trace_record (1, smt_thread_by_status_dump, 14);
#endif

#if (defined (BASE_STATS)  ||  defined (BASE_STATS_SMT_THREAD_BY_STATUS)  ||  defined (BASE_STATS_SMT_THREAD_BY_STATUS_REHOOK))
    icl_stats_inc ("smt_thread_by_status_rehook", &s_smt_thread_by_status_rehook_stats);
#endif

swap = (smt_thread_t *) left->by_status_next;
left->by_status_next = item->by_status_next;
item->by_status_next = swap;

swap = (smt_thread_t *) item->by_status_prev;
item->by_status_prev = right->by_status_prev;
right->by_status_prev = swap;

item->by_status_head = left->by_status_head;
#if (defined (BASE_TRACE_SMT_THREAD_BY_STATUS)   || defined (BASE_TRACE_SMT_THREAD_BY_STATUS_REHOOK))
    icl_trace_record (1, smt_thread_by_status_dump, 0x10000 + 14);
#endif

#if (defined (BASE_ANIMATE_SMT_THREAD_BY_STATUS)   || defined (BASE_ANIMATE_SMT_THREAD_BY_STATUS_REHOOK))
    if (smt_thread_by_status_animating)
        icl_console_print ("<smt_thread_by_status_rehook_finish"
#if (defined (BASE_THREADSAFE))
" thread=\"%pp\""
#endif
" item=\"%pp\""
" left=\"%pp\""
" right=\"%pp\""
"/>"
#if (defined (BASE_THREADSAFE))
, apr_os_thread_current ()
#endif
, item, left, right);
#endif

}
/*  -------------------------------------------------------------------------
    smt_thread_by_status_unhook

    Type: Component method
    -------------------------------------------------------------------------
 */

static inline smt_thread_t *
    smt_thread_by_status_unhook (
    smt_thread_t * item                 //  Not documented
)
{

#if (defined (BASE_ANIMATE)  ||  defined (BASE_ANIMATE_SMT_THREAD_BY_STATUS)  ||  defined (BASE_ANIMATE_SMT_THREAD_BY_STATUS_UNHOOK))
    if (smt_thread_by_status_animating)
        icl_console_print ("<smt_thread_by_status_unhook_start"
#if (defined (BASE_THREADSAFE))
" thread=\"%pp\""
#endif
" item=\"%pp\""
"/>"
#if (defined (BASE_THREADSAFE))
, apr_os_thread_current ()
#endif
, item);
#endif

#if (defined (BASE_TRACE_SMT_THREAD_BY_STATUS)   || defined (BASE_TRACE_SMT_THREAD_BY_STATUS_UNHOOK))
    icl_trace_record (1, smt_thread_by_status_dump, 15);
#endif

#if (defined (BASE_STATS)  ||  defined (BASE_STATS_SMT_THREAD_BY_STATUS)  ||  defined (BASE_STATS_SMT_THREAD_BY_STATUS_UNHOOK))
    icl_stats_inc ("smt_thread_by_status_unhook", &s_smt_thread_by_status_unhook_stats);
#endif

smt_thread_by_status_rehook (item, (smt_thread_t *) item->by_status_prev, (smt_thread_t *) item->by_status_next);
item->by_status_head = NULL;
#if (defined (BASE_TRACE_SMT_THREAD_BY_STATUS)   || defined (BASE_TRACE_SMT_THREAD_BY_STATUS_UNHOOK))
    icl_trace_record (1, smt_thread_by_status_dump, 0x10000 + 15);
#endif

#if (defined (BASE_ANIMATE_SMT_THREAD_BY_STATUS)   || defined (BASE_ANIMATE_SMT_THREAD_BY_STATUS_UNHOOK))
    if (smt_thread_by_status_animating)
        icl_console_print ("<smt_thread_by_status_unhook_finish"
#if (defined (BASE_THREADSAFE))
" thread=\"%pp\""
#endif
" item=\"%pp\""
"/>"
#if (defined (BASE_THREADSAFE))
, apr_os_thread_current ()
#endif
, item);
#endif


    return (item);
}
/*  -------------------------------------------------------------------------
    smt_thread_by_status_terminate

    Type: Component method
    -------------------------------------------------------------------------
 */

void
    smt_thread_by_status_terminate (
void)
{

#if (defined (BASE_ANIMATE)  ||  defined (BASE_ANIMATE_SMT_THREAD_BY_STATUS)  ||  defined (BASE_ANIMATE_SMT_THREAD_BY_STATUS_TERMINATE))
    if (smt_thread_by_status_animating)
        icl_console_print ("<smt_thread_by_status_terminate_start"
#if (defined (BASE_THREADSAFE))
" thread=\"%pp\""
#endif
"/>"
#if (defined (BASE_THREADSAFE))
, apr_os_thread_current ()
#endif
);
#endif

#if (defined (BASE_TRACE)   || defined (BASE_TRACE_SMT_THREAD_BY_STATUS)   || defined (BASE_TRACE_SMT_THREAD_BY_STATUS_TERMINATE))
    icl_trace_record (1, smt_thread_by_status_dump, 16);
#endif

#if (defined (BASE_STATS)  ||  defined (BASE_STATS_SMT_THREAD_BY_STATUS)  ||  defined (BASE_STATS_SMT_THREAD_BY_STATUS_TERMINATE))
    icl_stats_inc ("smt_thread_by_status_terminate", &s_smt_thread_by_status_terminate_stats);
#endif


#if (defined (BASE_TRACE)   || defined (BASE_TRACE_SMT_THREAD_BY_STATUS)   || defined (BASE_TRACE_SMT_THREAD_BY_STATUS_TERMINATE))
    icl_trace_record (1, smt_thread_by_status_dump, 0x10000 + 16);
#endif

#if (defined (BASE_ANIMATE)   || defined (BASE_ANIMATE_SMT_THREAD_BY_STATUS)   || defined (BASE_ANIMATE_SMT_THREAD_BY_STATUS_TERMINATE))
    if (smt_thread_by_status_animating)
        icl_console_print ("<smt_thread_by_status_terminate_finish"
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
    smt_thread_by_status_show

    Type: Component method
    -------------------------------------------------------------------------
 */

void
    smt_thread_by_status_show_ (
    void * item,                        //  The opaque pointer
    int opcode,                         //  The callback opcode
    FILE * trace_file,                  //  File to print to
    char * file,                        //  Source file
    size_t line                         //  Line number
)
{
    smt_thread_by_status_t
        *self;
    int
        container_links;
#if defined (DEBUG) || defined (BASE_HISTORY) || defined (BASE_HISTORY_SMT_THREAD_BY_STATUS)
    qbyte
        history_index;
#endif


#if (defined (BASE_ANIMATE)  ||  defined (BASE_ANIMATE_SMT_THREAD_BY_STATUS)  ||  defined (BASE_ANIMATE_SMT_THREAD_BY_STATUS_SHOW))
    if (smt_thread_by_status_animating)
        icl_console_print ("<smt_thread_by_status_show_start"
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

#if (defined (BASE_TRACE)   || defined (BASE_TRACE_SMT_THREAD_BY_STATUS)   || defined (BASE_TRACE_SMT_THREAD_BY_STATUS_SHOW))
    icl_trace_record (1, smt_thread_by_status_dump, 17);
#endif

#if (defined (BASE_STATS)  ||  defined (BASE_STATS_SMT_THREAD_BY_STATUS)  ||  defined (BASE_STATS_SMT_THREAD_BY_STATUS_SHOW))
    icl_stats_inc ("smt_thread_by_status_show", &s_smt_thread_by_status_show_stats);
#endif

self = item;
container_links = 0;
    assert (opcode == ICL_CALLBACK_DUMP);

    fprintf (trace_file, "    <smt_thread_by_status zombie = \"%u\" links = \"%u\" containers = \"%u\" file = \"%s\" line = \"%lu\"  pointer = \"%p\" />\n", self->zombie, self->links, container_links, file, (unsigned long) line, self);
#if defined (DEBUG) || defined (BASE_HISTORY) || defined (BASE_HISTORY_SMT_THREAD_BY_STATUS)
    if (self->history_last > SMT_THREAD_BY_STATUS_HISTORY_LENGTH) {
        fprintf (trace_file, "        <!-- possess history too large (%d) - call iMatix-tech -->\n",
            self->history_last);
        history_index = (self->history_last + 1) % SMT_THREAD_BY_STATUS_HISTORY_LENGTH;
        self->history_last %= SMT_THREAD_BY_STATUS_HISTORY_LENGTH;
    }
    else
        history_index = 0;

    for (; history_index != self->history_last; history_index = (history_index + 1) % SMT_THREAD_BY_STATUS_HISTORY_LENGTH) {
        fprintf (trace_file, "       <%s file = \"%s\" line = \"%lu\" links = \"%lu\" />\n",
            self->history_type [history_index],
            self->history_file [history_index],
            (unsigned long) self->history_line  [history_index],
            (unsigned long) self->history_links [history_index]);
    }
    fprintf (trace_file, "    </smt_thread_by_status>\n");
#endif

#if (defined (BASE_TRACE)   || defined (BASE_TRACE_SMT_THREAD_BY_STATUS)   || defined (BASE_TRACE_SMT_THREAD_BY_STATUS_SHOW))
    icl_trace_record (1, smt_thread_by_status_dump, 0x10000 + 17);
#endif

#if (defined (BASE_ANIMATE)   || defined (BASE_ANIMATE_SMT_THREAD_BY_STATUS)   || defined (BASE_ANIMATE_SMT_THREAD_BY_STATUS_SHOW))
    if (smt_thread_by_status_animating)
        icl_console_print ("<smt_thread_by_status_show_finish"
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
    smt_thread_by_status_destroy

    Type: Component method
    Destroys a smt_thread_by_status_t object. Takes the address of a
    smt_thread_by_status_t reference (a pointer to a pointer) and nullifies the
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
    smt_thread_by_status_destroy_ (
    smt_thread_by_status_t * ( * self_p ),   //  Reference to object reference
    char * file,                        //  Source fileSource file
    size_t line                         //  Line numberLine number
)
{
    smt_thread_by_status_t *
        self = *self_p;                 //  Dereferenced Reference to object reference

#if (defined (BASE_ANIMATE)  ||  defined (BASE_ANIMATE_SMT_THREAD_BY_STATUS)  ||  defined (BASE_ANIMATE_SMT_THREAD_BY_STATUS_DESTROY_PUBLIC))
    if (smt_thread_by_status_animating)
        icl_console_print ("<smt_thread_by_status_destroy_public_start"
#if (defined (BASE_THREADSAFE))
" thread=\"%pp\""
#endif
" self=\"%pp\""
" self=\"%pp\""
" file=\"%s\""
" line=\"%i\""
" links=\"%i\""
" zombie=\"%i\""
"/>"
#if (defined (BASE_THREADSAFE))
, apr_os_thread_current ()
#endif
, self, self, file, line, self?self->links:0, self?self->zombie:0);
#endif

#if (defined (BASE_TRACE)   || defined (BASE_TRACE_SMT_THREAD_BY_STATUS)   || defined (BASE_TRACE_SMT_THREAD_BY_STATUS_DESTROY_PUBLIC))
    icl_trace_record (1, smt_thread_by_status_dump, 18);
#endif

#if (defined (BASE_STATS)  ||  defined (BASE_STATS_SMT_THREAD_BY_STATUS)  ||  defined (BASE_STATS_SMT_THREAD_BY_STATUS_DESTROY_PUBLIC))
    icl_stats_inc ("smt_thread_by_status_destroy", &s_smt_thread_by_status_destroy_stats);
#endif

if (self) {
    if (icl_atomic_cas32 (&self->zombie, TRUE, FALSE) == FALSE)
        smt_thread_by_status_annihilate_ (self_p, file, line);

    if (self->links == 0) {
        icl_system_panic ("smt_thread_by_status", "E: missing link on smt_thread_by_status object");
        smt_thread_by_status_show_ (self, ICL_CALLBACK_DUMP, stderr, file, line);
        abort ();
    }

    if (icl_atomic_dec32 ((volatile qbyte *) &self->links) == 0)
        smt_thread_by_status_free_ ((smt_thread_by_status_t *) self, file, line);
    *self_p = NULL;
}
#if (defined (BASE_TRACE)   || defined (BASE_TRACE_SMT_THREAD_BY_STATUS)   || defined (BASE_TRACE_SMT_THREAD_BY_STATUS_DESTROY_PUBLIC))
    icl_trace_record (1, smt_thread_by_status_dump, 0x10000 + 18);
#endif

#if (defined (BASE_ANIMATE)   || defined (BASE_ANIMATE_SMT_THREAD_BY_STATUS)   || defined (BASE_ANIMATE_SMT_THREAD_BY_STATUS_DESTROY_PUBLIC))
    if (smt_thread_by_status_animating)
        icl_console_print ("<smt_thread_by_status_destroy_public_finish"
#if (defined (BASE_THREADSAFE))
" thread=\"%pp\""
#endif
" self=\"%pp\""
" self=\"%pp\""
" file=\"%s\""
" line=\"%i\""
" links=\"%i\""
" zombie=\"%i\""
"/>"
#if (defined (BASE_THREADSAFE))
, apr_os_thread_current ()
#endif
, self, self, file, line, self?self->links:0, self?self->zombie:0);
#endif

}
/*  -------------------------------------------------------------------------
    smt_thread_by_status_alloc

    Type: Component method
    -------------------------------------------------------------------------
 */

static smt_thread_by_status_t *
    smt_thread_by_status_alloc_ (
    char * file,                        //  Source file for call
    size_t line                         //  Line number for call
)
{

    smt_thread_by_status_t *
        self = NULL;                    //  Object reference

#if (defined (BASE_ANIMATE)  ||  defined (BASE_ANIMATE_SMT_THREAD_BY_STATUS)  ||  defined (BASE_ANIMATE_SMT_THREAD_BY_STATUS_ALLOC))
    if (smt_thread_by_status_animating)
        icl_console_print ("<smt_thread_by_status_alloc_start"
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

#if (defined (BASE_TRACE)   || defined (BASE_TRACE_SMT_THREAD_BY_STATUS)   || defined (BASE_TRACE_SMT_THREAD_BY_STATUS_ALLOC))
    icl_trace_record (1, smt_thread_by_status_dump, 19);
#endif

#if (defined (BASE_STATS)  ||  defined (BASE_STATS_SMT_THREAD_BY_STATUS)  ||  defined (BASE_STATS_SMT_THREAD_BY_STATUS_ALLOC))
    icl_stats_inc ("smt_thread_by_status_alloc", &s_smt_thread_by_status_alloc_stats);
#endif

//  Initialise cache if necessary
if (!s_cache)
    smt_thread_by_status_cache_initialise ();

self = (smt_thread_by_status_t *) icl_mem_cache_alloc_ (s_cache, file, line);
if (self)
    memset (self, 0, sizeof (smt_thread_by_status_t));


#if (defined (BASE_TRACE)   || defined (BASE_TRACE_SMT_THREAD_BY_STATUS)   || defined (BASE_TRACE_SMT_THREAD_BY_STATUS_ALLOC))
    icl_trace_record (1, smt_thread_by_status_dump, 0x10000 + 19);
#endif

#if (defined (BASE_ANIMATE)   || defined (BASE_ANIMATE_SMT_THREAD_BY_STATUS)   || defined (BASE_ANIMATE_SMT_THREAD_BY_STATUS_ALLOC))
    if (smt_thread_by_status_animating)
        icl_console_print ("<smt_thread_by_status_alloc_finish"
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
    smt_thread_by_status_free

    Type: Component method
    Freess a smt_thread_by_status_t object.
    -------------------------------------------------------------------------
 */

static void
    smt_thread_by_status_free_ (
    smt_thread_by_status_t * self,      //  Object reference
    char * file,                        //  Source file
    size_t line                         //  Line number
)
{
#if defined (DEBUG) || defined (BASE_HISTORY) || defined (BASE_HISTORY_SMT_THREAD_BY_STATUS)
    int
        history_last;
#endif


#if (defined (BASE_ANIMATE)  ||  defined (BASE_ANIMATE_SMT_THREAD_BY_STATUS)  ||  defined (BASE_ANIMATE_SMT_THREAD_BY_STATUS_FREE))
    if (smt_thread_by_status_animating)
        icl_console_print ("<smt_thread_by_status_free_start"
#if (defined (BASE_THREADSAFE))
" thread=\"%pp\""
#endif
" self=\"%pp\""
" file=\"%s\""
" line=\"%u\""
"/>"
#if (defined (BASE_THREADSAFE))
, apr_os_thread_current ()
#endif
, self, file, line);
#endif

#if (defined (BASE_TRACE)   || defined (BASE_TRACE_SMT_THREAD_BY_STATUS)   || defined (BASE_TRACE_SMT_THREAD_BY_STATUS_FREE))
    icl_trace_record (1, smt_thread_by_status_dump, 20);
#endif

#if (defined (BASE_STATS)  ||  defined (BASE_STATS_SMT_THREAD_BY_STATUS)  ||  defined (BASE_STATS_SMT_THREAD_BY_STATUS_FREE))
    icl_stats_inc ("smt_thread_by_status_free", &s_smt_thread_by_status_free_stats);
#endif

    if (self) {
#if defined (DEBUG) || defined (BASE_HISTORY) || defined (BASE_HISTORY_SMT_THREAD_BY_STATUS)
        //  Track possession operation in history
        history_last = icl_atomic_inc32 ((volatile qbyte *) &self->history_last) + 1;
        self->history_file  [history_last % SMT_THREAD_BY_STATUS_HISTORY_LENGTH] = file;
        self->history_line  [history_last % SMT_THREAD_BY_STATUS_HISTORY_LENGTH] = line;
        self->history_type  [history_last % SMT_THREAD_BY_STATUS_HISTORY_LENGTH] = "free";
        self->history_links [history_last % SMT_THREAD_BY_STATUS_HISTORY_LENGTH] = self->links;
#endif

        memset (&self->object_tag, 0, sizeof (smt_thread_by_status_t) - ((byte *) &self->object_tag - (byte *) self));
//        memset (self, 0, sizeof (smt_thread_by_status_t));
        self->object_tag = SMT_THREAD_BY_STATUS_DEAD;
        icl_mem_free (self);
    }
    self = NULL;
#if (defined (BASE_TRACE)   || defined (BASE_TRACE_SMT_THREAD_BY_STATUS)   || defined (BASE_TRACE_SMT_THREAD_BY_STATUS_FREE))
    icl_trace_record (1, smt_thread_by_status_dump, 0x10000 + 20);
#endif

#if (defined (BASE_ANIMATE)   || defined (BASE_ANIMATE_SMT_THREAD_BY_STATUS)   || defined (BASE_ANIMATE_SMT_THREAD_BY_STATUS_FREE))
    if (smt_thread_by_status_animating)
        icl_console_print ("<smt_thread_by_status_free_finish"
#if (defined (BASE_THREADSAFE))
" thread=\"%pp\""
#endif
" self=\"%pp\""
" file=\"%s\""
" line=\"%u\""
"/>"
#if (defined (BASE_THREADSAFE))
, apr_os_thread_current ()
#endif
, self, file, line);
#endif

}
/*  -------------------------------------------------------------------------
    smt_thread_by_status_link

    Type: Component method
    Adds a link (reference count) to an object.

    If the object has been zombified (ie destroyed while extra links were present),
    this method returns NULL and does not add any links.

    This method does not lock the object.
    -------------------------------------------------------------------------
 */

smt_thread_by_status_t *
    smt_thread_by_status_link_ (
    smt_thread_by_status_t * self,      //  Not documented
    char * file,                        //  Source file for call
    size_t line                         //  Line number for call
)
{
#if defined (DEBUG) || defined (BASE_HISTORY) || defined (BASE_HISTORY_SMT_THREAD_BY_STATUS)
    int
        history_last;
#endif

#if (defined (BASE_ANIMATE)  ||  defined (BASE_ANIMATE_SMT_THREAD_BY_STATUS)  ||  defined (BASE_ANIMATE_SMT_THREAD_BY_STATUS_LINK))
    if (smt_thread_by_status_animating)
        icl_console_print ("<smt_thread_by_status_link_start"
#if (defined (BASE_THREADSAFE))
" thread=\"%pp\""
#endif
" self=\"%pp\""
" file=\"%s\""
" line=\"%u\""
" links=\"%i\""
" zombie=\"%i\""
"/>"
#if (defined (BASE_THREADSAFE))
, apr_os_thread_current ()
#endif
, self, file, line, self?self->links:0, self?self->zombie:0);
#endif

#if (defined (BASE_TRACE)   || defined (BASE_TRACE_SMT_THREAD_BY_STATUS)   || defined (BASE_TRACE_SMT_THREAD_BY_STATUS_LINK))
    icl_trace_record (1, smt_thread_by_status_dump, 21);
#endif

#if (defined (BASE_STATS)  ||  defined (BASE_STATS_SMT_THREAD_BY_STATUS)  ||  defined (BASE_STATS_SMT_THREAD_BY_STATUS_LINK))
    icl_stats_inc ("smt_thread_by_status_link", &s_smt_thread_by_status_link_stats);
#endif

    if (self) {
        SMT_THREAD_BY_STATUS_ASSERT_SANE (self);
        if (file)
            icl_mem_possess_ (self, file, line);
        icl_atomic_inc32 ((volatile qbyte *) &self->links);
#if defined (DEBUG) || defined (BASE_HISTORY) || defined (BASE_HISTORY_SMT_THREAD_BY_STATUS)
        //  Track possession operation in history
        history_last = icl_atomic_inc32 ((volatile qbyte *) &self->history_last) + 1;
        self->history_file  [history_last % SMT_THREAD_BY_STATUS_HISTORY_LENGTH] = file;
        self->history_line  [history_last % SMT_THREAD_BY_STATUS_HISTORY_LENGTH] = line;
        self->history_type  [history_last % SMT_THREAD_BY_STATUS_HISTORY_LENGTH] = "link";
        self->history_links [history_last % SMT_THREAD_BY_STATUS_HISTORY_LENGTH] = self->links;
#endif
    }
#if (defined (BASE_TRACE)   || defined (BASE_TRACE_SMT_THREAD_BY_STATUS)   || defined (BASE_TRACE_SMT_THREAD_BY_STATUS_LINK))
    icl_trace_record (1, smt_thread_by_status_dump, 0x10000 + 21);
#endif

#if (defined (BASE_ANIMATE)   || defined (BASE_ANIMATE_SMT_THREAD_BY_STATUS)   || defined (BASE_ANIMATE_SMT_THREAD_BY_STATUS_LINK))
    if (smt_thread_by_status_animating)
        icl_console_print ("<smt_thread_by_status_link_finish"
#if (defined (BASE_THREADSAFE))
" thread=\"%pp\""
#endif
" self=\"%pp\""
" file=\"%s\""
" line=\"%u\""
" links=\"%i\""
" zombie=\"%i\""
"/>"
#if (defined (BASE_THREADSAFE))
, apr_os_thread_current ()
#endif
, self, file, line, self?self->links:0, self?self->zombie:0);
#endif


    return (self);
}
/*  -------------------------------------------------------------------------
    smt_thread_by_status_unlink

    Type: Component method
    Removes a link (reference count) to an object.  Sets the object pointer to NULL
    to indicate that it is no longer valid.
    -------------------------------------------------------------------------
 */

void
    smt_thread_by_status_unlink_ (
    smt_thread_by_status_t * ( * self_p ),   //  Reference to object reference
    char * file,                        //  Source file for call
    size_t line                         //  Line number for call
)
{
#if defined (DEBUG) || defined (BASE_HISTORY) || defined (BASE_HISTORY_SMT_THREAD_BY_STATUS)
    int
        history_last;
#endif

    smt_thread_by_status_t *
        self = *self_p;                 //  Dereferenced Reference to object reference

#if (defined (BASE_ANIMATE)  ||  defined (BASE_ANIMATE_SMT_THREAD_BY_STATUS)  ||  defined (BASE_ANIMATE_SMT_THREAD_BY_STATUS_UNLINK))
    if (smt_thread_by_status_animating)
        icl_console_print ("<smt_thread_by_status_unlink_start"
#if (defined (BASE_THREADSAFE))
" thread=\"%pp\""
#endif
" self=\"%pp\""
" file=\"%s\""
" line=\"%i\""
" links=\"%i\""
" zombie=\"%i\""
"/>"
#if (defined (BASE_THREADSAFE))
, apr_os_thread_current ()
#endif
, self, file, line, self?self->links:0, self?self->zombie:0);
#endif

#if (defined (BASE_TRACE)   || defined (BASE_TRACE_SMT_THREAD_BY_STATUS)   || defined (BASE_TRACE_SMT_THREAD_BY_STATUS_UNLINK))
    icl_trace_record (1, smt_thread_by_status_dump, 22);
#endif

#if (defined (BASE_STATS)  ||  defined (BASE_STATS_SMT_THREAD_BY_STATUS)  ||  defined (BASE_STATS_SMT_THREAD_BY_STATUS_UNLINK))
    icl_stats_inc ("smt_thread_by_status_unlink", &s_smt_thread_by_status_unlink_stats);
#endif

    if (self) {
        SMT_THREAD_BY_STATUS_ASSERT_SANE (self);
        if (self->links == 0) {
            icl_system_panic ("smt_thread_by_status", "E: missing link on smt_thread_by_status object");
            smt_thread_by_status_show_ (self, ICL_CALLBACK_DUMP, stderr, file, line);
            abort ();
        }

#if defined (DEBUG) || defined (BASE_HISTORY) || defined (BASE_HISTORY_SMT_THREAD_BY_STATUS)
        //  Track possession operation in history.  Pre-empt value of links
        //  after operation; otherwise race condition can result in writing
        //  to freed memory.
        history_last = icl_atomic_inc32 ((volatile qbyte *) &self->history_last) + 1;
        self->history_file  [history_last % SMT_THREAD_BY_STATUS_HISTORY_LENGTH] = file;
        self->history_line  [history_last % SMT_THREAD_BY_STATUS_HISTORY_LENGTH] = line;
        self->history_type  [history_last % SMT_THREAD_BY_STATUS_HISTORY_LENGTH] = "unlink";
        self->history_links [history_last % SMT_THREAD_BY_STATUS_HISTORY_LENGTH] = self->links - 1;
#endif
        if (icl_atomic_dec32 ((volatile qbyte *) &self->links) == 0) {

        if (icl_atomic_cas32 (&self->zombie, TRUE, FALSE) == FALSE)
            smt_thread_by_status_annihilate_ (self_p, file, line);
        smt_thread_by_status_free_ ((smt_thread_by_status_t *) self, file, line);
    }
    *self_p = NULL;
}
#if (defined (BASE_TRACE)   || defined (BASE_TRACE_SMT_THREAD_BY_STATUS)   || defined (BASE_TRACE_SMT_THREAD_BY_STATUS_UNLINK))
    icl_trace_record (1, smt_thread_by_status_dump, 0x10000 + 22);
#endif

#if (defined (BASE_ANIMATE)   || defined (BASE_ANIMATE_SMT_THREAD_BY_STATUS)   || defined (BASE_ANIMATE_SMT_THREAD_BY_STATUS_UNLINK))
    if (smt_thread_by_status_animating)
        icl_console_print ("<smt_thread_by_status_unlink_finish"
#if (defined (BASE_THREADSAFE))
" thread=\"%pp\""
#endif
" self=\"%pp\""
" file=\"%s\""
" line=\"%i\""
" links=\"%i\""
" zombie=\"%i\""
"/>"
#if (defined (BASE_THREADSAFE))
, apr_os_thread_current ()
#endif
, self, file, line, self?self->links:0, self?self->zombie:0);
#endif

}
/*  -------------------------------------------------------------------------
    smt_thread_by_status_cache_initialise

    Type: Component method
    Initialise the cache and register purge method with the meta-cache.
    -------------------------------------------------------------------------
 */

static void
    smt_thread_by_status_cache_initialise (
void)
{

#if (defined (BASE_ANIMATE)  ||  defined (BASE_ANIMATE_SMT_THREAD_BY_STATUS)  ||  defined (BASE_ANIMATE_SMT_THREAD_BY_STATUS_CACHE_INITIALISE))
    if (smt_thread_by_status_animating)
        icl_console_print ("<smt_thread_by_status_cache_initialise_start"
#if (defined (BASE_THREADSAFE))
" thread=\"%pp\""
#endif
"/>"
#if (defined (BASE_THREADSAFE))
, apr_os_thread_current ()
#endif
);
#endif

#if (defined (BASE_TRACE)   || defined (BASE_TRACE_SMT_THREAD_BY_STATUS)   || defined (BASE_TRACE_SMT_THREAD_BY_STATUS_CACHE_INITIALISE))
    icl_trace_record (1, smt_thread_by_status_dump, 23);
#endif

#if (defined (BASE_STATS)  ||  defined (BASE_STATS_SMT_THREAD_BY_STATUS)  ||  defined (BASE_STATS_SMT_THREAD_BY_STATUS_CACHE_INITIALISE))
    icl_stats_inc ("smt_thread_by_status_cache_initialise", &s_smt_thread_by_status_cache_initialise_stats);
#endif

s_cache = icl_cache_get (sizeof (smt_thread_by_status_t));
icl_system_register (smt_thread_by_status_cache_purge, smt_thread_by_status_cache_terminate);
#if (defined (BASE_TRACE)   || defined (BASE_TRACE_SMT_THREAD_BY_STATUS)   || defined (BASE_TRACE_SMT_THREAD_BY_STATUS_CACHE_INITIALISE))
    icl_trace_record (1, smt_thread_by_status_dump, 0x10000 + 23);
#endif

#if (defined (BASE_ANIMATE)   || defined (BASE_ANIMATE_SMT_THREAD_BY_STATUS)   || defined (BASE_ANIMATE_SMT_THREAD_BY_STATUS_CACHE_INITIALISE))
    if (smt_thread_by_status_animating)
        icl_console_print ("<smt_thread_by_status_cache_initialise_finish"
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
    smt_thread_by_status_cache_purge

    Type: Component method
    -------------------------------------------------------------------------
 */

static void
    smt_thread_by_status_cache_purge (
void)
{

#if (defined (BASE_ANIMATE)  ||  defined (BASE_ANIMATE_SMT_THREAD_BY_STATUS)  ||  defined (BASE_ANIMATE_SMT_THREAD_BY_STATUS_CACHE_PURGE))
    if (smt_thread_by_status_animating)
        icl_console_print ("<smt_thread_by_status_cache_purge_start"
#if (defined (BASE_THREADSAFE))
" thread=\"%pp\""
#endif
"/>"
#if (defined (BASE_THREADSAFE))
, apr_os_thread_current ()
#endif
);
#endif

#if (defined (BASE_TRACE)   || defined (BASE_TRACE_SMT_THREAD_BY_STATUS)   || defined (BASE_TRACE_SMT_THREAD_BY_STATUS_CACHE_PURGE))
    icl_trace_record (1, smt_thread_by_status_dump, 24);
#endif

#if (defined (BASE_STATS)  ||  defined (BASE_STATS_SMT_THREAD_BY_STATUS)  ||  defined (BASE_STATS_SMT_THREAD_BY_STATUS_CACHE_PURGE))
    icl_stats_inc ("smt_thread_by_status_cache_purge", &s_smt_thread_by_status_cache_purge_stats);
#endif

icl_mem_cache_purge (s_cache);

#if (defined (BASE_TRACE)   || defined (BASE_TRACE_SMT_THREAD_BY_STATUS)   || defined (BASE_TRACE_SMT_THREAD_BY_STATUS_CACHE_PURGE))
    icl_trace_record (1, smt_thread_by_status_dump, 0x10000 + 24);
#endif

#if (defined (BASE_ANIMATE)   || defined (BASE_ANIMATE_SMT_THREAD_BY_STATUS)   || defined (BASE_ANIMATE_SMT_THREAD_BY_STATUS_CACHE_PURGE))
    if (smt_thread_by_status_animating)
        icl_console_print ("<smt_thread_by_status_cache_purge_finish"
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
    smt_thread_by_status_cache_terminate

    Type: Component method
    -------------------------------------------------------------------------
 */

static void
    smt_thread_by_status_cache_terminate (
void)
{

#if (defined (BASE_ANIMATE)  ||  defined (BASE_ANIMATE_SMT_THREAD_BY_STATUS)  ||  defined (BASE_ANIMATE_SMT_THREAD_BY_STATUS_CACHE_TERMINATE))
    if (smt_thread_by_status_animating)
        icl_console_print ("<smt_thread_by_status_cache_terminate_start"
#if (defined (BASE_THREADSAFE))
" thread=\"%pp\""
#endif
"/>"
#if (defined (BASE_THREADSAFE))
, apr_os_thread_current ()
#endif
);
#endif

#if (defined (BASE_TRACE)   || defined (BASE_TRACE_SMT_THREAD_BY_STATUS)   || defined (BASE_TRACE_SMT_THREAD_BY_STATUS_CACHE_TERMINATE))
    icl_trace_record (1, smt_thread_by_status_dump, 25);
#endif

#if (defined (BASE_STATS)  ||  defined (BASE_STATS_SMT_THREAD_BY_STATUS)  ||  defined (BASE_STATS_SMT_THREAD_BY_STATUS_CACHE_TERMINATE))
    icl_stats_inc ("smt_thread_by_status_cache_terminate", &s_smt_thread_by_status_cache_terminate_stats);
#endif

s_cache = NULL;

#if (defined (BASE_TRACE)   || defined (BASE_TRACE_SMT_THREAD_BY_STATUS)   || defined (BASE_TRACE_SMT_THREAD_BY_STATUS_CACHE_TERMINATE))
    icl_trace_record (1, smt_thread_by_status_dump, 0x10000 + 25);
#endif

#if (defined (BASE_ANIMATE)   || defined (BASE_ANIMATE_SMT_THREAD_BY_STATUS)   || defined (BASE_ANIMATE_SMT_THREAD_BY_STATUS_CACHE_TERMINATE))
    if (smt_thread_by_status_animating)
        icl_console_print ("<smt_thread_by_status_cache_terminate_finish"
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
    smt_thread_by_status_show_animation

    Type: Component method
    Enables animation of the component. Animation is sent to stdout.
    To enable animation you must generate using the option -animate:1.
    -------------------------------------------------------------------------
 */

void
    smt_thread_by_status_show_animation (
    Bool enabled                        //  Are we enabling or disabling animation?
)
{

#if (defined (BASE_ANIMATE)  ||  defined (BASE_ANIMATE_SMT_THREAD_BY_STATUS)  ||  defined (BASE_ANIMATE_SMT_THREAD_BY_STATUS_SHOW_ANIMATION))
    if (smt_thread_by_status_animating)
        icl_console_print ("<smt_thread_by_status_show_animation_start"
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

#if (defined (BASE_TRACE)   || defined (BASE_TRACE_SMT_THREAD_BY_STATUS)   || defined (BASE_TRACE_SMT_THREAD_BY_STATUS_SHOW_ANIMATION))
    icl_trace_record (1, smt_thread_by_status_dump, 26);
#endif

#if (defined (BASE_STATS)  ||  defined (BASE_STATS_SMT_THREAD_BY_STATUS)  ||  defined (BASE_STATS_SMT_THREAD_BY_STATUS_SHOW_ANIMATION))
    icl_stats_inc ("smt_thread_by_status_show_animation", &s_smt_thread_by_status_show_animation_stats);
#endif

smt_thread_by_status_animating = enabled;
#if (defined (BASE_TRACE)   || defined (BASE_TRACE_SMT_THREAD_BY_STATUS)   || defined (BASE_TRACE_SMT_THREAD_BY_STATUS_SHOW_ANIMATION))
    icl_trace_record (1, smt_thread_by_status_dump, 0x10000 + 26);
#endif

#if (defined (BASE_ANIMATE)   || defined (BASE_ANIMATE_SMT_THREAD_BY_STATUS)   || defined (BASE_ANIMATE_SMT_THREAD_BY_STATUS_SHOW_ANIMATION))
    if (smt_thread_by_status_animating)
        icl_console_print ("<smt_thread_by_status_show_animation_finish"
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
    smt_thread_by_status_new_in_scope

    Type: Component method
    -------------------------------------------------------------------------
 */

void
    smt_thread_by_status_new_in_scope_ (
    smt_thread_by_status_t * * self_p,   //  Not documented
    icl_scope_t * _scope,               //  Not documented
    char * file,                        //  Source file for call
    size_t line                         //  Line number for call
)
{
    icl_destroy_t *
        _destroy;                       //  Not documented

#if (defined (BASE_ANIMATE)  ||  defined (BASE_ANIMATE_SMT_THREAD_BY_STATUS)  ||  defined (BASE_ANIMATE_SMT_THREAD_BY_STATUS_NEW_IN_SCOPE))
    if (smt_thread_by_status_animating)
        icl_console_print ("<smt_thread_by_status_new_in_scope_start"
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

#if (defined (BASE_TRACE)   || defined (BASE_TRACE_SMT_THREAD_BY_STATUS)   || defined (BASE_TRACE_SMT_THREAD_BY_STATUS_NEW_IN_SCOPE))
    icl_trace_record (1, smt_thread_by_status_dump, 27);
#endif

#if (defined (BASE_STATS)  ||  defined (BASE_STATS_SMT_THREAD_BY_STATUS)  ||  defined (BASE_STATS_SMT_THREAD_BY_STATUS_NEW_IN_SCOPE))
    icl_stats_inc ("smt_thread_by_status_new_in_scope", &s_smt_thread_by_status_new_in_scope_stats);
#endif

*self_p = smt_thread_by_status_new_ (file,line);

if (*self_p) {
    _destroy = icl_destroy_new   ((void * *) self_p, (icl_destructor_fn *) smt_thread_by_status_destroy_);
    icl_destroy_list_queue (_scope, _destroy);
    icl_destroy_unlink (&_destroy);
}
#if (defined (BASE_TRACE)   || defined (BASE_TRACE_SMT_THREAD_BY_STATUS)   || defined (BASE_TRACE_SMT_THREAD_BY_STATUS_NEW_IN_SCOPE))
    icl_trace_record (1, smt_thread_by_status_dump, 0x10000 + 27);
#endif

#if (defined (BASE_ANIMATE)   || defined (BASE_ANIMATE_SMT_THREAD_BY_STATUS)   || defined (BASE_ANIMATE_SMT_THREAD_BY_STATUS_NEW_IN_SCOPE))
    if (smt_thread_by_status_animating)
        icl_console_print ("<smt_thread_by_status_new_in_scope_finish"
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
#if (defined (BASE_TRACE)   || defined (BASE_TRACE_SMT_THREAD_BY_STATUS)   || defined (BASE_TRACE_SMT_THREAD_BY_STATUS_REMOVE)   || defined (BASE_TRACE_SMT_THREAD_BY_STATUS_SELFTEST)   || defined (BASE_TRACE_SMT_THREAD_BY_STATUS_NEW)   || defined (BASE_TRACE_SMT_THREAD_BY_STATUS_DESTROY)   || defined (BASE_TRACE_SMT_THREAD_BY_STATUS_ISEMPTY)   || defined (BASE_TRACE_SMT_THREAD_BY_STATUS_PUSH)   || defined (BASE_TRACE_SMT_THREAD_BY_STATUS_POP)   || defined (BASE_TRACE_SMT_THREAD_BY_STATUS_QUEUE)   || defined (BASE_TRACE_SMT_THREAD_BY_STATUS_RELINK_BEFORE)   || defined (BASE_TRACE_SMT_THREAD_BY_STATUS_FIRST)   || defined (BASE_TRACE_SMT_THREAD_BY_STATUS_LAST)   || defined (BASE_TRACE_SMT_THREAD_BY_STATUS_NEXT)   || defined (BASE_TRACE_SMT_THREAD_BY_STATUS_PREV)   || defined (BASE_TRACE_SMT_THREAD_BY_STATUS_REHOOK)   || defined (BASE_TRACE_SMT_THREAD_BY_STATUS_UNHOOK)   || defined (BASE_TRACE_SMT_THREAD_BY_STATUS_TERMINATE)   || defined (BASE_TRACE_SMT_THREAD_BY_STATUS_SHOW)   || defined (BASE_TRACE_SMT_THREAD_BY_STATUS_DESTROY_PUBLIC)   || defined (BASE_TRACE_SMT_THREAD_BY_STATUS_ALLOC)   || defined (BASE_TRACE_SMT_THREAD_BY_STATUS_FREE)   || defined (BASE_TRACE_SMT_THREAD_BY_STATUS_LINK)   || defined (BASE_TRACE_SMT_THREAD_BY_STATUS_UNLINK)   || defined (BASE_TRACE_SMT_THREAD_BY_STATUS_CACHE_INITIALISE)   || defined (BASE_TRACE_SMT_THREAD_BY_STATUS_CACHE_PURGE)   || defined (BASE_TRACE_SMT_THREAD_BY_STATUS_CACHE_TERMINATE)   || defined (BASE_TRACE_SMT_THREAD_BY_STATUS_SHOW_ANIMATION)   || defined (BASE_TRACE_SMT_THREAD_BY_STATUS_NEW_IN_SCOPE) )
void
smt_thread_by_status_dump (icl_os_thread_t thread, apr_time_t time, qbyte info)
{
    dbyte
        method = info & 0xFFFF;
    char
        *method_name = NULL;

    switch (method) {
        case 1: method_name = "remove"; break;
        case 2: method_name = "selftest"; break;
        case 3: method_name = "new"; break;
        case 4: method_name = "destroy"; break;
        case 5: method_name = "isempty"; break;
        case 6: method_name = "push"; break;
        case 7: method_name = "pop"; break;
        case 8: method_name = "queue"; break;
        case 9: method_name = "relink before"; break;
        case 10: method_name = "first"; break;
        case 11: method_name = "last"; break;
        case 12: method_name = "next"; break;
        case 13: method_name = "prev"; break;
        case 14: method_name = "rehook"; break;
        case 15: method_name = "unhook"; break;
        case 16: method_name = "terminate"; break;
        case 17: method_name = "show"; break;
        case 18: method_name = "destroy public"; break;
        case 19: method_name = "alloc"; break;
        case 20: method_name = "free"; break;
        case 21: method_name = "link"; break;
        case 22: method_name = "unlink"; break;
        case 23: method_name = "cache initialise"; break;
        case 24: method_name = "cache purge"; break;
        case 25: method_name = "cache terminate"; break;
        case 26: method_name = "show animation"; break;
        case 27: method_name = "new in scope"; break;
    }
    icl_console_print_thread_time (thread, time,
                                   "smt_thread_by_status %s%s",
                                   (info > 0xFFFF) ? "/" : "",
                                   method_name);
}
#endif

//  Embed the version information in the resulting binary

char *EMBED__smt_thread_by_status_version_start  = "VeRsIoNsTaRt:ipc";
char *EMBED__smt_thread_by_status_component  = "smt_thread_by_status ";
char *EMBED__smt_thread_by_status_version  = "1.0 ";
char *EMBED__smt_thread_by_status_copyright  = "Copyright (c) 1996-2009 iMatix Corporation";
char *EMBED__smt_thread_by_status_filename  = "smt_thread_by_status.icl ";
char *EMBED__smt_thread_by_status_builddate  = "2011/03/01 ";
char *EMBED__smt_thread_by_status_version_end  = "VeRsIoNeNd:ipc";

