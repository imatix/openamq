<?xml?>
<!--
    Copyright (c) 1996-2009 iMatix Corporation

    This program is free software; you can redistribute it and/or modify
    it under the terms of the GNU General Public License as published by
    the Free Software Foundation; either version 2 of the License, or (at
    your option) any later version.

    This program is distributed in the hope that it will be useful, but
    WITHOUT ANY WARRANTY; without even the implied warranty of
    MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
    General Public License for more details.

    For information on alternative licensing for OEMs, please contact
    iMatix Corporation.
 -->
<class
    name      = "icl_mem"
    comment   = "iPR memory handler abstract class"
    version   = "1.1"
    script    = "icl_gen"
    abstract  = "1"
    >

<doc>
    Abstract class defining the interface for a cached memory manager.

    Memory allocations are always cached, but the implementation of the cache
    can vary.  The reason for this is that a lock-free cache brings better
    performance while a locking cache can provide better debug information
    for trapping memory leaks.

    The methods are 'cache alloc' to allocate a new block to a cache or
    directly and 'cache insert', an abstract private method to insert the newly
    allocated block into a cache.

    To use this class, define the body of 'cache alloc'.
</doc>

<inherit class = "icl_base"   />

<import class = "icl_cache"  />
<import class = "icl_system" />
<import class = "icl_stats"  />
<import class = "icl_system" />

<public>
//  Macros

#define $(selfname:UPPER)_SIZE (((sizeof ($(selftype)) + 7) / 8) * 8)

#define $(selfname:UPPER)_ASSERT_SANE(self, item)\\
{\\
    if (self->tag != ICL_CACHE_ALIVE) {\\
        icl_system_panic ("", "FATAL ERROR at %s:%u, in %s\\n", __FILE__, __LINE__, ICL_ASSERT_SANE_FUNCTION);\\
        icl_system_panic ("", "Allocated item at %p expected tag=0x%x, actual tag=0x%x\\n", item, ICL_CACHE_ALIVE, self->tag);\\
        icl_system_panic ("", "Please report this to openamq-dev@lists.openamq.org\\n");\\
        abort ();\\
    }\\
}

//  Global variables

extern Bool
    s_$(selfname)_active;
</public>

<context>
    qbyte
        tag;                            //  To detect corruption
    icl_cache_t
        *cache;                         //  The cache for this object
    qbyte
        size;                           //  The size of this object
</context>

<private>
//  Global variables

Bool
    s_$(selfname)_active = FALSE;
</private>

<method name = "cache alloc" return = "item">
    <doc>
    Reuses or allocates a block in a cache.  The size of the block is the size
    of the cache.
    </doc>
    <argument name = "cache" type = "icl_cache_t *">The cache for the allocation</argument>
    <argument name = "file" type = "char *" precalc = "__FILE__">Source file for call</argument>
    <argument name = "line" type = "size_t" precalc = "__LINE__">Line number for call</argument>
    <declare  name = "item" type = "void *" default = "NULL">The returned client pointer</declare>
    item = $(selfname)_cache_pop_ (cache, file, line);
    if (!item)
        item = $(selfname)_cache_new_ (cache, file, line);
</method>

<method name = "cache pop" return = "item">
    <doc>
    Pops a block from a cache.
    cache.
    </doc>
    <argument name = "cache" type = "icl_cache_t *">The cache for the allocation</argument>
    <argument name = "file" type = "char *" precalc = "__FILE__">Source file for call</argument>
    <argument name = "line" type = "size_t" precalc = "__LINE__">Line number for call</argument>
    <declare  name = "item" type = "void *" default = "NULL">The returned client pointer</declare>
    <local>
    $(selftype)
        *self;
    </local>
    self = $(selfname)_cache_reuse_ (cache, file, line);
    if (self) {
#if (defined (BASE_STATS) || defined (BASE_STATS_$(class.name:UPPER,c)))
        icl_stats_inc (NULL, &cache->allocs_stat);
#endif
        self->tag = ICL_CACHE_ALIVE;
        $(selfname)_cache_insert (cache, self);
        item = (byte *) self + $(selfname:UPPER)_SIZE;
        $(selfname)_possess_ (item, file, line);
    }
</method>

