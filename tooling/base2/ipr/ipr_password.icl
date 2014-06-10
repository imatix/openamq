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
    name      = "ipr_password"
    comment   = "Manages an entry in a password table"
    version   = "1.0"
    script    = "icl_gen"
    >
<doc>
    Password tables are held as dictionaries.  Supports passwords in
    either basic or digest format as defined by RFC 2617. ipr_password
    items do not use links.  This class is threadsafe and can be used
    from concurrent threads.
</doc>

<inherit class = "icl_object">
    <option name = "alloc" value = "cache" />
    <option name = "links" value = "0" />
    <option name = "mutex" value = "1" />
</inherit>
<!-- Password table organised as random-access hash table -->
<inherit class = "icl_hash_item">
    <option name = "hash_type" value = "str" />
</inherit>
<!-- Password table organised as serial-access list -->
<inherit class = "icl_list_item">
    <option name = "count" value = "1" />
</inherit>

<import class = "ipr_regexp" />
<import class = "ipr_nonce" />

<public name = "header">
#define IPR_PASSWD_BASIC        1       //  Basic encoding
#define IPR_PASSWD_DIGEST       2       //  Digest encoding
</public>

<context>
    char
        *hash;                          //  Password hash
    int
        type;                           //  IPR_PASSWD_ value
    //  Guard data
    size_t
        hits,                           //  Total hits
        hit_quota,                      //  Hits remaining, or 0
        misses,                         //  Total misses
        miss_quota;                     //  Misses remaining, or 0
    apr_time_t
        expiry,                         //  Expiration time, or 0
        hit_time,                       //  Time of last hit
        miss_time;                      //  Time of last miss
    icl_shortstr_t
        hit_from,                       //  Last good IP address
        miss_from;                      //  Last bad IP address
    Bool
        expired;                        //  Run out of luck...
</context>

<method name = "new">
    <argument name = "hash" type = "char *" />
    //
    self->hash = icl_mem_strdup (hash);

    //  Basic password keys are user
    //  Digest password keys are realm:user
    if (strchr (key, ':'))
        self->type = IPR_PASSWD_DIGEST;
    else
        self->type = IPR_PASSWD_BASIC;
</method>

<method name = "destroy">
    icl_mem_free (self->hash);
</method>

<method name = "update" template = "function">
    <doc>
    Forces an update/clean of the password, removing any guard it had.
    </doc>
    <argument name = "hash" type = "char *" />
    <inherit name = "mutex" />
    //
    icl_mem_free (self->hash);
    self->hash = icl_mem_strdup (hash);

    //  The entry is unguarded until it is re-guarded
    self->hits          = 0;
    self->hit_quota     = 0;
    self->misses        = 0;
    self->miss_quota    = 0;
    self->expiry        = 0;
    self->hit_time      = 0;
    self->miss_time     = 0;
    self->hit_from  [0] = 0;
    self->miss_from [0] = 0;
    self->expired   = FALSE;
    self->table_head->dirty = TRUE;
</method>

<method name = "assume" return = "self">
    <doc>
    Create or update entry with specified hash.  Returns reference to entry.
    </doc>
    <argument name = "table" type = "ipr_password_table_t *" />
    <argument name = "key"  type = "char *" />
    <argument name = "hash" type = "char *" />
    <declare name = "self" type = "$(selftype) *" default = "NULL" />
    <inherit name = "mutex" />
    //
    self = ipr_password_table_search (table, key);
    if (self) {
        if (hash && strneq (hash, self->hash))
            ipr_password_update (self, hash);
    }
    else
        self = ipr_password_new (table, key, hash);
</method>

<method name = "guard set" template = "function">
    <doc>
    Sets the guard data for hits, misses, and expiry.  Does not destroy data
    already collected.  Does not change the guard data for expired passwords.
    </doc>
    <argument name = "hit quota" type = "size_t" />
    <argument name = "miss quota" type = "size_t" />
    <argument name = "time to live" type = "size_t" />
    <inherit name = "mutex" />
    //
    if (!self->expired) {
        if (self->hit_quota == 0)
            self->hit_quota = hit_quota;
        if (self->miss_quota == 0)
            self->miss_quota = miss_quota;
        if (self->expiry == 0 && time_to_live)
            self->expiry = apr_time_now () + (time_to_live * 1000 * 1000);
    }
</method>

