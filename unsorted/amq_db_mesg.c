/*--------------------------------------------------------------------------
 *  amq_db_mesg.c
 *
 *  This file is the implementation of the mesg table's access
 *  functions.
 *
 *  Copyright (c) 2002-2004 JPMorgan
 *  Parts copyright (c) 1991-2004 iMatix Corporation.
 *--------------------------------------------------------------------------*/

#include "amq_db_mesg.h"

static amq_db_query_t *s_query_new (void);
static int s_query_destroy (amq_db_query_t **query);

static int s_extractkey_mesg_bydest (
                                                     DB *dbp,
                                                     const DBT *pkey,
                                                     const DBT *pdata,
                                                     DBT *skey);
#if 0
static int s_keycmp_mesg_bydest (
                                                 DB *dbp,
                                                 const DBT *dbt1,
                                                 const DBT *dbt2);
#endif
static int s_is_mesg_bydest_invalid (amq_db_mesg_t *item);


static int s_is_mesg_index_invalid (amq_db_mesg_t *item);

/*
 *  Internal table initialization function -- this might go away.
 */
int amq_db_mesg_init (amq_db_t *db, uint idx, uint numdbs, uint *sidxcount, uint seqdbidx)
{
    int rc;
    DB *pdbp;
    DB *sdbp;
    DB_SEQUENCE *dbseq;
    qbyte id;
    DBT seqkey;

    /* Init the table itself                                                 */
    rc = db_create (&db->dbp [idx], db->dbenv, 0);
    if (rc) {
        S_ERR (db, rc, "Unable to create db handle");
        return AMQ_DB_EINVAL;
    }

    db->dbname [idx] = "mesg";
    pdbp = db->dbp [idx];

    /*  Open it XXX -- add txnid                                         */
    /*
     *  We are opening a BTREE type DB.  We create it if necessary,
     *  have support for free threading
     */
    rc = pdbp->open (pdbp, NULL, "amq_db.dat", "mesg", DB_BTREE,
                     DB_CREATE | DB_THREAD, 0600);
    if (rc) {
        S_ERR (db, rc, "Unable to create db 'mesg'");
        return AMQ_DB_EINVAL;
    }

    /*
     * Create the bdb sequence that will be used to autogenerate
     * id sequence numbers
     */
    rc = db_sequence_create (&db->dbseq [idx], db->dbp [seqdbidx], 0);
    if (rc) {
        S_ERR (db, rc, "Unable to create sequence for db 'mesg'");
        return AMQ_DB_EINVAL;
    }
    dbseq = db->dbseq [idx];

    rc = dbseq->set_range (dbseq, 0, 4294967295UL);
    if (rc) {
        S_ERR (db, rc, "Unable to set range");
        return AMQ_DB_EINVAL;
    }

    id = idx;
    memset (&seqkey, 0, sizeof (DBT));
    seqkey.data = &id;
    seqkey.size = sizeof (qbyte);

    rc = dbseq->open (dbseq, NULL, &seqkey, DB_CREATE);
    if (rc) {
        S_ERR (db, rc, "Unable to open sequence");
        return AMQ_DB_EINVAL;
    }


    /* Index: bydest                                                         */
    rc = db_create (&db->dbp [numdbs + *sidxcount], db->dbenv, 0);
    if (rc) {
        S_ERR (db, rc, "Error creating secondary index");
        return AMQ_DB_EINVAL;
    }
    sdbp = db->dbp [numdbs + *sidxcount];

    /*  Set appropriate flags on the index                                   */
    /*  Allow the db to have duplicate keys                                  */
    rc = sdbp->set_flags (sdbp, DB_DUPSORT);
    if (rc) {
        S_ERR (db, rc, "Error setting flags on secondary index");
        return AMQ_DB_EINVAL;
    }

    /*  Open it XXX -- add txnid                                             */
    rc = sdbp->open (sdbp, NULL, "amq_db.dat", "bydest", DB_BTREE,
                     DB_CREATE | DB_THREAD, 0600);
    if (rc) {
        S_ERR (db, rc, "Error opening secondary index");
        return AMQ_DB_EINVAL;
    }

    /*  Associate the secondary w/ the primary                           */
    rc = pdbp->associate (pdbp, NULL, sdbp, s_extractkey_mesg_bydest, DB_CREATE);
    if (rc) {
        S_ERR (db, rc, "Error associating index");
        return AMQ_DB_EINVAL;
    }

    /*  Add index name                                                   */
    db->dbname [numdbs + *sidxcount] = "mesg: bydest";
    (*sidxcount)++;

    return 0;
}

