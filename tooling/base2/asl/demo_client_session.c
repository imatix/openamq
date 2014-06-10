/*---------------------------------------------------------------------------
    demo_client_session.c - demo_client_session component

This class provides the session serialisation API.
    Generated from demo_client_session.icl by icl_gen using GSL/4.
    
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
#include "demo_client_session.h"        //  Definitions for our class

//  Shorthand for class type

#define self_t              demo_client_session_t

//  Shorthands for class methods

#define self_new            demo_client_session_new
#define self_annihilate     demo_client_session_annihilate
#define self_wait           demo_client_session_wait
#define self_channel_flow   demo_client_session_channel_flow
#define self_channel_flow_ok  demo_client_session_channel_flow_ok
#define self_exchange_declare  demo_client_session_exchange_declare
#define self_exchange_delete  demo_client_session_exchange_delete
#define self_queue_declare  demo_client_session_queue_declare
#define self_queue_bind     demo_client_session_queue_bind
#define self_queue_purge    demo_client_session_queue_purge
#define self_queue_delete   demo_client_session_queue_delete
#define self_basic_consume  demo_client_session_basic_consume
#define self_basic_cancel   demo_client_session_basic_cancel
#define self_basic_publish  demo_client_session_basic_publish
#define self_basic_get      demo_client_session_basic_get
#define self_push_arrived   demo_client_session_push_arrived
#define self_push_returned  demo_client_session_push_returned
#define self_get_basic_arrived_count  demo_client_session_get_basic_arrived_count
#define self_basic_arrived  demo_client_session_basic_arrived
#define self_get_basic_returned_count  demo_client_session_get_basic_returned_count
#define self_basic_returned  demo_client_session_basic_returned
#define self_get_alive      demo_client_session_get_alive
#define self_get_error_text  demo_client_session_get_error_text
#define self_get_channel_id  demo_client_session_get_channel_id
#define self_get_active     demo_client_session_get_active
#define self_get_reply_code  demo_client_session_get_reply_code
#define self_get_reply_text  demo_client_session_get_reply_text
#define self_get_class_id   demo_client_session_get_class_id
#define self_get_method_id  demo_client_session_get_method_id
#define self_get_lease      demo_client_session_get_lease
#define self_get_consumer_tag  demo_client_session_get_consumer_tag
#define self_get_exchange   demo_client_session_get_exchange
#define self_get_routing_key  demo_client_session_get_routing_key
#define self_get_delivery_tag  demo_client_session_get_delivery_tag
#define self_get_message_count  demo_client_session_get_message_count
#define self_get_queue      demo_client_session_get_queue
#define self_get_properties  demo_client_session_get_properties
#define self_dp_new         demo_client_session_dp_new
#define self_dp_lookup      demo_client_session_dp_lookup
#define self_dp_destroy     demo_client_session_dp_destroy
#define self_initialise     demo_client_session_initialise
#define self_selftest       demo_client_session_selftest
#define self_terminate      demo_client_session_terminate
#define self_show           demo_client_session_show
#define self_destroy        demo_client_session_destroy
#define self_alloc          demo_client_session_alloc
#define self_free           demo_client_session_free
#define self_cache_initialise  demo_client_session_cache_initialise
#define self_cache_purge    demo_client_session_cache_purge
#define self_cache_terminate  demo_client_session_cache_terminate
#define self_show_animation  demo_client_session_show_animation
#define self_new_in_scope   demo_client_session_new_in_scope

static void
    demo_client_session_annihilate (
demo_client_session_t * ( * self_p )    //  Reference to object reference
);

static void
    demo_client_session_initialise (
void);

static void
    demo_client_session_terminate (
void);

#define demo_client_session_alloc()     demo_client_session_alloc_ (__FILE__, __LINE__)
static demo_client_session_t *
    demo_client_session_alloc_ (
char * file,                            //  Source file for call
size_t line                             //  Line number for call
);

static void
    demo_client_session_free (
demo_client_session_t * self            //  Object reference
);

static void
    demo_client_session_cache_initialise (
void);

static void
    demo_client_session_cache_purge (
void);

static void
    demo_client_session_cache_terminate (
void);

Bool
    demo_client_session_animating = TRUE;  //  Animation enabled by default
static Bool
    s_demo_client_session_active = FALSE;
#if (defined (BASE_THREADSAFE))
static icl_mutex_t
    *s_demo_client_session_mutex  = NULL;
#endif
static icl_cache_t
    *s_cache = NULL;


static int
    s_wait_for_methods (demo_client_session_t *self, int timeout, Bool blocking, Bool want_content);
static void
    s_process_chrono (demo_client_session_t *self, demo_content_basic_t *content);
static void
    s_close_dp_connection (ipr_hash_t *hash, void *argument);
/*  -------------------------------------------------------------------------
    demo_client_session_new

    Type: Component method
    Creates and initialises a new demo_client_session_t object, returns a
    reference to the created object.
    Create new session on an established connection.

    The connection argument refers to a previously-created connection
    object.
    -------------------------------------------------------------------------
 */

demo_client_session_t *
    demo_client_session_new_ (
    char * file,                        //  Source file for call
    size_t line,                        //  Line number for call
    demo_client_connection_t * connection  //  Not documented
)
{
int
    rc;
    demo_client_session_t *
        self = NULL;                    //  Object reference

if (!s_demo_client_session_active)
    self_initialise ();
    self = demo_client_session_alloc_ (file, line);
    if (self) {
        self->object_tag   = DEMO_CLIENT_SESSION_ALIVE;
#if defined (DEBUG)
        icl_mem_set_callback (self, demo_client_session_show_);
#endif

//
assert (connection);

self->connection   = connection;
self->channel_nbr  = ++connection->channel_nbr;
self->method_queue = smt_method_queue_sync_new ();
self->error_text   = "no error";
self->arrived_basic_list = demo_content_basic_list_new ();
self->returned_basic_list = demo_content_basic_list_new ();
self->chrono_list = demo_content_basic_list_new ();
self->dp_sinks = ipr_hash_table_new ();
self->dp_feeds = ipr_hash_table_new ();

//  Chrono block is the batch size plus enough for one delta
//  Thus we dispatch the chrono block when it's GE the batch size
self->chrono_block = icl_mem_alloc (
    demo_client_config_chrono_batch (demo_client_config)
    + ICL_SHORTSTR_MAX);
strclr (self->chrono_block);

if (connection->alive) {
    rc = demo_client_agent_channel_open (
        self->connection->thread,
        self->method_queue,
        &self->alive,
        &self->reply_code,
        self->reply_text,
        self->channel_nbr);

    //  If no immediate error, wait for confirmation that session was OK
    if (!rc) {
        if (s_wait_for_methods (self, self->connection->timeout, TRUE, FALSE) == 0) {
            self->alive = TRUE;     //  Session is ready for use
            self->timestamp = apr_time_now ();
        }
    }
}
else {
    self->error_text = "connection is closed";
    rc = -1;
}
if (!self->alive)
    self_destroy (&self);
}

    return (self);
}
/*  -------------------------------------------------------------------------
    demo_client_session_annihilate

    Type: Component method
    Close the session.
    -------------------------------------------------------------------------
 */

