
#include "gtw.h"

#include "base.h"
#include "base_apr.h"
#include "gtw_ll.h"
#include "gtw_hash.h"
#include "gtw_tbl.h"
#include "gtw_oum.h"
#include "gtw_gmm.h"
#include "sfl.h"
#include "amq_stdc_client.h"

#define GET_SHORT(host, net) \
        (host) = (((byte*) net) [0] << 8) + ((byte*) net) [1];

#define PUT_SHORT(net, host) \
        {\
            ((byte*) net) [0] = (byte) (((host) >> 8) & 255); \
            ((byte*) net) [1] = (byte) ((host) & 255);\
        }

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

int JAMQ_m_get_buffer (
    JAMQ_tsBufcb  **apBufferHandle,
    int           ibufferLength, 
    int           *aireturn_Code
    )
{
    if (!aireturn_Code)
        return NOT_OK;

    if (!apBufferHandle || ibufferLength <= 0) {
        *aireturn_Code = JAMQ_MISC_INPUT_ERR;
        return NOT_OK;
    }

    if (*apBufferHandle) {
        *aireturn_Code = JAMQ_MISC_HANDLE_ACTIVE;
        return NOT_OK;
    }

    *apBufferHandle = (JAMQ_tsBufcb*) malloc (sizeof (JAMQ_tsBufcb));
    if (!apBufferHandle) {
       *aireturn_Code = JAMQ_MISC_MEM_ERR;
       return NOT_OK;
    }

    (*apBufferHandle)->iCurrentLen = 0;
    (*apBufferHandle)->iLogicalMaxLen = ibufferLength;
    (*apBufferHandle)->iPhysicalLen = ibufferLength;
    (*apBufferHandle)->iFailed = 0;

    (*apBufferHandle)->pData = (char*) malloc (ibufferLength);
    if (!(*apBufferHandle)->pData) {
        free ((void*) *apBufferHandle);
        *apBufferHandle = NULL;
        *aireturn_Code = JAMQ_MISC_MEM_ERR;
       return NOT_OK;
    }

    *aireturn_Code = 0;
    return OK;
}

int JAMQ_m_put_buffer (
    JAMQ_tsBufcb  **apBufferHandle,
    int           *aireturn_Code
    )
{
    if (!aireturn_Code)
        return NOT_OK;

    if (!apBufferHandle) {
        *aireturn_Code = JAMQ_MISC_INPUT_ERR;
        return NOT_OK;
    }

    if (!*apBufferHandle) {
        *aireturn_Code = JAMQ_MISC_HANDLE_INVALID;
        return NOT_OK;
    }

    free ((void*) ((*apBufferHandle)->pData));
    free ((void*) *apBufferHandle);
    *apBufferHandle = NULL;

    *aireturn_Code = 0;
    return OK;
}

int JAMQ_m_mem_nchar_dup (
    JAMQ_tsNCharcb  *pNCharOut,
    JAMQ_tsNCharcb  *pNCharIn,
    int             *aireturn_Code
    )
{
    char
        *duplicate;

    if (!aireturn_Code)
        return NOT_OK;

    if (!pNCharOut || !pNCharIn ||
          !pNCharIn->pData || pNCharIn->iDataLen <= 0) {
        *aireturn_Code = JAMQ_MISC_INPUT_ERR;
        return NOT_OK;
    }

    duplicate = (char*) malloc (pNCharIn->iDataLen);
    if (!duplicate) {
        *aireturn_Code = JAMQ_MISC_MEM_ERR;
        return NOT_OK;
    }
    memcpy ((void*) duplicate, (void*) pNCharIn->pData, pNCharIn->iDataLen);

    /*  If destination string is already allocated, deallocate it            */
    if (pNCharOut->pData)
        free ((void*) pNCharOut->pData);

    pNCharOut->pData = duplicate;
    pNCharOut->iDataLen = pNCharIn->iDataLen;

    *aireturn_Code = 0;
    return OK;
}

int JAMQ_m_mem_nchar_undup (
    JAMQ_tsNCharcb  *pBuffer,
    int             *aireturn_Code
    )
{
    if (!aireturn_Code)
        return NOT_OK;

    if (!pBuffer || !pBuffer->pData || pBuffer->iDataLen < 0) {
        *aireturn_Code = JAMQ_MISC_INPUT_ERR;
        return NOT_OK;
    }
 
    free ((void*) pBuffer->pData);
    pBuffer->pData = NULL;
    pBuffer->iDataLen = 0;

    *aireturn_Code = 0;
    return OK;
}

/*---------------------------------------------------------------------------*/

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

/*---------------------------------------------------------------------------*/

int JAMQ_gmm_open (
    void  **pGmmHandle, 
    int   *aireturn_Code
    )
{
    return gtw_gmm_open ((gtw_gmm_t**) pGmmHandle, aireturn_Code);
}

int JAMQ_gmm_close (
    void  **pGmmHandle, 
    int   *aireturn_Code
    )
{
    return gtw_gmm_close ((gtw_gmm_t**) pGmmHandle, aireturn_Code);
}

int JAMQ_gmm_set_buffers (
    void          *pGmmHandle, 
    JAMQ_tsBufcb  *pUseBuf,
    JAMQ_tsBufcb  *pParseBuf,
    int           *aireturn_Code
    )
{
    return gtw_gmm_set_buffers ((gtw_gmm_t*) pGmmHandle, pUseBuf, pParseBuf,
        aireturn_Code);
}

int JAMQ_gmm_start_msg (
    void          *pGmmHandle,
    JAMQ_tsBufcb  *pMsgBuffer,
    int           *aireturn_Code
    )
{
    return gtw_gmm_start_msg ((gtw_gmm_t*) pGmmHandle, pMsgBuffer,
        aireturn_Code);
}

int JAMQ_gmm_add_data (
    void            *pGmmHandle, 
    int             iFieldId, 
    int             iDataItemCount, 
    JAMQ_tsNCharcb  *pDataHandle, 
    int             *aireturn_Code
    )
{
    return gtw_gmm_add_data ((gtw_gmm_t*) pGmmHandle, iFieldId, iDataItemCount,
        pDataHandle, aireturn_Code);
}

int JAMQ_gmm_delete_field (
    void  *pGmmHandle, 
    int   iFieldId, 
    int   *aireturn_Code
    )
{
    return gtw_gmm_delete_field ((gtw_gmm_t*) pGmmHandle, iFieldId,
        aireturn_Code);
}

int JAMQ_gmm_get_field_count (
    void  *pGmmHndlIn, 
    int   *iFieldCount,
    int   *aireturn_Code
    )
{
    return gtw_gmm_get_field_count ((gtw_gmm_t*) pGmmHndlIn, iFieldCount,
        aireturn_Code);
}

int JAMQ_gmm_get_field (
    void  *pGmmHandle, 
    int   iFieldId, 
    int   *iDataItemCount, 
    int   *aireturn_Code
    )
{
    return gtw_gmm_get_field ((gtw_gmm_t*) pGmmHandle, iFieldId,
        iDataItemCount, aireturn_Code);
}

int JAMQ_gmm_get_next_field (
    void  *pGmmHandle, 
    int   *iFieldId, 
    int   *iDataItemCount, 
    int   *aireturn_Code
    )
{
    return gtw_gmm_get_next_field ((gtw_gmm_t*) pGmmHandle, iFieldId,
        iDataItemCount, aireturn_Code);
}

int JAMQ_gmm_get_first_field (
    void  *pGmmHandle, 
    int   *iFieldId, 
    int   *iDataItemCount, 
    int   *aireturn_Code
    )
{
    return gtw_gmm_get_first_field ((gtw_gmm_t*) pGmmHandle, iFieldId,
        iDataItemCount, aireturn_Code);
}

