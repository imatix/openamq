
#include "mxr_message.h"

int mxr_message_add (mxr_messagelist_t **list, char *msg)
{
    if (!msg || !list)
        return -1;

    /* Allocate the list if necessary */
    if (*list == NULL) {
        if ( ((*list) = mxr_messagelist_new (msg)) )
            return 0;
        return -1;
    }

    /* Find the last element */
    while ((*list)->next != NULL)
        list = &((*list)->next);

    if ( ((*list)->next = mxr_messagelist_new (msg)) )
        return 0;
    return -1;
}
