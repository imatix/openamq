<?xml?>
<class
    name      = "mxr_messagelist"
    comment   = "Matcher message list structure"
    version   = "1.0a0"
    script    = "icl_gen"
    >

<!-- $Id$ -->

<import  class = "ipr_bits" />
<inherit class = "mxr_slist" />
<doc>
    This class is an implementation of list structures that are used
    to store messages in the matcher.
</doc>

<public>
</public>

<context>
    char       *message;
    ipr_bits_t *criterias;
</context>

<private>

</private>

<!--  Object allocator  ------------------------------------------------------>

<method name = "new">
    <doc>
        Return a new messagelist item.
    </doc>
    <argument name = "message" type = "char *" />

    /* Element has already been allocated, now we just set it to 0           */
    if (message)
        self->message = strdup (message);
    else
        self->message = NULL;

    self->criterias = NULL;
</method>

<!--  Object destructor  ----------------------------------------------------->

<method name = "destroy">
    <doc>
        Free everything in the list item.
    </doc>

    if (self->message) {
        mem_free (self->message);
        self->message = NULL;
    }

    if (self->criterias) {
        mem_free (self->criterias);
        self->criterias = NULL;
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
