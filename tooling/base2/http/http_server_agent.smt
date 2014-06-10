<?xml?>
<!---------------------------------------------------------------------------
    http_server_agent.smt - http_server component

    This implements the HTTP server object, which is the top-level object
    holding server-wide values and owning server-wide structures.
    Generated from http_server.icl by smt_object_gen using GSL/4.

    
    Copyright (c) 1996-2009 iMatix Corporation
    
    This file is licensed under the GPL as follows:
    
    This program is free software; you can redistribute it and/or modify
    it under the terms of the GNU General Public License as published by
    the Free Software Foundation; either version 2 of the License, or (at
    your option) any later version.
    
    This program is distributed in the hope that it will be useful, but
    WITHOUT ANY WARRANTY; without even the implied warranty of
    MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
    General Public License for more details.
    
    For information on alternative licensing for OEMs, please contact
    iMatix Corporation.
 ----------------------------------------------------------------------------->

<agent
    name = "http_server_agent"
    script = "smt_gen.gsl"
    animate = "0"
    >
<import class = "http_server" />

<method name = "destroy">
    <argument name = "file" type = "char *" />
    <argument name = "line" type = "size_t" />
</method>

<method name = "start">
</method>

<method name = "ban address">
    <argument name = "address" type = "char *" />
    <possess>

    address = icl_mem_strdup (address);
    
    </possess>
    <release>

    icl_mem_free (address);
    
    </release>
</method>

<method name = "sync hashes">
</method>

<method name = "new password">
    <argument name = "portal" type = "http_access_module_t *" />
    <argument name = "key" type = "char *" />
    <argument name = "hash" type = "char *" />
    <argument name = "digest" type = "Bool" />
    <possess>

    
        key = icl_mem_strdup (key);
        hash = icl_mem_strdup (hash);
        
    
    </possess>
    <release>

    
        icl_mem_strfree (&key);
        icl_mem_strfree (&hash);
        
    
    </release>
</method>

<method name = "_http_access_module unbind">
    <argument name = "portal" type = "http_access_module_t *" />
</method>

<method name = "_http_driver_module unbind">
    <argument name = "portal" type = "http_driver_module_t *" />
</method>

<thread name = "class">
    <context>
        http_server_t
            *http_server;
    </context>
    
    <handler name = "thread new">
        <argument name = "self" type = "http_server_t *">The iCL object</argument>
        tcb->http_server = self;
    </handler>
        
    <state name = "main">

        <method name = "destroy"  nextstate = ""  >
            <action name = "destroy">

#define self tcb->http_server
char *
    file;
size_t
    line;

    file = destroy_m->file;
    line = destroy_m->line;
        {
{
http_driver_module_t
    *http_driver_module;            //  Portal object

if (icl_atomic_cas32 ((volatile qbyte *) &self->http_driver_module_stopped, TRUE, FALSE) == FALSE) {
    //  Destroy own portals
    while ((http_driver_module = (http_driver_module_t *) ipr_looseref_pop (self->http_driver_module_list))) {
        http_driver_module->client_looseref = NULL;
        http_driver_module_destroy (&http_driver_module);
    }
    ipr_looseref_list_destroy (&self->http_driver_module_list);
    //  Destroy own portal factories
    while ((http_driver_module = (http_driver_module_t *) ipr_looseref_pop (self->http_driver_module_factory_list))) {
        http_driver_module->client_looseref = NULL;
        http_driver_module_destroy (&http_driver_module);
    }
    ipr_looseref_list_destroy (&self->http_driver_module_factory_list);
}
}

{
http_access_module_t
    *http_access_module;            //  Portal object

if (icl_atomic_cas32 ((volatile qbyte *) &self->http_access_module_stopped, TRUE, FALSE) == FALSE) {
    //  Destroy own portals
    while ((http_access_module = (http_access_module_t *) ipr_looseref_pop (self->http_access_module_list))) {
        http_access_module->client_looseref = NULL;
        http_access_module_destroy (&http_access_module);
    }
    ipr_looseref_list_destroy (&self->http_access_module_list);
    //  Destroy own portal factories
    while ((http_access_module = (http_access_module_t *) ipr_looseref_pop (self->http_access_module_factory_list))) {
        http_access_module->client_looseref = NULL;
        http_access_module_destroy (&http_access_module);
    }
    ipr_looseref_list_destroy (&self->http_access_module_factory_list);
}
}

//  Stop child objects
ipr_meter_destroy (&self->imeter);
ipr_meter_destroy (&self->ometer);
smt_log_destroy (&self->access_log);
smt_log_destroy (&self->alert_log);
smt_log_destroy (&self->debug_log);
ipr_dict_table_destroy (&self->content_types);
ipr_dict_table_destroy (&self->blacklist);
ipr_password_table_destroy (&self->basic);
ipr_password_table_destroy (&self->digest);
if (self->stats)
    fclose (self->stats);
http_agent_term ();
        }
