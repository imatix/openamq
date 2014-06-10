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
    comment   = "iPR memory class API"
    version   = "1.1"
    script    = "icl_gen"
    >

<doc>
    Provides API for memory allocator.  Methods are implemented in icl_mem_fat
    and icl_mem_thin.
</doc>

<inherit class = "icl_init" />

<import class = "icl_mem_fat"/>
<import class = "icl_mem_thin"/>
<import class = "icl_system" />

<public name = "header">
//  Types

typedef void (icl_mem_free_fn) (void *data);

typedef enum {
    ICL_MEM_UNDEFINED,
    ICL_MEM_FAT,
    ICL_MEM_THIN,
    ICL_MEM_DIRECT
} icl_mem_allocator_t;

//  Global variables

extern icl_mem_allocator_t
    icl_mem_allocator;
extern volatile void *
    icl_mem_total_used;
</public>

<private name = "header">
//  Global variables

//  Set default memory allocagtor depending on build model and
//  definition or otherwise of ICL_MEM_FAT/ICL_MEM_THIN.
icl_mem_allocator_t
    icl_mem_allocator;

volatile void *
    icl_mem_total_used = 0;
</private>

<method name = "set allocator" base = "icl">
    <argument name = "allocator" type = "icl_mem_allocator_t">The allocator to use</argument>
    <doc>
    This method allows application control over the memory allocator to use.

    It affects all successive memory allocation, so should be called before
    initialisation.
    </doc>
    icl_mem_allocator = allocator;
</method>

<method name = "initialise" private = "0">
    <local>
    apr_status_t
        status;
    char
        *value;
    </local>
    //
    status = apr_env_get (&value, "ALLOCATOR", icl_global_pool);
    if (status == APR_SUCCESS) {
        if (streq (value, "thin"))
            icl_mem_allocator = ICL_MEM_THIN;
        else if (streq (value, "fat"))
            icl_mem_allocator = ICL_MEM_FAT;
        else if (streq (value, "direct"))
            icl_mem_allocator = ICL_MEM_DIRECT;
        else {
            icl_system_panic ("icl_mem", "Unknown allocator: %s, legal values are 'fat', 'thin', 'direct'", value);
            abort ();
        }
    }
    else
        //  If no value was provided choose a default based on value of DEBUG
#if defined (DEBUG)
        icl_mem_allocator = ICL_MEM_FAT;
#else
        icl_mem_allocator = ICL_MEM_DIRECT;
#endif
</method>

<method name = "cache reuse" return = "item" inline = "1">
    <doc>
    Gets a block from a cache but does not allocate a new one if the cache is
    empty.
    </doc>
    <argument name = "cache" type = "icl_cache_t *">The cache for the allocation</argument>
    <argument name = "file" type = "char *" precalc = "__FILE__">Source file for call</argument>
    <argument name = "line" type = "size_t" precalc = "__LINE__">Line number for call</argument>
    <declare  name = "item" type = "void *" default = "NULL">The returned client pointer</declare>
    if (icl_mem_allocator == ICL_MEM_THIN)
        item = icl_mem_thin_cache_reuse_ (cache, file, line);
    else
    if (icl_mem_allocator == ICL_MEM_FAT)
        item = icl_mem_fat_cache_reuse_ (cache, file, line);
</method>

<method name = "cache alloc" return = "item" inline = "1">
    <doc>
    Allocate a block in a cache.  The size of the block is the size of the
    cache.
    </doc>
    <argument name = "cache" type = "icl_cache_t *">The cache for the allocation</argument>
    <argument name = "file" type = "char *" precalc = "__FILE__">Source file for call</argument>
    <argument name = "line" type = "size_t" precalc = "__LINE__">Line number for call</argument>
    <declare name = "item" type = "void *" default = "NULL">The returned client pointer</declare>
    if (icl_mem_allocator == ICL_MEM_THIN)
        item = icl_mem_thin_cache_alloc_ (cache, file, line);
    else
    if (icl_mem_allocator == ICL_MEM_FAT)
        item = icl_mem_fat_cache_alloc_ (cache, file, line);
    else
        item = malloc (cache->size);
