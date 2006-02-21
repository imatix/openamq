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

<import class = "amq_server_classes"/>
<import class = "asl_field_list" />

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
    <argument name = "object id"  type = "qbyte">Object id</argument>
    <argument name = "object ref" type = "void *">Object reference</argument>
    <argument name = "class ref"  type = "amq_console_class_t *" />
    <argument name = "parent id"  type = "qbyte">Parent object id</argument>
    <action>
    if (object_id &lt; self->max_objects) {
        self->object_ref [object_id] = object_ref;
        self->class_ref  [object_id] = class_ref;
        self->parent_id  [object_id] = parent_id;
    }
    else {
        asl_log_print (amq_broker->alert_log,
            "E: Console object store is full, please restart server");
        asl_log_print (amq_broker->alert_log,
            "I: Current limitation will be removed in next release");
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

<method name = "accept" template = "async function" async = "1">
    <doc>
    Accepts an AMQ Console message, which must be formatted in CML according
    to the AMQ Console specifications.
    </doc>
    <argument name = "content" type = "amq_content_basic_t *">The message content</argument>
    <possess>
    amq_content_basic_link (content);
    </possess>
    <release>
    amq_content_basic_unlink (&content);
    </release>
    <action>
    //
    DESCR
        descr;                          //  XML string as descriptor
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
    amq_content_basic_link (request);
    asl_field_list_link (fields);
    notice = icl_mem_strdup (notice);
    </possess>
    <release>
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

    cml_item = ipr_xml_new (NULL, "cml", NULL);
    ipr_xml_attr_set (cml_item, "version", "1.0");
    ipr_xml_attr_set (cml_item, "xmlns", "http://www.openamq.org/schema/cml");

    cur_item = ipr_xml_new (cml_item, name, NULL);
    ipr_xml_attr_set (cur_item, "class",  self->class_ref [object_id]->name);
    ipr_xml_attr_set (cur_item, "object", icl_shortstr_fmt (strvalue, "%ld", object_id));
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
    amq_content_basic_link (request);
    </possess>
    <release>
    amq_content_basic_unlink (&request);
    </release>
    <action>
    ipr_xml_t
        *cml_item,                      //  CML item
        *cur_item;                      //  Top level object
    icl_shortstr_t
        strvalue;                       //  Stringified numeric value

    cml_item = ipr_xml_new (NULL, "cml", NULL);
    ipr_xml_attr_set (cml_item, "version", "1.0");
    ipr_xml_attr_set (cml_item, "xmlns", "http://www.openamq.org/schema/cml");

    cur_item = ipr_xml_new (cml_item, name, NULL);
    ipr_xml_attr_set (cur_item, "class",  self->class_ref [object_id]->name);
    ipr_xml_attr_set (cur_item, "object", icl_shortstr_fmt (strvalue, "%ld", object_id));
    ipr_xml_attr_set (cur_item, "status", status);

    s_reply_xml (request, cml_item);
    ipr_xml_unlink  (&cur_item);
    ipr_xml_destroy (&cml_item);
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
static asl_field_list_t *
    s_get_field_list  (ipr_xml_t *xml_command);
static void
    s_execute_monitor (amq_console_t *self, amq_content_basic_t *request, ipr_xml_t *xml_command);
static void
    s_execute_method  (amq_console_t *self, amq_content_basic_t *request, ipr_xml_t *xml_command);
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
            s_reply_bucket (request, bucket);
            ipr_bucket_destroy (&bucket);
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
    amq_content_basic_t *request,
    ipr_xml_t *xml_command)
{
    char
        *object_str;
    qbyte
        object_id;
    asl_field_list_t
        *fields;                        //  Properties to set

    object_str = ipr_xml_attr_get (xml_command, "object", "");
    if (*object_str) {
        object_id = atol (object_str);
        if (object_id < self->max_objects
        && self->object_ref [object_id]) {
            fields = s_get_field_list (xml_command);
            self->class_ref [object_id]->modify (
                self->object_ref [object_id], request, fields);
            asl_field_list_unlink (&fields);
        }
        else
            s_reply_error (request, "modify-reply", "notfound");
    }
    else
        s_reply_error (request, "modify-reply", "invalid");
}


//  Builds field list from body of XML command
//  Caller must destroy field list when finished

static asl_field_list_t *
s_get_field_list (ipr_xml_t *xml_command)
{
    asl_field_list_t
        *fields;
    ipr_xml_t
        *xml_field;
    char
        *field_value;

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
    return (fields);
}

static void
s_execute_monitor (
    amq_console_t *self,
    amq_content_basic_t *request,
    ipr_xml_t *xml_command)
{
    asl_log_print (amq_broker->debug_log, "amq_console: monitor");
}

static void
s_execute_method (
    amq_console_t *self,
    amq_content_basic_t *request,
    ipr_xml_t *xml_command)
{
    char
        *object_str,
        *method_name;                   //  Method to invoke
    qbyte
        object_id;
    asl_field_list_t
        *fields;                        //  Properties to pass

    object_str  = ipr_xml_attr_get (xml_command, "object", "");
    method_name = ipr_xml_attr_get (xml_command, "name", "");
    if (*object_str && *method_name) {
        object_id = atol (object_str);
        if (object_id < self->max_objects
        && self->object_ref [object_id]) {
            fields = s_get_field_list (xml_command);
            self->class_ref [object_id]->method (
                self->object_ref [object_id], method_name, request, fields);
            asl_field_list_unlink (&fields);
        }
        else
            s_reply_error (request, "method-reply", "notfound");
    }
    else
        s_reply_error (request, "method-reply", "invalid");
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
    ipr_bucket_destroy (&bucket);
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

    if (*request->reply_to) {
        exchange = amq_exchange_search (amq_vhost->exchange_table, "amq.direct");
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
        amq_exchange_publish (exchange, NULL, (amq_server_method_t *) method);
        amq_client_method_unlink (&method);

        amq_exchange_unlink (&exchange);
    }
    else
        asl_log_print (amq_broker->alert_log,
            "E: amq.console: client did not specify Reply-To queue");
}
</private>

<method name = "selftest">
    smt_initialise ();
    amq_console = amq_console_new ();
    amq_console_destroy (&amq_console);
</method>

</class>
