<?xml?>
<class
    name      = "amq_console"
    comment   = "AMQ Console class"
    version   = "1.0"
    script    = "smt_object_gen"
    target    = "smt"
    >
<doc>
This implements the AMQ Console object, which is a system-wide
object responsible for implementing the AMQ Console service.

The console works as follows:

 - AMQ Console is instantiated as a global object of this class,
   which has its own context.

 - All operable objects register with the console class, using a
   console class object as a holder for the methods the object
   class implements.
 
 - The amq.system exchange sends it messages using the publish
   method. For now, it is hard-coded in amq.system that any
   messages with the "amq.console" will get sent to amq_console.

 - The amq_console decodes the messages and routes it to an
   object (if found), via the amq_console_class definition for
   that object.

 - The operable objects implement a register, cancel, inspect,
   and modify methods, and return their replies to amq_console
   asynchrously via inspect_ok, modify_ok.
   
 - All interfaces between amq_console and operable classes is
   done using asl_field lists.
</doc>

<inherit class = "smt_object" />

<public name = "include">
#include "amq_server_classes.h"
</public>

<public>
extern $(selftype)
    *amq_console;                       //  Single system-wide console
extern qbyte
    amq_object_id;                      //  Global object ID
</public>

<private name = "header">
qbyte
    amq_object_id = 0;
$(selftype)
    *amq_console = NULL;
</private>

<context>
    //  Quick and dirty object store, will need to be redesigned for
    //  large-scale use.  The goal of this is to allow the Console to
    //  be bootstrapped from alpha upwards...
    qbyte
        max_objects;
    void
        **object_ref;
    amq_console_class_t
        **class_ref;
    qbyte
        *parent_id;
</context>

<method name = "new">
#   define MAX_OBJECTS  32000
    self->max_objects = MAX_OBJECTS;
    self->object_ref  = icl_mem_alloc (sizeof (void *) * MAX_OBJECTS);
    self->class_ref   = icl_mem_alloc (sizeof (void *) * MAX_OBJECTS);
    self->parent_id   = icl_mem_alloc (sizeof (qbyte)  * MAX_OBJECTS);
    memset (self->object_ref, 0, sizeof (void *) * MAX_OBJECTS);
    memset (self->class_ref,  0, sizeof (void *) * MAX_OBJECTS);
    memset (self->parent_id,  0, sizeof (qbyte)  * MAX_OBJECTS);
</method>

<method name = "destroy">
    <action>
    icl_mem_free (self->object_ref);
    icl_mem_free (self->class_ref);
    icl_mem_free (self->parent_id);
    </action>
</method>

<method name = "register" template = "async function" async = "1">
    <doc>
    Accepts an object registration request.
    </doc>
    <argument name = "object id"    type = "qbyte">Object id</argument>
    <argument name = "object ref"   type = "void *">Object reference</argument>
    <argument name = "class ref"    type = "amq_console_class_t *" />
    <argument name = "parent id"    type = "qbyte">Parent object id</argument>
    <action>
    if (object_id &lt; self->max_objects) {
        self->object_ref [object_id] = object_ref;
        self->class_ref  [object_id] = class_ref;
        self->parent_id  [object_id] = parent_id;
    }
    else {
        icl_console_print ("W: Console object store is full, please restart server");
        icl_console_print ("W: Current limitation will be removed in next release");
    }
    </action>
</method>

<method name = "cancel" template = "async function" async = "1">
    <doc>
    Accepts an object cancellation request.
    </doc>
    <argument name = "object id" type = "qbyte">Object ID</argument>
    <action>
    self->object_ref [object_id] = NULL;
    </action>
</method>

