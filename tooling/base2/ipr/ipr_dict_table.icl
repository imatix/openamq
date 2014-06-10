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
    name      = "ipr_dict_table"
    comment   = "Implements the hash table container for ipr_dict"
    version   = "1.0"
    script    = "icl_gen"
    >
<doc>
    This class implements the hash table container for ipr_dict.  Note
    that this class is not thread safe and uses no rwlocks.  The table
    allows linking for reference counting.
</doc>

<inherit class = "icl_hash_head" />

<option name = "links"      value = "1" />
<option name = "prefix"     value = "table"/>
<option name = "childname"  value = "ipr_dict" />
<option name = "childtype"  value = "ipr_dict_t" />
<option name = "hash_size"  value = "65535" />
<option name = "hash_type"  value = "str" />
<option name = "link_to_child" value = "0" />

<import class = "ipr_dict" />
<import class = "ipr_bucket" />
<import class = "ipr_str" />
<import class = "ipr_http" />

<context>
    ipr_dict_list_t
        *list;                          //  List of all dictionary items
    char
        *file_name;                     //  If data was loaded from file
    int64_t
        file_size;                      //  Size of loaded file
    apr_time_t
        file_time;                      //  Time of loaded file
</context>

<method name = "new">
    self->list = ipr_dict_list_new ();
</method>

<method name = "destroy">
    ipr_dict_list_destroy (&self->list);
    icl_mem_free (self->file_name);
</method>

<method name = "insert">
    ipr_dict_list_queue (self->list, item);
    assert (ipr_dict_list_count (self->list) == self->nbr_items);
</method>

<method name = "remove">
    ipr_dict_list_remove (item);
</method>

<method name = "lookup" return = "value">
    <doc>
    Returns the value for the specified field.  If the field was not
    present in the table, returns an empty string.
    </doc>
    <argument name = "self" type = "$(selftype) *">Reference to self</argument>
    <argument name = "name" type = "char *">Field to look for</argument>
    <declare name = "value" type = "char *">Value of header field</declare>
    <local>
    ipr_dict_t
        *item;                        //  Field item in dictionary, if any
    </local>
    //
    assert (name);
    item = ipr_dict_table_search (self, name);
    value = item? item->value: "";
</method>

<method name = "matches" template = "function">
    <doc>
    Returns TRUE if the specified item exists and matches the supplied regular
    expression, FALSE if the item does not exist or exists but does not match
    the specified expression.
    </doc>
    <argument name = "name"  type = "char *">Name of item to look for</argument>
    <argument name = "match" type = "char *">Regular expression to check</argument>
    <local>
    ipr_dict_t
        *item;                          //  Symbol in dictionary
    </local>
    //
    item = self_search (self, name);
    if (item)
        rc = ipr_regexp_eq (match, item->value);
    else
        rc = FALSE;
</method>

<method name = "template" return = "result">
    <doc>
    Performs symbol substitution into a specified string. Returns a
    newly-allocated string containing the result.  The dictionary holds
    the set of symbols that may be inserted.  Undefined symbols are
    inserted as an empty value. Symbols are specified in the string
    using this syntax: $name, where 'name' is case-sensitive. The name
    can be terminated by a backslash.  $\ is replaced by $.  Backslashes
    elsewhere are not modified.  The name must consist of letters,
    digits, hyphens, and underscores.
    </doc>
    <argument name = "self" type = "$(selftype) *">Symbol table</argument>
    <argument name = "string" type = "char *">Original string</argument>
    <declare name = "result" type = "char *">Returned value</declare>
    <local>
    ipr_regexp_t
        *regexp;                        //  The compiled regexp
    int
        nbr_matches;                    //  Number of matched items
    char
        *before,                        //  Text before symbol
        *name,                          //  Matched symbol name
        *after,                         //  Text after symbol
        *value;                         //  Value of symbol
    ipr_dict_t
        *item;                          //  Symbol in dictionary
    </local>
    //
    //  Duplicate string into working copy
    result = icl_mem_strdup (string);
    string = result;

    //  Compile regular expression that will match symbols
    //  Code generation turns two backslashes into a single one
    //  and we need two in the regular expression, and C will turn
    //  two into one... we could use ` except ipr_regexp treats ``
    //  as `.  Sorry for this, it's the best deal I could get.
    regexp = ipr_regexp_new (
        "(.*)"                          //  Anything up to symbol
        "`$\([A-Za-z0-9-_]*)\\\\\\\\?"  //  Match symbol name
        "(.*)");                        //  Everything after symbol

    FOREVER {
        before = name = after = NULL;
        nbr_matches = ipr_regexp_match (regexp, string, &before, &name, &after);
        if (nbr_matches < 3)
            break;                      //  No symbol found to replace

        //  Lookup symbol by name
        if (streq (name, ""))
            value = "\\001";
        else {
            item = self_search (self, name);
            if (item)
                value = item->value;
            else
                value = "";
        }
        //  Paste resulting pieces together into a new string
        result = icl_mem_alloc (strlen (before) + strlen (value) + strlen (after) + 1);
        strcpy (result, before);
        strcat (result, value);
        strcat (result, after);
        icl_mem_free (string);
        string = result;

        icl_mem_free (before);
        icl_mem_free (name);
        icl_mem_free (after);
    }
    ipr_str_subch (result, '\\001', '$');
    ipr_regexp_destroy (&regexp);
