<?xml?>
<class
    name      = "amq_vhost_table"
    comment   = "Implements the hash table container for amq_vhost"
    version   = "1.0"
    script    = "icl_gen"
    >
<doc>
Container class for the virtual hosts defined for the server, which
is currently limited to 255 virtual hosts.
</doc>

<inherit class = "ipr_hash_head" />
<import class = "amq_vhost" />
<option name = "childname"  value = "amq_vhost" />
<option name = "childtype"  value = "amq_vhost_t" />
<option name = "rwlock"     value = "1" />
<option name = "hash_size"  value = "255" />
<option name = "hash_type"  value = "str"/>

<import class = "ipr_shortstr"/>

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
    amq_vhost_t
        *vhost;
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
                vhost_config = ipr_config_new (directory, AMQ_VHOST_CONFIG, TRUE);
                ipr_config_load (vhost_config, directory, AMQ_CUSTOM_CONFIG, FALSE);
                if (vhost_config) {
                    vhost_name = ipr_config_locattr (vhost_config, "/config/vhost", "name", NULL);
                    if (vhost_name) {
                        vhost = amq_vhost_new (self, vhost_name, directory, vhost_config);
                        amq_vhost_unlink (&vhost);
                    }
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

<method name = "selftest"/>

</class>
