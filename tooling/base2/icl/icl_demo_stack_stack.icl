<?xml?>
<class
    name      = "icl_demo_stack_stack"
    comment   = "Implements the list container for icl_demo_stack"
    version   = "1.0"
    script    = "icl_gen"
    >
<doc>
    This class implements the list container for icl_demo_stack
</doc>

<inherit class = "icl_stack_head" >
    <option name = "links"      value = "1" />
    <option name = "prefix"     value = "stack"/>
    <option name = "childname"  value = "icl_demo_stack" />
    <option name = "childtype"  value = "icl_demo_stack_t" />
    <option name = "count"      value = "1" />
    <option name = "link_to_child" value = "1"/>
</inherit>
<option name = "rwlock"    value = "0" />
<option name = "alloc"     value = "cache" />

<import class = "icl_demo_stack" />

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
