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
    name      = "icl_mem_thin"
    comment   = "iPR memory class including caching"
    version   = "1.1"
    script    = "icl_gen"
    >

<doc>
    A lightwieght memory allocator using a lock-free singly-linked stack.
    
</doc>

<inherit class = "icl_mem_lib" />

<option name = "links"  value = "0" />

<inherit class = "icl_list_item">
    <option name = "prefix" value = "list"   />
    <option name = "alloc"  value = "direct" />
    <option name = "rwlock" value = "0"      />
    <option name = "mutex"  value = "1"      />
    <option name = "count"  value = "1"      />
</inherit>

<import class = "icl_cache" />

<method name = "cache alloc"  inline = "1"/>
<method name = "alloc"        inline = "1"/>
<method name = "realloc"      inline = "1"/>
<method name = "free"         inline = "1"/>

<method name = "cache insert" private = "0" inline = "1">
    icl_atomic_inc32 (&cache->thin_used_count);
</method>

<method name = "cache reuse" private = "0" inline = "1">
    self = icl_mem_thin_list_pop (cache->thin_unused_list);
</method>

<method name = "cache recycle" private = "0" inline = "1">
    icl_atomic_dec32 (&cache->thin_used_count);
    if (cache->size)                    //  ie not direct cache
        icl_mem_thin_list_queue (cache->thin_unused_list, self);
</method>

<method name = "cache purge">
    <local>
    qbyte
        target_unused_count;
    $(selftype)
        *self;
    </local>
    target_unused_count = icl_mem_thin_list_count (cache->thin_unused_list) / 2;
    while (icl_mem_thin_list_count (cache->thin_unused_list) > target_unused_count) {
        self = icl_mem_thin_cache_reuse (cache);
        if (self)
            icl_mem_thin_destroy (&self);
        else
            break;
    }
</method>

<method name = "cache reset" private = "0" inline = "1">
    <doc>
    Don't need to do anything.
    </doc>
</method>

<method name = "cache assert">
    if (cache->thin_used_count) {
        rc = -1;
        icl_console_print ("E: %lu active cache allocations of size: %lu.",
                           (long) cache->thin_used_count, (long) cache->size);
        icl_console_print ("E: Rebuild with fat memory allocator for details");
    }
</method>

<method name = "show" inherit = "none" />

<method name = "possess" inline = "1">
    <doc>
    Does nothing but required to get function declared
    </doc>
</method>

<method name = "set callback" inline = "1">
    <doc>
    Does nothing but required to get function declared
    </doc>
</method>

<method name = "selftest" export = "none">
    <local>
    icl_cache_t
        *cache;
    void
        *buffer;
    </local>
    cache  = icl_cache_get (500);
    buffer = icl_mem_thin_cache_alloc (cache);
    icl_mem_thin_free (buffer);
</method>

</class>
