/*--------------------------------------------------------------------------
 *  amq_db_mesg.h
 *
 *  Header for the mesg table, which defines the table structure
 *  and its access functions.
 *
 *  Copyright (c) 2002-2004 JPMorgan
 *  Parts copyright (c) 1991-2004 iMatix Corporation.
 *--------------------------------------------------------------------------*/

#ifndef _AMQ_DB_MESG_H_
#define _AMQ_DB_MESG_H_

#include "amq_db.h"

typedef struct {
    amq_db_query_t *query;               /*  Contains current query info     */
    qbyte        id;
    qbyte        destid;
    qbyte        clientid;
    dbyte        body_size;
    byte         priority;
    dbyte        expiration;
    ipr_shortstr_t  mime_type;
    ipr_shortstr_t  encoding;
    idb_hackstr_t  identifier;
    idb_hackstr_t  headers;
    idb_hackstr_t  content;
} amq_db_mesg_t;

#ifdef __cplusplus
extern "C" {
#endif
/*
 * Constructors/Destructors
 */
amq_db_mesg_t *amq_db_mesg_new (void);
int amq_db_mesg_destroy (amq_db_mesg_t **table);

/*
 * Access functions
 */
int amq_db_mesg_insert (amq_db_t *db, amq_db_mesg_t *tbl);
int amq_db_mesg_update (amq_db_t *db, amq_db_mesg_t *tbl);
int amq_db_mesg_fetch (amq_db_t *db, amq_db_mesg_t *tbl, uint command);
int amq_db_mesg_delete (amq_db_t *db, amq_db_mesg_t *tbl);
int amq_db_mesg_purge (amq_db_t *db);

int amq_db_mesg_fetch_bydest (amq_db_t *db, amq_db_mesg_t *tbl, uint command);

/*
 * Private internal constructor [may go away]
 */
int amq_db_mesg_init (amq_db_t *db, uint idx, uint numpdbs, uint *sidxoffset, uint seqdbidx);

#ifdef __cplusplus
}
#endif

#endif
