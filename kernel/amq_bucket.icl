<?xml?>
<class
    name      = "amq_bucket"
    comment   = "Bucket buffer class"
    version   = "2.0"
    copyright = "Copyright (c) 2004-2005 JPMorgan and iMatix Corporation"
    script    = "icl_gen"
    >
<doc>
    Defines a bucket, being a buffer of AMQ_BUCKET_MAX bytes.  Buckets can
    be passed in messages and freed by the recipient.  Buckets are cached
    so that heavy new/destroy use is very cheap.  We use reference counting
    so that buckets can be freely passed between objects.
</doc>

<inherit class = "icl_ref_count" />
<inherit class = "icl_base"      />

<import class = "icl_system"  />
<import class = "icl_mem"     />

<!-- Get list management functions -->
<invoke script = "icl_list_lib" prefix = "cache">
.   my.base = selfname
.   my.type = selftype
.   my.next = "cache_next"
.   my.prev = "cache_prev"
</invoke>

<data>
    <size value = "256"  />
    <size value = "512"  />
    <size value = "1024" />
    <size value = "2048" />
    <size value = "4096" />
    <size value = "8192" />
    <size value = "16384"/>
    <size value = "32768"/>
</data>

<public>
#define AMQ_BUCKET_MAX_SIZE  32768
</public>

<private>
static const size_t
    bucket_size [] = {\
.for class->data.size
                       $(value)$(!last()??','?)\
.endfor
                     };

#define S_NUM_BUCKET_SIZES $(count (class->data.size))

static Bool
    s_class_active = FALSE;

static $(selftype) *
    s_$(selfname)_cache_active [S_NUM_BUCKET_SIZES] = {\
.for class->data.size
                       NULL$(!last()??','?)\
.endfor
                       };
static $(selftype) *
    s_$(selfname)_cache_unused [S_NUM_BUCKET_SIZES] = {\
.for class->data.size
                       NULL$(!last()??','?)\
.endfor
                       };
static size_t
    s_$(selfname)_cache_active_count [S_NUM_BUCKET_SIZES];
static size_t
    s_$(selfname)_cache_unused_count [S_NUM_BUCKET_SIZES];

/*  Low-level memory allocation/deallocation functions, all inlined          */

static inline $(selftype) *
    s_$(selfname)_alloc (int idx, char *file, size_t line);
static inline void
    s_$(selfname)_free ($(selftype) *item);

/*  Object allocation functions, all cached to reduce heap access            */

static inline $(selftype) *
    s_$(selfname)_cache_alloc (int idx, char *file, size_t line);
static inline void
    s_$(selfname)_cache_free ($(selftype) *self);
static inline void
    s_$(selfname)_cache_new (void);
static void
    s_$(selfname)_cache_purge (void);
static void
    s_$(selfname)_cache_destroy (void);
</private>

<context>
    $(selftype) *cache_prev;            /*  All objects are in a list        */
    $(selftype) *cache_next;            /*    managed by these pointers      */
    int    idx;
    size_t cur_size;
    size_t max_size;
    byte  *data;
</context>

<method name = "new" template = "constructor">
    <argument name = "size" type = "size_t">Required length of data</argument>
    <local>
    int
        chop_min = 0,
        chop_max = S_NUM_BUCKET_SIZES,
        chop_mid;
    </local>
    <header>
    /*  Use a binary chop to work out which cache to use.                    */
    while (chop_max > chop_min) {
        chop_mid = (chop_max + chop_min) / 2;
        if (size > bucket_size [chop_mid])
            chop_min = chop_mid + 1;
        else
            chop_max = chop_mid;
    }
    if (chop_min >= S_NUM_BUCKET_SIZES)
        chop_min = S_NUM_BUCKET_SIZES - 1;

    self = s_$(selfname)_alloc (chop_min, file, line);
    self->data     = (byte *) self + sizeof ($(selftype));
    self->idx      = chop_min;
    self->cur_size = 0;
    self->max_size = bucket_size [chop_min];
    </header>
</method>

<method name = "cache purge">
    s_$(selfname)_cache_purge ();
</method>