<method name = "guard get" template = "function">
    <doc>
    Sets the password options from a serialized string, the same format
    as produced by the save method.
    </doc>
    <argument name = "options" type = "char *">name=value ...</argument>
    <inherit name = "mutex" />
    <local>
    ipr_regexp_t
        *regexp;                        //  Regexp for matching each line
    char
        *name = NULL,
        *value = NULL,
        *rest =  NULL;
    </local>
    //
    options = icl_mem_strdup (options);
    regexp = ipr_regexp_new ("^`s*([`w-]+)`s*=`s*([^`s]+)(.*)");
    while (ipr_regexp_match (regexp, options, &name, &value, &rest) == 3) {
        if (streq (name, "expiry"))
            self->expiry = ipr_time_iso8601_decode (value);
        else
        if (streq (name, "expired"))
            self->expired = atoi (value);
        else
        if (streq (name, "hits"))
            self->hits = atoi (value);
        else
        if (streq (name, "hit-quota"))
            self->hit_quota = atoi (value);
        else
        if (streq (name, "hit-time"))
            self->hit_time = ipr_time_iso8601_decode (value);
        else
        if (streq (name, "hit-from"))
            icl_shortstr_cpy (self->hit_from, value);
        else
        if (streq (name, "misses"))
            self->misses = atoi (value);
        else
        if (streq (name, "miss-quota"))
            self->miss_quota = atoi (value);
        else
        if (streq (name, "miss-time"))
            self->miss_time = ipr_time_iso8601_decode (value);
        else
        if (streq (name, "miss-from"))
            icl_shortstr_cpy (self->miss_from, value);

        icl_mem_strfree (&name);
        icl_mem_strfree (&value);
        icl_mem_strfree (&options);
        options = rest;
        rest = NULL;
        self->table_head->dirty = TRUE;
    }
    ipr_regexp_destroy (&regexp);
    icl_mem_free (options);
</method>

<method name = "guard put" template = "function">
    <doc>
    Writes the password to a text file.  The password is serialised in this
    format: '{key}:{proplist}' where proplist is a pair=value list.
    </doc>
    <argument name = "file" type = "FILE *" />
    <inherit name = "mutex" />
    <local>
    icl_shortstr_t
        time_str;
    </local>
    //
    assert (file);
    fprintf (file, "%s:", self->key);
    if (self->expiry) {
        ipr_time_iso8601 (self->expiry, ipr_date_format_minute, 0, TRUE, time_str);
        fprintf (file, " expiry=%s", time_str);
    }
    if (self->expired)
        fprintf (file, " expired=1");
    if (self->hits)
        fprintf (file, " hits=%lu", (unsigned long) self->hits);
    if (self->hit_quota)
        fprintf (file, " hit-quota=%lu", (unsigned long) self->hit_quota);
    if (*self->hit_from)
        fprintf (file, " hit-from=%s", self->hit_from);
    if (self->hit_time) {
        ipr_time_iso8601 (self->hit_time, ipr_date_format_minute, 0, TRUE, time_str);
        fprintf (file, " hit-time=%s", time_str);
    }
    if (self->misses)
        fprintf (file, " misses=%lu", (unsigned long) self->misses);
    if (self->miss_quota)
        fprintf (file, " miss-quota=%lu", (unsigned long) self->miss_quota);
    if (*self->miss_from)
        fprintf (file, " miss-from=%s", self->miss_from);
    if (self->miss_time) {
        ipr_time_iso8601 (self->miss_time, ipr_date_format_minute, 0, TRUE, time_str);
        fprintf (file, " miss-time=%s", time_str);
    }
    fprintf (file, "\\n");
</method>

<method name = "hit" template = "function">
    <doc>
    Updates the password for a successful hit and expires the password if
    it ran out of hit quotas, or passed its expiry time.  We do not reset
    the miss quota even after a successful hit, since that would mask any
    ongoing slow dictionary attack.
    </doc>
    <argument name = "address" type = "char *" />
    <inherit name = "mutex" />
    //
    icl_shortstr_cpy (self->hit_from, address);
    self->hit_time = apr_time_now ();
    self->hits++;

    //  Expire password if the hit quota went down to 1
    //  Ignore the hit quota and expiry if they are already zero
    if ((self->hit_quota && --self->hit_quota == 0)
    ||  (self->expiry && apr_time_now () > self->expiry))
        self->expired = TRUE;

    self->table_head->dirty = TRUE;
</method>

<method name = "miss" template = "function">
    <doc>
    Updates the password for a failed request and expires the password if
    it ran out of miss quotas, or passed its expiry time.
    </doc>
    <argument name = "address" type = "char *" />
    <inherit name = "mutex" />
    //
    icl_shortstr_cpy (self->miss_from, address);
    self->miss_time = apr_time_now ();
    self->misses++;

    //  Expire password if the miss quota went down to 1
    //  Ignore the miss quota and expiry if they are already zero
    if ((self->miss_quota && --self->miss_quota == 0)
    ||  (self->expiry && apr_time_now () > self->expiry))
        self->expired = TRUE;

    self->table_head->dirty = TRUE;
</method>

<method name = "selftest">
    //  selftest is in ipr_password_table
</method>

</class>
