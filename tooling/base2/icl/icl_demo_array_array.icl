<?xml?>
<class
    name      = "icl_demo_array_array"
    comment   = "Implements the array container for icl_demo_array"
    version   = "1.0"
    script    = "icl_gen"
    >
<doc>
    This class implements the array container for icl_demo_array
</doc>

<inherit class = "icl_array_head" >
    <option name = "links"      value = "1" />
    <option name = "prefix"     value = "array"/>
    <option name = "childname"  value = "icl_demo_array" />
    <option name = "childtype"  value = "icl_demo_array_t" />
    <option name = "count"      value = "1" />
    <option name = "link_to_child" value = "1"/>
</inherit>
<option name = "rwlock" value = "1" />
<option name = "alloc"  value = "cache" />

<import class = "icl_demo_array" />

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
