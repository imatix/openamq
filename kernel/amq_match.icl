<?xml?>
<class
    name      = "amq_match"
    comment   = "Matcher class"
    version   = "1.0"
    copyright = "Copyright (c) 2004-2005 JPMorgan and iMatix Corporation"
    script    = "icl_gen"
    >
<doc>
This class implements the message matching item.  Each entry in the
match table is a search term along with the set of subscriptions that
have requested it.
</doc>

<inherit class = "ipr_hash_str" />

<import class = "amq_global" />
<import class = "amq_db" />

<public name = "header">
#include "amq_core.h"
</public>

<context>
    ipr_bits_t
        *bits;                          /*  Set of subscriptions for term    */
</context>

<method name = "new">
    self->bits = ipr_bits_new ();
</method>

<method name = "destroy">
    ipr_bits_destroy (&self->bits);
</method>

<private name = "header">
#define S_WILDCARD_SINGLE     "[a-zA-Z0-9]+"
#define S_WILDCARD_MULTIPLE   "[a-zA-Z0-9]+(`.[a-zA-Z0-9]+)*"
</private>

<method name = "topic_re">
    <doc>
    Converts a topic destination name into a regular expression.  The topic
    name can contain wildcards that match part or all of a topic name tree.
    '*' in the destination name means wildcard a single level of topics.
    '#' in the destination name means wildcard zero or more levels.
    Topic levels are separated by '.'.
    </doc>
    <argument name = "pattern"   type = "char *">Regexp shortstr to fill</argument>
    <argument name = "dest name" type = "char *">Destination name</argument>
    <local>
    char
        *from_ptr,
        *to_ptr;
    </local>
    /*  We want a regexp starting with ^, ending with $, and with the
        * and # topic wildcards replaced by appropriate regexp chars.
        We also filter out any non-alphanum characters.  We may allow
        full RE matching on topic names at a later stage.
     */
    to_ptr = pattern;
    *to_ptr++ = '^';                    /*  Match start of topic name        */
    for (from_ptr = dest_name; *from_ptr; from_ptr++) {
        if (isalnum (*from_ptr))
            *to_ptr++ = *from_ptr;
        else
        if (*from_ptr == '.') {
            *to_ptr++ = '`';
            *to_ptr++ = '.';
        }
        else
        if (*from_ptr == '*') {
            strcpy (to_ptr, S_WILDCARD_SINGLE);
            to_ptr += strlen (S_WILDCARD_SINGLE);
        }
        else
        if (*from_ptr == '#') {
            strcpy (to_ptr, S_WILDCARD_MULTIPLE);
            to_ptr += strlen (S_WILDCARD_MULTIPLE);
        }
    }
    *to_ptr++ = '$';                    /*  Match end of topic name          */
    *to_ptr++ = 0;
</method>

<private name = "header">
#define FIELD_NAME_MAX      30          /*  Arbitrary limit to get           */
#define FIELD_VALUE_MAX     30          /*    name=value into shortstr       */
</private>

<method name = "field name">
    <doc>
    Format field name into a match key. This simply truncates the field
    name to a maximum size.
    </doc>
    <argument name = "match key" type = "char *"       >Match term shortstr to fill</argument>
    <argument name = "field"     type = "amq_field_t *">Field</argument>
    ipr_shortstr_ncpy (match_key, field->name, FIELD_NAME_MAX);
</method>

<method name = "field value">
    <doc>
    Format field name and value into a match key.
    </doc>
    <argument name = "match key" type = "char *"       >Match term shortstr to fill</argument>
    <argument name = "field"     type = "amq_field_t *">Field</argument>
    <local>
    char
        *field_value;                   /*  Field string value               */
    </local>
    ipr_shortstr_ncpy (match_key, field->name, FIELD_NAME_MAX);
    field_value = amq_field_string (field);
    if (*field_value) {
        ipr_shortstr_cat  (match_key, "=");
        ipr_shortstr_ncat (match_key, field_value, FIELD_VALUE_MAX);
    }
</method>

<method name = "selftest" />

</class>
