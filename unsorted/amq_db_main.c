/*
 *  Sample usage of the bdb generated code from the amq dfl.
 */

#include "amq_db.h"
#include <assert.h>

static void s_print_dest_db (amq_db_t *db);
static void s_print_mesg_db (amq_db_t *db);

/*
 * Insert an item, then retreive it to see if it worked
 */
static void s_test_singleinsert (amq_db_t *db)
{
    amq_db_dest_t *item1, *item2;

    /*  Create two tables                                                    */
    item1 = amq_db_dest_new ();
    item2 = amq_db_dest_new ();

    /*  Insert one item                                                      */
    item1->type = 1;
    strcpy (item1->name, "foo");
    assert (amq_db_dest_insert (db, item1) == 0);

    /*  Fetch the first item by searching for its primary index              */
    item2->id = item1->id;
    assert (amq_db_dest_fetch (db, item2, AMQ_DB_FETCH_EQ) == 0);
    assert (memcmp (item1, item2, sizeof (*item1)) == 0);

    amq_db_dest_destroy (&item1);
    amq_db_dest_destroy (&item2);
}

static void s_test_multipleinsert (amq_db_t *db)
{
    uint idx;
    amq_db_dest_t *item;

    item = amq_db_dest_new ();

    for (idx = 2; idx < 10; idx++) {
        item->type = idx;
        memset (item->name, 0, sizeof (item->name));
        sprintf (item->name, "foo%d", idx);
        assert (amq_db_dest_insert (db, item) == 0);
    }

    /* Print them out                                                        */
    s_print_dest_db (db);

    amq_db_dest_destroy (&item);
}

static void s_test_secondaryfetch (amq_db_t *db)
{
    amq_db_dest_t *item;

    item = amq_db_dest_new ();

    /* Look for one of them by secondary id                                  */
    item->type = 2;
    sprintf (item->name, "foo2");
    assert (amq_db_dest_fetch_byname (db, item, AMQ_DB_FETCH_GT) == 0);
    printf ("Fetched GT 2:foo2: id: %ld type: %d Name: '%s'\n",
            item->id, item->type, item->name);

    amq_db_dest_destroy (&item);
}

static void s_test_deletefast (amq_db_t *db)
{
    amq_db_dest_t *item;

    item = amq_db_dest_new ();

    assert (amq_db_dest_fetch_byname (db, item, AMQ_DB_FETCH_FIRST) == 0);
    /* Delete a couple manually */
    printf ("Going to delete_fast: %ld, %ld\n", item->id, item->id + 1);
    assert (amq_db_dest_delete_fast (db, item->id) == 0);
    assert (amq_db_dest_delete_fast (db, item->id + 1) == 0);
    printf ("What's left after the delete fast:\n");
    s_print_dest_db (db);

    amq_db_dest_destroy (&item);
}

static void s_test_purge (amq_db_t *db)
{
    /* Delete them all                                                       */
    printf ("Going to purge all\n");
    assert (amq_db_dest_purge (db) == 0);

    /* Print them out */
    printf ("Deleted ... What's left:\n");
    s_print_dest_db (db);
}

static void s_test_txns (amq_db_t *db)
{
    uint idx = 0;
    amq_db_dest_t *item;
    amq_db_txn_t *txn;

    item = amq_db_dest_new ();
    txn  = amq_db_txn_new (db);

    /* Insert a bunch of stuff in a transaction                              */
    amq_db_txn_start (txn);
    for (idx = 1; idx < 5; idx++) {
        item->type = idx;
        memset (item->name, 0, sizeof (item->name));
        sprintf (item->name, "foo%d", idx);
        assert (amq_db_dest_insert (db, item) == 0);
    }

    printf ("Reading inside of the transaction:\n");
    s_print_dest_db (db);

#if 0 //this locks..
    printf ("Reading outside of a transaction:\n");
    amq_db_txn_set (db, NULL);
    s_print_dest_db (db);
#endif

    assert (amq_db_txn_commit (txn) == 0);
    printf ("Comitted:\n");
    s_print_dest_db (db);

    /*  Now add some more stuff, and rollback                                */
    amq_db_txn_start (txn);
    for (idx = 5; idx < 10; idx++) {
        item->type = idx;
        memset (item->name, 0, sizeof (item->name));
        sprintf (item->name, "foo%d", idx);
        assert (amq_db_dest_insert (db, item) == 0);
    }

    printf ("Reading inside of the transaction:\n");
    s_print_dest_db (db);

    assert (amq_db_txn_rollback (txn) == 0);
    printf ("After rollback of this transaction:\n");
    s_print_dest_db (db);

    amq_db_txn_destroy (&txn);
    amq_db_dest_destroy (&item);
}