<method name = "cache destroy">
    s_$(selfname)_cache_destroy ();
</method>

<method name = "destroy" template = "destructor">
    s_$(selfname)_free (self);
</method>

<method name = "fill" template = "function">
    <doc>
    Fill the bucket with data from some place: the data is copied into
    the bucket, and the bucket cur_size is updated accordingly.  If
    the size of the data to copy is greater than the bucket size, we
    raise an assertion (this is considered a structural error).
    </doc>
    <argument name = "source" type = "void *">Address of data to copy from</argument>
    <argument name = "size"   type = "size_t">Amount of data to copy</argument>
    assert (size <= self->max_size);
    memcpy (self->data, source, size);
    self->cur_size = size;
</method>

<method name = "load" template = "function">
    <doc>
    Fill the bucket with data from a file: the data is copied into
    the bucket, and the bucket cur_size is updated accordingly.  If
    the size of the data to copy is greater than the bucket size, we
    raise an assertion (this is considered a structural error).  If the
    end of the file is reached before the bucket is full, no error is
    reported but the bucket cur_size will be less than the size argument.
    </doc>
    <argument name = "file" type = "FILE *">File pointer of open file to copy from</argument>
    <argument name = "size" type = "size_t">Amount of data to copy</argument>
    assert (size <= self->max_size);
    self->cur_size = fread (self->data, 1, size, file);
</method>

<method name = "save" template = "function">
    <doc>
    Saves the data in the bucket to a file.
    </doc>
    <argument name = "file" type = "FILE *">File pointer of open file to copy to</argument>
    fwrite (self->data, 1, self->cur_size, file);
</method>

<private name = "footer">
/*  Allocate a new object from cached heap memory                            */
static inline $(selftype) *
s_$(selfname)_alloc (int idx, char *file, size_t line)
{
    $(selftype)
        *item;

    /*  Initialise cache if necessary                                        */
    if (!s_class_active) {
        s_$(selfname)_cache_new ();
        s_class_active = TRUE;
    }

    /*  Take object off cache unused list if possible                        */
    if (s_$(selfname)_cache_unused[idx]->cache_next != s_$(selfname)_cache_unused[idx]) {
        item = s_$(selfname)_cache_unlink (s_$(selfname)_cache_unused[idx]->cache_next);
        s_$(selfname)_cache_unused_count[idx]--;
    }
    else
        item = s_$(selfname)_cache_alloc (idx, file, line);

    if (item) {
        /*  Attach object to active list                                     */
        s_$(selfname)_cache_attach (item, s_$(selfname)_cache_active[idx]);
        s_$(selfname)_cache_active_count[idx]++;
    }
    return (item);
}

/*  Return an object to cached heap memory                                   */
static inline void
s_$(selfname)_free ($(selftype) *item)
{
    int
        idx = item->idx;
        
    s_$(selfname)_cache_unlink (item);
    s_$(selfname)_cache_active_count[idx]--;
.   if (class->option (name = "nullify").value?1) = 1
    memset (item, 0, sizeof ($(selftype)));
.   endif
    item->cache_next = item;
    item->cache_prev = item;

    /*  Attach object to unused list                                         */
    s_$(selfname)_cache_attach (item, s_$(selfname)_cache_unused[idx]);
    s_$(selfname)_cache_unused_count[idx]++;
}

/*  Initialise the cache and register it with the meta-cache                 */
static inline void
s_$(selfname)_cache_new (void)
{
    int
        idx;

    for (idx = 0; idx < S_NUM_BUCKET_SIZES; idx ++) {
        /*  Our list heads are empty objects                                     */
        s_$(selfname)_cache_active[idx] = s_$(selfname)_cache_alloc (-1, __FILE__, __LINE__);
        s_$(selfname)_cache_unused[idx] = s_$(selfname)_cache_alloc (-1, __FILE__, __LINE__);
        s_$(selfname)_cache_active_count[idx] = 0;
        s_$(selfname)_cache_unused_count[idx] = 0;
    }

    /*  Register the purge and destroy call-back functions                   */
    icl_system_register ($(selfname)_cache_purge, $(selfname)_cache_destroy);
}

