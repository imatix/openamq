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
    name      = "asl_field"
    comment   = "Field class"
    version   = "1.0"
    script    = "icl_gen"
    >
<doc>
    Provides functionality to implement the ASL RFC 'Field Table'
    data type.  This class provides functions at a per-field level.
</doc>

<inherit class = "icl_object">
    <option name = "alloc" value = "cache" />
</inherit>
<inherit class = "icl_list_item">
    <option name = "links" value = "1" />
</inherit>

<import class = "asl" />

<option name = "links" value = "1"/>
<option name = "rwlock" value = "0"/>
<option name = "nullify" value = "0"/>

<public name = "header">
#define ASL_FIELD_TYPE_STRING       'S'
#define ASL_FIELD_TYPE_INTEGER      'I'
#define ASL_FIELD_TYPE_DECIMAL      'D'
#define ASL_FIELD_TYPE_TIME         'T'
#define ASL_FIELD_TYPE_FIELD_TABLE  'F'  // not implemented so far
#define ASL_FIELD_TYPE_VOID         'V'
#define ASL_FIELD_NAME_MAX          128
</public>

<private name = "header">
//  Network byte ordering                                                      

#define ASL_FIELD_PUT_LONG(net, host) \\
    (net) [0] = (byte) (((host) >> 24) & 255); \\
    (net) [1] = (byte) (((host) >> 16) & 255); \\
    (net) [2] = (byte) (((host) >> 8)  & 255); \\
    (net) [3] = (byte) (((host))       & 255); \\
    (net) += 4
#define ASL_FIELD_GET_LONG(host, net) \\
    (host) = ((net) [0] << 24) \\
           + ((net) [1] << 16) \\
           + ((net) [2] << 8) \\
           +  (net) [3]; \\
    (net)  += 4
</private>

<context>
    icl_shortstr_t
        name;                           //  Field name                         
    byte
        type;                           //  Field type                         
    byte
        decimals;                       //  Decimals, if necessary             
    icl_longstr_t
        *string;                        //  Value as string                    
    int64_t
        integer;                        //  Value as integer, decimal, time    
</context>

<method name = "new">
    strclr (self->name);
    self->type     = 0;
    self->decimals = 0;
    self->integer  = 0;
    self->string   = NULL;
</method>

<method name = "destroy">
    icl_longstr_destroy (&self->string);
</method>

<method name = "new string">
    <argument name = "file" type = "char *" precalc = "__FILE__">Source file for call</argument>
    <argument name = "line" type = "size_t" precalc = "__LINE__">Line number for call</argument>
    <argument name = "list"  type = "asl_field_list_t *">Parent field list</argument>
    <argument name = "name"  type = "char *">Field name</argument>
    <argument name = "value" type = "char *">Field string value</argument>
    <local>
    asl_field_t
        *self;
    </local>
    //
    assert (strlen (name) <= ASL_FIELD_NAME_MAX);

    self = asl_field_new_ (file, line);
    strcpy (self->name, name);
    self->type   = ASL_FIELD_TYPE_STRING;
    self->string = icl_longstr_new (value, strlen (value) + 1);
    self->string->data [strlen (value)] = '\\0';
    asl_field_list_queue (list, self);
    asl_field_unlink (&self);
</method>

<method name = "new longstr">
    <argument name = "file" type = "char *" precalc = "__FILE__">Source file for call</argument>
    <argument name = "line" type = "size_t" precalc = "__LINE__">Line number for call</argument>
    <argument name = "list"  type = "asl_field_list_t *">Parent field list</argument>
    <argument name = "name"  type = "char *">Field name</argument>
    <argument name = "value" type = "icl_longstr_t *">Field string value</argument>
    <local>
    asl_field_t
        *self;
    </local>
    //
    assert (strlen (name) <= ASL_FIELD_NAME_MAX);

    self = asl_field_new_ (file, line);
    strcpy (self->name, name);
    self->type   = ASL_FIELD_TYPE_STRING;
    self->string = icl_longstr_dup (value);
    asl_field_list_queue (list, self);
    asl_field_unlink (&self);
</method>

<method name = "new integer">
    <argument name = "file" type = "char *" precalc = "__FILE__">Source file for call</argument>
    <argument name = "line" type = "size_t" precalc = "__LINE__">Line number for call</argument>
    <argument name = "list"  type = "asl_field_list_t *">Parent field list</argument>
    <argument name = "name"  type = "char *">Field name</argument>
    <argument name = "value" type = "int"   >Field integer value</argument>
    <local>
    asl_field_t
        *self;
    </local>
    //
    assert (strlen (name) <= ASL_FIELD_NAME_MAX);

    self = asl_field_new_ (file, line);
    strcpy (self->name, name);
    self->type    = ASL_FIELD_TYPE_INTEGER;
    self->integer = value;
    asl_field_list_queue (list, self);
    asl_field_unlink (&self);
