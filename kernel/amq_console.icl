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
    <argument name = "content" type = "amq_content_jms_t *">The message content</argument>
    <possess>
    amq_content_jms_possess (content);
    </possess>
    <release>
    amq_content_jms_destroy (&content);
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
    bucket->cur_size = amq_content_jms_get_body (
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
                if (streq (ipr_xml_name (xml_command), "schema-request"))
                    s_execute_schema (content, xml_command);
                else 
                if (streq (ipr_xml_name (xml_command), "inspect-request"))
                    s_execute_inspect (self, content, xml_command);
                else 
                if (streq (ipr_xml_name (xml_command), "modify-request"))
                    s_execute_modify (self, content, xml_command);
                else 
                if (streq (ipr_xml_name (xml_command), "monitor-request"))
                    s_execute_monitor (self, content, xml_command);
                else 
                if (streq (ipr_xml_name (xml_command), "method-request"))
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
    <argument name = "request"   type = "amq_content_jms_t *">Original request</argument>
    <argument name = "object id" type = "qbyte">Object id</argument>
    <argument name = "fields"    type = "asl_field_list_t *">Object fields</argument>
    <possess>
    amq_content_jms_possess (request);
    asl_field_list_possess (fields);
    </possess>
    <release>
    amq_content_jms_destroy (&request);
    asl_field_list_destroy (&fields);
    </release>
    <action>
    asl_field_t
        *field;
    ipr_xml_t
        *cml_item,                      //  CML item
        *cur_item,                      //  Top level object
        *sub_item,                      //  Field or class within object
        *val_item;                      //  Value of field
    icl_shortstr_t
        strvalue;                       //  Stringified numeric value
    
    cml_item = ipr_xml_new (NULL, "cml", NULL);
    ipr_xml_attr_set (cml_item, "version", "1.0");
    
    cur_item = ipr_xml_new (cml_item, "inspect-reply", NULL);
    ipr_xml_attr_set (cur_item, "class",  self->class_ref [object_id]->name);
    ipr_xml_attr_set (cur_item, "object", icl_shortstr_fmt (strvalue, "%ld", object_id));
    ipr_xml_attr_set (cur_item, "status", "ok");

    field = asl_field_list_first (fields);
    while (field) {
        sub_item = ipr_xml_new (cur_item, "field", NULL);
        ipr_xml_attr_set (sub_item, "name", field->name);
        val_item = ipr_xml_new (sub_item, NULL, asl_field_string (field));
        ipr_xml_unlink (&val_item);
        ipr_xml_unlink (&sub_item);
        field = asl_field_list_next (&field);
    }
    s_reply_xml (request, cml_item);
    ipr_xml_unlink  (&cur_item);
    ipr_xml_destroy (&cml_item);
    </action>
</method>

<method name = "modify ok" template = "async function" async = "1">
    <doc>
    Accepts a modify response from an object.
    </doc>
    <argument name = "request"   type = "amq_content_jms_t *">Original request</argument>
    <argument name = "object id" type = "qbyte">Object id</argument>
    <possess>
    amq_content_jms_possess (request);
    </possess>
    <release>
    amq_content_jms_destroy (&request);
    </release>
    <action>
    ipr_xml_t
        *cml_item,                      //  CML item
        *cur_item;                      //  Top level object
    icl_shortstr_t
        strvalue;                       //  Stringified numeric value
    
    cml_item = ipr_xml_new (NULL, "cml", NULL);
    ipr_xml_attr_set (cml_item, "version", "1.0");
    cur_item = ipr_xml_new (cml_item, "modify-reply", NULL);
    ipr_xml_attr_set (cur_item, "object", icl_shortstr_fmt (strvalue, "%ld", object_id));
    ipr_xml_attr_set (cur_item, "status", "ok");
    s_reply_xml (request, cml_item);
    ipr_xml_unlink  (&cur_item);
    ipr_xml_destroy (&cml_item);
    </action>
</method>

<private name = "async header">
//  This file is generated when we build the project
#define AMQ_CONSOLE_SCHEMA      "amq_console_schema.cml"

static void
    s_execute_schema  (amq_content_jms_t *request, ipr_xml_t *xml_command);
static void
    s_execute_inspect (amq_console_t *self, amq_content_jms_t *request, ipr_xml_t *xml_command);
static void
    s_execute_modify  (amq_console_t *self, amq_content_jms_t *request, ipr_xml_t *xml_command);
static void
    s_execute_monitor (amq_console_t *self, amq_content_jms_t *request, ipr_xml_t *xml_command);
static void
    s_execute_method  (amq_console_t *self, amq_content_jms_t *request, ipr_xml_t *xml_command);
static void
    s_invalid_cml     (amq_content_jms_t *request, ipr_bucket_t *bucket, char *error);
static void
    s_reply_error     (amq_content_jms_t *request, char *top, char *status);
static void
    s_reply_xml       (amq_content_jms_t *request, ipr_xml_t *xml_item);
static void
    s_reply_bucket    (amq_content_jms_t *request, ipr_bucket_t *bucket);
</private>

<private name = "async footer">
static void
s_execute_schema (amq_content_jms_t *request, ipr_xml_t *xml_command)
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
            s_reply_error (request, "schema-reply", "notfound");
        }
    }
    else {
        icl_console_print ("E: can't find '%s'", schema_file);
        s_reply_error (request, "schema-reply", "notfound");
    }
}