<method name = "cache new" return = "item">
    <doc>
    Allocates a block in a cache.  The size of the block is the size of the
    cache.
    </doc>
    <argument name = "cache" type = "icl_cache_t *">The cache for the allocation</argument>
    <argument name = "file" type = "char *" precalc = "__FILE__">Source file for call</argument>
    <argument name = "line" type = "size_t" precalc = "__LINE__">Line number for call</argument>
    <declare  name = "item" type = "void *" default = "NULL">The returned client pointer</declare>
    <local>
    $(selftype)
        *self;
    </local>
    self = $(selfname)_new (cache->size);
    if (self) {
        self->cache = cache;
#if (defined (BASE_STATS) || defined (BASE_STATS_$(class.name:UPPER,c)))
        icl_stats_inc (NULL, &cache->allocs_stat);
#endif
        self->tag = ICL_CACHE_ALIVE;
        $(selfname)_cache_insert (cache, self);
        item = (byte *) self + $(selfname:UPPER)_SIZE;
        $(selfname)_possess_ (item, file, line);
    }
</method>

<method name = "new" return = "self">
    <doc>
    Allocate a block.
    </doc>
    <argument name = "size" type = "size_t">How much memory to allocate</argument>
    <declare  name = "self" type = "$(selftype) *" >The allocation block</declare>
    <header>
    self = malloc ($(selfname:UPPER)_SIZE + size);

    //  Increment the record of total memory allocations
    icl_mem_increment_used ($(selfname:UPPER)_SIZE + size);

    if (self) {
        self->cache = NULL;
        self->size  = size;
    </header>
    <footer>
    }
    </footer>
</method>

<method name = "destroy">
    <doc>
    Destroy an allocated block.
    </doc>
    <argument name = "self" type = "$(selftype) *" ref = "1">The allocation block</argument>
    <header>
    if (self) {
    </header>
    <footer>
        //  Decrement the record of total memory allocations
        icl_mem_decrement_used ($(selfname:UPPER)_SIZE + self->size);

        free (self);
        *self_p = NULL;
    }
    </footer>
</method>

<method name = "alloc" return = "item">
    <doc>
    Allocate a block of a given size in the direct cache.  This isn't really
    a cache at all and just serves to track allocated blocks.
    cache.
    </doc>
    <argument name = "size" type = "size_t">How much memory to allocate</argument>
    <argument name = "file" type = "char *" precalc = "__FILE__">Source file for call</argument>
    <argument name = "line" type = "size_t" precalc = "__LINE__">Line number for call</argument>
    <declare  name = "item" type = "void *" default = "NULL">The returned client pointer</declare>
    <local>
    $(selftype)
        *self;
    </local>
    if (!s_direct_cache)
        icl_cache_initialise ();

    self = $(selfname)_new (size);
    if (self) {
        self->cache = s_direct_cache;
        self->size  = size;
#if (defined (BASE_STATS) || defined (BASE_STATS_$(class.name:UPPER,c)))
        icl_stats_inc (NULL, &s_direct_cache->allocs_stat);
#endif
        self->tag = ICL_CACHE_ALIVE;
        $(selfname)_cache_insert (s_direct_cache, self);
        item = (byte *) self + $(selfname:UPPER)_SIZE;
        $(selfname)_possess_ (item, file, line);
    }
</method>

<method name = "realloc" return = "item">
    <doc>
    Adjust the size of the pointed to 'buffer' to the new 'size'.
    If 'buffer' is NULL, self call behaves as a regular alloc().
    </doc>
    <argument name = "item"   type = "void *">Cient buffer to reallocate</argument>
    <argument name = "size"   type = "qbyte">How much memory to allocate</argument>
    <argument name = "file"   type = "char *" precalc = "__FILE__">Source file for call</argument>
    <argument name = "line"   type = "size_t" precalc = "__LINE__">Line number for call</argument>
    <local>
    $(selftype)
        *self;
    </local>
    if (item == NULL)
        item = $(selfname)_alloc_ (size, file, line);
    else {
        self = ($(selftype) *) (((byte *) item) - $(selfname:UPPER)_SIZE);
        $(selfname:UPPER)_ASSERT_SANE (self, item);

        item = NULL;
        if (!self->cache->size) {       //  ie direct cache
            $(selfname)_cache_recycle (&self);
            self = ($(selftype)*) realloc (self, $(selfname:UPPER)_SIZE + size);
            if (self) {
                //  Need to reset the cache pointer(s) following realloc.
                $(selfname)_cache_reset (self);

                self->size  = size;
                self->tag = ICL_CACHE_ALIVE;
                $(selfname)_cache_insert (s_direct_cache, self);

                item = (byte *) self + $(selfname:UPPER)_SIZE;
            }
        }
    }
</method>