/*  Same as above, but for queues                                            */
static void s_test_qsingleinsert (amq_db_t *db)
{
    amq_db_mesg_t *item1, *item2;

    /*  Create two tables                                                    */
    item1 = amq_db_mesg_new ();
    item2 = amq_db_mesg_new ();

    /*  Insert one item                                                      */
    item1->destid = 1;
    assert (amq_db_mesg_append (db, item1) == 0);

    /*  Fetch the first item by searching for its primary index              */
    item2->id = item1->id;
    assert (amq_db_mesg_fetch (db, item2, AMQ_DB_FETCH_EQ) == 0);
    assert (memcmp (item1, item2, sizeof (amq_db_mesg_t)) == 0);
    printf ("Inserted and retreived items match!\n");

    amq_db_mesg_destroy (&item1);
    amq_db_mesg_destroy (&item2);
}

static void s_test_qmultipleinsert (amq_db_t *db)
{
    uint idx;
    amq_db_mesg_t *item;

    item = amq_db_mesg_new ();

    for (idx = 2; idx < 10; idx++) {
        item->destid = idx;
        assert (amq_db_mesg_append (db, item) == 0);
    }

    /* Print them out                                                        */
    s_print_mesg_db (db);
    amq_db_mesg_destroy (&item);
}

static void s_test_qdeletefast (amq_db_t *db)
{
    amq_db_mesg_t *item;

    item = amq_db_mesg_new ();

    assert (amq_db_mesg_fetch (db, item, AMQ_DB_FETCH_FIRST) == 0);
    /* Delete a couple manually */
    printf ("Going to delete_fast: %ld, %ld\n", item->id, item->id + 1);
    assert (amq_db_mesg_delete_fast (db, item->id) == 0);
    assert (amq_db_mesg_delete_fast (db, item->id + 1) == 0);

    printf ("What's left after the delete fast:\n");
    s_print_mesg_db (db);
    amq_db_mesg_destroy (&item);
}

static void s_test_qpop (amq_db_t *db)
{
    amq_db_mesg_t *item1, *item2;

    item1 = amq_db_mesg_new ();
    item2 = amq_db_mesg_new ();

    assert (amq_db_mesg_fetch (db, item1, AMQ_DB_FETCH_FIRST) == 0);
    assert (amq_db_mesg_pop (db, item2) == 0);
    assert (memcmp (item1, item2, sizeof (amq_db_mesg_t)) == 0);
    assert (amq_db_mesg_fetch (db, item2, AMQ_DB_FETCH_FIRST) == 0);
    assert (memcmp (item1, item2, sizeof (amq_db_mesg_t)) != 0);

    amq_db_mesg_destroy (&item1);
    amq_db_mesg_destroy (&item2);
}

static void s_test_qpurge (amq_db_t *db)
{
    /* Delete them all                                                       */
    printf ("Going to purge all\n");
    assert (amq_db_mesg_purge (db) == 0);

    /* Print them out */
    printf ("Deleted ... What's left:\n");
    s_print_mesg_db (db);
}

