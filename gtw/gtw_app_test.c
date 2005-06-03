
#include "base.h"
#include "gtw.h"

int ltw_open (JAMQ_tsApicb *pApiHndl, int *aiCode)
{
    *aiCode = 0;
    return OK;
}

int ltw_close (JAMQ_tsApicb *pApiHndl, int *aiCode)
{
    *aiCode = 0;
    return OK;
}

int ltwio_srvc_input (
    JAMQ_tsApicb    *pApiHndl,
    void            *pApplsHndl,
    JAMQ_tsApiDvcb  *pDvHndl,
    int             *aiCode
)
{
    printf ("Message handle called.\n");
    return OK;
}

int coral_reset_order_keys (
    JAMQ_tsApicb    *pApiHndl,
    void            *pApplsHndl,
    JAMQ_tsApiDvcb  *pDvHndl,
    int             *aiCode)
{
    printf ("Reseting order keys.\n");
    return OK;
}

int coral_reset_tables (
    JAMQ_tsApicb    *pApiHndl,
    void            *pApplsHndl,
    JAMQ_tsApiDvcb  *pDvHndl,
    int             *aiCode)
{
    printf ("Reseting tables.\n");
    return OK;
}

static JAMQ_tsUnrefSymInfo
        pUnrefSymArray [] = {
            {"ltw_open", ltw_open},
            {"ltw_close", ltw_close},
            {"ltwio_srvc_input", ltwio_srvc_input},
            {"coral_reset_order_keys", coral_reset_order_keys},
            {"coral_reset_tables", coral_reset_tables}
        };

int application_init (
    JAMQ_tsApicb  *pApiHndl,
    int           *aiCode
    )
{
    return JAMQ_apiu_add_unref_syms_to_hash (pApiHndl, pUnrefSymArray,
        5, aiCode);
}

int application_uninit (
    JAMQ_tsApicb  *pApiHndl,
    int           *aiCode
    )
{
    return JAMQ_apiu_delete_unref_syms_from_hash (pApiHndl, pUnrefSymArray,
        5, aiCode);
}
