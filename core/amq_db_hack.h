/*===========================================================================
 *
 *  amq_db_hack.h   Temporary work-arounds for missing functions
 *
 *  Copyright (c) 2004 JPMorgan
 *===========================================================================*/

/*  Safe/unsafe header inclusion for compatibility with iCL code             */
#if !defined (ICL_IMPORT_HEADERS) || (ICL_IMPORT_HEADERS == 1)
# ifndef INCLUDE_AMQ_DB_HACK_SAFE
#   define INCLUDE_AMQ_DB_HACK_SAFE
#   if !defined (ICL_IMPORT_HEADERS)
#       define ICL_IMPORT_AMQ_DB_HACK_ROOT
#       define ICL_IMPORT_HEADERS 1
#   endif

typedef struct _amq_db_tx_t amq_db_tx_t;

#   if defined (ICL_IMPORT_AMQ_DB_HACK_ROOT)
#       undef  ICL_IMPORT_HEADERS
#       define ICL_IMPORT_HEADERS 2
#   endif
# endif
#endif

#if !defined (ICL_IMPORT_HEADERS) || (ICL_IMPORT_HEADERS == 2)
# ifndef INCLUDE_AMQ_DB_HACK_UNSAFE
#   define INCLUDE_AMQ_DB_HACK_UNSAFE

struct _amq_db_tx_t {
    amq_db_t *db;
    long      id;
};

/*  Prototypes for methods                                                   */

#ifdef __cplusplus
extern "C" {
#endif

amq_db_tx_t *
     amq_db_tx_new              (amq_db_t *db);
int  amq_db_tx_start            (amq_db_tx_t *tx);
int  amq_db_tx_commit           (amq_db_tx_t *tx);
int  amq_db_tx_rollback         (amq_db_tx_t *tx);
void amq_db_tx_destroy          (amq_db_tx_t **tx);

#ifdef __cplusplus
}
#endif

#   if defined (ICL_IMPORT_AMQ_DB_HACK_ROOT)
#       undef ICL_IMPORT_HEADERS
#       undef ICL_IMPORT_AMQ_DB_HACK_ROOT
#   endif
# endif
#endif