</method>

<method name = "cache pop" return = "item" inline = "1">
    <doc>
    Pops a block from a cache.
    cache.
    </doc>
    <argument name = "cache" type = "icl_cache_t *">The cache for the allocation</argument>
    <argument name = "file" type = "char *" precalc = "__FILE__">Source file for call</argument>
    <argument name = "line" type = "size_t" precalc = "__LINE__">Line number for call</argument>
    <declare name = "item" type = "void *" default = "NULL">The returned client pointer</declare>
    if (icl_mem_allocator == ICL_MEM_THIN)
        item = icl_mem_thin_cache_pop_ (cache, file, line);
    else
    if (icl_mem_allocator == ICL_MEM_FAT)
        item = icl_mem_fat_cache_pop_ (cache, file, line);
</method>

<method name = "cache new" return = "item" inline = "1">
    <doc>
    Allocates a block in a cache.  The size of the block is the size of the
    cache.
    </doc>
    <argument name = "cache" type = "icl_cache_t *">The cache for the allocation</argument>
    <argument name = "file" type = "char *" precalc = "__FILE__">Source file for call</argument>
    <argument name = "line" type = "size_t" precalc = "__LINE__">Line number for call</argument>
    <declare name = "item" type = "void *" default = "NULL">The returned client pointer</declare>
    if (icl_mem_allocator == ICL_MEM_THIN)
        item = icl_mem_thin_cache_new_ (cache, file, line);
    else
    if (icl_mem_allocator == ICL_MEM_FAT)
        item = icl_mem_fat_cache_new_ (cache, file, line);
    else
        item = malloc (cache->size);
</method>

<method name = "alloc" return = "item" inline = "1">
    <doc>
    Allocate a block in a cache.  The size of the block is the size of the
    cache.
    </doc>
    <argument name = "size" type = "size_t">How much memory to allocate</argument>
    <argument name = "file" type = "char *" precalc = "__FILE__">Source file for call</argument>
    <argument name = "line" type = "size_t" precalc = "__LINE__">Line number for call</argument>
    <declare name = "item" type = "void *" default = "NULL">The returned client pointer</declare>
    if (icl_mem_allocator == ICL_MEM_THIN)
        item = icl_mem_thin_alloc_ (size, file, line);
    else
    if (icl_mem_allocator == ICL_MEM_FAT)
        item = icl_mem_fat_alloc_ (size, file, line);
    else
        item = malloc (size);
</method>

<method name = "realloc" return = "item" inline = "1">
    <doc>
    Adjust the size of the pointed to 'buffer' to the new 'size'.
    If 'buffer' is NULL, self call behaves as a regular alloc().
    </doc>
    <argument name = "item"   type = "void *">Cient buffer to reallocate</argument>
    <argument name = "size"   type = "qbyte">How much memory to allocate</argument>
    <argument name = "file"   type = "char *" precalc = "__FILE__">Source file for call</argument>
    <argument name = "line"   type = "size_t" precalc = "__LINE__">Line number for call</argument>
    if (icl_mem_allocator == ICL_MEM_THIN)
        item = icl_mem_thin_realloc_ (item, size, file, line);
    else
    if (icl_mem_allocator == ICL_MEM_FAT)
        item = icl_mem_fat_realloc_ (item, size, file, line);
    else
        item = realloc (item, size);
</method>

<method name = "free" inline = "1">
    <doc>
    Destroys an allocation block.
    </doc>
    <argument name = "item" type = "void *">Client buffer to free</argument>
    if (icl_mem_allocator == ICL_MEM_THIN)
        icl_mem_thin_free (item);
    else
    if (icl_mem_allocator == ICL_MEM_FAT)
        icl_mem_fat_free (item);
    else
        free (item);
</method>

