/*
 *
 */

#ifndef __MXR_REQUEST_H__
#define __MXR_REQUEST_H__

#include "sfl.h"
#include "mxr_requestlist.h"

/*
 * Adds req to the list.  If list is NULL, it allocates it.
 */
int mxr_request_add (mxr_requestlist_t **list, char *req);

#endif
