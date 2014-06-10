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
    name      = "ipr_password_table"
    comment   = "Password table management"
    version   = "1.0"
    script    = "icl_gen"
    >
<doc>
    This class implements a hash container for ipr_password with the ability
    to load password files in various formats. One table can hold passwords
    of any type (Basic or Digest), the type is verified when the password is
    validated.  See http://tools.ietf.org/html/rfc2617. The class is
    threadsafe and may be used from concurrent threads.
</doc>

<inherit class = "icl_hash_head" />

<option name = "mutex"      value = "1" />
<option name = "links"      value = "1" />
<option name = "prefix"     value = "table"/>
<option name = "childname"  value = "ipr_password" />
<option name = "childtype"  value = "ipr_password_t" />
<option name = "hash_size"  value = "65535" />
<option name = "hash_type"  value = "str" />
<option name = "link_to_child" value = "0" />

<import class = "ipr_password" />
<import class = "ipr_nonce" />
<import class = "ipr_str" />
<import class = "ipr_file" />
<import class = "ipr_dict" />

<public name = "header">
#define IPR_PASSWORD_OK             0
#define IPR_PASSWORD_CHALLENGE      1   //  No credentials provided
#define IPR_PASSWORD_NOTFOUND       2   //  No such user[:realm]
#define IPR_PASSWORD_EXPIRED        3   //  Password expired
#define IPR_PASSWORD_INCORRECT      4   //  Credentials are wrong
#define IPR_PASSWORD_BADTYPE        5   //  Wrong mechanism
#define IPR_PASSWORD_SYNTAX         6   //  Credential syntax bogus
#define IPR_PASSWORD_HOSTILE        7   //  Credentials look hostile
#define IPR_PASSWORD_TRYAGAIN       8   //  Client should try again
</public>

<context>
    ipr_password_list_t
        *list;                          //  List of all password items
    char
        *file_name;                     //  If data was loaded from file
    int64_t
        file_size;                      //  Size of loaded file
    apr_time_t
        file_time;                      //  Time of loaded file
    Bool
        debug,                          //  Yes to get debug output
        dirty;                          //  Guard data changed
    uint
        result;                         //  IPR_PASSWORD_something
    icl_shortstr_t
        username,                       //  User name if known
        realm;                          //  Realm, if known
    size_t
        hit_quota,                      //  Guard properties
        miss_quota,
        time_to_live;
    ipr_nonce_table_t
        *nonces;                        //  Table of nonces, if needed
    size_t
        nonce_ttl,                      //  Time to live for new nonces
        nonce_quota;                    //  Hit quota for new nonces
    char
        *nonce_test;                    //  Force nonce value for tests
</context>

<method name = "new">
    <argument name = "filename" type = "char *">Name of associated file</argument>
    //
    self->list = ipr_password_list_new ();
    self->nonce_ttl = 3600;             //  Default values
    self->nonce_quota = 256;
    if (filename) {
        self->file_name = icl_mem_strdup (filename);
        self_load (self, filename);
    }
</method>

<method name = "destroy">
    icl_mem_free (self->file_name);
    ipr_password_list_destroy (&self->list);
    ipr_nonce_table_destroy (&self->nonces);
</method>

<method name = "insert">
    ipr_password_list_queue (self->list, item);
    assert (ipr_password_list_count (self->list) == self->nbr_items);
</method>

<method name = "remove">
    ipr_password_list_remove (item);
</method>

<method name = "search digest" return = "password">
    <doc>
    Searches for an entry using username and realm, the key for digest
    passwords being "username:realm" rather than "username" as for basic
    passwords.
    </doc>
    <argument name = "self" type = "$(selftype) *" />
    <argument name = "username" type = "char *" />
    <argument name = "realm" type = "char *" />
    <declare name = "password" type = "ipr_password_t *" />
    <local>
    icl_shortstr_t
        key;
    </local>
    //
    icl_shortstr_fmt (key, "%s:%s", username, realm);
    password = self_search (self, key);
</method>

<method name = "load" template = "function">
    <doc>
    Loads a password file into the table, merging the loaded passwords with
    any passwords already in the table.  Password files are in the format
    produced by the Apache htpasswd or htdigest tools.  Each line contains
    {key}:{hash}, where {key} consists of one or more words separated by ":".
    Stores the loaded file's time and size, and returns the number of entries
    loaded from the file.  If the file could not be found, returns -1. Lines
    are limited to 4096 bytes, keys to 255 characters.  Existing entries are
    updated it their hash has changed, else left unchanged.
    </doc>
    <argument name = "filename" type = "char *">Name of associated file</argument>
    <inherit name = "mutex" />
    <local>