static void
    demo_client_session_annihilate (
    demo_client_session_t * ( * self_p )  //  Reference to object reference
)
{

    demo_client_session_t *
        self = *self_p;                 //  Dereferenced Reference to object reference

DEMO_CLIENT_SESSION_ASSERT_SANE (self);

//  Close and flush any Direct Mode connections
ipr_hash_table_apply (self->dp_sinks, s_close_dp_connection, self);
ipr_hash_table_destroy (&self->dp_sinks);
ipr_hash_table_apply (self->dp_feeds, s_close_dp_connection, self);
ipr_hash_table_destroy (&self->dp_feeds);

//  Close main connection
if (self->alive) {
    demo_client_agent_channel_close (self->connection->thread, self->channel_nbr);
    while (self->alive) {
        //  Wait until we get back an CLOSED method, or timeout
        s_wait_for_methods (self, self->connection->timeout, FALSE, FALSE);
    }
}
icl_longstr_destroy (&self->channel_id);
icl_longstr_destroy (&self->properties);
icl_mem_free (self->chrono_block);
smt_method_queue_destroy (&self->method_queue);
demo_content_basic_list_destroy (&self->arrived_basic_list);
demo_content_basic_list_destroy (&self->returned_basic_list);
demo_content_basic_list_destroy (&self->chrono_list);

}
/*  -------------------------------------------------------------------------
    demo_client_session_wait

    Type: Component method
    Accepts a demo_client_session_t reference and returns zero in case of success,
    1 in case of errors.
    Waits for any activity on the session.  Returns 0 if content arrived
    or the timeout expired, -1 if the session ended abnormally.  If the timeout
    is zero, waits forever.  If the timeout is -1, does not wait at all, but
    processes pending methods and then returns.
    -------------------------------------------------------------------------
 */

int
    demo_client_session_wait (
    demo_client_session_t * self,       //  Reference to object
    int timeout                         //  Timeout, in milliseconds
)
{
    int
        rc = 0;                         //  Return code

DEMO_CLIENT_SESSION_ASSERT_SANE (self);

//
if (!demo_content_basic_list_isempty (self->arrived_basic_list)
||  !demo_content_basic_list_isempty (self->returned_basic_list))
    return (0);                    //  Return if there is content to process
if (self->alive) {
    if (s_wait_for_methods (self, timeout, FALSE, TRUE))
        rc = -1;
}
else {
    self->error_text = "session is closed";
    rc = -1;
}


    return (rc);
}
/*  -------------------------------------------------------------------------
    demo_client_session_channel_flow

    Type: Component method
    Accepts a demo_client_session_t reference and returns zero in case of success,
    1 in case of errors.
    Enable/Disable Flow From Peer.
    Returns 0 on success, 1 on failure.
    -------------------------------------------------------------------------
 */

int
    demo_client_session_channel_flow (
    demo_client_session_t * self,       //  Reference to object
    Bool active                         //  start/stop content frames
)
{
    int
        rc = 0;                         //  Return code

DEMO_CLIENT_SESSION_ASSERT_SANE (self);

if (self->alive) {
    rc = demo_client_agent_channel_flow (
        self->connection->thread,    //  connection thread
        self->channel_nbr,           //  channel to send on
        active);                     //  start/stop content frames

    if (rc == 0) {
        if (self->alive) {
            //  Wait until the agent has synchronised with us
            if (s_wait_for_methods (self, self->connection->timeout, TRUE, FALSE))
                rc = -1;
        }
        else {
            self->error_text = "session is closed";
            rc = -1;
        }
    }
}
else {
    self->error_text = "session is closed";
    rc = -1;
}


    return (rc);
}
/*  -------------------------------------------------------------------------
    demo_client_session_channel_flow_ok

    Type: Component method
    Accepts a demo_client_session_t reference and returns zero in case of success,
    1 in case of errors.
    Confirm A Flow Method.
    Returns 0 on success, 1 on failure.
    -------------------------------------------------------------------------
 */

int
    demo_client_session_channel_flow_ok (
    demo_client_session_t * self,       //  Reference to object
    Bool active                         //  current flow setting
)
{
    int
        rc = 0;                         //  Return code

DEMO_CLIENT_SESSION_ASSERT_SANE (self);

if (self->alive) {
    rc = demo_client_agent_channel_flow_ok (
        self->connection->thread,    //  connection thread
        self->channel_nbr,           //  channel to send on
        active);                     //  current flow setting

    if (rc == 0) {
        if (s_wait_for_methods (self, self->connection->timeout, FALSE, FALSE))
            rc = -1;
    }
}
else {
    self->error_text = "session is closed";
    rc = -1;
}


    return (rc);
}
/*  -------------------------------------------------------------------------
    demo_client_session_exchange_declare

    Type: Component method
    Accepts a demo_client_session_t reference and returns zero in case of success,
    1 in case of errors.
    Declare Exchange, Create If Needed.
    Returns 0 on success, 1 on failure.
    -------------------------------------------------------------------------
 */

int
    demo_client_session_exchange_declare (
    demo_client_session_t * self,       //  Reference to object
    char * exchange,                    //  exchange name
    char * type,                        //  exchange class
    Bool passive,                       //  do not create exchange
    Bool durable,                       //  request a durable exchange
    Bool auto_delete                    //  auto-delete when unused
)
{
    int
        rc = 0;                         //  Return code

DEMO_CLIENT_SESSION_ASSERT_SANE (self);

if (self->alive) {
    rc = demo_client_agent_exchange_declare (
        self->connection->thread,    //  connection thread
        self->channel_nbr,           //  channel to send on
        exchange,                    //  exchange name
        type,                        //  exchange class
        passive,                     //  do not create exchange
        durable,                     //  request a durable exchange
        auto_delete);                //  auto-delete when unused

    if (rc == 0) {
        if (self->alive) {
            //  Wait until the agent has synchronised with us
            if (s_wait_for_methods (self, self->connection->timeout, TRUE, FALSE))
                rc = -1;
        }
        else {
            self->error_text = "session is closed";
            rc = -1;
        }
    }
}
else {
    self->error_text = "session is closed";
    rc = -1;
}


    return (rc);
}
/*  -------------------------------------------------------------------------
    demo_client_session_exchange_delete

    Type: Component method
    Accepts a demo_client_session_t reference and returns zero in case of success,
    1 in case of errors.
    Delete An Exchange.
    Returns 0 on success, 1 on failure.
    -------------------------------------------------------------------------
 */