int JAMQ_gmm_get_data (
    void            *pGmmHandle, 
    int             iFieldId, 
    int             iDataItem, 
    JAMQ_tsNCharcb  *pData, 
    int             *aireturn_Code
    )
{
    return gtw_gmm_get_data ((gtw_gmm_t*) pGmmHandle, iFieldId,
        iDataItem, pData, aireturn_Code);
}

int JAMQ_gmm_copy_field_as (
    void  *pGmmHandleTo, 
    void  *pGmmHandleFrom, 
    int   iFieldIdTo, 
    int   iFieldIdFrom, 
    int   *aireturn_Code
    )
{
    return gtw_gmm_copy_field_as ((gtw_gmm_t*) pGmmHandleTo,
        (gtw_gmm_t*) pGmmHandleFrom, iFieldIdTo,
        iFieldIdFrom, aireturn_Code);
}

int JAMQ_gmm_copy_msg (
    void  *pGmmHandleTo, 
    void  *pGmmHandleFrom, 
    int   *aireturn_Code
    )
{
    return gtw_gmm_copy_msg ((gtw_gmm_t*) pGmmHandleTo,
        (gtw_gmm_t*) pGmmHandleFrom, aireturn_Code);
}

int JAMQ_gmm_from_cmdline (
    void            *pGmmHandle,
    int             iCharCount,
    JAMQ_tsNCharcb  *pCharHandle,
    JAMQ_tsBufcb    *pBufferHandle,
    int             *aireturn_Code
    )
{
    return gtw_gmm_from_cmdline ((gtw_gmm_t*) pGmmHandle, iCharCount,
        pCharHandle, pBufferHandle, aireturn_Code);
}

int JAMQ_gmm_to_cmdline (
    void          *pGmmHandle, 
    JAMQ_tsBufcb  *pBufferHandle,
    int           *aireturn_Code
    )
{
    return gtw_gmm_to_cmdline ((gtw_gmm_t*) pGmmHandle, pBufferHandle,
        aireturn_Code);
}

int JAMQ_gmm_parse_first_msg (
    void          *pGmmHandle, 
    JAMQ_tsBufcb  *pBufferToBuild, 
    JAMQ_tsBufcb  *pBufferToParse, 
    int           *aireturn_Code
    )
{
    return gtw_gmm_parse_first_msg ((gtw_gmm_t*) pGmmHandle, pBufferToBuild,
        pBufferToParse, aireturn_Code);
}

int JAMQ_gmm_parse_next_msg (
    void          *pGmmHandle, 
    JAMQ_tsBufcb  *pBufferToBuild, 
    int           *aireturn_Code
    )
{
    return gtw_gmm_parse_next_msg ((gtw_gmm_t*) pGmmHandle, pBufferToBuild,
        aireturn_Code);
}

/*---------------------------------------------------------------------------*/

static byte s_initialised = 0;

int JAMQ_pim_init (
    char  **envp, 
    int   *aireturn_Code
    )
{
    apr_status_t
        result;

    if (!aireturn_Code)
        return NOT_OK;

    if (!envp) {
        *aireturn_Code = JAMQ_PIM_BAD_INPUT;
        return NOT_OK;
    }

    result = apr_initialize ();
    if (result != APR_SUCCESS) {
        *aireturn_Code = JAMQ_PIM_SYSTEM_ERROR;
        return NOT_OK;
    }

    s_initialised = 1;

    *aireturn_Code = 0;
    return OK;
}

int JAMQ_pim_uninit (
    int  *aireturn_Code
    )
{
    if (!aireturn_Code)
        return NOT_OK;

    if (!s_initialised) {
        *aireturn_Code = JAMQ_PIM_MISC_ERR;
        return NOT_OK;
    }

    apr_terminate();

    s_initialised = 0;

    *aireturn_Code = 0;
    return OK;
}

int JAMQ_pim_time_open (
    void  **apTimeHandle, 
    int   *aireturn_Code
    )
{
    if (!aireturn_Code)
        return NOT_OK;

    if (!apTimeHandle) {
        *aireturn_Code = JAMQ_PIM_BAD_INPUT;
        return NOT_OK;
    }

    if (*apTimeHandle) {
        *aireturn_Code = JAMQ_PIM_HANDLE_ACTIVE;
        return NOT_OK;
    }

    *apTimeHandle = malloc (sizeof (apr_time_t));
    if (!*apTimeHandle) {
        *aireturn_Code = JAMQ_PIM_MEM_ERR;
        return NOT_OK;
    }

    *((apr_time_t*) *apTimeHandle) = apr_time_now ();

    *aireturn_Code = 0;
    return OK;
}

int JAMQ_pim_time_record (
    void  *pTimeHandle, 
    int   *aireturn_Code
    )
{
    if (!aireturn_Code)
        return NOT_OK;

    if (!pTimeHandle) {
        *aireturn_Code = JAMQ_PIM_HANDLE_INVALID;
        return NOT_OK;
    }

    *((apr_time_t*) pTimeHandle) = apr_time_now ();

    *aireturn_Code = 0;
    return OK;
}

int JAMQ_pim_time_delta (
    double  *adSecs,
    void    *pMinuendTmHndlIn,
    void    *pSubtrahendTmHndlIn,
    int     *aireturn_Code
    )
{
    if (!aireturn_Code)
        return NOT_OK;

    if (!adSecs || !pMinuendTmHndlIn || !pSubtrahendTmHndlIn) {
        *aireturn_Code = JAMQ_PIM_BAD_INPUT;
        return NOT_OK;
    }

    *adSecs = (*((apr_time_t*) pMinuendTmHndlIn) -
        *((apr_time_t*) pSubtrahendTmHndlIn)) / 1000000;

    *aireturn_Code = 0;
    return OK;
}

int JAMQ_pim_time_close (
    void  **pTimeHandle,
    int   *aireturn_Code
    )
{
    if (!aireturn_Code)
        return NOT_OK;

    if (!pTimeHandle) {
        *aireturn_Code = JAMQ_PIM_BAD_INPUT;
        return NOT_OK;
    }

    if (!*pTimeHandle) {
        *aireturn_Code = JAMQ_PIM_HANDLE_INVALID;
        return NOT_OK;
    }

    free ((void*) *pTimeHandle);
    *pTimeHandle = NULL;

    *aireturn_Code = 0;
    return OK;
}

int JAMQ_pim_sleep (
    double  dSeconds,
    int     *aireturn_Code
    )
{
    if (!aireturn_Code)
        return NOT_OK;

    if (dSeconds <= 0) {
        *aireturn_Code = JAMQ_PIM_BAD_INPUT;
        return NOT_OK;
    }

    apr_sleep (dSeconds * 1000000);

    *aireturn_Code = 0;
    return OK;
}

int JAMQ_pim_mem_get (
    void  *pMemHandleIn,
    void  **pPtr,
    int   *aiLenGiven,
    int   iLenNeeded,
    int   *aireturn_Code
    )
{
    if (!aireturn_Code)
        return NOT_OK;

    if (pMemHandleIn || !pPtr || !aiLenGiven || iLenNeeded <= 0) {
        *aireturn_Code = JAMQ_PIM_BAD_INPUT;
        return NOT_OK;
    }

    *pPtr = malloc (iLenNeeded);
    if (!pPtr) {
        *aireturn_Code = JAMQ_PIM_MEM_ERR;
        return NOT_OK;
    }
    *aiLenGiven = iLenNeeded;

    *aireturn_Code = 0;
    return OK;
}

int JAMQ_pim_mem_put (
    void  *pMemHandle,
    void  **pPtr,
    int   *aireturn_Code
    )
{
    if (!aireturn_Code)
        return NOT_OK;

    if (pMemHandle || !pPtr || !*pPtr) {
        *aireturn_Code = JAMQ_PIM_BAD_INPUT;
        return NOT_OK;
    }

    free (*pPtr);

    *aireturn_Code = 0;
    return OK;
}