#   define IPR_PASSWD_LINE_MAX    4096
    FILE
        *file;
    char
        *line;
    ipr_regexp_t
        *regexp;                        //  Regexp for matching each line
    char
        *key = NULL,
        *hash = NULL;
    ipr_password_t
        *password;
    </local>
    //
    assert (filename);
    file = fopen (filename, "r");
    if (file) {
        //                        key                     hash
        //                         |                        |
        regexp = ipr_regexp_new ("^((?:[^: ]+)(?::[^: ]+)?):([^`s]+)");
        line = icl_mem_alloc (IPR_PASSWD_LINE_MAX + 1);
        while (fgets (line, IPR_PASSWD_LINE_MAX, file) != NULL) {
            if (ipr_regexp_match (regexp, line, &key, &hash) == 2) {
                password = ipr_password_table_search (self, key);
                if (password) {
                    if (strneq (password->hash, hash)) {
                        ipr_password_update (password, hash);
                        rc++;
                    }
                }
                else {
                    password = ipr_password_new (self, key, hash);
                    ipr_password_guard_set (password,
                        self->hit_quota, self->miss_quota, self->time_to_live);
                    rc++;
                }
                icl_mem_strfree (&key);
                icl_mem_strfree (&hash);
            }
        }
        ipr_regexp_destroy (&regexp);
        icl_mem_free (line);
        fclose (file);

        icl_mem_free (self->file_name);
        self->file_name = icl_mem_strdup (filename);
        self->file_size = ipr_file_size (self->file_name);
        self->file_time = ipr_file_time (self->file_name);
    }
    else
        rc = -1;
</method>

<method name = "save" template = "function">
    <doc>
    Saves the table to a text file, one entry per line.  If file name is null
    the saved password filename is used.  Returns the number of entries
    written.  If there was an error writing to the file, returns -1.  This
    method is UNSAFE if external tools also modify the password file.
    </doc>
    <inherit name = "mutex" />
    <local>
    FILE
        *file;
    char
        *tmpname;
    ipr_password_t
        *password;
    </local>
    //
    //  Save to temporary file and switch when done, to avoid data loss if
    //  we crash part way through saving (disk full, etc.)
    assert (self->file_name);
    tmpname = ipr_file_tmpfile ();
    file = fopen (tmpname, "w");
    if (file) {
        password = ipr_password_list_first (self->list);
        while (password) {
            fprintf (file, "%s:%s\\n", password->key, password->hash);
            password = ipr_password_list_next (&password);
            rc++;
        }
        fclose (file);
        ipr_file_rename (self->file_name, tmpname);
        icl_mem_free (tmpname);
        self->file_size = ipr_file_size (self->file_name);
        self->file_time = ipr_file_time (self->file_name);
    }
    else
        rc = -1;
</method>

<method name = "sync" template = "function">
    <doc>
    Reloads a password table from a disk file, if the disk file changed.
    Returns number of items changed, or -1 if the file could not be read.
    </doc>
    //
    if (ipr_file_size (self->file_name) != self->file_size
    ||  ipr_file_time (self->file_name) != self->file_time)
        rc = ipr_password_table_load (self, self->file_name);
</method>

<method name = "guard set" template = "function">
    <doc>
    Sets the password guard policy for all passwords in the table.  Should
    be invoked after loading a new password table for the first time.
    </doc>
    <argument name = "hit quota" type = "size_t" />
    <argument name = "miss quota" type = "size_t" />
    <argument name = "time to live" type = "size_t" />
    <inherit name = "mutex" />
    <local>
    ipr_password_t
        *password;
    </local>
    //
    self->hit_quota = hit_quota;
    self->miss_quota = miss_quota;
    self->time_to_live = time_to_live;
    password = ipr_password_list_first (self->list);
    while (password) {
        ipr_password_guard_set (password, hit_quota, miss_quota, time_to_live);
        password = ipr_password_list_next (&password);
    }
</method>