int
    demo_client_session_exchange_delete (
    demo_client_session_t * self,       //  Reference to object
    char * exchange,                    //  exchange name
    Bool if_unused                      //  delete only if unused
)
{
    int
        rc = 0;                         //  Return code

DEMO_CLIENT_SESSION_ASSERT_SANE (self);

if (self->alive) {
    rc = demo_client_agent_exchange_delete (
        self->connection->thread,    //  connection thread
        self->channel_nbr,           //  channel to send on
        exchange,                    //  exchange name
        if_unused);                  //  delete only if unused

    if (rc == 0) {
        if (self->alive) {
            //  Wait until the agent has synchronised with us
            if (s_wait_for_methods (self, self->connection->timeout, TRUE, FALSE))
                rc = -1;
        }
        else {
            self->error_text = "session is closed";
            rc = -1;
        }
    }
}
else {
    self->error_text = "session is closed";
    rc = -1;
}


    return (rc);
}
/*  -------------------------------------------------------------------------
    demo_client_session_queue_declare

    Type: Component method
    Accepts a demo_client_session_t reference and returns zero in case of success,
    1 in case of errors.
    Declare Queue, Create If Needed.
    Returns 0 on success, 1 on failure.
    -------------------------------------------------------------------------
 */

int
    demo_client_session_queue_declare (
    demo_client_session_t * self,       //  Reference to object
    char * queue,                       //  queue name
    Bool passive,                       //  do not create queue
    Bool durable,                       //  request a durable queue
    Bool exclusive,                     //  request an exclusive queue
    Bool auto_delete                    //  auto-delete queue when unused
)
{
smt_thread_t
*thread;
    int
        rc = 0;                         //  Return code

DEMO_CLIENT_SESSION_ASSERT_SANE (self);

if (self->alive) {
    rc = demo_client_agent_queue_declare (
        self->connection->thread,    //  connection thread
        self->channel_nbr,           //  channel to send on
        queue,                       //  queue name
        passive,                     //  do not create queue
        durable,                     //  request a durable queue
        exclusive,                   //  request an exclusive queue
        auto_delete);                //  auto-delete queue when unused

    if (rc == 0) {
        if (self->alive) {
            //  Wait until the agent has synchronised with us
            if (s_wait_for_methods (self, self->connection->timeout, TRUE, FALSE))
                rc = -1;
        }
        else {
            self->error_text = "session is closed";
            rc = -1;
        }
    }
}
else {
    self->error_text = "session is closed";
    rc = -1;
}
//  Action handler after calling method
if (rc == 0) {
if (self->connection->direct && exclusive) {
    thread = demo_client_session_dp_lookup (self, queue, DP_FEED);
    if (!thread) {
        thread = demo_client_session_dp_new (self, queue, DP_FEED);
        assert (thread);
    }
    smt_thread_unlink (&thread);
}
}


    return (rc);
}
/*  -------------------------------------------------------------------------
    demo_client_session_queue_bind

    Type: Component method
    Accepts a demo_client_session_t reference and returns zero in case of success,
    1 in case of errors.
    Bind Queue To An Exchange.
    Returns 0 on success, 1 on failure.
    -------------------------------------------------------------------------
 */

int
    demo_client_session_queue_bind (
    demo_client_session_t * self,       //  Reference to object
    char * queue,                       //  queue name
    char * exchange,                    //  exchange name
    icl_longstr_t * arguments           //  arguments for binding
)
{
    int
        rc = 0;                         //  Return code

DEMO_CLIENT_SESSION_ASSERT_SANE (self);

if (self->alive) {
    rc = demo_client_agent_queue_bind (
        self->connection->thread,    //  connection thread
        self->channel_nbr,           //  channel to send on
        queue,                       //  queue name
        exchange,                    //  exchange name
        arguments);                  //  arguments for binding

    if (rc == 0) {
        if (self->alive) {
            //  Wait until the agent has synchronised with us
            if (s_wait_for_methods (self, self->connection->timeout, TRUE, FALSE))
                rc = -1;
        }
        else {
            self->error_text = "session is closed";
            rc = -1;
        }
    }
}
else {
    self->error_text = "session is closed";
    rc = -1;
}


    return (rc);
}
/*  -------------------------------------------------------------------------
    demo_client_session_queue_purge

    Type: Component method
    Accepts a demo_client_session_t reference and returns zero in case of success,
    1 in case of errors.
    Purge A Queue.
    Returns 0 on success, 1 on failure.
    -------------------------------------------------------------------------
 */

int
    demo_client_session_queue_purge (
    demo_client_session_t * self,       //  Reference to object
    char * queue                        //  queue name
)
{
    int
        rc = 0;                         //  Return code

DEMO_CLIENT_SESSION_ASSERT_SANE (self);

if (self->alive) {
    rc = demo_client_agent_queue_purge (
        self->connection->thread,    //  connection thread
        self->channel_nbr,           //  channel to send on
        queue);                      //  queue name

    if (rc == 0) {
        if (self->alive) {
            //  Wait until the agent has synchronised with us
            if (s_wait_for_methods (self, self->connection->timeout, TRUE, FALSE))
                rc = -1;
        }
        else {
            self->error_text = "session is closed";
            rc = -1;
        }
    }
}
else {
    self->error_text = "session is closed";
    rc = -1;
}


    return (rc);
}
/*  -------------------------------------------------------------------------
    demo_client_session_queue_delete

    Type: Component method
    Accepts a demo_client_session_t reference and returns zero in case of success,
    1 in case of errors.
    Delete A Queue.
    Returns 0 on success, 1 on failure.
    -------------------------------------------------------------------------
 */

int
    demo_client_session_queue_delete (
    demo_client_session_t * self,       //  Reference to object
    char * queue,                       //  queue name
    Bool if_unused,                     //  delete only if unused
    Bool if_empty                       //  delete only if empty
)
{
    int
        rc = 0;                         //  Return code

DEMO_CLIENT_SESSION_ASSERT_SANE (self);

if (self->alive) {
    rc = demo_client_agent_queue_delete (
        self->connection->thread,    //  connection thread
        self->channel_nbr,           //  channel to send on
        queue,                       //  queue name
        if_unused,                   //  delete only if unused
        if_empty);                   //  delete only if empty

    if (rc == 0) {
        if (self->alive) {
            //  Wait until the agent has synchronised with us
            if (s_wait_for_methods (self, self->connection->timeout, TRUE, FALSE))
                rc = -1;
        }
        else {
            self->error_text = "session is closed";
            rc = -1;
        }
    }
}
else {
    self->error_text = "session is closed";
    rc = -1;
}
//  Action handler after calling method
if (rc == 0) {
if (self->connection->direct)
    demo_client_session_dp_destroy (self, queue, DP_FEED);
}


    return (rc);
}
/*  -------------------------------------------------------------------------
    demo_client_session_basic_consume

    Type: Component method
    Accepts a demo_client_session_t reference and returns zero in case of success,
    1 in case of errors.
    Start A Queue Consumer.
    Returns 0 on success, 1 on failure.
    -------------------------------------------------------------------------
 */