#undef self
            </action>
            <action name = "free">

#define self tcb->http_server
char *
    file;
size_t
    line;

    file = destroy_m->file;
    line = destroy_m->line;
        {
if (self->links == 0) {
    icl_console_print ("E: missing link on http_server object");
    http_server_show_ (self, ICL_CALLBACK_DUMP, stderr, file, line);
}
if (icl_atomic_dec32 ((volatile qbyte *) &self->links) == 0) {
    http_server_free (self);
    self = NULL;
}
        }
#undef self
            </action>
            //  Force release of method immediately
            smt_method_destroy (&thread->method);
        </method>


        <method name = "start"  >
            <action name = "start">

#define self tcb->http_server

        {
//  We will run the broker monitor once per second, unconditionally
smt_timer_request_delay (self->thread, 1000 * 1000, monitor_event);
        }
#undef self
            </action>
            //  Force release of method immediately
            smt_method_destroy (&thread->method);
        </method>


        <method name = "ban address"  >
            <action name = "ban address">

#define self tcb->http_server
char *
    address;

    address = ban_address_m->address;
        {
//  Insert into loaded blacklist table, append to file if new address
if (ipr_dict_assume (self->blacklist, address, NULL)) {
    //  Append to blacklist file
    FILE
        *file_stream = fopen (http_config_blacklist (http_config), "a");
    if (file_stream) {
        fprintf (file_stream, "%s\n", address);
        fclose (file_stream);
    }
}
        }
#undef self
            </action>
            //  Force release of method immediately
            smt_method_destroy (&thread->method);
        </method>


        <method name = "sync hashes"  >
            <action name = "sync hashes">

#define self tcb->http_server

        {
icl_shortstr_t
    time_string;                    //  Modification time of file, for display
char
    *filename;

//  Load/reload blacklist data
filename = http_config_blacklist (http_config);
if (ipr_dict_table_file_sync (&self->blacklist, filename, "(.+)(.*)") > 0)
    smt_log_print (self->alert_log,
        "I: blacklist file '%s' loaded (%s, %ld entries)", filename,
        ipr_time_mime (self->blacklist->file_time, time_string), self->blacklist->nbr_items);

//  Load/reload Basic authentication data and set policies if necessary
if (self->basic == NULL) {
    self->basic = ipr_password_table_new (http_config_basic_auth (http_config));
    ipr_password_table_guard_set (self->basic,
        http_config_hit_quota    (http_config),
        http_config_miss_quota   (http_config),
        http_config_password_ttl (http_config));
    if (self->basic->file_time)
        smt_log_print (self->alert_log,
            "I: basic auth file '%s' loaded (%s, %ld entries)", self->basic->file_name,
            ipr_time_mime (self->basic->file_time, time_string), self->basic->nbr_items);
    else
        smt_log_print (self->alert_log,
            "I: basic auth file '%s' not found", self->basic->file_name);
}
else
if (ipr_password_table_sync (self->basic))
    smt_log_print (self->alert_log,
        "I: basic auth file '%s' reloaded (%s, %ld entries)", self->basic->file_name,
        ipr_time_mime (self->basic->file_time, time_string), self->basic->nbr_items);

if (self->digest == NULL) {
    self->digest = ipr_password_table_new (http_config_digest_auth (http_config));
    ipr_password_table_guard_set (self->digest,
        http_config_hit_quota    (http_config),
        http_config_miss_quota   (http_config),
        http_config_password_ttl (http_config));
    if (self->digest->file_time)
        smt_log_print (self->alert_log,
            "I: digest auth file '%s' loaded (%s, %ld entries)", self->digest->file_name,
            ipr_time_mime (self->digest->file_time, time_string), self->digest->nbr_items);
    else
        smt_log_print (self->alert_log,
            "I: digest auth file '%s' not found", self->digest->file_name);

    ipr_password_table_set_nonce_ttl   (self->digest, http_config_nonce_ttl (http_config));
    ipr_password_table_set_nonce_quota (self->digest, http_config_nonce_quota (http_config));
}
else
if (ipr_password_table_sync (self->digest))
    smt_log_print (self->alert_log,
        "I: digest auth file '%s' reloaded (%s, %ld entries)", self->digest->file_name,
        ipr_time_mime (self->digest->file_time, time_string), self->digest->nbr_items);
        }
