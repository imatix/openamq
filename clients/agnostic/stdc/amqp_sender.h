
#ifndef __AMQP_SENDER_H__
#define __AMQP_SENDER_H__

#include "amqp_common.h"

void *sender_thread(apr_thread_t *thread, void *data);

#endif
