<?xml?>
<class
    name      = "ipr_mempool"
    comment   = "iPR memory pool class"
    version   = "1.0a1"
    script    = "icl_gen"
    >

<inherit class = "icl_base" />
<import  class = "ipr_mem"  />

<doc>
    Defines the basic interface for iPR memory pool objects.
</doc>

<context>
    size_t size;
    byte  *data;
</context>

<!--  Object allocator  ------------------------------------------------------>

<method name = "new" return = "pool" >
    <doc>
        Creates a new memory pool.  If 'pool' is not NULL then a subpool of
        that pool is returned.
    </doc>
    <argument name = "pool" type = "void *">Pointer to Memory Pool</argument>

    /*
     * For now, we don't use APR, and simply use malloc - subpooling
     * not available.
     */
    if (pool == NULL) {
        pool = ipr_mem_alloc (sizeof ($(selftype)));
        memset (pool, 0, sizeof ($(selftype)));
    }
</method>

<method name = "alloc" return = "ret" >
    <doc>
        Allocate 'size' bytes of memory from the 'pool' and return a pointer
        to it.
    </doc>
    <argument name = "pool" type = "void *">Pointer to Memory Pool</argument>
    <argument name = "size" type = "size_t">How much memory</argument>
    <declare  name = "ret" type = "void *" default = "NULL" />
    <local>
        $(selftype) *mypool = ($(selftype) *)pool;
    </local>

    /*
     * We simply add memory to the pool and return a pointer to that.
     */
    if (mypool != NULL) {
        mypool->data  = ipr_mem_realloc (mypool->data, mypool->size + size);
        ret           = mypool->data + mypool->size;
        mypool->size += size;
    }
</method>

<!--  Object destructor  ----------------------------------------------------->

<method name = "destroy" export = "after">
    <doc>
        Free everything in the pool.
    </doc>
    <argument name = "pool" type = "void *" />
    <local>
        $(selftype) *mypool = ($(selftype) *)pool;
    </local>

    if (mypool != NULL) {
        if (mypool->data != NULL) {
            ipr_mem_free (mypool->data);
        }
        memset (mypool, 0, sizeof ($(selftype)));
    }
</method>


<!-- Testing ----------------------------------------------------------------->

<method name = "selftest" export = "none">
    <local>
        $(selftype) *pool;
        char        *buffer;
    </local>

    pool   = $(selfname)_new (NULL);
    buffer = $(selfname)_alloc (pool, 50);
    buffer = $(selfname)_alloc (pool, 100);
    $(selfname)_destroy (pool);
</method>


</class>