/*
 *  Allocates table 'mesg'
 */
amq_db_mesg_t *amq_db_mesg_new (void)
{
    amq_db_mesg_t *table;

    table = ipr_mem_alloc (sizeof (amq_db_mesg_t));
coprintf ("xxx 5: alloc %d", sizeof (amq_db_mesg_t));
    if (table) {
        memset (table, 0, sizeof (amq_db_mesg_t));
        table->query = s_query_new ();
        if (table->query == NULL) {
            ipr_mem_free (table);
            table = NULL;
        }
    }
    return table;
}

static amq_db_query_t *s_query_new (void)
{
    amq_db_query_t *query;

    query = ipr_mem_alloc (sizeof (amq_db_query_t));
    if (query) {
        memset (query, 0, sizeof (amq_db_query_t));
    }
    return query;
}

static int s_query_destroy (amq_db_query_t **query)
{
    if (query == NULL || *query == NULL)
        return -1;

    if ((*query)->cursor)
        (*query)->cursor->c_close ((*query)->cursor);

    ipr_mem_free (*query);
    *query = NULL;
    return 0;
}

/*
 *  Frees the table and sets the pointer to null so as to avoid accidental
 *  dereferences later on.
 */
int amq_db_mesg_destroy (amq_db_mesg_t **table)
{
    if (table == NULL || *table == NULL)
        return -1;

    s_query_destroy (&(*table)->query);
    memset (*table, 0, sizeof (amq_db_mesg_t));
    ipr_mem_free (*table);
    *table = NULL;
    return 0;
}

/*
 *  Simple fetch on primary index.
 */
int amq_db_mesg_fetch (
                                     amq_db_t *db,
                                     amq_db_mesg_t *item,
                                     uint command)
{
    int  rc;
    DB  *dbp;
    DBC *dbc;
    DBT  key, val;

    dbp = db->dbp [2];

    memset (&key, 0, sizeof (DBT));
    memset (&val, 0, sizeof (DBT));

    /* Create the cursor if necessary                                     */
    if (item->query->cursor == NULL) {
        rc = dbp->cursor (dbp, NULL, &item->query->cursor, 0);
        if (rc) {
            S_ERR (db, rc, "Unable to create cursor");
            return AMQ_DB_EINVAL;
        }
    }
    dbc = item->query->cursor;

    /*  Copy primary key over                                             */
    key.data = &item->id;
    key.size = sizeof (qbyte);

    /*
     *  Tell bdb to allocate memory for us when returning the value.
     *  This is necessary since we are operating in a threaded
     *  environment.
     */
    key.flags = DB_DBT_MALLOC;
    val.flags = DB_DBT_MALLOC;

    switch (command)
    {
        case AMQ_DB_FETCH_EQ:
            rc = dbc->c_get (dbc, &key, &val, DB_SET);
            break;

        case AMQ_DB_FETCH_GE:
            rc = dbc->c_get (dbc, &key, &val, DB_SET_RANGE);
            break;

        case AMQ_DB_FETCH_GT:
            rc = dbc->c_get (dbc, &key, &val, DB_SET_RANGE);
            if (rc)
                break;

            rc = dbc->c_get (dbc, &key, &val, DB_NEXT_NODUP);
            break;

        case AMQ_DB_FETCH_LE:
            rc = dbc->c_get (dbc, &key, &val, DB_SET_RANGE);
            if (rc == DB_NOTFOUND) {
                /*  Nothing GE was found -- return last item then            */
                rc = dbc->c_get (dbc, &key, &val, DB_LAST);
                break;
            }

            if (rc != 0)
                break;

            /*  We found something. If it's EQ, good.  if it's GT, go back   */
            if (memcmp (&item->id,
                        &((amq_db_mesg_t *)key.data)->id,
                        sizeof (qbyte)) == 0)
                break;

            rc = dbc->c_get (dbc, &key, &val, DB_PREV);
            break;

        case AMQ_DB_FETCH_LT:
            rc = dbc->c_get (dbc, &key, &val, DB_SET_RANGE);
            if (rc == DB_NOTFOUND) {
                /*  Nothing GE was found, return LT then                    */
                rc = dbc->c_get (dbc, &key, &val, DB_LAST);
                break;
            }

            if (rc != 0)
                break;

            /*  We found something, it will be EQ or GE, so we go back      */
            rc = dbc->c_get (dbc, &key, &val, DB_PREV);
            break;

        case AMQ_DB_FETCH_FIRST:
            /* XXX works only for primary id */
            rc = dbc->c_get (dbc, &key, &val, DB_FIRST);
            break;

        case AMQ_DB_FETCH_NEXT:
            rc = dbc->c_get (dbc, &key, &val, DB_NEXT);
            break;

        case AMQ_DB_FETCH_PREV:
            rc = dbc->c_get (dbc, &key, &val, DB_PREV);
            break;

        case AMQ_DB_FETCH_LAST:
            rc = dbc->c_get (dbc, &key, &val, DB_LAST);
            break;

        default:
            S_ERRX (db, "Invalid fetch command '%d'", command);
            rc = AMQ_DB_EINVAL;
    }

    if (rc) {
        if (rc == DB_NOTFOUND)
            return AMQ_DB_ENOTFOUND;
        S_ERR (db, rc, "Error fetching");
        return AMQ_DB_EINVAL;
    }

    /*  Copy fetched item into result arg                                 */
    memcpy ((void *)item + sizeof (amq_db_query_t *), val.data, val.size);
    if (key.data != &item->id)
        ipr_mem_free (key.data);
    ipr_mem_free (val.data);
    return 0;
}

