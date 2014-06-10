<?xml?>
<class
    name      = "demo_server_connection_list"
    comment   = "Implements the list container for demo_server_connection"
    version   = "1.0"
    script    = "icl_gen"
    >
<doc>
    This class implements the list container for demo_server_connection
</doc>

<option name = "links"      value = "1" />

<inherit class = "icl_list_head" >
    <option name = "prefix"     value = "list"/>
    <option name = "childname"  value = "demo_server_connection" />
    <option name = "childtype"  value = "demo_server_connection_t" />
    <option name = "count"      value = "0" />
</inherit>
<option name = "rwlock" value = "1" />
<option name = "mutex"  value = "0" />
<option name = "alloc"  value = "cache" />

<import class = "demo_server_connection" />

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