#undef self
            </action>
            //  Force release of method immediately
            smt_method_destroy (&thread->method);
        </method>


        <method name = "new password"  >
            <action name = "new password">

#define self tcb->http_server
http_access_module_t *
    portal;
char *
    key;
char *
    hash;
Bool
    digest;

    portal = new_password_m->portal;
    key = new_password_m->key;
    hash = new_password_m->hash;
    digest = new_password_m->digest;
        {
ipr_password_table_t
    *table;

table = digest? self->digest: self->basic;
ipr_password_assume (table, key, hash);
ipr_password_table_save (table);
smt_log_print (self->alert_log,
    "I: %s auth file '%s' saved (%ld entries)",
    digest? "digest": "basic", table->file_name, table->nbr_items);
        }
#undef self
            </action>
            //  Force release of method immediately
            smt_method_destroy (&thread->method);
        </method>


        <method name = "_http_access_module unbind"  >
            <action name = "_http_access_module unbind">

#define self tcb->http_server
http_access_module_t *
    portal;

    portal = _http_access_module_unbind_m->portal;
        {
if (portal->client_looseref) {
    ipr_looseref_destroy (&portal->client_looseref);
    http_access_module_unlink (&portal);
}
        }
#undef self
            </action>
            //  Force release of method immediately
            smt_method_destroy (&thread->method);
        </method>


        <method name = "_http_driver_module unbind"  >
            <action name = "_http_driver_module unbind">

#define self tcb->http_server
http_driver_module_t *
    portal;

    portal = _http_driver_module_unbind_m->portal;
        {
if (portal->client_looseref) {
    ipr_looseref_destroy (&portal->client_looseref);
    http_driver_module_unlink (&portal);
}
        }
#undef self
            </action>
            //  Force release of method immediately
            smt_method_destroy (&thread->method);
        </method>

        <event name = "monitor"  >
            <action name = "monitor">
            
#define self tcb->http_server

        {
//  Security timer minimum is 1 so we're always GT 0 here
self->security_timer--;
if (self->security_timer == 0) {
    self->security_timer = http_config_nervosity (http_config);
    http_server_sync_hashes (self);
    if (self->password_guard) {
        self->password_guard -= http_config_nervosity (http_config);
        if (self->password_guard <= 0) {
            self->password_guard = http_config_password_guard (http_config);
            ipr_password_table_guard_sync (self->basic);
            ipr_password_table_guard_sync (self->digest);
        }
    }
}
//  Cycle log files at midnight
if (ipr_time_now () > 235900) {
    if (!self->cycled) {
        self->cycled = TRUE;        //  Don't cycle again today
        smt_log_cycle (self->access_log);
        smt_log_cycle (self->debug_log);
        smt_log_cycle (self->alert_log);
    }
}
else
    self->cycled = FALSE;

if (self->monitor_timer) {
    self->monitor_timer--;
    if (self->monitor_timer == 0) {
        Bool
            new_stats = FALSE;      //  Do we have new stats to print?

        self->monitor_timer = http_config_monitor (http_config);
        if (ipr_meter_mark (self->imeter, http_config_monitor (http_config))) {
            smt_log_print (self->debug_log,
                "I: request  rate=%d mean=%d peak=%d",
                self->imeter->current,
                self->imeter->average,
                self->imeter->maximum);
            new_stats = TRUE;
        }
        if (ipr_meter_mark (self->ometer, http_config_monitor (http_config))) {
            smt_log_print (self->debug_log,
                "I: response rate=%d mean=%d peak=%d iomean=%d",
                self->ometer->current,
                self->ometer->average,
                self->ometer->maximum,
                self->ometer->average + self->imeter->average);
            new_stats = TRUE;
        }
        smt_log_print (self->debug_log, "I: clients=%d requests=%d responses=%d",
                self->clients,
                self->requests,
                self->responses);

        //  Record statistics if necessary
        if (self->stats && new_stats) {
            fprintf (self->stats, "%zu\t%d\t%d\t%d\t%d\t%d\t%d\t%d\n",
                ipr_bucket_used () / 1024,
                self->imeter->current, self->ometer->current,
                self->imeter->average, self->ometer->average,
                self->clients, self->requests, self->responses);
        }
    }
}
smt_timer_request_delay (self->thread, 1000 * 1000, monitor_event);

if (self->auto_crash_timer) {
    if (--self->auto_crash_timer == 0) {
        smt_log_print (self->alert_log,
            "W: ************************  AUTO-CRASH  ************************");
        smt_log_print (self->alert_log,
            "W: server is now emulating a system crash, and will exit brutally.");
        exit (0);
    }
}
if (self->auto_block_timer) {
    if (--self->auto_block_timer == 0) {
        smt_log_print (self->alert_log,
            "W: ************************  AUTO-BLOCK  ************************");
        smt_log_print (self->alert_log,
            "W: server is now emulating a blockage, and will freeze for 5 minutes.");
        sleep (300);
    }
}
        }
        