/*
 *  Inserts item into db.
 */
int amq_db_mesg_insert (
                                      amq_db_t *db,
                                      amq_db_mesg_t *item)
{
    int rc;
    DB *dbp;
    DBT key, val;
    DB_SEQUENCE *dbseq;
    db_seq_t     seqn;

    dbp   = db->dbp [2];
    dbseq = db->dbseq [2];

    /*  Copy the new ID number into the table                              */
    rc = dbseq->get (dbseq, NULL, 1, &seqn, 0);
    if (rc) {
        S_ERR (db, rc, "Unable to get next sequence number");
        return AMQ_DB_EINVAL;
    }
    item->id = seqn;

    /*
     *  Check to see if any of the indexes are zero
     */
    if (s_is_mesg_index_invalid (item)) {
        S_ERRX (db, "Trying to insert a record with an zeroed index field");
        return AMQ_DB_EINVAL;
    }

    /*  Prepare to insert data                                            */
    memset (&key, 0, sizeof (DBT));
    memset (&val, 0, sizeof (DBT));

    /*  What the key is                                                   */
    key.data = &item->id;
    key.size = sizeof (qbyte);

    /*  Data itself                                                       */
    val.data = (void *)item + sizeof (amq_db_query_t *);
    val.size = sizeof (amq_db_mesg_t) - sizeof (amq_db_query_t *);

    /*  XXX add txnid                                                     */
    rc = dbp->put (dbp, NULL, &key, &val, DB_NOOVERWRITE);
    if (rc) {
        S_ERR (db, rc, "Unable to insert data into table '%s'",
               db->dbname [2]);
        return AMQ_DB_EINVAL;
    }
    return 0;
}

/*
 *  Delete item from DB -- looks at the primary key.
 */
int amq_db_mesg_delete (
                                      amq_db_t *db,
                                      amq_db_mesg_t *item)
{
    int  rc;
    DB  *dbp;
    DBT  key;
    DBC *dbc;

    dbp = db->dbp [2];
    dbc = item->query->cursor;

    /* If the cursor is open on something, get rid of it now              */
    if (dbc) {
        rc = dbc->c_close (dbc);
        if (rc) {
            S_ERR (db, rc, "W: Unable to close cursor prior to deletion");
        }
        item->query->cursor = NULL;
    }

    /*  Copy key over                                                     */
    memset (&key, 0, sizeof (DBT));
    key.data = &item->id;
    key.size = sizeof (qbyte);

    rc = dbp->del (dbp, NULL, &key, 0);
    if (rc) {
        if (rc == DB_NOTFOUND)
            return AMQ_DB_ENOTFOUND;

        S_ERR (db, rc, "Unable to delete an item from table '%s'",
               db->dbname [2]);
        return AMQ_DB_EINVAL;
    }
    return 0;
}

/*
 *  Overwrites an item in the DB with a new item.
 */
int amq_db_mesg_update (
                                          amq_db_t *db,
                                          amq_db_mesg_t *item)
{
    int  rc;
    DB  *dbp;
    DBT  key, val;

    dbp = db->dbp [2];

    /*  Prepare to insert data                                            */
    memset (&key, 0, sizeof (DBT));
    memset (&val, 0, sizeof (DBT));

    /*  What the key is                                                   */
    key.data = &item->id;
    key.size = sizeof (qbyte);

    /*  Data itself                                                       */
    val.data = item;
    val.size = sizeof (amq_db_mesg_t);

    /*  XXX add txnid                                                     */
    rc = dbp->put (dbp, NULL, &key, &val, 0);
    if (rc) {
        S_ERR (db, rc, "Unable to update data into table '%s'",
               db->dbname [2]);
        return AMQ_DB_EINVAL;
    }
    return 0;
}

