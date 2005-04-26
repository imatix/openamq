
#include "gtw.h"

#include "base.h"
#include "gtw_ll.h"
#include "gtw_hash.h"
#include "gtw_tbl.h"

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

