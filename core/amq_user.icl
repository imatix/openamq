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

<import class = "amq_vhost"      />
<import class = "ipr_classes"    />

<public name = "header">
#include "amq_core.h"
#include "amq_frames.h"
</public>

<context>
    /*  References to parent objects                                         */
    amq_vhost_t
        *vhost;                         /*  Parent virtual host              */

    /*  Object properties                                                    */
    ipr_shortstr_t
        password;                       /*  Clear-text password              */
</context>

<method name = "new">
    <argument name = "vhost"   type = "amq_vhost_t *" >Parent virtual host</argument>
    <argument name = "config"  type = "ipr_config_t *">Configuration entry</argument>
    <dismiss argument = "table" value = "vhost->user_hash" />

    ASSERT (config);

    /*  De-normalise from parent object, for simplicity of use               */
    self->vhost = vhost;

    /*  Initialise other properties                                          */
    ipr_shortstr_cpy (self->password, ipr_config_attr (config, "password", ""));
</method>

<method name = "destroy">
</method>

<method name = "selftest" />

</class>
