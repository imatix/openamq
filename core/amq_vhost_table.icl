<?xml?>
<class
    name      = "amq_vhost_table"
    comment   = "Implements hash table container for amq_vhost"
    version   = "1.0"
    copyright = "Copyright (c) 2004-2005 JPMorgan"
    script    = "icl_gen"
    >
<doc>
    This class implements hash table container for amq_vhost
</doc>

<inherit class = "ipr_hash_table" />
<import  class = "amq_vhost"      />
<import  class = "amq_global"     />
<option name = "childname" value = "amq_vhost" />
<option name = "bigtable"  value = "0" />

<context>
    ipr_config_t
        *config;                        /*  Server config table              */
</context>

<method name = "new">
    <doc>
    Loads all virtual hosts defined in the main configuration table.  If the
    config table is null, does not create any virtual hosts.
    </doc>
    <argument name = "config" type = "ipr_config_t *">Server config table</argument>
    <local>
    ipr_config_t
        *vhost_config;
    char
        *directory,                     /*  Directory for virtual host       */
        *vhost_name;                    /*  Extracted vhost name             */
    </local>

    self->config = config;
    if (config) {
        ipr_config_locate (config, "/config/vhosts/vhost", NULL);
        while (config->located) {
            /*  Load vhost config file to get vhost name                         */
            directory = ipr_config_attr (config, "directory", NULL);
            if (directory) {
                vhost_config = ipr_config_new (directory, AMQ_VHOST_CONFIG);
                if (vhost_config) {
                    vhost_name = ipr_config_locattr (vhost_config, "/config/vhost", "name", NULL);
                    if (vhost_name)
                        amq_vhost_new (self, vhost_name, directory, vhost_config);
                    else
                        ipr_config_destroy (&vhost_config);
                }
                else
                    break;
            }
            ipr_config_next (config);
        }
    }
</method>

<method name = "selftest">
    <local>
    </local>
</method>

</class>
