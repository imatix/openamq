<?xml?>
<class
    name      = "smt_signal_by_thread"
    comment   = "Implements the list container for smt_signal"
    version   = "1.0"
    script    = "icl_gen"
    >
<doc>
    This class implements the list container for smt_signal
</doc>

<option name = "links"      value = "0" />

<inherit class = "icl_list_head" >
    <option name = "prefix"     value = "by_thread"/>
    <option name = "childname"  value = "smt_signal" />
    <option name = "childtype"  value = "smt_signal_t" />
    <option name = "count"      value = "0" />
    <option name = "child_has_links"    value = "0"/>
    <option name = "link_to_child"      value = "0"/>
</inherit>
<option name = "rwlock" value = "1" />
<option name = "mutex"  value = "0" />
<option name = "alloc"  value = "cache" />

<import class = "smt_signal" />

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
