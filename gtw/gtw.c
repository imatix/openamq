
#include "gtw.h"

#include "base.h"
#include "gtw_ll.h"

int JAMQ_ll_open (
    void             **ap_link_List,
    JAMQ_tsLlParams  *ll_pParams,
    int              *aireturn_Code
    )
{
    gtw_ll_t
        *list;

    list = gtw_ll_new (ll_pParams, aireturn_Code);

    *ap_link_List = (void*) list;

    return OK;    
}

int JAMQ_ll_close (
    void  **ap_link_List,
    int   *aireturn_Code
    )
{
    gtw_ll_t
        **list = (gtw_ll_t**) ap_link_List;

    gtw_ll_destroy (list);

    return OK;
}

int JAMQ_ll_delete_item(
    void            *p_link_List,
    JAMQ_tsNCharcb  *p_ll_Key,
    void            **ap_ll_Item,
    int             *aireturn_Code
    )
{
    gtw_ll_t
        *list = (gtw_ll_t*) p_link_List;

    return gtw_ll_delete (list, p_ll_Key, ap_ll_Item, aireturn_Code);
}

int JAMQ_ll_insert_item (
    void            *p_link_List,
    JAMQ_tsNCharcb  *p_ll_Key,
    void            *p_ll_ItemIn,
    int             *aireturn_Code
    )
{
    gtw_ll_t
        *list = (gtw_ll_t*) p_link_List;

    return gtw_ll_insert (list, p_ll_Key, p_ll_ItemIn, aireturn_Code);
}

int JAMQ_ll_item_count (
    void * p_link_List
    )
{
    gtw_ll_t
        *list = (gtw_ll_t*) p_link_List;
    int
       count;

    if (gtw_ll_count (list, &count) == OK)
        return count;
    else
        return -1;
}

int JAMQ_ll_keyed_first_item (
    void            *p_link_List,
    JAMQ_tsNCharcb  *p_ll_KeyIn,
    void            **ap_ll_Item,
    int             *aireturn_Code
    )
{
    gtw_ll_t
        *list = (gtw_ll_t*) p_link_List;

    return gtw_ll_first_by_key (list, p_ll_KeyIn, ap_ll_Item, aireturn_Code);
}

int JAMQ_ll_seq_first_item (
    void           *p_link_List,
    JAMQ_tsNCharcb *p_ll_Key,
    void           **ap_ll_Item,
    int            *aireturn_Code
    )
{
    gtw_ll_t
        *list = (gtw_ll_t*) p_link_List;

    return gtw_ll_first_item (list, p_ll_Key, ap_ll_Item, aireturn_Code);
}

int JAMQ_ll_seq_next_item (
    void            *p_link_List,
    JAMQ_tsNCharcb  *p_ll_Key,
    void            **ap_ll_Item,
    int             *aireturn_Code
    )
{
    gtw_ll_t
        *list = (gtw_ll_t*) p_link_List;

    return gtw_ll_next_item (list, p_ll_Key, ap_ll_Item, aireturn_Code);
}

int JAMQ_ll_set_key (
    void            *p_link_List,
    JAMQ_tsNCharcb  *p_ll_KeyOut,
    JAMQ_tsNCharcb  *p_ll_KeyIn,
    int             *aireturn_Code
    )
{
    gtw_ll_t
        *list = (gtw_ll_t*) p_link_List;

    return gtw_ll_set_key (list, p_ll_KeyOut, p_ll_KeyIn, aireturn_Code);
}
