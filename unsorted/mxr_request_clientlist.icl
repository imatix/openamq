<?xml?>
<class
    name      = "mxr_request_clientlist"
    comment   = "Matcher client list structure"
    version   = "1.0a0"
    script    = "icl_gen"
    >

<!-- $Id$ -->

<inherit class = "mxr_slist" />
<doc>
    This class is an implementation of list structures that are used
    to store clients in the matcher.
</doc>

<public>
</public>

<context>
    char *data;
</context>

<private>

</private>

<!--  Object allocator  ------------------------------------------------------>

<method name = "new">
    <doc>
        Return a new clientlist item.
    </doc>
    <argument name = "data" type = "char *" />

    /* Element has already been allocated, now we just set it to 0           */
    if (data)
        self->data = strdup (data);
    else
        self->data = NULL;

</method>

<!--  Object destructor  ----------------------------------------------------->

<method name = "destroy">
    <doc>
        Free everything in the list item.
    </doc>
    if (self->data) {
        mem_free (self->data);
        self->data = NULL;
    }
</method>

<!--  Class support methods  ------------------------------------------------>

<method name = "selftest" return = "rc" inherit = "0" >
    <doc>
    Self test
    </doc>
    <declare name = "rc" type = "int" />

    rc = 0;
</method>

</class>
