/*---------------------------------------------------------------------------
    demo_server_connection.c - demo_server_connection component

    This class implements the ASL connection class for the demo server.
    Generated from demo_server_connection.icl by icl_gen using GSL/4.
    
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
#include "demo_server_connection.h"     //  Definitions for our class

//  Shorthand for class type

#define self_t              demo_server_connection_t

//  Shorthands for class methods

#define self_start_ok       demo_server_connection_start_ok
#define self_initialise     demo_server_connection_initialise
#define self_new            demo_server_connection_new
#define self_annihilate     demo_server_connection_annihilate
#define self_free           demo_server_connection_free
#define self_set_address    demo_server_connection_set_address
#define self_set_trace      demo_server_connection_set_trace
#define self_kill           demo_server_connection_kill
#define self_ready          demo_server_connection_ready
#define self_raise_exception  demo_server_connection_raise_exception
#define self_error          demo_server_connection_error
#define self_exception_raised  demo_server_connection_exception_raised
#define self_tune_ok        demo_server_connection_tune_ok
#define self_open           demo_server_connection_open
#define self_selftest       demo_server_connection_selftest
#define self_count          demo_server_connection_count
#define self_remove_from_all_containers  demo_server_connection_remove_from_all_containers
#define self_show           demo_server_connection_show
#define self_terminate      demo_server_connection_terminate
#define self_destroy        demo_server_connection_destroy
#define self_alloc          demo_server_connection_alloc
#define self_read_lock      demo_server_connection_read_lock
#define self_write_lock     demo_server_connection_write_lock
#define self_unlock         demo_server_connection_unlock
#define self_link           demo_server_connection_link
#define self_unlink         demo_server_connection_unlink
#define self_cache_initialise  demo_server_connection_cache_initialise
#define self_cache_purge    demo_server_connection_cache_purge
#define self_cache_terminate  demo_server_connection_cache_terminate
#define self_show_animation  demo_server_connection_show_animation
#define self_new_in_scope   demo_server_connection_new_in_scope

static void
    demo_server_connection_initialise (
void);

#define demo_server_connection_annihilate(self)  demo_server_connection_annihilate_ (self, __FILE__, __LINE__)
static void
    demo_server_connection_annihilate_ (
demo_server_connection_t * ( * self_p ),   //  Reference to object reference
char * file,                            //  Source file
size_t line                             //  Line number
);

#define demo_server_connection_free(self)  demo_server_connection_free_ (self, __FILE__, __LINE__)
static void
    demo_server_connection_free_ (
demo_server_connection_t * self,        //  Object reference
char * file,                            //  Source file
size_t line                             //  Line number
);

static void
    demo_server_connection_terminate (
void);

#define demo_server_connection_alloc()  demo_server_connection_alloc_ (__FILE__, __LINE__)
static demo_server_connection_t *
    demo_server_connection_alloc_ (
char * file,                            //  Source file for call
size_t line                             //  Line number for call
);

static void
    demo_server_connection_cache_initialise (
void);

static void
    demo_server_connection_cache_purge (
void);

static void
    demo_server_connection_cache_terminate (
void);

Bool
    demo_server_connection_animating = TRUE;  //  Animation enabled by default
static Bool
    s_demo_server_connection_active = FALSE;
#if (defined (BASE_THREADSAFE))
static icl_mutex_t
    *s_demo_server_connection_mutex  = NULL;
#endif
static icl_cache_t
    *s_cache = NULL;


static qbyte
    s_demo_server_connection_count = 0;
static int
    s_auth_plain (demo_server_connection_t *self, demo_server_connection_start_ok_t *method);
static uint
    s_collect_plain_token (byte *data, char *token, uint token_end);
static volatile qbyte
    s_context_seq_lo,                   //  Next context sequence id
    s_context_seq_hi;                   //  High part (64-bits)
//  Sets and returns connection group, zero indicating an error

static int s_auth_plain (
    demo_server_connection_t *self,
    demo_server_connection_start_ok_t *method)
{
    icl_shortstr_t
        login,
        password;
    uint
        token_null;

    //  method->response holds PLAIN data in format "[NULL]login[NULL]password"
    token_null = s_collect_plain_token (
        method->response->data, password, method->response->cur_size);
    if (token_null) 
        s_collect_plain_token (method->response->data, login, token_null);

    if (strnull (login) || strnull (password)) {
        self_raise_exception (self, ASL_SYNTAX_ERROR, 
            "Missing authentication data",
            DEMO_SERVER_CONNECTION, DEMO_SERVER_CONNECTION_START_OK);
        return (0);
    }
    if (streq (login, "guest") && streq (password, "guest"))
        return (TRUE);
    else
        return (FALSE);
}

//  Collects a token, returns offset of null octet at start
static uint
s_collect_plain_token (byte *data, char *token, uint token_end)
{
    uint
        token_null,
        token_size;

    //  Find start of token, scanning back from known end
    token_null = --token_end;
    while (token_null && data [token_null])
        token_null--;

    strclr (token);
    //  Token start must point to a null octet before the string
    token_size = token_end - token_null;
    if (token_size > ICL_SHORTSTR_MAX)
        smt_log_print (demo_broker->alert_log,
            "W: client used over-long authentication value - rejected");
    else {
        memcpy (token, data + token_null + 1, token_size);
        token [token_size] = 0;
    }
    return (token_null);
}
/*  -------------------------------------------------------------------------
    demo_server_connection_start_ok

    Type: Component method
    Accepts a demo_server_connection_t reference and returns zero in case of success,
    1 in case of errors.
    Implements the ASL Connection.Start-Ok protocol method.
    -------------------------------------------------------------------------
 */

