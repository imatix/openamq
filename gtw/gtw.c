
#include "gtw.h"

#include "base.h"
#include "gtw_ll.h"
#include "gtw_hash.h"
#include "gtw_tbl.h"
#include "gtw_oum.h"

int JAMQ_ll_open (
    void             **ap_link_List,
    JAMQ_tsLlParams  *ll_pParams,
    int              *aireturn_Code
    )
{
    return gtw_ll_open ((gtw_ll_t**) ap_link_List, ll_pParams, aireturn_Code);
}

int JAMQ_ll_close (
    void  **ap_link_List,
    int   *aireturn_Code
    )
{
    return gtw_ll_close ((gtw_ll_t**) ap_link_List, aireturn_Code);
}

int JAMQ_ll_delete_item(
    void            *p_link_List,
    JAMQ_tsNCharcb  *p_ll_Key,
    void            **ap_ll_Item,
    int             *aireturn_Code
    )
{
    return gtw_ll_delete ((gtw_ll_t*) p_link_List, p_ll_Key, ap_ll_Item,
        aireturn_Code);
}

int JAMQ_ll_insert_item (
    void            *p_link_List,
    JAMQ_tsNCharcb  *p_ll_Key,
    void            *p_ll_ItemIn,
    int             *aireturn_Code
    )
{
    return gtw_ll_insert ((gtw_ll_t*) p_link_List, p_ll_Key, p_ll_ItemIn,
        aireturn_Code);
}

int JAMQ_ll_item_count (
    void * p_link_List
    )
{
    return gtw_ll_count ((gtw_ll_t*) p_link_List);
}

int JAMQ_ll_keyed_first_item (
    void            *p_link_List,
    JAMQ_tsNCharcb  *p_ll_KeyIn,
    void            **ap_ll_Item,
    int             *aireturn_Code
    )
{
    return gtw_ll_keyed_first_item ((gtw_ll_t*) p_link_List, p_ll_KeyIn,
        ap_ll_Item, aireturn_Code);
}

int JAMQ_ll_seq_first_item (
    void           *p_link_List,
    JAMQ_tsNCharcb *p_ll_Key,
    void           **ap_ll_Item,
    int            *aireturn_Code
    )
{
    return gtw_ll_seq_first_item ((gtw_ll_t*) p_link_List, p_ll_Key,
        ap_ll_Item, aireturn_Code);
}

int JAMQ_ll_seq_next_item (
    void            *p_link_List,
    JAMQ_tsNCharcb  *p_ll_Key,
    void            **ap_ll_Item,
    int             *aireturn_Code
    )
{
    return gtw_ll_seq_next_item ((gtw_ll_t*) p_link_List, p_ll_Key,
        ap_ll_Item, aireturn_Code);
}

int JAMQ_ll_set_key (
    void            *p_link_List,
    JAMQ_tsNCharcb  *p_ll_KeyOut,
    JAMQ_tsNCharcb  *p_ll_KeyIn,
    int             *aireturn_Code
    )
{
    return gtw_ll_set_key ((gtw_ll_t*) p_link_List, p_ll_KeyOut, p_ll_KeyIn,
        aireturn_Code);
}

/*---------------------------------------------------------------------------*/

int JAMQ_hash_open (
    void               **aphash_Hndl,
    JAMQ_tsHashParams  *hash_init_Params,
    int                *aireturn_Code
    )
{
    return gtw_hash_open ((gtw_hash_t**) aphash_Hndl, hash_init_Params,
        aireturn_Code);
}

int JAMQ_hash_close(
    void  **aphash_Hndl,
    int   *aireturn_Code
    )
{
    return gtw_hash_close ((gtw_hash_t**) aphash_Hndl, aireturn_Code);
}

int JAMQ_hash_add (
    void            *phash_Hndl,
    JAMQ_tsNCharcb  *phash_Key,
    void            *phash_Item,
    int             *aireturn_Code
    )
{
    return gtw_hash_add ((gtw_hash_t*) phash_Hndl, phash_Key, phash_Item,
        aireturn_Code);
}

int JAMQ_hash_delete (
    void            *phash_Hndl,
    JAMQ_tsNCharcb  *phash_KeyIn,
    JAMQ_tsNCharcb  *phash_KeyOut,
    void            **aphash_Item,
    int             *aireturn_Code
    )
{
    return gtw_hash_delete ((gtw_hash_t*) phash_Hndl, phash_KeyIn,
        phash_KeyOut, aphash_Item, aireturn_Code);
}

int JAMQ_hash_find(
    void            *phash_Hndl,
    JAMQ_tsNCharcb  *phash_Key,
    void            **aphash_Item,
    int             *aireturn_Code
    )
{
    return gtw_hash_find ((gtw_hash_t*) phash_Hndl, phash_Key, aphash_Item,
        aireturn_Code);
}

int JAMQ_hash_first_item (
    void            *phash_Hndl,
    JAMQ_tsNCharcb  *phash_Key,
    void            **aphash_Item,
    int             *aireturn_Code
    )
{
    return gtw_hash_first_item ((gtw_hash_t*) phash_Hndl, phash_Key,
        aphash_Item, aireturn_Code);
}

