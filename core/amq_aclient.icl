<?xml?>
<class
    name      = "amq_aclient"
    comment   = "AMQP asynchronous client API"
    version   = "1.0"
    copyright = "Copyright (c) 2004 JPMorgan"
    script    = "icl_gen"
    >

<inherit class = "icl_alloc_cache" />
<option name = "nullify" value = "1" />

<import class = "ipr_classes" />
<import class = "smt_thread"  />
<import class = "amq_message" />
<private>
#include "amq_aclient_agent.h"
</private>

<public>
typedef enum {
    AMQ_ACLIENT_CONNECTED,
    AMQ_ACLIENT_HANDLE_CREATED,
    AMQ_ACLIENT_HANDLE_REPLY,
    AMQ_ACLIENT_HANDLE_NOTIFY,
    AMQ_ACLIENT_HANDLE_CLOSE,
    AMQ_ACLIENT_CHANNEL_CLOSE,
    AMQ_ACLIENT_MONITOR
} amq_aclient_callback_t;

typedef struct {
    amq_aclient_t *client;
} amq_aclient_connected_t;

typedef struct {
    amq_aclient_t *client;
    dbyte          handle_id;
    ipr_shortstr_t dest_name;
} amq_aclient_handle_created_t;

typedef struct {
    amq_aclient_t *client;
    dbyte          handle_id;
    dbyte          confirm_tag;
    dbyte          reply_code;
    ipr_shortstr_t reply_text;
} amq_aclient_handle_reply_t;

typedef struct {
    amq_aclient_t *client;
    dbyte          handle_id;
    qbyte          message_nbr;
    Bool           delivered;
    Bool           redelivered;
    ipr_shortstr_t dest_name;
    amq_message_t *message;
} amq_aclient_handle_notify_t;

typedef struct {
    amq_aclient_t *client;
    dbyte          handle_id;
} amq_aclient_handle_close_t;

typedef struct {
    amq_aclient_t *client;
    dbyte          channel_id;
} amq_aclient_channel_close_t;

typedef struct {
    amq_aclient_t *client;
} amq_aclient_monitor_t;

/*  Function types for each callback                                         */
typedef void (amq_aclient_connected_fn)      (amq_aclient_connected_t      *args);
typedef void (amq_aclient_handle_created_fn) (amq_aclient_handle_created_t *args);
typedef void (amq_aclient_handle_reply_fn)   (amq_aclient_handle_reply_t   *args);
typedef void (amq_aclient_handle_notify_fn)  (amq_aclient_handle_notify_t  *args);
typedef void (amq_aclient_handle_close_fn)   (amq_aclient_handle_close_t   *args);
typedef void (amq_aclient_channel_close_fn)  (amq_aclient_channel_close_t  *args);
typedef void (amq_aclient_monitor_fn)        (amq_aclient_monitor_t        *args);
</public>

<context>
    smt_thread_handle_t
        *thread_handle;
    dbyte
        top_channel;
    dbyte
        top_handle;
</context>

<method name = "new">
    <argument name = "client name" type = "char *">Client identifier</argument>
    <argument name = "login"       type = "char *">User login name</argument>
    <argument name = "password"    type = "char *">User password</argument>

    if (!s_class_active)
        s_class_initialise ();

    self->top_channel = 0;
    self->top_handle  = 0;
    self->thread_handle = smt_thread_handle_new (
        amq_aclient_agent_client_thread_new (self, client_name, login, password));
</method>

<method name = "destroy">
    smt_thread_handle_destroy (&self->thread_handle);
</method>

<method name = "register" template = "function">
    <doc>
    Registers a callback function with the client API.  You can register
    up to 1 callback function per event.  Note that the type of your
    callback function must match the event you are handling.  These
    are the valid callback types and their corresponding function types:

    AMQ_ACLIENT_CONNECTED:
        amq_connected_callback_fn, called when the connection is successful.
    AMQ_ACLIENT_HANDLE_CREATED:
        amq_handle_created_callback_fn, called when a temporary destination has been
        created for you.
    AMQ_ACLIENT_HANDLE_REPLY:
        amq_handle_reply_callback_fn, called when you get a confirmation back.
    AMQ_ACLIENT_HANDLE_NOTIFY:
        amq_handle_notify_callback_fn, called when a message has arrived for you.
    AMQ_ACLIENT_HANDLE_CLOSE:
        amq_handle_close_callback_fn, called when the server closes a handle.
    AMQ_ACLIENT_CHANNEL CLOSE:
        amq_channel_close_callback_fn, called when the server closes a channel.
    AMQ_ACLIENT_MONITOR:
        amq_monitor_callback_fn, called once per second.
    </doc>
    <argument name = "callback" type = "amq_aclient_callback_t"/>
    <argument name = "function" type = "void *"/>

    amq_aclient_agent_register (self->thread_handle, callback, function);