<method name = "possess" inline = "1">
    <doc>
    Sets file and line number of memory allocation.
    </doc>
    <argument name = "item" type = "void *">Client buffer to free</argument>
    <argument name = "file" type = "char *" precalc = "__FILE__">Source file for call</argument>
    <argument name = "line" type = "size_t" precalc = "__LINE__">Line number for call</argument>
    if (icl_mem_allocator == ICL_MEM_THIN)
        icl_mem_thin_possess_ (item, file, line);
    else
    if (icl_mem_allocator == ICL_MEM_FAT)
        icl_mem_fat_possess_ (item, file, line);
</method>

<method name = "set callback" inline = "1">
    <doc>
    Sets callback of memory allocation.
    </doc>
    <argument name = "item" type = "void *">Client buffer to free</argument>
    <argument name = "callback" type = "icl_callback_fn *">Callback function to set</argument>
    if (icl_mem_allocator == ICL_MEM_THIN)
        icl_mem_thin_set_callback (item, callback);
    else
    if (icl_mem_allocator == ICL_MEM_FAT)
        icl_mem_fat_set_callback (item, callback);
</method>

<method name = "cache purge">
    <doc>
    The 'purge' method frees half of the unused blocks.
    </doc>
    <argument name = "cache" type = "icl_cache_t *">The cache to purge</argument>
    if (icl_mem_allocator == ICL_MEM_THIN)
        icl_mem_thin_cache_purge (cache);
    else
    if (icl_mem_allocator == ICL_MEM_FAT)
        icl_mem_fat_cache_purge (cache);
</method>

<method name = "increment used" inline = "1">
    <doc>
    Increment the record of the total memory allocations.

    This is non-trivial because it must be thread-safe, and it must work on
    64 bit architectures.
    </doc>
    <argument name = "size" type = "size_t">The amount by which to increment the total</argument>
    <local>
    void
        *old_used;
    </local>
    do
        old_used = (void *) icl_mem_total_used;
    while (icl_atomic_casptr (&icl_mem_total_used, (byte *) old_used + size, old_used) != old_used);
</method>

<method name = "decrement used" inline = "1">
    <doc>
    Decrement the record of the total memory allocations.

    This is non-trivial because it must be thread-safe, and it must work on
    64 bit architectures.
    </doc>
    <argument name = "size" type = "size_t">The amount by which to decrement the total</argument>
    <local>
    void
        *old_used;
    </local>
    do
        old_used = (void *) icl_mem_total_used;
    while (icl_atomic_casptr (&icl_mem_total_used, (byte *) old_used - size, old_used) != old_used);
</method>

<method name = "used" return = "used">
    <doc>
    Return the total amount of memory allocated.
    </doc>
    <declare name = "used" type = "size_t">The returned value</declare>
    //  Do this in a loop to prevent race condition giving a messed-up value
    //  On 32-bit architecture, could use atomic assignment, but not on 64-bit.
    do
        used = (size_t) icl_mem_total_used;
    while (used != (size_t) icl_mem_total_used);
</method>

<method name = "strdup" return = "item" inline = "1">
    <doc>
    Duplicates the provided string, returns a fresh copy.  Accepts a null string
    argument, in which case it returns null.
    </doc>
    <argument name = "string" type = "void *">String to duplicate</argument>
    <argument name = "file" type = "char *" precalc = "__FILE__">Source file for call</argument>
    <argument name = "line" type = "size_t" precalc = "__LINE__">Line number for call</argument>
    <declare  name = "item" type = "void *" default = "NULL">The returned client pointer</declare>
    <local>
    qbyte
        len;
    </local>
    if (string) {
        len = strlen ((char*) string) + 1;
        item = $(selfname)_alloc_ (len, file, line);
        if (item)
            strncpy ((char*) item, (char*) string, len);
    }
    else
        item = NULL;
</method>

<method name = "strfree" inline = "1">
    <doc>
    Frees the memory space pointed to by 'buffer'.
    </doc>
    <argument name = "client" type = "char *" ref = "1">Client buffer to free</argument>
    if (client) {
        $(selfname)_free (client);
        *client_p = NULL;
    }
</method>

<method name = "selftest">
    <local>
    char
        *str;
    </local>
    str = icl_mem_strdup ("$(selfname): Hello world");
    icl_mem_strfree (&str);
    assert (str == NULL);
</method>

</class>