<method name = "guard load" template = "function">
    <doc>
    Loads password guard data, if any, for the password table.  The guard
    data is in a file with the same name as the password table file, with
    the string "_guard" following the file name.  Each line in the file is
    of the format {key}:{proplist} where proplist is a list of name=value
    pairs.
    </doc>
    <inherit name = "mutex" />
    <local>
    icl_shortstr_t
        guard_file;
    FILE
        *file;
    char
        *line;
    ipr_regexp_t
        *regexp;                        //  Regexp for matching each line
    char
        *key = NULL,
        *proplist = NULL;
    ipr_password_t
        *password;
    </local>
    //
    icl_shortstr_fmt (guard_file, "%s_guard", self->file_name);
    file = fopen (guard_file, "r");
    if (file) {
        //                        key                     proplist
        //                         |                        |
        regexp = ipr_regexp_new ("^((?:[^: ]+)(?::[^: ]+)?):(.*)");
        line = icl_mem_alloc (IPR_PASSWD_LINE_MAX + 1);
        while (fgets (line, IPR_PASSWD_LINE_MAX, file) != NULL) {
            if (ipr_regexp_match (regexp, line, &key, &proplist) == 2) {
                password = ipr_password_table_search (self, key);
                if (password)
                    ipr_password_guard_get (password, proplist);
                icl_mem_strfree (&key);
                icl_mem_strfree (&proplist);
                rc++;
            }
        }
        ipr_regexp_destroy (&regexp);
        icl_mem_free (line);
        fclose (file);
        self->dirty = FALSE;
    }
</method>

<method name = "guard save" template = "function">
    <doc>
    Saves the table's guard data to a text file, one entry per line.
    </doc>
    <inherit name = "mutex" />
    <local>
    icl_shortstr_t
        guard_file;
    FILE
        *file;
    char
        *tmpname;
    ipr_password_t
        *password;
    </local>
    //
    //  Save to temporary file and switch when done, to avoid data loss if
    //  we crash part way through saving (disk full, etc.)
    tmpname = ipr_file_tmpfile ();
    file = fopen (tmpname, "w");
    if (file) {
        password = ipr_password_list_first (self->list);
        while (password) {
            ipr_password_guard_put (password, file);
            password = ipr_password_list_next (&password);
        }
        fclose (file);
        icl_shortstr_fmt (guard_file, "%s_guard", self->file_name);
        ipr_file_rename (guard_file, tmpname);
        icl_mem_free (tmpname);
        self->dirty = FALSE;
    }
    else
        rc = -1;
</method>

<method name = "guard sync" template = "function">
    <doc>
    Saves the guard data for the table if the table was 'dirty', i.e. if any
    password has modified guard data.
    </doc>
    //
    if (self->dirty)
        ipr_password_table_guard_save (self);
</method>


<method name = "basic" return = "challenge">
    <doc>
    Authenticates using the Basic authentication mechanism defined in RFC
    2617. Accepts a response (the content of the HTTP Authorization header,
    which should start with "Basic "), and returns a challenge (sent back as
    the WWW-Authenticate header), or NULL if the authentication succeeded.
    Sets self->username to the resolved user name, if any, and self->result
    to 0 or an IPR_PASSWORD_xyz error code.  Allocates a new challenge string
    if needed, and the caller should free it when done.  The address is used
    for logging, and may be null.
    </doc>
    <argument name = "self" type = "$(selftype) *">Reference to self</argument>
    <argument name = "address"  type = "char *">IP address of caller</argument>
    <argument name = "realm"    type = "char *">Authentication realm</argument>
    <argument name = "response" type = "char *">Authentication response</argument>
    <declare name = "challenge" type = "char *" default = "NULL" />
    <inherit name = "mutex" />
    <local>
    char
        *decoded,                       //  Start of decoded response
        *password;                      //  After ':' in decoded response
    ipr_password_t
        *entry;                         //  Password entry in table
    </local>
    //
    strclr (self->username);
    strclr (self->realm);
    self->result = IPR_PASSWORD_SYNTAX;
    response = ipr_str_defix (response, "Basic ");
    if (response) {
        decoded = icl_mem_alloc (apr_base64_decode_len (response) + 1);
        apr_base64_decode (decoded, response);
        password = strchr (decoded, ':');

        if (password) {
            *password++ = 0;            //  Split password off decoded
            icl_shortstr_cpy (self->username, decoded);
            icl_shortstr_cpy (self->realm, realm);
            entry = ipr_password_table_search (self, self->username);
            if (entry) {
                if (entry->expired) {
                    ipr_password_miss (entry, address);
                    self->result = IPR_PASSWORD_EXPIRED;
                }
                else
                if (entry->type == IPR_PASSWD_BASIC) {
                    //  Do a check through APR's algorithms, and as plain text
                    if (apr_password_validate (password, entry->hash) == 0
                    ||  streq (password, entry->hash)) {
                        ipr_password_hit (entry, address);
                        self->result = IPR_PASSWORD_OK;
                    }
                    else {
                        ipr_password_miss (entry, address);
                        self->result = IPR_PASSWORD_INCORRECT;
                    }
                }
                else {
                    ipr_password_miss (entry, address);
                    self->result = IPR_PASSWORD_BADTYPE;
                }
            }
            else
                self->result = IPR_PASSWORD_NOTFOUND;
        }
        icl_mem_free (decoded);
    }
    else
        self->result = IPR_PASSWORD_CHALLENGE;

    if (self->result != IPR_PASSWORD_OK)
        challenge = ipr_str_format ("Basic realm=\\"%s\\"", realm);