int
    demo_server_connection_start_ok (
    demo_server_connection_t * self,    //  Reference to object
    demo_server_connection_start_ok_t * method  //  Not documented
)
{
#if (defined (BASE_THREADSAFE))
    icl_rwlock_t
        *rwlock;
#endif

asl_field_list_t
    *fields;                        //  Decoded responses
    int
        rc = 0;                         //  Return code

DEMO_SERVER_CONNECTION_ASSERT_SANE (self);
if (!self->zombie) {

#if (defined (BASE_THREADSAFE))
    rwlock = self ? self->rwlock : NULL;
    if (rwlock)
        icl_rwlock_write_lock (rwlock);
#endif

//
self->authorised = FALSE;
fields = asl_field_list_new (method->client_properties);
if (fields) {
    asl_field_list_cpy (fields, self->client_product,     "product");
    asl_field_list_cpy (fields, self->client_version,     "version");
    asl_field_list_cpy (fields, self->client_platform,    "platform");
    asl_field_list_cpy (fields, self->client_copyright,   "copyright");
    asl_field_list_cpy (fields, self->client_information, "information");
    //  Optionally, we could handle the following cases:
    //  1. duplicate connection instance name
    //  2. empty connection instance name
    asl_field_list_cpy (fields, self->client_instance,    "instance");
    asl_field_list_cpy (fields, self->client_proxy_name,  "proxy name");
    asl_field_list_cpy (fields, self->client_proxy_host,  "proxy host");
    asl_field_list_destroy (&fields);
}

if (s_auth_plain (self, method))
    self->authorised = TRUE;
else
    self_raise_exception (self, ASL_ACCESS_REFUSED, 
        "Invalid authentication data",
        DEMO_SERVER_CONNECTION, DEMO_SERVER_CONNECTION_START_OK);
#if (defined (BASE_THREADSAFE))
    if (rwlock)
        icl_rwlock_unlock (rwlock);
#endif

}
else
    rc = -1;                        //  Return error on zombie object.


    return (rc);
}
/*  -------------------------------------------------------------------------
    demo_server_connection_initialise

    Type: Component method
    -------------------------------------------------------------------------
 */

static void
    demo_server_connection_initialise (
void)
{

    //  Test for already active before applying any locks; avoids deadlock in
    //  some classes
    if (!s_demo_server_connection_active) {

#if (defined (BASE_THREADSAFE))
        //  First make sure the object mutex has been created
        if (!icl_global_mutex) {
            icl_system_panic ("icl_init", "iCL not initialised - call icl_system_initialise()\n");
            abort ();
        }
        apr_thread_mutex_lock (icl_global_mutex);
        if (!s_demo_server_connection_mutex)
            s_demo_server_connection_mutex = icl_mutex_new ();
        apr_thread_mutex_unlock (icl_global_mutex);

        //  Now lock the object mutex
        icl_mutex_lock   (s_demo_server_connection_mutex);

        //  Test again for already active now that we hold the lock
        if (!s_demo_server_connection_active) {
#endif
            //  Register the class termination call-back functions
            icl_system_register (NULL, self_terminate);

s_context_seq_hi = 0;
s_context_seq_lo = 1;
            s_demo_server_connection_active = TRUE;
#if (defined (BASE_THREADSAFE))
        }
        icl_mutex_unlock (s_demo_server_connection_mutex);
#endif

    }
}
/*  -------------------------------------------------------------------------
    demo_server_connection_new

    Type: Component method
    Creates and initialises a new demo_server_connection_t object, returns a
    reference to the created object.
    Initialises a new hash table item and plases it into the specified hash
    table, if not null.
    Initialises the new item to be a valid list item.
    -------------------------------------------------------------------------
 */