<method name = "publish" template = "async function" async = "1">
    <doc>
    Accepts an AMQ Console message, which must be formatted in CML according
    to the AMQ Console specifications.
    </doc>
    <argument name = "content" type = "amq_content_basic_t *">The message content</argument>
    <possess>
    amq_content_basic_possess (content);
    </possess>
    <release>
    amq_content_basic_destroy (&content);
    </release>
    <action>
    //
    DESCR
        descr;
    ipr_bucket_t
        *bucket = NULL;                 //  Message comes here
    ipr_xml_t
        *xml_root = NULL,               //  Tree of file we've loaded         
        *xml_cml,                       //  Top level cml item
        *xml_command;                   //  Command name
    int
        rc;

    //  Get content body into a bucket
    bucket = ipr_bucket_new (IPR_BUCKET_MAX_SIZE);
    bucket->cur_size = amq_content_basic_get_body (
        content, bucket->data, bucket->max_size);
    descr.data = bucket->data;
    descr.size = bucket->cur_size;

    //  Parse as XML message
    rc = ipr_xml_load_descr (&xml_root, &descr, FALSE);
    if (rc)
        s_invalid_cml (content, bucket, "can't parse XML");
    else {
        xml_cml = ipr_xml_first_child (xml_root);
        if (xml_cml && ipr_xml_name (xml_cml)
        && streq (ipr_xml_name (xml_cml), "cml")) {
            xml_command = ipr_xml_first_child (xml_cml);
            if (xml_command && ipr_xml_name (xml_command)) {
                if (streq (ipr_xml_name (xml_command), "schema"))
                    s_execute_schema (content, xml_command);
                else 
                if (streq (ipr_xml_name (xml_command), "inspect"))
                    s_execute_inspect (self, content, xml_command);
                else 
                if (streq (ipr_xml_name (xml_command), "modify"))
                    s_execute_modify (self, content, xml_command);
                else 
                if (streq (ipr_xml_name (xml_command), "monitor"))
                    s_execute_monitor (self, content, xml_command);
                else 
                if (streq (ipr_xml_name (xml_command), "method"))
                    s_execute_method (self, content, xml_command);
                else 
                    s_invalid_cml (content, bucket, "unknown CML command");
            }
            else
                s_invalid_cml (content, bucket, "missing CML command");
            ipr_xml_unlink (&xml_command);
        }
        else
            s_invalid_cml (content, bucket, "can't parse CML");
        ipr_xml_unlink (&xml_cml);
    }
    ipr_bucket_destroy (&bucket);
    ipr_xml_destroy (&xml_root);
    </action>
</method>