</method>

<method name = "digest" return = "challenge">
    <doc>
    Authenticates using the Digest authentication mechanism defined in RFC
    2617. Accepts a response (the content of the HTTP Authorization header,
    which should start with "Digest "), and returns a challenge (sent back as
    the WWW-Authenticate header), or NULL if the authentication succeeded.
    Sets self->username to the resolved user name, if any, and self->result
    to 0 or an IPR_PASSWORD_xyz error code.  Allocates a new challenge string
    if needed, and the caller should free it when done.  The address is used
    </doc>
    <argument name = "self"     type = "$(selftype) *">Reference to self</argument>
    <argument name = "address"  type = "char *">IP address of caller</argument>
    <argument name = "realm"    type = "char *">Authentication realm</argument>
    <argument name = "response" type = "char *">Authentication response</argument>
    <argument name = "method"   type = "char *">HTTP request method</argument>
    <argument name = "uri"      type = "char *">HTTP request URI</argument>
    <declare name = "challenge" type = "char *" default = "NULL" />
    <inherit name = "mutex" />
    <local>
    ipr_dict_table_t
        *fields;                        //  For parsing the credentials
    char
        *auth_username,                 //  Authorization credentials
        *auth_realm,                    //    broken into individual pieces
        *auth_nonce,
        *auth_uri,
        *auth_qop,
        *auth_count,
        *auth_cnonce,
        *auth_response;
    ipr_nonce_t
        *nonce;                         //  Security nonce
    ipr_password_t
        *entry;                         //  User name entry in passwd file
    </local>
    //
    if (self->nonces == NULL)
        self->nonces = ipr_nonce_table_new ();

    //  Check Authorization: header for Digest credentials
    strclr (self->username);
    strclr (self->realm);
    self->result = IPR_PASSWORD_SYNTAX;
    response = ipr_str_defix (response, "Digest ");
    if (response) {
        //  Break credentials into distinct pieces
        fields = ipr_dict_table_new ();
        ipr_dict_table_props_load (fields, response);
        auth_username = ipr_dict_table_lookup (fields, "username");
        auth_realm    = ipr_dict_table_lookup (fields, "realm");
        auth_nonce    = ipr_dict_table_lookup (fields, "nonce");
        auth_uri      = ipr_dict_table_lookup (fields, "uri");
        auth_qop      = ipr_dict_table_lookup (fields, "qop");
        auth_count    = ipr_dict_table_lookup (fields, "nc");
        auth_cnonce   = ipr_dict_table_lookup (fields, "cnonce");
        auth_response = ipr_dict_table_lookup (fields, "response");

        icl_shortstr_cpy (self->username, auth_username);
        icl_shortstr_cpy (self->realm, realm);

        //  Lookup nonce and password entry
        nonce = ipr_nonce_table_search (self->nonces, auth_nonce);
        entry = ipr_password_table_search_digest (self, self->username, realm);

        if (!entry) {
            self->result = IPR_PASSWORD_NOTFOUND;
            if (self->debug)
                icl_console_print ("P: no password for '%s:%s'", self->username, realm);
        }
        else
        if (entry->expired) {
            ipr_password_miss (entry, address);
            self->result = IPR_PASSWORD_EXPIRED;
            if (self->debug)
                icl_console_print ("P: '%s:%s' password has expired", self->username, realm);
        }
        else
        if (!nonce || ipr_nonce_expired (nonce)) {
            self->result = IPR_PASSWORD_TRYAGAIN;
            ipr_nonce_destroy (&nonce);
            if (self->debug)
                icl_console_print ("P: credentials used an expired nonce");
        }
        else
        //  Possible man-in-the-middle attacks, or proxy gone bad
        //  These could be caused by poorly coded clients, but that
        //  is marginally likely these days
        if (strneq (auth_uri, uri)
        ||  strneq (auth_realm, realm)
        ||  strneq (auth_qop, "auth")
        ||  strnull (auth_cnonce)
        ||  nonce->count >= (int64_t) strtoull (auth_count, NULL, 16)) {
            ipr_password_miss (entry, address);
            self->result = IPR_PASSWORD_HOSTILE;
            if (self->debug)
                icl_console_print ("P: credentials are badly formatted");
        }
        else {
            char
                *HA2,                       //  MD5 (method : uri)
                *HA3;                       //  Calculated response

            //  Format HA2 = md5 (method ":" uri)
            HA2 = ipr_str_md5 ("%s:%s", method, uri);
            HA3 = ipr_str_md5 ("%s:%s:%s:%s:%s:%s",
                entry->hash, nonce->key, auth_count, auth_cnonce, "auth", HA2);
            if (self->debug) {
                icl_console_print ("P: hash for '%s:%s' is %s", self->username, realm, entry->hash);
                icl_console_print ("P: realm=%s method=%s uri=%s qop=auth nonce=%s nc=%s cnonce=%s",
                    realm, method, uri, nonce->key, auth_count, auth_cnonce);
                icl_console_print ("P: response=%s HA2=%s HA3=%s", auth_response, HA2, HA3);
            }
            if (streq (HA3, auth_response)) {
                ipr_password_hit (entry, address);
                self->result = IPR_PASSWORD_OK;
            }
            else {
                ipr_password_miss (entry, address);
                self->result = IPR_PASSWORD_INCORRECT;
            }
            icl_mem_free (HA2);
            icl_mem_free (HA3);
        }
        ipr_dict_table_destroy (&fields);
    }
    else
        self->result = IPR_PASSWORD_CHALLENGE;

    if (self->result != IPR_PASSWORD_OK) {
        if (!self->nonce_test)
            nonce = ipr_nonce_new (self->nonces, NULL, self->nonce_ttl, self->nonce_quota);

        challenge = ipr_str_format (
            "Digest realm=\\"%s\\", qop=\\"auth\\", algorithm=\\"MD5\\", nonce=\\"%s\\"%s",
            realm, self->nonce_test? self->nonce_test: nonce->key,
            response? ", stale=\\"true\\"": "");
    }