</method>

<method name = "new decimal">
    <argument name = "file" type = "char *" precalc = "__FILE__">Source file for call</argument>
    <argument name = "line" type = "size_t" precalc = "__LINE__">Line number for call</argument>
    <argument name = "list"  type = "asl_field_list_t *">Parent field list</argument>
    <argument name = "name"  type = "char *">Field name</argument>
    <argument name = "value" type = "long"  >Field decimal value</argument>
    <argument name = "decimals" type = "int">Number of decimal places</argument>
    <local>
    asl_field_t
        *self;
    </local>
    //
    assert (strlen (name) <= ASL_FIELD_NAME_MAX);

    self = asl_field_new_ (file, line);
    strcpy (self->name, name);
    self->type     = ASL_FIELD_TYPE_DECIMAL;
    self->integer  = value;
    self->decimals = decimals;
    asl_field_list_queue (list, self);
    asl_field_unlink (&self);
</method>

<method name = "new time">
    <argument name = "file" type = "char *" precalc = "__FILE__">Source file for call</argument>
    <argument name = "line" type = "size_t" precalc = "__LINE__">Line number for call</argument>
    <argument name = "list"  type = "asl_field_list_t *">Parent field list</argument>
    <argument name = "name"  type = "char *">Field name</argument>
    <argument name = "value" type = "time_t">Field time value</argument>
    <local>
    asl_field_t
        *self;
    </local>
    //
    assert (strlen (name) <= ASL_FIELD_NAME_MAX);

    self = asl_field_new_ (file, line);
    strcpy (self->name, name);
    self->type    = ASL_FIELD_TYPE_TIME;
    self->integer = value;
    asl_field_list_queue (list, self);
    asl_field_unlink (&self);
</method>

<method name = "new field table">
    <argument name = "file" type = "char *" precalc = "__FILE__">Source file for call</argument>
    <argument name = "line" type = "size_t" precalc = "__LINE__">Line number for call</argument>
    <argument name = "list"  type = "asl_field_list_t *">Parent field list</argument>
    <argument name = "name"  type = "char *">Field name</argument>
    <argument name = "value" type = "asl_field_list_t *">The value</argument>
    <local>
    asl_field_t
        *self;
    icl_longstr_t
        *flattened;
    </local>
    //
    assert (strlen (name) <= ASL_FIELD_NAME_MAX);

    flattened = asl_field_list_flatten (value);
    self = asl_field_new_ (file, line);
    strcpy (self->name, name);
    self->type   = ASL_FIELD_TYPE_FIELD_TABLE;
    self->string = icl_longstr_dup (flattened);
    asl_field_list_queue (list, self);
    asl_field_unlink (&self);
    icl_longstr_destroy (&flattened);
</method>

<method name = "new void">
    <argument name = "file" type = "char *" precalc = "__FILE__">Source file for call</argument>
    <argument name = "line" type = "size_t" precalc = "__LINE__">Line number for call</argument>
    <argument name = "list"  type = "asl_field_list_t *">Parent field list</argument>
    <argument name = "name"  type = "char *">Field name</argument>
    <local>
    asl_field_t
        *self;
    </local>
    //
    assert (strlen (name) <= ASL_FIELD_NAME_MAX);

    self = asl_field_new_ (file, line);
    strcpy (self->name, name);
    self->type = ASL_FIELD_TYPE_VOID;
    asl_field_list_queue (list, self);
    asl_field_unlink (&self);
</method>

<method name = "assume">
    <doc>
    Looks for a field with the specified name, if not found, creates a
    new field with the specified value, else updates the field's value.
    </doc>
    <argument name = "list"  type = "asl_field_list_t *">Parent field list</argument>
    <argument name = "name"  type = "char *">Field name</argument>
    <argument name = "value" type = "char *">Field string value</argument>
    <local>
    asl_field_t
        *field;
    </local>
    //
    field = asl_field_list_search (list, name);
    if (field)
        self_destroy (&field);
    asl_field_new_string (list, name, value);
</method>