int
    demo_client_session_basic_consume (
    demo_client_session_t * self,       //  Reference to object
    char * queue                        //  queue name
)
{
    int
        rc = 0;                         //  Return code

DEMO_CLIENT_SESSION_ASSERT_SANE (self);

if (self->alive) {
    rc = demo_client_agent_basic_consume (
        self->connection->thread,    //  connection thread
        self->channel_nbr,           //  channel to send on
        queue);                      //  queue name

    if (rc == 0) {
        if (self->alive) {
            //  Wait until the agent has synchronised with us
            if (s_wait_for_methods (self, self->connection->timeout, TRUE, FALSE))
                rc = -1;
        }
        else {
            self->error_text = "session is closed";
            rc = -1;
        }
    }
}
else {
    self->error_text = "session is closed";
    rc = -1;
}


    return (rc);
}
/*  -------------------------------------------------------------------------
    demo_client_session_basic_cancel

    Type: Component method
    Accepts a demo_client_session_t reference and returns zero in case of success,
    1 in case of errors.
    End A Queue Consumer.
    Returns 0 on success, 1 on failure.
    -------------------------------------------------------------------------
 */

int
    demo_client_session_basic_cancel (
    demo_client_session_t * self,       //  Reference to object
    char * consumer_tag                 //  consumer tag
)
{
    int
        rc = 0;                         //  Return code

DEMO_CLIENT_SESSION_ASSERT_SANE (self);

if (self->alive) {
    rc = demo_client_agent_basic_cancel (
        self->connection->thread,    //  connection thread
        self->channel_nbr,           //  channel to send on
        consumer_tag);               //  consumer tag

    if (rc == 0) {
        if (self->alive) {
            //  Wait until the agent has synchronised with us
            if (s_wait_for_methods (self, self->connection->timeout, TRUE, FALSE))
                rc = -1;
        }
        else {
            self->error_text = "session is closed";
            rc = -1;
        }
    }
}
else {
    self->error_text = "session is closed";
    rc = -1;
}


    return (rc);
}
/*  -------------------------------------------------------------------------
    demo_client_session_basic_publish

    Type: Component method
    Accepts a demo_client_session_t reference and returns zero in case of success,
    1 in case of errors.
    Publish A Message.
    The caller must create a new demo_content_basic object
    for this call; the object is destroyed after being sent.
    Returns 0 on success, 1 on failure.
    -------------------------------------------------------------------------
 */

int
    demo_client_session_basic_publish (
    demo_client_session_t * self,       //  Reference to object
    demo_content_basic_t * content,     //  Message content
    char * exchange,                    //  exchange name
    char * routing_key                  //  Message routing key
)
{
smt_thread_t
*thread;
    int
        rc = 0;                         //  Return code

DEMO_CLIENT_SESSION_ASSERT_SANE (self);

assert (content);                   //  Content must be supplied
//  Action handler before calling method
if (self->connection->direct) {
    thread = demo_client_session_dp_lookup (self, exchange, DP_SINK);
    if (!thread) {
        thread = demo_client_session_dp_new (self, exchange, DP_SINK);
        assert (thread);
    }
    demo_content_basic_set_routing_key (content, exchange, routing_key, NULL);
    demo_client_agent_direct_out (thread, content, 0);
    smt_thread_unlink (&thread);
}
else
if (self->alive) {
    rc = demo_client_agent_basic_publish (
        self->connection->thread,    //  connection thread
        self->channel_nbr,           //  channel to send on
        content,                     //  message content
        exchange,                    //  exchange name
        routing_key);                //  message routing key

    if (rc == 0) {
        if (s_wait_for_methods (self, self->connection->timeout, FALSE, FALSE))
            rc = -1;
    }
    else
        self->error_text = "unable to send method to protocol agent";
}
else {
    self->error_text = "session is closed";
    rc = -1;
}


    return (rc);
}
/*  -------------------------------------------------------------------------
    demo_client_session_basic_get

    Type: Component method
    Accepts a demo_client_session_t reference and returns zero in case of success,
    1 in case of errors.
    Direct Access To A Queue.
    Returns 0 on success, 1 on failure.
    -------------------------------------------------------------------------
 */

int
    demo_client_session_basic_get (
    demo_client_session_t * self,       //  Reference to object
    char * queue                        //  queue name
)
{
    int
        rc = 0;                         //  Return code

DEMO_CLIENT_SESSION_ASSERT_SANE (self);

if (self->alive) {
    rc = demo_client_agent_basic_get (
        self->connection->thread,    //  connection thread
        self->channel_nbr,           //  channel to send on
        queue);                      //  queue name

    if (rc == 0) {
        if (self->alive) {
            //  Wait until the agent has synchronised with us
            if (s_wait_for_methods (self, self->connection->timeout, TRUE, FALSE))
                rc = -1;
        }
        else {
            self->error_text = "session is closed";
            rc = -1;
        }
    }
}
else {
    self->error_text = "session is closed";
    rc = -1;
}


    return (rc);
}
/*  -------------------------------------------------------------------------
    demo_client_session_push_arrived

    Type: Component method
    Accepts a demo_client_session_t reference and returns zero in case of success,
    1 in case of errors.
    Stores an arrived content, and takes appropriate action if the arrived
    queue hits the high water mark.
    -------------------------------------------------------------------------
 */