int JAMQ_pim_fd_open (
    void             **pDeviceHandle,
    JAMQ_tsFdParams  *pParams,
    int              *aireturn_Code
    )
{
    char
        *path;

    if (!aireturn_Code)
        return NOT_OK;

    if (!pDeviceHandle || !pParams ||
          pParams->iIoType != JAMQ_PIM_IO_INPUT_TYPE ||
          pParams->iFileOrg != JAMQ_PIM_IO_TEXT_ORG ||
          pParams->sFileName.iDataLen < 0 ||
          !pParams->sFileName.pData) {
        *aireturn_Code = JAMQ_PIM_BAD_INPUT;
        return NOT_OK;
    }

    if (*pDeviceHandle) {
        *aireturn_Code = JAMQ_PIM_HANDLE_ACTIVE;
        return NOT_OK;
    }

    path = (char*) malloc (pParams->sFileName.iDataLen + 1);
    if (!path) {
        *aireturn_Code = JAMQ_PIM_MEM_ERR;
        return NOT_OK;
    }
    memcpy ((void*) path, (void*) pParams->sFileName.pData,
        pParams->sFileName.iDataLen);
    path [pParams->sFileName.iDataLen] = 0;

    *pDeviceHandle = fopen (path, "r");
    free ((void*) path);
    if (!*pDeviceHandle) {
        *aireturn_Code = JAMQ_PIM_SYSTEM_ERROR;
        return NOT_OK;
    }

    *aireturn_Code = 0;
    return OK;
}

int JAMQ_pim_fd_read (
    void          *pDvHndlIn,
    void          *pTmHndl,
    JAMQ_tsBufcb  *pBuffer,
    int           *aireturn_Code
    )
{
    qbyte
        pos = 0;
    char
        c;

    if (!aireturn_Code)
        return NOT_OK;

    if (!pBuffer || !pBuffer->pData || pBuffer->iPhysicalLen < 0) {
        *aireturn_Code = JAMQ_PIM_BAD_INPUT;
        return NOT_OK;
    }

    if (!pDvHndlIn) {
        *aireturn_Code = JAMQ_PIM_HANDLE_INVALID;
        return NOT_OK;
    }

    while (1) {
        c = fgetc ((FILE*) pDvHndlIn);
        if (c == EOF || c == 0x0A || pos == pBuffer->iPhysicalLen)
            break;
        else if (c == 0x0C) {
            c = fgetc ((FILE*) pDvHndlIn);
            break;
        }
        (pBuffer->pData) [pos] = c;
        pos++;
    }
    pBuffer->iCurrentLen = pos;

    *aireturn_Code = 0;
    return c == EOF ? NOT_OK : OK;
}

int JAMQ_pim_fd_close (
    void  **apDvHndl,
    int   *aireturn_Code
    )
{
    if (!aireturn_Code)
        return NOT_OK;

    if (!apDvHndl) {
        *aireturn_Code = JAMQ_PIM_BAD_INPUT;
        return NOT_OK;
    }

    if (!*apDvHndl) {
        *aireturn_Code = JAMQ_PIM_HANDLE_INVALID;
        return NOT_OK;
    }

    if (fclose ((FILE*) *apDvHndl) == EOF) {
        *aireturn_Code = JAMQ_PIM_SYSTEM_ERROR;
        return NOT_OK;
    }
    *apDvHndl = NULL;

    *aireturn_Code = 0;
    return OK;
}

typedef struct tag_gtw_socket_t
{
    apr_socket_t
        *socket;
    apr_pool_t
        *pool;
} gtw_socket_t;

int JAMQ_pim_sd_open (
    void             **apDvHndl,
    JAMQ_tsSdParams  *pParams,
    int              *aireturn_Code
    )
{

    apr_status_t
        result;
    char
        *server = NULL;
    char
        *addr = NULL;
    char
        *scope_id = NULL;
    apr_sockaddr_t
        *sockaddr = NULL;
    apr_socket_t
        *socket = NULL;
    apr_pool_t
        *pool = NULL;
    apr_port_t
        port;
    gtw_socket_t
        *out;

    if (!aireturn_Code)
        return NOT_OK;

    if (!apDvHndl || !pParams || pParams->sSocketName.iDataLen < 0 ||
          !pParams->sSocketName.pData || pParams->sAddress.iDataLen < 0 ||
          !pParams->sAddress.pData || pParams->iProtocol != JAMQ_PIM_TCP ||
          pParams->iIoType != JAMQ_PIM_CLIENT ||
          pParams->iMode != JAMQ_PIM_BLOCKING_MODE ||
          pParams->iAuthRule != JAMQ_PIM_AUTHENTICATE_BY_NAME_OR_ADDR) {
        *aireturn_Code = JAMQ_PIM_BAD_INPUT;
        return NOT_OK;
    }

    if (*apDvHndl) {
        *aireturn_Code = JAMQ_PIM_HANDLE_ACTIVE;
        return NOT_OK;
    }

    result = apr_pool_create (&pool, NULL);
    if (result != APR_SUCCESS) {
        *aireturn_Code = JAMQ_PIM_SYSTEM_ERROR;
        return NOT_OK;
    }

    server = (char*) malloc (pParams->sAddress.iDataLen + 1);
    if (!server) {
        apr_pool_destroy (pool);
        *aireturn_Code = JAMQ_PIM_MEM_ERR;
        return NOT_OK;
    }
    memcpy ((void*) server, (void*) pParams->sAddress.pData,
        pParams->sAddress.iDataLen);
    server [pParams->sAddress.iDataLen] = 0;

    result = apr_parse_addr_port (&addr, &scope_id, &port, server, pool);
    free ((void*) server);
    if (result != APR_SUCCESS) {
        apr_pool_destroy (pool);
        *aireturn_Code = JAMQ_PIM_SYSTEM_ERROR;
        return NOT_OK;
    }

    result = apr_sockaddr_info_get (&sockaddr, addr, AF_UNSPEC, port,
        APR_IPV4_ADDR_OK, pool);
    if (result != APR_SUCCESS) {
        apr_pool_destroy (pool);
        *aireturn_Code = JAMQ_PIM_SYSTEM_ERROR;
        return NOT_OK;
    }

    result = apr_socket_create (&socket, APR_INET, SOCK_STREAM,
        APR_PROTO_TCP, pool);
    if (result != APR_SUCCESS) {
        apr_pool_destroy (pool);
        *aireturn_Code = JAMQ_PIM_SYSTEM_ERROR;
        return NOT_OK;
    }

    result = apr_socket_connect (socket, sockaddr);
    if (result != APR_SUCCESS) {
        apr_pool_destroy (pool);
        *aireturn_Code = JAMQ_PIM_SYSTEM_ERROR;
        return NOT_OK;
    }

    out = (gtw_socket_t*) malloc (sizeof (gtw_socket_t));
    if (!out) {
        apr_pool_destroy (pool);
        *aireturn_Code = JAMQ_PIM_MEM_ERR;
        return NOT_OK;
    }
    
    out->socket = socket;
    out->pool = pool;

    *apDvHndl = (void*) out;

    *aireturn_Code = 0;
    return OK;
}

int JAMQ_pim_sd_read (
    void          *pDvHndlIn,
    void          *pTmHndl,
    JAMQ_tsBufcb  *pBuffer,
    int           *aireturn_Code
    )
{
    apr_status_t
        result;
    apr_size_t
        size;

    if (!aireturn_Code)
        return NOT_OK;

    if (!pDvHndlIn) {
        *aireturn_Code = JAMQ_PIM_HANDLE_INVALID;
        return NOT_OK;
    }

    if (!pBuffer) {
        *aireturn_Code = JAMQ_PIM_BAD_INPUT;
        return NOT_OK;
    }

    size = pBuffer->iPhysicalLen;
    result = apr_socket_recv (((gtw_socket_t*) pDvHndlIn)->socket,
        pBuffer->pData, &size);
    if (result != APR_SUCCESS) {
        *aireturn_Code = JAMQ_PIM_SYSTEM_ERROR;
        return NOT_OK;
    }
    pBuffer->iCurrentLen = size;

    *aireturn_Code = 0;
    return OK;
}

