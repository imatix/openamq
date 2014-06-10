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
    name      = "ipr_nonce"
    comment   = "A security token for Digest authentication"
    version   = "1.0"
    script    = "icl_gen"
    >
<doc>
Nonces are held in a hash table.  The nonce value is the key into the
table.  This class generates the nonce value.
</doc>

<inherit class = "icl_object">
    <option name = "alloc" value = "cache" />
    <option name = "links" value = "0" />
</inherit>
<inherit class = "icl_hash_item">
    <option name = "hash_type" value = "str" />
</inherit>

<import class = "ipr_str" />

<context>
    int64_t
        count;                          //  Client nonce count value
    apr_time_t
        expiry;                         //  Calculated expiry time
    size_t
        quota;                          //  Hits remaining
</context>

<method name = "new">
    <argument name = "key" type = "char *">Nonce value or NULL</argument>
    <argument name = "ttl" type = "size_t">Time to live, seconds</argument>
    <argument name = "quota" type = "size_t">Lifespan, in hits</argument>
    <local>
    icl_shortstr_t
        nonce_data;
    char
        *digest;
    </local>
    //
    if (!key) {
        //  nonce is "time:{md5(time:random)}"
        //  This is overkill, a simpler nonce would work as well
        sprintf (nonce_data, "%016llx:", (long long unsigned int) apr_time_now ());
        ipr_str_random (nonce_data + strlen (nonce_data), "Ax16");
        digest = ipr_str_md5 (nonce_data);
        icl_shortstr_fmt (nonce_data + 16, digest);
        icl_mem_free (digest);
        key = nonce_data;
    }
    self->quota = quota;
    self->expiry = apr_time_now () + (ttl * 1000 * 1000);
</method>

<method name = "assume" return = "self">
    <argument name = "table" type = "ipr_nonce_table_t *">Nonce table</argument>
    <argument name = "key" type = "char *">Nonce value</argument>
    <argument name = "ttl" type = "size_t">Time to live, seconds</argument>
    <argument name = "quota" type = "size_t">Lifespan, in hits</argument>
    <declare name = "self" type = "$(selftype) *" />
    <doc>
    Create or update nonce with specified key
    </doc>
    <local>
    ipr_nonce_t
        *nonce;
    </local>
    //
    nonce = ipr_nonce_table_search (table, key);
    if (!nonce)
        nonce = ipr_nonce_new (table, key, ttl, quota);
    return (nonce);
</method>

<method name = "expired" template = "function">
    <doc>
    Tries to consume one request quota from the nonce, and checks the current
    time. If the nonce did not have sufficient quota, or TTL was expired,
    returns TRUE, else returns FALSE.
    </doc>
    //
    rc = FALSE;
    if (self->quota) {
        self->quota--;
        if (apr_time_now () > self->expiry)
            rc = TRUE;
    }
    else 
        rc = TRUE;
</method>

<method name = "destroy">
</method>

<method name = "selftest">
    ipr_nonce_table_t
        *table;
    ipr_nonce_t
        *nonce;

    table = ipr_nonce_table_new ();
    nonce = ipr_nonce_new (table, NULL, 60, 10);
    assert (strlen (nonce->key) == 48);
    ipr_nonce_table_destroy (&table);
</method>

</class>