int
    demo_client_session_push_arrived (
    demo_client_session_t * self,       //  Reference to object
    demo_content_basic_t * content,     //  Message content
    char * exchange,                    //  Value to stamp into content
    char * routing_key,                 //  Value to stamp into content
    char * consumer_tag,                //  Value to stamp into content
    int64_t delivery_tag                //  Value to stamp into content
)
{
size_t
    high_water,                     //  Value of high water setting
    queue_size;                     //  Queue size before we push our content
char
    *on_overflow;                   //  on_overflow action for direct connections
    int
        rc = 0;                         //  Return code

DEMO_CLIENT_SESSION_ASSERT_SANE (self);

//
//  Stamp content with values from method, if any
if (exchange && *exchange)
    icl_shortstr_cpy (content->exchange, exchange);
if (routing_key && routing_key)
    icl_shortstr_cpy (content->routing_key, routing_key);
if (consumer_tag && *consumer_tag)
    icl_shortstr_cpy (content->consumer_tag, consumer_tag);
content->delivery_tag = delivery_tag;

//  Send chronometer deltas, if any, to chrono@amq.status exchange
if (demo_client_config_chrono_enabled (demo_client_config))
    s_process_chrono (self, content);

//  Check if queue would overflow, take appropriate action
high_water = demo_client_config_high_water (demo_client_config);
if (high_water) {                   //  If it's zero, we don't interfere
    queue_size = self_get_basic_arrived_count (self);
    if (queue_size == high_water) {
        if (self->connection->direct) {
            on_overflow = demo_client_config_on_overflow (demo_client_config);
            if (streq (on_overflow, "warn")) {
                //  Warn that we hit high-water, but only once per tide
                if (icl_atomic_cas32 (&self->flow_stopped, TRUE, FALSE) == FALSE) {
                    icl_console_print ("W: high-water mark reached, application is too slow");
                    icl_console_print ("I: contact iMatix for help on tuning your application");
                }
            }
            else
            if (streq (on_overflow, "trim")) {
                //  Chuck away oldest content on arrived list
                demo_content_basic_t
                    *trimmed;
                trimmed = demo_content_basic_list_pop (self->arrived_basic_list);
                demo_content_basic_unlink (&trimmed);
            }
            else
            if (streq (on_overflow, "drop")) {
                //  Chuck away new incoming content
                content = NULL;     //  Just don't store the content
            }
            else
            if (streq (on_overflow, "kill")) {
                //  To die. In the rain. Alone.
                icl_system_panic ("demo_client_session",
                    "E: high water reached (%d): on_overflow requested fatal stop", high_water);
                icl_system_panic ("demo_client_session",
                    "I: contact iMatix for help on tuning your application");
                exit (1);
            }
        }
        else
        //  If we're in normal AMQP mode, send Channel.Flow Off if needed
        if (icl_atomic_cas32 (&self->flow_stopped, TRUE, FALSE) == FALSE)
            demo_client_session_channel_flow (self, FALSE);
    }
}
//  Queue content if we did not discard it
if (content)
    demo_content_basic_list_queue (self->arrived_basic_list, content);


    return (rc);
}
/*  -------------------------------------------------------------------------
    demo_client_session_push_returned

    Type: Component method
    Accepts a demo_client_session_t reference and returns zero in case of success,
    1 in case of errors.
    Stores a returned content.  Currently not guarded by any high water marks.
    -------------------------------------------------------------------------
 */

int
    demo_client_session_push_returned (
    demo_client_session_t * self,       //  Reference to object
    demo_content_basic_t * content,     //  Message content
    char * exchange,                    //  Value to stamp into content
    char * routing_key,                 //  Value to stamp into content
    char * consumer_tag                 //  Value to stamp into content
)
{
    int
        rc = 0;                         //  Return code

DEMO_CLIENT_SESSION_ASSERT_SANE (self);

//
//  Stamp content with values from method, if any
if (exchange && *exchange)
    icl_shortstr_cpy (content->exchange, exchange);
if (routing_key && routing_key)
    icl_shortstr_cpy (content->routing_key, routing_key);
if (consumer_tag && *consumer_tag)
    icl_shortstr_cpy (content->consumer_tag, consumer_tag);
content->delivery_tag = 0;

//  Set the returned flag - is this actually useful?
content->returned = TRUE;

//  Queue content
demo_content_basic_list_queue (self->returned_basic_list, content);
if (!self->connection->silent)
    icl_console_print ("W: basic message was returned: %d - %s",
        self->reply_code, self->reply_text);


    return (rc);
}
/*  -------------------------------------------------------------------------
    demo_client_session_get_basic_arrived_count

    Type: Component method
    Accepts a demo_client_session_t reference and returns zero in case of success,
    1 in case of errors.
    -------------------------------------------------------------------------
 */

int
    demo_client_session_get_basic_arrived_count (
    demo_client_session_t * self        //  Reference to object
)
{
    int
        rc = 0;                         //  Return code

DEMO_CLIENT_SESSION_ASSERT_SANE (self);

rc = demo_content_basic_list_count (self->arrived_basic_list);


    return (rc);
}
/*  -------------------------------------------------------------------------
    demo_client_session_basic_arrived

    Type: Component method
    -------------------------------------------------------------------------
 */

demo_content_basic_t *
    demo_client_session_basic_arrived (
    demo_client_session_t * self        //  Session object
)
{
    demo_content_basic_t *
        content = NULL;                 //  Message content

//
//  Get next message off list, if any
content = demo_content_basic_list_pop (self->arrived_basic_list);

//  Patch session to properly reflect this content's properties
if (content) {
    icl_shortstr_cpy (self->exchange, content->exchange);
    icl_shortstr_cpy (self->routing_key, content->routing_key);
    if (*content->consumer_tag)
        icl_shortstr_cpy (self->consumer_tag, content->consumer_tag);
    self->delivery_tag = content->delivery_tag;
}
if (demo_client_config_high_water (demo_client_config)
&& self_get_basic_arrived_count (self) == demo_client_config_low_water (demo_client_config)
&& icl_atomic_cas32 (&self->flow_stopped, FALSE, TRUE) == TRUE)
    self_channel_flow (self, TRUE);

    return (content);
}
/*  -------------------------------------------------------------------------
    demo_client_session_get_basic_returned_count

    Type: Component method
    Accepts a demo_client_session_t reference and returns zero in case of success,
    1 in case of errors.
    -------------------------------------------------------------------------
 */

int
    demo_client_session_get_basic_returned_count (
    demo_client_session_t * self        //  Reference to object
)
{
    int
        rc = 0;                         //  Return code

DEMO_CLIENT_SESSION_ASSERT_SANE (self);

rc = demo_content_basic_list_count (self->returned_basic_list);


    return (rc);
}
/*  -------------------------------------------------------------------------
    demo_client_session_basic_returned

    Type: Component method
    -------------------------------------------------------------------------
 */

demo_content_basic_t *
    demo_client_session_basic_returned (
    demo_client_session_t * self        //  Session object
)
{
    demo_content_basic_t *
        content = NULL;                 //  Message content

//
//  Get next message off list, if any
content = demo_content_basic_list_pop (self->returned_basic_list);

    return (content);
}
/*  -------------------------------------------------------------------------
    demo_client_session_get_alive

    Type: Component method
    -------------------------------------------------------------------------
 */

Bool
    demo_client_session_get_alive (
    demo_client_session_t * self        //  Object reference
)
{
    Bool
        alive;                          //  Not documented

//
alive = self->alive;

    return (alive);
}
/*  -------------------------------------------------------------------------
    demo_client_session_get_error_text

    Type: Component method
    -------------------------------------------------------------------------
 */

char *
    demo_client_session_get_error_text (
    demo_client_session_t * self        //  Object reference
)
{
    char *
        error_text;                     //  Not documented

//
error_text = self->error_text;

    return (error_text);
}
/*  -------------------------------------------------------------------------
    demo_client_session_get_channel_id

    Type: Component method
    -------------------------------------------------------------------------
 */