int JAMQ_pim_sd_write (
    void          *pDvHndlIn,
    void          *pTmHndl,
    JAMQ_tsBufcb  *pBuffer,
    int           *aireturn_Code
    )
{
    apr_status_t
        result;
    apr_size_t
        size;

    if (!aireturn_Code)
        return NOT_OK;

    if (!pDvHndlIn) {
        *aireturn_Code = JAMQ_PIM_HANDLE_INVALID;
        return NOT_OK;
    }

    if (!pBuffer) {
        *aireturn_Code = JAMQ_PIM_BAD_INPUT;
        return NOT_OK;
    }

    size = pBuffer->iCurrentLen;
    result = apr_socket_send (((gtw_socket_t*) pDvHndlIn)->socket,
        pBuffer->pData, &size);
    if (result != APR_SUCCESS) {
        *aireturn_Code = JAMQ_PIM_SYSTEM_ERROR;
        return NOT_OK;
    }

    *aireturn_Code = 0;
    return OK;
}

int JAMQ_pim_sd_close (
    void  **apDvHndl,
    int   *aireturn_Code
    )
{
    apr_status_t
        result;

    if (!aireturn_Code)
        return NOT_OK;

    if (!apDvHndl) {
        *aireturn_Code = JAMQ_PIM_BAD_INPUT;
        return NOT_OK;
    }

    if (!*apDvHndl) {
        *aireturn_Code = JAMQ_PIM_HANDLE_INVALID;
        return NOT_OK;
    }

    result = apr_socket_close (((gtw_socket_t*) *apDvHndl)->socket);
    if (result != APR_SUCCESS) {
        *aireturn_Code = JAMQ_PIM_SYSTEM_ERROR;
        return NOT_OK;
    }

    apr_pool_destroy (((gtw_socket_t*) *apDvHndl)->pool);

    *apDvHndl = NULL;

    *aireturn_Code = 0;
    return OK;
}

int JAMQ_pim_log (
    void  *pLogHndlIn,
    int   iPriority,
    int   *aireturn_Code,
    char  *pExpandedMsg,
    char  *pFmtIn,
    ...
    )
{
    va_list args;
    va_start (args, pFmtIn);
    vprintf (pFmtIn, args);
    va_end (args);
    return OK;
}

/*---------------------------------------------------------------------------*/

typedef struct tag_gtw_context_t
{
    JAMQ_tsApicb
        appctx;
    void
        *functions;
    void
        *services;
    amq_stdc_connection_t
        connection;
    amq_stdc_channel_t
        channel;
    dbyte
        handle_id;
    JAMQ_tsApplcb
        appctx_internal;
    char
        queue_name [256];
    JAMQ_tsNCharcb
        running_service;
    JAMQ_tsNCharcb
        reply_to;
} gtw_context_t;

