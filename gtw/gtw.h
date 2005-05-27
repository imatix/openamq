
#ifndef GTW_H_INCLUDED
#define GTW_H_INCLUDED

#define tsApicb JAMQ_tsApicb
#define tsApiDvcb JAMQ_tsApiDvcb
#define tsBufcb JAMQ_tsBufcb
#define apiu_add_unref_syms_to_hash JAMQ_apiu_add_unref_syms_to_hash
#define apiu_delete_unref_syms_from_hash JAMQ_apiu_delete_unref_syms_from_hash
#define tsUnrefSymInfo JAMQ_tsUnrefSymInfo

#define API_OK 0
#define mArraySize(array) (sizeof (array) / sizeof (*array))

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
        *pMemHndl;
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

#define JAMQ_MISC_HANDLE_INVALID  300
#define JAMQ_MISC_HANDLE_ACTIVE   301
#define JAMQ_MISC_INPUT_ERR       302
#define JAMQ_MISC_MEM_ERR         303

typedef struct JAMQ_sBufcb  
{
    int
        iCurrentLen;
    int
        iLogicalMaxLen;
    int
        iPhysicalLen;
    char
        *pData;
}JAMQ_tsBufcb;

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

int JAMQ_m_get_buffer (
    JAMQ_tsBufcb  **apBufferHandle,
    int           ibufferLength, 
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
    int             *aireturn_Code
    );

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

/*---------------------------------------------------------------------------
 *  GTW message module
 *---------------------------------------------------------------------------*/

#define JAMQ_GMM_HANDLE_INVALID  500
#define JAMQ_GMM_HANDLE_ACTIVE   501
#define JAMQ_GMM_INPUT_ERR       502
#define JAMQ_GMM_MEM_ERR         503
#define JAMQ_GMM_DATA_UNV        504

int JAMQ_gmm_open (
    void  **pGmmHandle, 
    int   *aireturn_Code
    );

int JAMQ_gmm_close (
    void  **pGmmHandle, 
    int   *aireturn_Code
    );

int JAMQ_gmm_start_msg (
    void          *pGmmHandle,
    JAMQ_tsBufcb  *pMsgBuffer,
    int           *aireturn_Code
    );

int JAMQ_gmm_add_data (
    void            *pGmmHandle, 
    int             iFieldId, 
    int             iDataItemCount, 
    JAMQ_tsNCharcb  *pDataHandle, 
    int             *aireturn_Code
    );

int JAMQ_gmm_delete_field (
    void  *pGmmHandle, 
    int   iFieldId, 
    int   *aireturn_Code
    );

int JAMQ_gmm_get_field_count (
    void  *pGmmHndlIn, 
    int   *iFieldCount,
    int   *aireturn_Code
    );

int JAMQ_gmm_get_field (
    void  *pGmmHandle, 
    int   iFieldId, 
    int   *iDataItemCount, 
    int   *aireturn_Code
    );

int JAMQ_gmm_get_next_field (
    void  *pGmmHandle, 
    int   *iFieldId, 
    int   *iDataItemCount, 
    int   *aireturn_Code
    );

int JAMQ_gmm_get_first_field (
    void  *pGmmHandle, 
    int   *iFieldId, 
    int   *iDataItemCount, 
    int   *aireturn_Code
    );

int JAMQ_gmm_get_data (
    void            *pGmmHandle, 
    int             iFieldId, 
    int             iDataItem, 
    JAMQ_tsNCharcb  *pData, 
    int             *aireturn_Code
    );

int JAMQ_gmm_copy_field_as (
    void  *pGmmHandleTo, 
    void  *pGmmHandleFrom, 
    int   iFieldIdTo, 
    int   iFieldIdFrom, 
    int   *aireturn_Code
    );

int JAMQ_gmm_copy_msg (
    void  *pGmmHandleTo, 
    void  *pGmmHandleFrom, 
    int   *aireturn_Code
    );

int JAMQ_gmm_from_cmdline (
    void            *pGmmHandle,
    int             iCharCount,
    JAMQ_tsNCharcb  *pCharHandle,
    JAMQ_tsBufcb    *pBufferHandle,
    int             *aireturn_Code
    );