<method name = "parse" return = "field_end">
    <doc>
    Parses a field definition. Returns the address of the first octet after
    the field definition, or NULL if there was an error parsing the field.
    The limit points to the octet following the last legal octet of the string.
    </doc>
    <argument name = "list"  type = "asl_field_list_t *">Parent field list</argument>
    <argument name = "input" type = "byte *">Field definition structure</argument>
    <argument name = "limit" type = "byte *">End of field-table string</argument>
    <declare name = "field_end" type = "byte *">Calculated end position</declare>
    <local>
    asl_field_t
        *self;
    size_t
        string_size;
    </local>
    //
    assert (list);
    self = self_new ();

    //  Copy field name                                                        
    string_size = *input;
    input += 1;
    memcpy (self->name, input, string_size);
    self->name [string_size] = 0;
    input += string_size;

    assert (input <= limit);
    self->type = *input;
    input += 1;

    if (self->type == ASL_FIELD_TYPE_STRING) {
        ASL_FIELD_GET_LONG (string_size, input);

        //  Ensure that the longstr->data is a valid C string
        self->string = icl_longstr_new (NULL, string_size + 1);
        memcpy (self->string->data, input, string_size);
        self->string->data [string_size] = '\\0';
        self->string->cur_size = string_size;
        input += string_size;
        field_end = input;
    }
    else
    if (self->type == ASL_FIELD_TYPE_INTEGER) {
        ASL_FIELD_GET_LONG (self->integer, input);
        field_end = input;
    }
    else
    if (self->type == ASL_FIELD_TYPE_DECIMAL) {
        self->decimals = *input;
        input += 1;
        ASL_FIELD_GET_LONG (self->integer, input);
        field_end = input;
    }
    else
    if (self->type == ASL_FIELD_TYPE_TIME) {
        ASL_FIELD_GET_LONG (self->integer, input);
        field_end = input;
    }
    else
    if (self->type == ASL_FIELD_TYPE_FIELD_TABLE) {
        ASL_FIELD_GET_LONG (string_size, input);

        //  Ensure that the longstr->data is a valid C string
        self->string = icl_longstr_new (NULL, string_size);
        memcpy (self->string->data, input, string_size);
        self->string->cur_size = string_size;
        input += string_size;
        field_end = input;
    }
    else
    if (self->type == ASL_FIELD_TYPE_VOID) {
        field_end = input;
    }
    else {
        icl_console_print ("$(selfname): invalid field type - '%c'", *input);
        field_end = NULL;
    }
    if (field_end == NULL || input > limit) {
        icl_console_print ("$(selfname): field definition badly formed - rejected");
        field_end = NULL;
        self_destroy (&self);
    }
    else
        asl_field_list_queue (list, self);

    asl_field_unlink (&self);
</method>

<method name = "string" return = "value">
    <doc>
    Returns string value for field.  If the field is not a string, will
    attempt to convert the field value into a string. The returned string
    is always be null-terminated.  NULLs in the string will be replaced
    by binary 255 - this will modify binary string values.
    </doc>
    <argument name = "self" type = "$(selftype) *">Reference to object</argument>
    <declare name = "value" type = "char *">Returned string value</declare>
    //
    $(selfname:upper)_ASSERT_SANE (self);
    if (self->type == ASL_FIELD_TYPE_STRING) {
        //  Suppress nulls in string value
        int
            index = 0;

        for (index = 0; index < (int) self->string->cur_size - 1; index++)
            if (self->string->data [index] == 0)
                self->string->data [index] = 255;
    }
    else {
        //  Add a string value and format the field accordingly
        if (!self->string)
            self->string = icl_longstr_new (NULL, 30);

        if (self->type == ASL_FIELD_TYPE_INTEGER)
            icl_longstr_fmt (self->string, "%d", (long) self->integer);
        else
        if (self->type == ASL_FIELD_TYPE_DECIMAL)
            icl_longstr_fmt (self->string, "%g",
                (double) self->integer / exp (self->decimals * log (10.0)));
        else
        if (self->type == ASL_FIELD_TYPE_TIME) {
            struct tm
                *time_struct;
            time_struct = gmtime ((time_t *) &self->integer);
            icl_longstr_fmt (self->string, "%4d/%02d/%02dT%02d:%02d:%02dZ",
                self->name,
                time_struct->tm_year + 1900,
                time_struct->tm_mon + 1,
                time_struct->tm_mday,
                time_struct->tm_hour,
                time_struct->tm_min,
                time_struct->tm_sec);
        }
        else
        if (self->type == ASL_FIELD_TYPE_FIELD_TABLE)
            icl_longstr_fmt (self->string, "(table)");
        else
        if (self->type == ASL_FIELD_TYPE_VOID)
            icl_longstr_fmt (self->string, "(void)");
    }
    value = (char *) self->string->data;
