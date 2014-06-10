<?xml?>
<class
    name      = "smt_signal_table"
    comment   = "Implements the hash table container for smt_signal"
    version   = "1.0"
    script    = "icl_gen"
    >
<doc>
    This class implements the hash table container for smt_signal
</doc>

<inherit class = "icl_hash_head" >
    <option name = "links"      value = "0" />
    <option name = "prefix"     value = "table"/>
    <option name = "childname"  value = "smt_signal" />
    <option name = "childtype"  value = "smt_signal_t" />
    <option name = "count"      value = "1" />
    <option name = "link_to_child" value = "0"/>
    <option name = "initial_size"  value = "255"/>
    <option name = "hash_type"  value = "int"/>
</inherit>
<option name = "rwlock" value = "1" />
<option name = "alloc"  value = "cache" />

<import class = "smt_signal" after = "1" />


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