int JAMQ_apiu_open (
    JAMQ_tsApicb    **apApiHndl,
    JAMQ_tsNCharcb  *pClientName,
    int             *aireturn_Code
    )
{
    int
        res = NOT_OK;
    gtw_context_t
        *self;
    apr_status_t
        result;
    apr_pool_t
        *pool;
    char
        *grm_config_filename;
    XML_ITEM
        *grm_config = NULL;
    XML_ITEM
        *item;
    XML_ATTR
        *attr;
    const char
        *server;
    const char
        *port;
    const char
        *vhost;
    JAMQ_tsHashParams
        hparams;
    char
        client_name_buffer [256];
    int
        dummy_retcode;

    if (!aireturn_Code)
        return NOT_OK;

    if (!apApiHndl || !pClientName || !pClientName->pData ||
          pClientName->iDataLen < 0 || pClientName->iDataLen > 255) {
        *aireturn_Code = JAMQ_APIU_INPUT_ERR;
        return NOT_OK;
    }

    if (*apApiHndl) {
        *aireturn_Code = JAMQ_APIU_HANDLE_ACTIVE;
        return NOT_OK;
    }

    /*  Fill in client name buffer                                           */
    memcpy ((void*) client_name_buffer, (void*) pClientName->pData,
        pClientName->iDataLen);
    client_name_buffer [pClientName->iDataLen] = 0;

    /*  Load and parse grm-config file                                       */
    result = apr_pool_create (&pool, NULL);
    if (result != APR_SUCCESS) {
        *aireturn_Code = JAMQ_APIU_APR_ERROR;
        goto err1;
    }
    result = apr_env_get (&grm_config_filename, "GRM_AMQ_CONFIG", pool);
    if (result == APR_ENOENT)
        grm_config_filename = "grm-config.xml";
    else if (result != APR_SUCCESS) {
        *aireturn_Code = JAMQ_APIU_APR_ERROR;
        goto err2;
    }
    if (xml_load (&grm_config, "", grm_config_filename) != XML_NOERROR) {
        *aireturn_Code = JAMQ_APIU_CONFIG_ERROR;
        goto err2;
    }
    item = xml_find_item (grm_config, "/grm-config");
    if (!item) {
        *aireturn_Code = JAMQ_APIU_CONFIG_ERROR;
        goto err3;
    }
    attr = xml_attr (item, "server");
    if (!attr) {
        *aireturn_Code = JAMQ_APIU_CONFIG_ERROR;
        goto err3;
    }
    server = xml_attr_value (attr);
    attr = xml_attr (item, "port");
    if (!attr) {
        *aireturn_Code = JAMQ_APIU_CONFIG_ERROR;
        goto err3;
    }
    port = xml_attr_value (attr);
    attr = xml_attr (item, "virtual-host");
    if (!attr) {
        *aireturn_Code = JAMQ_APIU_CONFIG_ERROR;
        goto err3;
    }
    vhost = xml_attr_value (attr);
    
    /*  Allocate and initialise context                                      */
    self = (gtw_context_t*) malloc (sizeof (gtw_context_t));
    if (!self) {
        *aireturn_Code = JAMQ_APIU_MEM_ERR;
        goto err3;
    }

    /*  Following four fields to be set by main function                     */
    self->appctx.argc = 0;
    self->appctx.argv = NULL;
    self->appctx.sName.pData = NULL;
    self->appctx.sName.iDataLen = 0;
    self->appctx.sStateHndl.iOkToRun = 1;
    self->appctx.sStateHndl.iPermissionLevel = 1;
    self->appctx.pApplHndl = &(self->appctx_internal);
    self->appctx.pGMMReadHndl = NULL;
    self->appctx.pGMMWriteHndl = NULL;
    self->appctx.pTmHndl = NULL;
    self->appctx.pApiDvHndlList = NULL;
    self->appctx.sReadBuf.pData = NULL;
    self->appctx.sReadBuf.iCurrentLen = 0;
    self->appctx.sReadBuf.iPhysicalLen = 0;
    self->appctx.sReadBuf.iLogicalMaxLen = 0;
    self->appctx.sWriteBuf.pData = NULL;
    self->appctx.sWriteBuf.iCurrentLen = 0;
    self->appctx.sWriteBuf.iPhysicalLen = 0;
    self->appctx.sWriteBuf.iLogicalMaxLen = 0;
    self->appctx.sBuildBuf.pData = NULL;
    self->appctx.sBuildBuf.iCurrentLen = 0;
    self->appctx.sBuildBuf.iPhysicalLen = 0;
    self->appctx.sBuildBuf.iLogicalMaxLen = 0;
    self->appctx.sOutputBuf.pData = NULL;
    self->appctx.sOutputBuf.iCurrentLen = 0;
    self->appctx.sOutputBuf.iPhysicalLen = 0;
    self->appctx.sOutputBuf.iLogicalMaxLen = 0;
    self->appctx.sLogBuf.pData = NULL;
    self->appctx.sLogBuf.iCurrentLen = 0;
    self->appctx.sLogBuf.iPhysicalLen = 0;
    self->appctx.sLogBuf.iLogicalMaxLen = 0;
    self->appctx.pLogHndl = NULL;
    self->appctx.pDvHndl = NULL;
    self->functions = NULL;
    self->services = NULL;
    self->connection = NULL;
    self->channel = NULL;
    self->handle_id = 0;

    /*  Following three fields to be set by application                      */
    self->appctx_internal.sName.pData = "";
    self->appctx_internal.sName.iDataLen = 0;
    self->appctx_internal.pApplsHndl = NULL;

    if (JAMQ_gmm_open (&(self->appctx.pGMMReadHndl), aireturn_Code) != OK) {
        *aireturn_Code = JAMQ_APIU_GMM_ERROR;
        goto err4;
    }
    if (JAMQ_gmm_open (&(self->appctx.pGMMWriteHndl), aireturn_Code) != OK) {
        *aireturn_Code = JAMQ_APIU_GMM_ERROR;
        goto err5;
    }
    hparams.iTableSize = 199;
    hparams.sRoutineName.pData = JAMQ_HASH_TOREK_ROUTINE_NAME;
    hparams.sRoutineName.iDataLen = JAMQ_HASH_TOREK_ROUTINE_NAME_LEN;
    hparams.pMemHndl = JAMQ_OS_NO_MEM_MGR;
    if (!JAMQ_hash_open (&(self->functions), &hparams, aireturn_Code)) {
        *aireturn_Code = JAMQ_APIU_HASH_ERROR;
        goto err6;
    }
    if (!JAMQ_hash_open (&(self->services), &hparams, aireturn_Code)) {
        *aireturn_Code = JAMQ_APIU_HASH_ERROR;
        goto err7;
    }

    /*  Initialise AMQ client API, open connection and channel               */
    result = amq_stdc_init ();
    if (result != APR_SUCCESS) {
        *aireturn_Code = JAMQ_APIU_AMQ_ERROR;
        goto err8;
    }
    result = amq_stdc_open_connection (server, atoi (port), vhost,
        client_name_buffer, amq_stdc_heartbeats_off, amq_stdc_heartbeats_off,
        0, 0, NULL, 0, &(self->connection));
    if (result != APR_SUCCESS) {
        *aireturn_Code = JAMQ_APIU_AMQ_ERROR;
        goto err9;
    }
    result = amq_stdc_open_channel (self->connection, 0, 0, 0, NULL, "", 0,
        &(self->channel));
    if (result != APR_SUCCESS) {
        *aireturn_Code = JAMQ_APIU_AMQ_ERROR;
        goto err10;
    }
    result = amq_stdc_open_handle (self->channel, amq_stdc_service_type_queue,
        1, 1, 0, 0, "", "", 0, NULL, 0, NULL, &(self->handle_id));
if (result != APR_SUCCESS) {
        *aireturn_Code = JAMQ_APIU_AMQ_ERROR;
        goto err11;
    }

    *apApiHndl = (JAMQ_tsApicb*) self;
    *aireturn_Code = 0;
    res = OK;
    goto err3;
err11:
    if (amq_stdc_close_channel (self->channel) != APR_SUCCESS &&
          res == OK) {
        *aireturn_Code = JAMQ_APIU_AMQ_ERROR;
        res = NOT_OK;
    }
err10:
    if (amq_stdc_close_connection (self->connection) != APR_SUCCESS &&
          res == OK) {
        *aireturn_Code = JAMQ_APIU_AMQ_ERROR;
        res = NOT_OK;
    }    
err9:
    if (amq_stdc_term () != APR_SUCCESS && res == OK) {
        *aireturn_Code = JAMQ_APIU_AMQ_ERROR;
        res = NOT_OK;
    }
err8:
    if (JAMQ_hash_close (&(self->services), &dummy_retcode) != OK &&
        res == OK) {
        *aireturn_Code = dummy_retcode;
        res = NOT_OK;
    }
err7:
    if (JAMQ_hash_close (&(self->functions), &dummy_retcode) != OK &&
        res == OK) {
        *aireturn_Code = dummy_retcode;
        res = NOT_OK;
    }
err6:
    if (JAMQ_gmm_close (&(self->appctx.pGMMWriteHndl), &dummy_retcode) != OK &&
        res == OK) {
        *aireturn_Code = dummy_retcode;
        res = NOT_OK;
    }
err5:
    if (JAMQ_gmm_close (&(self->appctx.pGMMReadHndl), &dummy_retcode) != OK &&
        res == OK) {
        *aireturn_Code = dummy_retcode;
        res = NOT_OK;
    }
err4:
    free ((void*) self);
err3:
    xml_free (grm_config);
err2:
    apr_pool_destroy (pool);
err1:
    return res;
}

int JAMQ_apiu_close (
    JAMQ_tsApicb  **apApiHndl,
    int           *aireturn_Code
    )
{
    gtw_context_t
        *self;

    if (!aireturn_Code)
        return NOT_OK;

    if (!apApiHndl) {
        *aireturn_Code = JAMQ_APIU_INPUT_ERR;
        return NOT_OK;
    }

    if (!*apApiHndl) {
        *aireturn_Code = JAMQ_APIU_HANDLE_INVALID;
        return NOT_OK;
    }

    self = (gtw_context_t*) *apApiHndl;

    if (amq_stdc_close_handle (self->channel, self->handle_id) != APR_SUCCESS) {
        *aireturn_Code = JAMQ_APIU_AMQ_ERROR;
        return NOT_OK;
    }  
    if (amq_stdc_close_channel (self->channel) != APR_SUCCESS) {
        *aireturn_Code = JAMQ_APIU_AMQ_ERROR;
        return NOT_OK;
    }    
    if (amq_stdc_close_connection (self->connection) != APR_SUCCESS) {
        *aireturn_Code = JAMQ_APIU_AMQ_ERROR;
        return NOT_OK;
    }    
    if (amq_stdc_term () != APR_SUCCESS) {
        *aireturn_Code = JAMQ_APIU_AMQ_ERROR;
        return NOT_OK;
    }
    if (!JAMQ_hash_close (&(self->services), aireturn_Code)) {
        *aireturn_Code = JAMQ_APIU_HASH_ERROR;
        return NOT_OK;
    }
    if (!JAMQ_hash_close (&(self->functions), aireturn_Code)) {
        *aireturn_Code = JAMQ_APIU_HASH_ERROR;
        return NOT_OK;
    }
    if (!JAMQ_gmm_close (&(self->appctx.pGMMWriteHndl), aireturn_Code)) {
        *aireturn_Code = JAMQ_APIU_GMM_ERROR;
        return NOT_OK;
    }
    if (!JAMQ_gmm_close (&(self->appctx.pGMMReadHndl), aireturn_Code)) {
        *aireturn_Code = JAMQ_APIU_GMM_ERROR;
        return NOT_OK;
    }
    free ((void*) self);

    *apApiHndl = NULL;
    *aireturn_Code = 0;
    return OK;
}

