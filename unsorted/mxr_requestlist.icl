<?xml?>
<class
    name      = "mxr_requestlist"
    comment   = "Matcher request list structure"
    version   = "1.0a0"
    script    = "icl_gen"
    >

<!-- $Id$ -->

<import  class = "ipr_bits" />
<import  class = "mxr_request_clientlist" />
<inherit class = "mxr_slist" />
<doc>
    This class is an implementation of list structures that are used
    to store requests in the matcher.
</doc>

<public>
</public>

<context>
    mxr_request_clientlist_t *clients;
    ipr_bits_t               *criterias;
</context>

<private>

</private>

<!--  Object allocator  ------------------------------------------------------>

<method name = "new">
    <doc>
        Return a new requestlist item.
    </doc>
    <argument name = "request" type = "char *" />

    /* Element has already been allocated, now we just set it to 0           */
    self->clients   = NULL;
    self->criterias = NULL;
    if (request) {
        self->clients = mxr_request_clientlist_new (request);
    }
</method>

<!--  Object destructor  ----------------------------------------------------->

<method name = "destroy">
    <doc>
        Free everything in the list item.
    </doc>

    if (self->criterias) {
        mem_free (self->criterias);
        self->criterias = NULL;
    }

    //XXX this should get rid of the _whole_ list!
    mxr_request_clientlist_destroy (self->clients);
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