</method>

<method name = "set nonce ttl" template = "function">
    <doc>
    Sets the time-to-live for new nonces, in seconds.  By default, nonces
    expire after one hour (ttl = 3600).  If ttl is zero, new nonces are
    granted infinite time to live.
    </doc>
    <argument name = "ttl" type = "size_t">Time to live, seconds</argument>
    <inherit name = "mutex" />
    //
    self->nonce_ttl = ttl;
</method>

<method name = "set nonce quota" template = "function">
    <doc>
    Sets the lifespan for new nonces, as a number of hits.  By default,
    nonces expire after 256 hits.  If the quota is zero, new nonces are
    granted infinite hits.
    </doc>
    <argument name = "quota" type = "size_t">Lifespan, in hits</argument>
    <inherit name = "mutex" />
    //
    self->nonce_quota = quota;
</method>

<method name = "set nonce test" template = "function">
    <doc>
    Sets the test nonce value: if set, there will only be once nonce with
    this precise value.
    </doc>
    <argument name = "value" type = "char *">Test nonce value</argument>
    <inherit name = "mutex" />
    //
    if (self->nonces == NULL)
        self->nonces = ipr_nonce_table_new ();
    self->nonce_test = value;
    ipr_nonce_assume (self->nonces, self->nonce_test, self->nonce_ttl, self->nonce_quota);
</method>

<method name = "set debug" template = "function">
    <doc>
    Sets the debug option, causes detailed output of the authentication methods.
    </doc>
    <argument name = "value" type = "Bool">Enable debug?</argument>
    <inherit name = "mutex" />
    //
    self->debug = value;
</method>