int JAMQ_apiu_consume (
    JAMQ_tsApicb    *pApiHndl,
    JAMQ_tsNCharcb  *pQueueName,
    int             iCreate,
    int             *aireturn_Code
    )
{
    gtw_context_t
        *self;
    apr_status_t
        result;
    char
        dest_name_buffer [256];

    if (!aireturn_Code)
        return NOT_OK;

    if (!pApiHndl) {
        *aireturn_Code = JAMQ_APIU_HANDLE_INVALID;
        return NOT_OK;
    }

    if (!pQueueName || pQueueName->iDataLen < 0 || pQueueName->iDataLen > 255 ||
          !pQueueName->pData) {
        *aireturn_Code = JAMQ_APIU_INPUT_ERR;
        return NOT_OK;
    }

    self = (gtw_context_t*) pApiHndl;

    /*  iCreate parameter unused yet, queues must be preconfigured           */
    iCreate = 0;

    /*  Construct destination name                                           */
    memcpy ((void*) dest_name_buffer, (void*) pQueueName->pData,
        pQueueName->iDataLen);
    dest_name_buffer [pQueueName->iDataLen] = 0;

    result = amq_stdc_consume (self->channel, self->handle_id, 1, 0, 1, 1,
        dest_name_buffer, 0, NULL, 0);
    if (result != APR_SUCCESS) {
        *aireturn_Code = JAMQ_APIU_AMQ_ERROR;
        return NOT_OK;
    }

    *aireturn_Code = 0;
    return OK;
}

int JAMQ_apiu_receive_message (
    JAMQ_tsApicb    *pApiHndl,
    JAMQ_tsNCharcb  *pQueueName,
    int             *aireturn_Code
    )
{
    apr_status_t
        result;
    gtw_context_t
        *self;
    amq_stdc_message_t
        message;
    amq_stdc_message_desc_t
        *message_desc;
    dbyte
        iDataLen;
    JAMQ_tsNCharcb
        service;

    if (!aireturn_Code)
        return NOT_OK;

    if (!pApiHndl) {
        *aireturn_Code = JAMQ_APIU_HANDLE_INVALID;
        return NOT_OK;
    }

    if (!pQueueName) {
        *aireturn_Code = JAMQ_APIU_INPUT_ERR;
        return NOT_OK;
    }

    self = (gtw_context_t*) pApiHndl;

    result = amq_stdc_get_message (self->channel, 1, &message_desc, &message);
    if (result != APR_SUCCESS) {
        *aireturn_Code = JAMQ_APIU_AMQ_ERROR;
        return NOT_OK;
    }
    strcpy (self->queue_name, message_desc->dest_name);
    pQueueName->iDataLen = strlen (self->queue_name);
    pQueueName->pData = self->queue_name;

    /*  Set the context for the handler                                      */
    iDataLen = amq_stdc_table_size (message_desc->headers) + sizeof (dbyte);
    if (self->appctx.sBuildBuf.iCurrentLen < iDataLen) {
        self->appctx.sBuildBuf.pData = realloc (self->appctx.sBuildBuf.pData,
            iDataLen);
        self->appctx.sBuildBuf.iLogicalMaxLen = iDataLen;
        self->appctx.sBuildBuf.iPhysicalLen = iDataLen;
        self->appctx.sBuildBuf.iCurrentLen = iDataLen;
        PUT_SHORT (self->appctx.sBuildBuf.pData, iDataLen - sizeof (dbyte))
        memcpy (self->appctx.sBuildBuf.pData + sizeof (dbyte),
            amq_stdc_table_data (message_desc->headers), iDataLen);
    }
    if (self->appctx.sReadBuf.iCurrentLen < iDataLen) {
        self->appctx.sReadBuf.pData = realloc (self->appctx.sReadBuf.pData,
            iDataLen);
        self->appctx.sReadBuf.iLogicalMaxLen = iDataLen;
        self->appctx.sReadBuf.iPhysicalLen = iDataLen;
        self->appctx.sReadBuf.iCurrentLen = iDataLen;
        PUT_SHORT (self->appctx.sReadBuf.pData, iDataLen - sizeof (dbyte))
        memcpy (self->appctx.sReadBuf.pData + sizeof (dbyte),
            amq_stdc_table_data (message_desc->headers), iDataLen);
    }
    self->appctx.sWriteBuf.iCurrentLen = 0;
    if (!JAMQ_gmm_set_buffers (self->appctx.pGMMReadHndl,
          &(self->appctx.sBuildBuf), &(self->appctx.sReadBuf), aireturn_Code))
        return NOT_OK;
    if (!JAMQ_gmm_set_buffers (self->appctx.pGMMWriteHndl,
          &(self->appctx.sWriteBuf), NULL, aireturn_Code))
        return NOT_OK;
    if (!JAMQ_gmm_start_msg (self->appctx.pGMMWriteHndl,
          &(self->appctx.sWriteBuf), aireturn_Code))
        return NOT_OK;

    result = amq_stdc_close_message (message, 0);
    if (result != APR_SUCCESS) {
        *aireturn_Code = JAMQ_APIU_AMQ_ERROR;
        return NOT_OK;
    }

    /*  Set the context with values needed                                   */
    if (!JAMQ_gmm_get_data (self->appctx.pGMMReadHndl, JAMQ_GMM_REQUEST, 0,
          &service, aireturn_Code)) {
        *aireturn_Code = JAMQ_APIU_RUNTIME_ERROR;
        return NOT_OK;
    }
    self->running_service = service;
    if (!JAMQ_gmm_get_data (self->appctx.pGMMReadHndl, JAMQ_GMM_SYSTEM_MSG, 0,
          &(self->reply_to), aireturn_Code)) {
        *aireturn_Code = JAMQ_APIU_RUNTIME_ERROR;
        return NOT_OK;
    }

    *aireturn_Code = 0;
    return OK;
}

int JAMQ_apiu_handle_message (
    JAMQ_tsApicb  *pApiHndl,
    int           *aireturn_Code
    )
{
    gtw_context_t
        *self;
    JAMQ_fMessageHandlerRoutine
        fx;

    if (!aireturn_Code)
        return NOT_OK;

    if (!pApiHndl) {
        *aireturn_Code = JAMQ_APIU_HANDLE_INVALID;
        return NOT_OK;
    }

    self = (gtw_context_t*) pApiHndl;
  
    if (!JAMQ_hash_find (self->services, &(self->running_service),
          (void**) &fx, aireturn_Code)) {
        *aireturn_Code = JAMQ_APIU_RUNTIME_ERROR;
        return NOT_OK;
    }

    /*  Run the handler                                                      */
    if (!fx (pApiHndl, self->appctx.pApplHndl->pApplsHndl, NULL,
          aireturn_Code)) {
        /*  Handler failed... What now ?                                     */
        /*  TODO: implement is somehow                                       */
        *aireturn_Code = 0;
    }

    *aireturn_Code = 0;
    return OK;
}

int JAMQ_apiu_add_unref_syms_to_hash (
    JAMQ_tsApicb         *pApiHndl,
    JAMQ_tsUnrefSymInfo  *pUnrefSymArray,
    int                  iUnrefSymArrayLen,
    int                  *aireturn_Code
    )
{
    int
        counter;
    JAMQ_tsNCharcb
        key;

    if (!aireturn_Code)
        return NOT_OK;

    if (!pApiHndl || !pUnrefSymArray) {
        *aireturn_Code = JAMQ_APIU_HANDLE_INVALID;
        return NOT_OK;
    }

    if (iUnrefSymArrayLen < 0) {
        *aireturn_Code = JAMQ_APIU_INPUT_ERR;
        return NOT_OK;
    }

    for (counter = 0; counter != iUnrefSymArrayLen; counter++) {
        if (!pUnrefSymArray [counter].pUnreferencedSymbol) {
            *aireturn_Code = JAMQ_APIU_INPUT_ERR;
            return NOT_OK;
        }
        key.pData = pUnrefSymArray [counter].pUnreferencedSymbol;
        key.iDataLen = strlen (pUnrefSymArray [counter].pUnreferencedSymbol);
        if (!JAMQ_hash_add (((gtw_context_t*) pApiHndl)->functions, &key,
              (void*) pUnrefSymArray [counter].pUnreferencedRoutine,
              aireturn_Code)) {
            *aireturn_Code = JAMQ_APIU_HASH_ERROR;
            return NOT_OK;
        }
    }

    *aireturn_Code = 0;
    return OK;
}

