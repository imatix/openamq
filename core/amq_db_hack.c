/*===========================================================================
 *
 *  amq_db_hack.c   Temporary work-arounds for missing functions
 *
 *  Copyright (c) 2004 JPMorgan
 *===========================================================================*/

#include "amq_db.h"
#include "amq_db_hack.h"
#include "amq_classes.h"

amq_db_tx_t *
amq_db_tx_new (amq_db_t *db)
{
    amq_db_tx_t
        *tx;

    tx = icl_mem_alloc (sizeof (amq_db_tx_t));
    tx->db = db;
    tx->id = 12345;
    return (tx);
}

int
amq_db_tx_start (amq_db_tx_t *tx)
{
    return (0);
}

int
amq_db_tx_commit (amq_db_tx_t *tx)
{
    return (0);
}

int
amq_db_tx_rollback (amq_db_tx_t *tx)
{
    return (0);
}

void
amq_db_tx_destroy (amq_db_tx_t **tx)
{
    icl_mem_free (*tx);
    *tx = NULL;
}

