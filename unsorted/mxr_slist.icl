<?xml?>
<class
    name      = "mxr_slist"
    comment   = "Implements a singly linked list"
    version   = "1.0b0"
    copyright = "Copyright (c) 2004 iMatix Corporation"
    script    = "icl_gen"
    >
<doc>
    Base class for singly-linked list objects.
</doc>

<inherit class = "icl_base"  />

<context>
    $(selftype) *next;
</context>


<!--  Object allocator  ------------------------------------------------------>

<method name = "new" return = "self">
    <declare name = "self" type = "$(selftype) *" default = "NULL" />

    self = ($(selftype) *) mem_alloc (sizeof ($(selftype)));
    self->next = NULL;
</method>

<!--  Object destructor  ----------------------------------------------------->

<method name = "destroy" export = "after">
    <doc>
    Simply frees itself.  It is up to the parent to to the unlinking.
    </doc>
    <argument name = "self" type = "$(selftype) *" />

    if (self) {
        self->next = NULL;
        mem_free (self);
        self = NULL;
    }
</method>

<method name = "destroy_p">
    <doc>
    Destroys a $(selfname) object calling the destroy method, and sets the
    supplied reference to null.
    </doc>
    <argument name = "self" type = "$(selftype) **" />

    if (self) {
        $(selfbase)_destroy (*self);
        *self = NULL;
    }
</method>

<!--  Class support methods  ------------------------------------------------>

<method name = "selftest">
    <local>
    </local>

</method>

</class>
