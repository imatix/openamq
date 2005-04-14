<?xml?>
<class
    name      = "amq_dest"
    comment   = "Destination base class"
    version   = "1.0"
    copyright = "Copyright (c) 2004-2005 JPMorgan and iMatix Corporation"
    script    = "icl_gen"
    abstract  = "1"
    >
<doc>
Extends the base amq_mesgq class with functionality to support a
destination.  This includes serialisation of the destination name to
the db_dest table, and serialisation of messages.
</doc>

<inherit class = "amq_mesgq"    />
<inherit class = "ipr_db_queue" />
<inherit class = "ipr_hash_str" />
<option name = "bigtable" value = "1" />

<import class = "amq_db" />

<!-- This is the structure of a serialised message  -->
<!-- not correctly inherited :-/
<data>
    <field name = "sender id"   type = "longint" >Original sender</field>
    <field name = "client id"   type = "longint" >Owned by this client</field>
    <field name = "header size" type = "longint" >Saved header size</field>
    <field name = "body size"   type = "longint" >Message body size</field>
    <field name = "priority"    type = "octet"   >Priority, 0-9</field>
    <field name = "expiration"  type = "longint" >Expiry date/time</field>
    <field name = "mime type"   type = "shortstr">Content MIME type</field>
    <field name = "encoding"    type = "shortstr">Content encoding</field>
    <field name = "identifier"  type = "shortstr">Message identifier</field>
    <field name = "headers"     type = "longstr" >Application headers</field>
    <field name = "content"     type = "longstr" >First body fragment</field>
    <field name = "store size"  type = "longint" >Stored file size, or 0</field>
</data>
-->

<context>
    amq_db_dest_t
        *db_dest;                       /*  Destination record               */
</context>

<method name = "new">
    <!-- Remove these inherited arguments from the API -->
    <dismiss argument = "db"        value = "vhost->db"  />
    <dismiss argument = "filename"  value = "key"        />
    assert (*key);

    /*  Assumes key, self->vhost in scope                                    */
    /*  Fetch existing dest if caller specified a name                       */
    self->db      = db;
    self->db_dest = amq_db_dest_new ();
    ipr_shortstr_cpy (self->db_dest->key, key);
//XXX    amq_db_dest_fetch_bykey (self->vhost->ddb, self->db_dest, AMQ_DB_FETCH_EQ);
    amq_db_dest_fetch_bykey (vhost->ddb, self->db_dest, AMQ_DB_FETCH_EQ);

    /*  If new queue, create it now                                          */
    if (self->db_dest->id == 0) {
        self_locate (self, self->db_dest->filename);
        self->db_dest->active = TRUE;
        amq_db_dest_insert (vhost->ddb, self->db_dest);
    }
    else {
        self->db_dest->active = TRUE;
        amq_db_dest_update (vhost->ddb, self->db_dest);
    }
</method>

<method name = "full search" return = "dest">
    <doc>
    Handles destination name pasting, whereby the destination name is
    built from a prefix specified at handle-open time, and a suffix
    specified at the individual command level.
    </doc>
    <argument name = "table"  type = "$(selfname)_table_t *">Table to search</argument>
    <argument name = "prefix" type = "char *"               >Dest prefix</argument>
    <argument name = "suffix" type = "char *"               >Dest suffix</argument>
    <argument name = "type"   type = "int"                  >Destination type</argument>
    <declare  name = "dest"   type = "$(selftype) *" default = "NULL"/>
    <local>
    ipr_shortstr_t
        pasted;
    </local>
    ipr_shortstr_fmt (pasted, "%s%s", prefix, suffix);
//    TODO: icl instability
//    dest = self_search (table, pasted, type);
    dest = self_search (type, table, pasted);
</method>

<method name = "search">
    <argument name = "service type" type = "int">Destination service type</argument>
    <local>
    ipr_shortstr_t
        internal;                       /*  Internal mesgq name              */
    </local>
    <header>
    /*  Before searching, set correct domain for destination name            */
    self_map_name (internal, key, service_type);
    key = internal;
    </header>
</method>

<method name = "destroy">
    if (self->temporary)
        amq_db_dest_delete (self->vhost->ddb, self->db_dest);
    amq_db_dest_destroy (&self->db_dest);
</method>

</class>