<method name = "selftest">
    <local>
    ipr_password_table_t
        *table;
    size_t
        nbr_items;
    ipr_password_t
        *password;
    char
        *challenge,
        *response;
    </local>
    //
    //  Simple tests of password management
    table = ipr_password_table_new (NULL);
    password = ipr_password_assume (table, "Aladdin", "hash");
    assert (password->type == IPR_PASSWD_BASIC);
    assert (streq (password->hash, "hash"));
    assert (ipr_password_table_search (table, "Aladdin"));
    assert (ipr_password_table_search (table, "Unknown") == NULL);
    ipr_password_destroy (&password);
    assert (ipr_password_table_search (table, "Aladdin") == NULL);
    ipr_password_table_destroy (&table);

    //  Test file load/sync/save functions
    ipr_file_delete ("nosuch");
    table = ipr_password_table_new ("nosuch");
    assert (table->nbr_items == 0);
    ipr_password_table_destroy (&table);

    ipr_file_delete ("test_password");
    table = ipr_password_table_new ("test_password");
    assert (ipr_password_table_load (table, "nosuch") == -1);
    ipr_password_assume (table, "Aladdin-md5", "$apr1$Uibxh...$Ud.6/BCLoSHsscd08Vj8q1");
    ipr_password_assume (table, "Aladdin-sha", "{SHA}W8r/fyL/UzygmbNAjq2HbA67qac=");
    ipr_password_assume (table, "Aladdin-crypt", "yyih1EpzpWg46");
    ipr_password_assume (table, "Aladdin-plain", "open sesame");
    nbr_items = table->nbr_items;
    assert (ipr_password_table_save (table) == nbr_items);
    ipr_password_table_destroy (&table);

    table = ipr_password_table_new ("test_password");
    assert (ipr_password_table_sync (table) == 0);
    assert (table->nbr_items == nbr_items);
    ipr_file_delete ("test_password");
    assert (ipr_password_table_sync (table) == -1);
    assert (table->nbr_items == nbr_items);
    ipr_password_table_destroy (&table);

    //  Test guard methods
    table = ipr_password_table_new ("test_password");
    ipr_password_assume (table, "Aladdin-1", "password1");
    ipr_password_assume (table, "Aladdin-2", "password2");
    assert (table->nbr_items == 2);
    assert (table->dirty == FALSE);
    ipr_password_table_guard_set (table, 256, 16, 3600);
    table->dirty = TRUE;
    ipr_password_table_guard_save (table);
    assert (ipr_password_table_save (table) == 2);
    assert (table->dirty == FALSE);
    ipr_password_table_destroy (&table);

    table = ipr_password_table_new ("test_password");
    assert (table->nbr_items == 2);
    ipr_password_table_guard_load (table);
    password = ipr_password_table_search (table, "Aladdin-1");
    assert (password);
    assert (password->hit_quota == 256);
    assert (password->miss_quota == 16);
    ipr_password_table_destroy (&table);

    //  Test Basic authentication, using the example from RFC 2617
    //
    table = ipr_password_table_new (NULL);

    challenge = ipr_password_table_basic (table, "127.0.0.1", "WallyWorld", NULL);
    assert (challenge);
    assert (streq (challenge, "Basic realm=\\"WallyWorld\\""));
    icl_mem_free (challenge);

    ipr_password_assume (table, "Aladdin", "$apr1$Uibxh...$Ud.6/BCLoSHsscd08Vj8q1");
    challenge = ipr_password_table_basic (
        table, "127.0.0.1", "WallyWorld", "Basic QWxhZGRpbjpvcGVuIHNlc2FtZQ==");
    assert (challenge == NULL);
    assert (streq (table->username, "Aladdin"));

    ipr_password_assume (table, "Aladdin", "{SHA}W8r/fyL/UzygmbNAjq2HbA67qac=");
    challenge = ipr_password_table_basic (
        table, "127.0.0.1", "WallyWorld", "Basic QWxhZGRpbjpvcGVuIHNlc2FtZQ==");
    assert (challenge == NULL);
    assert (streq (table->username, "Aladdin"));

#   if defined (__UNIX__)
    //  APR only supports crypt hash on Unix/Linux systems
    ipr_password_assume (table, "Aladdin", "yyih1EpzpWg46");
    challenge = ipr_password_table_basic (
        table, "127.0.0.1", "WallyWorld", "Basic QWxhZGRpbjpvcGVuIHNlc2FtZQ==");
    assert (challenge == NULL);
    assert (streq (table->username, "Aladdin"));
