<?xml?>
<class
    name      = "amq_console"
    comment   = "AMQ Console class"
    version   = "1.0"
    script    = "smt_object_gen"
    target    = "smt"
    >
<doc>
This implements the AMQ Console object, which is a system-wide object
responsible for implementing the AMQ Console service.

The console works as follows:

 - AMQ Console is instantiated as a global object of this class, which
   has its own context.

 - All operable objects register with the console class, using a console
   class object as a holder for the methods the object class implements.

 - The amq.system exchange sends it messages using the publish method.
   For now, it is hard-coded in amq.system that any messages with the
   "amq.console" will get sent to amq_console.

 - The amq_console decodes the messages and routes it to an object (if
   found), via the amq_console_class definition for that object.

 - The operable objects implement a register, cancel, inspect, and modify
   methods, and return their replies to amq_console asynchrously via
   inspect_ok, modify_ok.

 - All interfaces between amq_console and operable classes is done using
   asl_field lists.
</doc>

<inherit class = "smt_object" />

<import class = "amq_server_classes"/>
<import class = "asl_field_list" />

<public name = "header">
#include "ezxml.h"                      //  XML parsing library

extern $(selftype)
    *amq_console;                       //  Single system-wide console
extern qbyte
    amq_object_id;                      //  Global object ID
</public>

<public name = "header">
typedef struct _amq_console_entry_t amq_console_entry_t;
</public>

<public>
struct _amq_console_entry_t {
    amq_console_class_t
        *class_ref;                     //  Reference to object class
    void
        *object_ref;                    //  Reference to object
    ipr_hash_t
        *hash;                          //  Pointer back to own hash
};
</public>

<private name = "header">
qbyte
    amq_object_id = 0;
$(selftype)
    *amq_console = NULL;
</private>

<context>
    ipr_hash_table_t
        *object_store;                  //  Object store is a hash table
</context>

<method name = "new">
    self->object_store = ipr_hash_table_new ();
</method>

<method name = "destroy">
    <action>
    uint
        table_idx;
    ipr_hash_t
        *hash;
    amq_console_entry_t
        *entry;

    for (table_idx = 0; table_idx < IPR_HASH_TABLE_MAXSIZE; table_idx++) {
        hash = self->object_store->table_items [table_idx];
        if (hash && hash != IPR_HASH_DELETED) {
            entry = hash->data;
            icl_mem_free (entry);
            ipr_hash_destroy (&hash);
        }
    }
    ipr_hash_table_destroy (&self->object_store);
    </action>
</method>

<method name = "register" template = "async function" async = "1">
    <doc>
    Accepts an object registration request.
    </doc>
    <argument name = "object id"  type = "qbyte">Object id</argument>
    <argument name = "object ref" type = "void *">Object reference</argument>
    <argument name = "class ref"  type = "amq_console_class_t *" />
    <argument name = "parent id"  type = "qbyte">Parent object id</argument>
    //
    <action>
    ipr_hash_t
        *hash;                          //  Entry into hash table
    icl_shortstr_t
        object_key;                     //  Stringified object id
    amq_console_entry_t
        *entry;

    sprintf (object_key, "%X", object_id);
    hash = ipr_hash_new (self->object_store, object_key, NULL);
    assert (hash);

    entry = icl_mem_alloc (sizeof (amq_console_entry_t));
    entry->object_ref = object_ref;
    entry->class_ref  = class_ref;
    entry->hash       = hash;           //  Holds link to hash
    hash->data        = entry;
    </action>
</method>

<method name = "cancel" template = "async function" async = "1">
    <doc>
    Accepts an object cancellation request.
    </doc>
    <argument name = "object id" type = "qbyte">Object ID</argument>
    <action>
    amq_console_entry_t
        *entry;

    entry = s_lookup_object (self, object_id);
    if (entry) {
        ipr_hash_destroy (&entry->hash);
        icl_mem_free (entry);
    }
    </action>
</method>

