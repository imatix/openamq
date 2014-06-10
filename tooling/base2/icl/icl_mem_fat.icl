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
    name      = "icl_mem_fat"
    comment   = "iPR memory class including caching"
    version   = "1.1"
    script    = "icl_gen"
    >

<doc>
    A memory allocator using fully linked lists and providing debug features.
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

<context>
    char 
        *file;                          //  Source file of last allocation
    size_t
        line;                           //  Source line of last allocation
    icl_callback_fn
        *callback;                      //  Function to callback memory contents
</context>

<method name = "new">
    self->file     = NULL;
    self->line     = 0;
    self->callback = NULL;
</method>

<method name = "destroy">
    <header>
    icl_mem_fat_list_remove (self);
    </header>
</method>

<method name = "cache insert">
    icl_mem_fat_list_queue (cache->fat_used_list, self);
</method>

<method name = "cache reuse">
    self = icl_mem_fat_list_pop (cache->fat_unused_list);
</method>

<method name = "cache recycle">
    icl_mem_fat_list_remove (self);
    if (cache->size)                    //  ie not direct cache
        icl_mem_fat_list_queue (cache->fat_unused_list, self);
</method>

<method name = "cache purge">
    <local>
    qbyte
        target_unused_count;
    $(selftype)
        *self;
    </local>
    target_unused_count = icl_mem_fat_list_count (cache->fat_unused_list) / 2;
    while (icl_mem_fat_list_count (cache->fat_unused_list) > target_unused_count) {
        self = icl_mem_fat_cache_reuse (cache);
        icl_mem_fat_destroy (&self);
    }
</method>

<method name = "cache reset">
    //  Manipulate the list pointers manually as this is an unusual case.
    self->list_next = self;
    self->list_prev = self;
</method>

<method name = "cache assert">
    <local>
    $(selftype)
        *self;
    </local>
    self = icl_mem_fat_list_first (cache->fat_used_list);
    if (self) {
        rc = -1;
        if (*trace_file == NULL) {
            icl_console_print ("E: Active cache allocation details are in icl_mem_fat.xml");
            *trace_file = fopen ("icl_mem_fat.xml", "w");
            fprintf (*trace_file, "&lt;?xml version=\\"1.0\\"?>\\n");
        }
        icl_console_print ("E: %lu active cache allocations of size: %lu",
                           (long) icl_mem_fat_list_count (cache->fat_used_list),
                           (long) cache->size);
        fprintf (*trace_file, "&lt;list>\\n");
        while (self) {
            if (self->callback)
                (self->callback) (((byte *) self) + $(selfname:UPPER)_SIZE, ICL_CALLBACK_DUMP, *trace_file, self->file, self->line);
            else
                fprintf (*trace_file,
                        "    &lt;block-size=\\"%lu\\" file=\\"%s\\" line=\\"%lu\\" pointer=\\"%pp\\"/>\\n",
                        (long) cache->size,
                        self->file ? self->file : "&lt;Unknown>",
                        (long) self->line,
                        (byte *) self + $(selfname:UPPER)_SIZE);
                        
            self = icl_mem_fat_list_next (&self);
        }
        fprintf (*trace_file, "&lt;/list>\\n");
    }
</method>

<method name = "possess">
    <local>
    $(selftype)
        *self;
    </local>
    if (item) {
        self = ($(selftype) *) (((byte *) item) - $(selfname:UPPER)_SIZE);
        $(selfname:UPPER)_ASSERT_SANE (self, item);
        self->file = file;
        self->line = line;
    }
</method>

<method name = "set callback">
    <local>
    $(selftype)
        *self;
    </local>
    if (item) {
        self = ($(selftype) *) (((byte *) item) - $(selfname:UPPER)_SIZE);
        $(selfname:UPPER)_ASSERT_SANE (self, item);
        self->callback = callback;
    }
</method>

<method name = "selftest" export = "none">
    <local>
    icl_cache_t
        *cache;
    void
        *buffer;
    </local>
    //
    cache  = icl_cache_get (500);
    buffer = icl_mem_fat_cache_alloc (cache);
    icl_mem_fat_free (buffer);
</method>

</class>