demo_server_connection_t *
    demo_server_connection_new_ (
    char * file,                        //  Source file for call
    size_t line,                        //  Line number for call
    demo_server_connection_table_t * table,   //  Table to insert into
    smt_thread_t * thread               //  Parent thread
)
{
#define key self->id
qbyte
    seq_lo,
    seq_hi;
    demo_server_connection_t *
        self = NULL;                    //  Object reference

if (!s_demo_server_connection_active)
    self_initialise ();
    self = demo_server_connection_alloc_ (file, line);
    if (self) {
        self->object_tag   = DEMO_SERVER_CONNECTION_ALIVE;
        self->links        = 1;
        self->zombie       = FALSE;
#if defined (DEBUG) || defined (BASE_HISTORY) || defined (BASE_HISTORY_DEMO_SERVER_CONNECTION)
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
        icl_mem_set_callback (self, demo_server_connection_show_);
#endif

self->list_next = self;
self->list_prev = self;
self->list_head = NULL;
self->table_head = NULL;
self->table_index = 0;              //  Will be set by container
icl_atomic_inc32 ((volatile qbyte *) &s_demo_server_connection_count);

//
if (thread)
    self->thread  = smt_thread_link (thread);
self->started     = smt_time_now ();
self->channel_max = 1;              //  We only support one channel 
self->frame_max   = demo_server_config_frame_max (demo_server_config);
self->heartbeat   = demo_server_config_heartbeat (demo_server_config);
self->trace       = demo_server_config_trace (demo_server_config);
self->channels    = demo_server_channel_table_new ();

//  Get 64-bit connection id, no wraparound handling
seq_lo = icl_atomic_inc32 (&s_context_seq_lo);
if (seq_lo == 0)
    seq_hi = icl_atomic_inc32 (&s_context_seq_hi);
else
    seq_hi = s_context_seq_hi;
icl_shortstr_fmt (self->id, "%x-%x", seq_hi, seq_lo);
if (table && self && demo_server_connection_table_insert (table, key, self))
    demo_server_connection_destroy (&self);
}

    return (self);
}
#undef key
/*  -------------------------------------------------------------------------
    demo_server_connection_annihilate

    Type: Component method
    -------------------------------------------------------------------------
 */

static void
    demo_server_connection_annihilate_ (
    demo_server_connection_t * ( * self_p ),   //  Reference to object reference
    char * file,                        //  Source file
    size_t line                         //  Line number
)
{
#if (defined (BASE_THREADSAFE))
    icl_rwlock_t
        *rwlock;
#endif
#if defined (DEBUG) || defined (BASE_HISTORY) || defined (BASE_HISTORY_DEMO_SERVER_CONNECTION)
    int
        history_last;
#endif

    demo_server_connection_t *
        self = *self_p;                 //  Dereferenced Reference to object reference

#if defined (DEBUG) || defined (BASE_HISTORY) || defined (BASE_HISTORY_DEMO_SERVER_CONNECTION)
    //  Track possession operation in history
    history_last = icl_atomic_inc32 ((volatile qbyte *) &self->history_last) + 1;
    self->history_file  [history_last % DEMO_SERVER_CONNECTION_HISTORY_LENGTH] = file;
    self->history_line  [history_last % DEMO_SERVER_CONNECTION_HISTORY_LENGTH] = line;
    self->history_type  [history_last % DEMO_SERVER_CONNECTION_HISTORY_LENGTH] = "destroy";
    self->history_links [history_last % DEMO_SERVER_CONNECTION_HISTORY_LENGTH] = self->links;
#endif

    DEMO_SERVER_CONNECTION_ASSERT_SANE (self);
    demo_server_connection_remove_from_all_containers (self);
#if (defined (BASE_THREADSAFE))
    rwlock = self->rwlock;
    if (rwlock)
         icl_rwlock_write_lock (rwlock);
#endif

icl_atomic_dec32 ((volatile qbyte *) &s_demo_server_connection_count);

//  This closes all channels when we call destroy on the connection
demo_server_channel_table_destroy (&self->channels);
#if (defined (BASE_THREADSAFE))
    if (rwlock)
        icl_rwlock_unlock (rwlock);
#endif

}
/*  -------------------------------------------------------------------------
    demo_server_connection_free

    Type: Component method
    Freess a demo_server_connection_t object.
    -------------------------------------------------------------------------
 */

static void
    demo_server_connection_free_ (
    demo_server_connection_t * self,    //  Object reference
    char * file,                        //  Source file
    size_t line                         //  Line number
)
{
#if defined (DEBUG) || defined (BASE_HISTORY) || defined (BASE_HISTORY_DEMO_SERVER_CONNECTION)
    int
        history_last;
#endif


    if (self) {
#if defined (DEBUG) || defined (BASE_HISTORY) || defined (BASE_HISTORY_DEMO_SERVER_CONNECTION)
        //  Track possession operation in history
        history_last = icl_atomic_inc32 ((volatile qbyte *) &self->history_last) + 1;
        self->history_file  [history_last % DEMO_SERVER_CONNECTION_HISTORY_LENGTH] = file;
        self->history_line  [history_last % DEMO_SERVER_CONNECTION_HISTORY_LENGTH] = line;
        self->history_type  [history_last % DEMO_SERVER_CONNECTION_HISTORY_LENGTH] = "free";
        self->history_links [history_last % DEMO_SERVER_CONNECTION_HISTORY_LENGTH] = self->links;
#endif

//  This ensures the thread is valid until the connection is no longer used
smt_thread_unlink (&self->thread);
#if (defined (BASE_THREADSAFE))
    if (self->rwlock)
        icl_rwlock_destroy (&self->rwlock);
#endif
        memset (&self->object_tag, 0, sizeof (demo_server_connection_t) - ((byte *) &self->object_tag - (byte *) self));
//        memset (self, 0, sizeof (demo_server_connection_t));
        self->object_tag = DEMO_SERVER_CONNECTION_DEAD;
        icl_mem_free (self);
    }
    self = NULL;
}
/*  -------------------------------------------------------------------------
    demo_server_connection_set_address

    Type: Component method
    Accepts a demo_server_connection_t reference and returns zero in case of success,
    1 in case of errors.
    Sets the connection's address string, which can be used for logging
    and reporting.
    -------------------------------------------------------------------------
 */

