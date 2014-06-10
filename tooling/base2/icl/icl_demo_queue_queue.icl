<?xml?>
<class
    name      = "icl_demo_queue_queue"
    comment   = "Implements the queue container for icl_demo_queue"
    version   = "1.0"
    script    = "icl_gen"
    >
<doc>
    This class implements the queue container for icl_demo_queue
</doc>

<inherit class = "icl_queue_head" >
    <option name = "links"      value = "0" />
    <option name = "prefix"     value = "queue"/>
    <option name = "childname"  value = "icl_demo_queue" />
    <option name = "childtype"  value = "icl_demo_queue_t" />
    <option name = "count"      value = "0" />
</inherit>
<option name = "alloc" value = "cache" />

<import class = "icl_demo_queue" />

<context>
    <!-- add properties for the container -->
</context>

<method name = "new">
    <!-- any initialisation code you need -->
</method>

<method name = "destroy">
    <!-- any destruction code you need, note that the queue is
         automatically destroyed, you don't need to do this.
      -->
</method>

<method name = "selftest">
    <!-- Add your selftest code -->
    <local>
    </local>
</method>
</class>