#   endif

    ipr_password_assume (table, "Aladdin", "open sesame");
    challenge = ipr_password_table_basic (
        table, "127.0.0.1", "WallyWorld", "Basic QWxhZGRpbjpvcGVuIHNlc2FtZQ==");
    assert (challenge == NULL);
    assert (streq (table->username, "Aladdin"));

    ipr_password_assume (table, "Aladdin", "bad password");
    challenge = ipr_password_table_basic (
        table, "127.0.0.1", "WallyWorld", "Basic QWxhZGRpbjpvcGVuIHNlc2FtZQ==");
    assert (challenge);
    assert (streq (challenge, "Basic realm=\\"WallyWorld\\""));
    assert (table->result == IPR_PASSWORD_INCORRECT);
    assert (streq (table->username, "Aladdin"));
    icl_mem_free (challenge);

    password = ipr_password_table_search (table, "Aladdin");
    ipr_password_destroy (&password);
    challenge = ipr_password_table_basic (
        table, "127.0.0.1", "WallyWorld", "Basic QWxhZGRpbjpvcGVuIHNlc2FtZQ==");
    assert (challenge);
    assert (streq (challenge, "Basic realm=\\"WallyWorld\\""));
    assert (table->result == IPR_PASSWORD_NOTFOUND);
    icl_mem_free (challenge);

    //  Test Digest authentication, using the example from RFC 2617
    //
    //  These methods don't report much, just test they are stable
    ipr_password_table_set_nonce_ttl   (table, 30);
    ipr_password_table_set_nonce_quota (table, 16);

    //  Check challenge if we don't supply any credentials
    challenge = ipr_password_table_digest (
        table, "127.0.0.1", "testrealm@host.com", NULL, "GET", "/dir/index.html");
    assert (challenge);
    assert (ipr_str_prefixed (challenge, "Digest realm="));
    assert (table->result == IPR_PASSWORD_CHALLENGE);
    icl_mem_free (challenge);

    password = ipr_password_assume (table, "Mufasa:testrealm@host.com", "939e7578ed9e3c518a452acee763bce9");
    assert (password->type == IPR_PASSWD_DIGEST);

    //  Force this nonce for the test case
    ipr_password_table_set_nonce_test (table, "dcd98b7102dd2f0e8b11d0f600bfb0c093");

    //  This is the Authorization header we expect from the browser
    response = "Digest username=\\"Mufasa\\", " \\
               "realm=\\"testrealm@host.com\\", " \\
               "nonce=\\"dcd98b7102dd2f0e8b11d0f600bfb0c093\\", " \\
               "uri=\\"/dir/index.html\\", " \\
               "qop=auth, " \\
               "nc=00000001, " \\
               "cnonce=\\"0a4f113b\\", " \\
               "response=\\"6629fae49393a05397450978507c4ef1\\", " \\
               "opaque=\\"5ccc069c403ebaf9f0171e9517f40e41\\"",

    challenge = ipr_password_table_digest (
        table, "127.0.0.1", "testrealm@host.com", response, "GET", "/dir/index.html");
    assert (challenge == NULL);
    assert (streq (table->username, "Mufasa"));

    //  Try various mangled Authorization headers
    //  Bad user name
    response = "Digest username=\\"Mustafa\\", " \\
               "realm=\\"testrealm@host.com\\", " \\
               "nonce=\\"dcd98b7102dd2f0e8b11d0f600bfb0c093\\", " \\
               "uri=\\"/dir/index.html\\", " \\
               "qop=auth, " \\
               "nc=00000001, " \\
               "cnonce=\\"0a4f113b\\", " \\
               "response=\\"6629fae49393a05397450978507c4ef1\\", " \\
               "opaque=\\"5ccc069c403ebaf9f0171e9517f40e41\\"",
    challenge = ipr_password_table_digest (
        table, "127.0.0.1", "testrealm@host.com", response, "GET", "/dir/index.html");
    assert (challenge);
    assert (table->result == IPR_PASSWORD_NOTFOUND);
    icl_mem_free (challenge);

    //  URI not accurate
    response = "Digest username=\\"Mufasa\\", " \\
               "realm=\\"testrealm@host.com\\", " \\
               "nonce=\\"dcd98b7102dd2f0e8b11d0f600bfb0c093\\", " \\
               "uri=\\"/dir/index.htm\\", " \\
               "qop=auth, " \\
               "nc=00000001, " \\
               "cnonce=\\"0a4f113b\\", " \\
               "response=\\"6629fae49393a05397450978507c4ef1\\", " \\
               "opaque=\\"5ccc069c403ebaf9f0171e9517f40e41\\"",
    challenge = ipr_password_table_digest (
        table, "127.0.0.1", "testrealm@host.com", response, "GET", "/dir/index.html");
    assert (challenge);
    assert (table->result == IPR_PASSWORD_HOSTILE);
    icl_mem_free (challenge);

    //  Realm does not match
    response = "Digest username=\\"Mufasa\\", " \\
               "realm=\\"testrealm@hosts.com\\", " \\
               "nonce=\\"dcd98b7102dd2f0e8b11d0f600bfb0c093\\", " \\
               "uri=\\"/dir/index.html\\", " \\
               "qop=auth, " \\
               "nc=00000001, " \\
               "cnonce=\\"0a4f113b\\", " \\
               "response=\\"6629fae49393a05397450978507c4ef1\\", " \\
               "opaque=\\"5ccc069c403ebaf9f0171e9517f40e41\\"",
    challenge = ipr_password_table_digest (
        table, "127.0.0.1", "testrealm@host.com", response, "GET", "/dir/index.html");
    assert (challenge);
    assert (table->result == IPR_PASSWORD_HOSTILE);
    icl_mem_free (challenge);

    //  Nonce has changed
    response = "Digest username=\\"Mufasa\\", " \\
               "realm=\\"testrealm@host.com\\", " \\
               "nonce=\\"dcd98b7102dd2f0e8b11d0f600bfb0c0931\\", " \\
               "uri=\\"/dir/index.html\\", " \\
               "qop=auth, " \\
               "nc=00000001, " \\
               "cnonce=\\"0a4f113b\\", " \\
               "response=\\"6629fae49393a05397450978507c4ef1\\", " \\
               "opaque=\\"5ccc069c403ebaf9f0171e9517f40e41\\"",
    challenge = ipr_password_table_digest (
        table, "127.0.0.1", "testrealm@host.com", response, "GET", "/dir/index.html");
    assert (challenge);
    assert (table->result == IPR_PASSWORD_TRYAGAIN);
    icl_mem_free (challenge);

    //  QoP is incorrect
    response = "Digest username=\\"Mufasa\\", " \\
               "realm=\\"testrealm@host.com\\", " \\
               "nonce=\\"dcd98b7102dd2f0e8b11d0f600bfb0c093\\", " \\
               "uri=\\"/dir/index.html\\", " \\
               "qop=auth-int, " \\
               "nc=00000001, " \\
               "cnonce=\\"0a4f113b\\", " \\
               "response=\\"6629fae49393a05397450978507c4ef1\\", " \\
               "opaque=\\"5ccc069c403ebaf9f0171e9517f40e41\\"",
    challenge = ipr_password_table_digest (
        table, "127.0.0.1", "testrealm@host.com", response, "GET", "/dir/index.html");
    assert (challenge);
    assert (table->result == IPR_PASSWORD_HOSTILE);
    icl_mem_free (challenge);

    //  NC has not changed
    response = "Digest username=\\"Mufasa\\", " \\
               "realm=\\"testrealm@host.com\\", " \\
               "nonce=\\"dcd98b7102dd2f0e8b11d0f600bfb0c093\\", " \\
               "uri=\\"/dir/index.html\\", " \\
               "qop=auth, " \\
               "nc=00000000, " \\
               "cnonce=\\"0a4f113b\\", " \\
               "response=\\"6629fae49393a05397450978507c4ef1\\", " \\
               "opaque=\\"5ccc069c403ebaf9f0171e9517f40e41\\"",
    challenge = ipr_password_table_digest (
        table, "127.0.0.1", "testrealm@host.com", response, "GET", "/dir/index.html");
    assert (challenge);
    assert (table->result == IPR_PASSWORD_HOSTILE);
    icl_mem_free (challenge);

    //  cnonce not specified
    response = "Digest username=\\"Mufasa\\", " \\
               "realm=\\"testrealm@host.com\\", " \\
               "nonce=\\"dcd98b7102dd2f0e8b11d0f600bfb0c093\\", " \\
               "uri=\\"/dir/index.html\\", " \\
               "qop=auth, " \\
               "nc=00000001, " \\
               "response=\\"6629fae49393a05397450978507c4ef1\\", " \\
               "opaque=\\"5ccc069c403ebaf9f0171e9517f40e41\\"",
    challenge = ipr_password_table_digest (
        table, "127.0.0.1", "testrealm@host.com", response, "GET", "/dir/index.html");
    assert (challenge);
    assert (table->result == IPR_PASSWORD_HOSTILE);
    icl_mem_free (challenge);

    //  Bad response hash
    response = "Digest username=\\"Mufasa\\", " \\
               "realm=\\"testrealm@host.com\\", " \\
               "nonce=\\"dcd98b7102dd2f0e8b11d0f600bfb0c093\\", " \\
               "uri=\\"/dir/index.html\\", " \\
               "qop=auth, " \\
               "nc=00000001, " \\
               "cnonce=\\"0a4f113b\\", " \\
               "response=\\"7629fae49393a05397450978507c4ef1\\", " \\
               "opaque=\\"5ccc069c403ebaf9f0171e9517f40e41\\"",
    challenge = ipr_password_table_digest (
        table, "127.0.0.1", "testrealm@host.com", response, "GET", "/dir/index.html");
    assert (challenge);
    assert (table->result == IPR_PASSWORD_INCORRECT);
    assert (streq (table->username, "Mufasa"));
    icl_mem_free (challenge);

    ipr_password_table_destroy (&table);
</method>

</class>
