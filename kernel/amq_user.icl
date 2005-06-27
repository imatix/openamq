<?xml?>
<class
    name      = "amq_user"
    comment   = "Authorised user class"
    version   = "1.0"
    copyright = "Copyright (c) 2004-2005 JPMorgan and iMatix Corporation"
    script    = "icl_gen"
    >

<inherit class = "icl_object">
    <option name = "cache"  value = "1" />
    <option name = "rwlock" value = "1" />
</inherit>
<inherit class = "ipr_hash_item">
    <option name = "hash_type" value = "str" />
    <option name = "hash_size" value = "65535" />
</inherit>

<import class = "amq_global" />

<public name = "header">
#include "amq_core.h"
#include "amq_frames.h"
</public>

<context>
    /*  Object properties                                                    */
    ipr_shortstr_t
        password;                       /*  Clear-text password              */
</context>

<method name = "new">
    <argument name = "config" type = "ipr_config_t *">Configuration entry</argument>
    assert (config);

    /*  Initialise other properties                                          */
    ipr_shortstr_cpy (self->password, ipr_config_attr (config, "password", ""));
</method>

<method name = "selftest" />

</class>
