/*
 *
 */

#ifndef __MXR_MESSAGE_H__
#define __MXR_MESSAGE_H__

#include "sfl.h"
#include "mxr_messagelist.h"

/*
 * Adds message msg to the list.  If list is null, it allocates it
 * first.
 */
int mxr_message_add (mxr_messagelist_t **list, char *msg);

#endif