</method>

<method name = "integer" return = "value">
    <doc>
    Returns integer value for field.  If the field is not an integer, will
    attempt to convert the field into an integer.
    </doc>
    <argument name = "self" type = "$(selftype) *">Reference to object</argument>
    <declare name = "value" type = "long">Returned integer value</declare>
    //
    $(selfname:upper)_ASSERT_SANE (self);

    value = 0;
    if (self->type == ASL_FIELD_TYPE_STRING)
        value = atol ((char *) self->string->data);
    else
    if (self->type == ASL_FIELD_TYPE_INTEGER)
        value = (long) self->integer;
    else
    if (self->type == ASL_FIELD_TYPE_DECIMAL)
        value = (long) (self->integer / exp (self->decimals * log (10.0)));
    else
    if (self->type == ASL_FIELD_TYPE_TIME)
        value = (long) self->integer;
    else
        value = 0;
</method>

<method name = "field table" return = "value">
    <doc>
    Returns field table stored in the field. If field value is not a field
    table it returns NULL.
    </doc>
    <argument name = "self" type = "$(selftype) *">Reference to object</argument>
    <declare name = "value" type = "asl_field_list_t*">Returned field table</declare>
    //
    $(selfname:upper)_ASSERT_SANE (self);

    if (self->type == ASL_FIELD_TYPE_FIELD_TABLE)
        value = asl_field_list_new (self->string);
    else
        value = NULL;
</method>

<method name = "flatten" template = "function">
    <doc>
    Creates a field definition string from the field. Writes the
    field definition to the provided string.  Returns -1 if the field
    was invalid or there was not enough space to write the field.
    </doc>
    <argument name = "string" type = "icl_longstr_t *">String to write to</argument>
    <local>
    byte
        *output;
    size_t
        name_length;
    </local>
    //
    if (string->cur_size + self_flat_size (self) <= string->max_size) {
        //  Set-up pointer to current writing position in string               
        output = string->data + string->cur_size;

        //  Write field name                                                   
        name_length = strlen (self->name);
        *output = (byte) name_length;
        output += 1;
        memcpy (output, self->name, name_length);
        output += name_length;

        //  Write field type                                                   
        *(char *) output = self->type;
        output += 1;

        if (self->type == ASL_FIELD_TYPE_STRING) {
            ASL_FIELD_PUT_LONG (output, self->string->cur_size);
            memcpy (output, self->string->data, self->string->cur_size);
            output += self->string->cur_size;
        }
        else
        if (self->type == ASL_FIELD_TYPE_INTEGER) {
            ASL_FIELD_PUT_LONG (output, self->integer);
        }
        else
        if (self->type == ASL_FIELD_TYPE_DECIMAL) {
            *output = self->decimals;
            output += 1;
            ASL_FIELD_PUT_LONG (output, self->integer);
        }
        else
        if (self->type == ASL_FIELD_TYPE_TIME) {
            ASL_FIELD_PUT_LONG (output, self->integer);
        }
        else
        if (self->type == ASL_FIELD_TYPE_FIELD_TABLE) {
            ASL_FIELD_PUT_LONG (output, self->string->cur_size);
            memcpy (output, self->string->data, self->string->cur_size);
            output += self->string->cur_size;
        }
        else
        if (self->type == ASL_FIELD_TYPE_VOID) {
            //  Do nothing
        }
        else
            rc = -1;

        if (rc == 0)
            string->cur_size = output - string->data;
    }
    else
        rc = -1;
</method>

<method name = "flat size" template = "function">
    <doc>
    Returns the flattened size of the field, in octets.
    </doc>
    //
    //  All fields have a length-specified name and a 1-octet type             
    rc = strlen (self->name) + 2;
    if (self->type == ASL_FIELD_TYPE_STRING)
        rc += self->string->cur_size + ASL_LSTR_HSIZE;
    else
    if (self->type == ASL_FIELD_TYPE_INTEGER)
        rc += 4;
    else
    if (self->type == ASL_FIELD_TYPE_DECIMAL)
        rc += 4 + 1;
    else
    if (self->type == ASL_FIELD_TYPE_TIME)
        rc += 4;
    else
    if (self->type == ASL_FIELD_TYPE_FIELD_TABLE)
        rc += self->string->cur_size + ASL_LSTR_HSIZE;
    else
    if (self->type == ASL_FIELD_TYPE_VOID)
        rc += 0;
</method>