static void
s_execute_inspect (
    amq_console_t *self,
    amq_content_jms_t *request,
    ipr_xml_t *xml_command)
{
    char
        *object_str;
    qbyte
        object_id;

    object_str = ipr_xml_attr_get (xml_command, "object", "");
    if (*object_str) {
        object_id = atol (object_str);
        if (object_id < self->max_objects
        && self->object_ref [object_id]) {
            self->class_ref [object_id]->inspect (self->object_ref [object_id], request);
        }
        else
            s_reply_error (request, "inspect-reply", "notfound");
    }        
    else
        s_reply_error (request, "inspect-reply", "invalid");
}

static void
s_execute_modify (
    amq_console_t *self,
    amq_content_jms_t *request,
    ipr_xml_t *xml_command)
{
    char
        *object_str,
        *field_value;
    qbyte
        object_id;
    asl_field_list_t
        *fields;                        //  Properties to set
    ipr_xml_t
        *xml_field;

    object_str = ipr_xml_attr_get (xml_command, "object", "");
    if (*object_str) {
        object_id = atol (object_str);
        if (object_id < self->max_objects
        && self->object_ref [object_id]) {
            fields = asl_field_list_new (NULL);
            xml_field = ipr_xml_first_child (xml_command);
            while (xml_field) {
                field_value = ipr_xml_child_value (xml_field);
                asl_field_new_string (
                    fields,
                    ipr_xml_attr_get (xml_field, "name", "unnamed"),
                    field_value);
                icl_mem_free (field_value);
                xml_field = ipr_xml_next_sibling (&xml_field);
            }
            self->class_ref [object_id]->modify (
                self->object_ref [object_id], request, fields);
            asl_field_list_destroy (&fields);
        }
        else
            s_reply_error (request, "modify-reply", "notfound");
    }        
    else
        s_reply_error (request, "modify-reply", "invalid");
}

static void
s_execute_monitor (
    amq_console_t *self, 
    amq_content_jms_t *request,
    ipr_xml_t *xml_command)
{
    icl_console_print ("amq_console: monitor");
}

static void
s_execute_method (
    amq_console_t *self,
    amq_content_jms_t *request,
    ipr_xml_t *xml_command)
{
    icl_console_print ("amq_console: method");
}

static void
s_invalid_cml (amq_content_jms_t *request, ipr_bucket_t *bucket, char *error)
{
    icl_console_print ("W: amq.console: content body is not valid CML: %s", error);
    ipr_bucket_dump (bucket);
    s_reply_error (request, "invalid", NULL);
}

static void
s_reply_error (amq_content_jms_t *request, char *top, char *status)
{
    ipr_xml_t
        *cml_item,
        *cur_item;

    cml_item = ipr_xml_new (NULL, "cml", NULL);
    ipr_xml_attr_set (cml_item, "version", "1.0");
    cur_item = ipr_xml_new (cml_item, top, NULL);
    if (status) 
        ipr_xml_attr_set (cur_item, "status", status);

    s_reply_xml (request, cml_item);
    ipr_xml_unlink  (&cur_item);
    ipr_xml_destroy (&cml_item);
}


//  Send XML tree as reply back to original requestor, using same
//  message id and using original reply-to as routing key.
//
static void
s_reply_xml (amq_content_jms_t *request, ipr_xml_t *xml_item)
{
    char
        *xml_text;                      //  Serialised XML text
    ipr_bucket_t
        *bucket;
        
    xml_text = ipr_xml_save_string (xml_item);
    bucket = ipr_bucket_new (strlen (xml_text));
    ipr_bucket_fill (bucket, xml_text, strlen (xml_text));
    icl_mem_free (xml_text);
    s_reply_bucket (request, bucket);
    ipr_bucket_destroy (&bucket);
}


//  Send ipr bucket reply back to original requestor, using same
//  message id and using original reply-to as routing key.
//
static void
s_reply_bucket (amq_content_jms_t *request, ipr_bucket_t *bucket)
{
    amq_content_jms_t
        *reply;                         //  Reply content
    amq_exchange_t
        *exchange;                      //  Reply to amq.direct

    icl_console_print ("I: replying to %s", request->reply_to);
    exchange = amq_exchange_search (amq_vhost->exchange_table, "amq.direct");
    assert (exchange);

    reply = amq_content_jms_new ();
    amq_content_jms_set_message_id   (reply, request->message_id);
    amq_content_jms_set_content_type (reply, "text/xml");
    amq_content_jms_record_body      (reply, bucket);
    amq_content_jms_set_routing_key  (reply, "amq.direct", request->reply_to, 0);

    amq_exchange_publish (exchange, NULL, AMQ_SERVER_JMS, reply, FALSE, FALSE);
    amq_exchange_unlink (&exchange);
    amq_content_jms_destroy (&reply);
}
</private>

<method name = "selftest">
    smt_os_thread_initialise ();
    amq_console = amq_console_new ();
    amq_console_destroy (&amq_console);
</method>

</class>