</method>

<method name = "export" return = "strings">
    <doc>
    Saves the dictionary as an array of strings in name=value format.
    The env option formats names in uppercase with underscores in place
    of non-alphanumeric characters.
    </doc>
    <argument name = "self" type = "$(selftype) *">Reference to self</argument>
    <argument name = "env" type = "Bool">Format as environment</argument>
    <argument name = "prefix" type = "char *">Optional prefix for exported names</argument>
    <declare name = "strings" type = "char **">Returned arrary of strings</declare>
    <local>
    ipr_dict_t
        *item;
    size_t
        dict_size;
    uint
        string_nbr;                     //  Index into symbol_array
    char
        *value_ptr,                     //  Pointer into value block
        *name_ptr;
    </local>
    //
    //  Count total size of dictionary
    //  Main array consist of N+1 char pointers
    dict_size = sizeof (char *) * (self->nbr_items + 1);
    item = ipr_dict_list_first (self->list);
    while (item) {
        dict_size += strlen (item->key) + strlen (item->value) + 2;
        if (prefix)
            dict_size += strlen (prefix);
        item = ipr_dict_list_next (&item);
    }
    strings = icl_mem_alloc (dict_size);

    //  Now save the strings into the array
    string_nbr = 0;
    value_ptr = (char *) strings + sizeof (char *) * (self->nbr_items + 1);
    item = ipr_dict_list_first (self->list);
    while (item) {
        if (prefix) {
            strcpy (value_ptr, prefix);
            strcat (value_ptr, item->key);
        }
        else
            strcpy (value_ptr, item->key);

        if (env) {
            for (name_ptr = value_ptr; *name_ptr; name_ptr++) {
                if (isalnum (*name_ptr))
                    *name_ptr = toupper (*name_ptr);
                else
                    *name_ptr = '_';
            }
        }
        strcat (value_ptr, "=");
        strcat (value_ptr, item->value);
        strings [string_nbr++] = value_ptr;
        value_ptr += strlen (value_ptr) + 1;
        item = ipr_dict_list_next (&item);
    }
    strings [string_nbr] = NULL;        //  Store final null pointer
</method>

<method name = "import" template = "function">
    <doc>
    Imports an array of strings in name=value format into the dictionary.
    The lower option converts the names to lower case.
    </doc>
    <argument name = "strings" type = "char **">Array of strings to import</argument>
    <argument name = "lower" type = "Bool">Format names into lower case</argument>
    <local>
    char
        **stringp,                      //  Pointer to next string
        *equals;                        //  Position of '=' in string
    icl_shortstr_t
        name;
    </local>
    //
    for (stringp = strings; *stringp; stringp++) {
        //  Ignore any strings that are not in name=value format
        equals = strchr (*stringp, '=');
        if (equals) {
            icl_shortstr_ncpy (name, *stringp, equals - *stringp);
            if (lower)
                ipr_str_lower (name);
            ipr_dict_assume (self, name, equals + 1);
        }
    }
</method>