int
    demo_server_connection_set_address (
    demo_server_connection_t * self,    //  Reference to object
    char * address                      //  Client address
)
{
#if (defined (BASE_THREADSAFE))
    icl_rwlock_t
        *rwlock;
#endif

    int
        rc = 0;                         //  Return code

#if (defined (BASE_THREADSAFE))
    rwlock = self ? self->rwlock : NULL;
    if (rwlock)
        icl_rwlock_write_lock (rwlock);
#endif

DEMO_SERVER_CONNECTION_ASSERT_SANE (self);
if (!self->zombie) {

//
icl_shortstr_cpy (self->client_address, address);
}
else
    rc = -1;                        //  Return error on zombie object.

#if (defined (BASE_THREADSAFE))
    if (rwlock)
        icl_rwlock_unlock (rwlock);
#endif


    return (rc);
}
/*  -------------------------------------------------------------------------
    demo_server_connection_set_trace

    Type: Component method
    Accepts a demo_server_connection_t reference and returns zero in case of success,
    1 in case of errors.
    Sets the connection's trace level, used to determine the logging done
    at the connection thread level (in the server agent).
    -------------------------------------------------------------------------
 */

int
    demo_server_connection_set_trace (
    demo_server_connection_t * self,    //  Reference to object
    int trace                           //  Desired trace level
)
{
#if (defined (BASE_THREADSAFE))
    icl_rwlock_t
        *rwlock;
#endif

    int
        rc = 0;                         //  Return code

#if (defined (BASE_THREADSAFE))
    rwlock = self ? self->rwlock : NULL;
    if (rwlock)
        icl_rwlock_write_lock (rwlock);
#endif

DEMO_SERVER_CONNECTION_ASSERT_SANE (self);
if (!self->zombie) {

//
demo_server_agent_set_trace (self->thread, trace);
self->trace = trace;                //  Last known value
}
else
    rc = -1;                        //  Return error on zombie object.

#if (defined (BASE_THREADSAFE))
    if (rwlock)
        icl_rwlock_unlock (rwlock);
#endif


    return (rc);
}
/*  -------------------------------------------------------------------------
    demo_server_connection_kill

    Type: Component method
    Accepts a demo_server_connection_t reference and returns zero in case of success,
    1 in case of errors.
    Kills the connection by sending a connection-kill method to the
    agent.
    -------------------------------------------------------------------------
 */

int
    demo_server_connection_kill (
    demo_server_connection_t * self     //  Reference to object
)
{
#if (defined (BASE_THREADSAFE))
    icl_rwlock_t
        *rwlock;
#endif

    int
        rc = 0;                         //  Return code

#if (defined (BASE_THREADSAFE))
    rwlock = self ? self->rwlock : NULL;
    if (rwlock)
        icl_rwlock_read_lock (rwlock);
#endif

DEMO_SERVER_CONNECTION_ASSERT_SANE (self);
if (!self->zombie) {

//
demo_server_agent_kill_connection (self->thread);
}
else
    rc = -1;                        //  Return error on zombie object.

#if (defined (BASE_THREADSAFE))
    if (rwlock)
        icl_rwlock_unlock (rwlock);
#endif


    return (rc);
}
/*  -------------------------------------------------------------------------
    demo_server_connection_ready

    Type: Component method
    Accepts a demo_server_connection_t reference and returns zero in case of success,
    1 in case of errors.
    -------------------------------------------------------------------------
 */

int
    demo_server_connection_ready (
    demo_server_connection_t * self     //  Reference to object
)
{
#if (defined (BASE_THREADSAFE))
    icl_rwlock_t
        *rwlock;
#endif

    int
        rc = 0;                         //  Return code

#if (defined (BASE_THREADSAFE))
    rwlock = self ? self->rwlock : NULL;
    if (rwlock)
        icl_rwlock_read_lock (rwlock);
#endif

DEMO_SERVER_CONNECTION_ASSERT_SANE (self);
if (!self->zombie) {

rc = TRUE;
}
else
    rc = -1;                        //  Return error on zombie object.

#if (defined (BASE_THREADSAFE))
    if (rwlock)
        icl_rwlock_unlock (rwlock);
#endif


    return (rc);
}
/*  -------------------------------------------------------------------------
    demo_server_connection_raise_exception

    Type: Component method
    Accepts a demo_server_connection_t reference and returns zero in case of success,
    1 in case of errors.
    Raise a connection exception.  This method is for internal use by 
    the connection class or server agent only.  External callers should 
    call the "error" method instead.
    -------------------------------------------------------------------------
 */

