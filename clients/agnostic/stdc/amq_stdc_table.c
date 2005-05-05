/*---------------------------------------------------------------------------
 *  amq_stdc_table.c - Field table object header file
 *
 *  Copyright (c) 2004-2005 JPMorgan
 *  Copyright (c) 1991-2005 iMatix Corporation
 *---------------------------------------------------------------------------*/

#include "amq_stdc_private.h"
#include "amq_stdc_table.h"

typedef struct tag_table_context_t
{
    char
        *data;
    dbyte
        size;
} table_context_t;

static apr_status_t s_table_add_field (
    table_context_t  *table,
    const char       *name,
    char             type,
    size_t           size,
    void             **data           /*  Out parameter; pointer to newly    */
                                      /*  allocated data part of the string  */
    )
{
    void
        *newdata;
    dbyte
        newsize;
    size_t
        length;

    length = strlen (name);
    if (length > 0xff)
       AMQ_ASSERT (Table field name too long)
    newsize = table->size + sizeof (byte) + length +
        sizeof (byte) + size;
    newdata = amq_realloc (table->data, newsize);
    if (!newdata)
        AMQ_ASSERT (Not enough memory)

    /*  Copy field name to buffer                                            */
    *((byte*) (newdata + (table->size))) = (byte) length;
    memcpy (newdata + table->size + sizeof (byte), name, length);

    /*  Copy field type to buffer                                            */
    *((byte*) (newdata + (table->size) + sizeof (byte) + length)) =
        (byte) type;

    table->data = newdata;
    table->size = newsize;
    if (data)
        *data = table->data + table->size - size;
    return APR_SUCCESS;
}

apr_status_t amq_stdc_table_create (
    dbyte             size,
    const char        *data,
    amq_stdc_table_t  *table
    )
{
    if (table) {
        *table = amq_malloc (sizeof (table_context_t));
        if (!(*table))
            AMQ_ASSERT (Not enough memory)
        if (size) {
            (*table)->data = amq_malloc (size);
            if (!(*table)->data)
                AMQ_ASSERT (Not enough memory)
            (*table)->size = size;
            memcpy ((*table)->data, data, size);
        }
        else {
            (*table)->data = NULL;
            (*table)->size = 0;
        }
    }
    return APR_SUCCESS;
}

apr_status_t amq_stdc_table_add_string (
    amq_stdc_table_t  table,
    const char        *name,
    dbyte             size,
    const char        *string
    )
{
    apr_status_t
        result;
    void
        *dest;

    result = s_table_add_field (table, name, 'S', sizeof (dbyte) + size,
        &dest);
    AMQ_ASSERT_STATUS (result, s_table_add_field)
    *((dbyte*) dest) = htons (size);
    memcpy (dest + sizeof (dbyte), (void*) string, size);

    return APR_SUCCESS;
}

apr_status_t amq_stdc_table_add_integer (
    amq_stdc_table_t  table,
    const char        *name,
    long              integer
    )
{
    apr_status_t
        result;
    long
        *dest;

    result = s_table_add_field (table, name, 'I', sizeof (long),
        (void**) &dest);
    AMQ_ASSERT_STATUS (result, s_table_add_field)
    *dest = htonl (integer);

    return APR_SUCCESS;
}

apr_status_t amq_stdc_table_add_decimal (
    amq_stdc_table_t  table,
    const char        *name,
    byte              decimals,
    long              value
    )
{
    apr_status_t
        result;
    void
        *dest;

    result = s_table_add_field (table, name, 'D', sizeof (byte) +
        sizeof (long), &dest);
    AMQ_ASSERT_STATUS (result, s_table_add_field)
    *((byte*) dest) = decimals;
    *((long*) (dest + sizeof (byte))) = htonl (value);

    return APR_SUCCESS;

}

apr_status_t amq_stdc_table_add_time (
    amq_stdc_table_t  table,
    const char        *name,
    time_t            time
    )
{
    apr_status_t
        result;
    long
        *dest;

    result = s_table_add_field (table, name, 'I', sizeof (long),
        (void**) &dest);
    AMQ_ASSERT_STATUS (result, s_table_add_field)
    *dest = htonl (time);

    return APR_SUCCESS;
}

apr_status_t amq_stdc_table_remove (
    amq_stdc_table_t  table,
    const char        *name
    );

apr_status_t amq_stdc_table_get_string (
    amq_stdc_table_t  table,
    const char        *name,
    dbyte             *size, 
    const char        **string
    );

apr_status_t amq_stdc_table_get_integer (
    amq_stdc_table_t  table,
    const char        *name,
    long              *integer
    );

apr_status_t amq_stdc_table_get_decimal (
    amq_stdc_table_t  table,
    const char        *name,
    byte              *decimals,
    long              **value
    );

apr_status_t amq_stdc_table_get_time (
    amq_stdc_table_t  table,
    const char        *name,
    time_t            *time
    );

dbyte amq_stdc_table_size (
    amq_stdc_table_t  table
    )
{
    if (!table)
        return 0;
    return table->size;
}

const char *amq_stdc_table_data (
    amq_stdc_table_t  table
    )
{
    if (!table)
        return NULL;
    return table->data;
}

apr_status_t amq_stdc_table_destroy (
    amq_stdc_table_t  table
    )
{
    if (table->data)
        amq_free ((void*) (table->data));
    amq_free ((void*) table);

    return APR_SUCCESS;
}
