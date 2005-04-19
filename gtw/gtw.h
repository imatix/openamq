
#ifndef GTW_H_INCLUDED
#define GTW_H_INCLUDED

/*---------------------------------------------------------------------------
 *  Common GTW definitions
 *---------------------------------------------------------------------------*/

#define OK     1
#define NOT_OK 0

#define JAMQ_OS_NO_MEM_MGR NULL

typedef struct JAMQ_sNCharcb                                      
{
    char
        *pData;
    int
        iDataLen;
} JAMQ_tsNCharcb;

/*---------------------------------------------------------------------------
 *  Linked list
 *---------------------------------------------------------------------------*/

#define JAMQ_LL_HANDLE_INVALID 1
#define JAMQ_LL_HANDLE_DUPLICATE 2
#define JAMQ_LL_MEM_UNV 3
#define JAMQ_LL_INPUT_ERR 4
#define JAMQ_LL_DATA_UNV 5

#define JAMQ_LL_TYPE_1 1
#define JAMQ_LL_TYPE_2 2
#define JAMQ_LL_TYPE_3 3

typedef struct JAMQ_sLlParams      
{
    int
        iType;
    void
        * pLinkedList_Hndl;
} JAMQ_tsLlParams;

int JAMQ_ll_open (
    void             **ap_link_List,
    JAMQ_tsLlParams  *ll_pParams,
    int              *aireturn_Code
    );

int JAMQ_ll_close (
    void  **ap_link_List,
    int   *aireturn_Code
    );

int JAMQ_ll_delete_item(
    void            *p_link_List,
    JAMQ_tsNCharcb  *p_ll_Key,
    void            **ap_ll_Item,
    int             *aireturn_Code
    );

int JAMQ_ll_insert_item (
    void            *p_link_List,
    JAMQ_tsNCharcb  *p_ll_Key,
    void            *p_ll_ItemIn,
    int             *aireturn_Code
    );

int JAMQ_ll_item_count (
    void * p_link_List
    );

int JAMQ_ll_keyed_first_item (
    void            *p_link_List,
    JAMQ_tsNCharcb  *p_ll_KeyIn,
    void            **ap_ll_Item,
    int             *aireturn_Code
    );

int JAMQ_ll_seq_first_item (
    void           *p_link_List,
    JAMQ_tsNCharcb *p_ll_Key,
    void           **ap_ll_Item,
    int            *aireturn_Code
    );

int JAMQ_ll_seq_next_item (
    void            *p_link_List,
    JAMQ_tsNCharcb  *p_ll_Key,
    void            **ap_ll_Item,
    int             *aireturn_Code
    );

int JAMQ_ll_set_key (
    void            *p_link_List,
    JAMQ_tsNCharcb  *p_ll_KeyOut,
    JAMQ_tsNCharcb  *p_ll_KeyIn,
    int             *aireturn_Code
    );

/*---------------------------------------------------------------------------
 *  Hash table
 *---------------------------------------------------------------------------*/

#define JAMQ_HASH_HANDLE_INVALID 6
#define JAMQ_HASH_HANDLE_DUPLICATE 7
#define JAMQ_HASH_INPUT_ERR 8
#define JAMQ_HASH_MEM_ERR 9
#define JAMQ_HASH_ALREADY_EXISTS 10
#define JAMQ_HASH_DATA_UNV 11

#define LTW_HASH_TOREK_ROUTINE_NAME NULL

typedef struct JAMQ_sHashParams    
{
    int
        ihash_TableSize;
    JAMQ_tsNCharcb
        shash_RoutineName;
    void
        *p_hash_mem_Hndl;
} JAMQ_tsHashParams;

int JAMQ_hash_open (
    void               **aphash_Hndl,
    JAMQ_tsHashParams  *hash_init_Params,
    int                *aireturn_Code
    );

int JAMQ_hash_add (
    void            *phash_Hndl,
    JAMQ_tsNCharcb  *phash_Key,
    void            *phash_Item,
    int             *aireturn_Code
    );

int JAMQ_hash_delete (
    void            *phash_Hndl,
    JAMQ_tsNCharcb  *phash_KeyIn,
    JAMQ_tsNCharcb  *phash_KeyOut,
    void            **aphash_Item,
    int             *aireturn_Code
    );

int JAMQ_hash_find(
    void            *phash_Hndl,
    JAMQ_tsNCharcb  *phash_Key,
    void            **aphash_Item,
    int             *aireturn_Code
    );

int JAMQ_hash_first_item (
    void            *phash_Hndl,
    JAMQ_tsNCharcb  *phash_Key,
    void            **aphash_Item,
    int             *aireturn_Code
    );

int JAMQ_hash_next_item (
    void            *phash_Hndl,
    JAMQ_tsNCharcb  *phash_Key,
    void            **aphash_Item,
    int             *aireturn_Code
    );

int JAMQ_hash_close(
    void  **aphash_Hndl,
    int   *aireturn_Code
    );

#endif