int amq_db_mesg_purge (
                                     amq_db_t *db)
{
    int  rc;
    amq_db_mesg_t *item;

    item = amq_db_mesg_new ();

    while ( (rc = amq_db_mesg_fetch (db, item, AMQ_DB_FETCH_FIRST)) == 0) {
        rc = amq_db_mesg_delete (db, item);
        if (rc)
            break;
    }
    amq_db_mesg_destroy (&item);

    if (rc == DB_NOTFOUND)
        rc = 0;
    return rc;
}


/*
 *  Simple fetch on secondary index 'bydest'
 */
int amq_db_mesg_fetch_bydest (
                                     amq_db_t *db,
                                     amq_db_mesg_t *item,
                                     uint command)
{
    int  rc;
    DB  *dbp;
    DBC *dbc;
    DBT  key, val;
    struct s_idx {
        qbyte destid;
        qbyte id;
    } sidx;

    dbp = db->dbp [5];

    memset (&key, 0, sizeof (DBT));
    memset (&val, 0, sizeof (DBT));

    /* Create the cursor if necessary                                     */
    if (item->query->cursor == NULL) {
        rc = dbp->cursor (dbp, NULL, &item->query->cursor, 0);
        if (rc) {
            S_ERR (db, rc, "Unable to create cursor");
            return AMQ_DB_EINVAL;
        }
    }
    dbc = item->query->cursor;

    /*  Copy key over                                                    */
    memcpy (&sidx.destid, &item->destid, sizeof (qbyte));
    memcpy (&sidx.id, &item->id, sizeof (qbyte));
    key.data = &sidx;
    key.size = sizeof (sidx);

    /*
     *  Tell bdb to allocate memory for us when returning the value.
     *  This is necessary since we are operating in a threaded
     *  environment.
     */
    key.flags = DB_DBT_MALLOC;
    val.flags = DB_DBT_MALLOC;

    switch (command)
    {
        case AMQ_DB_FETCH_EQ:
            rc = dbc->c_get (dbc, &key, &val, DB_SET);
            break;

        case AMQ_DB_FETCH_GE:
            rc = dbc->c_get (dbc, &key, &val, DB_SET_RANGE);
            break;

        case AMQ_DB_FETCH_GT:
            rc = dbc->c_get (dbc, &key, &val, DB_SET_RANGE);
            if (rc)
                break;

            rc = dbc->c_get (dbc, &key, &val, DB_NEXT_NODUP);
            break;

        case AMQ_DB_FETCH_LE:
            rc = dbc->c_get (dbc, &key, &val, DB_SET_RANGE);
            if (rc == DB_NOTFOUND) {
                /*  Nothing GE was found -- return last item then            */
                rc = dbc->c_get (dbc, &key, &val, DB_LAST);
                break;
            }

            if (rc != 0)
                break;

            /*  We found something. If it's EQ, good.  if it's GT, go back   */
            if (1
                && memcmp (&item->destid,
                           &((amq_db_mesg_t *)key.data)->destid,
                           sizeof (qbyte)) == 0
                && memcmp (&item->id,
                           &((amq_db_mesg_t *)key.data)->id,
                           sizeof (qbyte)) == 0
            )
                break;

            rc = dbc->c_get (dbc, &key, &val, DB_PREV);
            break;

        case AMQ_DB_FETCH_LT:
            rc = dbc->c_get (dbc, &key, &val, DB_SET_RANGE);
            if (rc == DB_NOTFOUND) {
                /*  Nothing GE was found, return LT then                    */
                rc = dbc->c_get (dbc, &key, &val, DB_LAST);
                break;
            }

            if (rc != 0)
                break;

            /*  We found something. it will be EQ or GE, so we go back      */
            rc = dbc->c_get (dbc, &key, &val, DB_PREV);
            break;

        case AMQ_DB_FETCH_FIRST:
            rc = dbc->c_get (dbc, &key, &val, DB_FIRST);
            break;

        case AMQ_DB_FETCH_NEXT:
            rc = dbc->c_get (dbc, &key, &val, DB_NEXT);
            break;

        case AMQ_DB_FETCH_PREV:
            rc = dbc->c_get (dbc, &key, &val, DB_PREV);
            break;

        case AMQ_DB_FETCH_LAST:
            rc = dbc->c_get (dbc, &key, &val, DB_LAST);
            break;

        default:
            S_ERRX (db, "Invalid fetch command '%d'", command);
            rc = AMQ_DB_EINVAL;
    }

    if (rc) {
        if (rc == DB_NOTFOUND)
            return AMQ_DB_ENOTFOUND;
        return AMQ_DB_EINVAL;
    }

    /*  Copy fetched item into result arg                                 */
    memcpy ((void *)item + sizeof (amq_db_query_t *), val.data, val.size);
    if (key.data != &sidx)
        ipr_mem_free (key.data);
    ipr_mem_free (val.data);
    return 0;
}

