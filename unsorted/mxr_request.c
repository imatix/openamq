
#include "mxr_request.h"

int mxr_request_add (mxr_requestlist_t **list, char *req)
{
    if (!req || !list)
        return -1;

    /* Allocate the list if necessary */
    if (*list == NULL) {
        if ( (*list = mxr_requestlist_new (req)) )
            return 0;
        return -1;
    }

    /* find the last item on the list */
    while ((*list)->next != NULL)
        list = &((*list)->next);

    if ( ((*list)->next = mxr_requestlist_new (req)) )
        return 0;
    return -1;
}