int JAMQ_gmm_to_cmdline (
    void          *pGmmHandle, 
    JAMQ_tsBufcb  *pBufferHandle,
    int           *aireturn_Code
    );

int JAMQ_gmm_parse_first_msg (
    void          *pGmmHandle, 
    JAMQ_tsBufcb  *pBufferToBuild, 
    JAMQ_tsBufcb  *pBufferToParse, 
    int           *aireturn_Code
    );

int JAMQ_gmm_parse_next_msg (
    void          *pGmmHandle, 
    JAMQ_tsBufcb  *pBufferToBuild, 
    int           *aireturn_Code
    );

/*---------------------------------------------------------------------------
 *  Platform interface module
 *---------------------------------------------------------------------------*/

#define JAMQ_PIM_HANDLE_INVALID  600
#define JAMQ_PIM_HANDLE_ACTIVE   601
#define JAMQ_PIM_BAD_INPUT       602
#define JAMQ_PIM_MISC_ERR        603
#define JAMQ_PIM_MEM_ERR         604
#define JAMQ_PIM_SYSTEM_ERROR    605

#define JAMQ_PIM_IO_INPUT_TYPE                 0
#define JAMQ_PIM_IO_TEXT_ORG                   0
#define JAMQ_PIM_TCP                           0
#define JAMQ_PIM_CLIENT                        0
#define JAMQ_PIM_BLOCKING_MODE                 0
#define JAMQ_PIM_AUTHENTICATE_BY_NAME_OR_ADDR  0

typedef struct sFdParams
{
    JAMQ_tsNCharcb
        sFileName;
    int
        iIoType;
    int
        iFileOrg;
    int
        iRecLen;
} JAMQ_tsFdParams;

typedef struct JAMQ_sSdParams
{
    JAMQ_tsNCharcb
        sSocketName;
    JAMQ_tsNCharcb
        sAddress;                       
    int
        iProtocol;                      
    int
        iIoType;
    int
        iMode;
    int
        iAuthRule;
    char
        cBreakChar;
    int
        iOesId;                   
    void
        *pCnnctrDvHndl;              
} JAMQ_tsSdParams;

int JAMQ_pim_init (
    char  **envp, 
    int   *aireturn_Code
    );

int JAMQ_pim_uninit (
    int  *aireturn_Code
    );

int JAMQ_pim_time_open (
    void  ** apTimeHandle, 
    int   *aireturn_Code
    );

int JAMQ_pim_time_record (
    void  *pTimeHandle, 
    int   *aireturn_Code
    );

int JAMQ_pim_time_delta (
    double  *adSecs,
    void    *pMinuendTmHndlIn,
    void    *pSubtrahendTmHndlIn,
    int     *aireturn_Code
    );

int JAMQ_pim_time_close (
    void  **pTimeHandle,
    int   *aireturn_Code
    );

int JAMQ_pim_sleep (
    double  dSeconds,
    int     *aireturn_Code
    );

int JAMQ_pim_mem_get (
    void  *pMemHandleIn,
    void  **pPtr,
    int   *aiLenGiven,
    int   iLenNeeded,
    int   *aireturn_Code
    );

int JAMQ_pim_mem_put (
    void  *pMemHandle,
    void  **pPtr,
    int   *aireturn_Code
    );

int JAMQ_pim_fd_open (
    void             **pDeviceHandle,
    JAMQ_tsFdParams  *pParams,
    int              *aireturn_Code
    );

int JAMQ_pim_fd_read (
    void          *pDvHndlIn,
    void          *pTmHndl,
    JAMQ_tsBufcb  *pBuffer,
    int           *aireturn_Code
    );

int JAMQ_pim_fd_close (
    void  **apDvHndl,
    int   *aireturn_Code
    );

int JAMQ_pim_sd_open (
    void             **apDvHndl,
    JAMQ_tsSdParams  *pParams,
    int              *aireturn_Code
    );

int JAMQ_pim_sd_read (
    void          *pDvHndlIn,
    void          *pTmHndl,
    JAMQ_tsBufcb  *pBuffer,
    int           *aireturn_Code
    );

int JAMQ_pim_sd_write (
    void          *pDvHndlIn,
    void          *pTmHndl,
    JAMQ_tsBufcb  *pBuffer,
    int           *aireturn_Code
    );