<method name = "headers load" template = "function">
    <doc>
    Imports a multi-line block of headers ('Name: value') into the dictionary.
    Converts the names to lower case.  The headers block is terminated by a
    NULL byte. The import process ends at the null byte, or if there is an
    invalid header or if there is a blank line.  Returns number of headers
    loaded.  Lines may end in CRLF or LF.
    </doc>
    <argument name = "headers" type = "char *">Headers block to import</argument>
    <argument name = "trace" type = "Bool">Trace headers?</argument>
    <local>
    ipr_regexp_t
        *regexp;                        //  The compiled regexp
    char
        *header_name = NULL,
        *header_value = NULL;
    </local>
    //
    regexp = ipr_regexp_new ("^([`w-]+):`s+([^\\r\\n]*)\\r?\\n");
    while (headers) {
        if (ipr_regexp_match (regexp, headers, &header_name, &header_value) == 2) {
            if (trace)
                icl_console_print ("I: %s: %s", header_name, header_value);
            ipr_str_lower (header_name);
            //  Chop header name and value to sane sizes to stop overflow attacks
            //  Header name fits into an icl_shortstr_t
            if (strlen (header_name) > ICL_SHORTSTR_MAX)
                header_name [ICL_SHORTSTR_MAX] = 0;
            //  We truncate the header value at 2K, arbitrarily
            if (strlen (header_value) > 2048)
                header_value [2048] = 0;

            ipr_dict_assume (self, header_name, header_value);
            icl_mem_strfree (&header_name);
            icl_mem_strfree (&header_value);
            rc++;
        }
        else
            break;                      //  Not a valid header, stop parsing

        headers = strchr (headers, '\\n');
        if (headers)
            headers += 1;               //  Skip to start of next header line
    }
    ipr_regexp_destroy (&regexp);
</method>

<method name = "headers save" return = "bucket">
    <doc>
    Exports a dictionary as a multiline block of header fields in the
    format 'Name: value'. Converts the names to mixed case. Returns a new
    bucket containing the resulting headers.  The caller can ask for a block
    of text to be inserted at the start of the bucket (the prefix).
    </doc>
    <argument name = "self" type = "$(selftype) *">Reference to self</argument>
    <argument name = "prefix" type = "icl_longstr_t *">Bucket prefix</argument>
    <argument name = "trace" type = "Bool">Trace headers?</argument>
    <declare name = "bucket" type = "ipr_bucket_t *">Bucket to fill</declare>
    <local>
    ipr_dict_t
        *item;
    size_t
        dict_size;
    byte
        *value_ptr;                     //  Pointer into value block
    </local>
    //
    //  Count total size of dictionary for bucket allocation
    dict_size = 2;                      //  Ends in blank line
    if (prefix)
        dict_size += prefix->cur_size;

    item = ipr_dict_list_first (self->list);
    while (item) {
        //  Allow for Name: item[cr][lf]
        dict_size += strlen (item->key) + strlen (item->value) + 4;
        item = ipr_dict_list_next (&item);
    }
    bucket = ipr_bucket_new (dict_size);
    if (prefix)
        ipr_bucket_cat (bucket, prefix->data, prefix->cur_size);

    value_ptr = bucket->data + bucket->cur_size;
    item = ipr_dict_list_first (self->list);
    while (item) {
        sprintf ((char *) value_ptr, "%s: %s", item->key, item->value);
        value_ptr [0] = toupper (value_ptr [0]);
        if (trace)
            icl_console_print ("I: %s", value_ptr);
        strcat ((char *) value_ptr, "\\r\\n");
        value_ptr += strlen ((char *) value_ptr);
        item = ipr_dict_list_next (&item);
    }
    //  End data with blank line
    strcat ((char *) value_ptr, "\\r\\n");
    value_ptr += strlen ((char *) value_ptr);
    bucket->cur_size = value_ptr - bucket->data;
    assert (bucket->cur_size <= bucket->max_size);
</method>

