

#include "sfl.h"

#include "mxr_request.h"
#include "mxr_message.h"

/*
 * Message  -> Parser -> Criterias
 * Requests -> Parser -> Criterias
 */


static void s_get_messages (mxr_messagelist_t **message_list)
{
    /*
     * Populate message list
     */
    mxr_message_add (message_list, "bar");
    mxr_message_add (message_list, "foo");
    mxr_message_add (message_list, "baz");
}

static void s_get_requests (mxr_requestlist_t **request_list)
{
    /*
     * Populate request list
     */
    mxr_request_add (request_list, "a"); //0 hits
    mxr_request_add (request_list, "b"); //2 hits
    mxr_request_add (request_list, "f"); //1 hit
}

static void s_service_requests (mxr_messagelist_t *msg_list,
                                mxr_requestlist_t *req_list)
{
    /*
     * By now all the messages and requests have been parsed into
     * criterias.  We try to match each request to each message and
     * take care of every match.  This is the naive implementation --
     * we can optimize this by not having to match everything after
     * the first time.  Also, we can have several messages that
     * conform to the same criteria.  Once we have a match from one of
     * the requests, we can just start sending all messages conforming
     * to that criteria automatically to the requester, without having
     * to do this kind of n^2 matching.
     */
    mxr_requestlist_t *req_orig = req_list;

    while (msg_list)
    {
        req_list = req_orig;
        while (req_list)
        {
            //XXX fix this matching technique..
            if (msg_list->message[0] == req_list->clients->data[0])
                printf ("Matched: %s -> %s\n",
                        req_list->clients->data, msg_list->message);

            req_list = req_list->next;
        }

        msg_list = msg_list->next;
    }
}

int main (void)
{
    /*
     * Our master tables
     */
    mxr_requestlist_t *request_list = NULL;
    mxr_messagelist_t *message_list = NULL;

    s_get_messages (&message_list);
    s_get_requests (&request_list);
    s_service_requests (message_list, request_list);

    return 0;
}