int
    demo_server_connection_raise_exception (
    demo_server_connection_t * self,    //  Reference to object
    dbyte reply_code,                   //  Error code
    char * reply_text,                  //  Error text
    dbyte faulting_method_id,           //  Faulting method id
    dbyte faulting_class_id             //  Faulting class id
)
{
    int
        rc = 0;                         //  Return code

DEMO_SERVER_CONNECTION_ASSERT_SANE (self);
if (!self->zombie) {

//
//  TODO Find a way to assert this is not being called from the wrong place
self->exception_raised  = TRUE;
self->reply_code = reply_code;
icl_shortstr_cpy (self->reply_text, reply_text);
self->faulting_class_id = faulting_class_id;
self->faulting_method_id = faulting_method_id;
}
else
    rc = -1;                        //  Return error on zombie object.


    return (rc);
}
/*  -------------------------------------------------------------------------
    demo_server_connection_error

    Type: Component method
    Accepts a demo_server_connection_t reference and returns zero in case of success,
    1 in case of errors.
    Raise a connection exception.  This method is for external use only,
    internal callers should call the "raise exception" method instead.
    -------------------------------------------------------------------------
 */

int
    demo_server_connection_error (
    demo_server_connection_t * self,    //  Reference to object
    dbyte reply_code,                   //  Error code
    char * reply_text,                  //  Error text
    dbyte faulting_method_id,           //  Faulting method id
    dbyte faulting_class_id             //  Faulting class id
)
{
    int
        rc = 0;                         //  Return code

DEMO_SERVER_CONNECTION_ASSERT_SANE (self);
if (!self->zombie) {

//
demo_server_agent_connection_error (self->thread, reply_code, reply_text,
    faulting_method_id, faulting_class_id);
}
else
    rc = -1;                        //  Return error on zombie object.


    return (rc);
}
/*  -------------------------------------------------------------------------
    demo_server_connection_exception_raised

    Type: Component method
    Accepts a demo_server_connection_t reference and returns zero in case of success,
    1 in case of errors.
    Returns TRUE if a connection exception has been raised.
    -------------------------------------------------------------------------
 */

int
    demo_server_connection_exception_raised (
    demo_server_connection_t * self     //  Reference to object
)
{
#if (defined (BASE_THREADSAFE))
    icl_rwlock_t
        *rwlock;
#endif

    int
        rc = 0;                         //  Return code

#if (defined (BASE_THREADSAFE))
    rwlock = self ? self->rwlock : NULL;
    if (rwlock)
        icl_rwlock_read_lock (rwlock);
#endif

DEMO_SERVER_CONNECTION_ASSERT_SANE (self);
if (!self->zombie) {

rc = self->exception_raised;
}
else
    rc = -1;                        //  Return error on zombie object.

#if (defined (BASE_THREADSAFE))
    if (rwlock)
        icl_rwlock_unlock (rwlock);
#endif


    return (rc);
}
/*  -------------------------------------------------------------------------
    demo_server_connection_tune_ok

    Type: Component method
    Accepts a demo_server_connection_t reference and returns zero in case of success,
    1 in case of errors.
    Implements the ASL Connection.Tune-Ok protocol method.
    -------------------------------------------------------------------------
 */

int
    demo_server_connection_tune_ok (
    demo_server_connection_t * self,    //  Reference to object
    demo_server_connection_tune_ok_t * method  //  Not documented
)
{
#if (defined (BASE_THREADSAFE))
    icl_rwlock_t
        *rwlock;
#endif

    int
        rc = 0;                         //  Return code

#if (defined (BASE_THREADSAFE))
    rwlock = self ? self->rwlock : NULL;
    if (rwlock)
        icl_rwlock_write_lock (rwlock);
#endif

DEMO_SERVER_CONNECTION_ASSERT_SANE (self);
if (!self->zombie) {

//
self->heartbeat = method->heartbeat;
self->frame_max = method->frame_max;
if (self->frame_max < ASL_FRAME_MIN_SIZE
&&  self->frame_max > 0)
    self->frame_max = ASL_FRAME_MIN_SIZE;
}
else
    rc = -1;                        //  Return error on zombie object.

#if (defined (BASE_THREADSAFE))
    if (rwlock)
        icl_rwlock_unlock (rwlock);
#endif


    return (rc);
}
/*  -------------------------------------------------------------------------
    demo_server_connection_open

    Type: Component method
    Accepts a demo_server_connection_t reference and returns zero in case of success,
    1 in case of errors.
    Implements the ASL Connection.Open protocol method.
    -------------------------------------------------------------------------
 */