/*
 *  This function extracts the secondary index 'bydest' from
 *  the table 'mesg'.
 */
static int s_extractkey_mesg_bydest (
                                                     DB *dbp,
                                                     const DBT *pkey,
                                                     const DBT *pdata,
                                                     DBT *skey)
{
    uint offset = 0;

    memset (skey, 0, sizeof (DBT));
    skey->flags = DB_DBT_APPMALLOC;
    skey->size += sizeof (qbyte);
    skey->size += sizeof (qbyte);
    skey->data  = ipr_mem_alloc (skey->size);
coprintf ("xxx 7: alloc %d", sizeof (skey->size));

    memcpy (skey->data + offset,
            &(((amq_db_mesg_t *)((void *)pdata->data - sizeof (amq_db_query_t *)))->destid),
            sizeof (qbyte));
    offset += sizeof (qbyte);

    memcpy (skey->data + offset,
            &(((amq_db_mesg_t *)((void *)pdata->data - sizeof (amq_db_query_t *)))->id),
            sizeof (qbyte));
    offset += sizeof (qbyte);

    return 0;
}

/*
 *  This function compares keys to each other.  We have this function
 *  because we don't simply compare keys lexicographically: we support
 *  wildcard values in some fields.
 */
#if 0
static int s_keycmp_mesg_bydest (
                                                 DB *dbp,
                                                 const DBT *dbt1,
                                                 const DBT *dbt2)
{
    /*
     *  dbt1 is the application's supplied key, and dbt2 is the tree's
     *  current key. We return the size of dbt1 relative to dbt2.
     *  Meaning, < 0 if dbt1 < dbt2; 0 if dbt1 == dbt2; > 0 if dbt1 >
     *  dbt2.
     */
    int rc;
    struct {
        qbyte destid;
        qbyte id;
    } *key1, *key2, zero;

    /*
     *  Here we're dealing with a composite field upon which we want to
     *  do wildcard matching.
     */
    key1 = dbt1->data;
    key2 = dbt2->data;

    /*
     *  We only allow wildcard values in the least significant values.
     *  So first we match the most significant field, if that doesn't
     *  match, then we return immediately.
     */
    memset (&zero, 0, sizeof (zero));

    rc = memcmp(&key1->destid, &key2->destid, sizeof (qbyte));
    if (rc != 0)
        return rc;

    /*  This is a wildcard, then we found a match                         */
    if (!memcmp (&key1->id, &zero.id, sizeof (qbyte)))
        return 0;

    rc = memcmp(&key1->id, &key2->id, sizeof (qbyte));
    if (rc != 0)
        return rc;

    return rc;
}
#endif

/*
 *  Prior to insertion, we want to know if a table item does not contain
 *  index fields that are all 0 [or all 1].  The reason is because
 *  the all-zero and all-1 indexes are used for wildcard matching.
 *  This function is specific to the 'bydest' secondary index.
 */
static int s_is_mesg_bydest_invalid (amq_db_mesg_t *item)
{
    struct {
        qbyte destid;
        qbyte id;
    } zero, max;

    /*
     *  We set the structs to 0 and max for ease of comparison
     */
    memset (&zero,    0, sizeof (zero));
    memset (&max , 0xff, sizeof (max));

    if (!memcmp (&zero.destid, &item->destid, sizeof (qbyte)))
        return AMQ_DB_EINVAL;
    if (!memcmp (&max.destid, &item->destid, sizeof (qbyte)))
        return AMQ_DB_EINVAL;

    if (!memcmp (&zero.id, &item->id, sizeof (qbyte)))
        return AMQ_DB_EINVAL;
    if (!memcmp (&max.id, &item->id, sizeof (qbyte)))
        return AMQ_DB_EINVAL;


    return 0;
}


/*
 *  This function looks at all the secondary indices and makes sure that
 *  none of them are all-zero or all-ones.
 */
static int s_is_mesg_index_invalid (amq_db_mesg_t *item)
{
    if (s_is_mesg_bydest_invalid (item))
        return AMQ_DB_EINVAL;

    return 0;
}

