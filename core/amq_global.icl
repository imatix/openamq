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

extern amq_vhost_table_t
    *amq_vhosts;                        /*  Virtual hosts table              */
extern ipr_config_table_t
    *amq_config;                        /*  Server configuration table       */
</public>

<private>
amq_vhost_table_t
    *amq_vhosts = NULL;                 /*  Virtual hosts table              */
ipr_config_table_t
    *amq_config = NULL;                 /*  Server configuration table       */
</private>

</class>