int
    demo_server_connection_open (
    demo_server_connection_t * self,    //  Reference to object
    demo_server_connection_open_t * method  //  Not documented
)
{
#if (defined (BASE_THREADSAFE))
    icl_rwlock_t
        *rwlock;
#endif

    int
        rc = 0;                         //  Return code

#if (defined (BASE_THREADSAFE))
    rwlock = self ? self->rwlock : NULL;
    if (rwlock)
        icl_rwlock_write_lock (rwlock);
#endif

DEMO_SERVER_CONNECTION_ASSERT_SANE (self);
if (!self->zombie) {

}
else
    rc = -1;                        //  Return error on zombie object.

#if (defined (BASE_THREADSAFE))
    if (rwlock)
        icl_rwlock_unlock (rwlock);
#endif


    return (rc);
}
/*  -------------------------------------------------------------------------
    demo_server_connection_selftest

    Type: Component method
    -------------------------------------------------------------------------
 */

void
    demo_server_connection_selftest (
void)
{

}
/*  -------------------------------------------------------------------------
    demo_server_connection_count

    Type: Component method
    -------------------------------------------------------------------------
 */

qbyte
    demo_server_connection_count (
void)
{
    qbyte
        count;                          //  Number of instances

count = (int) s_demo_server_connection_count;

    return (count);
}
/*  -------------------------------------------------------------------------
    demo_server_connection_remove_from_all_containers

    Type: Component method
    The method to call to remove an item from its container.  Is called by
    the 'destroy' method if the possession count hits zero.
    The method to call to remove an item from its container.  Is called by
    the 'destroy' method if the possession count hits zero.
    -------------------------------------------------------------------------
 */

void
    demo_server_connection_remove_from_all_containers (
    demo_server_connection_t * self     //  The itemThe item
)
{

DEMO_SERVER_CONNECTION_ASSERT_SANE (self);
demo_server_connection_table_remove (self);
demo_server_connection_list_remove (self);
}
/*  -------------------------------------------------------------------------
    demo_server_connection_show

    Type: Component method
    -------------------------------------------------------------------------
 */

void
    demo_server_connection_show_ (
    void * item,                        //  The opaque pointer
    int opcode,                         //  The callback opcode
    FILE * trace_file,                  //  File to print to
    char * file,                        //  Source file
    size_t line                         //  Line number
)
{
    demo_server_connection_t
        *self;
    int
        container_links;
#if defined (DEBUG) || defined (BASE_HISTORY) || defined (BASE_HISTORY_DEMO_SERVER_CONNECTION)
    qbyte
        history_index;
#endif


self = item;
container_links = 0;
if (self->table_head)
   container_links++;
if (self->list_head)
   container_links++;
    assert (opcode == ICL_CALLBACK_DUMP);

    fprintf (trace_file, "    <demo_server_connection zombie = \"%u\" links = \"%u\" containers = \"%u\" file = \"%s\" line = \"%lu\"  pointer = \"%p\" />\n", self->zombie, self->links, container_links, file, (unsigned long) line, self);
#if defined (DEBUG) || defined (BASE_HISTORY) || defined (BASE_HISTORY_DEMO_SERVER_CONNECTION)
    if (self->history_last > DEMO_SERVER_CONNECTION_HISTORY_LENGTH) {
        fprintf (trace_file, "        <!-- possess history too large (%d) - call iMatix-tech -->\n",
            self->history_last);
        history_index = (self->history_last + 1) % DEMO_SERVER_CONNECTION_HISTORY_LENGTH;
        self->history_last %= DEMO_SERVER_CONNECTION_HISTORY_LENGTH;
    }
    else
        history_index = 0;

    for (; history_index != self->history_last; history_index = (history_index + 1) % DEMO_SERVER_CONNECTION_HISTORY_LENGTH) {
        fprintf (trace_file, "       <%s file = \"%s\" line = \"%lu\" links = \"%lu\" />\n",
            self->history_type [history_index],
            self->history_file [history_index],
            (unsigned long) self->history_line  [history_index],
            (unsigned long) self->history_links [history_index]);
    }
    fprintf (trace_file, "    </demo_server_connection>\n");
#endif

}
/*  -------------------------------------------------------------------------
    demo_server_connection_terminate

    Type: Component method
    -------------------------------------------------------------------------
 */

