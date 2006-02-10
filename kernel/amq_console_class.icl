<?xml?>
<class
    name    = "amq_console_class"
    comment = "AMQ Console class descriptor"
    version = "1.0"
    script  = "icl_gen"
    >
<doc>
This class defines a class descriptor.
</doc>

<inherit class = "icl_object">
    <option name = "alloc" value = "cache" />
</inherit>

<import class = "amq_server_classes" />

<context>
    char
        *name;                          //  Name of class
    int (*inspect) (
        void
            *self,                      //  Object reference
        amq_content_basic_t
            *request                    //  Original request
    );
    int (*modify) (
        void
            *self,                      //  Object reference
        amq_content_basic_t
            *request,                   //  Original request
        asl_field_list_t
            *fields                     //  Fields to modify
    );
    int (*method) (
        void
            *self,                      //  Object reference
        char
            *method_name,               //  Method to invoke
        amq_content_basic_t
            *request,                   //  Original request
        asl_field_list_t
            *fields                     //  Arguments to method
    );
</context>

<method name = "selftest" />

</class>