<method name = "inspect ok" template = "async function" async = "1">
    <doc>
    Accepts an inspect response from an object, in the form of a field
    list which the console can then reformat as a CML response.
    </doc>
    <argument name = "request"   type = "amq_content_basic_t *">Original request</argument>
    <argument name = "object id" type = "qbyte">Object id</argument>
    <argument name = "children"  type = "qbyte">Number of children</argument>
    <argument name = "fields"    type = "asl_field_list_t *">Object properties</argument>
    <possess>
    amq_content_basic_possess (request);
    asl_field_list_possess (fields);
    </possess>
    <release>
    amq_content_basic_destroy (&request);
    asl_field_list_destroy (&fields);
    </release>
    <action>
    ipr_bucket_t
        *bucket;
    asl_field_t
        *field;

    bucket = ipr_bucket_new (IPR_BUCKET_MAX_SIZE);

    //  Better to build this as a tree and then send the tree separately
    
    sprintf (bucket->data,
        "&amp;lt;cml version = \\"1.0\\"&amp;gt;\\n"
        "    &amp;lt;inspect object = \\"%ld\\" children = \\"%ld\\" status = \\"ok\\"&amp;gt;\\n",
        object_id, children);

    field = asl_field_list_first (fields);
    while (field) {
        if (*field->name == 'F') {
            strcat (bucket->data, "    &amp;lt;field name = \\"");
            strcat (bucket->data, field->name + 1);
            strcat (bucket->data, "\\"&amp;gt;");
            strcat (bucket->data, asl_field_string (field));
            strcat (bucket->data, "&amp;lt;/field&amp;gt;\\n");
        }
        else
        if (*field->name == 'C') {
            strcat (bucket->data, "    &amp;lt;object class = \\"");
            strcat (bucket->data, field->name + 1);
            strcat (bucket->data, "\\" id = \\"");
            strcat (bucket->data, asl_field_string (field));
            strcat (bucket->data, "\\"/&amp;gt;\\n");
        }
        field = asl_field_list_next (&field);
    }
    strcat (bucket->data, "&amp;lt;/cml&amp;gt;");
    bucket->cur_size = strlen (bucket->data);
    s_reply_bucket (request, bucket);
    ipr_bucket_destroy (&bucket);
    </action>
</method>

<private name = "async header">
//  This file is generated when we build the project
#define AMQ_CONSOLE_SCHEMA      "amq_console_schema.cml"

static void
    s_execute_schema  (amq_content_basic_t *request, ipr_xml_t *xml_command);
static void
    s_execute_inspect (amq_console_t *self, amq_content_basic_t *request, ipr_xml_t *xml_command);
static void
    s_execute_modify  (amq_console_t *self, amq_content_basic_t *request, ipr_xml_t *xml_command);
static void
    s_execute_monitor (amq_console_t *self, amq_content_basic_t *request, ipr_xml_t *xml_command);
static void
    s_execute_method  (amq_console_t *self, amq_content_basic_t *request, ipr_xml_t *xml_command);
static void
    s_invalid_cml     (amq_content_basic_t *request, ipr_bucket_t *bucket, char *error);
static void
    s_reply_error     (amq_content_basic_t *request, char *body);
static void
    s_reply_bucket    (amq_content_basic_t *request, ipr_bucket_t *bucket);
</private>

<private name = "async footer">
static void
s_execute_schema (amq_content_basic_t *request, ipr_xml_t *xml_command)
{
    ipr_bucket_t
        *bucket;                        //  Schema loaded from disk
    icl_shortstr_t
        schema_file;                    //  Full name of schema file
        
    if (ipr_file_where (AMQ_CONSOLE_SCHEMA, "PATH", schema_file) == 0) {
        bucket = ipr_file_slurp (schema_file);
        if (bucket) {
            icl_console_print ("I: send schema - %s", schema_file);
            s_reply_bucket (request, bucket);
            ipr_bucket_destroy (&bucket);
        }
        else {
            icl_console_print ("E: can't read '%s'", schema_file);
            s_reply_error (request, "&amp;lt;schema status = \\"notfound\\"/&amp;gt;");
        }
    }
    else {
        icl_console_print ("E: can't find '%s'", schema_file);
        s_reply_error (request, "&amp;lt;schema status = \\"notfound\\"/&amp;gt;");
    }
}

static void
s_execute_inspect (
    amq_console_t *self,
    amq_content_basic_t *request,
    ipr_xml_t *xml_command)
{
    char
        *object_str,
        *detail_str;
    qbyte
        object_id;
        
    object_str = ipr_xml_attr_get (xml_command, "object", "");
    detail_str = ipr_xml_attr_get (xml_command, "detail", "");

    if (*object_str) {
        object_id = atol (object_str);
        if (object_id < self->max_objects
        && self->object_ref [object_id]) {
            self->class_ref [object_id]->inspect (
                self->object_ref [object_id], request, (Bool) atoi (detail_str));
        }
        else
            s_reply_error (request, "&amp;lt;inspect status = \\"notfound\\"/&amp;gt;");
    }        
    else
        s_reply_error (request, "&amp;lt;inspect status = \\"invalid\\"/&amp;gt;");
}

static void
s_execute_modify (
    amq_console_t *self,
    amq_content_basic_t *request,
    ipr_xml_t *xml_command)
{
    icl_console_print ("amq_console: modify");
}

static void
s_execute_monitor (
    amq_console_t *self, 
    amq_content_basic_t *request,
    ipr_xml_t *xml_command)
{
    icl_console_print ("amq_console: monitor");
}

static void
s_execute_method (
    amq_console_t *self,
    amq_content_basic_t *request,
    ipr_xml_t *xml_command)
{
    icl_console_print ("amq_console: method");
}

static void
s_invalid_cml (amq_content_basic_t *request, ipr_bucket_t *bucket, char *error)
{
    icl_console_print ("W: amq.console: content body is not valid CML: %s", error);
    ipr_bucket_dump (bucket);
    s_reply_error (request, "&amp;lt;invalid/&amp;gt;");
}

static void
s_reply_error (amq_content_basic_t *request, char *body)
{
    ipr_bucket_t
        *bucket;                        //  Schema loaded from disk

    icl_console_print ("I: console error: %s", body);
    bucket = ipr_bucket_new (1024);
    bucket->cur_size = sprintf (bucket->data,
        "&amp;lt;cml version = \\"1.0\\"&amp;gt;%s&amp;lt;/cml&amp;gt;", body);
    s_reply_bucket (request, bucket);
    ipr_bucket_destroy (&bucket);
}

//  Send reply back to original requestor, using same message id
//  and using original reply-to as routing key.
//
static void
s_reply_bucket (amq_content_basic_t *request, ipr_bucket_t *bucket)
{
    amq_content_basic_t
        *reply;                         //  Reply content
    amq_exchange_t
        *exchange;                      //  Reply to amq.direct

    icl_console_print ("I: replying to %s", request->reply_to);
    exchange = amq_exchange_search (amq_vhost->exchange_table, "amq.direct");
    assert (exchange);

    reply = amq_content_basic_new ();
    amq_content_basic_set_message_id   (reply, request->message_id);
    amq_content_basic_set_content_type (reply, "text/xml");
    amq_content_basic_record_body      (reply, bucket);
    amq_content_basic_set_routing_key  (reply, "amq.direct", request->reply_to, 0);

    amq_exchange_publish (exchange, NULL, AMQ_SERVER_BASIC, reply, FALSE, FALSE);
    amq_exchange_unlink (&exchange);
    amq_content_basic_destroy (&reply);
}
</private>

<method name = "selftest">
    smt_os_thread_initialise ();
    amq_console = amq_console_new ();
    amq_console_destroy (&amq_console);
</method>

</class>