static void
    demo_server_connection_terminate (
void)
{

if (s_demo_server_connection_active) {

#if (defined (BASE_THREADSAFE))
        icl_mutex_destroy (&s_demo_server_connection_mutex);
#endif
        s_demo_server_connection_active = FALSE;
    }
}
/*  -------------------------------------------------------------------------
    demo_server_connection_destroy

    Type: Component method
    Destroys a demo_server_connection_t object. Takes the address of a
    demo_server_connection_t reference (a pointer to a pointer) and nullifies the
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
    demo_server_connection_destroy_ (
    demo_server_connection_t * ( * self_p ),   //  Reference to object reference
    char * file,                        //  Source fileSource file
    size_t line                         //  Line numberLine number
)
{
    demo_server_connection_t *
        self = *self_p;                 //  Dereferenced Reference to object reference

if (self) {
    if (icl_atomic_cas32 (&self->zombie, TRUE, FALSE) == FALSE)
        demo_server_connection_annihilate_ (self_p, file, line);

    if (self->links == 0) {
        icl_system_panic ("demo_server_connection", "E: missing link on demo_server_connection object");
        demo_server_connection_show_ (self, ICL_CALLBACK_DUMP, stderr, file, line);
        abort ();
    }

    if (icl_atomic_dec32 ((volatile qbyte *) &self->links) == 0)
        demo_server_connection_free_ ((demo_server_connection_t *) self, file, line);
    *self_p = NULL;
}
}
/*  -------------------------------------------------------------------------
    demo_server_connection_alloc

    Type: Component method
    -------------------------------------------------------------------------
 */

static demo_server_connection_t *
    demo_server_connection_alloc_ (
    char * file,                        //  Source file for call
    size_t line                         //  Line number for call
)
{

    demo_server_connection_t *
        self = NULL;                    //  Object reference

//  Initialise cache if necessary
if (!s_cache)
    demo_server_connection_cache_initialise ();

self = (demo_server_connection_t *) icl_mem_cache_alloc_ (s_cache, file, line);
if (self)
    memset (self, 0, sizeof (demo_server_connection_t));



    return (self);
}
/*  -------------------------------------------------------------------------
    demo_server_connection_read_lock

    Type: Component method
    Accepts a demo_server_connection_t reference and returns zero in case of success,
    1 in case of errors.
    -------------------------------------------------------------------------
 */

