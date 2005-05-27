/*---------------------------------------------------------------------------
 *  amq_stdc_table.c - Field table object implementation
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

static apr_status_t s_table_get_field (
    table_context_t  *table,
    const char       *name,
    char             *type,
    void             **data           /*  Pointer to the memory where        */
                                      /*  field data are stored              */
    )
{
    qbyte
       pos = 0;
    char
       field_type;
    const char
       *field_name;
    byte
       field_name_size;
    dbyte
       dsize;

    while (pos < table->size) {
        if (pos + sizeof (byte) >= table->size)
            AMQ_ASSERT (Invalid field table)
        field_name_size = table->data [pos];
        pos += sizeof (byte);
        if (pos + field_name_size >= table->size)
            AMQ_ASSERT (Invalid field table)
        field_name = table->data + pos;
        pos += field_name_size;
        if (pos + sizeof (byte) >= table->size)
            AMQ_ASSERT (Invalid field table)
        field_type = table->data [pos];
        pos += sizeof (byte);
        if (strlen (name) == field_name_size &&
              memcmp ((void*) field_name, (void*) name, field_name_size) == 0) { 
            if (type)
                *type = field_type;
            if (data)
                *data = table->data + pos;

            return APR_SUCCESS;
        }
  
        /*  Move to the next field                                           */
        switch (field_type)
        {
        case 'S':
            if (pos + sizeof (dbyte) >= table->size)
                AMQ_ASSERT (Invalid field table)
            GET_SHORT (dsize, (byte*) (table->data + pos))
            pos += sizeof (dbyte);
            if (pos + dsize >= table->size)
                AMQ_ASSERT (Invalid field table)
            pos += dsize;
            break;
        case 'I':
            if (pos + sizeof (qbyte) >= table->size)
                AMQ_ASSERT (Invalid field table)
            pos += sizeof (qbyte);
            break;
        case 'D':
            if (pos + sizeof (byte) + sizeof (qbyte) >= table->size)
                AMQ_ASSERT (Invalid field table)
            pos += sizeof (byte) + sizeof (qbyte);
            break;
        case 'T':
            if (pos + sizeof (qbyte) >= table->size)
                AMQ_ASSERT (Invalid field table)
            pos += sizeof (qbyte);
            break;
        default:
            AMQ_ASSERT (Unsupported type in field table)
        }
    }

    /*  Field not found                                                      */
    if (type)
        *type = 0;
    if (data)
        *data = NULL;

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
    PUT_SHORT ((char*) dest, size)
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
    PUT_LONG ((char*) dest, integer)

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
    PUT_LONG ((char*) (dest + sizeof (byte)), (qbyte) value) 

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

    result = s_table_add_field (table, name, 'T', sizeof (long),
        (void**) &dest);
    AMQ_ASSERT_STATUS (result, s_table_add_field)
    PUT_LONG ((char*) dest, (qbyte) time)

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
    )
{
    apr_status_t
        result;
    char
        type;
    void
        *data;
    dbyte
        strsize;

    result = s_table_get_field (table, name, &type, &data);
    if (result != APR_SUCCESS)
        AMQ_ASSERT (Invalid field table)
    if (!data)
        AMQ_ASSERT (Field not found in field table)
    if (type != 'S')
        AMQ_ASSERT (Field in field table has noncoforming type)

    GET_SHORT (strsize, (byte*) data)
    if (size)
        *size = strsize;
    if (string)
        *string = (const char*) (data + sizeof (dbyte));

    return APR_SUCCESS;
}

apr_status_t amq_stdc_table_get_integer (
    amq_stdc_table_t  table,
    const char        *name,
    long              *integer
    )
{
    apr_status_t
        result;
    char
        type;
    void
        *data;
 
    result = s_table_get_field (table, name, &type, &data);
    if (result != APR_SUCCESS)
        AMQ_ASSERT (Invalid field table)
    if (!data)
        AMQ_ASSERT (Field not found in field table)
    if (type != 'I')
        AMQ_ASSERT (Field in field table has noncoforming type)

    if (integer)
        GET_LONG (*integer, (byte*) data)
    
    return APR_SUCCESS;
}

apr_status_t amq_stdc_table_get_decimal (
    amq_stdc_table_t  table,
    const char        *name,
    byte              *decimals,
    long              *value
    )
{
    apr_status_t
        result;
    char
        type;
    void
        *data;
 
    result = s_table_get_field (table, name, &type, &data);
    if (result != APR_SUCCESS)
        AMQ_ASSERT (Invalid field table)
    if (!data)
        AMQ_ASSERT (Field not found in field table)
    if (type != 'D')
        AMQ_ASSERT (Field in field table has noncoforming type)

    if (decimals)
        *decimals = *((byte*) data);
    if (value)
        GET_LONG (*value, (byte*) (data + sizeof (byte)))
    
    return APR_SUCCESS;
}

apr_status_t amq_stdc_table_get_time (
    amq_stdc_table_t  table,
    const char        *name,
    time_t            *time
    )
{
    apr_status_t
        result;
    char
        type;
    void
        *data;
 
    result = s_table_get_field (table, name, &type, &data);
    if (result != APR_SUCCESS)
        AMQ_ASSERT (Invalid field table)
    if (!data)
        AMQ_ASSERT (Field not found in field table)
    if (type != 'T')
        AMQ_ASSERT (Field in field table has noncoforming type)

    if (time)
        GET_LONG (*time, (byte*) data)
    
    return APR_SUCCESS;
}

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