icl_longstr_t *
    demo_client_session_get_channel_id (
    demo_client_session_t * self        //  Object reference
)
{
    icl_longstr_t *
        channel_id;                     //  Not documented

//
channel_id = self->channel_id;

    return (channel_id);
}
/*  -------------------------------------------------------------------------
    demo_client_session_get_active

    Type: Component method
    -------------------------------------------------------------------------
 */

Bool
    demo_client_session_get_active (
    demo_client_session_t * self        //  Object reference
)
{
    Bool
        active;                         //  Not documented

//
active = self->active;

    return (active);
}
/*  -------------------------------------------------------------------------
    demo_client_session_get_reply_code

    Type: Component method
    -------------------------------------------------------------------------
 */

int
    demo_client_session_get_reply_code (
    demo_client_session_t * self        //  Object reference
)
{
    int
        reply_code;                     //  Not documented

//
reply_code = self->reply_code;

    return (reply_code);
}
/*  -------------------------------------------------------------------------
    demo_client_session_get_reply_text

    Type: Component method
    -------------------------------------------------------------------------
 */

char *
    demo_client_session_get_reply_text (
    demo_client_session_t * self        //  Object reference
)
{
    char *
        reply_text;                     //  Not documented

//
reply_text = self->reply_text;

    return (reply_text);
}
/*  -------------------------------------------------------------------------
    demo_client_session_get_class_id

    Type: Component method
    -------------------------------------------------------------------------
 */

int
    demo_client_session_get_class_id (
    demo_client_session_t * self        //  Object reference
)
{
    int
        class_id;                       //  Not documented

//
class_id = self->class_id;

    return (class_id);
}
/*  -------------------------------------------------------------------------
    demo_client_session_get_method_id

    Type: Component method
    -------------------------------------------------------------------------
 */

int
    demo_client_session_get_method_id (
    demo_client_session_t * self        //  Object reference
)
{
    int
        method_id;                      //  Not documented

//
method_id = self->method_id;

    return (method_id);
}
/*  -------------------------------------------------------------------------
    demo_client_session_get_lease

    Type: Component method
    -------------------------------------------------------------------------
 */

char *
    demo_client_session_get_lease (
    demo_client_session_t * self        //  Object reference
)
{
    char *
        lease;                          //  Not documented

//
lease = self->lease;

    return (lease);
}
/*  -------------------------------------------------------------------------
    demo_client_session_get_consumer_tag

    Type: Component method
    -------------------------------------------------------------------------
 */

char *
    demo_client_session_get_consumer_tag (
    demo_client_session_t * self        //  Object reference
)
{
    char *
        consumer_tag;                   //  Not documented

//
consumer_tag = self->consumer_tag;

    return (consumer_tag);
}
/*  -------------------------------------------------------------------------
    demo_client_session_get_exchange

    Type: Component method
    -------------------------------------------------------------------------
 */

char *
    demo_client_session_get_exchange (
    demo_client_session_t * self        //  Object reference
)
{
    char *
        exchange;                       //  Not documented

//
exchange = self->exchange;

    return (exchange);
}
/*  -------------------------------------------------------------------------
    demo_client_session_get_routing_key

    Type: Component method
    -------------------------------------------------------------------------
 */

char *
    demo_client_session_get_routing_key (
    demo_client_session_t * self        //  Object reference
)
{
    char *
        routing_key;                    //  Not documented

//
routing_key = self->routing_key;

    return (routing_key);
}
/*  -------------------------------------------------------------------------
    demo_client_session_get_delivery_tag

    Type: Component method
    -------------------------------------------------------------------------
 */

int64_t
    demo_client_session_get_delivery_tag (
    demo_client_session_t * self        //  Object reference
)
{
    int64_t
        delivery_tag;                   //  Not documented

//
delivery_tag = self->delivery_tag;

    return (delivery_tag);
}
/*  -------------------------------------------------------------------------
    demo_client_session_get_message_count

    Type: Component method
    -------------------------------------------------------------------------
 */

qbyte
    demo_client_session_get_message_count (
    demo_client_session_t * self        //  Object reference
)
{
    qbyte
        message_count;                  //  Not documented

//
message_count = self->message_count;

    return (message_count);
}
/*  -------------------------------------------------------------------------
    demo_client_session_get_queue

    Type: Component method
    -------------------------------------------------------------------------
 */

char *
    demo_client_session_get_queue (
    demo_client_session_t * self        //  Object reference
)
{
    char *
        queue;                          //  Not documented

//
queue = self->queue;

    return (queue);
}
/*  -------------------------------------------------------------------------
    demo_client_session_get_properties

    Type: Component method
    -------------------------------------------------------------------------
 */

icl_longstr_t *
    demo_client_session_get_properties (
    demo_client_session_t * self        //  Object reference
)
{
    icl_longstr_t *
        properties;                     //  Not documented

//
properties = self->properties;

    return (properties);
}
/*  -------------------------------------------------------------------------
    demo_client_session_dp_new

    Type: Component method
    -------------------------------------------------------------------------
 */

smt_thread_t *
    demo_client_session_dp_new (
    demo_client_session_t * self,       //  Session object
    char * name,                        //  Resource name
    int type                            //  Resource type
)
{
int
    rc = 0;
ipr_hash_t
    *hash;
    smt_thread_t *
        thread = NULL;                  //  Not documented

//
//  Start a new direct connection
//  - send Direct.Put or Direct.Get via main connection
//  - server provides lease via Put-Ok or Get-Ok
//  - create new direct thread and give it the lease
//
assert (type == DP_SINK || type == DP_FEED);
if (name == NULL)
    name = "";                            //  Blank name is valid

//  Request lease and wait for response
if (type == DP_SINK)
    demo_client_agent_direct_put (self->connection->thread, self->channel_nbr, name);
else
    demo_client_agent_direct_get (self->connection->thread, self->channel_nbr, name);
rc = s_wait_for_methods (self, self->connection->timeout, TRUE, FALSE);
assert (!rc);

//  Create direct connection thread and wait for it to signal ready
thread = demo_client_agent_direct_thread_new (
    type,
    self->method_queue,
    self->connection->host,
    self->lease,
    self->connection->trace,
    self->connection->timeout);
assert (thread);
smt_thread_wake (thread);
rc = s_wait_for_methods (self, self->connection->timeout, TRUE, FALSE);
assert (!rc);

//  Store thread in hash table with extra link, so that caller can
//  unlink thread properly.
if (type == DP_SINK)
    hash = ipr_hash_new (self->dp_sinks, name, smt_thread_link (thread));
else
    hash = ipr_hash_new (self->dp_feeds, name, smt_thread_link (thread));
ipr_hash_unlink (&hash);

    return (thread);
}
/*  -------------------------------------------------------------------------
    demo_client_session_dp_lookup

    Type: Component method
    -------------------------------------------------------------------------
 */