<method name = "free">
    <doc>
    Destroys an allocation block.
    </doc>
    <argument name = "item" type = "void *">Client buffer to free</argument>
    <local>
    $(selftype)
        *self;
    </local>
    if (item) {
        self = ($(selftype) *) (((byte *) item) - $(selfname:UPPER)_SIZE);
        $(selfname:UPPER)_ASSERT_SANE (self, item);
        self->tag = ICL_CACHE_DEAD;

        $(selfname)_cache_recycle (&self);
        if (!self->cache->size) {       //  ie direct cache
            $(selfname)_destroy (&self);
#if (defined (BASE_STATS) || defined (BASE_STATS_$(class.name:UPPER,c)))
            icl_stats_inc (NULL, &s_direct_cache->frees_stat);
#endif
        }
    }
</method>

<method name = "possess">
    <doc>
    Sets file and line number of memory allocation.
    </doc>
    <argument name = "item" type = "void *">Client buffer to free</argument>
    <argument name = "file" type = "char *" precalc = "__FILE__">Source file for call</argument>
    <argument name = "line" type = "size_t" precalc = "__LINE__">Line number for call</argument>
</method>

<method name = "set callback">
    <doc>
    Sets callback of memory allocation.
    </doc>
    <argument name = "item" type = "void *">Client buffer to free</argument>
    <argument name = "callback" type = "icl_callback_fn *">Callback function to set</argument>
</method>

<method name = "cache reuse" return = "self">
    <doc>
    This is a private method used by $(selfname)_cache_alloc.

    Its purpose is to find an allocator block from the cache for reuse.
    </doc>
    <argument name = "cache" type = "icl_cache_t *">The cache for the allocation</argument>
    <argument name = "file" type = "char *" precalc = "__FILE__">Source file for call</argument>
    <argument name = "line" type = "size_t" precalc = "__LINE__">Line number for call</argument>
    <declare  name = "self" type = "void *" default = "NULL">The returned client pointer</declare>
    <footer>
#if (defined (BASE_STATS) || defined (BASE_STATS_$(class.name:UPPER,c)))
    if (self)
        icl_stats_dec (NULL, &cache->unused_stat);
#endif
    </footer>
</method>

<method name = "cache insert" private = "1">
    <doc>
    This is a private method used by $(selfname)_cache_alloc.

    Its purpose is to record an allocator block in use.
    </doc>
    <argument name = "cache" type = "icl_cache_t *">The cache for the allocation</argument>
    <argument name = "self"  type = "$(selftype) *">The allocation block</argument>
#if (defined (BASE_STATS) || defined (BASE_STATS_$(class.name:UPPER,c)))
    icl_stats_inc (NULL, &cache->used_stat);
#endif
</method>

<method name = "cache recycle" private = "1">
    <doc>
    This is a private method used by $(selfname)_cache_alloc.

    Its purpose is put an allocated block in the unused list of the cache.
    </doc>
    <argument name = "self"  type = "$(selftype) *" ref = "1">The allocation block</argument>
    <declare  name = "cache" type = "icl_cache_t *" default = "self->cache">The cache for the allocation</declare>
    <header>
#if (defined (BASE_STATS) || defined (BASE_STATS_$(class.name:UPPER,c)))
    icl_stats_dec (NULL, &cache->used_stat);
#endif
    </header>
    <footer>
#if (defined (BASE_STATS) || defined (BASE_STATS_$(class.name:UPPER,c)))
    if (cache->size)                    //  ie not direct cache
        icl_stats_inc (NULL, &cache->unused_stat);
#endif
    </footer>
</method>

<method name = "cache reset" private = "1">
    <doc>
    This is a private method used by $(selfname)_cache_realloc.

    Its purpose is to reset the block's cache pointers following a realloc.
    </doc>
    <argument name = "self"  type = "$(selftype) *">The allocation block</argument>
</method>

<method name = "cache assert" return = "rc" >
    <declare name = "rc" type = "int" default = "0">Return code</declare>
    <doc>
    This is a private method used by $(selfname)_cache_alloc.

    Its purpose is report on leaked allocations in the cache.
    </doc>
    <argument name = "cache" type = "icl_cache_t *">The cache for the allocation</argument>
    <argument name = "trace_file" type = "FILE **">File to print to, opened on error</argument>
</method>

<method name = "cache purge">
    <doc>
    The 'purge' method frees half of the unused blocks.
    </doc>
    <argument name = "cache" type = "icl_cache_t *">The cache to purge</argument>
</method>

</class>