int JAMQ_apiu_find_unref_sym (
    JAMQ_tsApicb         *pApiHndl,
    JAMQ_tsUnrefSymInfo  *pUnrefSym,
    int                  *aireturn_Code
    )
{
    gtw_context_t
        *self;
    JAMQ_tsNCharcb
        key;

    if (!aireturn_Code)
        return NOT_OK;

    if (!pApiHndl) {
        *aireturn_Code = JAMQ_APIU_HANDLE_INVALID;
        return NOT_OK;
    }

    if (!pUnrefSym || !pUnrefSym->pUnreferencedSymbol) {
        *aireturn_Code = JAMQ_APIU_INPUT_ERR;
        return NOT_OK;
    }

    self = (gtw_context_t*) pApiHndl;

    key.iDataLen = strlen (pUnrefSym->pUnreferencedSymbol);
    key.pData = pUnrefSym->pUnreferencedSymbol;
    if (!JAMQ_hash_find (self->functions, &key,
          (void**) &(pUnrefSym->pUnreferencedRoutine) , aireturn_Code)) {
        if (*aireturn_Code == JAMQ_HASH_DATA_UNV)
            *aireturn_Code = JAMQ_APIU_DATA_UNV;
        else
            *aireturn_Code = JAMQ_APIU_HASH_ERROR;
        return NOT_OK;
    }

    *aireturn_Code = 0;
    return OK;
}

int JAMQ_apiu_delete_unref_syms_from_hash (
    JAMQ_tsApicb         *pApiHndl,
    JAMQ_tsUnrefSymInfo  *pUnrefSymArray,
    int                  iUnrefSymArrayLen,
    int                  *aireturn_Code
    )
{
    int
        counter;
    JAMQ_tsNCharcb
        key;
    void
        *fx;
    int
        retcode;

    if (!aireturn_Code)
        return NOT_OK;

    if (!pApiHndl) {
        *aireturn_Code = JAMQ_APIU_HANDLE_INVALID;
        return NOT_OK;
    }

    if (!pUnrefSymArray || iUnrefSymArrayLen < 0) {
        *aireturn_Code = JAMQ_APIU_INPUT_ERR;
        return NOT_OK;
    }

    for (counter = 0; counter != iUnrefSymArrayLen; counter++) {
        if (!pUnrefSymArray [counter].pUnreferencedSymbol) {
            *aireturn_Code = JAMQ_APIU_INPUT_ERR;
            return NOT_OK;
        }
        key.pData = pUnrefSymArray [counter].pUnreferencedSymbol;
        key.iDataLen = strlen (pUnrefSymArray [counter].pUnreferencedSymbol);
        if (!JAMQ_hash_delete (((gtw_context_t*) pApiHndl)->functions, &key,
              &key, &fx, aireturn_Code)) {
            if (retcode == JAMQ_HASH_DATA_UNV)
                *aireturn_Code = JAMQ_APIU_MINOR_ERROR;
            else
                *aireturn_Code = JAMQ_APIU_HASH_ERROR;

            return NOT_OK;
        }
    }

    *aireturn_Code = 0;
    return OK;
}

int JAMQ_apiu_register_service (
    JAMQ_tsApicb  *pApiHndl,
    char          *pServiceName,
    char          *pUnreferencedSymbol,
    int           *aireturn_Code
    )
{
    gtw_context_t
        *self;
    JAMQ_tsNCharcb
        service_name;
    JAMQ_tsNCharcb
        unreferenced_symbol;
    void
        *fx;

    if (!aireturn_Code)
        return NOT_OK;

    if (!pApiHndl) {
        *aireturn_Code = JAMQ_APIU_HANDLE_INVALID;
        return NOT_OK;
    }

    if (!pServiceName || !pUnreferencedSymbol) {
        *aireturn_Code = JAMQ_APIU_INPUT_ERR;
        return NOT_OK;
    }

    self = (gtw_context_t*) pApiHndl;
    service_name.iDataLen = strlen (pServiceName);
    service_name.pData = pServiceName;
    unreferenced_symbol.iDataLen = strlen (pUnreferencedSymbol);
    unreferenced_symbol.pData = pUnreferencedSymbol;

    if (JAMQ_hash_find (self->functions, &unreferenced_symbol, &fx,
          aireturn_Code) != OK) {
        *aireturn_Code = JAMQ_APIU_INPUT_ERR;
        return NOT_OK;
    }
    if (!JAMQ_hash_add (self->services, &service_name, fx, aireturn_Code)) {
        *aireturn_Code = JAMQ_APIU_HASH_ERROR;
        return NOT_OK;
    }

    *aireturn_Code = 0;
    return OK;
}

int JAMQ_apiu_add_and_start_timer (
    JAMQ_tsApicb    *pApiHndl,
    JAMQ_tsNCharcb  *pName,
    JAMQ_tsNCharcb  *pRelatedDevice,
    double          dExecFreq,
    JAMQ_tsNCharcb  *pSrvcRoutineName,
    JAMQ_tsNCharcb  *pUnSrvcRoutineName,
    int             *aiCode
    );

int JAMQ_apiu_stop_and_remove_timer (
    JAMQ_tsApicb    *pApiHndl,
    JAMQ_tsNCharcb  *pName,
    int             *aiCode
    );

int JAMQ_apiu_change_timer_freq (
    JAMQ_tsApicb   *pApiHndl,
    JAMQ_tsNCharcb *pName,
    JAMQ_tsNCharcb *pTmExecFreq,
    int            *aiCode);

int JAMQ_apiu_flush_broadcast (
    JAMQ_tsApicb  *pApiHndl,
    JAMQ_tsBufcb  *pBuf,
    int           *aireturn_Code)
{
    gtw_context_t
        *self;
    JAMQ_tsNCharcb
        data;
    char
        queue_name_buffer [255];
    char
        *queue_name;
    dbyte
        size;
    apr_status_t
        result;
    void
        *gmm = NULL;

    if (!aireturn_Code)
        return NOT_OK;

    if (!pApiHndl) {
        *aireturn_Code = JAMQ_APIU_HANDLE_INVALID;
        return NOT_OK;
    }

    if (!pBuf) {
        *aireturn_Code = JAMQ_APIU_INPUT_ERR;
        return NOT_OK;
    }

    self = (gtw_context_t*) pApiHndl;

    /*  Get queue name                                                       */
    if (!JAMQ_gmm_open (&gmm, aireturn_Code)) {
        *aireturn_Code = JAMQ_APIU_GMM_ERROR;
        return NOT_OK;
    }
    if (!JAMQ_gmm_set_buffers (gmm, pBuf, NULL, aireturn_Code)) {
        JAMQ_gmm_close (&gmm, aireturn_Code);
        *aireturn_Code = JAMQ_APIU_GMM_ERROR;
        return NOT_OK;
    }
    if (!JAMQ_gmm_get_data (gmm, 0, 0, &data, aireturn_Code)) {
        if (*aireturn_Code == JAMQ_GMM_DATA_UNV) {
            queue_name = "pubsub";
            *aireturn_Code = 0;
        }
        else {
            JAMQ_gmm_close (&gmm, aireturn_Code);
            *aireturn_Code = JAMQ_APIU_GMM_ERROR;
            return NOT_OK;
        }
    }
    else {
        queue_name = queue_name_buffer;
        memcpy ((void*) queue_name_buffer, (void*) data.pData, data.iDataLen);
        queue_name_buffer [data.iDataLen] = 0;
    }
    if (!JAMQ_gmm_close (&gmm, aireturn_Code)) {
        *aireturn_Code = JAMQ_APIU_GMM_ERROR;
        return NOT_OK;
    }

    /*  Built-in services                                                    */
    if (strcmp (queue_name, "subscribe_all") == 0) {
        printf ("SUBSCRIBE_ALL\n");
        *aireturn_Code = 0;
        return OK;
    }
    if (strcmp (queue_name, "subscribe_field") == 0) {
        printf ("SUBSCRIBE_FIELD\n");
        *aireturn_Code = 0;
        return OK;
    }
    if (strcmp (queue_name, "subscribe_data") == 0) {
        printf ("SUBSCRIBE_DATA\n");
        *aireturn_Code = 0;
        return OK;
    }
    if (strcmp (queue_name, "subscribe_list_data") == 0) {
        printf ("SUBSCRIBE_LIST_DATA\n");
        *aireturn_Code = 0;
        return OK;
    }
    if (strcmp (queue_name, "unsubscribe_data") == 0) {
        printf ("UNSUBSCRIBE_DATA\n");
        *aireturn_Code = 0;
        return OK;
    }
    if (strcmp (queue_name, "register") == 0) {
        if (!JAMQ_gmm_get_data (gmm, 0, 1, &data, aireturn_Code)) {
            *aireturn_Code = JAMQ_APIU_GMM_ERROR;
            return NOT_OK;
        }

        /* TODO: Parse 3th argument : load balance vs. pub-sub               */

        return JAMQ_apiu_consume (pApiHndl, &data, 1, aireturn_Code);
    }

    /*  Send message                                                         */
    GET_SHORT (size, pBuf->pData);
    result = amq_stdc_send_message (self->channel, self->handle_id, 0,
        0, queue_name, 0, 0, 5, 0, "", "", "", size,
        pBuf->pData + sizeof (dbyte), 1, "X", 1);
    if (result != APR_SUCCESS) {
        *aireturn_Code = JAMQ_APIU_AMQ_ERROR;
        return NOT_OK;
    }

    *aireturn_Code = 0;
    return OK;
}