smt_thread_t *
    demo_client_session_dp_lookup (
    demo_client_session_t * self,       //  Session object
    char * name,                        //  Resource name
    int type                            //  Resource type
)
{
ipr_hash_t
    *hash;
    smt_thread_t *
        thread = NULL;                  //  Not documented

//

assert (type == DP_SINK || type == DP_FEED);
if (type == DP_SINK)
    hash = ipr_hash_table_search (self->dp_sinks, name);
else
    hash = ipr_hash_table_search (self->dp_feeds, name);

if (hash) {
    thread = smt_thread_link ((smt_thread_t *) hash->data);
    ipr_hash_unlink (&hash);
}

    return (thread);
}
/*  -------------------------------------------------------------------------
    demo_client_session_dp_destroy

    Type: Component method
    Accepts a demo_client_session_t reference and returns zero in case of success,
    1 in case of errors.
    -------------------------------------------------------------------------
 */

int
    demo_client_session_dp_destroy (
    demo_client_session_t * self,       //  Reference to object
    char * name,                        //  Resource name
    int type                            //  Resource type
)
{
ipr_hash_t
    *hash;
    int
        rc = 0;                         //  Return code

DEMO_CLIENT_SESSION_ASSERT_SANE (self);

//
assert (type == DP_SINK || type == DP_FEED);
if (type == DP_SINK) {
    hash = ipr_hash_table_search (self->dp_sinks, name);
    if (hash) {
        s_close_dp_connection (hash, self);
        ipr_hash_destroy (&hash);
    }
}
else {
    hash = ipr_hash_table_search (self->dp_feeds, name);
    if (hash) {
        s_close_dp_connection (hash, self);
        ipr_hash_destroy (&hash);
    }
}


    return (rc);
}
/*  -------------------------------------------------------------------------
    demo_client_session_initialise

    Type: Component method
    -------------------------------------------------------------------------
 */

static void
    demo_client_session_initialise (
void)
{

    //  Test for already active before applying any locks; avoids deadlock in
    //  some classes
    if (!s_demo_client_session_active) {

#if (defined (BASE_THREADSAFE))
        //  First make sure the object mutex has been created
        if (!icl_global_mutex) {
            icl_system_panic ("icl_init", "iCL not initialised - call icl_system_initialise()\n");
            abort ();
        }
        apr_thread_mutex_lock (icl_global_mutex);
        if (!s_demo_client_session_mutex)
            s_demo_client_session_mutex = icl_mutex_new ();
        apr_thread_mutex_unlock (icl_global_mutex);

        //  Now lock the object mutex
        icl_mutex_lock   (s_demo_client_session_mutex);

        //  Test again for already active now that we hold the lock
        if (!s_demo_client_session_active) {
#endif
            //  Register the class termination call-back functions
            icl_system_register (NULL, self_terminate);

demo_client_agent_init ();
            s_demo_client_session_active = TRUE;
#if (defined (BASE_THREADSAFE))
        }
        icl_mutex_unlock (s_demo_client_session_mutex);
#endif

    }
}
/*  -------------------------------------------------------------------------
    demo_client_session_selftest

    Type: Component method
    -------------------------------------------------------------------------
 */

void
    demo_client_session_selftest (
void)
{

}
/*  -------------------------------------------------------------------------
    demo_client_session_terminate

    Type: Component method
    -------------------------------------------------------------------------
 */

static void
    demo_client_session_terminate (
void)
{

if (s_demo_client_session_active) {

#if (defined (BASE_THREADSAFE))
        icl_mutex_destroy (&s_demo_client_session_mutex);
#endif
        s_demo_client_session_active = FALSE;
    }
}
/*  -------------------------------------------------------------------------
    demo_client_session_show

    Type: Component method
    -------------------------------------------------------------------------
 */

void
    demo_client_session_show_ (
    void * item,                        //  The opaque pointer
    int opcode,                         //  The callback opcode
    FILE * trace_file,                  //  File to print to
    char * file,                        //  Source file
    size_t line                         //  Line number
)
{
demo_client_session_t
    *self;
int
    container_links;


self = item;
container_links = 0;
assert (opcode == ICL_CALLBACK_DUMP);

fprintf (trace_file, "    <demo_client_session file = \"%s\" line = \"%lu\"  pointer = \"%p\" />\n", file, (unsigned long) line, self);

}
/*  -------------------------------------------------------------------------
    demo_client_session_destroy

    Type: Component method
    Destroys a demo_client_session_t object. Takes the address of a
    demo_client_session_t reference (a pointer to a pointer) and nullifies the
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
    demo_client_session_destroy_ (
    demo_client_session_t * ( * self_p ),   //  Reference to object reference
    char * file,                        //  Source fileSource file
    size_t line                         //  Line numberLine number
)
{
    demo_client_session_t *
        self = *self_p;                 //  Dereferenced Reference to object reference

if (self) {
    demo_client_session_annihilate (self_p);
    demo_client_session_free ((demo_client_session_t *) self);
    *self_p = NULL;
}
}
/*  -------------------------------------------------------------------------
    demo_client_session_alloc

    Type: Component method
    -------------------------------------------------------------------------
 */

static demo_client_session_t *
    demo_client_session_alloc_ (
    char * file,                        //  Source file for call
    size_t line                         //  Line number for call
)
{

    demo_client_session_t *
        self = NULL;                    //  Object reference

//  Initialise cache if necessary
if (!s_cache)
    demo_client_session_cache_initialise ();

self = (demo_client_session_t *) icl_mem_cache_alloc_ (s_cache, file, line);
if (self)
    memset (self, 0, sizeof (demo_client_session_t));



    return (self);
}
/*  -------------------------------------------------------------------------
    demo_client_session_free

    Type: Component method
    Freess a demo_client_session_t object.
    -------------------------------------------------------------------------
 */

static void
    demo_client_session_free (
    demo_client_session_t * self        //  Object reference
)
{


if (self) {

        memset (&self->object_tag, 0, sizeof (demo_client_session_t) - ((byte *) &self->object_tag - (byte *) self));
//        memset (self, 0, sizeof (demo_client_session_t));
        self->object_tag = DEMO_CLIENT_SESSION_DEAD;
        icl_mem_free (self);
    }
    self = NULL;
}
/*  -------------------------------------------------------------------------
    demo_client_session_cache_initialise

    Type: Component method
    Initialise the cache and register purge method with the meta-cache.
    -------------------------------------------------------------------------
 */

static void
    demo_client_session_cache_initialise (
void)
{

s_cache = icl_cache_get (sizeof (demo_client_session_t));
icl_system_register (demo_client_session_cache_purge, demo_client_session_cache_terminate);
}
/*  -------------------------------------------------------------------------
    demo_client_session_cache_purge

    Type: Component method
    -------------------------------------------------------------------------
 */

