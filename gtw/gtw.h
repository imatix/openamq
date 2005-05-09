
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

#define JAMQ_LL_HANDLE_INVALID   001
#define JAMQ_LL_HANDLE_ACTIVE    002
#define JAMQ_LL_MEM_ERR          003
#define JAMQ_LL_INPUT_ERR        004
#define JAMQ_LL_DATA_UNV         005

#define JAMQ_LL_TYPE_1 1
#define JAMQ_LL_TYPE_2 2
#define JAMQ_LL_TYPE_3 3

typedef struct JAMQ_sLlParams      
{
    int
        iType;
    void
        *p_LinkedList_Hndl;
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

#define JAMQ_HASH_HANDLE_INVALID   100
#define JAMQ_HASH_HANDLE_ACTIVE    101
#define JAMQ_HASH_INPUT_ERR        102
#define JAMQ_HASH_MEM_ERR          103
#define JAMQ_HASH_ALREADY_EXISTS   104
#define JAMQ_HASH_DATA_UNV         105

#define LTW_HASH_TOREK_ROUTINE_NAME "hashr_torek"
#define LTW_HASH_TOREK_ROUTINE_NAME_LEN 11

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

/*---------------------------------------------------------------------------
 *  Table
 *---------------------------------------------------------------------------*/

#define JAMQ_TBL_HANDLE_INVALID   200
#define JAMQ_TBL_HANDLE_ACTIVE    201
#define JAMQ_TBL_INPUT_ERR        202
#define JAMQ_TBL_MEM_ERR          203
#define JAMQ_TBL_LOC_MEM_ERR      204
#define JAMQ_TBL_TOKEN_ERR        205
#define JAMQ_TBL_DATA_UNV         206

#define JAMQ_DOS_FILE  1
#define JAMQ_UNIX_FILE 2

typedef struct JAMQ_sTblParams     
{
    int
        iRowIncrement;
} JAMQ_tsTblParams;

int JAMQ_tbl_open (
    void              **apTableHandle,
    JAMQ_tsTblParams  *pTableParams,  
    int               *aireturn_Code
    );

int JAMQ_tbl_load_ssv (
    void            *pTableHandle,
    JAMQ_tsNCharcb  *pJAMQ_FileName,
    int             iJAMQ_FileType, 
    int             *aireturn_Code
    );

int JAMQ_tbl_load_tbl (
    void            *pTableHandle,
    JAMQ_tsNCharcb  *pJAMQ_FileName,
    int             *aireturn_Code
    );

int JAMQ_tbl_find_first_match (
    void            *pTableHandle,
    int             iTableCol,
    JAMQ_tsNCharcb  *pTableItem,
    int             *aiTableRow,
    int             *aireturn_Code
    );

int JAMQ_tbl_get_item (
    void            *pTableHandle,
    int             iTableRow,
    int             iTableCol,
    JAMQ_tsNCharcb  *pTableItem,
    int             *aireturn_Code
    );

int JAMQ_tbl_update_item (
    void            *pTableHandle,
    int             iTableRow,
    int             iTableCol,
    JAMQ_tsNCharcb  *pTableItem,
    int             *aireturn_Code
    );

int JAMQ_tbl_get_size (
    void  *pTableHandle,
    int   *aiTableRows,
    int   *aiTableCols,
    int   *aireturn_Code
    );

int JAMQ_tbl_write_tbl (
    void            *pTableHandle,
    JAMQ_tsNCharcb  *pJAMQFileName, 
    int             *aireturn_Code
    );

int JAMQ_tbl_delete_item (
    void  *pTableHandle,
    int   iTableRow,
    int   iTableCol,
    int   *aireturn_Code
    );

int JAMQ_tbl_close (
    void  **apTableHandle,
    int   *aireturn_Code
    );

/*---------------------------------------------------------------------------
 *  Miscellaneous
 *---------------------------------------------------------------------------*/

#define JAMQ_MISC_INPUT_ERR 300

int JAMQ_m_chars_to_double (
    double          *pDouble, 
    JAMQ_tsNCharcb  *pNumber,
    int             *aireturn_Code
    );

int JAMQ_m_chars_to_int (
    int             *aiInt, 
    JAMQ_tsNCharcb  *pChars,
    int             *aireturn_Code
    );
 /*
int JAMQ_m_get_buffer (
    JAMQ_tsBufcb  **apBufferHandle,
    int           iPhysicalLen, 
    int           *aireturn_Code
    );

int JAMQ_m_put_buffer (
    JAMQ_tsBufcb  **apBufferHandle,
    int           *aireturn_Code
    );

int JAMQ_m_mem_nchar_dup (
    JAMQ_tsNCharcb  *pNCharOut,
    JAMQ_tsNCharcb  *pNCharIn,
    int             *aireturn_Code
    );

int JAMQ_m_mem_nchar_undup (
    JAMQ_tsNCharcb  *pBuffer,
    int             *iRetCode
    );
*/

/*---------------------------------------------------------------------------
 *  Ordered unique map
 *---------------------------------------------------------------------------*/

#define JAMQ_OUM_HANDLE_INVALID  400
#define JAMQ_OUM_HANDLE_ACTIVE   401
#define JAMQ_OUM_INPUT_ERR       402
#define JAMQ_OUM_MEM_ERR         403
#define JAMQ_OUM_HASH_ERR        404
#define JAMQ_OUM_DATA_UNV        405
#define JAMQ_OUM_LOC_MEM_ERR     406

typedef struct JAMQ_soumParams
{
    JAMQ_tsHashParams
        sHashParams;
    int
        iKeyStartCount;
    int
        iKeyIncrement;
    int
        iNCharCount;
}JAMQ_tsoumParams;

int JAMQ_oum_open (
    void              **apoumHandle,
    JAMQ_tsoumParams  *poumParams,
    int               *aireturn_Code
    );

int JAMQ_oum_get_nchars (
    void            *poumHandle,
    int             iKey,
    JAMQ_tsNCharcb  *pString,
    int             *aireturn_Code
    );

int JAMQ_oum_get_int_key (
    void            *poumHandle,
    JAMQ_tsNCharcb  *pString,
    int             *pKey,
    int             *aireturn_Code);

int JAMQ_oum_put_int_key (
    void  *poumHandle,
    int   iKey,
    int   *aireturn_Code
    );

int JAMQ_oum_close (
    void  **apoumHandle,
    int   *aireturn_Code
    );

#endif
