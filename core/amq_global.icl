<?xml?>
<class
    name      = "amq_global"
    comment   = "Holds server-global variables"
    version   = "1.0"
    copyright = "Copyright (c) 2004-2005 JPMorgan and iMatix Corporation"
    script    = "icl_gen"
    >

<inherit class = "icl_base" />

<import class = "amq_classes" />

<public>
/*  These globals can be used anywhere in the OpenAMQ software               */

extern ipr_config_t
    *amq_config;                        /*  Server configuration table       */
extern amq_vhost_table_t
    *amq_vhosts;                        /*  Virtual hosts table              */
extern amq_user_table_t
    *amq_users;                         /*  Users for this vhost             */

extern size_t
    amq_max_memory;                     /*  Configured memory limit, if any  */
extern size_t
    amq_txn_limit;                      /*  Configured txn limit, if any     */

/*  Server security mechanisms                                               */

#define AMQ_MECHANISM_NONE          1
#define AMQ_MECHANISM_PLAIN         2
</public>

<private>
ipr_config_t
    *amq_config = NULL;                 /*  Server configuration table       */
amq_vhost_table_t
    *amq_vhosts = NULL;                 /*  Virtual hosts table              */
amq_user_table_t
    *amq_users = NULL;                  /*  Users for this vhost             */
size_t
    amq_max_memory = 0;                 /*  Configured memory limit, if any  */
size_t
    amq_txn_limit = 0;                  /*  Configured txn limit, if any     */

static int
    s_error_code = 0;                   /*  Last error code                  */
static char
    *s_error_text = "";                 /*  Last error text                  */
static int
    s_mechanism = AMQ_MECHANISM_NONE;   /*  Current security mechanism       */
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

<method name = "set mechanism">
    <argument name = "mechanism" type = "int" />
    s_mechanism = mechanism;
</method>

<method name = "mechanism" return = "mechanism">
    <declare name = "mechanism" type = "int" />
    mechanism = s_mechanism;
</method>

<method name = "mechanism_str" return = "mechanism">
    <declare name = "mechanism" type = "char *" />
    if (s_mechanism == AMQ_MECHANISM_PLAIN)
        mechanism = "PLAIN";
    else
        mechanism = "NONE";
</method>

<method name = "selftest">
</method>

</class>