/*  Release all unused memory back to the heap                               */
static void
s_$(selfname)_cache_purge (void)
{
    int
        idx;
    size_t
        used_target_count;
    $(selftype) *
        item;
        
    for (idx = 0; idx < S_NUM_BUCKET_SIZES; idx ++) {
        /*  Free at least half of the unused memory.                             */
        used_target_count = s_$(selfname)_cache_unused_count[idx] / 2;
        
        while (s_$(selfname)_cache_unused_count[idx] > used_target_count) {
            item = s_$(selfname)_cache_unlink (s_$(selfname)_cache_unused[idx]->cache_next);
            s_$(selfname)_cache_unused_count[idx]--;
            s_$(selfname)_cache_free (item);
        }
    }
}

/*  Destroy all active objects and release all cache memory to the heap      */
static void
s_$(selfname)_cache_destroy (void)
{
    int
        idx;
    $(selftype) *
        item;

    for (idx = 0; idx < S_NUM_BUCKET_SIZES; idx ++) {
        /*  Report if there are active $(selfname)s                          */
        if (s_$(selfname)_cache_active[idx]->cache_next
        !=  s_$(selfname)_cache_active[idx]) {
            coprintf ("E: $(selfname:): Active allocations at destroy.");
            break;
        }
    }

    for (idx = 0; idx < S_NUM_BUCKET_SIZES; idx ++) {
        /*  Purge the unused list                                            */
        while (s_$(selfname)_cache_unused[idx]->cache_next != s_$(selfname)_cache_unused[idx]) {
            item = s_$(selfname)_cache_unlink (s_$(selfname)_cache_unused[idx]->cache_next);
            s_$(selfname)_cache_unused_count[idx]--;
            s_$(selfname)_cache_free (item);
        }

        /*  Now free the two lists themselves                                */
        s_$(selfname)_cache_free (s_$(selfname)_cache_active[idx]);
        s_$(selfname)_cache_free (s_$(selfname)_cache_unused[idx]);
    
        /*  Finally, reset our cache state so that it's inactive again       */
        s_$(selfname)_cache_active[idx] = NULL;
        s_$(selfname)_cache_unused[idx] = NULL;
    }
}

/*  Allocate a new object from heap memory                                   */
static inline $(selftype) *
s_$(selfname)_cache_alloc (int idx, char *file, size_t line)
{
    $(selftype)
        *item;

    if (idx >= 0)
        item = ($(selftype) *) icl_mem_alloc_ (sizeof ($(selftype)) + bucket_size [idx], file, line);
    else
        item = ($(selftype) *) icl_mem_alloc_ (sizeof ($(selftype)), file, line);
    if (item) {
        memset (item, 0, sizeof ($(selftype)));
        item->cache_next = item;
        item->cache_prev = item;
    }
    return (item);
}

/*  Return an object to heap memory                                          */
static inline void
s_$(selfname)_cache_free ($(selftype) *item)
{
    memset (item, 0, sizeof ($(selftype)));
    icl_mem_free (item);
}
</private>

<method name = "selftest">
    <local>
    $(selftype)
        *item;
    </local>
    item = $(selfname)_new (511);
    assert (item->max_size == 512);
    memset (item->data, 0, item->max_size);
    $(selfname)_destroy (&item);

    item = $(selfname)_new (512);
    assert (item->max_size == 512);
    memset (item->data, 0, item->max_size);
    $(selfname)_destroy (&item);

    item = $(selfname)_new (513);
    assert (item->max_size == 1024);
    memset (item->data, 0, item->max_size);
    $(selfname)_destroy (&item);

    item = $(selfname)_new (32767);
    assert (item->max_size == 32768);
    memset (item->data, 0, item->max_size);
    $(selfname)_destroy (&item);

    item = $(selfname)_new (32768);
    assert (item->max_size == 32768);
    memset (item->data, 0, item->max_size);
    $(selfname)_destroy (&item);

    item = $(selfname)_new (32769);
    assert (item->max_size == 32768);
    memset (item->data, 0, item->max_size);
    $(selfname)_destroy (&item);

    icl_system_destroy ();
    icl_mem_assert ();
</method>

</class>