<method name = "print" template = "function">
    <doc>
    Prints the field format and contents to output (using icl_console_print).
    </doc>
    //
    if (self->type == ASL_FIELD_TYPE_STRING) {
        icl_shortstr_t
            string_value;
        icl_shortstr_cpy (string_value, self_string (self));
        if (self->string->cur_size > 20) {
            string_value [20] = 0;
            icl_shortstr_cat (string_value, "...");
        }
        icl_console_print (" - %s - string, length=%d content=%s", 
            self->name, self->string->cur_size, string_value);
    }
    else
    if (self->type == ASL_FIELD_TYPE_INTEGER)
        icl_console_print (" - %s - integer, value=%d", self->name, (int) self->integer);
    else
    if (self->type == ASL_FIELD_TYPE_DECIMAL)
        icl_console_print (" - %s - decimal, value=%g", self->name,
            (double) self->integer / exp (self->decimals * log (10.0)));
    else
    if (self->type == ASL_FIELD_TYPE_TIME) {
        struct tm
            *time_struct;
        time_struct = gmtime ((time_t *) &self->integer);
        icl_console_print (" - %s - time, value=%4d/%02d/%02dT%02d:%02d:%02dZ",
            self->name,
            time_struct->tm_year + 1900,
            time_struct->tm_mon + 1,
            time_struct->tm_mday,
            time_struct->tm_hour,
            time_struct->tm_min,
            time_struct->tm_sec);
    }
    else
    if (self->type == ASL_FIELD_TYPE_FIELD_TABLE)
        icl_console_print (" - %s - field table", self->name);
    else
    if (self->type == ASL_FIELD_TYPE_VOID)
        icl_console_print (" - %s - void", self->name);
    else
        icl_console_print (" - undefined field type");
</method>

<method name = "selftest" inherit = "none">
    <local>
    asl_field_list_t
        *list,                          //  List of fields
        *embedded_list;                 //  List embedded in the top list
    asl_field_t
        *field,                         //  One field in list
        *embedded_field;                //  Field in the embedded list
    icl_longstr_t
        *string;                        //  Serialised field list
    </local>
    //
    //  We create a new empty list by passing it a null string
    //
    list = asl_field_list_new (NULL);

    //  To add fields to the list, we use these methods, which
    //  do not return any values.
    //
    asl_field_new_string  (list, "string",  "ring a ding ding");
    asl_field_new_integer (list, "integer", 123456789);
    asl_field_new_decimal (list, "decimal", 123456789, 3);
    asl_field_new_time    (list, "time",    123456789);
    asl_field_new_void    (list, "void");
    embedded_list = asl_field_list_new (NULL);
    asl_field_new_string  (embedded_list, "string",  "hello world");
    asl_field_new_integer (embedded_list, "integer", 654321);
    asl_field_new_field_table (list, "table", embedded_list);
    asl_field_list_unlink (&embedded_list);

    //  When we want to send a field list, we flatten it into a
    //  string, which the list_flatten method allocates for us.
    //
    string = asl_field_list_flatten (list);
    asl_field_list_destroy (&list);

    //  Let's recreate the list and check that it's correct
    list = asl_field_list_new (string);

    field = asl_field_list_search (list, "string");
    assert (streq (asl_field_string (field), "ring a ding ding"));
    asl_field_unlink (&field);

    field = asl_field_list_search (list, "integer");
    assert (asl_field_integer (field) == 123456789);
    assert (streq (asl_field_string (field), "123456789"));
    asl_field_unlink (&field);

    field = asl_field_list_search (list, "decimal");
    assert (asl_field_integer (field) == 123456);
    assert (streq (asl_field_string (field), "123457"));
    asl_field_unlink (&field);

    field = asl_field_list_search (list, "time");
    assert (asl_field_integer (field) == 123456789);
    asl_field_unlink (&field);

    field = asl_field_list_search (list, "void");
    assert (asl_field_integer (field) == 0);
    asl_field_unlink (&field);

    field = asl_field_list_search (list, "table");
    embedded_list = asl_field_field_table (field);
    embedded_field = asl_field_list_search (embedded_list, "string");
    assert (streq (asl_field_string (embedded_field), "hello world"));
    asl_field_unlink (&embedded_field);
    embedded_field = asl_field_list_search (embedded_list, "integer");
    assert (asl_field_integer (embedded_field) == 654321);
    asl_field_unlink (&embedded_field);
    asl_field_list_unlink (&embedded_list);
    asl_field_unlink (&field);

    //  We must destroy the flat string when finished with it
    asl_field_list_destroy (&list);
    icl_longstr_destroy (&string);
</method>

</class>
