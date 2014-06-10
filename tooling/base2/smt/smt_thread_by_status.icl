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
    name      = "smt_thread_by_status"
    comment   = "Implements the list container for smt_thread"
    version   = "1.0"
    script    = "icl_gen"
    >
<doc>
    This class implements the list container for smt_thread
</doc>

<option name = "links"      value = "1" />

<inherit class = "icl_list_head" >
    <option name = "prefix"     value = "by_status"/>
    <option name = "childname"  value = "smt_thread" />
    <option name = "childtype"  value = "smt_thread_t" />
    <option name = "count"      value = "0" />
    <option name = "child_has_links"    value = "0"/>
    <option name = "link_to_child"      value = "0"/>
</inherit>
<option name = "rwlock"    value = "0" />
<option name = "mutex"     value = "0" />
<option name = "alloc"     value = "cache" />
<option name = "stats"     value = "1" />
<option name = "trace"     value = "1" />
<option name = "animate"   value = "1" />

<import class = "smt_thread" />

<method name = "remove">
    <doc>
    This method removes a threadlet from a status list, updating the
    priority pointers if needed.
    </doc>
    <local>
    smt_os_thread_t *
        os_thread = item->os_thread;
    smt_priority_t
        priority_index;
    smt_thread_t
        *next_thread;
    </local>
#if (defined (BASE_THREADSAFE) && defined (DEBUG))
    if (os_thread)
        assert (apr_os_thread_current () == os_thread->apr_os_thread);
#endif
    if (os_thread
    &&  item->by_status_head == os_thread->waiting_list
    &&  item->priority > 0) {
        next_thread = item;
        next_thread = smt_thread_by_status_next (&next_thread);
        
        //  Adjust insertion points for higher priority
        priority_index = item->priority;
        while (priority_index > 0) {
            priority_index--;
            if (item == os_thread->waiting_list_entry [priority_index])
                os_thread->waiting_list_entry [priority_index] = next_thread;
            else
                break;
        }
    }
</method>

<method name = "selftest" />

</class>