int JAMQ_hash_next_item (
    void            *phash_Hndl,
    JAMQ_tsNCharcb  *phash_Key,
    void            **aphash_Item,
    int             *aireturn_Code
    )
{
    return gtw_hash_next_item ((gtw_hash_t*) phash_Hndl, phash_Key,
        aphash_Item, aireturn_Code);
}

/*---------------------------------------------------------------------------*/

int JAMQ_tbl_open (
    void              **apTableHandle,
    JAMQ_tsTblParams  *pTableParams,  
    int               *aireturn_Code
    )
{
    return gtw_tbl_open ((gtw_tbl_t**) apTableHandle, pTableParams,
        aireturn_Code);
}

int JAMQ_tbl_close(
    void  **apTableHandle,
    int   *aireturn_Code
    )
{
    return gtw_tbl_close ((gtw_tbl_t**) apTableHandle, aireturn_Code);
}

int JAMQ_tbl_load_ssv (
    void            *pTableHandle,
    JAMQ_tsNCharcb  *pJAMQ_FileName,
    int             iJAMQ_FileType, 
    int             *aireturn_Code
    )
{
    return gtw_tbl_load_ssv ((gtw_tbl_t*) pTableHandle, pJAMQ_FileName,
        iJAMQ_FileType, aireturn_Code);
}

int JAMQ_tbl_load_tbl (
    void            *pTableHandle,
    JAMQ_tsNCharcb  *pJAMQ_FileName,
    int             *aireturn_Code
    )
{
    return gtw_tbl_load_tbl ((gtw_tbl_t*) pTableHandle, pJAMQ_FileName,
        aireturn_Code);
}

int JAMQ_tbl_find_first_match (
    void            *pTableHandle,
    int             iTableCol,
    JAMQ_tsNCharcb  *pTableItem,
    int             *aiTableRow,
    int             *aireturn_Code
    )
{
    return gtw_tbl_find_first_match ((gtw_tbl_t*) pTableHandle, iTableCol,
        pTableItem, aiTableRow, aireturn_Code);
}

int JAMQ_tbl_get_item (
    void            *pTableHandle,
    int             iTableRow,
    int             iTableCol,
    JAMQ_tsNCharcb  *pTableItem,
    int             *aireturn_Code
    )
{
    return gtw_tbl_get_item ((gtw_tbl_t*) pTableHandle, iTableRow, iTableCol,
        pTableItem, aireturn_Code);
}

int JAMQ_tbl_update_item (
    void            *pTableHandle,
    int             iTableRow,
    int             iTableCol,
    JAMQ_tsNCharcb  *pTableItem,
    int             *aireturn_Code
    )
{
    return gtw_tbl_update_item ((gtw_tbl_t*) pTableHandle, iTableRow,
        iTableCol, pTableItem, aireturn_Code);
}

int JAMQ_tbl_get_size (
    void  *pTableHandle,
    int   *aiTableRows,
    int   *aiTableCols,
    int   *aireturn_Code
    )
{
    return gtw_tbl_get_size ((gtw_tbl_t*) pTableHandle, aiTableRows,
        aiTableCols, aireturn_Code);
}

int JAMQ_tbl_write_tbl (
    void            *pTableHandle,
    JAMQ_tsNCharcb  *pJAMQFileName, 
    int             *aireturn_Code
    )
{
    return gtw_tbl_write_tbl ((gtw_tbl_t*) pTableHandle, pJAMQFileName,
        aireturn_Code);
}

int JAMQ_tbl_delete_item (
    void  *pTableHandle,
    int   iTableRow,
    int   iTableCol,
    int   *aireturn_Code
    )
{
    return gtw_tbl_delete_item ((gtw_tbl_t*) pTableHandle, iTableRow,
        iTableCol, aireturn_Code);
}

/*---------------------------------------------------------------------------*/

