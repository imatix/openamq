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
    name      = "icl_cache"
    comment   = "Cache item for memory allocations"
    version   = "1.0"
    script    = "icl_gen"
    >

<inherit class = "icl_init" />
<inherit class = "icl_object"/>
<inherit class = "icl_list_item" >
    <option name = "links"  value = "0"  />
    <option name = "rwlock" value = "0"  />
    <option name = "mutex"  value = "1"  />
</inherit>

<option name = "alloc" value = "direct" />
<option name = "links" value = "0"      />

<import class = "icl_mem_fat" />
<import class = "icl_mem_thin" />

<private>
//  Global variables local to this source                                      

static icl_cache_list_t
    *s_cache_list = NULL;               //  The cache list

//  Global variables                                                           

icl_cache_t
    *s_direct_cache = NULL;             //  Pseudo-cache for direct alloc'ns   
</private>

<public name = "header">
//  Macros                                                                     

#define ICL_CALLBACK_DUMP 0

#define ICL_CACHE_HEADER 0xEdeaU        //      if block is a cache header.    

//  Types

typedef void (icl_callback_fn) (void *item, int opcode, FILE *trace_file, char *file, size_t line);
typedef void (icl_mem_thin_free_fn) (void *data);

//  Global variables                                                           

extern icl_cache_t
    *s_direct_cache;                    //  Pseudo-cache for direct alloc'ns   
</public>

<context>
    icl_mem_fat_list_t
        *fat_used_list,                 //  List of used objects               
        *fat_unused_list;               //  List of unused objects
    icl_mem_thin_list_t
        *thin_unused_list;              //  List of unused objects
    qbyte
        thin_used_count;                //  Number of used objects
    size_t
        size;                           //  Size of objects in this cache
#if (defined (BASE_STATS) || defined (BASE_STATS_$(class.name:UPPER,c)))
    icl_stats_t
        *used_stat,
        *unused_stat,
        *allocs_stat,
        *frees_stat;
#endif
</context>

<method name = "new">
    self->fat_used_list         = icl_mem_fat_list_new ();
    self->fat_unused_list       = icl_mem_fat_list_new ();
    self->thin_unused_list      = icl_mem_thin_list_new ();
    self->thin_used_count       = 0;
    self->size                  = 0;

#if (defined (BASE_STATS) || defined (BASE_STATS_$(class.name:UPPER,c)))
    self->used_stat   = NULL;
    self->unused_stat = NULL;
    self->allocs_stat = NULL;
    self->frees_stat  = NULL;
#endif
</method>

<method name = "destroy">
    icl_mem_fat_list_destroy      (&self->fat_used_list);
    icl_mem_fat_list_destroy      (&self->fat_unused_list);
    icl_mem_thin_list_destroy     (&self->thin_unused_list);
</method>

<method name = "create" return = "self">
    <argument name = "size" type = "qbyte"/>
    <declare  name = "self" type = "$(selftype) *"/>
    <doc>
    Create a new cache with a given size.  Use for objects that don't share
    their cache, presumably because they re-use more than the memory.
    </doc>
    <local>
#if (defined (BASE_STATS) || defined (BASE_STATS_$(class.name:UPPER,c)))
    char
        name [] = "icl_mem_xxxxxx_yyyyyy ";
#endif
    </local>
    if (!s_$(selfname)_active)
        $(selfname)_initialise ();

#if (defined (BASE_THREADSAFE))
    icl_mutex_lock (s_$(selfname)_mutex);
#endif
    
    //  Shift size up to nearest 64-byte boundary
    size = (size + 63) & 0xFFFFFFC0;

    self = icl_cache_new ();
    self->size = size;

#if (defined (BASE_STATS) || defined (BASE_STATS_$(class.name:UPPER,c)))
    sprintf (&name [8], "%06lu ", (long) size);
    strcpy (&name [15], "used");
    self->used_stat = icl_stats_new (name);
    strcpy (&name [15], "unused");
    self->unused_stat = icl_stats_new (name);
    strcpy (&name [15], "allocs");
    self->allocs_stat = icl_stats_new (name);
    strcpy (&name [15], "frees");
    self->frees_stat = icl_stats_new (name);
#endif
    icl_cache_list_queue (s_cache_list, self);

