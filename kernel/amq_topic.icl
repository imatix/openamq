<?xml?>
<class
    name      = "amq_topic"
    comment   = "Topic destination class"
    version   = "1.0"
    copyright = "Copyright (c) 2004-2005 JPMorgan and iMatix Corporation"
    script    = "icl_gen"
    >
<doc>
This class defines an AMQP topic destination.  Topics hold messages and
dispatch them to a set of subscriptions.  Both topics and subscriptions
are based on the mesgq base class: subscriptions hold message references
rather than actual messages.  Topics hold the 'real' messages.
</doc>

<inherit class = "amq_dest" />

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

<context>
    amq_smessage_list_t
        **message_list;                 /*  Pending non-persistent messages  */
</context>

<method name = "new">
    <dismiss argument = "table" value = "vhost->topic_hash" />
</method>

<method name = "destroy">
</method>

<method name = "cancel" template = "function">
</method>

<method name = "accept" template = "function">
</method>

<method name = "dispatch" template = "function">
</method>

<method name = "query" template = "function">
</method>

<method name = "browse" template = "function">
</method>

<private name = "header">
</private>

<private name = "footer">
</private>

<method name = "selftest" />

</class>