int JAMQ_m_chars_to_double (
    double          *pDouble, 
    JAMQ_tsNCharcb  *pNumber,
    int             *aireturn_Code
    )
{
    qbyte
        pos = 0;
    byte
        negative = 0;
    int
        number;
    int
        fraction1;
    int
        fraction2;

    if (!aireturn_Code)
        return NOT_OK;

    if (!pDouble || !pNumber || pNumber->iDataLen < 0 || !pNumber->pData) {
        *aireturn_Code = JAMQ_MISC_INPUT_ERR;
        return NOT_OK;
    }

    while (1) {
        if (pos == pNumber->iDataLen)
            goto err;
        if ( pNumber->pData [pos] != ' ')
            break; 
        pos++;      
    }

    if (pNumber->pData [pos] == '+')
        negative = 0, pos++;
    else if (pNumber->pData [pos] == '-')
        negative = 1, pos++;

    number = -1;
    while (1) {
        if (pos == pNumber->iDataLen)
            break;
        if (pNumber->pData [pos] < '0' || pNumber->pData [pos] > '9')
            break;
        if (number == -1)
            number = pNumber->pData [pos] - '0';
        else
            number = number * 10 + pNumber->pData [pos] - '0';
        pos++;
    }
    if (number == -1)
        goto err;

    if (pos == pNumber->iDataLen) {

        /*  Integer                                                          */
        *pDouble = (double) number;
    }
    else if (pNumber->pData [pos] == '.') {

        /*  Floating point                                                   */
        pos++;
        fraction1 = 0;
        fraction2 = 1;
        if (pos != pNumber->iDataLen) {
            while (1) {
                if (pos == pNumber->iDataLen)
                    break;
                if (pNumber->pData [pos] < '0' || pNumber->pData [pos] > '9')
                    break;
                fraction1 = fraction1 * 10 + pNumber->pData [pos] - '0';
                pos++;
                fraction2 *= 10;
            }
        }
        *pDouble = (double) number + (double) fraction1 / (double) fraction2;
    }
    else if (pNumber->pData [pos] == ' ') {

        /*  Fraction                                                         */
        while (1) {
            if (pos == pNumber->iDataLen)
                goto err;
            if ( pNumber->pData [pos] != ' ')
                break; 
            pos++;
        }        

        fraction1 = -1;
        while (1) {
            if (pos == pNumber->iDataLen)
                break;
            if (pNumber->pData [pos] < '0' || pNumber->pData [pos] > '9')
                break;
            if (fraction1 == -1)
                fraction1 = pNumber->pData [pos] - '0';
            else
                fraction1 = fraction1 * 10 + pNumber->pData [pos] - '0';
            pos++;
        }
        if (fraction1 == -1)
            goto err;
        if (pNumber->pData [pos++] != '/')
            goto err;
        fraction2 = -1;
        while (1) {
            if (pos == pNumber->iDataLen)
                break;
            if (pNumber->pData [pos] < '0' || pNumber->pData [pos] > '9')
                break;
            if (fraction2 == -1)
                fraction2 = pNumber->pData [pos] - '0';
            else
                fraction2 = fraction2 * 10 + pNumber->pData [pos] - '0';
            pos++;
        }
        if (fraction2 == -1 || fraction2 == 0)
            goto err;
        if (!pos == pNumber->iDataLen)
            goto err;
        *pDouble = (double) number + (double) fraction1 / (double) fraction2;
    }
    else
        goto err;
    
    if (negative)
        *pDouble *= (double) -1;
    *aireturn_Code = 0;
    return OK;

err:
    *aireturn_Code = JAMQ_MISC_INPUT_ERR;
    return NOT_OK;
}


int JAMQ_m_chars_to_int (
    int             *aiInt, 
    JAMQ_tsNCharcb  *pNumber,
    int             *aireturn_Code
    )
{
    qbyte
        pos = 0;
    byte
        negative = 0;
    int
        number;

    if (!aireturn_Code)
        return NOT_OK;

    if (!aiInt || !pNumber || pNumber->iDataLen < 0 || !pNumber->pData) {
        *aireturn_Code = JAMQ_MISC_INPUT_ERR;
        return NOT_OK;
    }

    if (pNumber->pData [pos] == '+')
        negative = 0, pos++;
    else if (pNumber->pData [pos] == '-')
        negative = 1, pos++;

    number = -1;
    while (1) {
        if (pos == pNumber->iDataLen)
            break;
        if (pNumber->pData [pos] < '0' || pNumber->pData [pos] > '9')
            goto err;
        if (number == -1)
            number = pNumber->pData [pos] - '0';
        else
            number = number * 10 + pNumber->pData [pos] - '0';
        pos++;
    }
    if (number == -1)
        goto err;

    *aiInt = negative ? -number : number;
    *aireturn_Code = 0;
    return OK;

err:
    *aireturn_Code = JAMQ_MISC_INPUT_ERR;
    return NOT_OK;
}

int JAMQ_oum_open (
    void              **apoumHandle,
    JAMQ_tsoumParams  *poumParams,
    int               *aireturn_Code
    )
{
    return gtw_oum_open ((gtw_oum_t**) apoumHandle, poumParams, aireturn_Code);
}

int JAMQ_oum_get_nchars (
    void            *poumHandle,
    int             iKey,
    JAMQ_tsNCharcb  *pString,
    int             *aireturn_Code
    )
{
    return gtw_oum_get_nchars ((gtw_oum_t*) poumHandle, iKey, pString,
        aireturn_Code);
}


int JAMQ_oum_get_int_key (
    void            *poumHandle,
    JAMQ_tsNCharcb  *pString,
    int             *pKey,
    int             *aireturn_Code)
{
    return gtw_oum_get_int_key ((gtw_oum_t*) poumHandle, pString, pKey,
        aireturn_Code);
}

int JAMQ_oum_put_int_key (
    void  *poumHandle,
    int   iKey,
    int   *aireturn_Code
    )
{
    return gtw_oum_put_int_key ((gtw_oum_t*) poumHandle, iKey,
        aireturn_Code);
}


int JAMQ_oum_close (
    void  **apoumHandle,
    int   *aireturn_Code
    )
{
    return gtw_oum_close ((gtw_oum_t**) apoumHandle, aireturn_Code);
}