int JAMQ_apiu_submit_broadcast (
    JAMQ_tsApicb  *pApiHndl,
    JAMQ_tsBufcb  *pBuf,
    int           iMustWriteIt,
    int           *aireturn_Code
    )
{
    /*  iMustWriteIt unused as it makes no difference in behaviour           */
    return JAMQ_apiu_flush_broadcast (pApiHndl, pBuf, aireturn_Code);
}

int JAMQ_apiu_build_text_header_eng (
    JAMQ_tsApicb    *pApiHndl,
    JAMQ_tsNCharcb  *pRequest,
    int             *aireturn_Code
    )
{
    apr_time_t
        tm;
    apr_time_exp_t
        exptm;
    apr_status_t
        result;
    int
        res;
    char
        *buf;
    qbyte
        pos;
    qbyte
        width;
    qbyte
        counter;
    qbyte
        separator_width;
    JAMQ_tsNCharcb
        header;

    if (!aireturn_Code)
        return NOT_OK;

    if (!pApiHndl) {
        *aireturn_Code = JAMQ_APIU_HANDLE_INVALID;
        return NOT_OK;
    }

    if (!pRequest || !pRequest->pData || pRequest->iDataLen < 0) {
        *aireturn_Code = JAMQ_APIU_INPUT_ERR;
        return NOT_OK;
    }

    /*  Get local time                                                       */
    tm = apr_time_now ();
    result = apr_time_exp_lt (&exptm, tm);
    if (result != APR_SUCCESS) {
        *aireturn_Code = JAMQ_APIU_APR_ERROR;
        return NOT_OK;
    }

    /*  Compute header width                                                 */
    width = pApiHndl->sName.iDataLen + 2;
    if (width < 20)
        width = 20;
    if (width < pRequest->iDataLen + 2)
        width = pRequest->iDataLen + 2;

    buf = malloc ((width + 5) * 6);
    if (!buf) {
        *aireturn_Code = JAMQ_APIU_MEM_ERR;
        return NOT_OK;
    }

    pos = 0;
    memcpy (buf + pos, "  ", 2);
    pos += 2;
    for (counter = 0; counter != width; counter ++)
        buf [pos++] = '-';
    memcpy (buf + pos, "  \x00a /", 6);
    pos += 6;
    separator_width = (width - pApiHndl->sName.iDataLen) / 2;
    for (counter = 0; counter != separator_width; counter ++)
        buf [pos++] = ' ';
    memcpy (buf + pos, pApiHndl->sName.pData,pApiHndl->sName.iDataLen);
    pos += pApiHndl->sName.iDataLen;
    separator_width = (width - pApiHndl->sName.iDataLen) - 
        ((width - pApiHndl->sName.iDataLen) / 2);
    for (counter = 0; counter != separator_width; counter ++)
        buf [pos++] = ' ';
    memcpy (buf + pos, "\\ \x00a/ ", 6);
    pos += 6;
    sprintf (buf + pos, "%02ld:%02ld:%02ld", (long) exptm.tm_hour,
        (long) exptm.tm_min, (long) exptm.tm_sec);
    pos += 8;
    separator_width = width - 8 - 10;
    for (counter = 0; counter != separator_width; counter ++)
        buf [pos++] = ' ';
    sprintf (buf + pos, "%04ld-%02ld-%02ld", (long) exptm.tm_year + 1900,
        (long) exptm.tm_mon + 1, (long) exptm.tm_mday);
    pos += 10;
    memcpy (buf + pos, " \\\x00a\\ ", 6);
    pos += 6;
    for (counter = 0; counter != width; counter ++)
        buf [pos++] = ' ';
    memcpy (buf + pos, " /\x00a \\", 6);
    pos += 6;
    separator_width = (width - pRequest->iDataLen) / 2;
    for (counter = 0; counter != separator_width; counter ++)
        buf [pos++] = ' ';
    memcpy (buf + pos, pRequest->pData,pRequest->iDataLen);
    pos += pRequest->iDataLen;
    separator_width = (width - pRequest->iDataLen) - 
        ((width - pRequest->iDataLen) / 2);
    for (counter = 0; counter != separator_width; counter ++)
        buf [pos++] = ' ';
    memcpy (buf + pos, "/ \x00a   ", 7);
    pos += 7;
    for (counter = 0; counter < width -2; counter ++)
        buf [pos++] = '-';
    memcpy (buf + pos, "  ", 3);
    pos += 3;
    
    header.iDataLen = pos;
    header.pData = buf;
    res = JAMQ_gmm_add_data (pApiHndl->pGMMWriteHndl, JAMQ_GMM_TEXT, 1,
        &header, aireturn_Code);
    free (buf);
    return res;
}

int JAMQ_apiu_finish_gmm_response_msg (
    JAMQ_tsApicb  *pApiHndl,
    int           *aireturn_Code
    )
{
    gtw_context_t
        *self;
    JAMQ_tsNCharcb
        english;

    if (!aireturn_Code)
        return NOT_OK;

    if (!pApiHndl) {
        *aireturn_Code = JAMQ_APIU_HANDLE_INVALID;
        return NOT_OK;
    }

    self = (gtw_context_t*) pApiHndl;

    if (!JAMQ_gmm_add_data (self->appctx.pGMMWriteHndl, JAMQ_GMM_RESPONSE, 1,
        &(self->running_service), aireturn_Code)) {
        *aireturn_Code= JAMQ_APIU_GMM_ERROR;
        return NOT_OK;
    }
    if (!JAMQ_gmm_add_data (self->appctx.pGMMWriteHndl, JAMQ_GMM_SYSTEM_MSG, 1,
        &(self->reply_to), aireturn_Code)) {
        *aireturn_Code= JAMQ_APIU_GMM_ERROR;
        return NOT_OK;
    }
    english.pData = "English";
    english.iDataLen = 7;
    if (!JAMQ_gmm_add_data (self->appctx.pGMMWriteHndl, JAMQ_GMM_RESPONSE_LANG,
        1, &english, aireturn_Code)) {
        *aireturn_Code= JAMQ_APIU_GMM_ERROR;
        return NOT_OK;
    }

    *aireturn_Code = 0;
    return NOT_OK;
}