static void s_test_qtxns (amq_db_t *db)
{
    uint idx = 0;
    amq_db_mesg_t *item;
    amq_db_txn_t *txn;

    item = amq_db_mesg_new ();
    txn  = amq_db_txn_new (db);

    /* Insert a bunch of stuff in a transaction                              */
    amq_db_txn_start (txn);
    for (idx = 1; idx < 5; idx++) {
        item->destid = idx;
        assert (amq_db_mesg_append (db, item) == 0);
    }

    printf ("Reading inside of the transaction:\n");
    s_print_mesg_db (db);

#if 0 //this locks..
    printf ("Reading outside of a transaction:\n");
    amq_db_txn_set (db, NULL);
    s_print_mesg_db (db);
#endif

    assert (amq_db_txn_commit (txn) == 0);
    printf ("Comitted:\n");
    s_print_mesg_db (db);

    /*  Now add some more stuff, and rollback                                */
    amq_db_txn_start (txn);
    for (idx = 5; idx < 10; idx++) {
        item->destid = idx;
        assert (amq_db_mesg_append (db, item) == 0);
    }

    printf ("Reading inside of the transaction:\n");
    s_print_mesg_db (db);

    assert (amq_db_txn_rollback (txn) == 0);
    printf ("After rollback of this transaction:\n");
    s_print_mesg_db (db);

    /*  Now pop something off the queue and try to read from outside the txn */
    amq_db_txn_start (txn);

    assert (amq_db_mesg_pop (db, item) == 0);
    printf ("After pop:\n");
    s_print_mesg_db (db);

    printf ("Reading outside of the transaction:\n");
    amq_db_txn_set (db, NULL);
    s_print_mesg_db (db);

    amq_db_txn_set (db, txn);
    printf ("Added one item:\n");
    item->destid = 99;
    assert (amq_db_mesg_append (db, item) == 0);
    s_print_mesg_db (db);

    /*  This locks                                                           */
    printf ("Reading outside of the transaction:\n");
    amq_db_txn_set (db, NULL);
    s_print_mesg_db (db);

    assert (amq_db_txn_rollback (txn) == 0);
    printf ("After rollback of pop+add:\n");
    s_print_mesg_db (db);

    /*  Fetch the second item in the db and update it                        */
    amq_db_txn_start (txn);
    assert (amq_db_mesg_fetch (db, item, AMQ_DB_FETCH_FIRST) == 0);
    assert (amq_db_mesg_fetch (db, item, AMQ_DB_FETCH_NEXT) == 0);
    item->destid = 666;
    assert (amq_db_mesg_update (db, item) == 0);

    printf ("Updated an item in the queue:\n");
    s_print_mesg_db (db);

    printf ("Reading outside of the transaction:\n");
    amq_db_txn_set (db, NULL);
    s_print_mesg_db (db);

    amq_db_txn_rollback (txn);

    amq_db_txn_destroy (&txn);
    amq_db_mesg_destroy (&item);
}


int main (void)
{
    amq_db_t *db, *mesgdb;

    db = amq_db_new ("test.db");
    assert (db);

    printf ("\n** Testing BTree table **\n");
    s_test_singleinsert (db);
    s_test_multipleinsert (db);
    s_test_secondaryfetch (db);
    s_test_deletefast (db);
    s_test_purge (db);
    s_test_txns (db);
    amq_db_dest_purge (db);

    printf ("\n** Testing Queue table **\n");
    mesgdb = amq_db_mesg_table_new (db, "fooo");
    assert (mesgdb == amq_db_mesg_table_lookup (mesgdb, "fooo"));

    s_test_qsingleinsert (mesgdb);
    s_test_qmultipleinsert (mesgdb);
    s_test_qdeletefast (mesgdb);
    s_test_qpop (mesgdb);
    s_test_qpurge (mesgdb);
    s_test_qtxns (mesgdb);

    amq_db_mesg_purge (mesgdb);
    amq_db_mesg_table_destroy (&mesgdb);

    amq_db_destroy (&db);
    ipr_mem_assert ();
    return 0;
}

static void s_print_dest_db (amq_db_t *db)
{
    int rc = 1;
    amq_db_dest_t *item;
    amq_db_txn_t *txn;
    char islocal = 0;

    item = amq_db_dest_new ();

    if (!amq_db_txn_get (db)) {
        txn = amq_db_txn_new (db);
        amq_db_txn_start (txn);
        islocal = 1;
    }

    rc = amq_db_dest_fetch (db, item, AMQ_DB_FETCH_FIRST);
    while (rc == 0) {
        printf ("id: %ld type: %d Name: '%s'\n",
                item->id, item->type, item->name);
        rc = amq_db_dest_fetch (db, item, AMQ_DB_FETCH_NEXT);
    }

    if (islocal) {
        amq_db_txn_rollback (txn);
        amq_db_txn_destroy (&txn);
    }

    amq_db_dest_destroy (&item);

    assert (rc == AMQ_DB_NOTFOUND);
}

static void s_print_mesg_db (amq_db_t *db)
{
    int rc = 1;
    amq_db_mesg_t *item;
    amq_db_txn_t *txn;
    char islocal = 0;

    item = amq_db_mesg_new ();
    if (!amq_db_txn_get (db)) {
        txn = amq_db_txn_new (db);
        amq_db_txn_start (txn);
        islocal = 1;
    }

    rc = amq_db_mesg_fetch (db, item, AMQ_DB_FETCH_FIRST);
    while (rc == 0) {
            printf ("id: %ld destid: %ld\n", item->id, item->destid);
            rc = amq_db_mesg_fetch (db, item, AMQ_DB_FETCH_NEXT);
    }

    if (islocal) {
        amq_db_txn_rollback (txn);
        amq_db_txn_destroy (&txn);
    }

    amq_db_mesg_destroy (&item);

    assert (rc == AMQ_DB_NOTFOUND);
}
