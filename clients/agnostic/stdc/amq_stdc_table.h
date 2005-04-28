
#ifndef AMQ_STDC_TABLE_H_INCLUDED
#define AMQ_STDC_TABLE_H_INCLUDED

typedef struct tag_table_context_t* amq_stdc_table_t;

apr_status_t amq_stdc_table_create (
    dbyte             size,
    const char        *data,
    amq_stdc_table_t  *table
    );

apr_status_t amq_stdc_table_add_string (
    amq_stdc_table_t  table,
    const char        *name,
    dbyte             size,
    const char        *string
    );

apr_status_t amq_stdc_table_add_integer (
    amq_stdc_table_t  table,
    const char        *name,
    long              integer
    );

apr_status_t amq_stdc_table_add_decimal (
    amq_stdc_table_t  table,
    const char        *name,
    byte              decimals,
    long              value
    );

apr_status_t amq_stdc_table_add_time (
    amq_stdc_table_t  table,
    const char        *name,
    time_t            time
    );

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
    );

const char *amq_stdc_table_data (
    amq_stdc_table_t  table
    );

apr_status_t amq_stdc_table_destroy (
    amq_stdc_table_t  table
    );

#endif
