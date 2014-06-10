<?xml?>
<class
    name      = "demo_server_channel_table"
    comment   = "Implements the hash table container for demo_server_channel"
    version   = "1.0"
    script    = "icl_gen"
    >
<doc>
    This class implements the hash table container for demo_server_channel
</doc>

<inherit class = "icl_hash_head" >
    <option name = "links"      value = "1" />
    <option name = "prefix"     value = "table"/>
    <option name = "childname"  value = "demo_server_channel" />
    <option name = "childtype"  value = "demo_server_channel_t" />
    <option name = "count"      value = "1" />
    <option name = "link_to_child" value = "1"/>
    <option name = "initial_size"  value = "255"/>
    <option name = "hash_type"  value = "int"/>
</inherit>
<option name = "rwlock" value = "1" />
<option name = "alloc"  value = "cache" />

<import class = "demo_server_channel" after = "1" />


<context>
    <!-- add properties for the container -->
</context>

<method name = "new">
    <!-- any initialisation code you need -->
</method>

<method name = "destroy">
    <!-- any destruction code you need -->
</method>

<method name = "selftest">
    <!-- Add your selftest code -->
    <local>
    </local>
</method>
</class>