#if (defined (BASE_THREADSAFE))
    icl_mutex_unlock (s_$(selfname)_mutex);
#endif
</method>

<method name = "get" return = "self">
    <argument name = "size" type = "qbyte"/>
    <doc>
    Find or create a cache of a given size.
    </doc>
    <declare  name = "self" type = "$(selftype) *"/>
    <local>
    icl_cache_t
        *old_cache;
#if (defined (BASE_STATS) || defined (BASE_STATS_$(class.name:UPPER,c)))
    char
        name [] = "icl_mem_xxxxxx_yyyyyy ";
#endif
    </local>
    if (!s_$(selfname)_active)
        $(selfname)_initialise ();

#if (defined (BASE_THREADSAFE))
    icl_mutex_lock (s_$(selfname)_mutex);
#endif
    //  Shift size up to nearest 64-byte boundary
    size = (size + 63) & 0xFFFFFFC0;

    old_cache = icl_cache_list_first (s_cache_list);
    while (old_cache) {
        if (old_cache->size >= size)
            break;
        old_cache = icl_cache_list_next (&old_cache);
    }
    if (old_cache
    &&  old_cache->size == size)
        self = old_cache;
    else {
        self = icl_cache_new ();
        self->size = size;

#if (defined (BASE_STATS) || defined (BASE_STATS_$(class.name:UPPER,c)))
        sprintf (&name [8], "%06lu ", (long) size);
        strcpy (&name [15], "used");
        self->used_stat = icl_stats_new (name);
        strcpy (&name [15], "unused");
        self->unused_stat = icl_stats_new (name);
        strcpy (&name [15], "allocs");
        self->allocs_stat = icl_stats_new (name);
        strcpy (&name [15], "frees");
        self->frees_stat = icl_stats_new (name);
#endif
        if (old_cache)
            icl_cache_list_relink_before (self, old_cache);
        else
            icl_cache_list_queue (s_cache_list, self);
    }

#if (defined (BASE_THREADSAFE))
    icl_mutex_unlock (s_$(selfname)_mutex);
#endif
</method>

<method name = "assert" return = "rc">
    <argument name = "file" type = "char *" precalc = "__FILE__">Source file for call</argument>
    <argument name = "line" type = "size_t" precalc = "__LINE__">Line number for call</argument>
    <declare name = "rc" type = "int" default = "0">Return code</declare>
    <local>
    $(selftype)
        *self;
    FILE
        *trace_file = NULL;
    </local>
    if (s_cache_list) {
        rc |= icl_mem_fat_cache_assert      (s_direct_cache, &trace_file);
        rc |= icl_mem_thin_cache_assert     (s_direct_cache, &trace_file);
        self = icl_cache_list_first (s_cache_list);
        while (self) {
            rc |= icl_mem_fat_cache_assert      (self, &trace_file);
            rc |= icl_mem_thin_cache_assert     (self, &trace_file);
            self = icl_cache_list_next (&self);
        }
        if (trace_file) {
            icl_console_redirect (trace_file);
            icl_trace_dump (NULL);
            icl_trace_terminate ();
            icl_console_redirect (NULL);
            fclose (trace_file);
        }
        //  Let user choose exit procedure (profiling breaks with aborts)
        if (rc) {
            char answer [3];
            printf ("Press [Enter] to end process, or [d] to get a core dump: ");
            strclr (answer);
            if (fgets (answer, 2, stdin) && *answer == 'd')
                abort ();
         }
    }
</method>

<method name = "initialise" private = "0">
    s_icl_cache_active = TRUE;          //  To prevent recursion
    
    s_cache_list   = icl_cache_list_new ();
    s_direct_cache = icl_cache_new ();
#if (defined (BASE_STATS) || defined (BASE_STATS_$(class.name:UPPER,c)))
    s_direct_cache->used_stat   = icl_stats_new ("icl_mem_direct_used");
    s_direct_cache->allocs_stat = icl_stats_new ("icl_mem_direct_allocs");
    s_direct_cache->frees_stat  = icl_stats_new ("icl_mem_direct_frees");
#endif
</method>

<method name = "terminate">
    icl_cache_assert ();
    icl_cache_list_destroy (&s_cache_list);
</method>

<method name = "selftest">
</method>

</class>
