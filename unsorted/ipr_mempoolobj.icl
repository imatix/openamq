<?xml?>
<class
    name      = "ipr_mempoolobj"
    comment   = "iPR object class, used for dynamic objects"
    version   = "1.0a1"
    script    = "icl_gen"
    >

<inherit class = "icl_base"    />
<import  class = "ipr_mempool" />

<doc>
    Defines the basic interface for iPR mempooled objects.
</doc>

<context>
    void *pool;
</context>

<!--  Object allocator  ------------------------------------------------------>

<method name = "new" return = "self" >
    <declare name = "self" type = "$(selftype) *" default = "NULL" />
    <argument name = "pool" type = "void *">Pointer to Memory Pool</argument>

    /*
     * If pool is NULL, create_pool will return a new pool.  Else, a
     * subpool of the current pool is returned
     */
    pool = ipr_mempool_new (pool);
    if (pool != NULL) {
        self = ipr_mempool_alloc (pool, sizeof ($(selftype)));
        if (self != NULL) {
            self->pool = pool;
        }
    }
</method>

<!--  Object destructor  ----------------------------------------------------->

<method name = "destroy" export = "after">
    <argument name = "self" type = "$(selftype) *" />

    memset (self, 0, sizeof ($(selftype)));
</method>


<method name = "selftest" export = "none">
    <local>
        $(selftype) *item;
    </local>

    item = $(selfname)_new (NULL);
    $(selfname)_destroy (item);
</method>


<!-- Templates --------------------------------------------------------------->

<template name = "function" return = "rc" >
    <doc>
    Accepts a $(selftype) reference and returns zero in case of success,
    1 in case of errors.
    </doc>
    <argument name = "self" type = "$(selftype) *" />
    <declare  name = "rc"   type = "int"     default = "0" />
</template>

</class>
