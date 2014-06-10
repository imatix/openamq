<?xml?>
<class
    name      = "icl_mem_fat_list"
    comment   = "Implements the list container for icl_mem_fat"
    version   = "1.0"
    script    = "icl_gen"
    >
<doc>
    This class implements the list container for icl_mem_fat
</doc>

<option name = "links"      value = "0" />

<inherit class = "icl_list_head" >
    <option name = "prefix"     value = "list"/>
    <option name = "childname"  value = "icl_mem_fat" />
    <option name = "childtype"  value = "icl_mem_fat_t" />
    <option name = "count"      value = "1" />
    <option name = "child_has_links"    value = "0"/>
    <option name = "link_to_child"      value = "0"/>
</inherit>
<option name = "rwlock" value = "0" />
<option name = "mutex"  value = "1" />
<option name = "alloc"  value = "direct" />

<import class = "icl_mem_fat" />

<context>
    <!-- add properties for the container -->
</context>

<method name = "new">
    <!-- any initialisation code you need -->
</method>

<method name = "destroy">
    <!-- any destruction code you need, note that the list is
         automatically destroyed, you don't need to do this.
      -->
</method>

<method name = "selftest">
    <!-- Add your selftest code -->
    <local>
    </local>
</method>
</class>
