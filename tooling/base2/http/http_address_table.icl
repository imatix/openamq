<?xml?>
<class
    name      = "http_address_table"
    comment   = "Implements the hash table container for http_address"
    version   = "1.0"
    script    = "icl_gen"
    >
<doc>
    This class implements the hash table container for http_address
</doc>

<inherit class = "icl_hash_head" >
    <option name = "links"      value = "0" />
    <option name = "prefix"     value = "table"/>
    <option name = "childname"  value = "http_address" />
    <option name = "childtype"  value = "http_address_t" />
    <option name = "count"      value = "1" />
    <option name = "link_to_child" value = "0"/>
    <option name = "initial_size"  value = "255"/>
    <option name = "hash_type"  value = "str"/>
</inherit>
<option name = "rwlock" value = "1" />
<option name = "alloc"  value = "cache" />

<import class = "http_address" after = "1" />

<import class = "icl_shortstr"/>

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
