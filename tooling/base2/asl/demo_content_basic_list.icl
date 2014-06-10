<?xml?>
<!--
    *** GENERATED FROM demo.asl BY ASL_GEN USING GSL/4 ***

    
    Copyright (c) 1996-2009 iMatix Corporation
    
    This file is licensed under the GPL as follows:
    
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
    name      = "demo_content_basic_list"
    comment   = "Demo Basic content list container"
    version   = "1.0"
    script    = "icl_gen"
    license   = "gpl"
    >
<import class = "demo_content_basic" />
<inherit class = "icl_iter_list_head" >
    <option name = "prefix"     value = "list" />
    <option name = "childname"  value = "demo_content_basic" />
    <option name = "childtype"  value = "demo_content_basic_t" />
    <option name = "count"      value = "1" />
</inherit>

<option name = "alloc" value = "cache"/>

<method name = "selftest" />
</class>
