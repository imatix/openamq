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

<inherit class = "icl_base"/>

<import class = "icl_cache"/>
<import class = "icl_system"/>

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
#include "base_apr.h"

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

#if (defined (BASE_THREADSAFE))
static apr_thread_mutex_t
    *s_class_mutex = NULL;
#endif

static icl_cache_t *
    s_$(selfname)_cache [S_NUM_BUCKET_SIZES] = {\
.for class->data.size
                       NULL$(!last()??','?)\
.endfor
                       };

static void
    s_class_initialise (void);
static void
    s_class_terminate (void);
</private>

<context>
#if (defined (BASE_THREADSAFE))
    apr_thread_rwlock_t *
        rwlock;
#endif
    int    possess_count;
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
    if (! s_class_active)
        s_class_initialise ();
        
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

    self = icl_cache_alloc_ (s_$(selfname)_cache [chop_min], file, line);
#if (defined (BASE_THREADSAFE))
    icl_apr_assert (apr_thread_rwlock_create (&self->rwlock,
                                              icl_global_pool));
#endif
    self->possess_count = 1;
    self->cur_size      = 0;
    self->max_size      = bucket_size [chop_min];
    self->data          = (byte *) self + sizeof ($(selftype));
</method>

<method name = "cache purge">
    <local>
    int
        cache_index;
    </local>
    for (cache_index = 0; cache_index < S_NUM_BUCKET_SIZES; cache_index++)
        icl_cache_purge (s_$(selfname)_cache [cache_index]);
</method>

<method name = "destroy" template = "destructor">
#if (defined (BASE_THREADSAFE))
    apr_thread_rwlock_t
        *rwlock;
#endif

    if (self) {
#if (defined (BASE_THREADSAFE))
        if (apr_atomic_dec32 (&self->possess_count) == 0) {
#else    
        if (--self->possess_count == 0) {
#endif
#if (defined (BASE_THREADSAFE))
            rwlock = self->rwlock;
            icl_apr_assert (apr_thread_rwlock_wrlock (rwlock));
#endif
            icl_cache_free (self);
#if (defined (BASE_THREADSAFE))
            icl_apr_assert (apr_thread_rwlock_unlock (rwlock));
#endif
        }
        *self_p = NULL;
    }
</method>

<method name = "possess" return = "self">
    <argument name = "self" type = "$(selftype) *"/>
    <argument name = "file" type = "char *" precalc = "__FILE__">Source file for call</argument>
    <argument name = "line" type = "size_t" precalc = "__LINE__">Line number for call</argument>
    <animate  name = "possess_count" value = "self->possess_count" format = "%i">The number of references to the object</animate>
#if (defined (BASE_THREADSAFE))
    apr_atomic_inc32 (&self->possess_count);
#else    
    self->possess_count++;
#endif
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
#if (defined (BASE_THREADSAFE))
    icl_apr_assert (apr_thread_rwlock_wrlock (self->rwlock));
#endif
    memcpy (self->data, source, size);
    self->cur_size = size;
#if (defined (BASE_THREADSAFE))
    icl_apr_assert (apr_thread_rwlock_unlock (self->rwlock));
#endif
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
#if (defined (BASE_THREADSAFE))
    icl_apr_assert (apr_thread_rwlock_wrlock (self->rwlock));
#endif
    self->cur_size = fread (self->data, 1, size, file);
#if (defined (BASE_THREADSAFE))
    icl_apr_assert (apr_thread_rwlock_unlock (self->rwlock));
#endif
</method>

<method name = "save" template = "function">
    <doc>
    Saves the data in the bucket to a file.
    </doc>
    <argument name = "file" type = "FILE *">File pointer of open file to copy to</argument>
#if (defined (BASE_THREADSAFE))
    icl_apr_assert (apr_thread_rwlock_rdlock (self->rwlock));
#endif
    fwrite (self->data, 1, self->cur_size, file);
#if (defined (BASE_THREADSAFE))
    icl_apr_assert (apr_thread_rwlock_unlock (self->rwlock));
#endif
</method>

<private name = "footer">
static void s_class_initialise (void)
{
    int
        cache_index;

#if (defined (BASE_THREADSAFE))
    icl_apr_assert (apr_thread_mutex_lock (icl_global_mutex));
    if (! s_class_mutex)
        icl_apr_assert (apr_thread_mutex_create (&s_class_mutex,
                                                APR_THREAD_MUTEX_DEFAULT,
                                                icl_global_pool));
    icl_apr_assert (apr_thread_mutex_unlock (icl_global_mutex));
    icl_apr_assert (apr_thread_mutex_lock (s_class_mutex));
#endif

    if (! s_class_active) {
        for (cache_index = 0; cache_index < S_NUM_BUCKET_SIZES; cache_index++)
            s_$(selfname)_cache [cache_index] = icl_cache_get (bucket_size [cache_index] + sizeof ($(selftype)));
            
        /*  Register the class termination call-back functions               */
        icl_system_register (NULL, s_class_terminate);
        
        s_class_active = TRUE;
    }
    
#if (defined (BASE_THREADSAFE))
    icl_apr_assert (apr_thread_mutex_unlock (s_class_mutex));
#endif
}

static void
s_class_terminate (void)
{
#if (defined (BASE_THREADSAFE))
    icl_apr_assert (apr_thread_mutex_destroy (s_class_mutex));
#endif
    s_class_active = FALSE;
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
</method>

</class>