int JAMQ_pim_sd_close (
    void  **apDvHndl,
    int   *aireturn_Code
    );

int JAMQ_pim_log(
    void  *pLogHndlIn,
    int   iPriority,
    int   *aiCode,
    char  *pExpandedMsg,
    char  *pFmtIn,
    ...
    );

/*---------------------------------------------------------------------------
 *  APIU
 *---------------------------------------------------------------------------*/

#define JAMQ_APIU_HANDLE_INVALID  700
#define JAMQ_APIU_HASH_ERROR      701
#define JAMQ_APIU_INPUT_ERR       702
#define JAMQ_APIU_MINOR_ERROR     703
#define JAMQ_APIU_LL_ERROR        704
#define JAMQ_APIU_DATA_UNV        705
#define JAMQ_APIU_GMM_ERROR       706

typedef struct sStatecb   
{
    int
        iOkToRun;
    int
        iPermissionLevel;
} JAMQ_tsStatecb;

typedef struct sApplcb                 
{
    JAMQ_tsNCharcb
        sName;
    void
        *pApplsHndl;
} JAMQ_tsApplcb;

typedef struct JAMQ_sEnccb            
{
    JAMQ_tsNCharcb
        sName;
    void
        *pHashHndl;                   
    int
        iSubscribers;
    int
        iUnknownPageMeter;
} JAMQ_tsEnccb;

typedef struct JAMQ_sApiDvcb
{
    int
        iDeviceType;
    JAMQ_tsNCharcb
        sName;
    JAMQ_tsNCharcb
        sRelatedDevice;
    JAMQ_tsEnccb
        pEncHndl;
    void
        *pApiDvsHndl;
} JAMQ_tsApiDvcb;

typedef struct JAMQ_sApicb
{
    int
        argc;
    char
        **argv;
    JAMQ_tsNCharcb
        sName;
    JAMQ_tsStatecb
        sStateHndl;
    JAMQ_tsApplcb
        *pApplHndl;
    void
        *pTmHndl;
    void
        *pGMMReadHndl;
    void
        *pGMMWriteHndl;
    void
        *pApiDvHndlList;
    JAMQ_tsBufcb
        sReadBuf;
    JAMQ_tsBufcb
        sBuildBuf;
    JAMQ_tsBufcb
        sWriteBuf;
    JAMQ_tsBufcb
        sOutputBuf;
    JAMQ_tsBufcb
        *sLogBuf;
    void
        *pLogHndl;
    JAMQ_tsApiDvcb
        *pDvHndl;
} JAMQ_tsApicb;

typedef struct tag_gtw_context_t
{
    JAMQ_tsApicb
        appctx;
    void
        *functions;
    void
        *services;
    JAMQ_tsApplcb
        appctx_internal;
} gtw_context_t;

typedef int (*JAMQ_fUnreferencedRoutine) ();

typedef struct JAMQ_sUnrefSymInfo
{
    char
        *pUnreferencedSymbol;
    JAMQ_fUnreferencedRoutine
        pUnreferencedRoutine;
} JAMQ_tsUnrefSymInfo;

int JAMQ_apiu_add_unref_syms_to_hash (
    JAMQ_tsApicb         *pApiHndl,
    JAMQ_tsUnrefSymInfo  *pUnrefSymArray,
    int                  iUnrefSymArrayLen,
    int                  *aireturn_Code
    );

int JAMQ_apiu_delete_unref_syms_from_hash (
    JAMQ_tsApicb         *pApiHndl,
    JAMQ_tsUnrefSymInfo  *pUnrefSymArray,
    int                  iUnrefSymArrayLen,
    int                  *aireturn_Code
    );

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
    int           *aiCode);

int JAMQ_apiu_submit_broadcast (
    JAMQ_tsApicb  *pApiHndl,
    JAMQ_tsBufcb  *pBuf,
    int           iMustWriteIt,
    int           *aiCode
    );

int JAMQ_apiu_build_text_header_eng (
    JAMQ_tsApicb    *pApiHndl,
    JAMQ_tsNCharcb  *pRequest,
    int             *aiCode
    );

int JAMQ_apiu_finish_gmm_response_msg (
    JAMQ_tsApicb  *pApiHndl,
    int           *aiCode
    );

#endif