<method name = "headers search" return = "value">
    <doc>
    Returns the value for the specified header field.  If the field was not
    present in the table, returns an empty string, else returns the value.
    The field name may be specified in any case; the lookup disregards case.
    </doc>
    <argument name = "self" type = "$(selftype) *">Reference to self</argument>
    <argument name = "name" type = "char *">Field to look for</argument>
    <declare name = "value" type = "char *">Value of header field</declare>
    <local>
    icl_shortstr_t
        item_name;                     //  Field name, normalised to lowercase
    ipr_dict_t
        *item;                    //  Field item in dictionary, if any
    </local>
    //
    assert (name);
    icl_shortstr_cpy (item_name, name);
    ipr_str_lower (item_name);
    item = ipr_dict_table_search (self, item_name);
    value = item? item->value: "";
</method>

<method name = "uri load" template = "function">
    <doc>
    Imports a URI argument string (name=value&name=value...) into the table.
    Returns number of argument fields loaded.  Each value is passed through
    ipr_http_unescape_uri ().
    </doc>
    <argument name = "arguments" type = "char *">Arguments to import</argument>
    <local>
    ipr_regexp_t
        *regexp;                        //  The compiled regexp
    char
        *name = NULL,
        *value = NULL;
    </local>
    //
    regexp = ipr_regexp_new ("^&?([`w-_]+)=([^&#]*)");
    while (arguments) {
        if (ipr_regexp_match (regexp, arguments, &name, &value) == 2) {
            //  Argument name fits into an icl_shortstr_t
            if (strlen (name) > ICL_SHORTSTR_MAX)
                name [ICL_SHORTSTR_MAX] = 0;
            //  We truncate the argument value at 2K, arbitrarily
            if (strlen (value) > 2048)
                value [2048] = 0;
            ipr_http_unescape_uri (value);

            ipr_dict_assume (self, name, value);
            icl_mem_strfree (&name);
            icl_mem_strfree (&value);
            arguments = strchr (arguments + 1, '&');
            rc++;                       //  We loaded one more argument
        }
        else
            break;                      //  Not valid, stop parsing
    }
    ipr_regexp_destroy (&regexp);
</method>