#undef self
            </action>
        </event>
        <event name = "shutdown" nextstate = "shutting down" />
    </state>
    
    <state name = "shutting down">

        <method name = "destroy"  nextstate = ""  >
            <action name = "destroy">

#define self tcb->http_server
char *
    file;
size_t
    line;

    file = destroy_m->file;
    line = destroy_m->line;
        {
{
http_driver_module_t
    *http_driver_module;            //  Portal object

if (icl_atomic_cas32 ((volatile qbyte *) &self->http_driver_module_stopped, TRUE, FALSE) == FALSE) {
    //  Destroy own portals
    while ((http_driver_module = (http_driver_module_t *) ipr_looseref_pop (self->http_driver_module_list))) {
        http_driver_module->client_looseref = NULL;
        http_driver_module_destroy (&http_driver_module);
    }
    ipr_looseref_list_destroy (&self->http_driver_module_list);
    //  Destroy own portal factories
    while ((http_driver_module = (http_driver_module_t *) ipr_looseref_pop (self->http_driver_module_factory_list))) {
        http_driver_module->client_looseref = NULL;
        http_driver_module_destroy (&http_driver_module);
    }
    ipr_looseref_list_destroy (&self->http_driver_module_factory_list);
}
}

{
http_access_module_t
    *http_access_module;            //  Portal object

if (icl_atomic_cas32 ((volatile qbyte *) &self->http_access_module_stopped, TRUE, FALSE) == FALSE) {
    //  Destroy own portals
    while ((http_access_module = (http_access_module_t *) ipr_looseref_pop (self->http_access_module_list))) {
        http_access_module->client_looseref = NULL;
        http_access_module_destroy (&http_access_module);
    }
    ipr_looseref_list_destroy (&self->http_access_module_list);
    //  Destroy own portal factories
    while ((http_access_module = (http_access_module_t *) ipr_looseref_pop (self->http_access_module_factory_list))) {
        http_access_module->client_looseref = NULL;
        http_access_module_destroy (&http_access_module);
    }
    ipr_looseref_list_destroy (&self->http_access_module_factory_list);
}
}

//  Stop child objects
ipr_meter_destroy (&self->imeter);
ipr_meter_destroy (&self->ometer);
smt_log_destroy (&self->access_log);
smt_log_destroy (&self->alert_log);
smt_log_destroy (&self->debug_log);
ipr_dict_table_destroy (&self->content_types);
ipr_dict_table_destroy (&self->blacklist);
ipr_password_table_destroy (&self->basic);
ipr_password_table_destroy (&self->digest);
if (self->stats)
    fclose (self->stats);
http_agent_term ();
        }
#undef self
            </action>
            <action name = "free">

#define self tcb->http_server
char *
    file;
size_t
    line;

    file = destroy_m->file;
    line = destroy_m->line;
        {
if (self->links == 0) {
    icl_console_print ("E: missing link on http_server object");
    http_server_show_ (self, ICL_CALLBACK_DUMP, stderr, file, line);
}
if (icl_atomic_dec32 ((volatile qbyte *) &self->links) == 0) {
    http_server_free (self);
    self = NULL;
}
        }
#undef self
            </action>
            //  Force release of method immediately
            smt_method_destroy (&thread->method);
        </method>

        <method name = "start" />
        <method name = "ban address" />
        <method name = "sync hashes" />
        <method name = "new password" />
        <method name = "_http_access_module unbind" />
        <method name = "_http_driver_module unbind" />
        <event name = "monitor" />
    </state>
</thread>

</agent>