int
    demo_server_connection_read_lock (
    demo_server_connection_t * self     //  Reference to object
)
{
    int
        rc = 0;                         //  Return code

DEMO_SERVER_CONNECTION_ASSERT_SANE (self);
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
    demo_server_connection_write_lock

    Type: Component method
    Accepts a demo_server_connection_t reference and returns zero in case of success,
    1 in case of errors.
    -------------------------------------------------------------------------
 */

int
    demo_server_connection_write_lock (
    demo_server_connection_t * self     //  Reference to object
)
{
    int
        rc = 0;                         //  Return code

DEMO_SERVER_CONNECTION_ASSERT_SANE (self);
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
    demo_server_connection_unlock

    Type: Component method
    Accepts a demo_server_connection_t reference and returns zero in case of success,
    1 in case of errors.
    -------------------------------------------------------------------------
 */

int
    demo_server_connection_unlock (
    demo_server_connection_t * self     //  Reference to object
)
{
    int
        rc = 0;                         //  Return code

DEMO_SERVER_CONNECTION_ASSERT_SANE (self);
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
    demo_server_connection_link

    Type: Component method
    Adds a link (reference count) to an object.

    If the object has been zombified (ie destroyed while extra links were present),
    this method returns NULL and does not add any links.

    This method does not lock the object.
    -------------------------------------------------------------------------
 */

demo_server_connection_t *
    demo_server_connection_link_ (
    demo_server_connection_t * self,    //  Not documented
    char * file,                        //  Source file for call
    size_t line                         //  Line number for call
)
{
#if defined (DEBUG) || defined (BASE_HISTORY) || defined (BASE_HISTORY_DEMO_SERVER_CONNECTION)
    int
        history_last;
#endif

    if (self) {
        DEMO_SERVER_CONNECTION_ASSERT_SANE (self);
        if (file)
            icl_mem_possess_ (self, file, line);
        icl_atomic_inc32 ((volatile qbyte *) &self->links);
#if defined (DEBUG) || defined (BASE_HISTORY) || defined (BASE_HISTORY_DEMO_SERVER_CONNECTION)
        //  Track possession operation in history
        history_last = icl_atomic_inc32 ((volatile qbyte *) &self->history_last) + 1;
        self->history_file  [history_last % DEMO_SERVER_CONNECTION_HISTORY_LENGTH] = file;
        self->history_line  [history_last % DEMO_SERVER_CONNECTION_HISTORY_LENGTH] = line;
        self->history_type  [history_last % DEMO_SERVER_CONNECTION_HISTORY_LENGTH] = "link";
        self->history_links [history_last % DEMO_SERVER_CONNECTION_HISTORY_LENGTH] = self->links;
#endif
    }

    return (self);
}
/*  -------------------------------------------------------------------------
    demo_server_connection_unlink

    Type: Component method
    Removes a link (reference count) to an object.  Sets the object pointer to NULL
    to indicate that it is no longer valid.
    -------------------------------------------------------------------------
 */

void
    demo_server_connection_unlink_ (
    demo_server_connection_t * ( * self_p ),   //  Reference to object reference
    char * file,                        //  Source file for call
    size_t line                         //  Line number for call
)
{
#if defined (DEBUG) || defined (BASE_HISTORY) || defined (BASE_HISTORY_DEMO_SERVER_CONNECTION)
    int
        history_last;
#endif

    demo_server_connection_t *
        self = *self_p;                 //  Dereferenced Reference to object reference

    if (self) {
        DEMO_SERVER_CONNECTION_ASSERT_SANE (self);
        if (self->links == 0) {
            icl_system_panic ("demo_server_connection", "E: missing link on demo_server_connection object");
            demo_server_connection_show_ (self, ICL_CALLBACK_DUMP, stderr, file, line);
            abort ();
        }

#if defined (DEBUG) || defined (BASE_HISTORY) || defined (BASE_HISTORY_DEMO_SERVER_CONNECTION)
        //  Track possession operation in history.  Pre-empt value of links
        //  after operation; otherwise race condition can result in writing
        //  to freed memory.
        history_last = icl_atomic_inc32 ((volatile qbyte *) &self->history_last) + 1;
        self->history_file  [history_last % DEMO_SERVER_CONNECTION_HISTORY_LENGTH] = file;
        self->history_line  [history_last % DEMO_SERVER_CONNECTION_HISTORY_LENGTH] = line;
        self->history_type  [history_last % DEMO_SERVER_CONNECTION_HISTORY_LENGTH] = "unlink";
        self->history_links [history_last % DEMO_SERVER_CONNECTION_HISTORY_LENGTH] = self->links - 1;
#endif
        if (icl_atomic_dec32 ((volatile qbyte *) &self->links) == 0) {

        if (icl_atomic_cas32 (&self->zombie, TRUE, FALSE) == FALSE)
            demo_server_connection_annihilate_ (self_p, file, line);
        demo_server_connection_free_ ((demo_server_connection_t *) self, file, line);
    }
    *self_p = NULL;
}
}
/*  -------------------------------------------------------------------------
    demo_server_connection_cache_initialise

    Type: Component method
    Initialise the cache and register purge method with the meta-cache.
    -------------------------------------------------------------------------
 */

static void
    demo_server_connection_cache_initialise (
void)
{

s_cache = icl_cache_get (sizeof (demo_server_connection_t));
icl_system_register (demo_server_connection_cache_purge, demo_server_connection_cache_terminate);
}
/*  -------------------------------------------------------------------------
    demo_server_connection_cache_purge

    Type: Component method
    -------------------------------------------------------------------------
 */

static void
    demo_server_connection_cache_purge (
void)
{

icl_mem_cache_purge (s_cache);

}
/*  -------------------------------------------------------------------------
    demo_server_connection_cache_terminate

    Type: Component method
    -------------------------------------------------------------------------
 */

static void
    demo_server_connection_cache_terminate (
void)
{

s_cache = NULL;

}
/*  -------------------------------------------------------------------------
    demo_server_connection_show_animation

    Type: Component method
    Enables animation of the component. Animation is sent to stdout.
    To enable animation you must generate using the option -animate:1.
    -------------------------------------------------------------------------
 */

void
    demo_server_connection_show_animation (
    Bool enabled                        //  Are we enabling or disabling animation?
)
{

demo_server_connection_animating = enabled;
}
/*  -------------------------------------------------------------------------
    demo_server_connection_new_in_scope

    Type: Component method
    -------------------------------------------------------------------------
 */

void
    demo_server_connection_new_in_scope_ (
    demo_server_connection_t * * self_p,   //  Not documented
    icl_scope_t * _scope,               //  Not documented
    char * file,                        //  Source file for call
    size_t line,                        //  Line number for call
    demo_server_connection_table_t * table,   //  Table to insert into
    smt_thread_t * thread               //  Parent thread
)
{
    icl_destroy_t *
        _destroy;                       //  Not documented

*self_p = demo_server_connection_new_ (file,line,table,thread);

if (*self_p) {
    _destroy = icl_destroy_new   ((void * *) self_p, (icl_destructor_fn *) demo_server_connection_destroy_);
    icl_destroy_list_queue (_scope, _destroy);
    icl_destroy_unlink (&_destroy);
}
}
//  Embed the version information in the resulting binary

char *EMBED__demo_server_connection_version_start  = "VeRsIoNsTaRt:ipc";
char *EMBED__demo_server_connection_component  = "demo_server_connection ";
char *EMBED__demo_server_connection_version  = "1.0 ";
char *EMBED__demo_server_connection_copyright  = "Copyright (c) 1996-2009 iMatix Corporation";
char *EMBED__demo_server_connection_filename  = "demo_server_connection.icl ";
char *EMBED__demo_server_connection_builddate  = "2011/03/01 ";
char *EMBED__demo_server_connection_version_end  = "VeRsIoNeNd:ipc";