<method name = "props load" template = "function">
    <doc>
    Imports a property string into the table.  Property strings consist of
    zero or more 'name = value' pairs, where the value can optionally be
    enclosed in double or single quotes, and the pairs are seperated by an
    optional comma. Spaces are optional around '=' and between pairs. Returns
    number of argument fields loaded.
    </doc>
    <argument name = "string" type = "char *">Properties to import</argument>
    <local>
    ipr_regexp_t
        *regexp;                        //  The compiled regexp
    char
        *name = NULL,
        *value1 = NULL,
        *value2 = NULL,
        *remainder = NULL;
    </local>
    //
    string = icl_mem_strdup (string);
    regexp = ipr_regexp_new ("^([`w-_]+)`s*=`s*(?:\\"([^\\"]*)\\"|([^ ,]+))`s*,?`s*(.*)");
    while (string) {
        if (ipr_regexp_match (regexp, string, &name, &value1, &value2, &remainder) == 4) {
            //  Argument name fits into an icl_shortstr_t
            if (strlen (name) > ICL_SHORTSTR_MAX)
                name [ICL_SHORTSTR_MAX] = 0;

            ipr_dict_assume (self, name, *value1? value1: value2);
            icl_mem_strfree (&name);
            icl_mem_strfree (&value1);
            icl_mem_strfree (&value2);
            icl_mem_strfree (&string);
            string = remainder;
            remainder = NULL;
            rc++;                       //  We loaded one more property
        }
        else
            break;                      //  Not valid, stop parsing
    }
    icl_mem_strfree (&string);
    ipr_regexp_destroy (&regexp);
</method>

<method name = "file load" template = "function">
    <doc>
    Loads a text file into the dictionary.  If the pattern is null, a default
    pattern of "^([^:]+): (.*)$" is used.  If the pattern is specified it must
    contain two () substitution arguments, the first is the key and the second
    is the value.  Lines that do not match are silently discarded. Blank lines
    and lines starting with '#' are silently discarded, irrespective of the
    pattern.  Stores the loaded file's time and size, and returns the number of
    entries loaded from the file.  If the file could not be found, returns -1.
    Lines are limited to 4096 bytes, keys to 255 characters.
    </doc>
    <argument name = "filename" type = "char *">Name of file to import</argument>
    <argument name = "pattern" type = "char *">Regexp to match each line</argument>
    <local>
#   define IPR_DICT_LINE_MAX    4096
    FILE
        *file;
    char
        *line;
    ipr_regexp_t
        *regexp;                        //  The compiled regexp
    char
        *name = NULL,
        *value = NULL;
    </local>
    //
    file = fopen (filename, "r");
    if (file) {
        icl_mem_free (self->file_name);
        self->file_name = icl_mem_strdup (filename);

        line = icl_mem_alloc (IPR_DICT_LINE_MAX + 1);
        if (!pattern)                   //  Default pattern is word + value
            pattern = "^([^:]+): (.*)$";
        regexp = ipr_regexp_new (pattern);
        while (fgets (line, IPR_DICT_LINE_MAX, file) != NULL) {
            if (line [0] == '#' || line [0] == 0)
                ;                       //  Ignore comments and empty lines
            else
            if (ipr_regexp_match (regexp, line, &name, &value) == 2) {
                if (strlen (name) > ICL_SHORTSTR_MAX)
                    name [ICL_SHORTSTR_MAX] = 0;
                ipr_dict_assume (self, name, value);
                icl_mem_strfree (&name);
                icl_mem_strfree (&value);
                rc++;
            }
        }
        ipr_regexp_destroy (&regexp);
        icl_mem_free (line);
        fclose (file);

        self->file_size = ipr_file_size (self->file_name);
        self->file_time = ipr_file_time (self->file_name);
    }
    else
        rc = -1;
</method>

<method name = "file save" template = "function">
    <doc>
    Saves the dictionary to a text file, one entry per line.  The format of
    each line in the file is specified a pattern.  The pattern is a string
    containing two '%s' string.  The first is the item name, and the second is
    the item value. If the pattern is null, a default pattern "%s: %s" is
    used.  Maximum length of one line is 4096 bytes.  If the filename is null,
    the saved dictionary filename is used.
    </doc>
    <argument name = "filename" type = "char *">Name of file to import</argument>
    <argument name = "pattern" type = "char *">Pattern for saved lines</argument>
    <local>
    FILE
        *file;
    char
        *line;
    ipr_dict_t
        *item;
    </local>
    //
    if (!filename)
        filename = self->file_name;
    assert (filename);
    if (!pattern)
        pattern = "%s: %s";

    file = fopen (filename, "w");
    line = icl_mem_alloc (IPR_DICT_LINE_MAX + 1);
    item = ipr_dict_list_first (self->list);
    while (item) {
        apr_snprintf (line, IPR_DICT_LINE_MAX, pattern,  item->key, item->value);
        fprintf (file, "%s\\n", line);
        item = ipr_dict_list_next (&item);
    }
    icl_mem_free (line);
    fclose (file);
</method>

<method name = "file sync" return = "rc">
    <doc>
    Syncs the table with a disk file.  This method checks the time
    and size of the file on disk, and if changed from the table in memory,
    loads the file data into memory.  The reload is done in a thread-safe
    manner using an atomic compare-and-swap, so that applications can use the
    old table while the new one is being constructed.  If the file cannot be
    opened, the table will be empty.  The pattern argument defines the format
    of the file, as for the file_load() method.  The table reference may be
    NULL, meaning no table has been allocated.  Returns number of items loaded
    or -1 if the file could not be loaded.
    </doc>
    <argument name = "self_p" type = "$(selftype) **">Pointer to table reference</argument>
    <argument name = "filename" type = "char *">Name of file to import</argument>
    <argument name = "pattern" type = "char *">Regexp to match each line</argument>
    <declare name = "rc" type = "int" default = "0" />
    <local>
    ipr_dict_table_t
        *table,                         //  Load here, then swap to self_p
        *old_table;                     //  For CAS operation
    </local>
    //
    assert (filename);
    if (!(*self_p)
    ||  ipr_file_size (filename) != (*self_p)->file_size
    ||  ipr_file_time (filename) != (*self_p)->file_time) {
        table = ipr_dict_table_new ();
        rc = ipr_dict_table_file_load (table, filename, pattern);
        //  A 64-bit safe CAS will update the table reference safely
        do {
            old_table = *self_p;
        }
        while (icl_atomic_casptr ((volatile void **) self_p, table, old_table) != old_table);
        //  When the CAS has completed, old_table and *self_p have swapped
        ipr_dict_table_destroy (&old_table);
    }
</method>

<method name = "selftest">
    <local>
    ipr_dict_table_t
        *table;
    ipr_bucket_t
        *bucket;
    ipr_dict_t
        *item;
    size_t
        nbr_items;
    char
        *pattern,
        **strings,                      //  Dictionary as array of strings
        **stringp;
    </local>

    table = ipr_dict_table_new ();

    //  Insert some items
    assert (ipr_dict_assume (table, "0xdeadbeef", "deadbeef"));
    assert (ipr_dict_assume (table, "0xabadcafe", "abadcafe"));
    assert (ipr_dict_assume (table, "0xabadcafe", "abadcafe") == FALSE);

    //  Retrieve items
    item = ipr_dict_table_search (table, "0xdeadbeef");
    assert (item && streq (item->value, "deadbeef"));
    item = ipr_dict_table_search (table, "0xabadcafe");
    assert (item && streq (item->value, "abadcafe"));
    assert (*ipr_dict_table_lookup (table, "0xdeadbeef"));
    assert (*ipr_dict_table_lookup (table, "nosuch") == 0);

    //  Get a non existent item
    item = ipr_dict_table_search (table, "0xf0000000");
    assert (item == NULL);

    //  Pattern string
    pattern = ipr_dict_table_template (table, "=$0xabadcafe\\\\xx$\=");
    assert (streq (pattern, "=abadcafexx$="));
    icl_mem_free (pattern);

    //  Import environment as table
    ipr_dict_table_import (table, environ, TRUE);

    //  Export list as array of name=value pairs
    strings = ipr_dict_table_export (table, TRUE, "TEST_");
    for (stringp = strings; *stringp; stringp++)
        assert (*stringp);
    icl_mem_free (strings);

    //  Delete an item
    item = ipr_dict_table_search (table, "0xdeadbeef");
    ipr_dict_destroy (&item);
    item = ipr_dict_table_search (table, "0xdeadbeef");
    assert (item == NULL);

    ipr_dict_table_destroy (&table);

    table = ipr_dict_table_new ();
    assert (ipr_dict_table_headers_load (table,
        "Content: usually\\n"
        "Disposition-mode: happy, sunny\\n"
        "\\n"
        "Difficulty: medium\\n",
        FALSE) == 2);

    bucket = ipr_dict_table_headers_save (table, NULL, FALSE);
    assert (bucket);
    assert (strlen ((char *) bucket->data) == 52);
    ipr_bucket_unlink (&bucket);

    assert (streq (ipr_dict_table_headers_search (table, "content"), "usually"));
    assert (streq (ipr_dict_table_headers_search (table, "DISPOSITION-mode"), "happy, sunny"));
    assert (streq (ipr_dict_table_headers_search (table, "Difficulty"), ""));

    assert (ipr_dict_table_uri_load (table, "movie=mad%20max&ranking=great#id=wally") == 2);
    assert (streq (ipr_dict_table_headers_search (table, "movie"), "mad max"));
    assert (ipr_dict_table_props_load (table, "movie=\\"snakes on a plane\\", ranking=great, id=\\"wally\\"") == 3);
    assert (streq (ipr_dict_table_headers_search (table, "movie"), "snakes on a plane"));

    ipr_dict_table_destroy (&table);

    //  Test file load/sync/save functions
    table = ipr_dict_table_new ();
    assert (ipr_dict_table_file_load (table, "ipr_dict_table.icl", "&lt;method name = \\"([^\\"]+)\\" (.*)&gt;") > 0);
    nbr_items = table->nbr_items;
    assert (ipr_dict_table_file_save (table, "ipr_dict_table.lst", NULL) == 0);
    assert (ipr_dict_table_file_sync (&table, "ipr_dict_table.lst", NULL) == nbr_items);
    assert (table->nbr_items == nbr_items);
    ipr_file_delete ("ipr_dict_table.lst");
    assert (ipr_dict_table_file_sync (&table, "ipr_dict_table.lst", NULL) == -1);
    assert (table->nbr_items == 0);
    ipr_dict_table_destroy (&table);
</method>

</class>
