<?xml?>
<class
    name      = "amq_global"
    comment   = "Holds server-global variables"
    version   = "1.0"
    copyright = "Copyright (c) 2004-2005 JPMorgan"
    script    = "icl_gen"
    >

<inherit class = "icl_base"    />
<import  class = "amq_classes" />
<option  name = "selftest" value = "0" />

<public>
/*  These globals can be used anywhere in the OpenAMQ software               */

extern ipr_config_t
    *amq_config;                        /*  Server configuration table       */
extern amq_vhost_table_t
    *amq_vhosts;                        /*  Virtual hosts table              */
extern size_t
    amq_allowed_memory;                 /*  Configured memory limit, if any  */
</public>

<private>
ipr_config_t
    *amq_config = NULL;                 /*  Server configuration table       */
amq_vhost_table_t
    *amq_vhosts = NULL;                 /*  Virtual hosts table              */
size_t
    amq_allowed_memory = 0;             /*  Configured memory limit, if any  */

static int
    s_error_code = 0;                   /*  Last error code                  */
static char
    *s_error_text = "";                 /*  Last error text                  */
</private>

<method name = "set error">
    <argument name = "error code" type = "int" />
    <argument name = "error text" type = "char *" />
    s_error_code = error_code;
    s_error_text = error_text;
</method>

<method name = "error code" return = "error code">
    <declare name = "error code" type = "int" />
    error_code = s_error_code;
</method>

<method name = "error text" return = "error text">
    <declare name = "error text" type = "char *" />
    error_text = s_error_text;
</method>

<method name = "reset error">
    self_set_error (0, "No error");
</method>

</class>