static void
    demo_client_session_cache_purge (
void)
{

icl_mem_cache_purge (s_cache);

}
/*  -------------------------------------------------------------------------
    demo_client_session_cache_terminate

    Type: Component method
    -------------------------------------------------------------------------
 */

static void
    demo_client_session_cache_terminate (
void)
{

s_cache = NULL;

}
/*  -------------------------------------------------------------------------
    demo_client_session_show_animation

    Type: Component method
    Enables animation of the component. Animation is sent to stdout.
    To enable animation you must generate using the option -animate:1.
    -------------------------------------------------------------------------
 */

void
    demo_client_session_show_animation (
    Bool enabled                        //  Are we enabling or disabling animation?
)
{

demo_client_session_animating = enabled;
}
/*  -------------------------------------------------------------------------
    demo_client_session_new_in_scope

    Type: Component method
    -------------------------------------------------------------------------
 */

void
    demo_client_session_new_in_scope_ (
    demo_client_session_t * * self_p,   //  Not documented
    icl_scope_t * _scope,               //  Not documented
    char * file,                        //  Source file for call
    size_t line,                        //  Line number for call
    demo_client_connection_t * connection  //  Not documented
)
{
    icl_destroy_t *
        _destroy;                       //  Not documented

*self_p = demo_client_session_new_ (file,line,connection);

if (*self_p) {
    _destroy = icl_destroy_new   ((void * *) self_p, (icl_destructor_fn *) demo_client_session_destroy_);
    icl_destroy_list_queue (_scope, _destroy);
    icl_destroy_unlink (&_destroy);
}
}
/*  Waits for methods from the protocol agent.  If the blocking argument
    is true, returns only when receiving a synchronous reply method.  If not,
    returns when any incoming method is received.  Returns 0 if ok, -1 if
    the connection is closed or a blocking wait timed-out.
 */
static int
s_wait_for_methods (
    demo_client_session_t *self,
    int timeout,
    Bool blocking,
    Bool want_content)
{
    smt_method_t
        *smt_method = NULL;
    demo_client_method_t
        *proto_method;
    int
        rc = 0;                         //  OK by default
    qbyte
        pending;                        //  Size of method queue
    Bool
        got_content = FALSE;            //  Did we get 1+ contents?

    smt_method_queue_t
        *smt_queue = smt_method_queue_link (self->method_queue);
    assert (!smt_queue->zombie);
    FOREVER {
        smt_method_unlink (&smt_method);
        if (timeout >= 0)
            smt_method_queue_wait (smt_queue, timeout);
        smt_method = smt_method_queue_pop (smt_queue);
        pending = smt_queue->count;
        //  If we don't have a method, we timed-out
        if (!smt_method) {
            if (blocking) {
                if (smt_queue->zombie)
                    self->error_text = "No reply - method queue destroyed";
                else
                    self->error_text = "No reply received - timeout";
                rc = -1;
            }
            break;
        }
        //  ALMOST means we should wait for the next method...
        if (smt_method->result == DEMO_CLIENT_REPLY_ALMOST) {
            proto_method = (demo_client_method_t *) smt_method->data;
            if (proto_method)
                demo_client_method_execute (proto_method, self->connection, self);
        }
        else
        //  INTERN is an internal synchronisation from the agent
        if (smt_method->result == DEMO_CLIENT_REPLY_INTERN)
            break;                      //  We can return to caller
        else
        //  METHOD means the agent is sending us either:
        //    - a sync method, which is a reply to a sync request
        //    - an async method, which we want to process and then continue
        //
        //  We always stop when we get a sync method. If we have an async
        //  method, we stop only if the queue is empty and we're not blocking.
        //  Blocking means we want a sync method within the timeout.
        //
        if (smt_method->result == DEMO_CLIENT_REPLY_METHOD) {
            proto_method = (demo_client_method_t *) smt_method->data;
            if (proto_method) {
                demo_client_method_execute (proto_method, self->connection, self);
                if (proto_method->content)
                    got_content = TRUE;
                if (proto_method->sync
                || (got_content && want_content && pending == 0))
                    break;              //  Got synchronous method, end
            }
            else
                break;                  //  Synchronisation from agent
        }
        else
        if (smt_method->result == DEMO_CLIENT_REPLY_CLOSED) {
            self->error_text = "session is closed";
            self->alive = FALSE;        //  Session is closed
            rc = -1;                    //  Signal session is closed
            break;                      //  We can return to caller
        }
        else
        //  DIRECT_OK means the direct thread started ok
        if (smt_method->result == DEMO_CLIENT_REPLY_DIRECT_OK)
            break;                      //  We can return to caller
        else
        //  DIRECT_NG means the direct thread did not start
        if (smt_method->result == DEMO_CLIENT_REPLY_DIRECT_NG) {
            self->error_text = "direct connection failed";
            rc = -1;                    //  Signal session is closed
            break;                      //  We can return to caller
        }
        else
        //  DIRECT_MSG means we have a message, process & continue
        if (smt_method->result == DEMO_CLIENT_REPLY_DIRECT_MSG) {
            self_push_arrived (
                self, (demo_content_basic_t *) smt_method->data, NULL, NULL, NULL, 0);
            if (want_content && pending == 0)
                break;                  //  We were waiting for content
        }
        else {
            self->error_text = "Internal error - invalid reply method from protocol agent";
            icl_console_print ("E: invalid reply result %d", smt_method->result);
        }
    }
    smt_method_queue_unlink (&smt_queue);
    smt_method_unlink (&smt_method);
    return (rc);
}

static void
s_process_chrono (
    demo_client_session_t *self,
    demo_content_basic_t *content)
{
}

static void
s_close_dp_connection (ipr_hash_t *hash, void *argument)
{
    demo_client_session_t
        *self = argument;
    smt_thread_t
        *thread = (smt_thread_t *) hash->data;

    //  Ensure connection is really closed before returning to caller
    //  This call will also flush any data pending to be written
    demo_client_agent_direct_close (thread);
    s_wait_for_methods (self, self->connection->timeout, FALSE, FALSE);
    smt_thread_unlink (&thread);
}

//  Embed the version information in the resulting binary

char *EMBED__demo_client_session_version_start  = "VeRsIoNsTaRt:ipc";
char *EMBED__demo_client_session_component  = "demo_client_session ";
char *EMBED__demo_client_session_version  = "1.0 ";
char *EMBED__demo_client_session_copyright  = "Copyright (c) 1996-2009 iMatix Corporation";
char *EMBED__demo_client_session_filename  = "demo_client_session.icl ";
char *EMBED__demo_client_session_builddate  = "2011/03/01 ";
char *EMBED__demo_client_session_version_end  = "VeRsIoNeNd:ipc";