<method name = "accept" template = "async function" async = "1">
    <doc>
    Accepts an AMQ Console message, which must be formatted in CML according
    to the AMQ Console specifications.
    </doc>
    <argument name = "content" type = "amq_content_basic_t *">The message content</argument>
    <possess>
    content = amq_content_basic_link (content);
    </possess>
    <release>
    amq_content_basic_unlink (&content);
    </release>
    <action>
    //
    ipr_bucket_t
        *bucket = NULL;                 //  Message comes here
    ezxml_t
        xml_root,
        xml_item;

    //  Get content body into a bucket
    bucket = ipr_bucket_new (IPR_BUCKET_MAX_SIZE);
    bucket->cur_size = amq_content_basic_get_body (content, bucket->data, bucket->max_size);
    assert (bucket->cur_size < IPR_BUCKET_MAX_SIZE);
    bucket->data [bucket->cur_size] = 0;
    
    if (amq_server_config_debug_console (amq_server_config))
        asl_log_print (amq_broker->debug_log, "C: accept  xml=%s", bucket->data

    //  Parse as XML message
    xml_root = ezxml_parse_str (bucket->data, bucket->cur_size);
    if (streq (ezxml_name (xml_root), "cml")) {
        xml_item = xml_root->child;
        if (streq (ezxml_name (xml_item), "schema-request"))
            s_execute_schema (content);
        else
        if (streq (ezxml_name (xml_item), "inspect-request"))
            s_execute_inspect (self, content, xml_item);
        else
        if (streq (ezxml_name (xml_item), "modify-request"))
            s_execute_modify (self, content, xml_item);
        else
        if (streq (ezxml_name (xml_item), "monitor-request"))
            s_execute_monitor (self, content, xml_item);
        else
        if (streq (ezxml_name (xml_item), "method-request"))
            s_execute_method (self, content, xml_item);
        else
            s_invalid_cml (content, bucket, "unknown CML command");
    }
    else
        s_invalid_cml (content, bucket, "Not a valid CML document");

    ipr_bucket_unlink (&bucket);
    ezxml_free (xml_root);
    </action>
</method>

<method name = "reply ok" template = "async function" async = "1">
    <doc>
    Generic reply to a request.  If the fields argument is not
    null, the fields are returned to the client.
    </doc>
    <argument name = "name"      type = "char *">Reply name</argument>
    <argument name = "request"   type = "amq_content_basic_t *">Original request</argument>
    <argument name = "object id" type = "qbyte">Object id</argument>
    <argument name = "fields"    type = "asl_field_list_t *">Object fields</argument>
    <argument name = "notice"    type = "char *">Reply notice, if any</argument>
    <possess>
    name = icl_mem_strdup (name);
    request = amq_content_basic_link (request);
    fields = asl_field_list_link (fields);
    notice = icl_mem_strdup (notice);
    </possess>
    <release>
    icl_mem_free (name);
    amq_content_basic_unlink (&request);
    asl_field_list_unlink (&fields);
    icl_mem_free (notice);
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
    amq_console_entry_t
        *entry;                         //  Object store entry

    entry = s_lookup_object (self, object_id);

    cml_item = ipr_xml_new (NULL, "cml", NULL);
    ipr_xml_attr_set (cml_item, "version", "1.0");
    ipr_xml_attr_set (cml_item, "xmlns", "http://www.openamq.org/schema/cml");

    cur_item = ipr_xml_new (cml_item, name, NULL);
    ipr_xml_attr_set (cur_item, "class",  entry->class_ref->name);
    ipr_xml_attr_set (cur_item, "object", icl_shortstr_fmt (strvalue, "%d", object_id));
    ipr_xml_attr_set (cur_item, "status", "ok");
    if (notice)
        ipr_xml_attr_set (cur_item, "notice", notice);

    if (fields) {
        field = asl_field_list_first (fields);
        while (field) {
            sub_item = ipr_xml_new (cur_item, "field", NULL);
            ipr_xml_attr_set (sub_item, "name", field->name);
            val_item = ipr_xml_new (sub_item, NULL, asl_field_string (field));
            ipr_xml_unlink (&val_item);
            ipr_xml_unlink (&sub_item);
            field = asl_field_list_next (&field);
        }
    }
    s_reply_xml (request, cml_item);
    ipr_xml_unlink  (&cur_item);
    ipr_xml_destroy (&cml_item);
    </action>
</method>

<method name = "reply error" template = "async function" async = "1">
    <doc>
    Generic error reply to a request.
    </doc>
    <argument name = "name"      type = "char *">Reply name</argument>
    <argument name = "status"    type = "char *">Object id</argument>
    <argument name = "request"   type = "amq_content_basic_t *">Original request</argument>
    <argument name = "object id" type = "qbyte">Object id</argument>
    <possess>
    name = icl_mem_strdup (name);
    status = icl_mem_strdup (status);
    request = amq_content_basic_link (request);
    </possess>
    <release>
    icl_mem_free (name);
    icl_mem_free (status);
    amq_content_basic_unlink (&request);
    </release>
    <action>
    ipr_xml_t
        *cml_item,                      //  CML item
        *cur_item;                      //  Top level object
    icl_shortstr_t
        strvalue;                       //  Stringified numeric value
    amq_console_entry_t
        *entry;                         //  Object store entry

    entry = s_lookup_object (self, object_id);
    cml_item = ipr_xml_new (NULL, "cml", NULL);
    ipr_xml_attr_set (cml_item, "version", "1.0");
    ipr_xml_attr_set (cml_item, "xmlns", "http://www.openamq.org/schema/cml");

    cur_item = ipr_xml_new (cml_item, name, NULL);
    ipr_xml_attr_set (cur_item, "class",  entry->class_ref->name);
    ipr_xml_attr_set (cur_item, "object", icl_shortstr_fmt (strvalue, "%d", object_id));
    ipr_xml_attr_set (cur_item, "status", status);

    s_reply_xml (request, cml_item);
    ipr_xml_unlink  (&cur_item);
    ipr_xml_destroy (&cml_item);
    </action>
</method>

<private name = "async header">
//  This file is generated when we build the project
#define AMQ_CONSOLE_SCHEMA      "amq_console_schema.cml"

static amq_console_entry_t *
    s_lookup_object   (amq_console_t *self, qbyte object_id);
static void
    s_execute_schema  (amq_content_basic_t *request);
static void
    s_execute_inspect (amq_console_t *self, amq_content_basic_t *request, ezxml_t xml_item);
static void
    s_execute_modify  (amq_console_t *self, amq_content_basic_t *request, ezxml_t xml_item);
static void
    s_execute_monitor (amq_console_t *self, amq_content_basic_t *request, ezxml_t xml_item);
static void
    s_execute_method  (amq_console_t *self, amq_content_basic_t *request, ezxml_t xml_item);
static asl_field_list_t *
    s_get_field_list  (ezxml_t xml_item);
static void
    s_invalid_cml     (amq_content_basic_t *request, ipr_bucket_t *bucket, char *error);
static void
    s_reply_error     (amq_content_basic_t *request, char *top, char *status);
static void
    s_reply_xml       (amq_content_basic_t *request, ipr_xml_t *xml_item);
static void
    s_reply_bucket    (amq_content_basic_t *request, ipr_bucket_t *bucket);
</private>

<private name = "async footer">
static amq_console_entry_t *
s_lookup_object (amq_console_t *self, qbyte object_id)
{
    icl_shortstr_t
        object_key;                     //  Stringified object id
    amq_console_entry_t
        *entry = NULL;
    ipr_hash_t
        *hash;                          //  Entry into hash table

    sprintf (object_key, "%X", object_id);
    hash = ipr_hash_table_search (self->object_store, object_key);
    if (hash) {
        entry = hash->data;
        ipr_hash_unlink (&hash);
    }
    return (entry);
}

static void
s_execute_schema (amq_content_basic_t *request)
{
    ipr_bucket_t
        *bucket;                        //  Schema loaded from disk
    icl_shortstr_t
        schema_file;                    //  Full name of schema file

    if (ipr_file_where (AMQ_CONSOLE_SCHEMA, "PATH", schema_file) == 0) {
        bucket = ipr_file_slurp (schema_file);
        if (bucket) {
            s_reply_bucket (request, bucket);
            ipr_bucket_unlink (&bucket);
        }
        else {
            asl_log_print (amq_broker->alert_log, "E: can't read '%s'", schema_file);
            s_reply_error (request, "schema-reply", "notfound");
        }
    }
    else {
        asl_log_print (amq_broker->alert_log, "E: can't find '%s'", schema_file);
        s_reply_error (request, "schema-reply", "notfound");
    }
}

static void
s_execute_inspect (
    amq_console_t *self,
    amq_content_basic_t *request,
    ezxml_t xml_item)
{
    const char
        *object_str;
    amq_console_entry_t
        *entry;                         //  Object store entry

    object_str = ezxml_attr (xml_item, "object");
    if (object_str) {
        entry = s_lookup_object (self, atol (object_str));
        if (entry)
            entry->class_ref->inspect (entry->object_ref, request);
        else {
            asl_log_print (amq_broker->alert_log, "E: no such object found (ID=%s)", object_str);
            s_reply_error (request, "inspect-reply", "notfound");
        }
    }
    else {
        asl_log_print (amq_broker->alert_log, "E: badly-formatted CML method, no object ID");
        s_reply_error (request, "inspect-reply", "invalid");
    }
}

static void
s_execute_modify (
    amq_console_t *self,
    amq_content_basic_t *request,
    ezxml_t xml_item)
{
    const char
        *object_str;
    asl_field_list_t
        *fields;                        //  Properties to set
    amq_console_entry_t
        *entry;                         //  Object store entry

    object_str = ezxml_attr (xml_item, "object");
    if (object_str) {
        entry = s_lookup_object (self, atol (object_str));
        if (entry) {
            fields = s_get_field_list (xml_item);
            entry->class_ref->modify (entry->object_ref, request, fields);
            asl_field_list_unlink (&fields);
        }
        else {
            asl_log_print (amq_broker->alert_log, "E: no such object found (ID=%s)", object_str);
            s_reply_error (request, "modify-reply", "notfound");
        }
    }
    else {
        asl_log_print (amq_broker->alert_log, "E: badly-formatted CML method, no object ID");
        s_reply_error (request, "modify-reply", "invalid");
    }
}


static void
s_execute_monitor (
    amq_console_t *self,
    amq_content_basic_t *request,
    ezxml_t xml_item)
{
    asl_log_print (amq_broker->debug_log, "amq_console: monitor");
}


static void
s_execute_method (
    amq_console_t *self,
    amq_content_basic_t *request,
    ezxml_t xml_item)
{
    const char
        *object_str,
        *method_name;                   //  Method to invoke
    asl_field_list_t
        *fields;                        //  Properties to pass
    amq_console_entry_t
        *entry;                         //  Object store entry

    object_str  = ezxml_attr (xml_item, "object");
    method_name = ezxml_attr (xml_item, "name");
    if (object_str && method_name) {
        entry = s_lookup_object (self, atol (object_str));
        if (entry) {
            fields = s_get_field_list (xml_item);
            entry->class_ref->method (
                entry->object_ref, (char *) method_name, request, fields);
            asl_field_list_unlink (&fields);
        }
        else {
            asl_log_print (amq_broker->alert_log, "E: no such object found (ID=%s)", object_str);
            s_reply_error (request, "modify-reply", "notfound");
        }
    }
    else {
        asl_log_print (amq_broker->alert_log, "E: badly-formatted CML method, no object ID");
        s_reply_error (request, "modify-reply", "invalid");
    }
}

//  Builds field list from body of XML command
//  Caller must destroy field list when finished

static asl_field_list_t *
s_get_field_list (ezxml_t xml_item)
{
    asl_field_list_t
        *fields;
    const char
        *field_name,
        *field_value;
    ezxml_t
        xml_field;

    fields = asl_field_list_new (NULL);
    xml_field = xml_item->child;
    while (xml_field) {
        field_name  = ezxml_attr (xml_field, "name");
        field_value = ezxml_txt  (xml_field);
        if (field_name)
            asl_field_new_string (fields, (char *) field_name, (char *) field_value);
        xml_field = xml_field->next;
    }
    return (fields);
}

static void
s_invalid_cml (amq_content_basic_t *request, ipr_bucket_t *bucket, char *error)
{
    asl_log_print (amq_broker->alert_log,
        "W: amq.console: content body is not valid CML: %s", error);
    ipr_bucket_dump (bucket, "I: ");
    s_reply_error (request, "invalid", NULL);
}

static void
s_reply_error (amq_content_basic_t *request, char *top, char *status)
{
    ipr_xml_t
        *cml_item,
        *cur_item;

    cml_item = ipr_xml_new (NULL, "cml", NULL);
    ipr_xml_attr_set (cml_item, "version", "1.0");
    ipr_xml_attr_set (cml_item, "xmlns", "http://www.openamq.org/schema/cml");

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
s_reply_xml (amq_content_basic_t *request, ipr_xml_t *xml_item)
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
    ipr_bucket_unlink (&bucket);
}


//  Send ipr bucket reply back to original requestor, using same
//  message id and using original reply-to as routing key.
//
static void
s_reply_bucket (amq_content_basic_t *request, ipr_bucket_t *bucket)
{
    amq_client_method_t
        *method;                        //  Basic.Publish method
    amq_exchange_t
        *exchange;                      //  We send the reply to amq.direct
    amq_vhost_t
        *vhost;

    vhost = amq_vhost_link (amq_broker->vhost);
    if (vhost) {
        if (*request->reply_to) {
            exchange = amq_exchange_table_search (vhost->exchange_table, "amq.direct");
            assert (exchange);

            //  Create a Basic.Publish method to carry the content
            method = amq_client_method_new_basic_publish (
                0, "amq.direct", request->reply_to, FALSE, FALSE);

            //  Create a content with our desired reply data
            method->content = amq_content_basic_new ();
            amq_content_basic_set_message_id   (method->content, request->message_id);
            amq_content_basic_set_content_type (method->content, "text/xml");
            amq_content_basic_record_body      (method->content, bucket);
            amq_content_basic_set_routing_key  (method->content, "amq.direct", request->reply_to, 0);

            //  Publish the message
            amq_exchange_publish (exchange, NULL, (amq_server_method_t *) method, FALSE);
            amq_client_method_unlink (&method);

            amq_exchange_unlink (&exchange);
        }
        else
            asl_log_print (amq_broker->alert_log,
                "E: amq.console: client did not specify Reply-To queue");
    }
    amq_vhost_unlink (&vhost);
}
</private>

<method name = "selftest">
    smt_initialise ();
    amq_console = amq_console_new ();
    amq_console_destroy (&amq_console);
    smt_wait (0);
</method>

</class>
