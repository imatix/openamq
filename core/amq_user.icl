<?xml?>
<class
    name      = "amq_user"
    comment   = "Authorised user class"
    version   = "1.0"
    copyright = "Copyright (c) 2004-2005 JPMorgan"
    script    = "icl_gen"
    >

<inherit class = "ipr_hash_str" />
<option name = "bigtable" value = "1" />

<import class = "ipr_classes" />
<import class = "amq_global"  />

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
    ASSERT (config);

    /*  Initialise other properties                                          */
    ipr_shortstr_cpy (self->password, ipr_config_attr (config, "password", ""));
</method>

<method name = "destroy">
</method>

<method name = "selftest" />

</class>
