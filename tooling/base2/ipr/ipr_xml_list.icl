<?xml?>
<class
    name      = "ipr_xml_list"
    comment   = "Implements the list container for ipr_xml"
    version   = "1.0"
    script    = "icl_gen"
    >
<doc>
    This class implements the list container for ipr_xml
</doc>

<option name = "links"      value = "1" />

<inherit class = "icl_list_head" >
    <option name = "prefix"     value = "list"/>
    <option name = "childname"  value = "ipr_xml" />
    <option name = "childtype"  value = "ipr_xml_t" />
    <option name = "count"      value = "0" />
</inherit>
<option name = "rwlock" value = "1" />
<option name = "mutex"  value = "0" />
<option name = "alloc"  value = "cache" />

<import class = "ipr_xml" />

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