</method>

<method name = "connect" template = "function">
    <argument name = "hostname"     type = "char *">Server to connect to</argument>
    <argument name = "virtual path" type = "char *">Virtual host path</argument>

    amq_aclient_agent_connection_open (
        self->thread_handle, hostname, virtual_path);
    rc = smt_thread_execute (SMT_EXEC_FULL);
</method>

<method name = "channel open" return = "channel_id" >
    <argument name = "self"        type = "$(selftype) *"/>
    <argument name = "transacted"  type = "Bool">Use transacted mode?</argument>
    <argument name = "restartable" type = "Bool">Use restartable mode?</argument>

    <declare name = "channel_id" type = "dbyte">New channel id</declare>

    channel_id = ++self->top_channel;
    amq_aclient_agent_channel_open (
        self->thread_handle, channel_id, transacted, restartable);
</method>

<method name = "handle open" return = "handle_id" >
    <argument name = "self"       type = "$(selftype) *"/>
    <argument name = "channel id" type = "dbyte" >Channel id</argument>
    <argument name = "temporary"  type = "Bool"  >Temporary access?</argument>
    <argument name = "dest name"  type = "char *">Destination name</argument>

    <declare name = "handle_id" type = "dbyte">New handle id</declare>

    handle_id = ++self->top_handle;
    amq_aclient_agent_handle_open (
        self->thread_handle, channel_id, handle_id, temporary, dest_name);
</method>

<method name = "handle close" template = "function" >
    <argument name = "handle id" type = "dbyte">Handle id</argument>
    amq_aclient_agent_handle_close (
        self->thread_handle, handle_id);
</method>

<method name = "handle consume" template = "function" >
    <argument name = "handle id"   type = "dbyte"          >Handle id</argument>
    <argument name = "prefetch"    type = "dbyte"          >Max pending messages</argument>
    <argument name = "no local"    type = "Bool"           >Don't deliver to self?</argument>
    <argument name = "unreliable"  type = "Bool"           >Don't want to ack</argument>
    <argument name = "dest name"   type = "ipr_shortstr_t" >Destination name</argument>
    <argument name = "identifier"  type = "ipr_shortstr_t" >Subscription identifier</argument>

    amq_aclient_agent_handle_consume (
        self->thread_handle,
        handle_id,
        prefetch,
        no_local,
        unreliable,
        dest_name,
        identifier);
</method>

<method name = "handle send" template = "function" >
    <argument name = "handle id"  type = "dbyte"          >Handle id</argument>
    <argument name = "message"    type = "amq_message_t *">Message to send</argument>
    <argument name = "dest name"  type = "ipr_shortstr_t" >Destination name</argument>
    amq_aclient_agent_handle_send (
        self->thread_handle, handle_id, message, dest_name);
</method>

<method name = "channel ack" template = "function" >
    <argument name = "channel id"  type = "dbyte">Channel id</argument>
    <argument name = "message nbr" type = "qbyte">Message number</argument>
    amq_aclient_agent_channel_ack (
        self->thread_handle, channel_id, message_nbr);
</method>

<method name = "channel commit" template = "function" >
    <argument name = "channel id"  type = "dbyte">Channel id</argument>
    amq_aclient_agent_channel_close (
        self->thread_handle, channel_id);
</method>

<method name = "channel rollback" template = "function" >
    <argument name = "channel id"  type = "dbyte">Channel id</argument>
    amq_aclient_agent_channel_rollback (
        self->thread_handle, channel_id);
</method>

<method name = "channel close" template = "function" >
    <argument name = "channel id"  type = "dbyte">Channel id</argument>
    amq_aclient_agent_channel_close (
        self->thread_handle, channel_id);
</method>

<method name = "connection close" template = "function" >
    amq_aclient_agent_connection_close (
        self->thread_handle);
</method>

<private name = "header">
/*  Class initialisation management                                          */

static Bool
    s_class_active = FALSE;
static void
    s_class_initialise (void);
static void
    s_class_terminate (void);
</private>

<private name = "footer">
static void
s_class_initialise (void)
{
    s_class_active = TRUE;

    /*  Start the client agent - gets stopped by icl_system               */
    amq_aclient_agent_init ();

    /*  Register the class termination call-back functions                   */
    icl_system_register (NULL, s_class_terminate);
}

static void
s_class_terminate (void)
{
    s_class_active = FALSE;
}
</private>

<method name = "selftest">
    <!-- see amqpcli.c -->
</method>

</class>
